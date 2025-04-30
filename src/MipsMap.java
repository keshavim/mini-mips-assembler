import java.util.HashMap;
import java.util.Map;

public class MipsMap {
    Map<String, Integer> map = new HashMap<String, Integer>();
    Mips.Types type;

    public MipsMap(Mips.Types type, Map<String, Integer> map) {
        this.type = type;
        this.map = map;
    }

    public Map<String, Integer> getMap() {
        return map;
    }
    public void setMap(Map<String, Integer> map) {
        this.map = map;
    }
    public String getName(){
        return type == Mips.Types.Register ?
                Mips.Instructions.getNameFromValue(map.get("funct")) :
                Mips.Instructions.getNameFromValue(map.get("opcode"));
    }
    public Mips.Instructions getInstruction(){
        return Mips.Instructions.getByName(getName());
    }

    @Override
    public String toString() {
        if(this.type == Mips.Types.Register){
            return String.format("%s {opcode: 00, rs: %02x, rt: %02x, rd: %02x, shmt: 00, funct: %02x}",
                    getName(), map.get("rs"), map.get("rt"), map.get("rd"), map.get("funct"));
        }
        else if(this.type == Mips.Types.Jump){
            return String.format("%s {opcode: %02x, index: %07x}", getName(), map.get("opcode"), (map.get("address") >> 2));

        }
        else if (this.type == Mips.Types.Special){
            return "syscall {opcode: 00, code: 000000, funct: 0c}";
        }
        else{
            return String.format("%s {opcode: %02x, rs(base): %02x, rt: %02x, immediate(offset): %04x}",
                    getName(),  map.get("opcode"),  map.get("rs"),  map.get("rt"),  map.get("immediate"));
        }
    }
}
