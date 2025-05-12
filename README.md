A assembler I made for the mips language to learn about assembly and how to parse data from files.
This is suposed to act similer to the MARS IDE for mips.
It has a smaller instruction set then MARS but the instructions it has should give the same hex numbers when assembled

# How to run:

This will create a folder called "filename" in the same directory as "filename.asm" that contains the data and text hex files.

`java -jar program.jar filename.asm`

Can compile multiple at once

`java -jar program.jar file1.asm file2.asm file3.asm`

Can send the files to a different directory from the originals asm files.
Will try to create the directory if not found.

`java -jar program.jar filename.asm -o dirname`

for disassembler, run -d as the first arg and give as many space separated assembled mips instructions as you want

`java -jar program.jar -d 02f43824"`

this will output

`and {opcode: 00, rs: 17, rt: 14, rd: 07, shmt: 00, funct: 24}`


to run a asm file with limited instruction use --run with the .data and .text files which can be created from mars
