#pragma pack_matrix(row_major)

// Material properties
struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // Hack: w holds the specular power
};

// Light properties
struct PhongADS
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
};

// Directional Light Data
struct DirectionalLight
{
    PhongADS LightInt;
    float4 Direction;
};

// Point Light data
struct PointLight
{
    PhongADS LightInt;
    float4 Position;
    float4 Attenuation;
    float Range;
    float3 Paddings;
};

// Spot light data
struct SpotLight
{
    PhongADS LightInt;
    float4 Position;
    float4 Attenuation;
    float4 Direction;
    float SpotExp;
    float Range;
    float2 Paddings;
};

Texture2D heightMap : register(t0);
SamplerState aSampler : register(s0);

// Structured buffers for lights
StructuredBuffer<PointLight> PointLights : register(t1);
StructuredBuffer<SpotLight> SpotLights : register(t2);

// Constants for the number of lights
cbuffer LightCounts : register(b1)
{
    DirectionalLight DirLight; // Directional light data
    float4 EyePosWorld; // Eye position in world space
    int NumPointLights; // Number of active point lights
    int NumSpotLights; // Number of active spotlights
};

// Constant Buffers
cbuffer CamData : register(b0)
{
    float4x4 View;
    float4x4 Projection;
};

cbuffer InstanceData : register(b2)
{
    float4x4 World;
    float4x4 WorldInv;
    Material Mater;
};

// Fog parameters
cbuffer FogData : register(b3)
{
    float4 FogColor; // Fog color
    float FogStart; // Distance at which fog starts
    float FogRange; // Range over which fog increases linearly
    float FogThickness; // Thickness of the fog layer (vertical extent)
    float FogBaseHeight; // Base height of the fog layer (bottom of the fog)
    float ExpFogDensity; // Density for exponential fog
    float Opacity;
    int FogType; // 0 = Linear, 1 = Exponential, 2 = Layered
}


// Water parameters
cbuffer WaveData : register(b4)
{
    float Time; // Time for animation
    float heightMapSize;
    float waterSizeY;
};

// Basic Phong Illumination computation
void PhongModel(
    Material mat,
    PhongADS lightint,
    float3 L,
    float3 normal,
    float3 DirToEye,
    out float4 ambient,
    out float4 diffuse,
    out float4 spec
)
{
    ambient = mat.Ambient * lightint.Ambient;

    float diffuseFactor = dot(L, normal);

    if (diffuseFactor > 0.0f)
    {
        diffuse = diffuseFactor * mat.Diffuse * lightint.Diffuse;

        float3 r = reflect(-L, normal);
        float spec_val = max(dot(r, DirToEye.xyz), 0);
        float specFactor = pow(spec_val, mat.Specular.w);
        spec = specFactor * mat.Specular * lightint.Specular;
    }
    else
    {
        diffuse = float4(0, 0, 0, 0);
        spec = float4(0, 0, 0, 0);
    }
}

// Directional Light Computation
void ComputeDirectionalLight(
    Material mat,
    DirectionalLight DirLit,
    float4 normal,
    float4 DirToEye,
    out float4 ambient,
    out float4 diffuse,
    out float4 spec
)
{
    ambient = float4(0, 0, 0, 0);
    diffuse = float4(0, 0, 0, 0);
    spec = float4(0, 0, 0, 0);

    float3 L = normalize(mul(-DirLit.Direction.xyz, (float3x3) WorldInv));
    PhongModel(mat, DirLit.LightInt, L, normal.xyz, DirToEye.xyz, ambient, diffuse, spec);
}

// Point Light Computation
void ComputePointLight(
    Material mat,
    PointLight PLight,
    float4 posms,
    float4 normal,
    float4 DirToEye,
    out float4 ambient,
    out float4 diffuse,
    out float4 spec
)
{
    ambient = float4(0, 0, 0, 0);
    diffuse = float4(0, 0, 0, 0);
    spec = float4(0, 0, 0, 0);

    float3 litPosMS = mul(PLight.Position, WorldInv).xyz;
    float3 L = litPosMS - posms.xyz;

    float d = length(L);
    if (d > PLight.Range)
        return;

    L /= d;

    PhongModel(mat, PLight.LightInt, L, normal.xyz, DirToEye.xyz, ambient, diffuse, spec);

    float att = 1 / dot(PLight.Attenuation.xyz, float3(1, d, d * d));
    diffuse *= att;
    spec *= att;
}

// Spot Light Computation
void ComputeSpotLight(
    Material mat,
    SpotLight SptLight,
    float4 posms,
    float4 normal,
    float4 DirToEye,
    out float4 ambient,
    out float4 diffuse,
    out float4 spec
)
{
    ambient = float4(0, 0, 0, 0);
    diffuse = float4(0, 0, 0, 0);
    spec = float4(0, 0, 0, 0);

    float3 litPosMS = mul(SptLight.Position, WorldInv).xyz;
    float3 L = litPosMS - posms.xyz;

    float d = length(L);
    if (d > SptLight.Range)
        return;

    L /= d;

    PhongModel(mat, SptLight.LightInt, L, normal.xyz, DirToEye.xyz, ambient, diffuse, spec);

    if (dot(L, normal.xyz) > 0.0f)     // New test
    {
        // Now we attenuate based on range
        float att = 1 / dot(SptLight.Attenuation.xyz, float3(1, d, d * d));

        // Ambient not attenuated
        diffuse *= att;
        spec *= att;

        // *****************************************

        // now we compute the spotlight factor
        float4 msLitDir = SptLight.Direction;
        msLitDir.w = 0; // making sure we have a vector and not a point
        msLitDir = normalize(mul(msLitDir, WorldInv));
        float spot = pow(max(dot(-L, msLitDir.xyz), 0), SptLight.SpotExp);

        //ambient *= spot;    // now removed
        diffuse *= spot;
        spec *= spot;
    }
}

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
    
    fogFactor =  max(fogFactor, 1.0f - Opacity);
    
    return lerp(FogColor, color, fogFactor);
}

float3 ComputeNormalFromHeightMap(float heightMapSize, float2 texcoord, float normalFactor)
{
    float texelSize = 1.0f / heightMapSize;
    
    float t = heightMap.SampleLevel(aSampler, float2(texcoord.x, texcoord.y - texelSize), 0).r;
    float b = heightMap.SampleLevel(aSampler, float2(texcoord.x, texcoord.y + texelSize), 0).r;
    float l = heightMap.SampleLevel(aSampler, float2(texcoord.x - texelSize, texcoord.y), 0).r;
    float r = heightMap.SampleLevel(aSampler, float2(texcoord.x + texelSize, texcoord.y), 0).r;

    t *= normalFactor;
    b *= normalFactor;
    l *= normalFactor;
    r *= normalFactor;
    
    float3 normal = float3(-(r - l) * 0.5f, 1.0f, -(b - t) * 0.5f);

    return normalize(normal);

}

float GerstnerWave(float2 position, float amplitude, float frequency, float speed, float2 direction)
{
    float phase = frequency * dot(direction, position) + Time * speed;
    return amplitude * sin(phase);
}

//--------------------------------------------------------------------------------------

// Shader operations
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 PosMS : POSITION;
    float2 Tex :  TEXCOORD;
    float4 Norm : NORMAL;
};

// Vertex Shader
VS_OUTPUT VS(float4 Pos : POSITION, float2 tex : TEXCOORD, float4 nor : NORMAL)
{
    VS_OUTPUT output;
    
    float2 texcoord = (tex * 8) + Time * 0.015f;
    float2 texcoord1 = tex;
    float2 texcoord2 = (float2(-tex.x, tex.y) * 8) + Time * 0.015f;
    
    float height0 = heightMap.SampleLevel(aSampler, texcoord1, 0).r;
    float height1 = heightMap.SampleLevel(aSampler, texcoord, 0).r;
    float height2 = heightMap.SampleLevel(aSampler, texcoord2, 0).r;
    
    // Combine multiple Gerstner waves
    float wave1 = GerstnerWave(Pos.xz, 0.5f, 1.5f, 1.0f, normalize(float2(-1.1f, 0.8f)));
    float wave2 = GerstnerWave(Pos.xz, 0.3f, 2.0f, 1.5f, normalize(float2(0.7f, -0.7f)));
    float wave3 = GerstnerWave(Pos.xz, 0.2f, 3.0f, 2.0f, normalize(float2(0.5f, 0.59f)));
    
    float wave = wave1 + wave2 + wave3;
    //float height = lerp(height1, height2, 0.5f);
    // Use a weighted average to smooth out the height blending
    float height = (height1 * 0.18f) + (height0 * 0.64f) + (height2 * 0.18f);
    // Apply smoothstep to smooth out height variation
    float smoothHeight = smoothstep(0.0f, 1.0f, height);
    
    output.Pos = float4(Pos.x + wave, /*height*/smoothHeight * waterSizeY, Pos.z, 1.0f);
    
    output.PosMS = output.Pos; // We pass along the raw model space position 
    output.Norm = nor;  // and the face normal
    output.Tex = tex;
    output.Pos = mul(output.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    return output;
}

// Pixel Shader
float4 PS(VS_OUTPUT input) : SV_Target
{   
    float4 msEyePos = mul(EyePosWorld, WorldInv);
    float4 msDirToEye = normalize(msEyePos - input.PosMS);
    
    float2 texcoord = (input.Tex * 8) + Time * 0.012f;
    float2 texcoord2 = (float2(-input.Tex.x, input.Tex.y) * 8) + Time * 0.016f;
    
    float3 normal1 = ComputeNormalFromHeightMap(heightMapSize, texcoord, waterSizeY * 3.5f);
    float3 normal2 = ComputeNormalFromHeightMap(heightMapSize, texcoord2, waterSizeY * 3.5f);
    
    float3 norm = normalize(lerp(normal1, normal2, 0.5f));
    
    float4 ambient = float4(0, 0, 0, 0);
    float4 diffuse = float4(0, 0, 0, 0);
    float4 spec = float4(0, 0, 0, 0);

    float4 A, D, S;
    float3 norm1 = input.Norm.xyz;
    norm = normalize(lerp(norm1, norm, 0.48f));
    
    ComputeDirectionalLight(Mater, DirLight, float4(norm, 0.0f), msDirToEye, A, D, S);
    ambient += A;
    diffuse += D;
    spec += S;

    for (int i = 0; i < NumPointLights; i++)
    {
        ComputePointLight(Mater, PointLights[i], input.PosMS, float4(norm, 0.0f), msDirToEye, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }

    for (int j = 0; j < NumSpotLights; j++)
    {
        ComputeSpotLight(Mater, SpotLights[j], input.PosMS, float4(norm, 0.0f), msDirToEye, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }

    float4 litColor = ambient + diffuse + spec;
    
    /********************* Water Color ********************/
    float4 waterColor = float4( /*0.24f, 0.37f, 0.49f*/0.15f, 0.2f, 0.25f, 1.0f);
    float4 crestColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float angle = abs(dot(norm, float3(0.0f, 1.0f, -0.0001f)));
    
    float4 color = waterColor;
    float minAngle = 0.4f;
    float maxAngle = 1.f;
    
    if (angle >= minAngle && angle <= maxAngle)
    {
        color = lerp(crestColor, color, (angle - minAngle) * (0.98f / (maxAngle - minAngle)));
    }
    else
    {
        color = crestColor;
    }
    
    litColor = color * litColor;
 
    
	
    /******************* Fog Effect ***********************/
    float4 worldPos = mul(input.PosMS, World);
    float3 cameraPos = EyePosWorld.xyz;
    litColor = ApplyFog(litColor, worldPos.xyz, cameraPos);
    /****************************************************/
    litColor.w = 0.88f;
    
    return litColor;
}