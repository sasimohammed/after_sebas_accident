package org.example;

import java.util.*;

public class SchedulerResult {
    public List<String> executionOrder = new ArrayList<>();
    public Map<String, Integer> waitingTime = new HashMap<>();
    public Map<String, Integer> turnaroundTime = new HashMap<>();
}

