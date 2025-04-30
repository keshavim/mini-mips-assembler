import javax.lang.model.type.NullType;
import java.util.HashMap;

public class Disassembler {

    public static MipsMap disassemble(long word){
        return decodeMipsInstruction(word);

    }

    private static MipsMap decodeMipsInstruction(long word){
        int opcode = (int)((word >>> 26) & 0x3F);

        if(opcode == 0x00){
            return decodeRtype(word);
        }
        if(opcode == 0x02){
            return decodeJtype(word);
        }

        return decodeItype(word);
    }
    private static MipsMap decodeRtype(long word){
        int funct = (int)(word & 0x3F); // Last 6 bits for R-type

        if(funct == 0x0c){
            return new MipsMap(Mips.Types.Special, null);
        }

        int rs = (int)(word >>> 21) & 0x1F;
        int rt = (int)(word >>> 16) & 0x1F;
        int rd = (int)(word >>> 11) & 0x1F;

        return new MipsMap(Mips.Types.Register, new HashMap<>(){{
            put("funct",funct);
            put("rs",rs);
            put("rt",rt);
            put("rd",rd);
        }});
    }
    private static MipsMap decodeItype(long word){

        int opcode = (int)(word >>> 26) & 0x3F; // Extract opcode (6 bits)
        int rs = (int)(word >>> 21) & 0x1F;     // Extract rs (5 bits)
        int rt = (int)(word >>> 16) & 0x1F;     // Extract rt (5 bits)
        int immediate = (int)(word & 0xFFFF);    // Extract immediate (16 bits)

        return new MipsMap(Mips.Types.Immediate, new HashMap<>(){{
            put("opcode",opcode);
            put("rs",rs);
            put("rt",rt);
            put("immediate",immediate);
        }});
    }
    private static MipsMap decodeJtype(long word){
        int address = (int)(word & 0x03FFFFFF); // Last 26 bits for J-type
        int opcode = (int)(word >>> 26) & 0x3F; // Extract opcode (6 bits)
        int absoluteAddress = (address << 2) | (Mips.TEXT_ADDRESS & 0xF0000000);
        return new MipsMap(Mips.Types.Jump, new HashMap<>(){{
            put("opcode",opcode);
            put("address",absoluteAddress);
        }});
    }
}
