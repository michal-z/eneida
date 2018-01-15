#include "demo01.h"
#include "library.h"


static GLuint s_cppo;
static GLuint s_f3_vao;
static GLuint s_vs;
static GLuint s_fs;
static GLuint s_vbo;

void demo_update(double frame_time, float frame_delta_time)
{
    (void)frame_time; (void)frame_delta_time;
}

void demo_draw(void)
{
    glUseProgramStages(s_cppo, GL_VERTEX_SHADER_BIT, s_vs);
    glUseProgramStages(s_cppo, GL_FRAGMENT_SHADER_BIT, s_fs);

    glBindVertexArray(s_f3_vao);

    glVertexArrayVertexBuffer(s_f3_vao, 0, s_vbo, 0, 8);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void demo_init(void)
{
    /* vbo */ {
        float data[] = { -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 1.0f };
        glCreateBuffers(1, &s_vbo);
        glNamedBufferStorage(s_vbo, sizeof(data), data, 0);
    }

    // vao
    glCreateVertexArrays(1, &s_f3_vao);
    glVertexArrayAttribBinding(s_f3_vao, 0, 0);
    glVertexArrayAttribFormat(s_f3_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(s_f3_vao, 0);

    // shaders
    glCreateProgramPipelines(1, &s_cppo);
    glBindProgramPipeline(s_cppo);

    char *glsl = load_text_file("demo01.glsl");

    const char *vsglsl[] = { "#version 450 core\n", "#define VS_FULL_TRIANGLE\n", (const char *)glsl };
    s_vs = glCreateShaderProgramv(GL_VERTEX_SHADER, 3, vsglsl);

    const char *fsglsl[] = { "#version 450 core\n", "#define FS_DESERT\n", (const char *)glsl };
    s_fs = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 3, fsglsl);

    free(glsl);




    //Mat4 m = mat4_perspective(1.5f, 1.0f, 0.1f, 10.0f);
    //(void)m;
    //
    //float x = sinf(20.383f);
    //assert(x == 0.0f);

    assert(glGetError() == GL_NO_ERROR);
}

void demo_shutdown(void)
{
}
