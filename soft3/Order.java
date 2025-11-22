import java.util.ArrayList;
import java.util.List;

public class Order {
    private final List<MenuItem> items = new ArrayList<>();
    private final List<EventListener> kitchenListeners = new ArrayList<>();
    private final List<EventListener> waiterListeners = new ArrayList<>();
    private Payment paymentMethod;
    private Discount discount;

    public void addItem(MenuItem item) {
        items.add(item);
        notifyListeners("itemAdded");
    }

    public void finishOrder() {
        notifyListeners("orderPlaced");
        printOrder();
    }

    private void notifyListeners(String eventType) {
        if (eventType.equals("itemAdded") || eventType.equals("orderPlaced")) {
            for (EventListener k : kitchenListeners) {
                k.update(eventType, this);
            }
        }
        if (eventType.equals("orderPlaced")) {
            for (EventListener w : waiterListeners) {
                w.update(eventType, this);
            }
        }
    }

    public void subscribeKitchen(EventListener listener) {
        kitchenListeners.add(listener);
    }

    public void subscribeWaiter(EventListener listener) {
        waiterListeners.add(listener);
    }

    public void printOrder() {
        System.out.println("\n---- Your Order ----");
        for (MenuItem i : items) {
            System.out.println("- " + i.getName() + " : " + i.getPrice());
        }
        System.out.println("Total = " + getTotal());
    }

    public double getTotal() {
        return items.stream().mapToDouble(MenuItem::getPrice).sum();
    }

    public List<MenuItem> getItems() {
        return items;
    }


    public void setPaymentMethod(Payment paymentMethod) {
        this.paymentMethod = paymentMethod;
    }

    public void payOrder() {
        if (paymentMethod != null) {

            paymentMethod.pay(this);
        } else {
            System.out.println("No payment method selected!");
        }
    }
}
