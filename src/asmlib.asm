PUBLIC memset
PUBLIC sqrtf

.CODE

; Very simple and slow implementation of memset.
; This is needed by the CL compiler if libc is not used (/NODEFAULTLIB).
; We do it in ASM because C solutions don't work with whole-program optimizations (/GL, /LTCG).
; TODO: Optimize if speed becomes a problem.
memset: xor     eax, eax
L0:     mov     [rcx+rax], dl
        inc     rax
        dec     r8
        jnz     L0
        mov     rax, rcx
        ret

; TODO: Add some debug stuff if needed.
sqrtf:  sqrtss  xmm0, xmm0
        ret
END
