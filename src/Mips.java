
//contains static variables and methods to be used by the assembler
//these are consts
public class Mips {

    public static final int DATA_ADDRESS = 0x10010000;
    public static final int TEXT_ADDRESS = 0x00400000;


    public static long parseNumber(String input) {
        input = input.toLowerCase().trim();
        long res = 0;
        if (input.startsWith("0b")) {
            return Long.parseLong(input.substring(2), 2);
        }
        if (input.startsWith("0x")) {
            return Long.parseLong(input.substring(2), 16);
        }
        return Long.parseLong(input);

    }

    public enum Types{
        Register,
        Immediate,
        Branch,
        Jump,
        Load,
        Store,
        Special,
        Psudo;

    }

    public enum Instructions {
        ADD(0x20, Types.Register),
        ADDU(0x21, Types.Register),
        ADDIU(0x9, Types.Immediate),
        AND(0x24, Types.Register),
        ANDI(0xC, Types.Immediate),
        BEQ(0x4, Types.Branch),
        BNE(0x5, Types.Branch),
        J(0x2, Types.Jump),
        LUI(0xF, Types.Load),
        LW(0x23, Types.Store),
        OR(0x25, Types.Register),
        ORI(0xD, Types.Immediate),
        SLT(0x2A, Types.Register),
        SUB(0x22, Types.Register),
        SW(0x2B, Types.Store),
        SYSCALL(0xC, Types.Special),
        LI(-1, Types.Immediate, 2),
        LA(-1, Types.Load, 2),
        MOVE(-1, Types.Register),
        BLT(-1, Types.Branch, 2);

        private final  long value;
        private final int ins_num;
        private final Types type;

        Instructions(long opcode, Types type) {
            this.value = opcode;
            this.type = type;
            this.ins_num = 1;
        }
        Instructions(long opcode, Types type, int ins_num) {
            this.value = opcode;
            this.type = type;
            this.ins_num = ins_num;
        }

        public  long getValue() {
            return value;
        }

        public Types getType() {
            return type;
        }

        public int getInsructionNum() {
            return ins_num;
        }

        public static Instructions getByName(String name) {

            return valueOf(name.toUpperCase());

        }
        public static String getNameFromValue(int value) {
            for (Instructions enumConstant : Instructions.values()) {
                if (enumConstant.getValue() == value) {
                    return enumConstant.name().toLowerCase();
                }
            }
            return null; // Return null if no matching enum is found
        }
    }

    public enum Registers {
        $zero(0x00),
        $at(0x01),
        $v0(0x02),
        $v1   (0x03),
        $a0   (0x04),
        $a1   (0x05),
        $a2   (0x06),
        $a3   (0x07),
        $t0   (0x08),
        $t1   (0x09),
        $t2   (0x0A),
        $t3   (0x0B),
        $t4   (0x0C),
        $t5   (0x0D),
        $t6   (0x0E),
        $t7   (0x0F),
        $s0   (0x10),
        $s1   (0x11),
        $s2   (0x12),
        $s3   (0x13),
        $s4   (0x14),
        $s5   (0x15),
        $s6   (0x16),
        $s7   (0x17),
        $t8   (0x18),
        $t9   (0x19),
        $k0   (0x1A),
        $k1   (0x1B),
        $gp   (0x1C),
        $sp   (0x1D),
        $fp   (0x1E),
        $ra   (0x1F);

        private final long value;

        Registers(long value) {
            this.value = value;
        }

        public long getValue() {
            return value;
        }

        public static Registers fromString(String register) {
            return valueOf(register);
        }
    }
}
