import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Objects;

//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
public class Main {



    public static void main(String[] args)  {

        System.out.println((100000 >> 16) & 0xFFFF);

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