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
memset: XOR eax, eax
.0:     MOV [rcx+rax], dl
        INC rax
        DEC r8
        JNZ .0
        MOV rax, rcx
        RET

; TODO: Add some debug stuff if needed.
sqrtf:  SQRTSS xmm0, xmm0
        ret

section '.data' data readable writeable

defshader vs_triangle
defshader ps_triangle
