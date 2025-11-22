public class PizzaDiscount implements Discount {
    @Override
    public double applyDiscount(MenuItem item) {
        if (item.getName().toLowerCase().contains("pizza")) {
            return item.getPrice() * 0.9; // 10% off
        }
        return item.getPrice();
    }
}
