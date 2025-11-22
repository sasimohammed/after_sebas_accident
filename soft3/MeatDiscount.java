public class MeatDiscount implements Discount {
    @Override
    public double applyDiscount(MenuItem item) {
        if (item.getName().toLowerCase().contains("meat")) {
            return item.getPrice() * 0.8; // 20% off
        }
        return item.getPrice();
    }
}
