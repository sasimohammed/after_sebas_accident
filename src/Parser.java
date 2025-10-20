import java.util.Arrays;
import java.util.*;

public class Parser {
    private String commandName;
    private String[] args;

    private static final Set<String> VALID_COMMANDS = Set.of("zip", "unzip", ">", ">>", "wc","rmdir","touch","cp","pwd");


    public boolean parse(String input) {
        if (input == null || input.trim().isEmpty()) return false;

        String[] tokens = input.trim().split("\\s+");
        commandName = null;

        // Find the command name (even if it's in the middle)
        for (String token : tokens) {
            if (VALID_COMMANDS.contains(token)) {
                commandName = token;
                break;
            }
        }

        if (commandName == null) {
            System.out.println("Unknown command!");
            return false;
        }

        // Build args (everything except the command itself)
        List<String> argList = new ArrayList<>();
        for (String token : tokens) {
            if (!token.equals(commandName)) {
                argList.add(token);
            }
        }

        args = argList.toArray(new String[0]);
        return true;
    }


    public String getCommandName() {
        return commandName;
    }

    public String[] getArgs() {
        return args;
    }


}