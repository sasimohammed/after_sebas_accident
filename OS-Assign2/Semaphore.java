class Semaphore {

    private int value = 0 ;

    public Semaphore(int initial) { value = initial ; }

    public synchronized void waitSem(){
        value-- ;
        if (value < 0)
            try { wait() ; } 
            catch( InterruptedException e ) { }
        }

    public synchronized void signalSem() {
        value++ ; 
        if (value <= 0) 
            notify() ;
        }
}
