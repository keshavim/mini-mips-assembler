A assembler I made for the mips language to learn about assembly and how to parse data from files.
This is suposed to act similer to the MARS IDE for mips.
It has a smaller instruction set then MARS but the instructions it has should give the same hex numbers when assembled


to run, or clone this project and compile with an ide

to run, you can download the jar in the out directory, or clone this project


Then give the asm / text files for it to assemble

it can take multiple files at once and will compile each into two files, data_filename and text_filename.

It also has a "-o" flag which reads the next arg as a directory and output the files to that directory instead of the directory
of the original file.
