import java.util.Queue;

public class Pump extends Thread {
    private String pumpId;
    private Queue<Car> carsLine;
    private Semaphore empty;
    private Semaphore full;
    private Semaphore mutex;
    private int pumpIndex;

    public Pump(String id, Queue<Car> line, Semaphore empty, Semaphore full, Semaphore mutex, int pumpIndex) {
        this.pumpId = id;
        this.carsLine = line;
        this.empty = empty;
        this.full = full;
        this.mutex = mutex;
        this.pumpIndex = pumpIndex;
    }

    public void run() {
        try {
            while (true) {
                full.waitSem();

                // Lock the queue before removing a car
                mutex.waitSem();

                // Remove the next car from the queue
                Car car = carsLine.remove();

                // Update GUI - Pump takes car
                Station.updatePumpActivity(pumpId, car.getCarId(), pumpIndex, true);
                System.out.println(pumpId + " takes " + car.getCarId());

                // Unlock the queue
                mutex.signalSem();

                // Notify cars that there is an available space
                empty.signalSem();

                System.out.println(pumpId + " begins service for " + car.getCarId());

                // Simulate service duration with progress updates
                for (int i = 0; i <= 100; i += 20) {
                    Thread.sleep(1500); // Total service time: 2000ms
                }

                System.out.println(pumpId + " finishes service for " + car.getCarId());

                // Update GUI - Pump finishes service
                Station.updatePumpActivity(pumpId, car.getCarId(), pumpIndex, false);

                System.out.println(pumpId + " waiting for a car...");
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}