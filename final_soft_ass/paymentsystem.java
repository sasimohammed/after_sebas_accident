interface PaymentStrategy {
    boolean processPayment(double amount);
    String getPaymentMethod();
}

class CreditCardPayment implements PaymentStrategy {
    private String cardNumber;
    private String expiryDate;
    private String cvv;

    public CreditCardPayment(String cardNumber, String expiryDate, String cvv) {
        this.cardNumber = cardNumber;
        this.expiryDate = expiryDate;
        this.cvv = cvv;
    }

    @Override
    public boolean processPayment(double amount) {
        System.out.printf("💳 Processing credit card payment of $%.2f...%n", amount);
        System.out.println("✅ Payment approved!");
        return true;
    }

    @Override
    public String getPaymentMethod() {
        return "Credit Card";
    }
}

class CashPayment implements PaymentStrategy {
    @Override
    public boolean processPayment(double amount) {
        System.out.printf("💰 Cash payment of $%.2f received%n", amount);
        System.out.println("✅ Payment completed!");
        return true;
    }

    @Override
    public String getPaymentMethod() {
        return "Cash";
    }
}

class MobileWalletPayment implements PaymentStrategy {
    private String walletType;
    private String phoneNumber;

    public MobileWalletPayment(String walletType, String phoneNumber) {
        this.walletType = walletType;
        this.phoneNumber = phoneNumber;
    }

    @Override
    public boolean processPayment(double amount) {
        System.out.printf("📱 Processing %s payment of $%.2f for %s...%n", walletType, amount, phoneNumber);
        System.out.println("✅ Payment approved!");
        return true;
    }

    @Override
    public String getPaymentMethod() {
        return walletType + " Wallet";
    }
}