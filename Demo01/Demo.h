#pragma once

#define NULL ((void *)0)
#define NL "\n"

#ifdef _DEBUG
#define Assert(Expression) if (!(Expression)) { __debugbreak(); }
#else
#define Assert(Expression) {}
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef float f32;
typedef double f64;
typedef u64 usize;
typedef i64 isize;

typedef struct array_header
{
    u32 Size;
    u32 Capacity;
} array_header;

static void *MemAllocate(usize Size);
static void *MemReAllocate(void *Address, usize Size);
static void MemFree(void *Address);

void *memset(void *Dest, i32 Value, usize Count);
#pragma intrinsic(memset)

#define ArrayHeader(A) \
    ((array_header *)((char *)(A) - sizeof(array_header)))

#define ArraySize(A) ((A) ? ArrayHeader(A)->Size : 0)
#define ArrayCapacity(A) ((A) ? ArrayHeader(A)->Capacity : 0)
#define ArrayIsFull(A) (ArraySize(A) == ArrayCapacity(A) ? 1 : 0)
#define ArrayFree(A) ((A) ? (MemFree(ArrayHeader(A)), 0) : 0)

#define ArrayAdd(A, Item) \
    (ArrayIsFull(A) ? A = ArrayGrow((A), sizeof(*(A))) : 0), \
    ((A)[ArrayHeader(A)->Size++] = (Item))

inline void *
ArrayGrow(void *Array, u32 ItemSize)
{
    u32 Size = ArraySize(Array);
    u32 Capacity = Size ? Size * 2 : 1;
    Array = Array ? ArrayHeader(Array) : NULL;
    array_header *Ha = (array_header *)MemReAllocate(Array, Capacity * ItemSize + sizeof(array_header));
    Ha->Size = Size;
    Ha->Capacity = Capacity;
    return (void *)(Ha + 1);
}

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_TRIANGLES                      0x0004

static void (__stdcall *glCreateTextures)(u32 Target, i32 N, u32 *Textures);
static void (__stdcall *glCreateBuffers)(i32 N, u32 *Buffers);
static void (__stdcall *glDeleteProgram)(u32 Program);
static void (__stdcall *glDeleteTextures)(i32 N, const u32 *Textures);
static void (__stdcall *glDeleteBuffers)(i32 N, const u32 *Buffers);
static void (__stdcall *glDrawArrays)(u32 Mode, i32 First, i32 Count);
static void (__stdcall *glNamedBufferStorage)(u32 Buffer, isize Size, const void *Data, u32 Flags);
static void (__stdcall *glDispatchCompute)(u32 NumGroupsX, u32 NumGroupsY, u32 NumGroupsZ);
static void (__stdcall *glTextureStorage2D)(u32 Texture, i32 Levels, u32 InternalFormat, i32 Width, i32 Height);
static void (__stdcall *glTextureParameteri)(u32 Texture, u32 ParamName, i32 Param);
static void (__stdcall *glBindTextureUnit)(u32 Unit, u32 Texture);
static void (__stdcall *glBindImageTexture)(u32 Unit, u32 Texture, i32 Level, u8 Layered, i32 Layer, u32 Access, u32 Format);
static u32 (__stdcall *glGetError)(void);
static void (__stdcall *glShaderSource)(u32 Shader, i32 Count, const char *const*String, const i32 *Length);
static void (__stdcall *glUseProgram)(u32 Program);
static u32 (__stdcall *glCreateProgram)(void);
static u32 (__stdcall *glCreateShader)(u32 Type);
static void (__stdcall *glCompileShader)(u32 Shader);
static void (__stdcall *glAttachShader)(u32 Program, u32 Shader);
static void (__stdcall *glDeleteShader)(u32 Shader);
static void (__stdcall *glDetachShader)(u32 Program, u32 Shader);
static void (__stdcall *glLinkProgram)(u32 Program);

inline u32
BuildProgram(const char *VertexShaderSource, const char *FragmentShaderSource)
{
    u32 VertexShader = glCreateShader(GL_VERTEX_SHADER);
    u32 FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(VertexShader, 1, &VertexShaderSource, NULL);
    glShaderSource(FragmentShader, 1, &FragmentShaderSource, NULL);

    glCompileShader(VertexShader);
    glCompileShader(FragmentShader);

    u32 Program = glCreateProgram();

    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);

    glLinkProgram(Program);

    glDetachShader(Program, VertexShader);
    glDetachShader(Program, FragmentShader);

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    return Program;
}
