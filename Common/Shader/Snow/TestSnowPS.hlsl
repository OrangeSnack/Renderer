#include "../CommonSharedPS.hlsli"

Texture2D _ambientOcclusion : register(t10);
Texture2D _lutMap : register(t11);

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

    float shadow = 0.0f;
    float2 texelSize = 1.0f / 4096.0f; // (가능하면 엔진에서 shadowmap size 넘기는게 베스트)

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

    return shadow / 9.0f;
}

// ---- noise 함수들은 그대로 ----
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
    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}
float fbm(float2 uv, int oct)
{
    float v = 0.0, amp = 0.5, freq = 1.0;
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
    // ===== Base Pattern =====
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

    float3 deepSnow = float3(0.75, 0.85, 1.0);
    float3 brightSnow = float3(1.0, 1.0, 1.0);
    float3 iceColor = float3(0.7, 0.85, 1.0);

    float3 albedo = lerp(deepSnow, brightSnow, base);
    albedo = lerp(albedo, iceColor, iceMask);

    // ===== Micro Normal =====
    float eps = 0.02;
    float hL = fbm(uv - float2(eps, 0), 3);
    float hR = fbm(uv + float2(eps, 0), 3);
    float hD = fbm(uv - float2(0, eps), 3);
    float hU = fbm(uv + float2(0, eps), 3);

    float3 microNormal = normalize(float3(hL - hR, 1.0, hD - hU));
    float3 N = normalize(input.Norm + microNormal * 1.5);

    base = pow(base, 1.8);

    float3 V = normalize(mCamPos.xyz - input.W_Pos.xyz);
    float3 L = normalize(-mLightDir.xyz);
    float NdotL = saturate(dot(N, L));

    // ===== AO =====
    float aoTex = saturate(_ambientOcclusion.Sample(_sp0, input.Tex).r);

    // ===== Shadow =====
    float shadow = CalculateShadowPCF(input.S_Pos); // 0~1

    // ✅ 핵심: 툰처럼 LUT로 “절도 있는” shadow 만들기
    // 툰 셰이더가 쓰던 x좌표 매핑 그대로 가져옴
    float shadowLut = _lutMap.Sample(_samPoint, float2(shadow * 0.5f + 0.495f, 0.5f)).r;

    // ✅ 너무 검게 떨어지는 걸 방지(원하면 값 조절)
    shadowLut = lerp(0.35f, 1.0f, shadowLut);

    // ===== Lighting =====
    float3 ambient = mAoStrength * mLightColor;
    ambient *= aoTex;

    float3 diffuse = (NdotL * mLightColor) * mIntensity;

    // ✅ Shadow는 diffuse에 절도있게 적용
    diffuse *= shadowLut;

    // AO는 diffuse에도 살짝
    diffuse *= lerp(1.0f, aoTex, 0.35f);

    float3 col = albedo * (ambient + diffuse);

    // Rim/Subsurface는 그대로
    float rim = pow(1.0 - saturate(dot(N, V)), 3.0);
    col += rim * float3(0.6, 0.75, 1.0) * 0.3;

    float subsurface = pow(saturate(dot(L, -V)), 4.0);
    col += subsurface * float3(0.5, 0.65, 1.0) * 0.25;

    return float4(col, 1.0f);
}
