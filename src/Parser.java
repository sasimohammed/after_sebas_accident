import java.util.Arrays;

public class Parser {
    private String commandName;
    private String[] args;

    public boolean parse(String input) {
        input = input.trim();
        if (input.isEmpty()) return false;

        String[] parts = input.split("\\s+");
        commandName = parts[0];
        args = Arrays.copyOfRange(parts, 1, parts.length);
        return true;
    }

    public String getCommandName() {
        return commandName;
    }

    public String[] getArgs() {
        return args;
    }
}