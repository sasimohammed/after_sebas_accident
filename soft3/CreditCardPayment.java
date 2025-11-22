import java.util.List;

public class CreditCardPayment implements Payment {
    private final String cardNumber;
    private final List<Discount> discounts;

    public CreditCardPayment(String cardNumber, List<Discount> discounts) {
        this.cardNumber = cardNumber;
        this.discounts = discounts;
    }

    @Override
    public void pay(Order order) {
        double total = 0;
        for (MenuItem item : order.getItems()) {
            double price = item.getPrice();
            for (Discount d : discounts) {
                price = d.applyDiscount(item);
            }
            total += price;
        }
        System.out.println("Paid " + total + " EGP using Credit Card: " + cardNumber);
    }
}
