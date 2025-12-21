package org.example;

import java.util.*;

public class ShortestJobFirstScheduler {

    private int contextSwitch;

    public ShortestJobFirstScheduler(int contextSwitch) {
        this.contextSwitch = contextSwitch;
    }

    public SchedulerResult run(List<Process> processes) {
        SchedulerResult result = new SchedulerResult();

        // خرائط (Maps) لتخزين البيانات المساعدة دون تعديل كلاس Process
        Map<String, Integer> remainingTimes = new HashMap<>();
        Map<String, Integer> completionTimes = new HashMap<>();

        // ترتيب العمليات حسب وقت الوصول
        List<Process> sortedProcesses = new ArrayList<>(processes);
        sortedProcesses.sort(Comparator.comparingInt(p -> p.arrival));

        for (Process p : sortedProcesses) {
            remainingTimes.put(p.name, p.burst);
        }

        // الـ Priority Queue تختار العملية ذات الـ Remaining Time الأقل
        PriorityQueue<Process> ready = new PriorityQueue<>((a, b) -> {
            int remA = remainingTimes.get(a.name);
            int remB = remainingTimes.get(b.name);
            if (remA != remB) return Integer.compare(remA, remB);
            return Integer.compare(a.arrival, b.arrival);
        });

        int time = 0;
        int completed = 0;
        int contextLeft = 0;
        Process current = null;
        Process lastRunning = null;
        int n = sortedProcesses.size();

        while (completed < n) {
            // 1. إضافة العمليات التي وصلت عند الوقت الحالي
            for (Process p : sortedProcesses) {
                if (p.arrival == time) {
                    ready.add(p);
                }
            }

            // 2. التعامل مع وقت الـ Context Switch
            if (contextLeft > 0) {
                contextLeft--;
                time++;
                continue;
            }

            // 3. اختيار العملية الأقصر (Preemptive)
            if (!ready.isEmpty()) {
                Process shortest = ready.peek();

                if (current == null || remainingTimes.get(shortest.name) < remainingTimes.get(current.name)) {
                    if (current != null) {
                        ready.add(current);
                        contextLeft = contextSwitch; // تفعيل وقت التبديل
                    }
                    current = ready.poll();
                }
            }

            // 4. تسجيل ترتيب التنفيذ
            if (current != null && current != lastRunning) {
                result.executionOrder.add(current.name);
                lastRunning = current;
            }

            // 5. التنفيذ
            if (current != null) {
                int currentRem = remainingTimes.get(current.name);
                remainingTimes.put(current.name, currentRem - 1);
                time++;

                if (remainingTimes.get(current.name) == 0) {
                    completionTimes.put(current.name, time);
                    completed++;
                    current = null;

                    if (!ready.isEmpty()) {
                        contextLeft = contextSwitch; // تبديل بعد الانتهاء
                    }
                }
            } else {
                time++; // CPU Idle
            }
        }

        // 6. حساب الإحصائيات النهائية
        calculateStats(processes, completionTimes, result);

        return result;
    }

    private void calculateStats(List<Process> processes, Map<String, Integer> completionTimes, SchedulerResult result) {
        int totalWT = 0, totalTAT = 0;
        for (Process p : processes) {
            int completionTime = completionTimes.get(p.name);
            int tat = completionTime - p.arrival;
            int wt = tat - p.burst;

            result.turnaroundTime.put(p.name, tat);
            result.waitingTime.put(p.name, wt);
            totalWT += wt;
            totalTAT += tat;
        }
        result.averageWaitingTime = (double) totalWT / processes.size();
        result.averageTurnaroundTime = (double) totalTAT / processes.size();
    }
}