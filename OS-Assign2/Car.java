import java.util.Queue;

public class Car extends Thread {
    private String carId;
    private Queue<Car> carsLine;
    private Semaphore empty;
    private Semaphore full;
    private Semaphore mutex;
    private int carNumber;

    public Car(String id, Queue<Car> line, Semaphore empty, Semaphore full, Semaphore mutex, int carNumber) {
        this.carId = id;
        this.carsLine = line;
        this.empty = empty;
        this.full = full;
        this.mutex = mutex;
        this.carNumber = carNumber;
    }

    @Override
    public void run() {
        // Update GUI - Car arrived
        Station.updateCarArrival(carId);

        empty.waitSem();
        mutex.waitSem();

        carsLine.add(this);

        if (carsLine.size() > 1) {
            System.out.println(carId + " arrived and waiting");
            // Update GUI - Car waiting
            Station.updateCarWaiting(carId, carsLine.size());
        } else {
            System.out.println(carId + " entered the waiting area");
            // Update GUI - Car entered waiting area
            Station.updateCarWaiting(carId, 1);
        }

        mutex.signalSem();
        full.signalSem();
    }

    public String getCarId() {
        return carId;
    }

    public int getCarNumber() {
        return carNumber;
    }
}