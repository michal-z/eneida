#if defined(VS_FULL_TRIANGLE)

layout(location = 0) in vec2 s_vertex;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main(void)
{
    gl_Position = vec4(s_vertex, 0.0, 1.0);
}

#elif defined(FS_DESERT)

layout(location = 0) out vec4 s_color;

void main(void)
{
    s_color = vec4(0.25, 0.0, 0.5, 1.0);
}

#endif
