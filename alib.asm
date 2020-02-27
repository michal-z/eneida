format MS64 COFF

public memset
public sqrtf

macro defshader sh {
  sh: file `sh#'.cso'
  sh#_size dd $-sh
  public sh
  public sh#_size
}

section '.text' code readable executable

; Very simple and slow implementation of memset.
; TODO: Optimize if speed becomes a problem.
memset:     xor         eax, eax
.0:         mov         [rcx+rax], dl
            inc         rax
            dec         r8
            jnz         .0
            mov         rax, rcx
            ret

; TODO: Add some debug stuff if needed.
sqrtf:      sqrtss      xmm0, xmm0
            ret

section '.data' data readable writeable

;defshader vs_e01_transform
;defshader ps_e01_shade
