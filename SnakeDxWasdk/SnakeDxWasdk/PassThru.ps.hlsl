struct VertexOut
{
    float4 color : COLOR0;
};


float4 main(
    VertexOut vOut
) : SV_TARGET
{
    return vOut.color;
}