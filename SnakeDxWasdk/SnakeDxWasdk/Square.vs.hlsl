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
    float4 color : COLOR;
    float4 pos : SV_POSITION;
};

cbuffer Camera
{
    float4x4 vp;
};


VertexOut main(
    float4 i_position : InstancePosition,
    float4 i_color : InstanceColor,
    uint i : SV_VertexID,
    uint j : SV_InstanceID
)
{
    VertexOut vOut =
    {
        i_color,
        mul(float4(triangles[i % 6] + i_position, 0, 1), vp),
    };
    return vOut;
}