public class WaiterListener implements EventListener {
    @Override
    public void update(String eventType, Order order) {
        if (eventType.equals("orderPlaced")) {
            System.out.println("Waiter: Order placed, ready to serve!");
        }
    }
}
