import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;

public class Tests {
    private static final String GREEN = "\u001B[32m";
    private static final String RESET = "\u001B[0m";

    private static void instruction_test(){
        String filename = "instructions_test";
        Assembler asm = new Assembler();
        try(BufferedReader br = new BufferedReader(new FileReader(filename))){
            String line;
            while((line = br.readLine()) != null){

                long expected = Mips.parseNumber("0x"+line.substring(0, 8));

                long[] result = asm.assemble_word(line.substring(9));

                if(result[0] == expected){
                    System.out.println(GREEN +"Passes: "+line + RESET);
                }
                else{
                    System.err.println("Failed: "+line);
                    System.exit(1);
                }
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    static void test_file() throws IOException {
        FileAssembler fasm = new FileAssembler();

        String testdir = "Tests/";

        File test = new File(testdir);
        File[] files = test.listFiles();

        assert files != null;
        fasm.assembleFile(testdir + files[0].getName());
        fasm.assembleFile(testdir + files[1].getName());
        fasm.assembleFile(testdir + files[2].getName());


    }
    public static boolean compareFiles(String file1Path, String file2Path) throws IOException {
        try (BufferedReader reader1 = new BufferedReader(new FileReader(file1Path));
             BufferedReader reader2 = new BufferedReader(new FileReader(file2Path))) {

            String line1 = reader1.readLine();
            String line2 = reader2.readLine();

            while (line1 != null && line2 != null) {
                if (!line1.equals(line2)) {
                    System.out.println(line1);
                    System.out.println(line2);
                    return false;
                }
                line1 = reader1.readLine();
                line2 = reader2.readLine();
            }
            return true;
        }
    }

    private static boolean _deasmble_test(long[] words){
        for (long word : words) {
            System.out.println(Disassembler.disassemble(word));
        }



        return true;

    }
    public static void dissassemble_test(){

        String filename = "Tests/test_instructions_prog2.txt";
        Assembler asm = new Assembler();
        try(BufferedReader br = new BufferedReader(new FileReader(filename))){
            String line;
            while((line = br.readLine()) != null){

                long instruction = Mips.parseNumber("0x"+line.substring(0, 8));

                String expected = line.substring(9);

                String result = Disassembler.disassemble(instruction);



                if(result.equals(expected)){
                    System.out.println(GREEN +"Passes: "+line + RESET);
                }
                else{
                    System.err.println("Failed: \nresult: "+result+"\nexpected: "+expected);
                    System.exit(1);
                }
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }


        //addiu {opcode: 09, rs(base): 17, rt: 1b, immediate(offset): 003e}
        //ori {opcode: 0d, rs(base): 00, rt: 13, immediate(offset): ff20}
        //sw {opcode: 2b, rs(base): 14, rt: 14, immediate(offset): 0000}
        // lui {opcode: 0f, rs(base): 00, rt: 1d, immediate(offset): 0071}
        // lw {opcode: 23, rs(base): 1d, rt: 12, immediate(offset): 0000}
        // beq {opcode: 04, rs(base): 1b, rt: 0a, immediate(offset): ffae}
        // bne {opcode: 05, rs(base): 16, rt: 0b, immediate(offset): 00aa}

        long[] iwords = new long[]{ 0x26fb003e, 0x3413ff20, 0xae940000L, 0x3c1d0071, 0x8fb20000L, 0x136affae, 0x16cb00aa};
        _deasmble_test(iwords);

        // j {opcode: 02, index: 00000cd}

        long[] jwords = new long[]{ 0x080000cd};
        _deasmble_test(jwords);

        // sub {opcode: 00, rs: 13, rt: 1b, rd: 09, shmt: 00, funct: 22}
        // add {opcode: 00, rs: 18, rt: 0a, rd: 01, shmt: 00, funct: 20}
        // slt {opcode: 00, rs: 06, rt: 1f, rd: 1c, shmt: 00, funct: 2a}
        // and {opcode: 00, rs: 17, rt: 14, rd: 07, shmt: 00, funct: 24}
        long[] rwords = new long[]{ 0x027b4822, 0x030a0820, 0x00dfe02a, 0x02f43824};
        _deasmble_test(rwords);
    }
}
