import java.util.Queue;

public class Car extends Thread {
    private String carId;
    private Queue<Car> carsLine;
    private Semaphore empty;
    private Semaphore full;
    private Semaphore mutex;

    public Car(String id, Queue<Car> line, Semaphore empty, Semaphore full, Semaphore mutex) {
        this.carId = id;
        this.carsLine = line;
        this.empty = empty;
        this.full = full;
        this.mutex = mutex;
    }

    @Override
    public void run() {
        
            System.out.println(carId + " arrived.\n");

            // Wait for an empty space in the waiting area
            empty.waitSem();

            // Lock queue before adding the car
            mutex.waitSem();

            // Add this car to the waiting queue
            carsLine.add(this);
            System.out.println(carId + " entered the waiting area.\n");

            // Unlock queue
            mutex.signalSem();

            // Notify pumps that a car is available
            full.signalSem();

    }

    public String getCarId() {
        return carId;
    }
}
