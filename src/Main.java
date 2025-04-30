
import java.io.IOException;
import java.util.ArrayList;


//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
public class Main {



    public static void main(String[] args)  {


        if(args[0].equals("--run")){
            String datafile, textfile;
            if(args[1].contains(".data")){
                datafile = args[1];
                textfile = args[2];
            }
            else{
                datafile = args[2];
                textfile = args[1];
            }
            try{
                MipsSimulator sim = new MipsSimulator(datafile, textfile);
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
            return;
        }

        //old stuff
        if(args[0].equals("-d") || args[0].equals("--dasm")) {
            String prefix = "";
            if (!args[1].substring(0, 2).equalsIgnoreCase("0x")) {
                prefix = "0x";
            }

            for(int i = 0; i < args.length; i++){
                MipsMap s = Disassembler.disassemble(Mips.parseNumber(prefix + args[1]));

                System.out.println(s);
            }

            return;
        }

        ArrayList<String> asmfiles = new ArrayList<>();
        FileAssembler fasm = new FileAssembler();
        for(int i = 0; i < args.length; i++){
            if(args[i].equals("-o")){
                i++;
                fasm.setOutputdir(args[i]);
                continue;
            }
            asmfiles.add(args[i]);
        }
        asmfiles.forEach(fasm::assembleFile);




    }
}