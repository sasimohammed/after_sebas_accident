import java.io.*;
import java.nio.file.*;
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
    }

    // -----------------------------
    // touch command
    // -----------------------------
    public void touch(String[] args) throws IOException {
    }

    // -----------------------------
    // cp command
    // -----------------------------
    public void cp(String[] args) throws IOException {
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
// wc command
// -----------------------------
    public void wc(String[] args) throws IOException {
        if (args.length < 1) {
            System.out.println("Usage: wc <filename>");
            return;
        }

        Path filePath = currentPath.resolve(args[0]);

        if (!Files.exists(filePath)) {
            System.out.println("File not found: " + args[0]);
            return;
        }

        int lines = 0;
        int words = 0;
        int chars = 0;

        try (BufferedReader reader = new BufferedReader(new FileReader(filePath.toFile()))) {
            String line;
            while ((line = reader.readLine()) != null) {
                lines++;


                chars += line.length();


                String trimmed = line.trim();
                if (!trimmed.isEmpty()) {
                    words += trimmed.split("\\s+").length;
                }
            }
        }
        System.out.println(lines + " " + words + " " + chars + " " + filePath.getFileName());
    }


    // -----------------------------
// > command (write to file)
// -----------------------------
    public void writeToFile(String[] args) {
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
            writer.newLine();
            System.out.println("Written to " + filename);
        } catch (IOException e) {
            System.out.println("Error writing to file: " + filename);
        }
    }


    public void pwd() throws IOException{
        System.out.println(currentPath);
    }

    // -----------------------------
// command selector
// -----------------------------
    public void chooseCommandAction(String input) throws IOException {
        if (!parser.parse(input)) return;

        String cmd = parser.getCommandName();

        String[] args = parser.getArgs();

        switch (cmd) {
            case "zip": zip(args); break;
            case "unzip": unzip(args); break;
            case ">>": appendToFile(args); break;
            case "wc": wc(args); break;
            case ">": writeToFile(args); break;
            case "rmdir": rmdir(args); break;
            case "touch": touch(args); break;
            case "cp": cp(args); break;
            case "pwd": pwd(); break;
            case "exit": System.exit(0); break;

            default: {
                System.out.println("Unknown command: " + cmd);
            }
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