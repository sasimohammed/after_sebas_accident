import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.example.PriorityScheduler;
import org.example.Process;
import org.example.SchedulerResult;
import org.junit.jupiter.api.Timeout;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

public class PrioritySchedulerJsonTest {

    // -------------------------
    // Helper: Average calculator
    // -------------------------
    private double average(Iterable<Integer> values) {
        int sum = 0;
        int count = 0;
        for (int v : values) {
            sum += v;
            count++;
        }
        return count == 0 ? 0.0 : (double) sum / count;
    }

    // -------------------------
    // Parameterized JSON tests
    // -------------------------
    @ParameterizedTest
    @ValueSource(strings = {
            "test.json",
            "test_2.json"
    })
    @Timeout(2) // Prevent infinite loops
    void testPrioritySchedulerFromJson(String fileName) throws Exception {

        // 1️⃣ Load JSON
        ObjectMapper mapper = new ObjectMapper();
        InputStream is =
                getClass().getClassLoader().getResourceAsStream(fileName);

        assertNotNull(is, "JSON file not found: " + fileName);

        JsonNode root = mapper.readTree(is);

        JsonNode input = root.get("input");
        int contextSwitch = input.get("contextSwitch").asInt();
        int agingInterval = input.get("agingInterval").asInt();

        // 2️⃣ Load processes
        List<Process> processes = new ArrayList<>();
        for (JsonNode p : input.get("processes")) {
            processes.add(new Process(
                    p.get("name").asText(),
                    p.get("arrival").asInt(),
                    p.get("burst").asInt(),
                    p.get("priority").asInt()
            ));
        }

        // 3️⃣ Run scheduler
        PriorityScheduler scheduler =
                new PriorityScheduler(contextSwitch, agingInterval);

        SchedulerResult result = scheduler.run(processes);

        // 4️⃣ Expected output
        JsonNode expected =
                root.get("expectedOutput").get("Priority");

        // -------------------------
        // Execution order
        // -------------------------
        List<String> expectedOrder = new ArrayList<>();
        for (JsonNode n : expected.get("executionOrder")) {
            expectedOrder.add(n.asText());
        }

        assertEquals(expectedOrder, result.executionOrder,
                "Execution order mismatch in " + fileName);

        // -------------------------
        // Per-process results
        // -------------------------
        for (JsonNode p : expected.get("processResults")) {
            String name = p.get("name").asText();
            int expectedWT = p.get("waitingTime").asInt();
            int expectedTAT = p.get("turnaroundTime").asInt();

            assertEquals(expectedWT,
                    result.waitingTime.get(name),
                    "Waiting time mismatch for " + name +
                            " in " + fileName);

            assertEquals(expectedTAT,
                    result.turnaroundTime.get(name),
                    "Turnaround time mismatch for " + name +
                            " in " + fileName);
        }

        // -------------------------
        // Averages
        // -------------------------
        double expectedAvgWT =
                expected.get("averageWaitingTime").asDouble();

        double expectedAvgTAT =
                expected.get("averageTurnaroundTime").asDouble();

        double actualAvgWT =
                average(result.waitingTime.values());

        double actualAvgTAT =
                average(result.turnaroundTime.values());

        assertEquals(expectedAvgWT, actualAvgWT, 0.01,
                "Average waiting time mismatch in " + fileName);

        assertEquals(expectedAvgTAT, actualAvgTAT, 0.01,
                "Average turnaround time mismatch in " + fileName);
    }
}
