package org.example;

import java.util.*;

/**
 * PriorityScheduler: Preemptive Priority Scheduling with Aging.
 */
public class PriorityScheduler {
    private final int contextSwitchTime;
    private final int agingInterval;

    public PriorityScheduler(int contextSwitchTime, int agingInterval) {
        this.contextSwitchTime = contextSwitchTime;
        this.agingInterval = agingInterval;
    }

    private static class ProcessState {
        Process p;
        int remainingBurst;
        int currentPriority;
        int timeInQueue = 0;
        boolean finished = false;

        ProcessState(Process p) {
            this.p = p;
            this.remainingBurst = p.burst;
            this.currentPriority = p.priority;
        }
    }

    public SchedulerResult run(List<Process> processes) {
        SchedulerResult result = new SchedulerResult();
        List<ProcessState> allProcesses = new ArrayList<>();

        for (Process p : processes) {
            allProcesses.add(new ProcessState(p));
        }

        List<ProcessState> queue = new ArrayList<>();
        ProcessState lastWorkingProcess = null;
        int time = 0;
        int finishedCount = 0;
        int contextSwitchTimer = 0;

        while (finishedCount < allProcesses.size()) {

            // 1. add processes arrived at i to Q
            for (ProcessState ps : allProcesses) {
                if (ps.p.arrival == time && !ps.finished) {
                    if (!queue.contains(ps)) {
                        queue.add(ps);
                    }
                }
            }

            // 2. find best process (Lowest priority number = highest priority)
            ProcessState best = findBest(queue, lastWorkingProcess);

            // 3. if moment is in context switch time -> continue to last step
            if (contextSwitchTimer > 0) {
                contextSwitchTimer--;
                handleLastStep(queue, null); // Last step logic
                time++;
                continue;
            }

            // 4. if best process = -1 (cpu is idle now) -> continue to last step
            if (best == null && lastWorkingProcess == null) {
                handleLastStep(queue, null); // Last step logic
                time++;
                continue;
            }

            // 5. if lastWorkingProcess != -1 && lastWorkingProcess != best -> context switch then continue to last step
            if (lastWorkingProcess != null && best != lastWorkingProcess) {
                contextSwitchTimer = contextSwitchTime;

                // Return current process to queue and start switch
                queue.add(lastWorkingProcess);
                lastWorkingProcess = null;

                // Process first moment of context switch
                if (contextSwitchTimer > 0) {
                    contextSwitchTimer--;
                }

                handleLastStep(queue, null); // Last step logic
                time++;
                continue;
            }

            // 6. else -> the best will work for this moment and if burst becomes 0 then remove from Q
            if (best != null) {
                if (lastWorkingProcess == null) {
                    lastWorkingProcess = best;
                    queue.remove(best);

                    // Update execution order if a new process starts its turn
                    if (result.executionOrder.isEmpty() ||
                            !result.executionOrder.get(result.executionOrder.size() - 1).equals(lastWorkingProcess.p.name)) {
                        result.executionOrder.add(lastWorkingProcess.p.name);
                    }
                }

                lastWorkingProcess.remainingBurst--;

                if (lastWorkingProcess.remainingBurst == 0) {
                    finalizeProcess(lastWorkingProcess, time + 1, result);
                    lastWorkingProcess.finished = true;
                    lastWorkingProcess = null;
                    finishedCount++;
                }
            }

            // 7. last step: loop over Q and increase timeInQ and reduce priority if needed
            // do this only if this process is not the process run in the above else case
            handleLastStep(queue, lastWorkingProcess);

            time++;
        }

        return result;
    }

    private ProcessState findBest(List<ProcessState> queue, ProcessState current) {
        ProcessState best = current;
        for (ProcessState ps : queue) {
            if (best == null || ps.currentPriority < best.currentPriority) {
                best = ps;
            }
        }
        return best;
    }

    private void handleLastStep(List<ProcessState> queue, ProcessState runningProcess) {
        for (ProcessState ps : queue) {
            // "do this only if this process is not the process run in the above else case"
            // Since runningProcess is already removed from 'queue', this is naturally handled.
            ps.timeInQueue++;

            // Aging logic: reduce priority number (increase actual priority)
            if (agingInterval > 0 && ps.timeInQueue >= agingInterval) {
                if (ps.currentPriority > 0) {
                    ps.currentPriority--;
                }
                ps.timeInQueue = 0;
            }
        }
    }

    private void finalizeProcess(ProcessState ps, int completionTime, SchedulerResult res) {
        int tat = completionTime - ps.p.arrival;
        int wt = tat - ps.p.burst;
        res.turnaroundTime.put(ps.p.name, tat);
        res.waitingTime.put(ps.p.name, wt);
    }
}