package org.example;

import java.util.*;




public class RoundRobinScheduler {

    public SchedulerResult run(List<Process> processes, int quantum, int contextSwitch) {
        SchedulerResult result = new SchedulerResult();

        // ترتيب العمليات حسب وقت الوصول
        processes.sort(Comparator.comparingInt(p -> p.arrival));

        Queue<Process> readyQueue = new LinkedList<>();
        int currentTime = 0;
        int completedProcesses = 0;
        int n = processes.size();

        // استخدام مصفوفة لمتابعة ما إذا كانت العملية في الطابور أم لا
        Set<String> inQueue = new HashSet<>();

        // نسخة من Burst Time الأصلي للحسابات النهائية
        Map<String, Integer> originalBurst = new HashMap<>();
        Map<String, Integer> remainingBurst = new HashMap<>();
        for (Process p : processes) {
            originalBurst.put(p.name, p.burst);
            remainingBurst.put(p.name, p.burst);
        }

        while (completedProcesses < n) {
            // إضافة العمليات التي وصلت إلى الطابور
            for (Process p : processes) {
                if (p.arrival <= currentTime && remainingBurst.get(p.name) > 0 && !inQueue.contains(p.name)) {
                    readyQueue.add(p);
                    inQueue.add(p.name);
                }
            }

            if (readyQueue.isEmpty()) {
                currentTime++;
                continue;
            }

            Process current = readyQueue.poll();
            inQueue.remove(current.name);

            result.executionOrder.add(current.name);

            // تحديد وقت التنفيذ (إما Quantum أو المتبقي)
            int currentRemaining = remainingBurst.get(current.name);
            int executeTime = Math.min(quantum, currentRemaining);

            currentTime += executeTime;
            remainingBurst.put(current.name, currentRemaining - executeTime);

            // إضافة العمليات التي وصلت أثناء التنفيذ قبل إعادة العملية الحالية للطابور
            for (Process p : processes) {
                if (p.arrival <= currentTime && remainingBurst.get(p.name) > 0 && !inQueue.contains(p.name) && !p.name.equals(current.name)) {
                    readyQueue.add(p);
                    inQueue.add(p.name);
                }
            }

            if (remainingBurst.get(current.name) == 0) {
                completedProcesses++;
                int tat = currentTime - current.arrival;
                int wt = tat - originalBurst.get(current.name);

                result.turnaroundTime.put(current.name, tat);
                result.waitingTime.put(current.name, wt);
            } else {
                // إعادة العملية للطابور إذا لم تنتهِ
                readyQueue.add(current);
                inQueue.add(current.name);
            }

            // إضافة وقت Context Switch إذا كان هناك عمليات باقية
            if (completedProcesses < n) {
                currentTime += contextSwitch;
            }
        }

        return result;
    }

    // Main Method مدمجة للتجربة السريعة
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);

        System.out.print("Number of processes: ");
        int n = sc.nextInt();
        System.out.print("Quantum: ");
        int quantum = sc.nextInt();
        System.out.print("Context Switch: ");
        int context = sc.nextInt();

        List<Process> processes = new ArrayList<>();
        for (int i = 0; i < n; i++) {
            System.out.println("Process " + (i+1) + " (Name Arrival Burst Priority):");
            processes.add(new Process(sc.next(), sc.nextInt(), sc.nextInt(), sc.nextInt(), 0));
        }

        RoundRobinScheduler rr = new RoundRobinScheduler();
        SchedulerResult res = rr.run(processes, quantum, context);

        System.out.println("\nExecution Order: " + res.executionOrder);
        res.waitingTime.forEach((name, wt) ->
                System.out.println(name + " -> WT: " + wt + ", TAT: " + res.turnaroundTime.get(name)));
    }
}