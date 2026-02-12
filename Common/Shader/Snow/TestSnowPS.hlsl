#include "../CommonSharedPS.hlsli"

Texture2D _ambientOcclusion : register(t10);

// b0은 Cambuffer라서 쓰면 충돌남.
// 엔진에서 비어있는 슬롯로 맞춰줘. (여기서는 b5로 가정)
cbuffer SnowParams : register(b5)
{
    float tileScale;
    float warpStrength;
    float windStrength;
    float iceStrength;

    int octaves;
    float mAoStrength;
    float2 padding;
};

// 수동 PCF
float CalculateShadowPCF(float4 LightPos)
{
    float currentShadowDepth = LightPos.z / LightPos.w;
    float2 shadowUV = LightPos.xy / LightPos.w;

    shadowUV.y *= -1.0f;
    shadowUV = (shadowUV * 0.5f) + 0.5f;

    if (shadowUV.x < 0.0f || shadowUV.x > 1.0f ||
        shadowUV.y < 0.0f || shadowUV.y > 1.0f)
        return 1.0f;

    if (currentShadowDepth < 0.0f || currentShadowDepth > 1.0f)
        return 1.0f;

    float bias = 0.0001f;
    currentShadowDepth -= bias;

    // 3x3 PCF
    float shadow = 0.0f;
    float2 texelSize = 1.0f / 4096.0f;

    [unroll]
    for (int x = -1; x <= 1; ++x)
    {
        [unroll]
        for (int y = -1; y <= 1; ++y)
        {
            float2 offset = float2(x, y) * texelSize;
            shadow += _shadowmap.SampleCmpLevelZero(_cmpsp0, shadowUV + offset, currentShadowDepth);
        }
    }
    shadow /= 9.0f;

    return shadow;
}

float hash(float2 p)
{
    return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453);
}

float valueNoise(float2 uv)
{
    float2 i = floor(uv);
    float2 f = frac(uv);

    float a = hash(i);
    float b = hash(i + float2(1, 0));
    float c = hash(i + float2(0, 1));
    float d = hash(i + float2(1, 1));

    float2 u = f * f * (3.0 - 2.0 * f);

    return lerp(lerp(a, b, u.x),
                lerp(c, d, u.x), u.y);
}

float fbm(float2 uv, int oct)
{
    float v = 0.0;
    float amp = 0.5;
    float freq = 1.0;

    [loop]
    for (int i = 0; i < oct; i++)
    {
        v += valueNoise(uv * freq) * amp;
        freq *= 2.0;
        amp *= 0.5;
    }

    return v;
}

float4 main(PS_INPUT input) : SV_Target
{
    // -------- Base Snow Pattern --------
    float2 uv = input.W_Pos.xz * tileScale;

    float2 warp;
    warp.x = fbm(uv + float2(12.7, 3.1), 3);
    warp.y = fbm(uv + float2(5.3, 19.1), 3);

    uv += (warp * 2.0 - 1.0) * warpStrength;

    float base = fbm(uv, octaves);
    base = base * base;

    float wind = 1.0 - abs(fbm(uv * 1.5, 3) * 2.0 - 1.0);
    base += wind * windStrength;

    float iceMask = smoothstep(0.65, 0.85, base) * iceStrength;

    float3 snow = float3(0.9, 0.95, 1.0);
    float3 ice = float3(0.75, 0.85, 1.0);

    float3 albedo = lerp(snow, ice, iceMask);
    albedo *= (0.7 + base * 0.6);

    // -------- AO --------
    float aoTex = saturate(_ambientOcclusion.Sample(_sp0, input.Tex).r);
    float ao = lerp(1.0f, aoTex, saturate(mAoStrength));

    // -------- Shadow (Directional) --------
    float shadow = CalculateShadowPCF(input.S_Pos); // 1=lit, 0=shadow
    const float minShadow = 0.35f;
    shadow = lerp(minShadow, 1.0f, shadow);

    // -------- Lighting (Directional LightBuffer from CommonSharedPS.hlsli) --------
    // CommonSharedPS.hlsli의 LightBuffer(b1):
    // float3 mLightDir; float3 mLightColor; float mIntensity;
    // mLightDir가 "빛이 향하는 방향"인지, "빛이 오는 방향"인지 헷갈릴 수 있어서
    // (보통 엔진은 '빛이 향하는 방향'을 많이 넣음) → 기본은 -mLightDir로 라이트벡터를 만듦.
    float3 N = normalize(input.Norm);
    float3 L = normalize(-mLightDir); // Light direction (to light)
    float NdotL = saturate(dot(N, L)); // Lambert

    // 최소한의 환경광(빛 색 기반)
    float3 ambient = 0.12f * mLightColor;

    // 확산광
    float3 diffuse = (NdotL * mLightColor) * mIntensity;

    // AO는 주로 ambient에 강하게 적용하는 편이 자연스러움
    ambient *= ao;
    // diffuse에도 살짝만(원하면 1.0f로 올려도 됨)
    diffuse *= lerp(1.0f, ao, 0.35f);

    // Shadow는 보통 diffuse에 적용 (ambient는 유지하거나 약하게만)
    diffuse *= shadow;

    float3 col = albedo * (ambient + diffuse);

    return float4(col, 1.0f);
}
