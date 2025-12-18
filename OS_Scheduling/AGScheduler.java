package org.example;



import java.util.*;

public class AGScheduler {

    public AGScheduler() {
        // No context switch or aging interval needed for AG
    }

    public SchedulerResult run(List<Process> processes) {
        SchedulerResult result = new SchedulerResult();

        List<Process> all = new ArrayList<>(processes);
        List<Process> ready = new ArrayList<>();

        int time = 0;
        int completed = 0;

        // Initialize remaining burst and quantum
        for (Process p : all) {
            p.remaining = p.burst;
            p.remainingQuantum = p.quantum;
        }

        while (completed < all.size()) {

            // 1️⃣ Add newly arrived processes to ready queue
            for (Process p : all) {
                if (p.arrival <= time && !ready.contains(p) && p.remaining > 0) {
                    ready.add(p);
                }
            }

            if (ready.isEmpty()) {
                time++;
                continue;
            }

            // 2️⃣ Pick first process in ready queue
            Process current = ready.remove(0);
            int originalQuantum = current.remainingQuantum;

            int timeFCFS = (int) Math.ceil(0.25 * originalQuantum);
            int timePriority = (int) Math.ceil(0.25 * originalQuantum);
            int timeSJF = originalQuantum - timeFCFS - timePriority;

            int executed = 0;

            // FCFS phase
            int t = Math.min(timeFCFS, current.remaining);
            current.remaining -= t;
            executed += t;
            time += t;
            result.executionOrder.add(current.name);

            // Priority phase (non-preemptive)
            if (current.remaining > 0) {
                t = Math.min(timePriority, current.remaining);
                current.remaining -= t;
                executed += t;
                time += t;
                result.executionOrder.add(current.name);
            }

            // SJF phase (preemptive)
            if (current.remaining > 0) {
                t = Math.min(timeSJF, current.remaining);
                current.remaining -= t;
                executed += t;
                time += t;
                result.executionOrder.add(current.name);
            }

            // 3️⃣ Update quantum based on scenario
            if (current.remaining == 0) {
                current.remainingQuantum = 0; // finished process
                current.completionTime = time;
                completed++;
            } else if (executed == originalQuantum) {
                current.remainingQuantum += 2; // used full quantum
                ready.add(current);
            } else if (executed < originalQuantum) {
                if (executed <= timeFCFS + timePriority) {
                    int remainingQ = originalQuantum - executed;
                    current.remainingQuantum += (int) Math.ceil(remainingQ / 2.0); // Priority phase
                } else {
                    int remainingQ = originalQuantum - executed;
                    current.remainingQuantum += remainingQ; // SJF phase
                }
                ready.add(current);
            }
        }

        // 4️⃣ Calculate turnaround and waiting times
        for (Process p : all) {
            int tat = p.completionTime - p.arrival;
            int wt = tat - p.burst;

            result.turnaroundTime.put(p.name, tat);
            result.waitingTime.put(p.name, wt);
        }

        return result;
    }
}

