package org.example;

import java.util.*;

public class PriorityScheduler {
    private int contextSwitch;
    private int agingInterval;

    public PriorityScheduler(int contextSwitch, int agingInterval) {
        this.contextSwitch = contextSwitch;
        this.agingInterval = agingInterval;
    }

    public SchedulerResult run(List<Process> processes) {
        SchedulerResult result = new SchedulerResult();
        Map<String, Integer> remaining = new HashMap<>();
        Map<String, Integer> priority = new HashMap<>();
        Map<String, Integer> lastUpdateTime = new HashMap<>(); // Last time priority was updated for aging
        Map<String, Integer> completion = new HashMap<>();
        Map<String, Integer> arrivalTimeMap = new HashMap<>();

        for (Process p : processes) {
            remaining.put(p.name, p.burst);
            priority.put(p.name, p.priority);
            lastUpdateTime.put(p.name, p.arrival); // Initialize last update time to arrival time
            arrivalTimeMap.put(p.name, p.arrival);
        }

        List<Process> all = new ArrayList<>(processes);
        all.sort(Comparator.comparingInt(p -> p.arrival));

        List<Process> readyQ = new ArrayList<>();
        int time = 0;
        int finished = 0;
        int csTimer = 0;
        Process currentRunning = null;
        Process processJustFinished = null; // Process that just finished and needs context switch
        String lastLogged = "";

        while (finished < processes.size()) {
            System.out.println("\n=== Time: " + time + " ===");
            System.out.println("ReadyQ: " + readyQ.stream().map(p -> p.name).toList());
            System.out.println("Current Running: " + (currentRunning != null ? currentRunning.name : "null"));
            System.out.println("CS Timer: " + csTimer);

            // 1. Add arrivals at current time
            for (Process p : all) {
                if (p.arrival == time && !readyQ.contains(p)) {
                    readyQ.add(p);
                    System.out.println("Process " + p.name + " arrived at time " + time);
                }
            }

            // 2. Apply aging FIRST for all processes in readyQ
            List<Process> processesToAge = new ArrayList<>(readyQ);
            for (Process p : processesToAge) {
                // Check if it's time for aging: (time - lastUpdateTime) % agingInterval == 0
                int timeSinceLastUpdate = time - lastUpdateTime.get(p.name);
                if (timeSinceLastUpdate > 0 && timeSinceLastUpdate % agingInterval == 0) {
                    // Reduce priority by 1, minimum is 1
                    int newPriority = Math.max(1, priority.get(p.name) - 1);
                    priority.put(p.name, newPriority);
                    lastUpdateTime.put(p.name, time); // Update last update time
                    System.out.println("Aging: " + p.name + " priority reduced to " + newPriority);
                }
            }

            // 3. Context Switch handling (if in progress)
            if (csTimer > 0) {
                csTimer--;
                System.out.println("Context switch in progress, remaining: " + csTimer);
                time++;
                continue; // Skip to next time unit during context switch
            }

            // 4. Find the best process in readyQ
            Process best = null;
            for (Process p : readyQ) {
                if (best == null) {
                    best = p;
                } else {
                    int bestPriority = priority.get(best.name);
                    int pPriority = priority.get(p.name);

                    if (pPriority < bestPriority) {
                        best = p;
                    } else if (pPriority == bestPriority) {
                        // Tie breaker: earlier arrival time
                        if (arrivalTimeMap.get(p.name) < arrivalTimeMap.get(best.name)) {
                            best = p;
                        } else if (arrivalTimeMap.get(p.name).equals(arrivalTimeMap.get(best.name))) {
                            // If same arrival, choose by name (consistent tie breaker)
                            if (p.name.compareTo(best.name) < 0) {
                                best = p;
                            }
                        }
                    }
                }
            }

            System.out.println("Best process selected: " + (best != null ? best.name : "none"));

            // 5. Check if we need context switch (preemption or after process finishes)
            boolean needContextSwitch = false;

            if (processJustFinished != null) {
                // Context switch needed after a process finished
                needContextSwitch = true;
                System.out.println("Context switch needed because " + processJustFinished.name + " just finished");
                processJustFinished = null;
            } else if (currentRunning != null && best != null && !currentRunning.name.equals(best.name)) {
                // Preemption needed - different process should run
                needContextSwitch = true;
                System.out.println("Preemption needed: switching from " + currentRunning.name + " to " + best.name);
            }

            if (needContextSwitch) {
                csTimer = contextSwitch;
                currentRunning = null; // No process is running during context switch
                continue; // Will handle context switch in next iteration
            }

            // 6. Execute the best process (if any)
            if (best != null) {
                // Log execution if this is a new execution segment
                if (result.executionOrder.isEmpty() ||
                        !result.executionOrder.getLast().equals(best.name)) {
                    result.executionOrder.add(best.name);
                    lastLogged = best.name;
                }

                currentRunning = best;
                System.out.println("Executing " + best.name + " for 1 unit");

                // Execute for 1 time unit
                remaining.put(best.name, remaining.get(best.name) - 1);
                time++;

                // Check if process finished
                if (remaining.get(best.name) == 0) {
                    completion.put(best.name, time);
                    readyQ.remove(best);
                    finished++;
                    System.out.println("Process " + best.name + " finished at time " + time);

                    // Mark that this process just finished and needs context switch
                    processJustFinished = best;
                    currentRunning = null;

                    // Update lastUpdateTime for the finished process (won't be used but for consistency)
                    lastUpdateTime.put(best.name, time);
                } else {
                    // Process not finished yet, will continue next time
                    // Update lastUpdateTime for running process (aging reset for running process)
                    lastUpdateTime.put(best.name, time);
                }
            } else {
                // CPU idle
                System.out.println("CPU idle at time " + time);
                time++;
            }
        }

        return calculateStats(processes, completion, result);
    }

    private SchedulerResult calculateStats(List<Process> processes, Map<String, Integer> completion, SchedulerResult result) {
        double totalWT = 0, totalTAT = 0;
        for (Process p : processes) {
            int ct = completion.get(p.name);
            int tat = ct - p.arrival;
            int wt = tat - p.burst;
            result.turnaroundTime.put(p.name, tat);
            result.waitingTime.put(p.name, wt);
            totalWT += wt;
            totalTAT += tat;
        }
        result.averageWaitingTime = totalWT / processes.size();
        result.averageTurnaroundTime = totalTAT / processes.size();
        return result;
    }
}
