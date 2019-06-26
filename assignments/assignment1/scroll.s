############################################################ -*- asm -*-
# COMP1521 18s2 -- Assignment 1 -- Scrolling Text!
# Scroll letters from a message in argv[1]
#
# Base code by Jashank Jeremy
# Tweaked by John Shepherd
# $Revision: 1.5 $
#
# Edit me with 8-column tabs!

# Requires:
#  - `all_chars', defined in chars.s

# Provides:
	.globl	main # :: int, [char *], [char *] -> int
	.globl	setUpDisplay # :: int, int -> void
	.globl	showDisplay # :: void -> void
	.globl	delay # :: int -> vovid
	.globl	isUpper # :: char -> int
	.globl	isLower # :: char -> int

	.globl	CHRSIZE
	.globl	NROWS
	.globl	NDCOLS
	.globl	MAXCHARS
	.globl	NSCOLS
	.globl	CLEAR


########################################################################
	.data

	# /!\ NOTE /!\
	# In C, the values of the symbols `CHRSIZE', `NROWS', `NDCOLS',
	# `NSCOLS', `MAXCHARS', and `CLEAR' would be substituted during
	# preprocessing.  SPIM does not have preprocessing facilities,
	# so instead we provide these values in the `.data' segment.

	# # of rows and columns in each big char
CHRSIZE:	.word	9
	# number of rows in all matrices
NROWS:		.word	9
	# number of columns in display matrix
NDCOLS:		.word	80
	# max length of input string
MAXCHARS:	.word	100
	# number of columns in bigString matrix
	# max length of buffer to hold big version
	# the +1 allows for one blank column between letters
NSCOLS:		.word	9000	# (NROWS * MAXCHARS * (CHRSIZE + 1))
	# ANSI escape sequence for 'clear-screen'
CLEAR:	.asciiz "\033[H\033[2J"
	# newline
NWLN:	.asciiz "\n"
#CLEAR:	.asciiz "__showpage__\n" # for debugging

main__0:	.asciiz	"Usage: ./scroll String\n"
main__1:	.asciiz	"Only letters and spaces are allowed in the string!\n"
main__2:	.asciiz "String mush be < "
main__3:	.asciiz " chars\n"
main__4:	.asciiz "Please enter a string with at least one character!\n"

	.align	4
theString:	.space	101	# MAXCHARS + 1
	.align	4
display:	.space	720	# NROWS * NDCOLS
	.align	4
bigString:	.space	81000	# NROWS * NSCOLS


########################################################################
# .TEXT <main>
	.text
main:

# Frame:	$fp, $ra,
# Uses:		$a0, $a1, $t0, $t1, $t2, $s0, $s1, $s2, $s3, $s4, $s5
# Clobbers:	$t5, $t6, $t7, $t8, $t9

# Locals:
#	- `theLength' in $s0
#	- `bigLength' in $s1
#	- `ch' in $s2
#	- `str' in $t2
#	- `i' in $t0
#	- `j' in $t1
#	- `row' in $t3
#	- `col' in $t4
#	- `iterations' in $t1
#	- `startingCol' in $t2


# Structure:
#	main
#	-> [prologue]
#	-> main_argc_gt_two
#	-> main_PTRs_init
#	  -> main_PTRs_cond
#	    -> main_ch_notspace
#	    -> main_ch_isLower
#	    -> main_ch_isSpace
#	  -> main_PTRs_step
#	-> main_PTRs_f
#	[theLength cond]
#	  | main_theLength_ge_MAXCHARS
#	  | main_theLength_lt_MAXCHARS
#	  | main_theLength_lt_1
#	  | main_theLength_ge_1
#	-> main_display:	
# 	  -> display_row_while
# 	  -> display_col_while
# 	  -> display_row_while_end
#	-> create_bigchars:
#	  -> bigchars_while
#	-> ch_space:
#   	   -> ch_space_row_while
#	   -> ch_space_col_while
#	   -> ch_space_col_while_end
#	-> ch_not_space
#	   -> upper_index
#	   -> lower_index:
#	   -> not_space_while:
#	   -> not_space_row_while:
#	   -> not_space_col_while_end:
#	-> end_bigchars_if:
#	   -> end_bigchars_if_while:
#	   -> bigchars_iw_end:
#	-> end_bigchars_while:
#	   -> iterations_while:
#	   -> iterations_while_end:
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)  # note: switch to $fp-relative
	sw	$s0, -8($fp)
	sw	$s1, -12($fp)
	sw	$s2, -16($fp)
	addi	$sp, $sp, -20

	# if (argc < 2)
	li	$t0, 2
	bge	$a0, $t0, main_argc_gt_two
	nop	# in delay slot
	# printf(...)
	la	$a0, main__0
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	# return 1  =>  load $v0, jump to epilogue
	li	$v0, 1
	j	main__post
	nop	# in delay slot
main_argc_gt_two:

	move	$s0, $zero
main_PTRs_init:
	# s = argv[1]
	lw	$t2, 4($a1)
main_PTRs_cond:
	# optimisation: `ch = *s' now
	# (ch = )*s
	lb	$s2, ($t2)
	# *s != '\0'  =>  ch != 0
	beqz	$s2, main_PTRs_f
	nop	# in delay slot

	# if (!isUpper(ch))
main_ch_upper:
	move	$a0, $s2
	jal	isUpper
	nop	# in delay slot
	beqz	$v0, main_ch_lower
	nop	# in delay slot
	j	main_ch_ok
	nop	# in delay slot
	# if (!isLower(ch))
main_ch_lower:
	move	$a0, $s2
	jal	isLower
	nop	# in delay slot
	beqz	$v0, main_ch_space
	nop	# in delay slot
	j	main_ch_ok
	nop	# in delay slot
	# if (ch != ' ')
main_ch_space:
	li	$t0, ' '
	bne	$s2, $t0, main_ch_fail
	nop	# in delay slot
	j	main_ch_ok
	nop	# in delay slot

main_ch_fail:
	# printf(...)
	la	$a0, main__1
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	# exit(1)  =>  return 1  =>  load $v0, jump to epilogue
	li	$v0, 1
	j	main__post
	nop	# in delay slot

main_ch_ok:
	# if (theLength >= MAXCHARS)
	la	$t0, MAXCHARS
	lw	$t0, ($t0)
	# break  =>  jump out of for(*s...)
	bge	$s0, $t0, main_PTRs_f

	# theString[theLength]
	la	$t0, theString
	addu	$t0, $t0, $s0	# ADDU because address
	# theString[theLength] = ch
	sb	$s2, ($t0)

	# theLength++
	addi	$s0, $s0, 1

main_PTRs_step:
	# s++  =>  s = s + 1
	addiu	$t2, $t2, 1	# ADDIU because address
	j	main_PTRs_cond
	nop
main_PTRs_f:

	# theString[theLength] = ...
	la	$t0, theString
	addu	$t0, $t0, $s0	# ADDU because address
	# theString[theLength] = '\0'
	sb	$zero, ($t0)

	# CHRSIZE + 1
	la	$t0, CHRSIZE
	lw	$t0, ($t0)
	addi	$t0, $t0, 1
	# bigLength = theLength * (CHRSIZE + 1)
	mul	$s1, $t0, $s0

	# if (theLength >= MAXCHARS)
	la	$t0, MAXCHARS
	lw	$t0, ($t0)
	blt	$s0, $t0, main_theLength_lt_MAXCHARS
	nop	# in delay slot
main_theLength_ge_MAXCHARS:
	# printf(..., ..., ...)
	la	$a0, main__2
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	move	$a0, $t0
	li	$v0, 1 # PRINT_INT_SYSCALL
	syscall
	la	$a0, main__3
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	# return 1  =>  load $v0, jump to epilogue
	li	$v0, 1
	j	main__post
	nop	# in delay slot
main_theLength_lt_MAXCHARS:

	# if (theLength < 1)
	li	$t0, 1
	bge	$s0, $t0, main_theLength_ge_1
	nop	# in delay slot
main_theLength_lt_1:
	# printf(...)
	la	$a0, main__4
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	# exit(1)  =>  return 1  =>  load $v0, jump to epilogue
	li	$v0, 1
	j	main__post
	nop	# in delay slot
main_theLength_ge_1:
main_display:	
	# initialise the display to all spaces
	li	$t0, 0			# i = 0
display_row_while:
	lw	$t7, NROWS
	# if (i >= NROWS)
	# break  =>  jump out of loop
	bge  	$t0, $t7, create_bigchars
	nop	# in delay slot
	move 	$t1, $zero			# j = 0
display_col_while:
	lw	$t8, NDCOLS
	# if (j >= NDCOLS)
	# break  =>  jump out of loop
	bge  	$t1, $t8, display_row_while_end
	nop	# in delay slot	

	# offset = i * NDCOLS + j
	mul 	$t9, $t0, $t8			# i * NDCOLS
	add 	$t9, $t9, $t1 			# + j = offset 
	li 	$t8, ' '			# space

	la 	$t7, display
	addu 	$t7, $t7, $t9 			# adding offset to display
	sb 	$t8, ($t7)			# display[i][j] = ' '

	addi 	$t1, $t1, 1 			# j++
	j 	display_col_while
	nop	# in delay slot	
display_row_while_end:
	addi 	$t0, $t0, 1 			# i++ 
	j 	display_row_while
	nop
create_bigchars:
	move 	$t0, $zero			# i = 0
bigchars_while:
	# if i >= theLength -> exit loop
	bge  	$t0, $s0, end_bigchars_while
	nop

	la 	$t7, theString
	addu 	$t7, $t7, $t0 			# add offset to theString, i.e. theString[i] (ch)
	li 	$t8, ' '			# space

	# if ch ($t7) != ' '  ->  ch_not_space
	lb 	$s2, ($t7)
	bne 	$s2, $t8, ch_not_space
	nop

	# else fall through
ch_space:
	li  	$t3, 0				# row = 0
ch_space_row_while:
	lw 	$t9, CHRSIZE

	# if row >= CHRSIZE -> end_bigchars_if
	bge  	$t3, $t9, end_bigchars_if
	nop
	move 	$t4, $zero			# col = 0
ch_space_col_while:
	la 	$t7, bigString

	# offset = row * NSCOLS + (col + i * (CHRSIZE+1))
	lw 	$t5, NSCOLS
	mul	$t5, $t3, $t5			# row * NSCOLS
	lw 	$t6, CHRSIZE		
	addi 	$t6, $t6, 1			# CHRSIZE + 1	
	mul 	$t6, $t0, $t6			# i * CHRSIZE + 1
	add 	$t5, $t5, $t6			# offset
	addu 	$t7, $t7, $t5 			# adding offset to bigString
	li 	$t8, ' '			# space
	sb 	$t8, ($t7)			# bigString[row][col + i * (CHRSIZE+1)] = ' '

	addi 	$t4, $t4, 1			# col++
	
	# if col >= CHRSIZE -> ch_space
	bge  	$t4, $t9, ch_space_col_while_end
	nop
	
	# otherwise fall through and loop
	j ch_space_col_while
	nop
ch_space_col_while_end:
	addi 	$t3, $t3, 1			# row++
	j 	ch_space_row_while
	nop
ch_not_space:
	move 	$t6, $s2			# $t6 = which = ch

	# if isUpper
	li 	$t5, 1
	move 	$a0, $s2 			# $a0 = ch
	jal 	isUpper
	nop
	beq  	$v0, $t5, upper_index 		# if isUpper == 1
	nop

	# if isLower
	li 	$t5, 1
	move 	$a0, $s2 			# $a0 = ch
	jal isLower
	nop
	beq 	$v0, $t5, lower_index		# if isLower == 1
	nop
upper_index:
	# which = ch - 'A'
	li 	$t5, -1
	li  	$t8, 'A'
	mul	$t8, $t8, $t5
	add 	$t6, $s2, $t8

	j not_space_while
	nop
lower_index:
	# which = ch - 'a' + 26
	li 	$t5, -1
	li 	$t8, 'a'
	mul 	$t8, $t8, $t5
	add 	$t6, $s2, $t8
	addi 	$t6, $t6, 26
not_space_while:
	move 	$t3, $zero			# row = 0
not_space_row_while:
	lw  	$t8, CHRSIZE		
	# if row >= CHRSIZE -> end_bigchars_if
	bge 	$t3, $t8, end_bigchars_if
	nop
	# otherwise fallthrough
	move	$t4, $zero			# col = 0
not_space_col_while:
	# copy char to the buffer
   	# bigString[row][col + i * (CHRSIZE+1)] = all_chars[which][row][col];	
	lw   	$t5, NSCOLS
	lw  	$t8, CHRSIZE		
	
	# if col >= CHRSIZE -> not_space_col_while_end
	bge  	$t4, $t8, not_space_col_while_end
	nop

	# bigString[row][col + i * (CHRSIZE+1)] 
	# offset = row * NSCOLS + (col + i * (CHRSIZE+1))
	mul  	$t5, $t3, $t5			# row * NSCOLS	
	addi 	$t9, $t8, 1			# CHRSIZE + 1	
	mul  	$t9, $t0, $t9			# i * CHRSIZE + 1
	add  	$t9, $t4, $t9			# + col
	add 	$t5, $t5, $t9			# offset

	la 	$t7, bigString
	addu  	$t7, $t7, $t5 			# adding offset to bigString

	# all_chars[which][row][col]
	# offset = (which * CHRSIZE * CHRSIZE) + (row * CHRSIZE) + col
	lw  	$t8, CHRSIZE		
	mul  	$t1, $t8, $t8			# CHRSIZE * CHRSIZE
	mul  	$t1, $t1, $t6		 	# which * CHRSIZE * CHRSIZE
	mul  	$t9, $t3, $t8			# row * CHRSIZE
	add 	$t1, $t1, $t9			# (which * CHRSIZE * CHRSIZE) + (row * CHRSIZE)
	add 	$t1, $t1, $t4			# + col = offset_all_chars

	la 	$t5, all_chars
	addu  	$t1, $t1, $t5 			# add offset to all_chars

	lb  	$t9, ($t1)
	sb   	$t9, ($t7)			# bigString[row][col + i * (CHRSIZE+1)] = all_chars[which][row][col];	

	addi 	$t4, $t4, 1			# col++
	j 	not_space_col_while
	nop
not_space_col_while_end:
	addi 	$t3, $t3, 1			# row++	
	j 	not_space_row_while
	nop
end_bigchars_if:
	lw   	$t4, CHRSIZE			# col = CHRSIZE
	move 	$t8, $t4			
	addi 	$t8, $t8, 1			# CHRSIZE + 1
	mul  	$t8, $t0, $t8			# i * (CHRSIZE+1)
	add  	$t4, $t4, $t8			# col = (i * (CHRSIZE+1)) + CHRSIZE

	move 	$t3, $zero			# row = 0
end_bigchars_if_while:
	lw   	$t8, CHRSIZE		
	la   	$t7, bigString
	
	# row >= CHRSIZE -> exit loop
	bge  	$t3, $t8, bigchars_iw_end
	
	# offset = row * NSCOLS + col
	lw   	$t5, NSCOLS
	mul  	$t5, $t3, $t5			# row * NSCOLS
	add  	$t5, $t5, $t4			# + col = offset 
	add  	$t7, $t7, $t5 			# adding offset to bigString
	li   	$t8, ' '			# space
	sb   	$t8, ($t7)			# bigString[row][col] = ' '

	addi 	$t3, $t3, 1			# row++
	j 	end_bigchars_if_while
	nop
bigchars_iw_end:
	addi 	$t0, $t0, 1 			# i++
	# return to beginning of loop
	j 	bigchars_while
	nop

end_bigchars_while:
	lw 	$t3, NDCOLS
	move 	$t1, $s1 			# iterations = bigLength
	add 	$t1, $t1, $t3 			# iterations += NDCOLS

	move 	$t2, $t3			# starting_col = NDCOLS
	addi 	$t2, $t2, -1			# starting_col += -1

	move 	$t0, $zero 			# i = 0
iterations_while:
	# i >= iterations -> iterations_while_end
	bge 	$t0, $t1, iterations_while_end
	nop

	# saving important variables
	move 	$s3, $t0			# i
	move 	$s4, $t1			# iterations
	move 	$s5, $t2			# starting_col

	# setUpDisplay(starting_col, bigLength)
	move 	$a0, $t2			
	move 	$a1, $s1

	jal 	setUpDisplay
	nop

	# showDisplay()
	jal 	showDisplay
	nop

	# reloading important values into appropriate registers
	move 	$t0, $s3
	move 	$t1, $s4
	move 	$t2, $s5			

	addi 	$t2, $t2, -1			# starting_col --
	li 	$t4, 1
	move 	$a0, $t4
	
	# saving important variables
	move 	$s3, $t0			# i
	move 	$s4, $t1			# iterations
	move 	$s5, $t2			# starting_col

	jal 	delay
	nop

	# reloading important values into appropriate registers
	move 	$t0, $s3
	move 	$t1, $s4
	move 	$t2, $s5

	addi 	$t0, $t0, 1 			# i++
	j 	iterations_while
	nop
iterations_while_end:
	# return 0
	move	$v0, $zero
main__post:
	# tear down stack frame
	lw	$s2, -16($fp)
	lw	$s1, -12($fp)
	lw	$s0, -8($fp)
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <setUpDisplay>
	.text
setUpDisplay:

# Frame:	$fp, $ra
# Uses:		$a0, $a1, $t0, $t1, $t2, $t3
# Clobbers:	$t4, $t5, $t6, $t7

# Locals:
#	- `row' in $t0
#	- `out_col' in $t1
#	- `in_col' in $t2
#	- `first_col' in $t3

# Structure:
#	setUpDisplay
#	-> [prologue]
#	-> starting_if
#	-> starting_else
#	   -> sUD_else_while1
#	   -> sUD_else_while2
#	   -> sUD_else_while2_end
#	   -> starting_end_else
#	-> starting_end_if
#	-> sUD_while
#	   -> sUD_while2
#	   -> end_sUD_while2
#	-> end_setUpDisplay
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)

starting_if:
	# if starting_col >= 0 -> starting_end_if
	bge  	$a0, $zero, starting_else
	nop

	move 	$t1, $zero			# out_col = 0

	move 	$t3, $a0			
	li   	$t4, -1
	mul  	$t3, $t3, $t4 			# first_col = -starting
	
	j 	starting_end_if
	nop
starting_else:
	move 	$t1, $zero			# out_col = 0
sUD_else_while1:
	# if out_col >= starting -> starting_end_else
	bge  	$t1, $a0, starting_end_else
	nop
	move 	$t0, $zero			# row = 0
sUD_else_while2:
	lw   	$t5, NROWS
	# if row >= NROWS -> loop
	bge 	$t0, $t5, sUD_else_while2_end
	nop

	# offset = row * NDCOLS + out_col
	lw   	$t5, NDCOLS
	mul  	$t5, $t5, $t0
	add  	$t5, $t5, $t1

	la   	$t6, display
	addu  	$t6, $t6, $t5 			# add offset to display

	li   	$t5, ' '			# space
	sb  	$t5, ($t6)			# display[row][out_col] = ' '

	addi 	$t0, $t0, 1			# row++
	j 	sUD_else_while2
	nop
sUD_else_while2_end:
	addi 	$t1, $t1, 1			# out_col++
	j 	sUD_else_while1
	nop
starting_end_else:
	move 	$t3, $zero			# first_col = 0
starting_end_if:
	move 	$t2, $t3 			# in_col = first_col
sUD_while:
	# if in_col >= bigLength -> end_setUpDisplay
	bge  	$t2, $a1, end_setUpDisplay
	nop

	# if out_col >= NDCOLS
	lw   	$t5, NDCOLS
	bge  	$t1, $t5, end_setUpDisplay  	# break
	nop

	move 	$t0, $zero 			# row = 0
sUD_while2:
	# if row >= NROWS -> exit loop
	lw   	$t5, NROWS
	bge  	$t0, $t5, end_sUD_while2
	nop
	
	# display offset = row * NDCOLS + out_col
	lw   	$t5, NDCOLS
	mul  	$t5, $t5, $t0
	add  	$t5, $t5, $t1
	la   	$t6, display
	addu  	$t6, $t6, $t5 			# add offset to display
	
	# bigString offset = row * NSCOLS + in_col
	lw   	$t5, NSCOLS
	mul  	$t5, $t5, $t0
	add  	$t5, $t5, $t2
	la   	$t7, bigString	
	addu  	$t7, $t7, $t5 			# add offset to bigString

	lb   	$t5, ($t7)
	sb   	$t5, ($t6) 			# display[row][out_col] = bigString[row][in_col]

	addi 	$t0, $t0, 1 			# row++
	j 	sUD_while2
	nop
end_sUD_while2:
	addi 	$t1, $t1, 1 			# out_col++
	addi 	$t2, $t2, 1 			# in_col++
	j 	sUD_while
	nop
end_setUpDisplay:
	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <showDisplay>
	.text
showDisplay:

# Frame:	$fp, $ra
# Uses:		$a0, $t0, $t1
# Clobbers:	$t2, $t3, $t4

# Locals:
#	- `i' in $t0
#	- `j' in $t1

# Structure:
#	showDisplay
#	-> [prologue]
#	-> sD_row_while
# 	   -> sD_col_while
# 	   -> print_new_line
# 	-> end_showDisplay
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)

	# printf(CLEAR)
	la	$a0, CLEAR
	li	$v0, 4 				# PRINT_STRING_SYSCALL
	syscall

	li  	$t0, 0				# i = 0
	lw  	$t2, NROWS
	lw  	$t3, NDCOLS
sD_row_while:
	# if i >= NROWS -> end_showDisplay
	bge  	$t0, $t2, end_showDisplay
	nop

	# else fallthrough
	li   	$t1, 0				# j = 0
sD_col_while:
	# if j >= NDCOLS -> print_new_line
	lw  	$t3, NDCOLS
	bge  	$t1, $t3, print_new_line
	nop

	# offset = i * NDCOLS + j
	move 	$t4, $t0
	mul  	$t4, $t4, $t3 			# i * NDCOLS
	add  	$t4, $t4, $t1 			# + j = offset

	la   	$t5, display
	addu  	$t5, $t5, $t4			# adding offset to display
 
	lb 	$a0, ($t5)
	li	$v0, 11 			# PRINT_CHAR_SYSCALL
	syscall					# putchar(display[i][j])

	addi 	$t1, $t1, 1			# j++
	j 	sD_col_while
	nop
print_new_line:
	# print '\n'
	la	$a0, NWLN
	li	$v0, 4 				# PRINT_STRING_SYSCALL
	syscall
	
	addi 	$t0, $t0, 1			# i++
	j 	sD_row_while
	nop
end_showDisplay:
	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <delay>
	.text
delay:

# Frame:	$fp, $ra
# Uses:		$a0, $t0, $t1, $t2, $t3, $t4, $t5
# Clobbers:	$t0, $t1, $t2, $t3, $t4, $t5

# Locals:
#	- `n' in $a0
#	- `x' in $t0
#	- `i' in $t1
#	- `j' in $t2
#	- `k' in $t3

# Structure:
#	delay
#	-> [prologue]
#	-> delay_i_init
#	-> delay_i_cond
#	   -> delay_j_init
#	   -> delay_j_cond
#	      -> delay_k_init
#	      -> delay_k_cond
#	         -> delay_k_step
#	      -> delay_k_f
#	      -> delay_j_step
#	   -> delay_j_f
#	   -> delay_i_step
#	-> delay_i_f
#	-> [epilogue]

# Code:
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)

	# x <- 0
	move	$t0, $zero
	# These values control the busy-wait.
	li	$t4, 2000
	li	$t5, 10

delay_i_init:
	# i = 0;
	move	$t1, $zero
delay_i_cond:
	# i < n;
	bge	$t1, $a0, delay_i_f
	nop	# in delay slot

delay_j_init:
	# j = 0;
	move	$t2, $zero
delay_j_cond:
	# j < DELAY_J;
	bge	$t2, $t4, delay_j_f
	nop	# in delay slot

delay_k_init:
	# k = 0;
	move	$t3, $zero
delay_k_cond:
	# k < DELAY_K;
	bge	$t3, $t5, delay_k_f
	nop	# in delay slot

	# x = x + 1
	addi	$t0, $t0, 1

delay_k_step:
	# k = k + 1
	addi	$t3, $t3, 1
	j	delay_k_cond
	nop	# in delay slot
delay_k_f:

delay_j_step:
	# j = j + 1
	addi	$t2, $t2, 1
	j	delay_j_cond
	nop	# in delay slot
delay_j_f:

delay_i_step:
	# i = i + 1
	addi	$t1, $t1, 1
	j	delay_i_cond
	nop	# in delay slot
delay_i_f:

delay__post:
	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <isUpper>
	.text
isUpper:

# Frame:	$fp, $ra
# Uses:		$a0
# Clobbers:	$v0

# Locals:
#	- `ch' in $a0
#	- ... $v0 used as temporary register

# Structure:
#	isUpper
#	-> [prologue]
#	[ch cond]
#	   | isUpper_ch_ge_a
#	   | isUpper_ch_le_z
#	   | isUpper_ch_lt_a
#	   | isUpper_ch_gt_z
#	-> isUpper_ch_phi
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)

	# if (ch >= 'A')
	li	$v0, 'A'
	blt	$a0, $v0, isUpper_ch_lt_A
	nop	# in delay slot
isUpper_ch_ge_A:
	# if (ch <= 'Z')
	li	$v0, 'Z'
	bgt	$a0, $v0, isUpper_ch_gt_Z
	nop	# in delay slot
isUpper_ch_le_Z:
	addi	$v0, $zero, 1
	j	isUpper_ch_phi
	nop	# in delay slot

	# ... else
isUpper_ch_lt_A:
isUpper_ch_gt_Z:
	move	$v0, $zero
	# fallthrough
isUpper_ch_phi:

isUpper__post:
	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <isLower>
	.text
isLower:

# Frame:	$fp, $ra
# Uses:		$a0
# Clobbers:	$v0

# Locals:
#	- `ch' in $a0
#	- ... $v0 used as temporary register

# Structure:
#	isLower
#	-> [prologue]
#	[ch cond]
#	   | isLower_ch_ge_a
#	   | isLower_ch_le_z
#	   | isLower_ch_lt_a
#	   | isLower_ch_gt_z
#	-> isLower_ch_phi
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)

	# if (ch >= 'a')
	li	$v0, 'a'
	blt	$a0, $v0, isLower_ch_lt_a
	nop	# in delay slot
isLower_ch_ge_a:
	# if (ch <= 'z')
	li	$v0, 'z'
	bgt	$a0, $v0, isLower_ch_gt_z
	nop	# in delay slot
isLower_ch_le_z:
	addi	$v0, $zero, 1
	j	isLower_ch_phi
	nop	# in delay slot

	# ... else
isLower_ch_lt_a:
isLower_ch_gt_z:
	move	$v0, $zero
	# fallthrough
isLower_ch_phi:

isLower__post:
	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

#################################################################### EOF

