
import java.io.*;
import java.nio.file.*;
import java.nio.file.attribute.FileTime;
import java.util.*;
import java.util.zip.*;

public class Terminal {
    private Parser parser = new Parser();
    private Path currentPath = Paths.get(System.getProperty("user.dir"));

    // -----------------------------
    // >> command (append to file)
    // -----------------------------
    public void appendToFile(String[] args) {
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

        try (RandomAccessFile raf = new RandomAccessFile(filePath.toFile(), "rw")) {
            long length = raf.length();
            if (length > 0) {
                raf.seek(length - 1);
                int lastByte = raf.read();
                if (lastByte != '\n') {
                    raf.writeBytes(System.lineSeparator());
                }
            }

            raf.writeBytes(text.toString().trim() + System.lineSeparator());
            System.out.println("Appended to " + filename);
        } catch (IOException e) {
            System.out.println("Error writing to file: " + filename);
        }
    }

    // -----------------------------
    // rmdir command
    // -----------------------------
    public void rmdir(String[] args) throws IOException {
        if (args.length < 1) {
            System.out.println("Usage: rmdir <dirname>");
            return;
        }

        // Case 1 : rmdir *
        if (args[0].equals("*")) {
            try (DirectoryStream<Path> stream = Files.newDirectoryStream(currentPath)) {
                for (Path p : stream) {
                    if (Files.isDirectory(p)) {
                        try (DirectoryStream<Path> inner = Files.newDirectoryStream(p)) {
                            boolean isEmpty = !inner.iterator().hasNext();
                            if (isEmpty) {
                                Files.delete(p);
                                System.out.println("Removed: " + p.getFileName());
                            } else {
                                System.out.println("Skipped (not empty): " + p.getFileName());
                            }
                        }
                    }
                }
            }
            return;
        }

        // Case 2 : rmdir + dir name
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
                System.out.println("Directory not empty. Use -r to remove recursively.");
                return;
            }
        }
        Files.delete(targetDir);
        System.out.println("Removed directory: " + args[0]);
    }

    // -----------------------------
    // touch command
    // -----------------------------
    public void touch(String[] args) throws IOException {
        if (args.length < 1) {
            System.out.println("Usage: touch <filename>");
            return;
        }

        Path newFile = currentPath.resolve(args[0]);

        if (Files.exists(newFile)) {
            // File already exists , edit last modified date
            Files.setLastModifiedTime(newFile, FileTime.fromMillis(System.currentTimeMillis()));
            System.out.println( "Updated timestamp: " + newFile.getFileName());
        }
        else {
            // Create new empty file
            Files.createFile(newFile);
            System.out.println("Created file: " + newFile.getFileName());
        }
    }

    // -----------------------------
    // cp command
    // -----------------------------
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
        if (Files.isDirectory(destination)) {
            destination = destination.resolve(source.getFileName());
        }
        
        Files.copy(source, destination, StandardCopyOption.REPLACE_EXISTING);
        System.out.println("Copied: " + source.getFileName() + " -> " + destination);
    }


    // -----------------------------
    // zip command
    // -----------------------------
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
                } else {
                    System.out.println("Warning: File not found -> " + args[i]);
                }
            }

            System.out.println("Created archive: " + zipName);
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

    // -----------------------------
    // unzip command
    // -----------------------------
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
            System.out.println("Unzipped successfully to " + destDir);
        }
    }

    // -----------------------------
    // mkdir command
    public boolean mkdir(String[] args) throws IOException {
        if (args.length < 1) {
            System.out.println("Usage: mkdir <directory name>");
            return false;
        }

        Path newDir = currentPath.resolve(args[0]);

        if (Files.exists(newDir)) {
            System.out.println("Directory already exists: " + args[0]);
            return false;
        }

        Files.createDirectory(newDir);
        System.out.println("Created directory: " + args[0]);
        return true;
    }
    // -----------------------------
    // ls command 
    public void ls() throws IOException {
        //make list to store each file and folders in current directory (list = DirectoryStream)
        try (DirectoryStream<Path> dirContent = Files.newDirectoryStream(currentPath)) {
            for (Path entry : dirContent) {  // iterate through each entry in the directory
                System.out.println(entry.getFileName());
            }
        }
    }
    // -----------------------------
    // cd command
    public boolean cd(String[] args) throws IOException {
    Path newPath;

    if (args.length == 0) {   // no arguments provided
        // Case 1: cd → go to the home directory
        String home = System.getProperty("user.home");
        newPath = Paths.get(home);
    } else if (args[0].equals("..")) {
        // Case 2: cd .. → go one directory up
        newPath = currentPath.getParent();
        if (newPath == null) {
            System.out.println("Already at the root directory.");
            return false;
        }
    } else {
        // Case 3: cd <path> → go to given path
        newPath = currentPath.resolve(args[0]).normalize();
    }

    // Check if the path exists and is a directory
    if (!Files.exists(newPath) || !Files.isDirectory(newPath)) {
        System.out.println("Directory not found: " + newPath);
        return false;
    }

    // Update the current path
    currentPath = newPath;
    System.out.println("Changed directory to: " + currentPath.toAbsolutePath());
    return true;
}

    // command selector
    // -----------------------------
    public void chooseCommandAction(String input) throws IOException {
        if (!parser.parse(input))
            return;

        String cmd = parser.getCommandName();
        String[] args = parser.getArgs();

        switch (cmd) {
            case "rmdir":
                rmdir(args);
                break;
            case "touch":
                touch(args);
                break;
            case "cp":
                cp(args);
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
            case "mkdir":
                mkdir(args);
                break;
            case "exit":
                System.exit(0);
                break;
            default:
                System.out.println("Unknown command: " + cmd);
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
