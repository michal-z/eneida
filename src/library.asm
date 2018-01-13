PUBLIC memset

.CODE

memset:     xor         eax, eax
A000:       mov         [rcx+rax], dl
            inc         rax
            dec         r8
            jnz         A000
            mov         rax, rcx
            ret
END
