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
    .word 256 -4     # A Matrix
    .word 257 4      
    .word 258 1      
    .word 259 -1     
    .word 260 3      
    .word 261 2      
    .word 262 4      
    .word 263 5     
    .word 264 6      
    .word 265 -3      
    .word 266 -1      
    .word 267 -5      
    .word 268 -1      
    .word 269 -2      
    .word 270 0x3      
    .word 271 0x4      
    .word 272 0        # B matrix   
    .word 273 0      
    .word 274 0      
    .word 275 2      
    .word 276 -6   
    .word 277 -4    
    .word 278 -2    
    .word 279 -9    
    .word 280 -1   
    .word 281 -2   
    .word 282 -3    
    .word 283 -4      
    .word 284 4      
    .word 285 3      
    .word 286 2     
    .word 287 1 


    



