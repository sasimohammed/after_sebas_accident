package org.example;

import org.json.JSONArray;
import org.json.JSONObject;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;

class Process {
    String name;
    int arrivalTime, burstTime, remainingBurst, priority, quantum;
    int waitingTime, turnaroundTime, completionTime;
    List<Integer> quantumHistory = new ArrayList<>();

    public Process(String name, int arrival, int burst, int priority, int quantum) {
        this.name = name;
        this.arrivalTime = arrival;
        this.burstTime = burst;
        this.remainingBurst = burst;
        this.priority = priority;
        this.quantum = quantum;
        this.quantumHistory.add(quantum);
    }
}

public class AGScheduler {
    public static void main(String[] args) {
        String[] testFiles = {"AG_test1.json", "AG_test2.json", "AG_test3.json", "AG_test4.json", "AG_test5.json", "AG_test6.json"};
        for (String file : testFiles) {
            runUnitTest(file);
        }
    }

    public static SimulationResult runAGScheduling(List<Process> processes) {
        int currentTime = 0;
        List<String> executionOrder = new ArrayList<>();
        List<Process> readyQueue = new LinkedList<>();
        List<Process> finished = new ArrayList<>();
        int totalProcesses = processes.size();
        Process current = null;

        // Sort by arrival initially [cite: 16, 49]
        processes.sort(Comparator.comparingInt(p -> p.arrivalTime));

        while (finished.size() < totalProcesses) {
            // Rule: Load arrivals before picking a process
            updateArrivals(processes, readyQueue, currentTime);

            if (current == null) {
                if (readyQueue.isEmpty()) {
                    currentTime++;
                    continue;
                }
                current = readyQueue.remove(0);
                addExecutionLog(executionOrder, current.name);
            }

            int q = current.quantum;
            int spentInTurn = 0;
            boolean preempted = false;

            // Clock-tick loop to handle multi-phase execution
            while (spentInTurn < q) {
                spentInTurn++;
                currentTime++;
                current.remainingBurst--;

                // Tie-breaker: New arrivals get priority in queue [cite: 11]
                updateArrivals(processes, readyQueue, currentTime);

                if (current.remainingBurst == 0) { // Scenario iv: Completed [cite: 14]
                    current.completionTime = currentTime;
                    current.quantum = 0;
                    current.quantumHistory.add(0);
                    finished.add(current);
                    current = null;
                    break;
                }

                // Phase 2: Non-preemptive Priority (starts after ceil(25%))
                if (spentInTurn >= Math.ceil(0.25 * q) && spentInTurn < Math.ceil(0.50 * q)) {
                    Process bestP = getBestPriority(readyQueue);
                    if (bestP != null && bestP.priority < current.priority) {
                        current.quantum += (int) Math.ceil((q - spentInTurn) / 2.0); // Scenario ii
                        current.quantumHistory.add(current.quantum);
                        readyQueue.add(current);
                        current = null;
                        preempted = true;
                        break;
                    }
                }
                // Phase 3: Preemptive SJF (starts after ceil(50%))
                else if (spentInTurn >= Math.ceil(0.50 * q)) {
                    Process shortest = getShortestRemaining(readyQueue);
                    if (shortest != null && shortest.remainingBurst < current.remainingBurst) {
                        current.quantum += (q - spentInTurn); // Scenario iii
                        current.quantumHistory.add(current.quantum);
                        readyQueue.add(current);
                        current = null;
                        preempted = true;
                        break;
                    }
                }
            }

            // Scenario i: Quantum exhausted [cite: 11]
            if (current != null && !preempted) {
                current.quantum += 2;
                current.quantumHistory.add(current.quantum);
                readyQueue.add(current);
                current = null;
            }
        }

        finished.sort(Comparator.comparing(p -> p.name));
        double totalWait = 0, totalTAT = 0;
        for (Process p : finished) {
            p.turnaroundTime = p.completionTime - p.arrivalTime; // [cite: 57]
            p.waitingTime = p.turnaroundTime - p.burstTime; // [cite: 56]
            totalWait += p.waitingTime;
            totalTAT += p.turnaroundTime;
        }

        return new SimulationResult(executionOrder, finished, totalWait / totalProcesses, totalTAT / totalProcesses);
    }

    private static void updateArrivals(List<Process> pList, List<Process> queue, int time) {
        Iterator<Process> it = pList.iterator();
        while (it.hasNext()) {
            Process p = it.next();
            if (p.arrivalTime <= time) {
                queue.add(p);
                it.remove();
            }
        }
    }

    private static void addExecutionLog(List<String> log, String name) {
        if (log.isEmpty() || !log.get(log.size() - 1).equals(name)) {
            log.add(name);
        }
    }

    private static Process getBestPriority(List<Process> queue) {
        return queue.stream().min(Comparator.comparingInt(p -> p.priority)).orElse(null);
    }

    private static Process getShortestRemaining(List<Process> queue) {
        return queue.stream().min(Comparator.comparingInt(p -> p.remainingBurst)).orElse(null);
    }

    public static void runUnitTest(String fileName) {
        try {
            System.out.println("\nTesting: " + fileName);
            String content = new String(Files.readAllBytes(Paths.get(fileName)));
            JSONObject json = new JSONObject(content);

            JSONArray inputJson = json.getJSONObject("input").getJSONArray("processes");
            List<Process> processes = new ArrayList<>();
            for (int i = 0; i < inputJson.length(); i++) {
                JSONObject p = inputJson.getJSONObject(i);
                processes.add(new Process(p.getString("name"), p.getInt("arrival"), p.getInt("burst"), p.getInt("priority"), p.getInt("quantum")));
            }

            SimulationResult actual = runAGScheduling(processes);
            JSONObject expected = json.getJSONObject("expectedOutput");

            boolean pass = actual.executionOrder.equals(jsonToList(expected.getJSONArray("executionOrder")));

            if (pass) {
                System.out.println("RESULT: TEST PASSED ✅");
            } else {
                System.out.println("RESULT: TEST FAILED ❌");
                System.out.println("Expected: " + expected.getJSONArray("executionOrder"));
                System.out.println("Actual:   " + actual.executionOrder);
            }
        } catch (Exception e) {
            System.out.println("Error reading " + fileName + ": " + e.getMessage());
        }
    }

    private static List<String> jsonToList(JSONArray arr) {
        List<String> list = new ArrayList<>();
        for (int i = 0; i < arr.length(); i++) list.add(arr.getString(i));
        return list;
    }
}

class SimulationResult {
    List<String> executionOrder;
    List<Process> processResults;
    double avgWait, avgTAT;
    public SimulationResult(List<String> o, List<Process> r, double w, double t) {
        this.executionOrder = o; this.processResults = r; this.avgWait = w; this.avgTAT = t;
    }
}