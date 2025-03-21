.data
array: .asciiz "MIPS!"
newline: .asciiz "\n"

.text
main:
    la $t0, array      # Load address of array
    li $t1, 5          # Loop counter (length of "MIPS!")

loop:
    #lbu $a0, ($t0)     # Load byte (unsigned) from array
    li $v0, 11         # Print character syscall
    syscall

    addiu $t0, $t0, 1  # Increment array pointer
    #subi $t1, $t1, 1   # Decrement counter
    bne $t1, $zero, loop   # Branch if not equal to zero

    la $a0, newline    # Load address of newline
    li $v0, 4          # Print string syscall
    syscall

    li $t2, 10         # Load 10 into $t2
    li $t3, 5          # Load 5 into $t3
    sub $t4, $t2, $t3  # Subtract $t3 from $t2

    #slti $t5, $t4, 6   # Set less than immediate
    blt $t5, $zero, end    # Branch on less than

    andi $t6, $t4, 1   # AND immediate with 1 (check if odd)

end:
    li $v0, 10         # Exit syscall
    syscall
