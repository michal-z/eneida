PUBLIC memset

.CODE

; Very simple and slow implementation of memset.
; This is needed by the CL compiler if libc is not used (/NODEFAULTLIB).
; TODO: Optimize if speed becomes a problem.
memset: xor     eax, eax
L0:     mov     [rcx+rax], dl
        inc     rax
        dec     r8
        jnz     L0
        mov     rax, rcx
        ret
END
