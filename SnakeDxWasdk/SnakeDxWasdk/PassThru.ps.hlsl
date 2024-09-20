struct VertexIn
{
    float4 color : COLOR;
};


float4 main(
    VertexIn vIn
) : SV_TARGET
{
    return vIn.color;
}