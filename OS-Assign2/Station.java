import java.util.LinkedList;
import java.util.Queue;

public class Station {

    public static void main(String[] args) {

        // Inputs (you can replace them with user input if needed)
        int waitingAreaCapacity = 3;
        int numberOfPumps = 2;    

        Queue<Car> queue = new LinkedList<>(); // cars Queue

        Semaphore empty = new Semaphore(waitingAreaCapacity);      // initially all spots are free
        Semaphore full = new Semaphore(0);                // no cars waiting at the start
        Semaphore mutex = new Semaphore(1);               // only one thread can use the queue at a time

        // Create and start pump threads
        for (int i = 1; i <= numberOfPumps; i++) {
            Pump pump = new Pump("Pump " + i, queue, empty, full, mutex);
            pump.start();
        }

        // Create and start car threads
        for (int i = 1; i <= 5; i++) {  // simulate 5 cars arriving
            Car car = new Car("Car " + i, queue, empty, full, mutex);
            car.start();

            try {
                Thread.sleep(1000); // small delay between car arrivals
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
