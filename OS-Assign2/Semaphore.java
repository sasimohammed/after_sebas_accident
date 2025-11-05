class Semaphore {
    private int value;

    public Semaphore(int initial) {
        value = initial;
    }

    public synchronized void waitSem() {
        while (value <= 0) {
            try {
                wait();
            } catch(InterruptedException e) {
                System.err.println("Semaphore wait interrupted: " + e.getMessage());
            }
        }
        value--;
    }

    public synchronized void signalSem() {
        value++;
        notifyAll();
    }
}
