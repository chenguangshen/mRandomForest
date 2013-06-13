	.file	"b.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"%d %d"
	.section	.text.startup,"ax",@progbits
	.p2align 2
	.p2align 4,,15
	.globl main
	.type	main, @function
main:
	// saved LR + FP regs size (bytes) = 8
	// callee saved regs size (bytes) = 0
	// local vars size (bytes) = 8
	// fixed args size (bytes) = 8
	{
		allocframe(#16)
		r0 = ##.LC0
	}
	{
		r1 = add(r29,#8)
		memw(r29+#0) = r1.new
	}
	{
		r1 = add(r29,#12)
		memw(r29+#4) = r1.new
		call scanf
	}
	{
		dealloc_return
	}
	.size	main, .-main
	.ident	"GCC: (Sourcery CodeBench Lite 2012.03-101) 4.6.1"
	.section	.note.GNU-stack,"",@progbits
