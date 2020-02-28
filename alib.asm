format MS64 COFF

    public memset public memcpy public sqrtf

        macro defshader
sh{sh : file `sh #'.cso' sh #_size dd $ - sh public sh public sh #_size}

section '.text' code readable executable

    ;
TODO(mziulek)
    : This is very slow.memset : xor eax,
eax .0 : mov[rcx + rax], dl inc rax dec r8 jnz .0 mov rax, rcx ret

    ;
TODO(mziulek)
    : This is very slow.memcpy : xor eax,
eax .0 : mov r10b,
         [rdx + rax] mov[rcx + rax],
         r10b inc rax dec r8 jnz .0 mov rax,
         rcx ret

    ;
TODO(mziulek)
    : Add some debug stuff if needed.sqrtf : sqrtss xmm0,
xmm0 ret

    section '.data' data readable writeable

    ;
defshader vs_e01_transform;
defshader ps_e01_shade
