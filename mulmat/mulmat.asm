add $s1,$zero,$zero,$zero,0,0
add $s2,$zero,$zero,$zero,0,0
add $t2,$zero,$zero,$zero,0,0
add $a2,$zero,$imm1,$zero,256,0 
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
    .word 255 1      # MEM[255] = 1
    .word 256 0      # MEM[256] = 0
    .word 257 0      # MEM[257] = 0
    .word 258 0      # MEM[258] = 0
    .word 259 0      # MEM[259] = 0
    .word 260 2      # MEM[260] = 2
    .word 261 0      # MEM[261] = 0
    .word 262 0      # MEM[262] = 0
    .word 263 0      # MEM[263] = 0
    .word 264 0      # MEM[264] = 0
    .word 265 3      # MEM[265] = 3
    .word 266 0      # MEM[266] = 0
    .word 267 0      # MEM[267] = 0
    .word 268 0      # MEM[268] = 0
    .word 269 0      # MEM[269] = 0
    .word 270 4      # MEM[270] = 4
    .word 271 1      # MEM[271] = 1
    .word 272 0      # MEM[272] = 0
    .word 273 0      # MEM[273] = 0
    .word 274 0      # MEM[274] = 0
    .word 275 0      # MEM[275] = 0
    .word 276 2      # MEM[276] = 2
    .word 277 0      # MEM[277] = 0
    .word 278 0      # MEM[278] = 0
    .word 279 0      # MEM[279] = 0
    .word 280 0      # MEM[280] = 0
    .word 281 3      # MEM[281] = 3
    .word 282 0      # MEM[282] = 0
    .word 283 0      # MEM[283] = 0
    .word 284 0      # MEM[284] = 0
    .word 285 0      # MEM[285] = 0
    .word 286 4      # MEM[286] = 4


    



