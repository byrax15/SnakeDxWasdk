static const float2 triangles[] =
{
    { -.5, .5 },
    { .5, -.5 },
    { -.5, -.5 },
    { -.5, .5 },
    { .5, .5 },
    { .5, -.5 },
};

static const float4 colors[] =
{
    { 0, 0, 0, 1 },
    { 1, 0, 0, 1 },
    { 0, 1, 0, 1 },
    { 0, 0, 1, 1 },
};

struct VertexOut
{
    float4 color : COLOR0;
    float4 pos : SV_POSITION;
};

struct VertexIn
{
    int4 i_position : InstancePosition;
    float4 i_color : InstanceColor;
};

cbuffer Camera
{
    float4x4 vp;
};

VertexOut main(
    VertexIn vIn,
    uint i : SV_VertexID,
    uint j : SV_InstanceID
)
{
    VertexOut vOut =
    {
        vIn.i_color,
        mul(float4(triangles[i % 6].xy + vIn.i_position.xy, 0, 1), vp),
    };
    return vOut;
}