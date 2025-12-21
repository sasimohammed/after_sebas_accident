package org.example;

import java.util.*;

public class AGScheduler {

    private static class AGProcess {
        Process p;
        int rem, quantum, arrival;
        boolean done = false;
        List<Integer> qHistory = new ArrayList<>();

        AGProcess(Process p) {
            this.p = p;
            this.rem = p.burst;
            this.arrival = p.arrival;
            this.quantum = p.quantum;
            this.qHistory.add(this.quantum);
        }
    }

    public SchedulerResult run(List<Process> processes) {
        SchedulerResult res = new SchedulerResult();
        List<AGProcess> all = new ArrayList<>();

        processes.sort(Comparator.comparingInt(p -> p.arrival));
        for (Process p : processes) all.add(new AGProcess(p));

        LinkedList<AGProcess> queue = new LinkedList<>();
        int time = 0, finished = 0, idx = 0;
        String lastProcess = "";

        while (finished < all.size()) {
            idx = addArrived(all, queue, idx, time);

            if (queue.isEmpty()) {
                time = all.get(idx).arrival;
                idx = addArrived(all, queue, idx, time);
            }

            AGProcess cur = queue.pollFirst();
            if (!cur.p.name.equals(lastProcess)) {
                res.executionOrder.add(cur.p.name);
                lastProcess = cur.p.name;
            }

            int oldQ = cur.quantum;
            int spent = 0;

            int p1 = (int) Math.ceil(oldQ * 0.25);
            spent += doWork(cur, p1);
            time += spent;
            idx = addArrived(all, queue, idx, time);

            if (isDone(cur, time, res)) { finished++; continue; }

            AGProcess bestP = findBestPriority(cur, queue);
            if (bestP != null) {
                updateQ(cur, oldQ + (int) Math.ceil((oldQ - spent) / 2.0));
                preempt(cur, bestP, queue);
                continue;
            }

            int p2 = (int) Math.ceil(oldQ * 0.25);
            int p2Work = doWork(cur, p2);
            time += p2Work;
            spent += p2Work;
            idx = addArrived(all, queue, idx, time);

            if (isDone(cur, time, res)) { finished++; continue; }

            AGProcess shortest = findShortest(cur, queue);
            if (shortest != null) {
                updateQ(cur, oldQ + (oldQ - spent));
                preempt(cur, shortest, queue);
                continue;
            }

            int p3Work = doWork(cur, oldQ - spent);
            time += p3Work;
            idx = addArrived(all, queue, idx, time);

            if (isDone(cur, time, res)) { finished++; continue; }

            updateQ(cur, oldQ + 2);
            queue.addLast(cur);
        }

        return getFinalResults(res, all, processes);
    }

    private int addArrived(List<AGProcess> all, LinkedList<AGProcess> q, int idx, int t) {
        while (idx < all.size() && all.get(idx).arrival <= t) {
            q.addLast(all.get(idx++));
        }
        return idx;
    }

    private int doWork(AGProcess cur, int limit) {
        int amount = Math.min(limit, cur.rem);
        cur.rem -= amount;
        return amount;
    }

    private void updateQ(AGProcess p, int val) {
        p.quantum = val;
        p.qHistory.add(val);
    }

    private void preempt(AGProcess cur, AGProcess next, LinkedList<AGProcess> q) {
        q.addLast(cur);
        q.remove(next);
        q.addFirst(next);
    }

    private boolean isDone(AGProcess p, int t, SchedulerResult res) {
        if (p.rem <= 0) {
            p.done = true;
            int tat = t - p.arrival;
            res.turnaroundTime.put(p.p.name, tat);
            res.waitingTime.put(p.p.name, tat - p.p.burst);
            updateQ(p, 0);
            return true;
        }
        return false;
    }

    private AGProcess findBestPriority(AGProcess cur, LinkedList<AGProcess> q) {
        AGProcess best = null;
        for (AGProcess p : q) {
            if (p.p.priority < cur.p.priority) {
                if (best == null || p.p.priority < best.p.priority) best = p;
            }
        }
        return best;
    }

    private AGProcess findShortest(AGProcess cur, LinkedList<AGProcess> q) {
        AGProcess best = null;
        for (AGProcess p : q) {
            if (p.rem < cur.rem) {
                if (best == null || p.rem < best.rem) best = p;
            }
        }
        return best;
    }

    private SchedulerResult getFinalResults(SchedulerResult res, List<AGProcess> all, List<Process> original) {
        double wSum = 0, tSum = 0;
        for (AGProcess ap : all) {
            res.quantumHistory.put(ap.p.name, ap.qHistory);
            wSum += res.waitingTime.get(ap.p.name);
            tSum += res.turnaroundTime.get(ap.p.name);
        }
        res.averageWaitingTime = wSum /original.size();
        res.averageTurnaroundTime = tSum/original.size();
        return res;
    }
}