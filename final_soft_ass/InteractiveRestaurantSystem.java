// InteractiveRestaurantSystem.java
// Enhanced restaurant system with completely option-based interface

import java.util.*;

public class InteractiveRestaurantSystem {
    private MenuFactory menuFactory;
    private StyleFactory styleFactory;
    private OrderSubject orderSubject;
    private List<DiscountStrategy> discountStrategies;
    private Map<String, MenuFactory> menuFactories;
    private Scanner scanner;
    private int orderCounter;

    // Pre-defined options
    private final Map<String, Double> pizzaSizes = Map.of(
            "Small", 2.0,
            "Medium", 0.0,
            "Large", 4.0
    );

    private final Map<String, Double> sauces = Map.of(
            "BBQ Sauce", 0.5,
            "Garlic Sauce", 0.5,
            "Hot Sauce", 0.5,
            "Mayonnaise", 0.5,
            "Ranch", 0.5
    );

    private final Map<String, Double> toppings = Map.of(
            "Mushrooms", 1.0,
            "Olives", 1.0,
            "Onions", 1.0,
            "Bell Peppers", 1.0,
            "Jalapenos", 1.0,
            "Tomatoes", 1.0,
            "Pineapple", 1.5,
            "Extra Cheese", 1.5
    );

    public InteractiveRestaurantSystem() {
        this.orderSubject = new OrderSubject();
        this.discountStrategies = new ArrayList<>();
        this.menuFactories = new HashMap<>();
        this.scanner = new Scanner(System.in);
        this.orderCounter = 1;
        initializeSystem();
    }

    private void initializeSystem() {
        // Initialize menu factories
        menuFactories.put("VEGETARIAN", new VegetarianMenuFactory());
        menuFactories.put("NON_VEGETARIAN", new NonVegetarianMenuFactory());
        menuFactories.put("KIDS", new KidsMenuFactory());

        // Initialize discount strategies
        discountStrategies.add(new CategoryDiscountStrategy("Vegetarian", 0.1));
        discountStrategies.add(new CategoryDiscountStrategy("Non-Vegetarian", 0.05));
        discountStrategies.add(new CategoryDiscountStrategy("Kids", 0.15));
        discountStrategies.add(new PercentageDiscountStrategy(0.08, 40.0));
        discountStrategies.add(new FixedAmountDiscountStrategy(5.0, 25.0));

        // Register observers
        orderSubject.addObserver(new KitchenNotification("MAIN_KITCHEN"));
        orderSubject.addObserver(new WaiterNotification("WAITER"));

        System.out.println(" Welcome to Our Restaurant Management System!");
        System.out.println("=============================================\n");
    }

    public void start() {
        while (true) {
            displayMainMenu();
            int choice = getIntInput("Choose an option: ");

            switch (choice) {
                case 1:
                    createNewOrder();
                    break;
                case 2:
                    displayAvailableDiscounts();
                    break;
                case 3:
                    displayMenuSamples();
                    break;
                case 4:
                    System.out.println(" Thank you for using our system! Goodbye!");
                    return;
                default:
                    System.out.println("Invalid option. Please try again.");
            }
        }
    }

    private void displayMainMenu() {
        System.out.println("\n MAIN MENU");
        System.out.println("1. Create New Order");
        System.out.println("2. View Available Discounts");
        System.out.println("3. View Menu Samples");
        System.out.println("4. Exit System");
    }

    private void createNewOrder() {
        System.out.println("\n CREATING NEW ORDER");

        // Select menu type and style
        String menuType = selectMenuType();
        setMenuType(menuType);

        String styleType = selectStyleType();
        setStyleType(styleType);

        // Create items
        List<MenuItem> orderItems = new ArrayList<>();
        boolean addingItems = true;

        while (addingItems) {
            System.out.println("\n ADD ITEM TO ORDER");
            System.out.println("1.  Add Pizza");
            System.out.println("2.  Add Burger");
            System.out.println("3.  Add Pasta");
            System.out.println("4.  Finish Adding Items");

            int choice = getIntInput("Choose item type: ");

            switch (choice) {
                case 1:
                    orderItems.add(createPizzaWithOptions());
                    break;
                case 2:
                    orderItems.add(createBurgerWithOptions());
                    break;
                case 3:
                    orderItems.add(createPastaWithOptions());
                    break;
                case 4:
                    addingItems = false;
                    break;
                default:
                    System.out.println(" Invalid choice.");
            }
        }

        if (orderItems.isEmpty()) {
            System.out.println(" No items added. Order cancelled.");
            return;
        }

        // Select order type
        String orderType = selectOrderType();

        // Place order
        Order order = placeOrder(orderItems, orderType);

        // Process payment
        processOrderPayment(order);
    }

    private String selectMenuType() {
        System.out.println("\n SELECT DIETARY MENU");
        System.out.println("1.Vegetarian Menu");
        System.out.println("2.Non-Vegetarian Menu");
        System.out.println("3.Kids Menu");

        int choice = getIntInput("Choose dietary menu: ");
        switch (choice) {
            case 1: return "VEGETARIAN";
            case 2: return "NON_VEGETARIAN";
            case 3: return "KIDS";
            default:
                System.out.println("Invalid choice, defaulting to Vegetarian.");
                return "VEGETARIAN";
        }
    }

    private String selectStyleType() {
        System.out.println("\n SELECT CUISINE STYLE");
        System.out.println("1.Italian Style");
        System.out.println("2.Eastern Style");
        System.out.println("3.Classic Style");

        int choice = getIntInput("Choose cuisine style: ");
        switch (choice) {
            case 1: return "ITALIAN";
            case 2: return "EASTERN";
            case 3: return "CLASSIC";
            default:
                System.out.println(" Invalid choice, defaulting to Italian.");
                return "ITALIAN";
        }
    }

    private String selectOrderType() {
        System.out.println("\n SELECT ORDER TYPE");
        System.out.println("1.Dine In");
        System.out.println("2.Takeaway");
        System.out.println("3.Delivery");

        int choice = getIntInput("Choose order type: ");
        switch (choice) {
            case 1: return "DINE_IN";
            case 2: return "TAKEAWAY";
            case 3: return "DELIVERY";
            default:
                System.out.println("Invalid choice, defaulting to Dine In.");
                return "DINE_IN";
        }
    }

    private MenuItem createPizzaWithOptions() {
        System.out.println("\nSELECT PIZZA");

        // Show available pizzas with prices
        System.out.println("Available Pizzas:");
        System.out.println("1. Margherita - $12.99");
        System.out.println("2. Pepperoni - $15.99");
        System.out.println("3. Vegetarian - $14.99");
        System.out.println("4. BBQ Chicken - $16.99");
        System.out.println("5. Supreme - $18.99");

        int pizzaChoice = getIntInput("Choose pizza: ");
        String pizzaName = "";
        String description = "";
        double basePrice = 0.0;

        switch (pizzaChoice) {
            case 1: pizzaName = "Margherita"; description = "Classic tomato and cheese"; basePrice = 12.99; break;
            case 2: pizzaName = "Pepperoni"; description = "Spicy pepperoni with cheese"; basePrice = 15.99; break;
            case 3: pizzaName = "Vegetarian"; description = "Mixed fresh vegetables"; basePrice = 14.99; break;
            case 4: pizzaName = "BBQ Chicken"; description = "BBQ sauce with chicken"; basePrice = 16.99; break;
            case 5: pizzaName = "Supreme"; description = "All toppings included"; basePrice = 18.99; break;
            default:
                System.out.println("Invalid choice, defaulting to Margherita.");
                pizzaName = "Margherita"; description = "Classic tomato and cheese"; basePrice = 12.99;
        }

        // Select size
        System.out.println("\n SELECT PIZZA SIZE:");
        int sizeIndex = 1;
        for (String size : pizzaSizes.keySet()) {
            double sizePrice = pizzaSizes.get(size);
            System.out.printf("%d. %s (%s$%.2f)%n", sizeIndex++, size, sizePrice > 0 ? "+$" : "", sizePrice);
        }

        int sizeChoice = getIntInput("Choose size: ");
        String selectedSize = "";
        double sizePrice = 0.0;

        if (sizeChoice >= 1 && sizeChoice <= pizzaSizes.size()) {
            selectedSize = (String) pizzaSizes.keySet().toArray()[sizeChoice - 1];
            sizePrice = pizzaSizes.get(selectedSize);
        } else {
            System.out.println("Invalid choice, defaulting to Medium.");
            selectedSize = "Medium";
        }

        double totalPrice = basePrice + sizePrice;

        // Use the pizza factory from the style factory
        PizzaFactory pizzaFactory = styleFactory.getPizzaFactory();
        MenuItem pizza = pizzaFactory.createPizza(pizzaName, description, totalPrice, selectedSize);

        System.out.printf("Selected: %s %s Pizza - $%.2f%n", selectedSize, pizzaName, totalPrice);

        // Add customizations
        return addCustomizationsToItem(pizza, "pizza");
    }

    private MenuItem createBurgerWithOptions() {
        System.out.println("\nSELECT BURGER");

        // Show available burgers with prices
        System.out.println("Available Burgers:");
        System.out.println("1. Classic Burger - $10.99");
        System.out.println("2. Cheese Burger - $12.99");
        System.out.println("3. Bacon Burger - $14.99");
        System.out.println("4. Veggie Burger - $9.99");
        System.out.println("5. Deluxe Burger - $16.99");

        int burgerChoice = getIntInput("Choose burger: ");
        String burgerName = "";
        String description = "";
        double basePrice = 0.0;

        switch (burgerChoice) {
            case 1: burgerName = "Classic Burger"; description = "Beef patty with lettuce and tomato"; basePrice = 10.99; break;
            case 2: burgerName = "Cheese Burger"; description = "Beef patty with cheese"; basePrice = 12.99; break;
            case 3: burgerName = "Bacon Burger"; description = "Beef patty with crispy bacon"; basePrice = 14.99; break;
            case 4: burgerName = "Veggie Burger"; description = "Vegetable patty with fresh veggies"; basePrice = 9.99; break;
            case 5: burgerName = "Deluxe Burger"; description = "All premium ingredients"; basePrice = 16.99; break;
            default:
                System.out.println("Invalid choice, defaulting to Classic Burger.");
                burgerName = "Classic Burger"; description = "Beef patty with lettuce and tomato"; basePrice = 10.99;
        }

        // Use the burger factory from the style factory
        BurgerFactory burgerFactory = styleFactory.getBurgerFactory();
        MenuItem burger = burgerFactory.createBurger(burgerName, description, basePrice);

        System.out.printf("Selected: %s - $%.2f%n", burgerName, basePrice);

        // Add customizations
        return addCustomizationsToItem(burger, "burger");
    }

    private MenuItem createPastaWithOptions() {
        System.out.println("\nSELECT PASTA");

        // Show available pastas with prices
        System.out.println("Available Pastas:");
        System.out.println("1. Spaghetti Marinara - $11.99");
        System.out.println("2. Fettuccine Alfredo - $13.99");
        System.out.println("3. Penne Arrabbiata - $12.99");
        System.out.println("4. Lasagna - $15.99");
        System.out.println("5. Carbonara - $14.99");

        int pastaChoice = getIntInput("Choose pasta: ");
        String pastaName = "";
        String description = "";
        double basePrice = 0.0;

        switch (pastaChoice) {
            case 1: pastaName = "Spaghetti Marinara"; description = "Tomato sauce with herbs"; basePrice = 11.99; break;
            case 2: pastaName = "Fettuccine Alfredo"; description = "Creamy Alfredo sauce"; basePrice = 13.99; break;
            case 3: pastaName = "Penne Arrabbiata"; description = "Spicy tomato sauce"; basePrice = 12.99; break;
            case 4: pastaName = "Lasagna"; description = "Layered pasta with cheese and meat"; basePrice = 15.99; break;
            case 5: pastaName = "Carbonara"; description = "Creamy sauce with bacon"; basePrice = 14.99; break;
            default:
                System.out.println("Invalid choice, defaulting to Spaghetti Marinara.");
                pastaName = "Spaghetti Marinara"; description = "Tomato sauce with herbs"; basePrice = 11.99;
        }

        // Use the pasta factory from the style factory
        PastaFactory pastaFactory = styleFactory.getPastaFactory();
        MenuItem pasta = pastaFactory.createPasta(pastaName, description, basePrice);

        System.out.printf("Selected: %s - $%.2f%n", pastaName, basePrice);

        // Add customizations
        return addCustomizationsToItem(pasta, "pasta");
    }

    private MenuItem addCustomizationsToItem(MenuItem item, String itemType) {
        MenuItem currentItem = item;
        boolean customizing = true;

        System.out.println("\nCUSTOMIZE YOUR " + itemType.toUpperCase());

        while (customizing) {
            System.out.println("\nAvailable Customizations:");
            System.out.println("1. Add Sauces");
            System.out.println("2. Add Toppings");
            System.out.println("3. Finish Customizing");

            int choice = getIntInput("Choose customization: ");

            switch (choice) {
                case 1:
                    currentItem = addSauces(currentItem);
                    break;
                case 2:
                    currentItem = addToppings(currentItem);
                    break;
                case 3:
                    customizing = false;
                    break;
                default:
                    System.out.println("Invalid choice.");
            }
        }

        return currentItem;
    }

    private MenuItem addSauces(MenuItem item) {
        MenuItem currentItem = item;
        boolean addingSauces = true;

        System.out.println("\nSELECT SAUCES:");

        while (addingSauces) {
            int sauceIndex = 1;
            for (String sauce : sauces.keySet()) {
                double price = sauces.get(sauce);
                System.out.printf("%d. %s (+$%.2f)%n", sauceIndex++, sauce, price);
            }
            System.out.printf("%d. Done adding sauces%n", sauceIndex);

            int choice = getIntInput("Choose sauce: ");

            if (choice >= 1 && choice <= sauces.size()) {
                String selectedSauce = (String) sauces.keySet().toArray()[choice - 1];
                double saucePrice = sauces.get(selectedSauce);
                currentItem = new SauceAddOn(currentItem, selectedSauce);
                System.out.printf("Added %s (+$%.2f)%n", selectedSauce, saucePrice);
                System.out.printf("Current total: $%.2f%n", currentItem.getPrice());
            } else if (choice == sauces.size() + 1) {
                addingSauces = false;
            } else {
                System.out.println("Invalid choice.");
            }
        }

        return currentItem;
    }

    private MenuItem addToppings(MenuItem item) {
        MenuItem currentItem = item;
        boolean addingToppings = true;

        System.out.println("\nSELECT TOPPINGS:");

        while (addingToppings) {
            int toppingIndex = 1;
            for (String topping : toppings.keySet()) {
                double price = toppings.get(topping);
                System.out.printf("%d. %s (+$%.2f)%n", toppingIndex++, topping, price);
            }
            System.out.printf("%d. Done adding toppings%n", toppingIndex);

            int choice = getIntInput("Choose topping: ");

            if (choice >= 1 && choice <= toppings.size()) {
                String selectedTopping = (String) toppings.keySet().toArray()[choice - 1];
                double toppingPrice = toppings.get(selectedTopping);
                currentItem = new ToppingAddOn(currentItem, selectedTopping, toppingPrice);
                System.out.printf("Added %s (+$%.2f)%n", selectedTopping, toppingPrice);
                System.out.printf("Current total: $%.2f%n", currentItem.getPrice());
            } else if (choice == toppings.size() + 1) {
                addingToppings = false;
            } else {
                System.out.println("Invalid choice.");
            }
        }

        return currentItem;
    }

    public void setMenuType(String menuType) {
        this.menuFactory = menuFactories.get(menuType.toUpperCase());
        if (menuFactory == null) {
            throw new IllegalArgumentException("Invalid menu type: " + menuType);
        }
        System.out.println("Dietary menu set to: " + menuType);
    }

    public void setStyleType(String styleType) {
        if (menuFactory == null) {
            throw new IllegalStateException("Please select a dietary menu first!");
        }

        switch (styleType.toUpperCase()) {
            case "ITALIAN":
                this.styleFactory = menuFactory.getItalianStyle();
                break;
            case "EASTERN":
                this.styleFactory = menuFactory.getEasternStyle();
                break;
            case "CLASSIC":
                this.styleFactory = menuFactory.getClassicStyle();
                break;
            default:
                throw new IllegalArgumentException("Invalid style type: " + styleType);
        }
        System.out.println("Cuisine style set to: " + styleType);
    }

    public Order placeOrder(List<MenuItem> items, String orderType) {
        String orderId = "ORD" + String.format("%04d", orderCounter++);

        System.out.println("\nPLACING ORDER #" + orderId);
        System.out.println("Items in order:");
        items.forEach(MenuItem::display);

        // Build order to calculate subtotal
        Order.OrderBuilder tempBuilder = new Order.OrderBuilder(orderId);
        for (MenuItem item : items) {
            tempBuilder.addItem(item);
        }

        Order tempOrder = tempBuilder
                .setOrderType(orderType)
                .setTax(0.0)
                .applyDiscount(0.0)
                .calculateTotal()
                .build();

        double subtotal = tempOrder.getSubtotal();
        System.out.printf("Subtotal: $%.2f%n", subtotal);

        // Calculate and display all applicable discounts with debug info
        double totalDiscount = calculateTotalDiscountWithDebug(tempOrder);

        if (totalDiscount > 0) {
            System.out.printf("TOTAL DISCOUNT: $%.2f%n", totalDiscount);
        } else {
            System.out.println("No discounts applicable");
        }

        // Build final order with discounts
        Order.OrderBuilder finalBuilder = new Order.OrderBuilder(orderId);
        for (MenuItem item : items) {
            finalBuilder.addItem(item);
        }

        Order order = finalBuilder.setOrderType(orderType)
                .setTax(0.1) // 10% tax
                .applyDiscount(totalDiscount)
                .calculateTotal()
                .build();
        order.setStatus("PLACED");

        System.out.printf("Tax (10%%): $%.2f%n", order.getTax());
        System.out.printf("FINAL TOTAL: $%.2f%n", order.getTotal());

        // Notify observers
        orderSubject.notifyObservers(order);

        System.out.println("Order placed successfully!");
        return order;
    }

    private double calculateTotalDiscountWithDebug(Order order) {
        double totalDiscount = 0.0;

        System.out.println("Checking for applicable discounts:");

        // Debug: Show order composition by category
        Map<String, Double> categoryTotals = new HashMap<>();
        for (MenuItem item : order.getItems()) {
            String category = item.getCategory();
            categoryTotals.put(category, categoryTotals.getOrDefault(category, 0.0) + item.getPrice());
        }

        System.out.println("Order composition by category:");
        for (Map.Entry<String, Double> entry : categoryTotals.entrySet()) {
            System.out.printf("   - %s: $%.2f%n", entry.getKey(), entry.getValue());
        }

        for (DiscountStrategy strategy : discountStrategies) {
            double strategyDiscount = strategy.calculateDiscount(order);
            if (strategyDiscount > 0) {
                System.out.printf("   APPLIED %s: -$%.2f%n",
                        strategy.getDescription(),
                        strategyDiscount);
                totalDiscount += strategyDiscount;
            } else {
                // Show why discount wasn't applied for debugging
                if (strategy instanceof CategoryDiscountStrategy) {
                    CategoryDiscountStrategy catStrategy = (CategoryDiscountStrategy) strategy;
                    String targetCategory = catStrategy.getCategory();
                    double categoryTotal = order.getItems().stream()
                            .filter(item -> item.getCategory().equalsIgnoreCase(targetCategory))
                            .mapToDouble(MenuItem::getPrice)
                            .sum();
                    if (categoryTotal == 0) {
                        System.out.printf("   SKIPPED %s (no %s items in order)%n",
                                catStrategy.getDescription(), targetCategory);
                    }
                } else if (strategy instanceof PercentageDiscountStrategy) {
                    PercentageDiscountStrategy percStrategy = (PercentageDiscountStrategy) strategy;
                    if (order.getSubtotal() < percStrategy.getMinAmount()) {
                        System.out.printf("   SKIPPED %s (need: $%.2f, have: $%.2f)%n",
                                percStrategy.getDescription(),
                                percStrategy.getMinAmount(), order.getSubtotal());
                    }
                } else if (strategy instanceof FixedAmountDiscountStrategy) {
                    FixedAmountDiscountStrategy fixedStrategy = (FixedAmountDiscountStrategy) strategy;
                    if (order.getSubtotal() < fixedStrategy.getMinAmount()) {
                        System.out.printf("   SKIPPED %s (need: $%.2f, have: $%.2f)%n",
                                fixedStrategy.getDescription(),
                                fixedStrategy.getMinAmount(), order.getSubtotal());
                    }
                }
            }
        }

        return totalDiscount;
    }

    private void processOrderPayment(Order order) {
        System.out.println("\nPROCESSING PAYMENT");
        System.out.printf("Order Total: $%.2f%n", order.getTotal());

        System.out.println("\nSELECT PAYMENT METHOD");
        System.out.println("1. Credit Card");
        System.out.println("2. Cash");
        System.out.println("3. Mobile Wallet");

        int choice = getIntInput("Choose payment method: ");
        PaymentStrategy paymentStrategy = null;

        switch (choice) {
            case 1:
                paymentStrategy = new CreditCardPayment("****-****-****-1234", "12/25", "123");
                break;
            case 2:
                paymentStrategy = new CashPayment();
                break;
            case 3:
                paymentStrategy = new MobileWalletPayment("PayPal", "***-***-7890");
                break;
            default:
                System.out.println("Invalid choice, using Cash.");
                paymentStrategy = new CashPayment();
        }

        boolean success = processPayment(order, paymentStrategy);
        if (success) {
            System.out.println("Order completed successfully!");
        }
    }

    public boolean processPayment(Order order, PaymentStrategy paymentStrategy) {
        boolean success = paymentStrategy.processPayment(order.getTotal());
        if (success) {
            order.setStatus("PAID");
            orderSubject.notifyObservers(order);
            generateReceipt(order, paymentStrategy);
        }
        return success;
    }

    private void generateReceipt(Order order, PaymentStrategy paymentStrategy) {
        System.out.println("\n === RESTAURANT RECEIPT ===");
        System.out.printf("Order ID: %s%n", order.getOrderId());
        System.out.printf("Order Type: %s%n", order.getOrderType());
        System.out.println("Items:");
        for (MenuItem item : order.getItems()) {
            System.out.printf("   - %s [%s Style]: $%.2f%n", item.getName(), item.getStyle(), item.getPrice());
        }
        System.out.printf("Subtotal: $%.2f%n", order.getSubtotal());
        System.out.printf("Tax (10%%): $%.2f%n", order.getTax());
        System.out.printf("Discount: $%.2f%n", order.getDiscount());
        System.out.printf("Total: $%.2f%n", order.getTotal());
        System.out.printf("Payment Method: %s%n", paymentStrategy.getPaymentMethod());
        System.out.println("Thank you for your order!\n");
    }

    public void displayAvailableDiscounts() {
        System.out.println("\nAVAILABLE DISCOUNTS:");
        for (DiscountStrategy strategy : discountStrategies) {
            System.out.println("  - " + strategy.getDescription());
        }
    }

    private void displayMenuSamples() {
        System.out.println("\nMENU SAMPLES");

        // Vegetarian samples
        System.out.println("\nVEGETARIAN MENU SAMPLES:");
        MenuFactory vegFactory = new VegetarianMenuFactory();

        System.out.println("\n🇮🇹 Italian Style:");
        StyleFactory italianVeg = vegFactory.getItalianStyle();
        italianVeg.getPizzaFactory().createPizza("Margherita", "Fresh tomatoes & basil", 12.99, "Medium").display();
        italianVeg.getBurgerFactory().createBurger("Portobello Burger", "Grilled mushroom", 10.99).display();

        System.out.println("\nEastern Style:");
        StyleFactory easternVeg = vegFactory.getEasternStyle();
        easternVeg.getPizzaFactory().createPizza("Spicy Veggie", "Mixed vegetables & spices", 14.99, "Medium").display();
        easternVeg.getPastaFactory().createPasta("Curry Noodles", "Spicy vegetable curry", 13.99).display();

        // Non-Vegetarian samples
        System.out.println("\nNON-VEGETARIAN MENU SAMPLES:");
        MenuFactory nonVegFactory = new NonVegetarianMenuFactory();

        System.out.println("\n🇮🇹 Italian Style:");
        StyleFactory italianNonVeg = nonVegFactory.getItalianStyle();
        italianNonVeg.getPizzaFactory().createPizza("Pepperoni", "Spicy pepperoni & cheese", 15.99, "Large").display();
        italianNonVeg.getPastaFactory().createPasta("Chicken Alfredo", "Creamy chicken pasta", 16.99).display();

        System.out.println("\n Classic Style:");
        StyleFactory classicNonVeg = nonVegFactory.getClassicStyle();
        classicNonVeg.getBurgerFactory().createBurger("Beef Classic", "Juicy beef patty", 12.99).display();
        classicNonVeg.getPastaFactory().createPasta("Meat Pasta", "With meat sauce", 14.99).display();

        // Kids samples
        System.out.println("\nKIDS MENU SAMPLES:");
        MenuFactory kidsFactory = new KidsMenuFactory();

        System.out.println("\n🇮🇹 Italian Style:");
        StyleFactory italianKids = kidsFactory.getItalianStyle();
        italianKids.getPizzaFactory().createPizza("Kids Cheese Pizza", "Simple cheese pizza", 8.99, "Small").display();
        italianKids.getPastaFactory().createPasta("Mini Pasta", "Small portion pasta", 7.99).display();

        System.out.println("\n🏛️ Classic Style:");
        StyleFactory classicKids = kidsFactory.getClassicStyle();
        classicKids.getBurgerFactory().createBurger("Classic Kids Burger", "Simple chicken burger", 6.49).display();
    }

    // Utility methods for input
    private int getIntInput(String prompt) {
        System.out.print(prompt);
        while (!scanner.hasNextInt()) {
            System.out.println("Please enter a valid number.");
            scanner.next();
            System.out.print(prompt);
        }
        int input = scanner.nextInt();
        scanner.nextLine(); // consume newline
        return input;
    }


    private boolean getYesNoInput(String prompt) {
        System.out.print(prompt);
        String input = scanner.nextLine().toLowerCase();
        return input.equals("y") || input.equals("yes");
    }
}