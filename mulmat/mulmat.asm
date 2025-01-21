add $s1,$zero,$zero,$zero,0,0
add $s2,$zero,$zero,$zero,0,0
add $t2,$zero,$zero,$zero,0,0
add $a2,$zero,$imm1,$zero,0,0 
add $t0,$zero,$zero,$zero,0,0
add $v0,$zero,$zero,$zero,0,0
add $gp,$imm1,$zero,$zero,32,0 # gp =32
Loop:
add $t1,$t0,$s2,$zero,0,0               # t1 =s2+t0 
lw $a0, $a2, $t1,$zero,0,0	            # a0 loads the value from the next place in matrix A
mac $t1,$t0,$imm1,$imm2,4,16            # t1 =  16+ 4*t0
add $t1,$t1,$t2,$zero,0,0               # t1 =  16+ 4*t0+t2
lw $a1, $a2, $t1,$zero,0,0	            # a1 loads the value from the next place in matrix B
mac $v0,$a0,$a1,$v0,0,0                 #v0 = A*B+v0   ----------------
add $t0,$t0,$imm1,$zero,1,0             #t0 ++
add $s1,$s1,$imm1,$zero,1,0             #s1 ++
beq $zero,$t0,$imm1,$imm2,4,fix         #if $t0 == 4 go fix >>> we did 4 multipications 
beq $zero,$zero,$zero,$imm2,0,Loop      #if $t0 != 4 continue looping >>> (we did less than 4 multipications) 
fix:
sw $v0,$a2,$gp,$zero,0,0                # mem[a2+$gp]= mem[a2+32+number of 4 multipications] = v0
add $gp,$gp,$imm1,$zero,1,0             #gp += 1
add $t2,$t2,$imm1,$zero,1,0             #t2 += 1
beq $zero,$s1,$imm1,$imm2,64,finish     #if s1=64 -->> finish
beq $zero,$t2,$imm1,$imm2,4,next        #if t2=4 -->> next
sub $t0,$t0,$t0,$zero,0,0               #initialize $t0 = 0
sub $v0,$v0,$v0,$zero,0,0               #initialize $v0 = 0
beq $zero,$zero,$zero,$imm2,0,Loop
next:
add $s2,$s2,$imm1,$zero,4,0 #s2 += 4
sub	$t2,$t2,$t2,$zero,0,0	# t2 = 0 start to count from 0 new 16 multi
sub $t0,$t0,$t0,$zero,0,0   # initialize $t0 = 0
sub $v0,$v0,$v0,$zero,0,0   #initialize $v0 = 0
beq $zero,$zero,$zero,$imm2,0,Loop
finish:
halt $zero,$zero,$zero,$zero,0,0
    .word 0 1      # MEM[0] = 1
    .word 1 0      # MEM[1] = 0
    .word 2 0      # MEM[2] = 0
    .word 3 0      # MEM[3] = 0
    .word 4 0      # MEM[4] = 0
    .word 5 2      # MEM[5] = 2
    .word 6 0      # MEM[6] = 0
    .word 7 0      # MEM[7] = 0
    .word 8 0      # MEM[8] = 0
    .word 9 0      # MEM[9] = 0
    .word 10 3     # MEM[10] = 3
    .word 11 0     # MEM[11] = 0
    .word 12 0     # MEM[12] = 0
    .word 13 0     # MEM[13] = 0
    .word 14 0     # MEM[14] = 0
    .word 15 4     # MEM[15] = 4
    .word 16 1     # MEM[16] = 1
    .word 17 0     # MEM[17] = 0
    .word 18 0     # MEM[18] = 0
    .word 19 0     # MEM[19] = 0
    .word 20 0     # MEM[20] = 0
    .word 21 2     # MEM[21] = 2
    .word 22 0     # MEM[22] = 0
    .word 23 0     # MEM[23] = 0
    .word 24 0     # MEM[24] = 0
    .word 25 0     # MEM[25] = 0
    .word 26 3     # MEM[26] = 3
    .word 27 0     # MEM[27] = 0
    .word 28 0     # MEM[28] = 0
    .word 29 0     # MEM[29] = 0
    .word 30 0     # MEM[30] = 0
    .word 31 4     # MEM[31] = 4

    



