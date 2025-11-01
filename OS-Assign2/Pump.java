import java.util.Queue;

public class Pump extends Thread{
    private String pumpId;
    private Queue<Car> carsLine;
    private Semaphore empty;
    private Semaphore full;
    private Semaphore mutex;

    public Pump(String id , Queue<Car> line , Semaphore empty, Semaphore full, Semaphore mutex) {
        this.pumpId = id;
        this.carsLine = line;
        this.empty = empty;
        this.full = full;
        this.mutex = mutex;
    }

    public void run(){
        try{
            while (true) {
                full.waitSem();

                // Lock the queue before removing a car
                mutex.waitSem();

                // Remove the next car from the queue
                Car car = carsLine.remove();
                System.out.println(pumpId + " takes " + car.getCarId()+"\n");

                //Unlock the queue
                mutex.signalSem();

                //notify cars that there is a avaliable space
                empty.signalSem();

                System.out.println(pumpId + " begins service for " + car.getCarId()+"\n");
                Thread.sleep(2000); // service duration
                System.out.println(pumpId + " finishes service for " + car.getCarId()+"\n");

                 System.out.println(pumpId + " waiting for a car...\n");
            }
            
        
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        
    }
}