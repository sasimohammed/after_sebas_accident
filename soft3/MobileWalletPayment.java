import java.util.List;

public class MobileWalletPayment implements Payment {
    private final String phoneNumber;
    private final List<Discount> discounts;

    public MobileWalletPayment(String phoneNumber, List<Discount> discounts) {
        this.phoneNumber = phoneNumber;
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
        System.out.println("Paid " + total + " EGP using Mobile Wallet: " + phoneNumber);
    }
}
