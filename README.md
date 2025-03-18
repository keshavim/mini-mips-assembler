# mini-mips-assembler
a mini mips assembler that takes a mips asm file and turns it into a .data and a .text file.

It basically is trying to recreate the fuctionality of the MARS MIPS Assembler and Runtime Simulator.

To compile, use a unix os (linux, mac, wsl) and install gcc. Use the makefile and run the output. curently in testing phase so can't send arguments yet.

todo:  
fix up the psudo instructions
add more instructions to handle
improve label and address handling
add error detection
test more with the .data portion,
add arguments so you can send a file or convert one line
