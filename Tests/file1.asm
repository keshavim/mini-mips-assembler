.data
prompt: .asciiz "Enter a number: "
result: .asciiz "The sum is: "

.text
main:
    la $a0, prompt     # Load address of prompt
    li $v0, 4          # Print string syscall
    syscall

    li $v0, 5          # Read integer syscall
    syscall
    move $t0, $v0      # Move input to $t0

    li $t1, 10         # Load immediate 10 to $t1
    add $t2, $t0, $t1  # Add $t0 and $t1, store in $t2

    la $a0, result     # Load address of result string
    li $v0, 4          # Print string syscall
    syscall

    move $a0, $t2      # Move sum to $a0 for printing
    li $v0, 1          # Print integer syscall
    syscall

    li $v0, 10         # Exit syscall
    syscall
