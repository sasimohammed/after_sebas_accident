import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.example.*;
import org.example.Process;
import org.junit.jupiter.api.Timeout;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;

import java.io.InputStream;
import java.util.*;

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
            "test_1.json, Priority",
            "test_1.json, RR",
            "test_2.json, Priority",
            "test_2.json, RR",
            "AG_test1.json, AG"
    })
    @Timeout(5)
    void testSchedulersFromJson(String fileName, String schedulerType) throws Exception {

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

        // 3️⃣ Load processes with quantum
        List<Process> processes = new ArrayList<>();
        for (JsonNode p : input.get("processes")) {
            int quantum = p.has("quantum") ? p.get("quantum").asInt() : rrQuantum;
            processes.add(new Process(
                    p.get("name").asText(),
                    p.get("arrival").asInt(),
                    p.get("burst").asInt(),
                    p.get("priority").asInt(),
                    quantum
            ));
        }

        // 4️⃣ Run scheduler
        SchedulerResult result;
        switch (schedulerType) {
            case "Priority" -> result = new PriorityScheduler(contextSwitch, agingInterval).run(processes);
            case "RR" -> result = new RoundRobinScheduler(rrQuantum, contextSwitch).run(processes);
            case "AG" -> result = new AGScheduler().run(processes);
            default -> throw new IllegalArgumentException("Unknown scheduler type: " + schedulerType);
        }

        // 5️⃣ Handle expected output flexibly
        JsonNode expectedRoot = root.get("expectedOutput");
        assertNotNull(expectedRoot, "Expected output missing in JSON");

        // If schedulerType exists in expectedOutput, use it; otherwise, assume expectedOutput is directly for this scheduler
        JsonNode expectedNode = expectedRoot.has(schedulerType) ? expectedRoot.get(schedulerType) : expectedRoot;
        assertNotNull(expectedNode, "Expected output for " + schedulerType + " is missing in " + fileName);

        // Execution order
        List<String> expectedOrder = new ArrayList<>();
        for (JsonNode n : expectedNode.get("executionOrder")) {
            expectedOrder.add(n.asText());
        }
        assertEquals(expectedOrder, result.executionOrder,
                schedulerType + " execution order mismatch in " + fileName);

        // Per-process results
        for (JsonNode p : expectedNode.get("processResults")) {
            String name = p.get("name").asText();
            assertEquals(
                    p.get("waitingTime").asInt(),
                    result.waitingTime.get(name),
                    schedulerType + " waiting time mismatch for " + name
            );
            assertEquals(
                    p.get("turnaroundTime").asInt(),
                    result.turnaroundTime.get(name),
                    schedulerType + " turnaround time mismatch for " + name
            );
        }

        // Averages
        assertEquals(
                expectedNode.get("averageWaitingTime").asDouble(),
                average(result.waitingTime.values()),
                0.01,
                schedulerType + " average waiting time mismatch"
        );
        assertEquals(
                expectedNode.get("averageTurnaroundTime").asDouble(),
                average(result.turnaroundTime.values()),
                0.01,
                schedulerType + " average turnaround time mismatch"
        );
    }
}
