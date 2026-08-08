#include "Common.hlsli"
#include "VertexTypes.hlsli"

VertexPCOut VSMain(VertexPCIn input)
{
    VertexPCOut output;

    output.outPosition = mul(frame.proj, mul(frame.view, mul(obj.world, float4(input.inPosition, 1.0))));
    output.fragColor = input.inColor;

    return output;
}

FragOut PSMain(VertexPCOut input)
{
    FragOut output;

    output.outColor = input.fragColor * obj.colorTint;

    return output;
}
