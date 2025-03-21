import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

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


//



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
}
