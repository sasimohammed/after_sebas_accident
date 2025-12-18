import java.util.ArrayList;
import java.util.Comparator;
import java.util.Deque;
import java.util.LinkedList;
import java.util.List;

public class ShortestJobFirstScheduler extends CPUScheduler {

    private final int contextSwitchCost;
    private int contextTimeLeft = 0;
    private Process currentProcess = null;

    public ShortestJobFirstScheduler(int contextSwitchCost) {
        this.contextSwitchCost = contextSwitchCost;
    }

    @Override
    public void start(List<Process> processes) {
        super.start(processes);

        if (!executionOrder.isEmpty()) {
            executionOrder.get(executionOrder.size() - 1).setEnd(currentTime - 1);
        }
    }

    @Override
    protected void step(List<Process> processes) {
        for (Process p : processes) {
            if (p.arrivalTime == currentTime) {
                readyQueue.add(p);
            }
        }

        sortByRemainingTime(readyQueue);

        if (contextTimeLeft > 0) {
            contextTimeLeft--;
            return;
        }

        if (readyQueue.isEmpty()) {
            currentProcess = null;
            return;
        }

        Process shortestProcess = readyQueue.peek();

        // 6) Preemption check
        if (currentProcess != null &&
            shortestProcess.remainingTime < currentProcess.remainingTime) {

            // Close current interval
            executionOrder.get(executionOrder.size() - 1).setEnd(currentTime - 1);

            currentProcess = null;
            contextTimeLeft = contextSwitchCost;
            return;
        }

        // 7) Start execution
        currentProcess = shortestProcess;

        if (executionOrder.isEmpty() ||
            executionOrder.get(executionOrder.size() - 1).process != currentProcess) {

            Interval interval = new Interval(currentProcess);
            interval.setStart(currentTime);
            executionOrder.add(interval);
        }

    
        currentProcess.remainingTime--;

        if (currentProcess.remainingTime == 0) {

            currentProcess.turnAroundTime = currentTime + 1 - currentProcess.arrivalTime;
            currentProcess.waitingTime = currentProcess.turnAroundTime - currentProcess.burstTime;

            readyQueue.poll();
            executionOrder.get(executionOrder.size() - 1).setEnd(currentTime);

            finished++;
            currentProcess = null;
            if (!readyQueue.isEmpty()) {
                contextTimeLeft = contextSwitchCost;
            }
        }
    }

    private void sortByRemainingTime(Deque<Process> queue) {
        List<Process> list = new ArrayList<>(queue);
        list.sort(Comparator.comparingInt(p -> p.remainingTime));
        queue.clear();
        queue.addAll(list);
    }
}
