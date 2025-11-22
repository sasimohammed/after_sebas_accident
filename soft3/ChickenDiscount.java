public class ChickenDiscount implements Discount {
    @Override
    public double applyDiscount(MenuItem item) {
        if (item.getName().toLowerCase().contains("chicken")) {
            return item.getPrice() * 0.85; // 15% off
        }
        return item.getPrice();
    }
}
