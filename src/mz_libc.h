#ifndef MZ_LIBC_INCLUDED__
#define MZ_LIBC_INCLUDED__

#include "mz_types.h"

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define INT_MAX 2147483647
#define SIZE_MAX 0xffffffffffffffffui64

typedef struct FILE FILE;

void *malloc(u64 size);
void *realloc(void *addr, u64 size);
void free(void *addr);

FILE *fopen(const char *filename, const char *mode);
i32 fopen_s(FILE **pFile, const char *filename, const char *mode);
i32 fseek(FILE *stream, i32 offset, i32 origin);
i32 ftell(FILE *stream);
u64 fread(void *buffer, u64 size, u64 count, FILE *stream);
i32 fclose(FILE *stream);
i32 feof(FILE *stream);

void *memset(void *dest, i32 ch, u64 count);
i32 memcmp(const void *lhs, const void *rhs, u64 count);
void *memmove(void *dest, const void *src, u64 count);
void *memcpy(void *dest, const void *src, u64 count);

u64 strlen(const char *str);
i32 strcmp(const char *string1, const char *string2);
i32 strncmp(const char *string1, const char *string2, u64 count);
long strtol(const char *string, char **end_ptr, int base);
char *strcpy(char *str_destination, const char *str_source);
char *strncpy(char *str_destination, const char *str_source, u64 count);
char *strrchr(const char *str, i32 c);
char *strchr(const char *str, i32 c);
char *strstr(const char *str, const char *str_search);
i32 atoi(const char *str);
f64 atof(const char *str);

f64 pow(f64 x, f64 y);
i32 abs(i32 n);
unsigned long _lrotl(unsigned long value, int shift);
f64 ldexp(f64 x, i32 exp);

void exit(i32 status);

#endif // #ifndef MZ_LIBC_INCLUDED__
