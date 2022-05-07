	.text
	.intel_syntax noprefix
	.file   "../samples/simple/simple.uir"
	.globl	simple
	.p2align	4, 0x90
	.type	simple,@function
simple:
.TmpLbl0: # bblock entry
	push	rbp
	mov	rbp, rsp
	sub	rsp, 8
	mov	dword ptr [rbp - 4], eax
	mov	dword ptr [rbp - 8], ebx
	mov	eax, dword ptr [rbp - 4]
	add	eax, dword ptr [rbp - 8]
	add	rsp, 8
	pop	rbp
	ret
.TmpProcEnd0:
	.size simple, .TmpProcEnd0-simple
