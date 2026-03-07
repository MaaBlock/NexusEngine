struct VSInput {
    [[vk::location(0)]] float2 Position : POSITION0;
    [[vk::location(1)]] float4 Color : COLOR0;
    [[vk::location(2)]] float2 UV : TEXCOORD0;
};

struct PSInput {
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
    float2 UV : TEXCOORD0;
};

struct BindlessConstants {
    float2 translation;
    float2 windowSize;
    uint textureIndex;
    uint samplerIndex;
};

[[vk::push_constant]]
ConstantBuffer<BindlessConstants> constants;

[[vk::binding(0, 0)]]
SamplerState samplers[];

[[vk::binding(1, 0)]]
Texture2D textures[];

PSInput VSMain(VSInput input) {
    PSInput output;
    
    // RmlUi coordinates are pixel-based, top-left origin.
    // Convert to Vulkan NDC (-1 to 1)
    float2 translatedPos = input.Position + constants.translation;
    float2 ndc = (translatedPos / constants.windowSize) * 2.0 - 1.0;
    
    // In Vulkan, Y is down in NDC, but RmlUi is already Y-down in pixels.
    // However, if we map (0,0) to (-1, -1) and (W,H) to (1, 1),
    // we match Vulkan's NDC directly.
    output.Pos = float4(ndc.x, ndc.y, 0.0, 1.0);
    
    output.Color = input.Color;
    
    output.UV = input.UV;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET {
    float4 texColor = textures[constants.textureIndex].Sample(samplers[constants.samplerIndex], input.UV);
    return input.Color * texColor;
}
