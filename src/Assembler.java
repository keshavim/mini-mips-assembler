import java.util.Objects;


public class Assembler {

    String delim;

    Assembler(){
        if(System.getProperty("os.name").toLowerCase().contains("win")){
            delim = "[`\\s,#]+";
        }
        else{
            delim = "[\\\\s,#]+";
        }
    }
    //for tests
    public String[] split_line(String line){
        return line.split(delim);
    }

    public String disassemble_word(long word){
        return Disassembler.disassemble(word);
    }

    public long[] assemble_word(String word){
        String[] word_split = word.split(delim);
        Mips.Instructions instruction = Mips.Instructions.getByName(word_split[0]);
        //normal instructions
        if(instruction.getValue() != -1){
            return new long[]{convertInstruction(word_split)};
        }
        //psudo instructions
        long address = 0;
        switch (instruction){
            case LI:
                address = Mips.parseNumber(word_split[2]);
                return new long[]{

                        convertPsudo("lui $at, 0x%08x", (address >> 16) & 0xFFFF),
                        convertPsudo("ori %s, $at, 0x%08x",word_split[1], address & 0xFFFF)
                };
            case LA:
                address = Mips.parseNumber(word_split[2]);
                return new long[]{
                        convertPsudo("lui $at, 0x%08x", (address >> 16) & 0xFFFF),
                        convertPsudo("ori %s $at, 0x%08x",word_split[1], address & 0xFFFF)
                };
            case BLT:
                return new long[]{
                        convertPsudo("slt $at, %s, %s", word_split[1], word_split[2]),
                        convertPsudo("bne $at, $zero, %s", word_split[3])
                };
            case MOVE:
                return new long[]{
                        convertPsudo("addu %s, $zero, %s", word_split[1], word_split[2])
                };

        }
        return null;

    }


    
    private long convertInstruction(String[] instr){
        Mips.Instructions instruction = Mips.Instructions.getByName(instr[0]);
        return switch (instruction.getType()) {
            case Register -> convertRegister(0, instr[2], instr[1], instr[3], 0, instr[0]);
            case Immediate -> convertImmediate(instruction.getValue(), instr[2], instr[1], instr[3]);
            case Branch -> convertImmediate(instruction.getValue(), instr[1], instr[2], instr[3]);
            case Store -> convertStore(instruction.getValue(), instr[1], instr[2]);
            case Load -> convertLoad(instruction.getValue(), instr[1], instr[2]);
            case Jump -> convertJump(instruction.getValue(), instr[1]);
            case Special -> instruction.getValue();
            default -> -1;
        };
    }

    private long convertPsudo(String format, Object... args){
        if (format == null || args == null) {
            throw new IllegalArgumentException("Format and arguments cannot be null");
        }
        String new_instrs = String.format(format, args);
        String[] split = new_instrs.split(delim);
        if (split.length == 0) {
            throw new IllegalArgumentException("Invalid instruction format");
        }
        return convertInstruction(split);
    }

    //helper converts for instructions
    private  long convertRegister(long op, String rs, String rd, String rt, long sh, String funct){
        return (op << 26) |
                (Mips.Registers.fromString(rs).getValue() << 21) |
                (Mips.Registers.fromString(rt).getValue() << 16) |
                (Mips.Registers.fromString(rd).getValue() << 11) |
                (sh << 6) |
                Objects.requireNonNull(Mips.Instructions.getByName(funct)).getValue();
    }
    private  long convertImmediate(long op, String rs, String rt, String im) {
        return (op << 26) |
                (Mips.Registers.fromString(rs).getValue() << 21) |
                (Mips.Registers.fromString(rt).getValue() << 16) |
                Mips.parseNumber(im) & 0xFFFF;

    }

    private  long convertLoad(long op, String rt, String im){
        return (op << 26) |
                (0 << 21) |
                (Mips.Registers.fromString(rt).getValue() << 16) |
                Mips.parseNumber(im) & 0xFFFF;

    }
    private  long convertStore(long op, String rt, String im){
        String[] temp = im.split("[()]+");
        String offset = temp[0].isEmpty() ? "0" : temp[0];
        return convertImmediate(op, temp[1], rt, offset);

    }
    private  long convertJump(long op, String im){
        return (op << 26) |
                Mips.parseNumber(im) & 0xFFFF;

    }
}
