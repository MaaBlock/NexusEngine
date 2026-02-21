struct VSInput {
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

// 使用 Push Constants 传递基础偏移或其他全局信息
struct PushConstants {
    uint textureIndex;
    uint samplerIndex;
    float4 baseColor;
};

[[vk::push_constant]]
PushConstants pushConstants;

// 绑定全局 Bindless 资源
[[vk::set(0)]] [[vk::binding(0)]] SamplerState samplers[64];
[[vk::set(0)]] [[vk::binding(1)]] Texture2D textures[1024];

VSOutput VSMain(VSInput input, uint instanceID : SV_InstanceID) {
    VSOutput output;
    
    // 简单的实例偏移: 每个实例在 X 轴上偏移
    float xOffset = (float(instanceID) - 2.0) * 0.5;
    float4 pos = float4(input.position, 1.0);
    pos.x += xOffset;
    
    output.position = pos;
    output.texCoord = input.texCoord;
    return output;
}

float4 PSMain(VSOutput input) : SV_Target {
    // 简单的带颜色的纹理采样
    float4 texColor = textures[pushConstants.textureIndex].Sample(samplers[pushConstants.samplerIndex], input.texCoord);
    return texColor * pushConstants.baseColor;
}
