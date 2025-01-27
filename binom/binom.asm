sll $sp, $imm1, $imm2, $zero, 1, 11			# set $sp = 1 << 11 = 2048
lw $a0, $zero, $imm2, $zero, 0, 256			# get n from address 256
lw $a1, $zero, $imm2, $zero, 0, 257			# get k from address 257
jal $ra, $zero, $zero, $imm2, 0, binom		# calc $v0 = binom(n,k)
sw $zero, $zero, $imm2, $v0, 0,258			# store binom(n,k)=$v0 in 258
halt $zero, $zero, $zero, $zero, 0, 0		# halt
binom:
add $sp, $sp, $imm2, $zero, 0, -4			# adjust stack for 4 items
sw $zero, $sp, $imm2, $s0, 0, 3				# save $s0
sw $zero, $sp, $imm2, $ra, 0, 2				# save return address
sw $zero, $sp, $imm2, $a0, 0, 1				# save argument n
sw $zero, $sp, $imm2, $a1, 0, 0				# save argument k
beq $zero,$a1,$zero,$imm2,0,Stop			# jump to Stop if k = 0
beq $zero,$a0,$a1,$imm2,0,Stop				# jump to Stop if n = k
beq $zero,$zero,$zero,$imm2,0,L1			# else,jump to L1
Stop:
add $v0, $imm1, $zero, $zero, 1, 0			# binom(n,k) = 1, copy input
beq $zero,$zero,$zero,$imm2,0,L2			# jump to L2
L1:
sub $a0, $a0, $imm2, $zero, 0, 1			# calculate n - 1
jal $ra, $zero, $zero, $imm2, 0, binom		# calc $v0=binom(n-1,k)
add $s0, $v0, $imm2, $zero, 0, 0			# $s0 = binom(n-1,k)
lw $a0, $sp, $imm2, $zero, 0, 1				# restore $a0 = n
sub $a0, $a0, $imm2, $zero, 0, 1			# calculate n - 1
sub $a1, $a1, $imm2, $zero, 0, 1			# calculate k - 1
jal $ra, $zero, $zero, $imm2, 0, binom		# calc binom(n-1,k-1)
add $v0, $v0, $s0, $zero, 0, 0				# $v0 = binom(n-1,k) + binom(n-1,k)
lw $a1, $sp, $imm2, $zero, 0, 0				# restore $a1
lw $a0, $sp, $imm2, $zero, 0, 1				# restore $a0
lw $ra, $sp, $imm2, $zero, 0, 2				# restore $ra
lw $s0, $sp, $imm2, $zero, 0, 3				# restore $s0
L2:
add $sp, $sp, $imm2, $zero, 0, 4			# pop 4 items from stack
beq $zero, $zero, $zero, $ra, 0, 0			# and return
.word 256 8
.word 257 3