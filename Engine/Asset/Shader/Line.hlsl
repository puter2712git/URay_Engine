#include "Common/Common.hlsli"
#include "Common/FrameConstants.hlsli"
#include "Common/ObjectConstants.hlsli"
#include "Common/VertexTypes.hlsli"

VertexPCOut VSMain(VertexPCIn input)
{
    VertexPCOut output;

    output.outPosition = mul(frame.proj, mul(frame.view, float4(input.inPosition, 1.0)));
    output.fragColor = input.inColor;

    return output;
}

FragOut PSMain(VertexPCOut input)
{
    FragOut output;

    output.outColor = input.fragColor;

    return output;
}
