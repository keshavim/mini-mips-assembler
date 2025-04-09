public class Disassembler {

    public static String disassemble(long word){
        return decodeMipsInstruction(word);

    }

    private static String decodeMipsInstruction(long word){
        int opcode = (int)((word >>> 26) & 0x3F);

        if(opcode == 0x00){
            return decodeRtype(word);
        }
        if(opcode == 0x02){
            return decodeJtype(word);
        }

        return decodeItype(word);
    }
    private static String decodeRtype(long word){
        int funct = (int)(word & 0x3F); // Last 6 bits for R-type

        if(funct == 0x0c){
            return "syscall {opcode: 00, code: 000000, funct: 0c}";
        }

        int rs = (int)(word >>> 21) & 0x1F;
        int rt = (int)(word >>> 16) & 0x1F;
        int rd = (int)(word >>> 11) & 0x1F;
        String memonic = Mips.Instructions.getNameFromValue(funct);
        return String.format("%s {opcode: 00, rs: %02x, rt: %02x, rd: %02x, shmt: 00, funct: %02x}",
                memonic, rs, rt, rd, funct);
    }
    private static String decodeItype(long word){

        int opcode = (int)(word >>> 26) & 0x3F; // Extract opcode (6 bits)
        int rs = (int)(word >>> 21) & 0x1F;     // Extract rs (5 bits)
        int rt = (int)(word >>> 16) & 0x1F;     // Extract rt (5 bits)
        int immediate = (int)(word & 0xFFFF);    // Extract immediate (16 bits)
        String memonic = Mips.Instructions.getNameFromValue(opcode);


        return String.format("%s {opcode: %02x, rs(base): %02x, rt: %02x, immediate(offset): %04x}",
                memonic, opcode, rs, rt, immediate);
    }
    private static String decodeJtype(long word){
        int address = (int)(word & 0x03FFFFFF); // Last 26 bits for J-type
        int opcode = (int)(word >>> 26) & 0x3F; // Extract opcode (6 bits)
        String memonic = Mips.Instructions.getNameFromValue(opcode);
        return String.format("%s {opcode: %02x, index: %07x}", memonic, opcode, address);
    }
}
