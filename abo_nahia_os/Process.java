package org.example;

public class Process {
    public String name;
    public int arrival;
    public int burst;
    public int priority;
    public int quantum;

    public Process(String name, int arrival, int burst, int priority, int quantum) {
        this.name = name;
        this.arrival = arrival;
        this.burst = burst;
        this.priority = priority;
        this.quantum = Math.max(1, quantum);
    }
}
