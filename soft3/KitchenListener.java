public class KitchenListener implements EventListener {
    @Override
    public void update(String eventType, Order order) {
        if (eventType.equals("itemAdded")) {
            System.out.println("Kitchen: New item added to prepare!");
        } else if (eventType.equals("orderPlaced")) {
            System.out.println("Kitchen: Order is ready to cook!");
        }
    }
}

