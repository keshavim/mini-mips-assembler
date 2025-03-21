
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
        return Long.parseLong(input) & 0xFFFF;

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
        LI(-1, Types.Immediate),
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
    }

    public enum Registers {
        $zero(0),
        $at(1),
        $v0(2),
        $v1(3),
        $a0(4),
        $a1(5),
        $a2(6),
        $a3(7),
        $t0(8),
        $t1(9),
        $t2(10),
        $t3(11),
        $t4(12),
        $t5(13),
        $t6(14),
        $t7(15),
        $s0(16),
        $s1(17),
        $s2(18),
        $s3(19),
        $s4(20),
        $s5(21),
        $s6(22),
        $s7(23),
        $t8(24),
        $t9(25),
        $k0(26),
        $k1(27),
        $gp(28),
        $sp(29),
        $fp(30),
        $ra(31);

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
