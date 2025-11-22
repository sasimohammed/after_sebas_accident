//public class Main {
//    public static void main(String[] args) {
//        RestaurantMenu menu = new RestaurantMenu();
//        menu.start();
//
//
//    }
//}

import java.util.*;

public class Main {

    public static void main(String[] args) {

        // Register menus (PLUGIN STYLE)
        MenuRegistry.register(new VegeterianMenu());
        MenuRegistry.register(new non_VegeterianMenu());
        MenuRegistry.register(new KidsMenu());



        Scanner sc = new Scanner(System.in);
        Order order = new Order();

        // --- Choose Menu ---
        System.out.println("Available Menus:");
        int idx = 1;
        Map<Integer,String> menuMap = new HashMap<>();

        for (String name : MenuRegistry.getMenuNames()) {
            System.out.println(idx + ". " + name);
            menuMap.put(idx, name);
            idx++;
        }

        int chosenMenu = sc.nextInt();
        Menu menu = MenuRegistry.getMenu(menuMap.get(chosenMenu));

        boolean running = true;


        while (running) {
            // --- Choose Category ---
            System.out.println("\nChoose Category:");

            List<Category> categories = menu.getAvailableCategories();
            for (int i = 0; i < categories.size(); i++) {
                System.out.println((i+1) + ". " + categories.get(i));
            }
            System.out.println((categories.size()+1) + ". Finish");

            int choice = sc.nextInt();

            if (choice == categories.size() + 1) break;

            Category selectedCat = categories.get(choice - 1);

            // --- Choose Item ---
            List<MenuItem> items = menu.getItemsByCategory(selectedCat);
            System.out.println("Choose item:");
            for (int i = 0; i < items.size(); i++) {
                System.out.println((i+1) + ". " + items.get(i).getName() + " - " + items.get(i).getPrice());
            }



            while (true) {
                try {
                    System.out.print("Enter item number: ");

                    if (!sc.hasNextInt()) {
                        System.out.println("Invalid input! Please enter a number.");
                        sc.nextLine();
                        continue;
                    }

                    int itemNum = sc.nextInt();
                    sc.nextLine();

                    if (itemNum < 1 || itemNum > items.size()) {
                        System.out.println("Item number out of range! Try again.");
                        continue;
                    }

                    order.addItem(items.get(itemNum - 1));
                    System.out.println("Item added successfully!");
                    break;

                } catch (Exception e) {
                    System.out.println("Unexpected error happened.");
                    sc.nextLine(); // clear buffer
                }
            }



        }

























        order.printOrder();


/// ////////////////////////////////////////////////////////////////
        // Payment with discounts
        Payment payment = new CreditCardPayment(
                "1234-5678-9012-3456",
                Arrays.asList(new PizzaDiscount(), new ChickenDiscount())
        );
        order.setPaymentMethod(payment);

        order.payOrder();

    }
}
