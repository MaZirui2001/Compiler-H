	.file	"sort.c"
	.text
	.globl	sort
	.type	sort, @function
sort:
.LFB0:
	.cfi_startproc
	pushl	%ebp								# save ebp
	.cfi_def_cfa_offset 8			
	.cfi_offset 5, -8		
	movl	%esp, %ebp							# save esp to ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp							# allocate 16 bytes in the stack
	call	__x86.get_pc_thunk.ax
	addl	$_GLOBAL_OFFSET_TABLE_, %eax
	movl	$0, -12(%ebp)						# i = 0
	jmp	.L2	
.L6:	
	movl	$0, -8(%ebp)						# j = 0
	jmp	.L3
.L5:											# compare and swap num[j] and num[j+1]
	movl	-8(%ebp), %eax						
	leal	0(,%eax,4), %edx
	movl	12(%ebp), %eax
	addl	%edx, %eax
	movl	(%eax), %edx						# load num[j]
	movl	-8(%ebp), %eax
	addl	$1, %eax
	leal	0(,%eax,4), %ecx
	movl	12(%ebp), %eax
	addl	%ecx, %eax
	movl	(%eax), %eax						# load num[j+1]
	cmpl	%eax, %edx							# compare num[j] and num[j+1]
	jle	.L4
	movl	-8(%ebp), %eax						# load j to eax
	leal	0(,%eax,4), %edx					# load offset to edx
	movl	12(%ebp), %eax						# load *num to eax
	addl	%edx, %eax							# calculate addr of num[j]
	movl	(%eax), %eax						# load num[j] to eax
	movl	%eax, -4(%ebp)						# temp =  num[j]
	movl	-8(%ebp), %eax						# load j to eax
	addl	$1, %eax							# j + 1
	leal	0(,%eax,4), %edx					# load offset to edx
	movl	12(%ebp), %eax						# load *num to eax
	addl	%edx, %eax							# calculate addr of num[j+1]
	movl	-8(%ebp), %edx						# load j to edx
	leal	0(,%edx,4), %ecx					# load offset to ecx
	movl	12(%ebp), %edx						# load *num to edx
	addl	%ecx, %edx							# calculate addr of num[j]
	movl	(%eax), %eax						# load num[j+1] to eax
	movl	%eax, (%edx)						# num[j] = num[j+1]
	movl	-8(%ebp), %eax						# load j to eax
	addl	$1, %eax							# j + 1
	leal	0(,%eax,4), %edx					# load offset to edx
	movl	12(%ebp), %eax						# load *num to eax
	addl	%eax, %edx							# calculate addr of num[j+1]
	movl	-4(%ebp), %eax						# load temp to eax
	movl	%eax, (%edx)						# num[j+1] = temp
.L4:							
	addl	$1, -8(%ebp)						# j = j + 1
.L3:											# calculate n - i - 1 and compare with j
	movl	8(%ebp), %eax
	subl	-12(%ebp), %eax
	subl	$1, %eax							# n - i - 1
	cmpl	%eax, -8(%ebp)
	jl	.L5
	addl	$1, -12(%ebp)
.L2:											# compare i with n
	movl	-12(%ebp), %eax
	cmpl	8(%ebp), %eax
	jl	.L6
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	sort, .-sort
	.section	.rodata
.LC0:
	.string	"%d"
.LC1:
	.string	"%d "
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	leal	4(%esp), %ecx
	.cfi_def_cfa 1, 0
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	.cfi_escape 0x10,0x5,0x2,0x75,0
	pushl	%ebx
	pushl	%ecx
	.cfi_escape 0xf,0x3,0x75,0x78,0x6
	.cfi_escape 0x10,0x3,0x2,0x75,0x7c
	leal	-36864(%esp), %eax
.LPSRL0:									# scanf n
	subl	$4096, %esp
	orl	$0, (%esp)
	cmpl	%eax, %esp
	jne	.LPSRL0
	subl	$3152, %esp
	call	__x86.get_pc_thunk.bx
	addl	$_GLOBAL_OFFSET_TABLE_, %ebx
	movl	%gs:20, %eax
	movl	%eax, -12(%ebp)
	xorl	%eax, %eax
	subl	$8, %esp
	leal	-40024(%ebp), %eax
	pushl	%eax							# save &n for scanf
	leal	.LC0@GOTOFF(%ebx), %eax			
	pushl	%eax							# save %d for scanf
	call	__isoc99_scanf@PLT
	addl	$16, %esp
	movl	$0, -40020(%ebp)
	jmp	.L9
.L10:										# scanf num
	movl	-40020(%ebp), %eax
	leal	0(,%eax,4), %edx
	leal	-40012(%ebp), %eax
	addl	%edx, %eax
	subl	$8, %esp
	pushl	%eax							# save num + i for scanf
	leal	.LC0@GOTOFF(%ebx), %eax
	pushl	%eax							# save %d for scanf
	call	__isoc99_scanf@PLT
	addl	$16, %esp
	addl	$1, -40020(%ebp)
.L9:										# sort
	movl	-40024(%ebp), %eax	
	cmpl	%eax, -40020(%ebp)
	jl	.L10
	movl	-40024(%ebp), %eax
	subl	$8, %esp
	leal	-40012(%ebp), %edx
	pushl	%edx							# save *num for sort
	pushl	%eax							# save n for sort
	call	sort
	addl	$16, %esp
	movl	$0, -40016(%ebp)
	jmp	.L11
.L12:										# printf num
	movl	-40016(%ebp), %eax
	movl	-40012(%ebp,%eax,4), %eax
	subl	$8, %esp
	pushl	%eax							# save num[i] for printf
	leal	.LC1@GOTOFF(%ebx), %eax			
	pushl	%eax							# save %d for printf
	call	printf@PLT
	addl	$16, %esp
	addl	$1, -40016(%ebp)
.L11:										# printf \n
	movl	-40024(%ebp), %eax
	cmpl	%eax, -40016(%ebp)
	jl	.L12
	subl	$12, %esp
	pushl	$10								# save \n for putchar
	call	putchar@PLT						
	addl	$16, %esp
	movl	$0, %eax
	movl	-12(%ebp), %edx
	subl	%gs:20, %edx
	je	.L14
	call	__stack_chk_fail_local
.L14:
	leal	-8(%ebp), %esp
	popl	%ecx
	.cfi_restore 1
	.cfi_def_cfa 1, 0
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
	.cfi_restore 5
	leal	-4(%ecx), %esp
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.section	.text.__x86.get_pc_thunk.ax,"axG",@progbits,__x86.get_pc_thunk.ax,comdat
	.globl	__x86.get_pc_thunk.ax
	.hidden	__x86.get_pc_thunk.ax
	.type	__x86.get_pc_thunk.ax, @function
__x86.get_pc_thunk.ax:
.LFB2:
	.cfi_startproc
	movl	(%esp), %eax
	ret
	.cfi_endproc
.LFE2:
	.section	.text.__x86.get_pc_thunk.bx,"axG",@progbits,__x86.get_pc_thunk.bx,comdat
	.globl	__x86.get_pc_thunk.bx
	.hidden	__x86.get_pc_thunk.bx
	.type	__x86.get_pc_thunk.bx, @function
__x86.get_pc_thunk.bx:
.LFB3:
	.cfi_startproc
	movl	(%esp), %ebx
	ret
	.cfi_endproc
.LFE3:
	.hidden	__stack_chk_fail_local
	.ident	"GCC: (Ubuntu 11.2.0-19ubuntu1) 11.2.0"
	.section	.note.GNU-stack,"",@progbits
