static const float2 triangles[] =
{
    { 0., .5 },
    { .5, -5 },
    { -.5, -.5 }
};

static const float2 offsets[] =
{
    { -.6, 0 },
    { .4, .1 }
};

float4 main(uint i : SV_VertexID, uint j : SV_InstanceID) : SV_POSITION
{
    float4 pos = { 0, 0, 0, 1 };
    switch (i % 3)
    {
        case 0:
            pos.xy = triangles[0];
            break;
        case 1:
            pos.xy = triangles[1];
            break;
        case 2:
            pos.xy = triangles[2];
            break;
    }
    switch (j % 2)
    {
        case 0:
            pos.xy += offsets[0];
            break;
        case 1:
            pos.xy += offsets[1];
            break;
    }
    
    return pos;
}