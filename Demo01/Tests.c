typedef struct test1_data
{
    u32 Program;
    u32 GeometryBuffer;
    u32 TextureBuffer;
} test1_data;

static const char *Test1_Vs =
"#version 450 core"
NL "layout(binding = 0) uniform samplerBuffer s_Positions;"
NL "void main()"
NL "{"
NL "    gl_Position = texelFetch(s_Positions, gl_VertexID);"
NL "}";

static const char *Test1_Fs =
"#version 450 core"
NL "void main()"
NL "{"
NL "    gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);"
NL "}";

static void
Test1_Update(test1_data *Data)
{
    glUseProgram(Data->Program);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Data->GeometryBuffer);
    glBindTextures(0, 1, &Data->TextureBuffer);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (const void *)24);
}

static void
Test1_Initialize(test1_data *Data)
{
    Data->Program = BuildGpuProgram(Test1_Vs, Test1_Fs);

    struct {
        f32 VertexData[6];
        u16 IndexData[6];
    } GeometryData = {
        { -0.7f, -0.7f, 0.7f, -0.7f, 0.0f, 0.7f },
        { 0, 1, 2, 3, 4, 5 }
    };
    glCreateBuffers(1, &Data->GeometryBuffer);
    glNamedBufferStorage(Data->GeometryBuffer, sizeof(GeometryData), &GeometryData, 0);

    glCreateTextures(GL_TEXTURE_BUFFER, 1, &Data->TextureBuffer);
    glTextureBufferRange(Data->TextureBuffer, GL_RG32F, Data->GeometryBuffer, 0, 24);
}

static void
Test1_Shutdown(test1_data *Data)
{
    glDeleteProgram(Data->Program);
    glDeleteTextures(1, &Data->TextureBuffer);
    glDeleteBuffers(1, &Data->GeometryBuffer);
}
