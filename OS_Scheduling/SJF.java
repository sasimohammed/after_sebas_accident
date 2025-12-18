import java.util.List;
import java.util.PriorityQueue;

public class ShortestJobFirstScheduler extends CPUScheduler {

    private final int contextSwitchCost;
    private int contextTimeLeft = 0;
    private Process currentProcess = null;
    private Process nextProcess = null;
    private PriorityQueue<Process> readyQueue = new PriorityQueue<>(
        (p1, p2) -> Integer.compare(p1.remainingTime, p2.remainingTime)
    );

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

        if (contextTimeLeft > 0) {
            contextTimeLeft--;
            if (contextTimeLeft == 0 && nextProcess != null) {
                currentProcess = nextProcess;
                nextProcess = null;
                Interval interval = new Interval(currentProcess);
                interval.setStart(currentTime);
                executionOrder.add(interval);
            }
            return;
        }

        if (!readyQueue.isEmpty()) {
            Process shortestProcess = readyQueue.peek();

            if (currentProcess == null) {
                currentProcess = readyQueue.poll();
                Interval interval = new Interval(currentProcess);
                interval.setStart(currentTime);
                executionOrder.add(interval);
            } 
            else if (shortestProcess.remainingTime < currentProcess.remainingTime) {
                executionOrder.get(executionOrder.size() - 1).setEnd(currentTime - 1);
                readyQueue.add(currentProcess);
                nextProcess = readyQueue.poll();
                currentProcess = null;
                contextTimeLeft = contextSwitchCost;
                return;
            }
        }

        if (currentProcess != null) {
            currentProcess.remainingTime--;
            if (currentProcess.remainingTime == 0) {
                currentProcess.turnAroundTime = currentTime + 1 - currentProcess.arrivalTime;
                currentProcess.waitingTime = currentProcess.turnAroundTime - currentProcess.burstTime;
                executionOrder.get(executionOrder.size() - 1).setEnd(currentTime);
                finished++;
                currentProcess = null;
                if (!readyQueue.isEmpty()) {
                    nextProcess = readyQueue.poll();
                    contextTimeLeft = contextSwitchCost;
                }
            }
        }
    }
}
