static const float2 triangles[] =
{
    { 0., .5 },
    { .5, -.5 },
    { -.5, -.5 }
};

static const float2 offsets[] =
{
    { .6, .1 },
    { -.4, -.2 },
    { .1, 0 },
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


VertexOut main(
    uint i : SV_VertexID,
    uint j : SV_InstanceID
)
{
    VertexOut vOut =
    {
        colors[i % 3] * .5 + colors[j % 3] * .5,
        float4(triangles[i % 3] + offsets[j % 3], 0, 1),
    };
    return vOut;
}