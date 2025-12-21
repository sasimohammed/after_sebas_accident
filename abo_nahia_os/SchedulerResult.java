package org.example;

import java.util.*;

public class SchedulerResult {

    public List<String> executionOrder = new ArrayList<>();

    public Map<String, Integer> waitingTime = new HashMap<>();
    public Map<String, Integer> turnaroundTime = new HashMap<>();


    public Map<String, List<Integer>> quantumHistory = new HashMap<>();


    public double averageWaitingTime;
    public double averageTurnaroundTime;


    public void printSummary() {
        System.out.println("\n" + "=".repeat(30));
        System.out.println("   AG SCHEDULING RESULTS");
        System.out.println("=".repeat(30));

        System.out.println("Execution Order: " + String.join(" -> ", executionOrder));
        System.out.println("-".repeat(30));

        System.out.printf("%-10s | %-12s | %-15s | %-20s\n",
                "Process", "Wait Time", "Turnaround", "Quantum History");
        System.out.println("-".repeat(65));

        for (String pName : waitingTime.keySet()) {
            System.out.printf("%-10s | %-12d | %-15d | %-20s\n",
                    pName,
                    waitingTime.get(pName),
                    turnaroundTime.get(pName),
                    quantumHistory.getOrDefault(pName, new ArrayList<>()).toString());
        }

        System.out.println("-".repeat(65));
        System.out.printf("Average Waiting Time: %.2f\n", averageWaitingTime);
        System.out.printf("Average Turnaround Time: %.2f\n", averageTurnaroundTime);
        System.out.println("=".repeat(30));
    }
}