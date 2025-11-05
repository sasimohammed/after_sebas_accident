import javax.swing.*;
import java.awt.*;
import java.util.LinkedList;
import java.util.Queue;

public class Station {
    private static JFrame frame;
    private static JTextArea logArea;
    private static JPanel waitingAreaPanel;
    private static JPanel pumpsPanel;
    private static JLabel waitingCountLabel;
    private static DefaultListModel<String> waitingListModel;
    private static JList<String> waitingList;

    // Configuration variables
    private static int waitingAreaCapacity = 5;
    private static int numberOfPumps = 3;
    private static int numberOfCars = 8;
    private static Queue<Car> queue;
    private static boolean simulationRunning = false;

    // Input fields
    private static JTextField waitingField;
    private static JTextField pumpsField;
    private static JTextField carsField;

    // Pump components for easy access
    private static JLabel[] pumpLabels;
    private static JLabel[] carLabels;

    public static void main(String[] args) {
        createGUI();
    }

    private static void startSimulation() {
        if (simulationRunning) {
            JOptionPane.showMessageDialog(frame, "Simulation is already running!");
            return;
        }

        // Get values from input fields
        try {
            waitingAreaCapacity = Integer.parseInt(waitingField.getText());
            numberOfPumps = Integer.parseInt(pumpsField.getText());
            numberOfCars = Integer.parseInt(carsField.getText());

            if (waitingAreaCapacity < 1 || numberOfPumps < 1 || numberOfCars < 1) {
                JOptionPane.showMessageDialog(frame, "Please enter numbers greater than 0!");
                return;
            }

        } catch (NumberFormatException e) {
            JOptionPane.showMessageDialog(frame, "Please enter valid numbers!");
            return;
        }

        simulationRunning = true;
        clearLog();

        // Initialize queue and semaphores
        queue = new LinkedList<>();
        Semaphore empty = new Semaphore(waitingAreaCapacity);
        Semaphore full = new Semaphore(0);
        Semaphore mutex = new Semaphore(1);

        // Update GUI panels
        updateWaitingAreaPanel();
        updatePumpsPanel();

        // Create and start pump threads
        for (int i = 1; i <= numberOfPumps; i++) {
            Pump pump = new Pump("Pump " + i, queue, empty, full, mutex, i-1);
            new Thread(pump).start();
        }

        // Create and start car threads
        new Thread(() -> {
            for (int i = 1; i <= numberOfCars; i++) {
                Car car = new Car("Car " + i, queue, empty, full, mutex, i);
                new Thread(car).start();

                try {
                    Thread.sleep(1500); // Delay between car arrivals
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();

        logArea.append("Simulation Started!\n");
        logArea.append("Cars: " + numberOfCars + ", Pumps: " + numberOfPumps + ", Waiting Spots: " + waitingAreaCapacity + "\n\n");
    }

    private static void stopSimulation() {
        if (!simulationRunning) {
            JOptionPane.showMessageDialog(frame, "No simulation is running!");
            return;
        }

        simulationRunning = false;
        logArea.append("Simulation Stopped!\n");

        // Clear waiting list and reset display
        waitingListModel.clear();
        updateWaitingCount();
        updateWaitingAreaVisual();

        // Reset all pumps to available
        if (pumpLabels != null) {
            for (int i = 0; i < pumpLabels.length; i++) {
                if (pumpLabels[i] != null) {
                    pumpLabels[i].setBackground(Color.GREEN);
                    pumpLabels[i].setText("Pump " + (i + 1));
                }
                if (carLabels != null && carLabels[i] != null) {
                    carLabels[i].setText("Available");
                }
            }
        }
    }

    private static void resetSimulation() {
        stopSimulation();
        clearLog();
        logArea.append("Simulation Reset - Ready for new configuration\n");
    }

    private static void createGUI() {
        frame = new JFrame("Car Wash Simulation");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setLayout(new BorderLayout());

        // Control Panel
        JPanel controlPanel = createControlPanel();
        frame.add(controlPanel, BorderLayout.NORTH);

        // Main content panel
        JPanel mainPanel = new JPanel(new GridLayout(1, 3, 10, 10));
        mainPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

        // Waiting Area Panel
        JPanel waitingPanel = createWaitingAreaPanel();
        mainPanel.add(waitingPanel);

        // Pumps Panel
        pumpsPanel = createPumpsPanel();
        mainPanel.add(pumpsPanel);

        // Log Panel
        JPanel logPanel = createLogPanel();
        mainPanel.add(logPanel);

        frame.add(mainPanel, BorderLayout.CENTER);

        frame.setSize(1200, 700);
        frame.setLocationRelativeTo(null);
        frame.setVisible(true);
    }

    private static JPanel createControlPanel() {
        JPanel panel = new JPanel(new FlowLayout());
        panel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

        // Waiting Area Capacity
        panel.add(new JLabel("Waiting Spots:"));
        waitingField = new JTextField("5", 3);
        panel.add(waitingField);

        // Number of Pumps
        panel.add(new JLabel("Pumps:"));
        pumpsField = new JTextField("3", 3);
        panel.add(pumpsField);

        // Number of Cars
        panel.add(new JLabel("Cars:"));
        carsField = new JTextField("8", 3);
        panel.add(carsField);

        // Control Buttons
        JButton startButton = new JButton("Start");
        startButton.addActionListener(e -> startSimulation());

        JButton stopButton = new JButton("Stop");
        stopButton.addActionListener(e -> stopSimulation());

        JButton resetButton = new JButton("Reset");
        resetButton.addActionListener(e -> resetSimulation());

        panel.add(startButton);
        panel.add(stopButton);
        panel.add(resetButton);

        return panel;
    }

    private static JPanel createWaitingAreaPanel() {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBorder(BorderFactory.createTitledBorder("Waiting Area"));

        waitingAreaPanel = new JPanel();
        waitingAreaPanel.setLayout(new GridLayout(0, 1, 5, 5));
        waitingAreaPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

        waitingListModel = new DefaultListModel<>();
        waitingList = new JList<>(waitingListModel);
        JScrollPane listScrollPane = new JScrollPane(waitingList);
        listScrollPane.setPreferredSize(new Dimension(150, 100));

        waitingCountLabel = new JLabel("Waiting: 0/" + waitingAreaCapacity, JLabel.CENTER);
        waitingCountLabel.setFont(new Font("Arial", Font.BOLD, 14));

        panel.add(waitingCountLabel, BorderLayout.NORTH);
        panel.add(waitingAreaPanel, BorderLayout.CENTER);
        panel.add(new JLabel("Queue List:"), BorderLayout.WEST);
        panel.add(listScrollPane, BorderLayout.SOUTH);

        return panel;
    }

    private static JPanel createPumpsPanel() {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBorder(BorderFactory.createTitledBorder("Service Pumps"));

        // Initialize with current values
        updatePumpsPanel();

        return panel;
    }

    private static JPanel createLogPanel() {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBorder(BorderFactory.createTitledBorder("Log"));

        logArea = new JTextArea();
        logArea.setEditable(false);
        JScrollPane scrollPane = new JScrollPane(logArea);

        JButton clearButton = new JButton("Clear Log");
        clearButton.addActionListener(e -> clearLog());

        panel.add(scrollPane, BorderLayout.CENTER);
        panel.add(clearButton, BorderLayout.SOUTH);

        return panel;
    }

    private static void updateWaitingAreaPanel() {
        SwingUtilities.invokeLater(() -> {
            waitingAreaPanel.removeAll();

            for (int i = 0; i < waitingAreaCapacity; i++) {
                JLabel spot = new JLabel("Empty", JLabel.CENTER);
                spot.setOpaque(true);
                spot.setBackground(Color.LIGHT_GRAY);
                spot.setBorder(BorderFactory.createLineBorder(Color.GRAY));
                spot.setPreferredSize(new Dimension(120, 35));
                spot.setFont(new Font("Arial", Font.BOLD, 12));
                waitingAreaPanel.add(spot);
            }

            waitingAreaPanel.revalidate();
            waitingAreaPanel.repaint();
        });
    }

    private static void updatePumpsPanel() {
        SwingUtilities.invokeLater(() -> {
            pumpsPanel.removeAll();

            JPanel pumpsContainer = new JPanel();
            pumpsContainer.setLayout(new GridLayout(0, 1, 5, 5));
            pumpsContainer.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

            // Initialize arrays to store pump components
            pumpLabels = new JLabel[numberOfPumps];
            carLabels = new JLabel[numberOfPumps];

            for (int i = 0; i < numberOfPumps; i++) {
                JPanel pumpPanel = new JPanel(new BorderLayout());
                pumpPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK, 2));
                pumpPanel.setPreferredSize(new Dimension(180, 70));

                pumpLabels[i] = new JLabel("Pump " + (i + 1), JLabel.CENTER);
                pumpLabels[i].setOpaque(true);
                pumpLabels[i].setBackground(Color.GREEN);
                pumpLabels[i].setFont(new Font("Arial", Font.BOLD, 14));

                carLabels[i] = new JLabel("Available", JLabel.CENTER);
                carLabels[i].setFont(new Font("Arial", Font.PLAIN, 12));

                pumpPanel.add(pumpLabels[i], BorderLayout.NORTH);
                pumpPanel.add(carLabels[i], BorderLayout.CENTER);

                pumpsContainer.add(pumpPanel);
            }

            pumpsPanel.add(new JScrollPane(pumpsContainer));
            pumpsPanel.revalidate();
            pumpsPanel.repaint();
        });
    }

    private static void clearLog() {
        logArea.setText("");
    }

    // GUI update methods
    public static void updateCarArrival(String carId) {
        SwingUtilities.invokeLater(() -> {
            logArea.append("▶ " + carId + " arrived\n");
            logArea.setCaretPosition(logArea.getDocument().getLength());
        });
    }

    public static void updateCarWaiting(String carId, int position) {
        SwingUtilities.invokeLater(() -> {
            logArea.append("⏳ " + carId + " waiting (position " + position + ")\n");
            waitingListModel.addElement(carId);
            updateWaitingCount();
            updateWaitingAreaVisual();
        });
    }

    public static void updatePumpActivity(String pumpId, String carId, int pumpIndex, boolean starting) {
        SwingUtilities.invokeLater(() -> {
            if (starting) {
                logArea.append("🔧 " + pumpId + " servicing " + carId + "\n");

                // Update pump display using stored references
                if (pumpIndex < pumpLabels.length && pumpLabels[pumpIndex] != null) {
                    pumpLabels[pumpIndex].setBackground(Color.RED);
                    pumpLabels[pumpIndex].setText(pumpId + " - BUSY");
                    carLabels[pumpIndex].setText("Servicing: " + carId);
                }

                // Remove from waiting list
                waitingListModel.removeElement(carId);
                updateWaitingAreaVisual();
            } else {
                logArea.append("✅ " + pumpId + " finished " + carId + "\n");

                // Update pump display using stored references
                if (pumpIndex < pumpLabels.length && pumpLabels[pumpIndex] != null) {
                    pumpLabels[pumpIndex].setBackground(Color.GREEN);
                    pumpLabels[pumpIndex].setText("Pump " + (pumpIndex + 1));
                    carLabels[pumpIndex].setText("Available");
                }
            }
            updateWaitingCount();
        });
    }

    private static void updateWaitingAreaVisual() {
        SwingUtilities.invokeLater(() -> {
            Component[] spots = waitingAreaPanel.getComponents();
            for (int i = 0; i < spots.length; i++) {
                JLabel spot = (JLabel) spots[i];
                if (i < waitingListModel.size()) {
                    String carId = waitingListModel.get(i);
                    spot.setText(carId);
                    spot.setBackground(Color.YELLOW);
                    spot.setForeground(Color.BLACK);
                } else {
                    spot.setText("Empty");
                    spot.setBackground(Color.LIGHT_GRAY);
                    spot.setForeground(Color.BLACK);
                }
            }
        });
    }

    private static void updateWaitingCount() {
        SwingUtilities.invokeLater(() -> {
            waitingCountLabel.setText("Waiting: " + waitingListModel.size() + "/" + waitingAreaCapacity);
        });
    }
}