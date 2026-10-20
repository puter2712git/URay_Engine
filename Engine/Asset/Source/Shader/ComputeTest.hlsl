RWStructuredBuffer<uint> Output : register(u0, space1);

[numthreads(1, 1, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    Output[id.x] = id.x + 1;
}