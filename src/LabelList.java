import java.util.ArrayList;

public class LabelList {


    private ArrayList<Label> labels;

    LabelList(){
        labels = new ArrayList<>();
    }

    public void add(String name, long address){
        labels.add(new Label(name, address));
    }

    public Label getLabel(String name) {
        for (Label label : labels) {
            if(label.name.equals(name)){
                return label;
            }
        }
        return null;
    }
    public Label getLabel(long address) {
        for (Label label : labels) {
            if(label.address == address){
                return label;
            }
        }
        return null;
    }

    @Override
    public String toString() {
        return labels.toString();
    }

    public static class Label{
        private String name;
        private long address;

        Label(String name, long address){
            this.name = name;
            this.address = address;
        }

        public long getAddress() {
            return address;
        }

        public String getName() {
            return name;
        }

        @Override
        public String toString() {
            return String.format("{%s = 0x%08x}",name, address);
        }
    }


}
