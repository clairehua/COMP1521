# COMP1521 Practice Prac Exam #1
# int rmOdd(int *src, int n, int*dest)

   .text
   .globl rmOdd

# params: src=$a0, n=$a1, dest=$a2
rmOdd:
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

   # add code for your rmOdd function here
   li   $t0, 0 		#t0 = i
   li   $t1, 0 		#t1 = j
forLoop:
   bge  $t0, $a1, endFor   # i >= n
   nop
   
   li   $t2, 4        # size of int
   la   $t3, ($a0)    # address of src
   
   mul  $t4, $t0, $t2 # offset = i * 4 + address
   addu  $t4, $t4, $t3
   
   lw   $t3, ($t4)
   li   $t5, 2
   rem  $t6, $t3, $t5
   
   beqz $t6, ifCondition
   nop
   
   #otherwise 
   
   j continueFor
   nop
   
ifCondition:
   la   $t7, ($a2)    # address of dest
   
   mul  $t4, $t1, $t2 # offset = j * 4 + address
   addu  $t4, $t4, $t7 
   
   sw   $t3, ($t4)  # dest[j] = src[i]

   addi $t1, $t1, 1  #j++
   
continueFor:
   addi $t0, $t0, 1  #i++
   j forLoop
   nop
   
endFor:
   move $v0, $t1

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

