import java.io.*;
import java.nio.file.*;
import java.nio.file.attribute.FileTime;
import java.util.*;
import java.util.zip.*;

public class Terminal {
    private Path currentPath = Paths.get(System.getProperty("user.dir"));
    private final Parser parser = new Parser();

    public void pwd() {
        System.out.println(currentPath.toAbsolutePath().normalize());
    }

    public void cd(String[] args) {
        Path newPath;

        if (args.length == 0) {
            String home = System.getProperty("user.home");
            newPath = Paths.get(home);
        }
        else if (args[0].equals("..")) {
            newPath = currentPath.getParent();
            if (newPath == null) {
                System.out.println("Already at the root directory.");
                return;
            }
        }
        else {
            newPath = currentPath.resolve(args[0]).normalize();
        }

        if (!Files.exists(newPath) || !Files.isDirectory(newPath)) {
            System.out.println("Directory not found: " + newPath);
            return;
        }
        currentPath = newPath;
    }

    public void ls() throws IOException {

        List<Path> entries = new ArrayList<>();
        try (DirectoryStream<Path> dirContent = Files.newDirectoryStream(currentPath)) {
            for (Path entry : dirContent) {
                entries.add(entry);
            }
        }


        entries.sort(Comparator.comparing(path -> path.getFileName().toString()));

        for (Path entry : entries) {
            System.out.println(entry.getFileName());
        }
    }

    public void mkdir(String[] args) throws IOException {

        if (args.length < 1) {
            System.out.println("Usage: mkdir <directory1> [directory2 ...]");
            return;
        }

        for (String dirName : args) {
            Path newDir;


            if (dirName.startsWith("/") || dirName.contains(File.separator)) {
                newDir = Paths.get(dirName).normalize();
            } else {
                newDir = currentPath.resolve(dirName);
            }

            if (Files.exists(newDir)) {
                System.out.println("Directory already exists: " + dirName);
                continue;
            }

            Files.createDirectories(newDir);
            System.out.println("Created directory: " + newDir);
        }
    }

    public void rmdir(String[] args) throws IOException {
        if (args.length != 1) {
            System.out.println("Usage: rmdir <dirname> or rmdir *");
            return;
        }


        if (args[0].equals("*")) {
            try (DirectoryStream<Path> stream = Files.newDirectoryStream(currentPath)) {
                int removedCount = 0;
                for (Path p : stream) {
                    if (Files.isDirectory(p)) {
                        try (DirectoryStream<Path> inner = Files.newDirectoryStream(p)) {
                            boolean isEmpty = !inner.iterator().hasNext();
                            if (isEmpty) {
                                Files.delete(p);
                                removedCount++;
                            }
                        } catch (IOException e) {
                            System.out.println("Cannot access directory: " + p);
                        }
                    }
                }
                System.out.println("Removed " + removedCount + " empty directories");
            } catch (IOException e) {
                System.out.println("Error accessing current directory: " + e.getMessage());
            }
            return;
        }


        Path targetDir = currentPath.resolve(args[0]);
        if (!Files.exists(targetDir)) {
            System.out.println("Directory not found: " + args[0]);
            return;
        }
        if (!Files.isDirectory(targetDir)) {
            System.out.println("Not a directory: " + args[0]);
            return;
        }

        try (DirectoryStream<Path> stream = Files.newDirectoryStream(targetDir)) {
            boolean isEmpty = !stream.iterator().hasNext();
            if (!isEmpty) {
                System.out.println("Directory not empty: " + args[0]);
                return;
            }
        }

        Files.delete(targetDir);
        System.out.println("Removed directory: " + args[0]);
    }

    public void touch(String[] args) throws IOException {

        if (args.length != 1) {
            System.out.println("Usage: touch <filename>");
            return;
        }

        Path newFile;
        if (args[0].startsWith("/") || args[0].contains(File.separator)) {
            newFile = Paths.get(args[0]).normalize();
        } else {
            newFile = currentPath.resolve(args[0]);
        }

        if (Files.exists(newFile)) {
            Files.setLastModifiedTime(newFile, FileTime.fromMillis(System.currentTimeMillis()));
        } else {

            Files.createDirectories(newFile.getParent());
            Files.createFile(newFile);
        }
    }

    public void cp(String[] args) throws IOException {
        if (args.length != 2) {
            System.out.println("Usage: cp <sourceFile> <destinationFile>");
            return;
        }

        Path source = currentPath.resolve(args[0]);
        Path destination = currentPath.resolve(args[1]);

        if (!Files.exists(source)) {
            System.out.println("Source file not found: " + args[0]);
            return;
        }
        if (Files.isDirectory(source)) {
            System.out.println("Source is a directory. Use 'cp -r' for directories.");
            return;
        }
        if (Files.isDirectory(destination)) {
            System.out.println("Destination is a directory. Specify a filename.");
            return;
        }

        Files.copy(source, destination, StandardCopyOption.REPLACE_EXISTING);
        System.out.println("Copied: " + args[0] + " to " + args[1]);
    }

    public void copyDirectoryRecursive(Path source, Path destination) throws IOException {
        Files.walk(source)
                .forEach(sourcePath -> {
                    Path destinationPath = destination.resolve(source.relativize(sourcePath));
                    try {
                        if (Files.isDirectory(sourcePath)) {
                            if (!Files.exists(destinationPath)) {
                                Files.createDirectories(destinationPath);
                            }
                        } else {
                            Files.copy(sourcePath, destinationPath, StandardCopyOption.REPLACE_EXISTING);
                        }
                    } catch (IOException e) {
                        System.out.println("Error copying: " + sourcePath);
                    }
                });
    }

    public void cpr(String[] args) throws IOException {

        if (args.length != 2) {
            System.out.println("Usage: cp -r <sourceDirectory> <destinationDirectory>");
            return;
        }

        Path source = currentPath.resolve(args[0]);
        Path destination = currentPath.resolve(args[1]);

        if (!Files.exists(source)) {
            System.out.println("Source directory not found: " + args[0]);
            return;
        }
        if (!Files.isDirectory(source)) {
            System.out.println("Source is not a directory: " + args[0]);
            return;
        }
        if (Files.exists(destination) && !Files.isDirectory(destination)) {
            System.out.println("Destination exists but is not a directory: " + args[1]);
            return;
        }


        if (!Files.exists(destination)) {
            Files.createDirectories(destination);
        }

        copyDirectoryRecursive(source, destination);
        System.out.println("Copied directory: " + args[0] + " to " + args[1]);
    }

    public void rm(String[] args) throws IOException {
        // Takes 1 argument which is a filename in current directory
        if (args.length != 1) {
            System.out.println("Usage: rm <filename>");
            return;
        }

        Path file = currentPath.resolve(args[0]);
        if (!Files.exists(file)) {
            System.out.println("File not found: " + args[0]);
            return;
        }
        if (Files.isDirectory(file)) {
            System.out.println("Cannot remove directory with rm: " + args[0]);
            return;
        }

        Files.delete(file);
        System.out.println("Removed: " + args[0]);
    }

    public void cat(String[] args) throws IOException {
        // Takes 1 argument OR takes 2 arguments and concatenates
        if (args.length < 1 || args.length > 2) {
            System.out.println("Usage: cat <file1> [file2]");
            return;
        }

        if (args.length == 1) {

            Path file = currentPath.resolve(args[0]);
            if (!Files.exists(file)) {
                System.out.println("File not found: " + args[0]);
                return;
            }
            if (Files.isDirectory(file)) {
                System.out.println("Is a directory: " + args[0]);
                return;
            }

            List<String> lines = Files.readAllLines(file);
            for (String line : lines) {
                System.out.println(line);
            }
        } else {
            // Two files - concatenate and print
            Path file1 = currentPath.resolve(args[0]);
            Path file2 = currentPath.resolve(args[1]);

            if (!Files.exists(file1) || !Files.exists(file2)) {
                System.out.println("File not found");
                return;
            }
            if (Files.isDirectory(file1) || Files.isDirectory(file2)) {
                System.out.println("Cannot concatenate directories");
                return;
            }

            List<String> lines1 = Files.readAllLines(file1);
            List<String> lines2 = Files.readAllLines(file2);

            for (String line : lines1) {
                System.out.println(line);
            }
            for (String line : lines2) {
                System.out.println(line);
            }
        }
    }

    public void wc(String[] args) throws IOException {
        // Takes 1 argument - counts lines, words, characters
        if (args.length != 1) {
            System.out.println("Usage: wc <filename>");
            return;
        }

        Path filePath = currentPath.resolve(args[0]);
        if (!Files.exists(filePath)) {
            System.out.println("File not found: " + args[0]);
            return;
        }
        if (Files.isDirectory(filePath)) {
            System.out.println("Is a directory: " + args[0]);
            return;
        }

        String content = Files.readString(filePath);
        int lines = content.split("\n").length;
        String[] words = content.trim().isEmpty() ? new String[0] : content.trim().split("\\s+");
        int chars = content.length();

        System.out.println(lines + " " + words.length + " " + chars + " " + filePath.getFileName());
    }

    public void writeToFile(String[] args) {
        // Format: command > FileName
        if (args.length < 2) {
            System.out.println("Usage: > <text> <filename>");
            return;
        }

        StringBuilder text = new StringBuilder();
        for (int i = 0; i < args.length - 1; i++) {
            text.append(args[i]).append(" ");
        }

        String filename = args[args.length - 1];
        Path filePath = currentPath.resolve(filename);

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filePath.toFile(), false))) {
            writer.write(text.toString().trim());
            System.out.println("Output written to: " + filename);
        } catch (IOException e) {
            System.out.println("Error writing to file: " + filename);
        }
    }

    public void appendToFile(String[] args) {
        // Like > but appends to the file
        if (args.length < 2) {
            System.out.println("Usage: >> <text> <filename>");
            return;
        }

        StringBuilder text = new StringBuilder();
        for (int i = 0; i < args.length - 1; i++) {
            text.append(args[i]).append(" ");
        }

        String filename = args[args.length - 1];
        Path filePath = currentPath.resolve(filename);

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filePath.toFile(), true))) {
            writer.write(text.toString().trim());
            writer.newLine();
            System.out.println("Output appended to: " + filename);
        } catch (IOException e) {
            System.out.println("Error writing to file: " + filename);
        }
    }

    public void zip(String[] args) throws IOException {
        if (args.length < 2) {
            System.out.println("Usage: zip <archive.zip> <file1> [file2 ...] or zip -r <archive.zip> <directory>");
            return;
        }

        String zipName;
        List<String> filesToZip = new ArrayList<>();
        boolean recursive = false;

        // Check for -r flag
        if (args[0].equals("-r")) {
            if (args.length < 3) {
                System.out.println("Usage: zip -r <archive.zip> <directory>");
                return;
            }
            recursive = true;
            zipName = args[1];
            filesToZip.add(args[2]);
        } else {
            zipName = args[0];
            for (int i = 1; i < args.length; i++) {
                filesToZip.add(args[i]);
            }
        }

        Path zipPath = currentPath.resolve(zipName);
        try (FileOutputStream fos = new FileOutputStream(zipPath.toFile());
             ZipOutputStream zos = new ZipOutputStream(fos)) {

            for (String fileArg : filesToZip) {
                Path fileToZip = currentPath.resolve(fileArg);

                if (!Files.exists(fileToZip)) {
                    System.out.println("File/directory not found: " + fileArg);
                    continue;
                }

                if (Files.isDirectory(fileToZip) && recursive) {
                    // Zip directory recursively
                    Files.walk(fileToZip)
                            .filter(path -> !Files.isDirectory(path))
                            .forEach(path -> {
                                try {
                                    String zipEntryName = fileToZip.getParent().relativize(path).toString();
                                    zos.putNextEntry(new ZipEntry(zipEntryName));
                                    Files.copy(path, zos);
                                    zos.closeEntry();
                                } catch (IOException e) {
                                    System.out.println("Error adding to zip: " + path);
                                }
                            });
                } else if (Files.isRegularFile(fileToZip)) {
                    // Zip single file
                    zos.putNextEntry(new ZipEntry(fileToZip.getFileName().toString()));
                    Files.copy(fileToZip, zos);
                    zos.closeEntry();
                }
            }
        }
        System.out.println("Created zip: " + zipName);
    }

    public void unzip(String[] args) throws IOException {
        if (args.length < 1) {
            System.out.println("Usage: unzip <archive.zip> [-d destination]");
            return;
        }

        Path zipPath = currentPath.resolve(args[0]);
        Path destDir = currentPath;

        // Check for -d option
        if (args.length >= 3 && args[1].equals("-d")) {
            destDir = currentPath.resolve(args[2]);
            Files.createDirectories(destDir);
        }

        if (!Files.exists(zipPath)) {
            System.out.println("Zip file not found: " + args[0]);
            return;
        }

        try (ZipInputStream zis = new ZipInputStream(new FileInputStream(zipPath.toFile()))) {
            ZipEntry entry;
            while ((entry = zis.getNextEntry()) != null) {
                Path filePath = destDir.resolve(entry.getName());

                if (entry.isDirectory()) {
                    Files.createDirectories(filePath);
                } else {
                    Files.createDirectories(filePath.getParent());
                    Files.copy(zis, filePath, StandardCopyOption.REPLACE_EXISTING);
                }
                zis.closeEntry();
            }
        }
        System.out.println("Extracted to: " + destDir);
    }

    // THE ONLY CHANGE: Modified chooseCommandAction to handle both > and >> redirection
    public void chooseCommandAction(String input) throws IOException {
        // Check for append redirection >>
        if (input.contains(">>")) {
            String[] parts = input.split(">>", 2);
            if (parts.length == 2) {
                String commandPart = parts[0].trim();
                String filename = parts[1].trim();

                // Execute the command and capture output
                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                PrintStream originalOut = System.out;

                try {
                    // Redirect System.out to capture output
                    System.setOut(new PrintStream(baos));

                    // Parse and execute the command (output goes to baos)
                    if (parser.parse(commandPart)) {
                        String commandName = parser.getCommandName();
                        String[] args = parser.getArgs();

                        switch (commandName) {
                            case "pwd":
                                pwd();
                                break;
                            case "cd":
                                cd(args);
                                break;
                            case "ls":
                                ls();
                                break;
                            case "mkdir":
                                mkdir(args);
                                break;
                            case "rmdir":
                                rmdir(args);
                                break;
                            case "touch":
                                touch(args);
                                break;
                            case "cp":
                                cp(args);
                                break;
                            case "cp -r":
                                cpr(args);
                                break;
                            case "rm":
                                rm(args);
                                break;
                            case "cat":
                                cat(args);
                                break;
                            case "wc":
                                wc(args);
                                break;
                            case "zip":
                                zip(args);
                                break;
                            case "unzip":
                                unzip(args);
                                break;
                            default:
                                System.out.println("Unknown command: " + commandName);
                        }
                    }

                    // Restore original output
                    System.setOut(originalOut);

                    // Write captured output to file in append mode
                    String output = baos.toString();
                    Path filePath = currentPath.resolve(filename);

                    try (BufferedWriter writer = new BufferedWriter(new FileWriter(filePath.toFile(), true))) {
                        writer.write(output);
                        System.out.println("Output appended to: " + filename);
                    } catch (IOException e) {
                        System.out.println("Error writing to file: " + filename);
                    }

                } catch (Exception e) {
                    // Always restore original output even if error occurs
                    System.setOut(originalOut);
                    System.out.println("Error: " + e.getMessage());
                }
                return;
            }
        }

        // Check for overwrite redirection >
        if (input.contains(">")) {
            String[] parts = input.split(">", 2);
            if (parts.length == 2) {
                String commandPart = parts[0].trim();
                String filename = parts[1].trim();

                // Execute the command and capture output
                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                PrintStream originalOut = System.out;

                try {
                    // Redirect System.out to capture output
                    System.setOut(new PrintStream(baos));

                    // Parse and execute the command (output goes to baos)
                    if (parser.parse(commandPart)) {
                        String commandName = parser.getCommandName();
                        String[] args = parser.getArgs();

                        switch (commandName) {
                            case "pwd":
                                pwd();
                                break;
                            case "cd":
                                cd(args);
                                break;
                            case "ls":
                                ls();
                                break;
                            case "mkdir":
                                mkdir(args);
                                break;
                            case "rmdir":
                                rmdir(args);
                                break;
                            case "touch":
                                touch(args);
                                break;
                            case "cp":
                                cp(args);
                                break;
                            case "cp -r":
                                cpr(args);
                                break;
                            case "rm":
                                rm(args);
                                break;
                            case "cat":
                                cat(args);
                                break;
                            case "wc":
                                wc(args);
                                break;
                            case "zip":
                                zip(args);
                                break;
                            case "unzip":
                                unzip(args);
                                break;
                            default:
                                System.out.println("Unknown command: " + commandName);
                        }
                    }

                    // Restore original output
                    System.setOut(originalOut);

                    // Write captured output to file in overwrite mode
                    String output = baos.toString();
                    Path filePath = currentPath.resolve(filename);

                    try (BufferedWriter writer = new BufferedWriter(new FileWriter(filePath.toFile(), false))) {
                        writer.write(output);
                        System.out.println("Output written to: " + filename);
                    } catch (IOException e) {
                        System.out.println("Error writing to file: " + filename);
                    }

                } catch (Exception e) {
                    // Always restore original output even if error occurs
                    System.setOut(originalOut);
                    System.out.println("Error: " + e.getMessage());
                }
                return;
            }
        }

        // Normal command execution (no redirection)
        if (!parser.parse(input)) return;

        String commandName = parser.getCommandName();
        String[] args = parser.getArgs();

        switch (commandName) {
            case "pwd":
                pwd();
                break;
            case "cd":
                cd(args);
                break;
            case "ls":
                ls();
                break;
            case "mkdir":
                mkdir(args);
                break;
            case "rmdir":
                rmdir(args);
                break;
            case "touch":
                touch(args);
                break;
            case "cp":
                cp(args);
                break;
            case "cp -r":
                cpr(args);
                break;
            case "rm":
                rm(args);
                break;
            case "cat":
                cat(args);
                break;
            case "wc":
                wc(args);
                break;
            case ">":
                writeToFile(args);
                break;
            case ">>":
                appendToFile(args);
                break;
            case "zip":
                zip(args);
                break;
            case "unzip":
                unzip(args);
                break;
            case "exit":
                System.exit(0);
                break;
            default:
                System.out.println("Unknown command: " + commandName);
        }
    }

    public void start() throws IOException {
        Scanner sc = new Scanner(System.in);
        while (true) {
            System.out.print(currentPath + " $ ");
            String input = sc.nextLine();
            chooseCommandAction(input);
        }
    }
}