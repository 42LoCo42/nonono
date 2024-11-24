// vim: expandtab:
.intel_syntax noprefix

.global copyBignum
.global add_bignum
.global mult
.global newBignum
.global freeBignum

/*
Signatur: mult(bignum *x,bignum *y,bignum *output)
rdi: bignum *x
rsi: bignum *y

Pointers assigned +
Stack alignment +
*/
mult:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15
    add rsp,8

    mov rbp,rdi /* a */
    mov rbx,rsi /* b */

    cmp qword ptr [rbp+20],1
    jne mult_skipIf
    cmp qword ptr [rbx+20],1
    jne mult_skipIf

    mult_if:
        // get first digits of a & b
        mov rax, [rbp+12]
        mov  al, [rax]
        mov rcx, [rbx+12]
        mov  cl, [rcx]
        mul cl

        // divide by 10
        mov cl, 10
        div cl
        push rax

        // create new bignum
        mov rdi, 2
        call newBignum

        // store digits
        pop rcx
        mov rdx, [rax+12]
        mov byte ptr [rdx], ch
        mov byte ptr [rdx+1], cl

        // set sign
        mov edx, [rbp]
        cmp edx, [rbx]
        je mult_one_setplus
        mov dword ptr [rax], -1
        jmp mult_end
        mult_one_setplus:
        mov dword ptr [rax], 1
        jmp mult_end
    mult_skipIf:

    mov rdi, rbp
    mov rsi, rbx
    call matchSizes

    mov r8, [rbp+20]
    shr r8 /* r8 = m (half size) */

    mov rdi, rbp
    mov rsi, r8
    call split
    push rcx

    mov rdi, rax
    call freeBignum

    pop rdi
    call freeBignum

    // mov rdi, rbx
    // mov rsi, r8
    // call split
    // mov r10, rax /* r10 = bp */

    // free bignum pairs
    // mov rdi, r9
    // call free
    // mov rdi, r10
    // call free

    mov rax,0 /* return result */
    mult_end:

    sub rsp,8
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
/*
AddBignum(bignum *a,bignum *b)
uses: rdi,rsi as I/O
      rax,rcx
Pointers assigned +
Stack alignment +
*/
add_bignum:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    mov rbp,rdi
    mov rbx,rsi

    push rax
    call matchSizes
    pop rax

    if_a_not_b:
        mov eax, [rbp]
        cmp eax, [rbx]
        je endif
        switch:
            mov rdi, rbp
            mov rsi, rbx
            call compareAbsBignum
            cmp rax,0
            jl LESSTHAN
            jg GREATERTHAN
            EQUAL:
                mov eax, [rbp]
                cmp eax, 1
                je endif
            LESSTHAN:
                mov rdi, rbx
                mov rsi, rbp
                call add_bignum
                jmp end_addBignum
            GREATERTHAN:
                mov eax, dword ptr [rbp]
                cmp eax, -1
                jne endif

                imul eax,-1
                mov dword ptr [rbp],eax

                mov eax, dword ptr [rbx]
                imul eax,-1
                mov dword ptr [rbx],eax

                mov rdi, rbp
                mov rsi, rbx
                call add_bignum

                mov ecx, dword ptr [rbp]
                imul ecx,-1
                mov dword ptr [rbp],ecx

                mov ecx, dword ptr [rbx]
                imul ecx,-1
                mov dword ptr [rbx],ecx

                mov ecx, dword ptr [rax]
                imul ecx,-1
                mov dword ptr [rax],ecx
                jmp end_addBignum
    endif:
    mov rdi,rbp
    call copyBignum
    mov r12,rax /* result */

    mov rdi,rbp
    call bignumLen
    mov rsi,rax
    inc rsi
    mov rdi,r12
    call resizeBignum

    xor r13b,r13b /* carry */
    mov r8, 0
    forV:
        // get B digit
        mov rsi, [rbx+12]
        mov dl, [rsi + r8]

        // multiply by signs
        imul dx, word ptr [rbp]
        imul dx, word ptr [rbx]

        // add A digit
        mov rsi, [rbp+12]
        add dl, [rsi + r8]

        // add carry
        add dl, r13b

        // get result digit ptr
        mov rsi, [r12+12]
        lea rdi, [rsi + r8]

        // store result
        mov byte ptr [rdi], dl

        // if result < 0:
        cmp byte ptr [rdi], 0
        jl add_borrow

            // result >= 0
            mov al, byte ptr [rdi]
            mov ah, 0
            mov r10b, 10
            div r10b
            mov r13b, al
            mov byte ptr [rdi], ah
            jmp add_carrycheck_end

        add_borrow:
            // result < 0
            mov r13b, -1
            add byte ptr [rdi], 10

        add_carrycheck_end:
        inc r8
        cmp r8, [rbp+20]
        jl forV

    mov rdi,rbp
    call bignumLen
    mov rsi,rax

    lea rdi,[r12+12]
    call d
    mov byte ptr [rax], r13b

    mov rdi,rbp
    call shrink
    mov rdi,rbx
    call shrink
    mov rdi,r12
    call shrink

    mov rax, r12
    end_addBignum:
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

/*
matchSizes(bignum *a,bignum *b)
uses: rdi,rsi for I/O
      rax,rcx
Pointers assigned +
Stack alignment +
*/
matchSizes:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    mov rbp,rdi
    mov rbx,rsi
    matchLSP:
        call bignumLSP
        mov r12,rax /* bignumLSP(a) */

        mov rdi,rbx
        call bignumLSP
        mov r13,rax /* bignumLSP(b) */

        mov rcx,r12
        sub rcx,rax
        cmp rcx,0
        jl deltaLSP_elseif
        je matchMSP

        mov rsi,r12
        mov rdi,rbx
        call extendLSP
        jmp matchMSP

        deltaLSP_elseif:
        mov rdi,rbp
        mov rsi,r13
        call extendLSP

    matchMSP:

        mov rdi,rbp
        call bignumMSP
        mov r12,rax /* bignumMSP(a) */

        mov rdi,rbx
        call bignumMSP
        mov r13,rax /* bignumMSP(b) */

        mov rcx,r12
        sub rcx,rax
        cmp rcx,0
        jl deltaMSP_elseif
        je end_matchSizes

        mov rdi,rbx
        mov rsi,r12
        call extendMSP
        jmp end_matchSizes

        deltaMSP_elseif:
        mov rdi,rbp
        mov rsi,r13
        call extendMSP

    end_matchSizes:
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
/*
extendLSP(bignum *b,size_t size)
uses: rdi,rsi,rdx for I/O
      rax,rcx
Pointers assigned +
Stack alignment +
*/
extendLSP:
    push rbp
    push rbx
    push r12
    mov rbp,rdi /* b */
    mov rbx,rsi /* size */

    call bignumLSP
    mov rdi,rbx
    sub rdi,rax /* delta */

    cmp rdi,0
    jle end_extendLSP

    mov rsi,rdi
    mov rdi,rbp
    call shlBignum

    mov [rbp+4],rbx

    end_extendLSP:
    pop r12
    pop rbx
    pop rbp
    ret

/*
extendMSP(bignum *b,size_t size)
uses: rdi,rsi for I/O
      rax
Pointers assigned +
Stack alignment +
*/
extendMSP:
    push rbp
    push rbx
    push r12
    mov rbp,rdi /* b */
    mov rbx,rsi /* size */

    call bignumLSP

    mov rsi,rbx
    add rsi,rax
    mov rdi,rbp
    call resizeBignum

    pop r12
    pop rbx
    pop rbp
    ret
/*
bignumLSP(bignum *b)
uses: rdi for I/O
      rax,rcx
Pointers assigned +
Stack alignment +
*/
bignumLSP:
    mov rax,[rdi+4]
    ret
/*
bignumLen(bignum *b)
uses: rdi for I/O
      rax,rcx
Pointers assigned +
Stack alignment +
*/
bignumLen:
    mov rax,[rdi+20]
    ret
/*
bignumMSP(bignum *b)
uses: rdi for I/O
      rax,rcx
Pointers assigned +
Stack alignment +
*/
bignumMSP:
    push rdi
    call bignumLSP
    pop rdi
    push rax
    call bignumLen
    pop rcx
    sub rax,rcx
    ret
/*
resizeBignum(bignum *b, size_t size)
uses: rdi,rsi,rdx for I/O
      rax,
Pointers assigned +
Stack alignment +
*/
resizeBignum:
    push rbp
    push rbx
    push r12
    mov rbp,rdi /* b */
    mov rbx,rsi /* size */

    call bignumLen
    mov r12,rax /* old */

    lea rdi,[rbp + 12]
    mov rsi,rbx
    call resizeV

    mov rdi,rbp
    call bignumLen

    cmp rax,r12
    jng end_resizeBignum

    mov rdi,[rdi+12]
    add rdi,r12

    mov rsi,0

    mov rdx,rbx
    sub rdx,r12

    push rax
    call memset
    pop rax
    end_resizeBignum:
    pop r12
    pop rbx
    pop rbp
    ret

/*
resizeV(vector *v, size_t size)
uses: rdi,rsi for I/O
Pointers assigned +
Stack alignment +
*/
resizeV:
    push rax
    push rsi
    push rdi

    mov rdi,[rdi]
    call realloc

    pop rdi
    mov [rdi], rax

    pop rsi
    mov [rdi+8],rsi
    pop rax
    ret
/*
ord compareAbsBignum(bignum* a, bignum* b)
uses: rdi,rsi for I/O
      rcx
Pointers assigned +
Stack alignment +
*/
compareAbsBignum:
    push rbp
    push rbx
    push r12
    push r13
    push rax
    mov rbp,rdi /* a */
    mov rbx,rsi /* b */

    call bignumLen
    mov r12, rax
    compareAbsBignum_ForLoop:
        dec r12
        lea rdi,[rbx+12]
        mov rsi,r12
        call d
        mov r13,rax /* int8_t* */

        lea rdi,[rbp+12]
        mov rsi,r12
        call d

        mov cl,[r13]
        mov sil,[rax]
        sub sil,cl /* delta */

        cmp sil,0
        jl delta_lt_z
        jg delta_gt_z
        cmp r12,0
        jne compareAbsBignum_ForLoop
        xor rdx,rdx /* ret EQ */
        jmp compareAbsBignum_ForLoop_end
        delta_lt_z:
            mov rdx,-1
            jmp compareAbsBignum_ForLoop_end
        delta_gt_z:
            mov rdx,1
            jmp compareAbsBignum_ForLoop_end


    compareAbsBignum_ForLoop_end:
    pop rax
    mov rax, rdx
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
/*
int8_t* d(vector* v, size_t pos)
uses: rdi,rsi for I/O
      rax
Pointers assigned +
Stack alignment +
*/
d:
    cmp rsi,[rdi+8]
    jb d_retData
    xor eax,eax
    ret
    d_retData:
    mov rax,[rdi]
    add rax,rsi
    ret
/*
bignum* copyBignum(bignum* b)
uses: rdi,rsi,rdx for I/O
      rax,rcx,r8
Pointers assigned +
Stack alignment +
*/
copyBignum:
    push rbp
    push rbx
    push r12
    mov rbp,rdi /* b */

    call bignumLen

    mov rdi,rax
    mov rbx,rdi /* bignumLen(b) */

    call newBignum

    mov ecx,[rbp]
    mov [rax],ecx

    mov rcx,[rbp+4]
    mov [rax+4], rcx

    push rax
    mov rdi, [rax+12]
    mov rsi, [rbp+12]
    mov rdx, rbx
    call memmove

    pop rax
    pop r12
    pop rbx
    pop rbp
    ret

/*
void shrink(bignum *b)
uses: rdi,rsi,rdx for I/O
      rax
Pointers assigned +
Stack alignment +
*/
shrink:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15
    add rsp,8
    mov rbp,rdi /* b */

    call bignumLen

    mov rbx,rax /* old */
    xor r12,r12 /* LSPZERO */
    xor r13,r13 /* MSPZERO */

    xor r15,r15 /* i */
    mov rdi,rbp
    call bignumLSP
    mov r14,rax /* bignumLSP */
    jmp shrink_firstForLoop_Cond
    shrink_firstForLoop:
        inc r15

        lea rdi,[rbp+12]
        mov rsi,r15
        call d
        cmp byte ptr [rax],0
        jne end_shrink_firstForLoop
        inc r12
        shrink_firstForLoop_Cond:
            cmp r15,r14
            jl shrink_firstForLoop
    end_shrink_firstForLoop:

    mov r15,rbx
    dec r15
    jmp shrink_secondForLoop_Cond
    shrink_secondForLoop:
        lea rdi,[rbp+12]
        mov rsi,r15
        call d
        cmp byte ptr [rax],0
        jne end_shrink_secondForLoop
        inc r13

        dec r15
        shrink_secondForLoop_Cond:
        mov rdi,[rbp+4]
        cmp r15,rdi
        jg shrink_secondForLoop
    end_shrink_secondForLoop:

    mov r14,rbx /* new */
    add r13,r12
    sub r14,r13

    cmp r14,0
    jne shrink_else

    mov rdi,rbp
    mov rsi,1
    call resizeBignum

    lea rdi,[rbp+12]
    mov rsi,0
    call d

    mov byte ptr [rax], 0

    mov qword ptr [rbp+4],0

    jmp end_shrink
    shrink_else:

    mov rdi, [rbp+12]
    mov rsi, [rbp+12]
    add rsi, r12
    mov rdx, r14
    call memmove

    mov rdi,rbp
    mov rsi,r14
    call resizeBignum

    sub [rbp+4],r12

    end_shrink:
    sub rsp,8
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

/*
bignum* newBignum (size_t size)
uses: rdi,esi,rdx for I/O
      rax
Pointers assigned +
Stack alignment +
*/
newBignum:
    push rbp
    push rbx
    push r12
    mov rbp,rdi /* size */

    mov rdi,28
    call malloc

    mov rbx,rax /* result */

    mov rdi,rax
    xor esi,esi
    mov rdx,28
    call memset

    mov rdi,rbx
    mov rsi,rbp
    call resizeBignum


    mov rdi,[rbx+12]
    xor esi,esi
    mov rdx,[rbx+20]
    call memset

    mov dword ptr [rbx],1

    mov qword ptr [rbx+4],0
    mov rax,rbx

    pop r12
    pop rbx
    pop rbp
    ret

/*
int abs(int a)
uses: rdi for I/O
      rax
Pointers assigned +
Stack alignment +
*/
abs:
    cmp rdi,0
    jg abs_ret
    imul rdi,-1
    abs_ret:
    mov rax,rdi
    ret

/*
bignumPair split(bignum* b, size_t loLen)
uses: rdi,rsi,rbx for I/O
Pointers assigned +
Stack alignment +
*/
split:
    // WARNING: NEW IMPLEMENTATION
    // hi, lo are returned in rax, rcx
    push rbp
    push rbx
    mov rbp,rdi /* b */
    mov rbx,rsi /* loLen */

    mov r8, [rbp+20]
    sub r8, rbx /* r8 = len - loLen */
    cmp r8, 0
    jg split_real

    // fake split: copy b to lo, set hi to 0
    mov rdi, rbp
    call copyBignum
    push rax

    mov rdi, 1
    call newBignum

    pop rcx
    jmp split_end

    split_real:
    // create hi
    mov rdi, r8
    call newBignum
    mov r9, rax /* r9 is hi */

    // copy data to hi
    mov rdi, [r9+12]
    mov rsi, [rbp+12]
    add rsi, rbx
    mov rdx, [r9+20]
    call memmove

    // shrink hi
    mov rdi, r9
    call shrink
    push r9

    // create lo
    mov rdi, rbx
    call newBignum
    mov r10, rax /* r10 is lo */

    // copy data to lo
    mov rdi, [r10+12]
    mov rsi, [rbp+12]
    mov rdx, [r10+20]
    call memmove

    // shrink lo
    mov rdi, r10
    call shrink

    // set results
    pop rax
    mov rcx, r10

    split_end:
    pop rbx
    pop rbp
    ret
/*
bignum* absorbA(bignum_2f func, bignum* a, bignum* b)
uses: rdi,rsi,rdx for I/O
      rax
Pointers assigned +
Stack alignment +
*/
absorbA:
    push rbp
    mov rbp,rsi

    mov rax,rdi
    mov rdi,rsi
    mov rsi,rdx
    call [rax]

    mov rdi,rbp
    mov rbp,rax
    call freeBignum

    mov rax,rbp
    pop rbp
    ret
/*
bignum* absorb2(bignum_2f func, bignum* a, bignum* b)
uses: rdi,rsi,rdx for I/O
      rax
Pointers assigned +
Stack alignment +
*/
absorb2:
    push rdx

    call absorbA
    pop rdi
    push rax
    call freeBignum
    pop rax
    ret

/*
bignum* shlBignum(bignum* b, size_t s)
uses: rdi,rsi,rdx for I/O
      rax
Pointers assigned +
Stack alignment +
*/
shlBignum:
    push rbp
    push rbx
    push r12
    mov rbp,rdi /* b */
    mov rbx,rsi /* s */

    call bignumLen
    mov r12,rax /* old */

    mov rdi,rbp
    mov rsi,r12
    sub rsi,rbx
    call resizeBignum

    mov rdi,[rbp+12]
    mov rsi,rdi
    add rdi,rbx

    mov rdx,r12

    call memmove

    mov rdi,[rbp+12]

    xor esi,esi
    mov rdx,rbx
    call memset

    mov rax,rbp
    pop r12
    pop rbx
    pop rbp
    ret
/*
void setPoint(bignum* b, size_t point)
uses: rdi,rsi for I/O
Pointers assigned +
Stack alignment +
*/
setPoint:
    push rbp
    push rbx
    push rax
    mov rbp,rdi
    mov rbx,rsi

    call bignumLen
    cmp rbx,rax
    jnge setPoint_endIf
    mov rdi,rbp
    mov rsi, rbx
    inc rsi
    call extendMSP

    setPoint_endIf:
    mov [rbp+4],rbx
    pop rax
    pop rbx
    pop rbp
    ret
/*
void freeBignum (bignum* b)
uses: rdi for I/O
      rax
Pointers assigned +
Stack alignment +
*/
freeBignum:
    push rbp
    mov rbp,rdi

    add rdi,12
    call freeV

    mov rdi,rbp
    call free

    pop rbp
    ret
/*
void freeV(vector* v)
uses: rdi for I/O
      rax
Pointers assigned +
Stack alignment +
*/
freeV:
    push rdi
    mov rdi,[rdi]
    call free
    pop rdi
    mov rdi,0
    ret
/*
int digitCount(int n)
uses: rdi for I/O
      rax
Pointers assigned +
Stack alignment +
*/
digitCount:
    push rbp
    call abs
    pop rbp
    cmp eax,10
    jl L1
    cmp eax,100
    jl L2
    cmp eax,1000
    jl L3
    cmp eax,10000
    jl L4
    cmp eax,100000
    jl L5
    cmp eax,1000000
    jl L6
    cmp eax,10000000
    jl L7
    cmp eax,100000000
    jl L8
    cmp eax,1000000000
    jl L9
    mov eax,10
    ret
    L1:
        mov eax,1
        ret
    L2:
        mov eax,2
        ret
    L3:
        mov eax,3
        ret
    L4:
        mov eax,4
        ret
    L5:
        mov eax,5
        ret
    L6:
        mov eax,6
        ret
    L7:
        mov eax,7
        ret
    L8:
        mov eax,8
        ret
    L9:
        mov eax,9
        ret
