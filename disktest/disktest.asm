		add		$s0,	$zero,	$zero,	$zero,	0,	0		# disk Read counter 0 -> 7
		add		$s1,	$imm1,	$zero,	$zero,	1,	0		# disk Write counter 1 -> 8
	read:
		beq		$zero,	$s0,	$imm1,	$imm2,	7,	write	# jump to write after we finished reading
		out		$zero,	$zero,	$imm1,	$s0,	15,	0		# Sector to copy to memory
		sll		$t0,	$s0,	$imm1,	$zero,	7,	0		# $t0 = sector*128 (getting memory "sector")
		out		$zero,	$zero,	$imm1,	$t0,	16,	0		# diskbuffer = $t0 
		out		$zero,	$zero,	$imm1,	$imm2,	14,	1		# Read from disk
		add		$s0,	$s0,	$imm1,	$zero,	1,	0		# $s0++
	loop1:
		in		$t0,	$imm1,	$zero,	$zero,	17,	0		# read disk status
		beq		$zero,	$t0,	$zero,	$imm2,	0,	read	# jump to read if the status is cleard
		beq		$zero,	$t0,	$imm1,	$imm2,	1,	loop1	# keep waiting
	write:
		beq		$zero,	$s1,	$imm1,	$imm2,	8,	end		# Halt after stopped writing
		out		$zero,	$zero,	$imm1,	$s1,	15,	0		# Sector to write to disk
		sub		$t0,	$s1,	$imm1,	$zero,	1,	0		# $t0 = $s1 -1
		sll		$t0,	$t0,	$imm1,	$zero,	7,	0		# $t0 = sector*128 (getting memory "sector")
		out		$zero,	$zero,	$imm1,	$t0,	16,	0		# diskbuffer = $t0 
		out		$zero,	$zero,	$imm1,	$imm2,	14,	2		# write from disk
		add		$s1,	$s1,	$imm1,	$zero,	1,	0		# $s1++
	loop2:
		in		$t0,	$imm1,	$zero,	$zero,	17,	0		# read disk status
		beq		$zero,	$t0,	$zero,	$imm2,	0,	write	# jump to read if the status is cleard
		beq		$zero,	$t0,	$imm1,	$imm2,	1,	loop2	# keep waiting
	end:
		halt	$zero,	$zero,	$zero,	$zero,	0,	0		# halt