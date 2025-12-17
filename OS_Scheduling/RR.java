import java.util.*;

class Process {
    String name;
    int arrival;
    int burst;
    int remaining;
    int completion;

    public Process(String name, int arrival, int burst) {
        this.name = name;
        this.arrival = arrival;
        this.burst = burst;
        this.remaining = burst;
        this.completion = 0;
    }
}

public class RR {

    public static void main(String[] args) {

        Scanner in = new Scanner(System.in);

        System.out.print("Enter number of processes: ");
        int n = in.nextInt();

        System.out.print("Enter RR Quantum: ");
        int quantum = in.nextInt();

        System.out.print("Enter Context Switching time: ");
        int context = in.nextInt();

        List<Process> processes = new ArrayList<>();

        for (int i = 0; i < n; i++) {
            System.out.println("\nProcess #" + (i + 1));

            System.out.print("Name: ");
            String name = in.next();

            System.out.print("Arrival: ");
            int arrival = in.nextInt();

            System.out.print("Burst: ");
            int burst = in.nextInt();

            processes.add(new Process(name, arrival, burst));
        }

        // sort by arrival
        processes.sort(Comparator.comparingInt(p -> p.arrival));

        Queue<Process> ready = new LinkedList<>();

        int time = 0;
        List<String> executionOrder = new ArrayList<>();

        // Round Robin Simulation
        while (true) {

            // add processes arriving now
            for (Process p : processes) {
                if (p.arrival <= time && p.remaining > 0 && !ready.contains(p)) {
                    ready.add(p);
                }
            }

            boolean unfinished = false;
            for (Process p : processes) {
                if (p.remaining > 0) {
                    unfinished = true;
                    break;
                }
            }

            if (ready.isEmpty() && unfinished) {
                time++;
                continue;
            }

            Process current = ready.poll();

            int executeTime = Math.min(quantum, current.remaining);

            executionOrder.add(current.name);

            current.remaining -= executeTime;

            time += executeTime;

            if (current.remaining == 0) {
                current.completion = time;
            } else {
                ready.add(current);
            }

            boolean stillExist = false;
            for (Process p : processes) {
                if (p.remaining > 0) {
                    stillExist = true;
                    break;
                }
            }

            if (stillExist) {
                time += context;
            }

            boolean allDone = true;
            for (Process p : processes) {
                if (p.remaining > 0) {
                    allDone = false;
                    break;
                }
            }

            if (allDone)
                break;
        }

        // Compute waiting/turnaround
        System.out.println("\nExecution Order:");
        System.out.println(executionOrder);

        double avgWT = 0, avgTAT = 0;

        System.out.println("\nProcess Results:");
        for (Process p : processes) {
            int tat = p.completion - p.arrival;
            int wt = tat - p.burst;

            avgWT += wt;
            avgTAT += tat;

            System.out.println(
                    p.name + " -> Waiting Time: " + wt + " , Turn Around Time: " + tat);
        }

        avgWT /= processes.size();
        avgTAT /= processes.size();

        System.out.println("\nAverage Waiting Time = " + avgWT);
        System.out.println("Average Turnaround Time = " + avgTAT);
    }
}
