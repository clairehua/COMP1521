# COMP1521 Practice Prac Exam #1
# int everyKth(int *src, int n, int k, int*dest)

   .text
   .globl everyKth

# params: src=$a0, n=$a1, k=$a2, dest=$a3
everyKth:
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

   # add code for your everyKth function here
   
   li   $t0, 0     # i = 0
   li   $t1, 0     # j = 0
   
forLoop:
   bge  $t0, $a1, endFor    # if i >= n exit
   nop

   rem  $t4, $t0, $a2
   bnez $t4, continueFor
   nop
   
   # else fall through to if condition
ifKth:   
   li   $t2, 4         # sizeof(int)
   la   $t3, ($a0)     # address of src
   mul  $t4, $t0, $t2  # i * 4
   addu $t4, $t4, $t3  # src[i]

   la   $t3, ($a3)     # address of dest
   mul  $t5, $t1, $t2  # i * 4
   addu $t5, $t5, $t3  # dest[i]

   lw   $t4, ($t4)
   sw   $t4, ($t5)     # dest[j] = src[i];
   
   addi $t1, 1 	  	   # j++

continueFor:
   addi $t0, $t0, 1    # i++
   j forLoop
   nop

endFor:
   move $v0, $t1	   # return j
   
   
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

