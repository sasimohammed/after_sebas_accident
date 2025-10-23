import java.io.*;
import java.nio.file.*;
import java.nio.file.attribute.FileTime;
import java.util.*;
import java.util.zip.*;

public class Terminal {
    private Path currentPath = Paths.get(System.getProperty("user.dir"));

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
            ">",
            ">>",
            "zip",
            "unzip",
            "exit"
    );

    private String commandName;
    private String[] args;

    private boolean parse(String input) {
        if (input == null || input.trim().isEmpty()) return false;
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
            if (!tokens[i].equals(commandName) && !tokens[i].equals("-r")) {
                argList.add(tokens[i]);
            }
        }
        args = argList.toArray(new String[0]);
        return true;
    }

    // ✅ Separate function for pwd
    public void pwd() {
        System.out.println(currentPath.toAbsolutePath().normalize());
    }

    public void appendToFile(String[] args) {
        if (args.length < 2) {
            System.out.println("Usage: >> <text> <filename>");
            return;
        }
        StringBuilder text = new StringBuilder();
        for (int i = 0; i < args.length - 1; i++) text.append(args[i]).append(" ");
        String filename = args[args.length - 1];
        Path filePath = currentPath.resolve(filename);
        try (RandomAccessFile raf = new RandomAccessFile(filePath.toFile(), "rw")) {
            long length = raf.length();
            if (length > 0) {
                raf.seek(length - 1);
                int lastByte = raf.read();
                if (lastByte != '\n') raf.writeBytes(System.lineSeparator());
            }
            raf.writeBytes(text.toString().trim() + System.lineSeparator());
        } catch (IOException e) {
            System.out.println("Error writing to file: " + filename);
        }
    }

    public void rmdir(String[] args) throws IOException {
        if (args.length < 1) {
            System.out.println("Usage: rmdir <dirname>");
            return;
        }
        if (args[0].equals("*")) {
            try (DirectoryStream<Path> stream = Files.newDirectoryStream(currentPath)) {
                for (Path p : stream) {
                    if (Files.isDirectory(p)) {
                        try (DirectoryStream<Path> inner = Files.newDirectoryStream(p)) {
                            boolean isEmpty = !inner.iterator().hasNext();
                            if (isEmpty) {
                                Files.delete(p);
                            }
                        }
                    }
                }
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
                System.out.println("Directory not empty.");
                return;
            }
        }
        Files.delete(targetDir);
    }

    public void touch(String[] args) throws IOException {
        if (args.length < 1) {
            System.out.println("Usage: touch <filename>");
            return;
        }
        Path newFile = currentPath.resolve(args[0]);
        if (Files.exists(newFile)) {
            Files.setLastModifiedTime(newFile, FileTime.fromMillis(System.currentTimeMillis()));
        } else {
            Files.createFile(newFile);
        }
    }

    public void cp(String[] args) throws IOException {
        if (args.length < 2) {
            System.out.println("Usage: cp <source> <destination>");
            return;
        }
        Path source = currentPath.resolve(args[0]);
        Path destination = currentPath.resolve(args[1]);
        if (!Files.exists(source)) {
            System.out.println("Source file not found: " + args[0]);
            return;
        }
        if (Files.isDirectory(source)) {
            System.out.println("Use -r to copy directories recursively.");
            return;
        }
        if (Files.isDirectory(destination)) destination = destination.resolve(source.getFileName());
        Files.copy(source, destination, StandardCopyOption.REPLACE_EXISTING);
    }

    public void copyDirectory(File source, File destination) throws IOException {
        if (!destination.exists()) {
            destination.mkdirs();
        }
        File[] files = source.listFiles();
        if (files != null) {
            for (File file : files) {
                File newDest = new File(destination, file.getName());
                if (file.isDirectory()) {
                    copyDirectory(file, newDest);
                }
                else {
                    Files.copy(file.toPath(), newDest.toPath(), StandardCopyOption.REPLACE_EXISTING);
                }
            }
        }
    }

    public void cpr(String[] args) throws IOException {
        if (args.length < 2) {
            System.out.println("Usage: cp -r <source> <destination>");
            return;
        }
        File source = new File(args[0]);
        File destination = new File(args[1]);
        if (source.isDirectory()) {
            copyDirectory(source, destination);
        }

        else{
                System.out.println("Use only cp if you want to copy files.");
            }
        }
    public void rm(String[] args) throws IOException{
    if (args.length > 1) {
        System.out.println("Usage: rm <filename>");
        return;
    }
    Path file = currentPath.resolve(args[0]);
    if (!Files.exists(file)) {
        System.out.println("file not found: " + args[0]);
        return;
    }
    if (Files.isDirectory(file)) {
        System.out.println("This is a directory,Not a file.");
        return;
    }
    Files.delete(file);
}
    public void cat(String[] args) throws IOException {
        if (args.length == 1) {
           Path filename = currentPath.resolve(args[0]);
           if (!Files.exists(filename)) {
               System.out.println("file not found: " + args[0]);
               return;
           }
           if (Files.isDirectory(filename)) {
               System.out.println("This is a directory,Not a file.");
               return;
           }
            List<String> lines = Files.readAllLines(filename);
            for (String line : lines) {
                System.out.println(line);
            }
        }
        else if(args.length == 2){
            Path filename1 = currentPath.resolve(args[0]);
            Path filename2 = currentPath.resolve(args[1]);
            if (!Files.exists(filename1) || !Files.exists(filename2)) {
                System.out.println("file not found: " + args[0]);
                return;
            }
            if (Files.isDirectory(filename1) || Files.isDirectory(filename2)) {
                System.out.println("This is a directory,Not a file.");
                return;
            }
            List<String> lines1 = Files.readAllLines(filename1);
            List<String> lines2 = Files.readAllLines(filename2);
            for (String line : lines1) {
                System.out.println(line);
            }
            for (String line : lines2) {
                System.out.println(line);
            }

        }
    }

    public void zip(String[] args) throws IOException {
        if (args.length < 2) {
            System.out.println("Usage: zip <archive-name.zip> <file1> <file2> ...");
            return;
        }
        String zipName = args[0];
        Path zipPath = currentPath.resolve(zipName);
        try (FileOutputStream fos = new FileOutputStream(zipPath.toFile());
             ZipOutputStream zos = new ZipOutputStream(fos)) {
            for (int i = 1; i < args.length; i++) {
                Path fileToZip = currentPath.resolve(args[i]);
                if (Files.isDirectory(fileToZip)) {
                    zipDirectory(fileToZip.toFile(), fileToZip.getFileName().toString(), zos);
                } else if (Files.exists(fileToZip)) {
                    zipFile(fileToZip.toFile(), fileToZip.getFileName().toString(), zos);
                }
            }
        }
    }

    private void zipFile(File file, String fileName, ZipOutputStream zos) throws IOException {
        zos.putNextEntry(new ZipEntry(fileName));
        Files.copy(file.toPath(), zos);
        zos.closeEntry();
    }

    private void zipDirectory(File folder, String parentFolder, ZipOutputStream zos) throws IOException {
        for (File file : folder.listFiles()) {
            if (file.isDirectory()) {
                zipDirectory(file, parentFolder + "/" + file.getName(), zos);
            } else {
                zipFile(file, parentFolder + "/" + file.getName(), zos);
            }
        }
    }

    public void unzip(String[] args) throws IOException {
        if (args.length < 1) {
            System.out.println("Usage: unzip <archive-name.zip> [-d destination]");
            return;
        }
        Path zipPath = currentPath.resolve(args[0]);
        Path destDir = currentPath;
        if (args.length >= 3 && args[1].equals("-d")) {
            destDir = currentPath.resolve(args[2]);
            Files.createDirectories(destDir);
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
    }

    public boolean mkdir(String[] args) throws IOException {
        if (args.length < 1) {
            System.out.println("Usage: mkdir <directory name> [more directories...]");
            return false;
        }

        boolean success = true;

        for (String dirName : args) {
            Path newDir = currentPath.resolve(dirName);
            if (Files.exists(newDir)) {
                System.out.println("Directory already exists: " + dirName);
                success = false;
                continue;
            }
            Files.createDirectory(newDir);
            System.out.println("Created directory: " + dirName);
        }

        return success;
    }


    public void ls() throws IOException {
        try (DirectoryStream<Path> dirContent = Files.newDirectoryStream(currentPath)) {
            for (Path entry : dirContent) System.out.println(entry.getFileName());
        }
    }

    public boolean cd(String[] args) throws IOException {
        Path newPath;
        if (args.length == 0) {
            String home = System.getProperty("user.home");
            newPath = Paths.get(home);
        } else if (args[0].equals("..")) {
            newPath = currentPath.getParent();
            if (newPath == null) {
                System.out.println("Already at the root directory.");
                return false;
            }
        } else {
            newPath = currentPath.resolve(args[0]).normalize();
        }
        if (!Files.exists(newPath) || !Files.isDirectory(newPath)) {
            System.out.println("Directory not found: " + newPath);
            return false;
        }
        currentPath = newPath;
        return true;
    }

    public void chooseCommandAction(String input) throws IOException {
        if (!parse(input)) return;
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
            case "zip":
                zip(args);
                break;
            case "unzip":
                unzip(args);
                break;
            case ">>":
                appendToFile(args);
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
