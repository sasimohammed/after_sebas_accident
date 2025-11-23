import java.util.ArrayList;
import java.util.List;

class Order {
    private String orderId;
    private List<MenuItem> items;
    private String orderType;
    private double subtotal;
    private double tax;
    private double discount;
    private double total;
    private String status;

    private Order(OrderBuilder builder) {
        this.orderId = builder.orderId;
        this.items = builder.items;
        this.orderType = builder.orderType;
        this.subtotal = builder.subtotal;
        this.tax = builder.tax;
        this.discount = builder.discount;
        this.total = builder.total;
        this.status = "CREATED";
    }

    public String getOrderId() { return orderId; }
    public List<MenuItem> getItems() { return items; }
    public String getOrderType() { return orderType; }
    public double getSubtotal() { return subtotal; }
    public double getTax() { return tax; }
    public double getDiscount() { return discount; }
    public double getTotal() { return total; }
    public String getStatus() { return status; }

    public void setStatus(String status) { this.status = status; }

    public static class OrderBuilder {
        private String orderId;
        private List<MenuItem> items = new ArrayList<>();
        private String orderType = "DINE_IN";
        private double subtotal = 0;
        private double tax = 0;
        private double discount = 0;
        private double total = 0;

        public OrderBuilder(String orderId) {
            this.orderId = orderId;
        }

        public OrderBuilder addItem(MenuItem item) {
            items.add(item);
            subtotal += item.getPrice();
            return this;
        }

        public OrderBuilder setOrderType(String orderType) {
            this.orderType = orderType;
            return this;
        }

        public OrderBuilder setTax(double taxRate) {
            this.tax = subtotal * taxRate;
            return this;
        }

        public OrderBuilder applyDiscount(double discountAmount) {
            this.discount = discountAmount;
            return this;
        }

        public OrderBuilder calculateTotal() {
            this.total = subtotal + tax - discount;
            return this;
        }

        public Order build() {
            return new Order(this);
        }
    }
}
