	.text
	.file	"sort.c"
	.globl	sort                            # -- Begin function sort
	.p2align	4, 0x90
	.type	sort,@function
sort:                                   # @sort
	.cfi_startproc
# %bb.0:
	pushl	%ebp						# save ebp
	.cfi_def_cfa_offset 8
	.cfi_offset %ebp, -8		
	movl	%esp, %ebp					# save esp to ebp
	.cfi_def_cfa_register %ebp
	subl	$12, %esp					# alloc 12 bytes in the stack
	movl	12(%ebp), %eax				# get parameter int *num
	movl	8(%ebp), %eax				# get parameter int n
	movl	$0, -4(%ebp)				# i = 0
.LBB0_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_3 Depth 2
	movl	-4(%ebp), %eax				# load i to eax
	cmpl	8(%ebp), %eax				# compare i and n
	jge	.LBB0_10						# if i >= n goto .LBB010
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movl	$0, -8(%ebp)				# j = 0
.LBB0_3:                                #   Parent Loop BB0_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	-8(%ebp), %eax				# load j to eax
	movl	8(%ebp), %ecx				# load n to ecx
	subl	-4(%ebp), %ecx				# ecx = n - i
	subl	$1, %ecx					# ecx = n - i - 1
	cmpl	%ecx, %eax					# compare n - i - 1 with j
	jge	.LBB0_8							# if j >= n - i - 1 goto .LBB0_8
# %bb.4:                                #   in Loop: Header=BB0_3 Depth=2
	movl	12(%ebp), %eax				# load *num to eax
	movl	-8(%ebp), %ecx				# load j to ecx
	movl	(%eax,%ecx,4), %eax			# load num[j] to eax
	movl	12(%ebp), %ecx				# load *num to ecx
	movl	-8(%ebp), %edx				# load j to edx
	cmpl	4(%ecx,%edx,4), %eax		# compare num[j+1] with num[j]
	jle	.LBB0_6							# if num[j] <= num[j+1] goto .LBB0_6
# %bb.5:                                #   in Loop: Header=BB0_3 Depth=2
	movl	12(%ebp), %eax				# load *num to eax
	movl	-8(%ebp), %ecx				# load j to ecx
	movl	(%eax,%ecx,4), %eax			# load num[j] to eax
	movl	%eax, -12(%ebp)				# temp = num[j]
	movl	12(%ebp), %eax				# load *num to eax
	movl	-8(%ebp), %ecx				# load j to ecx
	movl	4(%eax,%ecx,4), %edx		# load num[j+1] to edx
	movl	12(%ebp), %eax				# load *num to eax
	movl	-8(%ebp), %ecx				# load j to ecx
	movl	%edx, (%eax,%ecx,4)			# num[j] = num[j+1]
	movl	-12(%ebp), %edx				# load temp to edx
	movl	12(%ebp), %eax				# load *num to eax
	movl	-8(%ebp), %ecx				# load j to ecx
	movl	%edx, 4(%eax,%ecx,4)		# num[j+1] = temp
.LBB0_6:                                #   in Loop: Header=BB0_3 Depth=2
	jmp	.LBB0_7							# goto .LBB0_7
.LBB0_7:                                #   in Loop: Header=BB0_3 Depth=2
	movl	-8(%ebp), %eax				# load j to eax
	addl	$1, %eax					# j = j + 1
	movl	%eax, -8(%ebp)				# save j
	jmp	.LBB0_3							# end of inside loop
.LBB0_8:                                #   in Loop: Header=BB0_1 Depth=1
	jmp	.LBB0_9							# goto .LBB0_9
.LBB0_9:                                #   in Loop: Header=BB0_1 Depth=1
	movl	-4(%ebp), %eax				# load i to eax
	addl	$1, %eax					# i = i + 1
	movl	%eax, -4(%ebp)				# save i
	jmp	.LBB0_1							# end of outside loop
.LBB0_10:
	addl	$12, %esp					# free stack space
	popl	%ebp						# restore bep
	.cfi_def_cfa %esp, 4
	retl								# return
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
	pushl	%ebp							# save ebp
	.cfi_def_cfa_offset 8
	.cfi_offset %ebp, -8
	movl	%esp, %ebp						# save esp to ebp
	.cfi_def_cfa_register %ebp				
	pushl	%ebx							# save ebx
	subl	$40036, %esp                    # allocate 40036 byte in stack
	.cfi_offset %ebx, -12
	calll	.L1$pb
.L1$pb:
	popl	%ebx							# restore ebx
.Ltmp0:
	addl	$_GLOBAL_OFFSET_TABLE_+(.Ltmp0-.L1$pb), %ebx
	movl	%ebx, -40024(%ebp)              # 4-byte Spill
	movl	$0, -8(%ebp)					# n = 0
	leal	.L.str@GOTOFF(%ebx), %ecx		# get "%d" 
	leal	-12(%ebp), %eax					# get addr of n to eax
	movl	%ecx, (%esp)					# save "%d" for scanf
	movl	%eax, 4(%esp)					# save addr of n for scanf
	calll	__isoc99_scanf@PLT				# call scanf
	movl	$0, -40016(%ebp)				# i = 0
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	movl	-40016(%ebp), %eax			# load i to eax
	cmpl	-12(%ebp), %eax				# compare n with i
	jge	.LBB1_4							# if i >= n stop loop
# %bb.2:                                #   in Loop: Header=BB1_1 Depth=1
	movl	-40024(%ebp), %ebx              # 4-byte Reload
	leal	-40012(%ebp), %eax			# load addr of num to eax
	movl	-40016(%ebp), %ecx			# load i to ecx
	shll	$2, %ecx					# 4byte offset, mul 4
	addl	%ecx, %eax					# get addr of num[i] 
	leal	.L.str@GOTOFF(%ebx), %ecx	# load "%d" to ecx
	movl	%ecx, (%esp)				# save "%d" to stack, for scanf
	movl	%eax, 4(%esp)				# save addr of num[i], for scanf
	calll	__isoc99_scanf@PLT			# call scanf
# %bb.3:                                #   in Loop: Header=BB1_1 Depth=1
	movl	-40016(%ebp), %eax			# load i to eax
	addl	$1, %eax					# i = i + 1
	movl	%eax, -40016(%ebp)			# save i
	jmp	.LBB1_1							# end of input loop
.LBB1_4:
	movl	-40024(%ebp), %ebx              # 4-byte Reload
	movl	-12(%ebp), %ecx				# load n to ecx
	leal	-40012(%ebp), %eax			# load addr of num to eax
	movl	%ecx, (%esp)				# save n for sort
	movl	%eax, 4(%esp)				# save *num for sort
	calll	sort						# call sort
	movl	$0, -40020(%ebp)			# i = 0
.LBB1_5:                                # =>This Inner Loop Header: Depth=1
	movl	-40020(%ebp), %eax			# load i to eax
	cmpl	-12(%ebp), %eax				# conpare n with i
	jge	.LBB1_8							# if i >= n stop loop
# %bb.6:                                #   in Loop: Header=BB1_5 Depth=1
	movl	-40024(%ebp), %ebx              # 4-byte Reload
	movl	-40020(%ebp), %eax			# load i to eax
	movl	-40012(%ebp,%eax,4), %eax	# load num[i] to eax
	leal	.L.str.1@GOTOFF(%ebx), %ecx # load "%d" to ecx
	movl	%ecx, (%esp)				# save "%d" for printf
	movl	%eax, 4(%esp)				# save num[i] for printf
	calll	printf@PLT					# call printf
# %bb.7:                                #   in Loop: Header=BB1_5 Depth=1
	movl	-40020(%ebp), %eax			# load i to eax
	addl	$1, %eax					# i = i + 1
	movl	%eax, -40020(%ebp)			# save i
	jmp	.LBB1_5							# end of output loop
.LBB1_8:
	movl	-40024(%ebp), %ebx              # 4-byte Reload
	leal	.L.str.2@GOTOFF(%ebx), %eax		# load "\n" to eax
	movl	%eax, (%esp)					# save "\n" for printf
	calll	printf@PLT						# call printf
	xorl	%eax, %eax						# return 0
	addl	$40036, %esp                    # imm = 0x9C64
	popl	%ebx							# restore ebx
	popl	%ebp							# restore ebp
	.cfi_def_cfa %esp, 4
	retl
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
