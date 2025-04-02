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

// Structured buffers for lights
StructuredBuffer<PointLight> PointLights : register(t1);
StructuredBuffer<SpotLight> SpotLights : register(t2);

// Constants for the number of lights
cbuffer LightCounts : register(b1)
{
    DirectionalLight DirLight; // Directional light data
    float4 EyePosWorld;        // Eye position in world space
    int NumPointLights;        // Number of active point lights
    int NumSpotLights;         // Number of active spotlights
};

// Constant Buffers
// Constant Buffers
Texture2D mainTexture : register(t0);
Texture2D secondTexture : register(t3); // Second texture
Texture2D normalMap : register(t4);
Texture2D secondNormalMap : register(t5); // Third texture

SamplerState aSampler1 : register(s0); // Sampler1
SamplerState aSampler2 : register(s1); // Sampler2

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

//// Function to blend textures based on angle and number of textures
//float4 BlendTextures(
//    Texture2D mainTexture, SamplerState mainSampler,
//    Texture2D secondTexture, SamplerState secondSampler,
//    Texture2D thirdTexture, SamplerState thirdSampler,
//    float2 texCoord, float3 normal)
//{
//    const float uv1R = 1.f;
//    const float uv2R = 1.f;
//    const float uv3R = 1.f;    
//    // Sample the main texture
//    float4 texColor = mainTexture.Sample(mainSampler, texCoord * uv1R);
//    float4 secondTexColor = secondTexture.Sample(secondSampler, texCoord * uv2R);
//    float4 thirdTexColor = thirdTexture.Sample(thirdSampler, texCoord * uv3R);
//    // Constants for angle-based blending
//    const float minAngle = 0.55f;
//    const float maxAngle = 1.f;
//    const float lerp123 = 0.35f;
//    const float lerp23 = 0.35f;
//    // Calculate the angle between the normal and the up vector (0, 1, 0)
//    float angle = abs(dot(normal, float3(0.0f, 1.0f, 0.0f)));
//    // Blend between main and second texture based on angle
//    if (angle >= minAngle && angle <= maxAngle)
//    {
//        float blendFactor = (angle - minAngle) * (1.0f / (maxAngle - minAngle));
//        texColor = lerp(secondTexColor, texColor, blendFactor);
//        texColor = lerp(texColor, thirdTexColor, lerp123);
//    }
//    else if (angle < minAngle)
//    {
//        texColor = secondTexColor;
//        texColor = lerp(texColor, thirdTexColor, lerp23);
//    }
//    return texColor;
//}

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

// Shader operations
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 PosMS : POSITION;
    float2 Tex : TEXCOORD;
    float4 Norm : NORMAL;
    float4 Tangent : TANGENT;
};

// Vertex Shader
VS_OUTPUT VS(float4 Pos : POSITION, float2 tex : TEXCOORD, float4 nor : NORMAL, float4 tan : TANGENT)
{
    VS_OUTPUT output;
    output.PosMS = Pos;
    output.Norm = nor;
    output.Tex = tex;
    output.Tangent = tan;

    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    return output;
}

// Pixel Shader
float4 PS(VS_OUTPUT input) : SV_Target
{
    // Sample the normal map
    float3 normalMapSample1 = normalMap.Sample(aSampler1, input.Tex).xyz;
    float3 normalMapSample2 = secondNormalMap.Sample(aSampler2, input.Tex).xyz;
    // Unpack the normal from [0, 1] to [-1, 1]
    float3 normalTS1 = normalize(normalMapSample1 * 2.0f - float3(1.f, 1.f, 1.f));
    float3 normalTS2 = normalize(normalMapSample2 * 2.0f - float3(1.f, 1.f, 1.f));

    // Construct the TBN matrix in model space
    float3 N = normalize(input.Norm.xyz);
    float3 T = normalize(input.Tangent.xyz);
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);

    // Transform the normal from tangent space to model space
    float3 norm1 = normalize(mul(normalTS1, TBN));
    float3 norm2 = normalize(mul(normalTS2, TBN));
    float4 texColor = mainTexture.Sample(aSampler1, input.Tex * 1.0f);
    float4 secondTexColor = secondTexture.Sample(aSampler2, input.Tex * 1.0f);
    
    float4 msEyePos = mul(EyePosWorld, WorldInv);
    float4 msDirToEye = normalize(msEyePos - input.PosMS);

    float4 ambient1 = float4(0, 0, 0, 0);
    float4 diffuse1 = float4(0, 0, 0, 0);
    float4 spec1 = float4(0, 0, 0, 0);
    
    float4 ambient2 = float4(0, 0, 0, 0);
    float4 diffuse2 = float4(0, 0, 0, 0);
    float4 spec2 = float4(0, 0, 0, 0);

    float4 A, D, S;
    //float3 norm = input.Norm.xyz;
    //norm = normalize(norm);

    ComputeDirectionalLight(Mater, DirLight, float4(norm1, 0.0f), msDirToEye, A, D, S);
    ambient1 += A;
    diffuse1 += D;
    spec1 += S;
    
    ComputeDirectionalLight(Mater, DirLight, float4(norm2, 0.0f), msDirToEye, A, D, S);
    ambient2 += A;
    diffuse2 += D;
    spec2 += S;

    for (int i = 0; i < NumPointLights; i++)
    {
        ComputePointLight(Mater, PointLights[i], input.PosMS, float4(norm1, 0.0f), msDirToEye, A, D, S);
        ambient1 += A;
        diffuse1 += D;
        spec1 += S;
        
        ComputePointLight(Mater, PointLights[i], input.PosMS, float4(norm2, 0.0f), msDirToEye, A, D, S);
        ambient2 += A;
        diffuse2 += D;
        spec2 += S;
    }

    for (int j = 0; j < NumSpotLights; j++)
    {
        ComputeSpotLight(Mater, SpotLights[j], input.PosMS, float4(norm1, 0.0f), msDirToEye, A, D, S);
        ambient1 += A;
        diffuse1 += D;
        spec1 += S;
        
        ComputeSpotLight(Mater, SpotLights[j], input.PosMS, float4(norm2, 0.0f), msDirToEye, A, D, S);
        ambient2 += A;
        diffuse2 += D;
        spec2 += S;
    }
    
    float4 litColor1 = ambient1 + diffuse1 + spec1;
    litColor1 = litColor1 * texColor;
    
    float4 litColor2 = ambient2 + diffuse2 + spec2;
    litColor2 = litColor2 * secondTexColor;
    
    // Constants for angle-based blending
    const float minAngle = 0.32f;
    const float maxAngle = 0.97f;
    
    float4 litColor = litColor1;

    //// Calculate the angle between the normal and the up vector (0, 1, 0)
    float angle = abs(dot(N, float3(0.0f, 1.0f, 0.0f)));

    // Blend between main and second texture based on angle
    if (angle >= minAngle && angle <= maxAngle)
    {
        float blendFactor = (angle - minAngle) * (1.0f / (maxAngle - minAngle));
        litColor = lerp(litColor2, litColor, blendFactor);
    }
    else if (angle < minAngle)
    {
        litColor = litColor2;
    }
     
    /******************* Fog Effect ***********************/
    float4 worldPos = mul(input.PosMS, World);
    float3 cameraPos = EyePosWorld.xyz;
    litColor = ApplyFog(litColor, worldPos.xyz, cameraPos);
    /****************************************************/
    
    if (input.PosMS.y < -10.0f + 12.f)
    {
        litColor.w = exp((input.PosMS.y - 2.0f) * 0.55f);
    }
    else
    {
        litColor.w = 1.0f;
    }
    
    return litColor;
}