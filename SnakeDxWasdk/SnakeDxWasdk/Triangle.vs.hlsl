static const float2 triangles[] =
{
    { 0., .5 },
    { .5, -.5 },
    { -.5, -.5 }
};

static const float4 colors[] =
{
    { 1, 0, 0, 1 },
    { 0, 1, 0, 1 },
    { 0, 0, 1, 1 },
};

struct VertexOut
{
    float4 color : COLOR;
    float4 pos : SV_POSITION;
};

cbuffer Camera : register(b0)
{
    float4 color;
    float4x4 v;
    float4x4 p;
};


VertexOut main(
    float4 i_position : InstancePosition,
    uint i : SV_VertexID,
    uint j : SV_InstanceID
)
{
    VertexOut vOut =
    {
        color,
        mul(float4(triangles[i % 3] + i_position, 0, 1), mul(v, p)),
    };
    return vOut;
}