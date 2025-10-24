import java.util.*;

public class Parser {
    private String commandName;
    private String[] args;

    private static final Set<String> VALID_COMMANDS = Set.of(
            "pwd",
            "cd",
            "ls",
            "mkdir",
            "rmdir",
            "touch",
            "cp",
            "cp -r",
            "rm",
            "cat",
            "wc",
            "zip",
            "unzip"
    );

    public boolean parse(String input) {
        if (input == null || input.trim().isEmpty()) return false;


        input = input.replaceAll(">>", " >> ");
        input = input.replaceAll(">", " > ");

        String[] tokens = input.trim().split("\\s+");
        commandName = null;


        for (int i = 0; i < tokens.length; i++) {
            if (i < tokens.length - 1 && tokens[i].equals("cp") && tokens[i + 1].equals("-r")) {
                commandName = "cp -r";
                break;
            }
        }


        if (commandName == null) {
            for (String token : tokens) {
                if (VALID_COMMANDS.contains(token)) {
                    commandName = token;
                    break;
                }
            }
        }


        if (commandName == null) {
            System.out.println("Unknown command!");
            return false;
        }


        List<String> argList = new ArrayList<>();
        for (int i = 0; i < tokens.length; i++) {
            if (commandName.equals("cp -r") && i < tokens.length - 1 && tokens[i].equals("cp") && tokens[i + 1].equals("-r")) {
                i++;
                continue;
            }
            if (!tokens[i].equals(commandName) && !tokens[i].equals("-r") && !tokens[i].equals(">") && !tokens[i].equals(">>")) {
                argList.add(tokens[i]);
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
