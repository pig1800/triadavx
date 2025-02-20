;==============================================================================
; File: triad.asm
;
;  MASM x64 example template for a function:
;    void triad(void* a, void* b, void* c, size_t size);
;
;  Windows x64 calling convention:
;   RCX -> a
;   RDX -> b
;   R8  -> c
;   R9  -> size
;
;  Return:
;    void (no return value)
;
;  Build (MASM 64-bit):
;    ml64 /c triad.asm
;    link /dll triad.obj  (for a DLL) or link triad.obj (for an EXE stub)
;==============================================================================

;------------------------------------------------------------------------------
; Tell MASM that we're producing 64-bit code using the Microsoft x64 model
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Export the symbol "triad" so that C code can link against it.
;------------------------------------------------------------------------------
PUBLIC triad

.data
ALIGN 16
scalar REAL8 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0

.code

;------------------------------------------------------------------------------
; triad PROC
;   RCX = pointer a
;   RDX = pointer b
;   R8  = pointer c
;   R9  = size_t size
;------------------------------------------------------------------------------
triad PROC
    push    rbp
    mov     rbp, rsp
    push    rsi
    sub     rsp, 32             ; for local space, if needed
    mov     rsi, rsp
    mov     [rsp], RCX
    mov     [rsp + 8], RDX
    mov     [rsp + 16], R8
    mov     [rsp + 24], R9

    sub     rsp, 0a0h
    movdqu  [rsp + 90h], xmm6
    movdqu  [rsp + 80h], xmm7
    movdqu  [rsp + 70h], xmm8
    movdqu  [rsp + 60h], xmm9
    movdqu  [rsp + 50h], xmm10
    movdqu  [rsp + 40h], xmm11
    movdqu  [rsp + 30h], xmm12
    movdqu  [rsp + 20h], xmm13
    movdqu  [rsp + 10h], xmm14
    movdqu  [rsp], xmm15
    
    mov     rax, rdx            ; b
    mov     rdx, rcx            ; a
    sub     rdx, rax
    mov     rcx, r8             ; c
    sub     rcx, rax            
    
    ;----------------------------------------------------------------------
    ; From now
    ; RAX = Pointer b
    ; RDX = Pointer a diff
    ; RCX = Pointer c diff

    shr         r9, 8               ; 256 bytes per loop
    vmovapd     ymm8, ymmword ptr [scalar]     ; read scalar vector

ALIGN 16
    loop_start:
    vmovapd     ymm0, ymmword ptr [rax]
    vmovapd     ymm1, ymmword ptr [rax + 20h]
    vmovapd     ymm2, ymmword ptr [rax + 40h]
    vmovapd     ymm3, ymmword ptr [rax + 60h]
    vmovapd     ymm4, ymmword ptr [rax + 80h]
    vmovapd     ymm5, ymmword ptr [rax + 0a0h]
    vmovapd     ymm6, ymmword ptr [rax + 0c0h]
    vmovapd     ymm7, ymmword ptr [rax + 0e0h]
    vfmadd231pd ymm0, ymm8, ymmword ptr[rax + rcx]
    vfmadd231pd ymm1, ymm8, ymmword ptr[rax + rcx + 20h]
    vfmadd231pd ymm2, ymm8, ymmword ptr[rax + rcx + 40h]
    vfmadd231pd ymm3, ymm8, ymmword ptr[rax + rcx + 60h]
    vfmadd231pd ymm4, ymm8, ymmword ptr[rax + rcx + 80h]
    vfmadd231pd ymm5, ymm8, ymmword ptr[rax + rcx + 0a0h]
    vfmadd231pd ymm6, ymm8, ymmword ptr[rax + rcx + 0c0h]
    vfmadd231pd ymm7, ymm8, ymmword ptr[rax + rcx + 0e0h]
    vmovapd     ymmword ptr [rax + rdx], ymm0
    vmovapd     ymmword ptr [rax + rdx + 20h], ymm1
    vmovapd     ymmword ptr [rax + rdx + 40h], ymm2
    vmovapd     ymmword ptr [rax + rdx + 60h], ymm3
    vmovapd     ymmword ptr [rax + rdx + 80h], ymm4
    vmovapd     ymmword ptr [rax + rdx + 0a0h], ymm5
    vmovapd     ymmword ptr [rax + rdx + 0c0h], ymm6
    vmovapd     ymmword ptr [rax + rdx + 0e0h], ymm7

    lea         rax, [rax + 100h]
    dec         r9
    jne         loop_start
    vzeroupper

    ;----------------------------------------------------------------------
    ; Cleanup and return:
    movdqu  xmm15, [rsp]
    movdqu  xmm14, [rsp + 10h]
    movdqu  xmm13, [rsp + 20h]
    movdqu  xmm12, [rsp + 30h]
    movdqu  xmm11, [rsp + 40h]
    movdqu  xmm10, [rsp + 50h]
    movdqu  xmm9, [rsp + 60h]
    movdqu  xmm8, [rsp + 70h]
    movdqu  xmm7, [rsp + 80h]
    movdqu  xmm6, [rsp + 90h]
    add     rsp, 0a0h
    add     rsp, 32
    pop     rsi
    pop     rbp
    ret

triad ENDP

END
