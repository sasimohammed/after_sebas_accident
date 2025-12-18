package org.example;

public class Process {
    public String name;
    public int arrival;
    public int burst;
    public int remaining;
    public int priority;
    public int completionTime = -1;
    public int readySince = -1;
    public int remainingQuantum;
    public int quantum;
    public Process(String name, int arrival, int burst, int priority, int i) {
        this.name = name;
        this.arrival = arrival;
        this.burst = burst;
        this.remaining = burst;
        this.priority = priority;
    }
}

