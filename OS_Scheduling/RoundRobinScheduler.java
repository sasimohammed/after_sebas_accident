package org.example;

import java.util.*;

public class RoundRobinScheduler {

    private final int quantum;
    private final int contextSwitch;

    public RoundRobinScheduler(int quantum, int contextSwitch) {
        this.quantum = quantum;
        this.contextSwitch = contextSwitch;
    }

    public SchedulerResult run(List<Process> processes) {

        SchedulerResult result = new SchedulerResult();

        // Sort by arrival
        processes.sort(Comparator.comparingInt(p -> p.arrival));

        Queue<Process> ready = new LinkedList<>();
        int time = 0;
        int completed = 0;

        while (completed < processes.size()) {

            // Add arrivals
            for (Process p : processes) {
                if (p.arrival <= time && p.remaining > 0 && !ready.contains(p)) {
                    ready.add(p);
                }
            }

            // CPU idle
            if (ready.isEmpty()) {
                time++;
                continue;
            }

            Process current = ready.poll();
            result.executionOrder.add(current.name);

            int exec = Math.min(quantum, current.remaining);
            current.remaining -= exec;
            time += exec;

            if (current.remaining == 0) {
                current.completionTime = time;
                completed++;
            } else {
                ready.add(current);
            }

            // Context switch if work remains
            if (completed < processes.size()) {
                time += contextSwitch;
            }
        }

        // Calculate metrics
        double totalWT = 0, totalTAT = 0;

        for (Process p : processes) {
            int tat = p.completionTime - p.arrival;
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
