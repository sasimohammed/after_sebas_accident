import java.util.Scanner;
import java.util.List;
import java.util.ArrayList;







public class RestaurantMenu {

    private Scanner scanner = new Scanner(System.in);

    public void start() {
        int choice;

        do {
            System.out.println("\n====== RESTAURANT ORDERING SYSTEM ======");
            System.out.println("1. Display Menus");
            System.out.println("2. Create New Order");
            System.out.println("3. Add Item to Order");
            System.out.println("4. Add Add-ons / Customizations");
            System.out.println("5. Apply Discounts");
            System.out.println("6. Choose Order Type (Dine-in / Delivery / Takeaway)");
            System.out.println("7. Checkout & Generate Bill");
            System.out.println("8. Process Payment");
            System.out.println("9. Send Order Notification to Kitchen");
            System.out.println("0. Exit");
            System.out.print("Enter your choice: ");

            choice = scanner.nextInt();

            switch (choice) {
                case 1:
                    displayMenus();
                    break;
                case 2:
                    createOrder();
                    break;
                case 3:
                    addItem();
                    break;
                case 4:
                    addAddons();
                    break;
                case 5:
                    applyDiscounts();
                    break;
                case 6:
                    chooseOrderType();
                    break;
                case 7:
                    checkout();
                    break;
                case 8:
                    processPayment();
                    break;
                case 9:
                    notifyKitchen();
                    break;
                case 0:
                    System.out.println("Exiting system...");
                    break;
                default:
                    System.out.println("Invalid choice! Try again.");
            }

        } while (choice != 0);
    }

    // ======================
    // EMPTY METHODS (to be implemented later)
    // ======================

    private void displayMenus() {
        System.out.println("[Display Menus]");
    }

    private void createOrder() {
        System.out.println("[Create Order]");
    }

    private void addItem() {
        System.out.println("[Add Item]");
    }

    private void addAddons() {
        System.out.println("[Add Add-ons]");
    }

    private void applyDiscounts() {
        System.out.println("[Apply Discounts]");
    }

    private void chooseOrderType() {
        System.out.println("[Choose Order Type]");
    }

    private void checkout() {
        System.out.println("[Checkout]");
    }

    private void processPayment() {
        System.out.println("[Process Payment]");
    }

    private void notifyKitchen() {
        System.out.println("[Notify Kitchen]");
    }
}


