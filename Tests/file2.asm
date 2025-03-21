.data
msg1: .asciiz "Numbers are equal\n"
msg2: .asciiz "Numbers are not equal\n"

.text
main:
    li $t0, 0x1234     # Load immediate value to $t0
    lui $t1, 0x5678    # Load upper immediate to $t1

    ori $t1, $t1, 0x1234   # OR immediate with $t1

    and $t2, $t0, $t1  # AND operation
    or $t3, $t0, $t1   # OR operation

    beq $t2, $t3, equal    # Branch if equal
    j not_equal        # Jump to not_equal

equal:
    la $a0, msg1       # Load address of msg1
    j print            # Jump to print

not_equal:
    la $a0, msg2       # Load address of msg2

print:
    li $v0, 4          # Print string syscall
    syscall

    li $v0, 10         # Exit syscall
    syscall
