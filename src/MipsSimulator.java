import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;

public class MipsSimulator {
    private static class MipsState{
        int[] registers = new int[32];
        ByteBuffer memory;
        int pc = 0;
        boolean running = true;

        public MipsState(Path path) throws IOException {
            List<String> hexLines = Files.readAllLines(path);
            ByteArrayOutputStream bos = new ByteArrayOutputStream();

            //converts hex data to byte data in big endien
            for (String line : hexLines) {
                for (int i = line.length(); i > 0; i -= 2) {
                    int start = Math.max(i-2, 0);
                    String hexByte = line.substring(start, i);
                    bos.write(Integer.parseInt(hexByte, 16));
                }
            }
            memory = ByteBuffer.wrap(bos.toByteArray());
        }

        public String readCString(int startAddress) {
            memory.position(startAddress);

            //reads until null
            while(memory.hasRemaining() && memory.get() != 0){}

            int endAddr = memory.position() -1;
            byte[] stringBytes = new byte[endAddr  - startAddress];

            memory.position(startAddress);
            memory.get(stringBytes);

            return new String(stringBytes, StandardCharsets.UTF_8);
        }
    }



    private void handleSyscall(MipsState state){
        //implement read write exit syscall
    }

    public MipsSimulator(String[] args) throws IOException {
        List<String> text =  Files.readAllLines(Paths.get(args[0]));
        MipsState state = new MipsState(Path.of(args[1]));

        System.out.print(state.readCString(0));
        System.out.println(state.readCString(21));


        for(String line : text){
            long num = Long.parseLong(line, 16);
            MipsMap word = Disassembler.disassemble(num);
            System.out.println(word);
        }

    }
}
