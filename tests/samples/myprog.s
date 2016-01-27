	 .data
         .text
         .global __start
 __start:
    addi $s2, $zero, 125
	addi $s1, $zero, 25
    div $s2, $s1
    mfhi $t1
    mflo $t2
