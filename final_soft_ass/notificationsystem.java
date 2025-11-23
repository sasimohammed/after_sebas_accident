import java.util.ArrayList;
import java.util.List;

interface OrderObserver {
    void update(Order order);
}

class KitchenNotification implements OrderObserver {
    private String kitchenId;

    public KitchenNotification(String kitchenId) {
        this.kitchenId = kitchenId;
    }

    @Override
    public void update(Order order) {
        System.out.println("\n===== KITCHEN NOTIFICATION =====");
        System.out.printf("KITCHEN %s: Order #%s - Status: %s%n",
                kitchenId, order.getOrderId(), order.getStatus());
        if ("PLACED".equals(order.getStatus())) {
            System.out.println("Order Items:");
            order.getItems().forEach(item -> {
                if (item instanceof AddOnDecorator) {
                    AddOnDecorator addon = (AddOnDecorator) item;
                    System.out.printf("   - %s (with %s)%n", addon.getBaseItemName(), addon.getName());
                } else {
                    System.out.printf("   - %s%n", item.getName());
                }
            });
        }
        System.out.println("================================\n");
    }
}

class WaiterNotification implements OrderObserver {
    private String waiterId;

    public WaiterNotification(String waiterId) {
        this.waiterId = waiterId;
    }

    @Override
    public void update(Order order) {
        System.out.println("\n===== WAITER NOTIFICATION =====");
        System.out.printf("👨‍💼 WAITER %s: Order #%s status: %s%n",
                waiterId, order.getOrderId(), order.getStatus());
        if ("PAID".equals(order.getStatus())) {
            System.out.println(" Order ready for serving!");
        }
        System.out.println("================================\n");
    }
}

class OrderSubject {
    private List<OrderObserver> observers = new ArrayList<>();

    public void addObserver(OrderObserver observer) {
        observers.add(observer);
    }

    public void removeObserver(OrderObserver observer) {
        observers.remove(observer);
    }

    public void notifyObservers(Order order) {
        for (OrderObserver observer : observers) {
            observer.update(order);
        }
    }
}