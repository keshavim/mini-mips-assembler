import java.io.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class FileAssembler {

    private Assembler asm;
    private LabelList labelList;
    private long pc;
    private String outputdir;


    FileAssembler(){
        asm = new Assembler();
        labelList = new LabelList();
        pc = 0;
        outputdir = null;
    }

    public void setOutputdir(String outputdir) {
        if(!outputdir.endsWith("/")){
            outputdir += "/";
        }
        this.outputdir = outputdir;
    }

    public void assembleFile(String file){
        int i = 0;
        if (file.contains("/")) {
            i = file.lastIndexOf('/') +1;
        }
        else if(file.contains("\\")){
            i = file.lastIndexOf('\\');
        }



        String name = file.substring(i);
        String dir = file.substring(0, i);
        if(outputdir == null)
            outputdir = dir;
        assemble_data(dir, name);
        assemble_text(dir, name);
    }

    private void assemble_text(String dir, String name){
        try {
            BufferedReader asmFile = new BufferedReader(new FileReader(dir+name));

            BufferedWriter textFile = new BufferedWriter(new FileWriter(outputdir+ "text_" +name));

            String line;
            pc = Mips.TEXT_ADDRESS;

            while(!asmFile.readLine().equals(".text"));

            while((line = asmFile.readLine()) != null){
                if(line.contains("#"))
                    line = line.substring(0, line.indexOf('#'));
                if(line.isBlank())
                    continue;
                if(line.contains(":"))
                    continue;
                line = line.trim();



                //convert the label in an instruction to an apropreate number
                String lastWord = line.substring(line.lastIndexOf(' ')+1);
                LabelList.Label label;
                String new_line = line;
                if((label = labelList.getLabel(lastWord)) != null){
                    String lname = line.contains(" ") ? line.substring(0, line.indexOf(' ')) : line;

                    Mips.Instructions instructions = Mips.Instructions.getByName(lname);

                    switch (instructions.getType()){
                        case Branch -> {
                            String start = line.substring(0, line.lastIndexOf(' '));
                            long shift = (pc + (instructions.getInsructionNum()* 4L));
                            long offset = ((label.getAddress() - shift) / 4);
                            new_line = String.format("%s 0x%08x", start,  offset & 0xFFFF);

                        }
                        case Load -> {
                            String start = line.substring(0, line.lastIndexOf(' '));
                            long address = label.getAddress();
                            new_line = String.format("%s 0x%08x", start,  address);

                        }
                        case Jump -> {
                            String start = line.substring(0, line.lastIndexOf(' '));
                            long address = label.getAddress() >> 2;
                            new_line = String.format("%s 0x%08x", start,  address);
                        }

                    }


                }




                long[] assmbled_line = asm.assemble_word(new_line);
                for(int i = 0; i < assmbled_line.length; i++){


                    textFile.write(String.format("%08x%n",assmbled_line[i]));
//                    if(i == 0){
//                        textFile.write(line);
//                    }

                    pc += 4;
                }






            }
            textFile.flush();
            textFile.close();
            asmFile.close();

        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }


//creates data file and add the labels
    private void assemble_data(String dir, String name) {
        try {
            BufferedReader asmFile = new BufferedReader(new FileReader(dir+name));
            Files.createDirectories(Path.of(outputdir));

            BufferedWriter dataFile = new BufferedWriter(new FileWriter(outputdir +"data_"+name));

            String line;
            pc = Mips.DATA_ADDRESS;
            int data_buffer = 0;
            int data_shift = 0;
            Pattern pattern = Pattern.compile("\"[^\"]*\"|\\S+");
            while((line = asmFile.readLine()) != null){
                line = line.trim();

                if(line.equals(".data") || line.isBlank()){
                    continue;
                }
                if(line.equals(".text")){
                    break;
                }

                //spliting each part of the line
                Matcher matcher = pattern.matcher(line);
                List<String> result = new ArrayList<>();
                while (matcher.find()) {
                    result.add(matcher.group());
                }

                String label = result.get(0).substring(0, result.get(0).length() -1);
                labelList.add(label, pc);

                if(result.get(1).equals(".asciiz")) {
                    String data_str = result.get(2).substring(1, result.get(2).length() - 1);
                    data_str += '\0';
                    int escapes = 0;
                    //convert data to hex and write in 32 bit buffer
                    for (int i = 0; i < data_str.length(); i++) {
                        char ch = data_str.charAt(i);
                        if (data_shift == 4) {
                            dataFile.write(String.format("%08x%n", data_buffer)); // Write 8 hex digits
                            data_shift = 0;
                            data_buffer = 0;
                        }
                        //will need to update for more escape characters
                        if(ch == '\\'){
                            i++;
                            if(data_str.charAt(i) == 'n'){
                                ch = '\n';
                            }
                            escapes++;
                        }


                        data_buffer |= (ch & 0xFF) << (data_shift * 8);
                        data_shift++;
                    }

                    pc += data_str.length() - escapes;


                }
            }
            //leftover data
            if (data_shift > 0) {
                dataFile.write(String.format("%08x%n", data_buffer));
            }


            dataFile.flush();
            dataFile.close();

            addTextLabels(asmFile);

            asmFile.close();

        } catch (IOException e) {
            throw new RuntimeException(e);
        }

    }

    private void addTextLabels(BufferedReader file) throws IOException {
        String line;
        pc = Mips.TEXT_ADDRESS;
        while((line = file.readLine()) != null){
            line = line.trim();

            if(line.contains("#")){
                line = line.substring(0, line.indexOf('#'));

            }

            if(line.isBlank())
                continue;

            //is label
            if(line.endsWith(":")){
                if(line.contains(" ")){
                    throw new RuntimeException(pc + line + "  Label can not have spaces");
                }
                labelList.add(line.substring(0, line.length()-1), pc);
                continue;
            }

            String name = line.contains(" ") ? line.substring(0, line.indexOf(' ')) : line;

            Mips.Instructions instructions = Mips.Instructions.getByName(name);
            pc += instructions.getInsructionNum() * 4L;
        }

    }


}
