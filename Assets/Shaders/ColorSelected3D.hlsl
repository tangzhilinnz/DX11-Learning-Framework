#pragma pack_matrix( row_major )

cbuffer CamData : register(b0)
{
	float4x4 View;
	float4x4 Projection;
}

// Fog parameters
cbuffer FogData : register(b1)
{
    float4 EyePosWorld; // Eye position in world space
    float4 FogColor;    // Fog color
    float FogStart;     // Distance at which fog starts
    float FogRange;     // Range over which fog increases linearly
    float FogThickness; // Thickness of the fog layer (vertical extent)
    float FogBaseHeight; // Base height of the fog layer (bottom of the fog)
    float ExpFogDensity; // Density for exponential fog
    float Opacity;
    int   FogType; // 0 = Linear, 1 = Exponential, 2 = Layered
};

cbuffer InstanceData : register(b2)
{
    float4x4 World;
    float4 SelectedColor;
};

// Compute the intersection of a line segment with a horizontal plane at a given height
float3 ComputePlaneIntersection(float3 start, float3 end, float planeHeight)
{
    // Direction vector from start to end
    float3 dir = end - start;
    float t = (planeHeight - start.y) / dir.y;
    float3 intersection = start + t * dir;

    return intersection;
}

// Linear fog calculation
float CalcLinearFogFactor(float cameraToPixelDist)
{
    return saturate(1.0f - (cameraToPixelDist - FogStart) / FogRange);
}

// Exponential fog calculation (squared version)
float CalcExpFogFactor(float cameraToPixelDist)
{
    float gFogEnd = FogRange + FogStart;
    float distRatio = 4.0f * cameraToPixelDist / gFogEnd;
    float fogFactor = exp(-distRatio * ExpFogDensity * distRatio * ExpFogDensity);
    return fogFactor;
}

// Layered fog calculation
// https://ogldev.org/www/misc/fog_integral_proof.pdf
// https://www.youtube.com/watch?v=BYbIs1C7rkM
float CalcLayeredFogFactor(float3 cameraPos, float3 worldPos)
{
    if (FogThickness == 0.0f)
    {
        return 1.0f;
    }
    
    // Calculate the top of the fog layer
    float fogTopHeight = FogBaseHeight + FogThickness;
    
    // Determine if the camera and pixel are within the fog layer
    bool isCameraInFogLayer = cameraPos.y > FogBaseHeight && cameraPos.y < fogTopHeight;
    bool isPixelInFogLayer = worldPos.y > FogBaseHeight && worldPos.y < fogTopHeight;

    // Compute horizontal and vertical distances
    float horizontalDistance = 0.0f;
    float verticalDistance = 0.0f;

    if (isCameraInFogLayer && isPixelInFogLayer)
    {
        // Both camera and pixel are within the fog layer
        horizontalDistance = length(float2(cameraPos.x - worldPos.x, cameraPos.z - worldPos.z));
        verticalDistance = abs(cameraPos.y - worldPos.y);
    }
    else if (isCameraInFogLayer && !isPixelInFogLayer)
    {
        // Camera is in the fog layer, pixel is outside
        float3 intersectionTop = ComputePlaneIntersection(cameraPos, worldPos, fogTopHeight);
        float3 intersectionBottom = ComputePlaneIntersection(cameraPos, worldPos, FogBaseHeight);

        // Use intersection with the top or bottom plane based on the pixel's position
        float3 intersectionPoint = (worldPos.y > fogTopHeight ? intersectionTop : intersectionBottom);
        horizontalDistance = length(float2(cameraPos.x - intersectionPoint.x, cameraPos.z - intersectionPoint.z));
        verticalDistance = abs(cameraPos.y - intersectionPoint.y);
    }
    else if (!isCameraInFogLayer && isPixelInFogLayer)
    {
        // Pixel is in the fog layer, camera is outside
        float3 intersectionTop = ComputePlaneIntersection(cameraPos, worldPos, fogTopHeight);
        float3 intersectionBottom = ComputePlaneIntersection(cameraPos, worldPos, FogBaseHeight);

        // Use intersection with the top or bottom plane based on the camera's position
        float3 intersectionPoint = (cameraPos.y > fogTopHeight ? intersectionTop : intersectionBottom);
        horizontalDistance = length(float2(intersectionPoint.x - worldPos.x, intersectionPoint.z - worldPos.z));
        verticalDistance = abs(intersectionPoint.y - worldPos.y);
    }
    else
    {
        // Check if they are on the same side or opposite sides of the fog layer
        float fogMidHeight = FogBaseHeight + FogThickness * 0.5;
        bool isCameraAboveFogMid = cameraPos.y > fogMidHeight;
        bool isPixelAboveFogMid = worldPos.y > fogMidHeight;

        if (isCameraAboveFogMid == isPixelAboveFogMid)
        {
            // Camera and pixel are on the same side of the fog layer: no fog
            horizontalDistance = 0.0f;
            verticalDistance = 0.0f;
        }
        else
        {
            // Camera and pixel are on opposite sides of the fog layer: compute intersection points
            float3 intersectionTop = ComputePlaneIntersection(cameraPos, worldPos, fogTopHeight);
            float3 intersectionBottom = ComputePlaneIntersection(cameraPos, worldPos, FogBaseHeight);

            // Calculate the horizontal and vertical distances between the two intersection points
            horizontalDistance = length(float2(intersectionTop.x - intersectionBottom.x, intersectionTop.z - intersectionBottom.z));
            verticalDistance = abs(intersectionTop.y - intersectionBottom.y);
        }
    }
    
    float gFogEnd = FogRange + FogStart;
    float deltaD = horizontalDistance / gFogEnd;
    float deltaY = verticalDistance / FogThickness;
  
    float deltaCamera = saturate((fogTopHeight - cameraPos.y) / FogThickness);
    float densityIntegralCamera = deltaCamera * deltaCamera * 0.5f;
    float deltaPixel = saturate((fogTopHeight - worldPos.y) / FogThickness);
    float densityIntegralPixel = deltaPixel * deltaPixel * 0.5f;
    float densityIntegral = abs(densityIntegralCamera - densityIntegralPixel);
    
    float fogDensity = 0.0f;

    if (deltaY != 0)
    {
        fogDensity = (sqrt(1.0f + ((deltaD / deltaY) * (deltaD / deltaY)))) * densityIntegral;
    }

    float fogFactor = exp(-fogDensity * ExpFogDensity);

    return fogFactor * (1.0f - ExpFogDensity * deltaY);
}

// Fog color computation
float4 ApplyFog(float4 color, float3 worldPos, float3 cameraPos)
{
    float fogFactor = 1.0;
    float cameraToPixelDist = length(worldPos - cameraPos);

    if (FogType == 0) // Linear fog
    {
        fogFactor = CalcLinearFogFactor(cameraToPixelDist);
    }
    else if (FogType == 1) // Exponential fog
    {
        fogFactor = CalcExpFogFactor(cameraToPixelDist);
    }
    else if (FogType == 2) // Layered fog
    {
        fogFactor = CalcLayeredFogFactor(cameraPos, worldPos);
    }
    
    fogFactor = max(fogFactor, 1.0f - Opacity);
    
    return lerp(FogColor, color, fogFactor);
}

//--------------------------------------------------------------------------------------

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 PosMS : POSITION;
    float4 Color : COLOR;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION )
{
    VS_OUTPUT output;
    output.PosMS = Pos; // We pass along the raw model space position  
    output.Pos = mul( Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Color = SelectedColor;
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    float4 color = input.Color;
    
    /******************* Fog Effect ***********************/    
    float4 worldPos = mul(input.PosMS, World);
    float3 cameraPos = EyePosWorld.xyz;
    color = ApplyFog(color, worldPos.xyz, cameraPos);
    /****************************************************/
    
    return color;
}