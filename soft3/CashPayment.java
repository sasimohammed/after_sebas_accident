import java.util.List;

public class CashPayment implements Payment {
    private final List<Discount> discounts;

    public CashPayment(List<Discount> discounts) {
        this.discounts = discounts;
    }

    @Override
    public void pay(Order order) {
        double total = 0;
        for (MenuItem item : order.getItems()) {
            double price = item.getPrice();
            for (Discount d : discounts) {
                price = d.applyDiscount(item); // apply all discounts
            }
            total += price;
        }
        System.out.println("Paid " + total + " EGP in cash after discounts.");
    }
}
