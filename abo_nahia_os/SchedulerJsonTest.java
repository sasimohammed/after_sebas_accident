import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.example.*;
import org.example.Process;
import org.junit.jupiter.api.Timeout;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

public class SchedulerJsonTest {

    // -------------------------
    // Helper: Average calculator
    // -------------------------
    private double average(Collection<Integer> values) {
        int sum = 0;
        for (int v : values) sum += v;
        return values.isEmpty() ? 0 : (double) sum / values.size();
    }

    @ParameterizedTest
    @CsvSource({
            "AG_test1.json, AG",
            "AG_test2.json, AG",
            "AG_test3.json, AG",
            "AG_test4.json, AG",
            "AG_test5.json, AG",
            "AG_test6.json, AG",
            "test_1.json, RR",
            "test_2.json, RR",
            "test_3.json, RR",
            "test_4.json, RR",
            "test_5.json, RR",
            "test_1.json, SJF",
            "test_2.json, SJF",
            "test_3.json, SJF",
            "test_4.json, SJF",
            "test_5.json, SJF"
    })
    @Timeout(10) // زودت الـ Timeout شوية عشان الـ SJF والـ Context Switching
    void testSchedulersFromJson(String fileName, String schedulerType) throws Exception {
        System.out.println("\n=========================================");
        System.out.println("Testing " + fileName + " with " + schedulerType);
        System.out.println("=========================================");

        // 1️⃣ Load JSON
        ObjectMapper mapper = new ObjectMapper();
        InputStream is = getClass().getClassLoader().getResourceAsStream(fileName);
        assertNotNull(is, "JSON file not found: " + fileName);

        JsonNode root = mapper.readTree(is);
        JsonNode input = root.get("input");
        assertNotNull(input, "Input section missing in JSON");

        // 2️⃣ Read optional fields safely
        int contextSwitch = input.has("contextSwitch") ? input.get("contextSwitch").asInt() : 0;
        int agingInterval = input.has("agingInterval") ? input.get("agingInterval").asInt() : 0;
        int rrQuantum = input.has("rrQuantum") ? input.get("rrQuantum").asInt() : 1;

        // 3️⃣ Load processes
        List<Process> processes = new ArrayList<>();
        for (JsonNode p : input.get("processes")) {
            int quantum = p.has("quantum") ? p.get("quantum").asInt() : (rrQuantum > 0 ? rrQuantum : 1);
            Process process = new Process(
                    p.get("name").asText(),
                    p.get("arrival").asInt(),
                    p.get("burst").asInt(),
                    p.get("priority").asInt(),
                    quantum
            );
            processes.add(process);
        }

        // 4️⃣ Run scheduler
        SchedulerResult result;
        switch (schedulerType) {
            case "AG" -> result = new AGScheduler().run(processes);
            case "Priority" -> result = new PriorityScheduler(contextSwitch, agingInterval).run(processes);
            case "RR" -> result = new RoundRobinScheduler().run(processes, rrQuantum, contextSwitch);
            case "SJF" -> result = new ShortestJobFirstScheduler(contextSwitch).run(processes);
            default -> throw new IllegalArgumentException("Unknown scheduler type: " + schedulerType);
        }

        // 5️⃣ Handle expected output flexibly
        JsonNode expectedRoot = root.get("expectedOutput");
        assertNotNull(expectedRoot, "Expected output missing in JSON");

        // البحث عن الداتا (SJF ممكن تكون متسجلة باسم SRTF في بعض الملفات)
        String jsonKey = schedulerType;
        if (schedulerType.equals("SJF") && !expectedRoot.has("SJF") && expectedRoot.has("SRTF")) {
            jsonKey = "SRTF";
        }

        JsonNode expectedNode = expectedRoot.has(jsonKey) ? expectedRoot.get(jsonKey) : expectedRoot;
        assertNotNull(expectedNode, "Expected output for " + jsonKey + " is missing in " + fileName);

        // Execution order
        if (expectedNode.has("executionOrder")) {
            List<String> expectedOrder = new ArrayList<>();
            for (JsonNode n : expectedNode.get("executionOrder")) {
                expectedOrder.add(n.asText());
            }
            assertEquals(expectedOrder, result.executionOrder, schedulerType + " execution order mismatch in " + fileName);
        }

        // Per-process results (WT & TAT)
        if (expectedNode.has("processResults")) {
            for (JsonNode p : expectedNode.get("processResults")) {
                String name = p.get("name").asText();
                int expectedWT = p.get("waitingTime").asInt();
                int expectedTAT = p.get("turnaroundTime").asInt();

                assertEquals(expectedWT, result.waitingTime.get(name), schedulerType + " waiting time mismatch for " + name);
                assertEquals(expectedTAT, result.turnaroundTime.get(name), schedulerType + " turnaround time mismatch for " + name);
            }
        }

        // Averages
        if (expectedNode.has("averageWaitingTime")) {
            double expectedAvgWT = expectedNode.get("averageWaitingTime").asDouble();
            assertEquals(expectedAvgWT, average(result.waitingTime.values()), 0.02, schedulerType + " avg WT mismatch");
        }

        System.out.println("✓ All passed for " + fileName + " [" + schedulerType + "]");
    }
}