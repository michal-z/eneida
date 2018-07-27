typedef struct test1_data
{
    u32 Program;
} test1_data;

const char *Test1_Vs =
"#version 450 core"
NL "void main()"
NL "{"
NL "    vec2[] Positions = { vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(0.0, 1.0) };"
NL "    gl_Position = vec4(Positions[gl_VertexID], 0.0, 1.0);"
NL "}";

const char *Test1_Fs =
"#version 450 core"
NL "void main()"
NL "{"
NL "    gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);"
NL "}";

static void
Test1_Update(test1_data *Data)
{
    glUseProgram(Data->Program);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

static void
Test1_Initialize(test1_data *Data)
{
    Data->Program = BuildProgram(Test1_Vs, Test1_Fs);
}

static void
Test1_Shutdown(test1_data *Data)
{
    glDeleteProgram(Data->Program);
}
