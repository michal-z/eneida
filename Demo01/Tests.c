typedef struct test1_data
{
    u32 Program;
    u32 VertexBuffer;
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
    glBindTextures(0, 1, &Data->TextureBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

static void
Test1_Initialize(test1_data *Data)
{
    Data->Program = BuildGpuProgram(Test1_Vs, Test1_Fs);

    f32 BufferData[] = { -0.7f, -0.7f, 0.7f, -0.7f, 0.0f, 0.7f };
    glCreateBuffers(1, &Data->VertexBuffer);
    glNamedBufferStorage(Data->VertexBuffer, sizeof(BufferData), BufferData, 0);

    glCreateTextures(GL_TEXTURE_BUFFER, 1, &Data->TextureBuffer);
    glTextureBuffer(Data->TextureBuffer, GL_RG32F, Data->VertexBuffer);
}

static void
Test1_Shutdown(test1_data *Data)
{
    glDeleteProgram(Data->Program);
    glDeleteTextures(1, &Data->TextureBuffer);
    glDeleteBuffers(1, &Data->VertexBuffer);
}
