lw $v0,$zero,$imm1,$zero,256,0			#$v0 = radius 
add $s2,$v0,$imm1,$zero,128,0			#$s2 = last line
sub $a0,$imm1,$v0,$zero,128,0			#$a0 =128-radius ,number of first line
mac	$v0,$v0,$v0,$zero,0,0				#$v0 = radius^2
add $a1,$zero,$zero,$zero,0,0			#restarting a1= column	
mac $a2,$a0,$imm1,$a1,256,0				# $a2= wanted adress = monitor[line+column] = [256*a0+a1]
Search:
sub $t0,$imm1,$a0,$zero,128,0			#t0 = 128-a0 = x
mac $t0,$t0,$t0,$zero,0,0				#t0 = t0^2
sub $t1,$imm1,$a1,$zero,128,0			#t1 = 128-a1 = y
mac $t1,$t1,$t1,$zero,0,0				#t1 = t1^2
add	$t2,$t0,$t1,$zero,0,0				#t2 = t0^2 + t1^2 = x^2 +  y^2
beq $zero,$v0,$t2,$imm1,Light,0			# if t2= x^2 +  y^2 = $v0 = radius^2 go lighting
bgt $zero,$v0,$t2,$imm1,Light,0			# if t2= x^2 +  y^2 < $v0 = radius^2 go lighting
add	$a1,$a1,$imm1,$zero,1,0				# else, keep Searching a1 += 1 next pixel
beq $zero,$zero,$zero,$imm2,0,Search	# continue Searching
Light:
#sub $t0,$imm1,$a0,$zero,128,0			
#mac $t0,$t0,$t0,$zero,0,0				
sub $t1,$imm1,$a1,$zero,128,0			#t1 = 128-a1 = y
mac $t1,$t1,$t1,$zero,0,0				#t1 = t1^2
add	$t2,$t0,$t1,$zero,0,0				#t2 = t0^2 + t1^2 = x^2 +  y^2
bgt $zero,$t2,$v0,$imm1,fix,0			# if t2= x^2 +  y^2 > $v0 = radius^2 go fix
mac $a2,$a0,$imm1,$a1,256,0				# else, in a2 = wanted adress = monitor[line+column] = [256*a0+a1]
out $zero,$imm1,$zero,$imm2,22,1		# write
out $zero,$imm1,$zero,$a2,20,0			# in address a2
out $zero,$imm1,$zero,$imm2,21,255		# 255
out $zero,$imm1,$zero,$imm2,22,0		# stop writing
add	$a1,$a1,$imm1,$zero,1,0				# a1 += 1 next pixel
beq $zero,$zero,$zero,$imm2,0,Light		# continue lighting
fix:
add	$a0,$a0,$imm1,$zero,1,0				#a0 += 1 - one line down
sub $a1,$a1,$a1,$zero,0,0				#a1 = 0 
bgt $zero,$a0,$s2,$imm1,Finish,0		# if a0 > s2 than we passed last line
beq $zero,$zero,$zero,$imm2,0,Search	# else, go Searching
Finish:
halt $zero,$zero,$zero,$zero,0,0		# halt
		.word 256 3						