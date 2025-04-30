import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;
import java.util.Map;
import java.util.Scanner;

public class MipsSimulator {

    Scanner scan =  new Scanner(System.in);

    private static class MipsState{
        int[] registers = new int[32];
        ByteBuffer memory = ByteBuffer.allocate(4096);
        int stack_start = 0;
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

            memory.put(bos.toByteArray());
            memory.position(0);
            stack_start = memory.remaining();
            registers[29] = 4092;
            registers[1] = Mips.DATA_ADDRESS;
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
        int v0 = state.registers[2];
        int a0 = state.registers[4];


        switch (v0){
            case 1 ->{
                System.out.print(a0);
            }
            case 4->{
                System.out.print(state.readCString(a0 - Mips.DATA_ADDRESS));
            }
            case 5->{
                state.registers[2] = scan.nextInt();
                scan.nextLine();
            }
            case 10 -> {
                state.running = false;
            }
        }
    }

    private void handleInstruction(MipsState state, MipsMap word){
        if(word.type == Mips.Types.Special){
            handleSyscall(state);
            return;
        }
        Map<String, Integer> m = word.getMap();
        switch (word.getInstruction()){
            case ADD, ADDU -> {
                // ADD/ADDU: rd = rs + rt (no sign extension needed for operands)
                int rs = state.registers[m.get("rs")];
                int rt = state.registers[m.get("rt")];
                int result = rs + rt;

                state.registers[m.get("rd")] = result;

            }
            case ADDIU, ADDI -> {
                // ADDI/ADDIU: rt = rs + immediate (sign-extend immediate)
                int rs = state.registers[m.get("rs")];
                int immediate = m.get("immediate");

                // Sign-extend 16-bit immediate to 32 bits
                if (immediate > Short.MAX_VALUE) {
                    immediate |= 0xFFFF_0000;  // Set upper 16 bits to 1s
                }

                int result = rs + immediate;

                state.registers[m.get("rt")] = result;
            }
            case SUB -> {
                int val = m.get("rt");
                if (val > Short.MAX_VALUE){
                    val -= Short.MAX_VALUE*2+2;
                }
                state.registers[m.get("rd")] = state.registers[m.get("rs")] - val;
            }
            case AND -> {
                state.registers[m.get("rd")] = state.registers[m.get("rs")] & state.registers[m.get("rt")];
            }
            case ANDI -> {
                state.registers[m.get("rt")] = state.registers[m.get("rs")] & m.get("immediate");
            }
            case OR -> {
                state.registers[m.get("rd")] = state.registers[m.get("rs")] | state.registers[m.get("rt")];

            }
            case ORI -> {
                state.registers[m.get("rt")] = state.registers[m.get("rs")] | m.get("immediate");

            }
            case BEQ -> {
                if(state.registers[m.get("rt")] == state.registers[m.get("rs")]){
                    int offset = m.get("immediate") < (state.pc * 4 + Mips.TEXT_ADDRESS)+4 ?
                            m.get("immediate") : m.get("immediate") - Short.MAX_VALUE*2+2;
                    state.pc += offset;
                }
            }
            case BNE -> {
                if(state.registers[m.get("rt")] != state.registers[m.get("rs")]){
                    int offset = m.get("immediate") > (state.pc * 4 + Mips.TEXT_ADDRESS)+4 ?
                            m.get("immediate") : m.get("immediate") - Short.MAX_VALUE*2+2;
                    state.pc += offset;
                }
            }

            case LW -> {
                int pos =  state.registers[m.get("rs")] + m.get("immediate");

                // Read 4 bytes in little-endian order
                ByteBuffer buffer = ByteBuffer.wrap(state.memory.array(), pos, 4)
                        .order(ByteOrder.LITTLE_ENDIAN);
                state.registers[m.get("rt")] = buffer.getInt();
            }
            case SW -> {
                int pos = state.registers[m.get("rs")] + m.get("immediate");
                int value = state.registers[m.get("rt")];

                // Store all 4 bytes (little-endian)
                for(int i = 0; i < 4; i++){
                    state.memory.put(pos + i, (byte) ((value >> (i*8)) & 0xFF));
                }
            }
            case J -> {
                state.pc = (m.get("address") - Mips.TEXT_ADDRESS)/4 - 1;
            }
            case LUI -> {
                int immediate = m.get("immediate") & 0xFFFF; // Ensure 16-bit unsigned value
                state.registers[m.get("rt")] = immediate << 16; // Shift left and clear lower bits
            }

            case SLT -> {
                state.registers[m.get("rd")] = state.registers[m.get("rs")] < state.registers[m.get("rt")] ? 1 : 0;
            }

        }
    }

    public MipsSimulator(String[] args) throws IOException {
        List<String> text =  Files.readAllLines(Paths.get(args[0]));
        MipsState state = new MipsState(Path.of(args[1]));

        while(state.running){
            long num = Long.parseLong(text.get(state.pc), 16);
            MipsMap word = Disassembler.disassemble(num);
           // System.out.println(word);
            handleInstruction(state, word);
            state.pc++;
        }
    }
}
