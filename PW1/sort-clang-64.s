	.text
	.file	"sort.c"
	.globl	sort                            # -- Begin function sort
	.p2align	4, 0x90
	.type	sort,@function
sort:                                   # @sort
	.cfi_startproc
# %bb.0:
	pushq	%rbp						
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$0, -20(%rbp)				# i = 0
.LBB0_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_3 Depth 2
	movl	-20(%rbp), %eax
	cmpl	-4(%rbp), %eax				# compare i and n
	jge	.LBB0_10
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movl	$0, -24(%rbp)				# j = 0
.LBB0_3:                                #   Parent Loop BB0_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	-24(%rbp), %eax				
	movl	-4(%rbp), %ecx
	subl	-20(%rbp), %ecx
	subl	$1, %ecx					# n - i - 1
	cmpl	%ecx, %eax					# compare j and n - i - 1
	jge	.LBB0_8
# %bb.4:                                #   in Loop: Header=BB0_3 Depth=2
	movq	-16(%rbp), %rax				# load *num
	movslq	-24(%rbp), %rcx				# load j
	movl	(%rax,%rcx,4), %eax			# load num[j]	
	movq	-16(%rbp), %rcx				# load j
	movl	-24(%rbp), %edx				# load *num
	addl	$1, %edx					
	movslq	%edx, %rdx
	cmpl	(%rcx,%rdx,4), %eax			# compare num[j+1] with num[j]
	jle	.LBB0_6
# %bb.5:                                #   in Loop: Header=BB0_3 Depth=2
	movq	-16(%rbp), %rax
	movslq	-24(%rbp), %rcx
	movl	(%rax,%rcx,4), %eax			# load num[j]
	movl	%eax, -28(%rbp)				# temp = num[j]
	movq	-16(%rbp), %rax
	movl	-24(%rbp), %ecx
	addl	$1, %ecx
	movslq	%ecx, %rcx
	movl	(%rax,%rcx,4), %edx			# load num[j+1]
	movq	-16(%rbp), %rax
	movslq	-24(%rbp), %rcx
	movl	%edx, (%rax,%rcx,4)			# num[j] = num[j+1]
	movl	-28(%rbp), %edx
	movq	-16(%rbp), %rax
	movl	-24(%rbp), %ecx
	addl	$1, %ecx
	movslq	%ecx, %rcx					
	movl	%edx, (%rax,%rcx,4)			# num[j+1] = temp
.LBB0_6:                                #   in Loop: Header=BB0_3 Depth=2
	jmp	.LBB0_7
.LBB0_7:                                #   in Loop: Header=BB0_3 Depth=2
	movl	-24(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -24(%rbp)				# j = j + 1
	jmp	.LBB0_3
.LBB0_8:                                #   in Loop: Header=BB0_1 Depth=1
	jmp	.LBB0_9
.LBB0_9:                                #   in Loop: Header=BB0_1 Depth=1
	movl	-20(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -20(%rbp)				# i = i + 1
	jmp	.LBB0_1
.LBB0_10:
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	sort, .Lfunc_end0-sort
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp	
	.cfi_def_cfa_register %rbp
	subq	$40032, %rsp                    # imm = 0x9C60
	movl	$0, -4(%rbp)
	leaq	.L.str(%rip), %rdi			# %d
	leaq	-8(%rbp), %rsi				# addr of n
	movb	$0, %al
	callq	__isoc99_scanf@PLT
	movl	$0, -40020(%rbp)
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	movl	-40020(%rbp), %eax
	cmpl	-8(%rbp), %eax				# compare i with n
	jge	.LBB1_4	
# %bb.2:                                #   in Loop: Header=BB1_1 Depth=1
	leaq	-40016(%rbp), %rsi			
	movslq	-40020(%rbp), %rax
	shlq	$2, %rax
	addq	%rax, %rsi					# addr of num[i]
	leaq	.L.str(%rip), %rdi			# %d
	movb	$0, %al
	callq	__isoc99_scanf@PLT
# %bb.3:                                #   in Loop: Header=BB1_1 Depth=1
	movl	-40020(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -40020(%rbp)			# i = i + 1
	jmp	.LBB1_1
.LBB1_4:
	movl	-8(%rbp), %edi				# n
	leaq	-40016(%rbp), %rsi			# *num
	callq	sort
	movl	$0, -40024(%rbp)
.LBB1_5:                                # =>This Inner Loop Header: Depth=1
	movl	-40024(%rbp), %eax
	cmpl	-8(%rbp), %eax
	jge	.LBB1_8
# %bb.6:                                #   in Loop: Header=BB1_5 Depth=1
	movslq	-40024(%rbp), %rax
	movl	-40016(%rbp,%rax,4), %esi	# num[i]
	leaq	.L.str.1(%rip), %rdi		# %d
	movb	$0, %al
	callq	printf@PLT
# %bb.7:                                #   in Loop: Header=BB1_5 Depth=1
	movl	-40024(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -40024(%rbp)
	jmp	.LBB1_5
.LBB1_8:
	leaq	.L.str.2(%rip), %rdi
	movb	$0, %al
	callq	printf@PLT
	xorl	%eax, %eax
	addq	$40032, %rsp                    # imm = 0x9C60
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"%d"
	.size	.L.str, 3

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	"%d "
	.size	.L.str.1, 4

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	"\n"
	.size	.L.str.2, 2

	.ident	"Ubuntu clang version 14.0.0-1ubuntu1"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym sort
	.addrsig_sym __isoc99_scanf
	.addrsig_sym printf
