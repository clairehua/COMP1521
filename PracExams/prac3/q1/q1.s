# COMP1521 Practice Prac Exam #1
# int lowerfy(char *src, char *dest)
   .text
   .globl lowerfy

# params: src=$a0, dest=$a1
lowerfy:
# prologue
   addi $sp, $sp, -4
   sw   $fp, ($sp)
   la   $fp, ($sp)
   addi $sp, $sp, -4
   sw   $ra, ($sp)
   addi $sp, $sp, -4
   sw   $s0, ($sp)
   addi $sp, $sp, -4
   sw   $s1, ($sp)
   # if you need to save more $s? registers
   # add the code to save them here

# function body
# locals: ...
# $t0 = i
# $t1 = n
# $t5 = ch


   li   $t0, 0        # i = 0
   li   $t1, 0        # n = 0
   
forLoop:
   la   $t3, ($a0)    # address of src
   addu $t4, $t3, $t0 # address + i (offset in chars, sizeof(char) = 1
   lw   $t5, ($t4)    # ch = src[i]
   
   li   $t6, 0
   beq  $t5, $t6, endFor
   nop

ifCondition:
   li   $t6, 'A'
   blt  $t5, $t6, continueFor   # ch < A
   nop
   
   li   $t7, 'Z'
   bgt  $t5, $t7, continueFor   # ch > Z
   nop
   
   li   $t7, 'a'
   sub  $t5, $t5, $t6     # ch -'A'
   add  $t5, $t5, $t7     # + 'a'
   
   addi $t1, $t1, 1       # n++   
   

continueFor:
   la   $t3, ($a1)    # address of dest
   addu $t4, $t3, $t0 # address + i (offset in chars, sizeof(char) = 1
   sw   $t5, ($t4)    # dest[i] = ch
   
   addi $t0, $t0, 1  # i++
   
   j forLoop
   nop

endFor:
   la   $t3, ($a1)    # address of dest
   addu $t4, $t3, $t0 # address + i (offset in chars, sizeof(char) = 1
   li   $t5, 0
   sw   $t5, ($t4)    # dest[i] = '\0'

   move $v0, $t1      # return n
   
   
   
# epilogue
   # if you saved more than two $s? registers
   # add the code to restore them here
   lw   $s1, ($sp)
   addi $sp, $sp, 4
   lw   $s0, ($sp)
   addi $sp, $sp, 4
   lw   $ra, ($sp)
   addi $sp, $sp, 4
   lw   $fp, ($sp)
   addi $sp, $sp, 4
   j    $ra

