A assembler I made for the mips language to learn about assembly and how to parse data from files.
This is suposed to act similer to the MARS IDE for mips.
It has a smaller instruction set then MARS but the instructions it has should give the same hex numbers when assembled

# How to run:

This will create a folder called "filename" in the same directory as "filename.asm" that contains the data and text hex files.

`java -jar filename.asm`

Can compile multiple at once

`java -jar file1.asm file2.asm file3.asm`

Can send the files to a different directory from the originals asm files.
Will try to create the directory if not found.

`java -jar filename.asm -o dirname`


