interface DiscountStrategy {
    double calculateDiscount(Order order);
    String getDescription();
}

class CategoryDiscountStrategy implements DiscountStrategy {
    private String category;
    private double discountRate;

    public CategoryDiscountStrategy(String category, double discountRate) {
        this.category = category;
        this.discountRate = discountRate;
    }

    @Override
    public double calculateDiscount(Order order) {
        double categoryTotal = order.getItems().stream()
                .filter(item -> item.getCategory().equalsIgnoreCase(category))
                .mapToDouble(MenuItem::getPrice)
                .sum();
        return categoryTotal * discountRate;
    }

    @Override
    public String getDescription() {
        return String.format("%.0f%% discount on %s items", discountRate * 100, category);
    }

    // ADD THIS GETTER METHOD
    public String getCategory() {
        return category;
    }
}

class PercentageDiscountStrategy implements DiscountStrategy {
    private double discountRate;
    private double minAmount;

    public PercentageDiscountStrategy(double discountRate, double minAmount) {
        this.discountRate = discountRate;
        this.minAmount = minAmount;
    }

    @Override
    public double calculateDiscount(Order order) {
        if (order.getSubtotal() >= minAmount) {
            return order.getSubtotal() * discountRate;
        }
        return 0;
    }

    @Override
    public String getDescription() {
        return String.format("%.0f%% discount on orders above $%.2f", discountRate * 100, minAmount);
    }

    // ADD THIS GETTER METHOD
    public double getMinAmount() {
        return minAmount;
    }
}

class FixedAmountDiscountStrategy implements DiscountStrategy {
    private double discountAmount;
    private double minAmount;

    public FixedAmountDiscountStrategy(double discountAmount, double minAmount) {
        this.discountAmount = discountAmount;
        this.minAmount = minAmount;
    }

    @Override
    public double calculateDiscount(Order order) {
        if (order.getSubtotal() >= minAmount) {
            return discountAmount;
        }
        return 0;
    }

    @Override
    public String getDescription() {
        return String.format("$%.2f off on orders above $%.2f", discountAmount, minAmount);
    }

    // ADD THIS GETTER METHOD
    public double getMinAmount() {
        return minAmount;
    }
}