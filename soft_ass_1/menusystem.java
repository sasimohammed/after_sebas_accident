abstract class MenuItem {
    protected String name;
    protected String description;
    protected double price;
    protected String category; // Vegetarian, Non-Vegetarian, Kids
    protected String style;    // Italian, Eastern, Classic

    public MenuItem(String name, String description, double price, String category, String style) {
        this.name = name;
        this.description = description;
        this.price = price;
        this.category = category;
        this.style = style;
    }

    public String getName() { return name; }
    public String getDescription() { return description; }
    public double getPrice() { return price; }
    public String getCategory() { return category; }
    public String getStyle() { return style; }

    public abstract void display();
}

// 2. Food Items
class Pizza extends MenuItem {
    private String crustType;
    private String size;

    public Pizza(String name, String description, double price, String category, String style,
                 String crustType, String size) {
        super(name, description, price, category, style);
        this.crustType = crustType;
        this.size = size;
    }

    @Override
    public void display() {
        System.out.printf("%s [%s Style] (%s) - $%.2f - %s crust, %s size%n",
                name, style, description, price, crustType, size);
    }
}

class Burger extends MenuItem {
    private String pattyType;
    private boolean hasCheese;

    public Burger(String name, String description, double price, String category, String style,
                  String pattyType, boolean hasCheese) {
        super(name, description, price, category, style);
        this.pattyType = pattyType;
        this.hasCheese = hasCheese;
    }

    @Override
    public void display() {
        System.out.printf("%s [%s Style] (%s) - $%.2f - %s patty%s%n",
                name, style, description, price, pattyType,
                hasCheese ? " with cheese" : "");
    }
}

class Pasta extends MenuItem {
    private String pastaType;
    private String sauceType;

    public Pasta(String name, String description, double price, String category, String style,
                 String pastaType, String sauceType) {
        super(name, description, price, category, style);
        this.pastaType = pastaType;
        this.sauceType = sauceType;
    }

    @Override
    public void display() {
        System.out.printf("%s [%s Style] (%s) - $%.2f - %s pasta with %s sauce%n",
                name, style, description, price, pastaType, sauceType);
    }
}

// 3. Style Factory Interface (Small Factory inside big factory)
interface StyleFactory {
    MenuItem createPizza(String name, String description, double price, String size);
    MenuItem createBurger(String name, String description, double price);
    MenuItem createPasta(String name, String description, double price);
}

// 4. Concrete Style Factories
class ItalianStyleFactory implements StyleFactory {
    private String category;

    public ItalianStyleFactory(String category) {
        this.category = category;
    }

    @Override
    public MenuItem createPizza(String name, String description, double price, String size) {
        return new Pizza(name, description, price, category, "Italian", "Thin Crust", size);
    }

    @Override
    public MenuItem createBurger(String name, String description, double price) {
        String pattyType = category.equals("Vegetarian") ? "Portobello" : "Beef";
        return new Burger(name, description, price, category, "Italian", pattyType, true);
    }

    @Override
    public MenuItem createPasta(String name, String description, double price) {
        return new Pasta(name, description, price, category, "Italian", "Fresh", "Tomato Basil");
    }
}

class EasternStyleFactory implements StyleFactory {
    private String category;

    public EasternStyleFactory(String category) {
        this.category = category;
    }

    @Override
    public MenuItem createPizza(String name, String description, double price, String size) {
        return new Pizza(name, description, price, category, "Eastern", "Thick Crust", size);
    }

    @Override
    public MenuItem createBurger(String name, String description, double price) {
        String pattyType = category.equals("Vegetarian") ? "Spicy Vegetable" : "Chicken Tikka";
        return new Burger(name, description, price, category, "Eastern", pattyType, true);
    }

    @Override
    public MenuItem createPasta(String name, String description, double price) {
        return new Pasta(name, description, price, category, "Eastern", "Egg Noodles", "Spicy Curry");
    }
}

class ClassicStyleFactory implements StyleFactory {
    private String category;

    public ClassicStyleFactory(String category) {
        this.category = category;
    }

    @Override
    public MenuItem createPizza(String name, String description, double price, String size) {
        return new Pizza(name, description, price, category, "Classic", "Regular Crust", size);
    }

    @Override
    public MenuItem createBurger(String name, String description, double price) {
        String pattyType = category.equals("Vegetarian") ? "Vegetable" :
                category.equals("Kids") ? "Chicken" : "Beef";
        return new Burger(name, description, price, category, "Classic", pattyType, true);
    }

    @Override
    public MenuItem createPasta(String name, String description, double price) {
        return new Pasta(name, description, price, category, "Classic", "Traditional", "Creamy");
    }
}

// 5. Main Menu Factory Interface (Big Factory)
interface MenuFactory {
    StyleFactory getItalianStyle();
    StyleFactory getEasternStyle();
    StyleFactory getClassicStyle();
}

// 6. Concrete Main Factories
class VegetarianMenuFactory implements MenuFactory {
    @Override
    public StyleFactory getItalianStyle() {
        return new ItalianStyleFactory("Vegetarian");
    }

    @Override
    public StyleFactory getEasternStyle() {
        return new EasternStyleFactory("Vegetarian");
    }

    @Override
    public StyleFactory getClassicStyle() {
        return new ClassicStyleFactory("Vegetarian");
    }
}

class NonVegetarianMenuFactory implements MenuFactory {
    @Override
    public StyleFactory getItalianStyle() {
        return new ItalianStyleFactory("Non-Vegetarian");
    }

    @Override
    public StyleFactory getEasternStyle() {
        return new EasternStyleFactory("Non-Vegetarian");
    }

    @Override
    public StyleFactory getClassicStyle() {
        return new ClassicStyleFactory("Non-Vegetarian");
    }
}

class KidsMenuFactory implements MenuFactory {
    @Override
    public StyleFactory getItalianStyle() {
        return new ItalianStyleFactory("Kids");
    }

    @Override
    public StyleFactory getEasternStyle() {
        return new EasternStyleFactory("Kids");
    }

    @Override
    public StyleFactory getClassicStyle() {
        return new ClassicStyleFactory("Kids");
    }
}

// 7. Add-on Decorators
abstract class AddOnDecorator extends MenuItem {
    protected MenuItem menuItem;

    public AddOnDecorator(MenuItem menuItem, String name, String description, double price) {
        super(name, description, price, "AddOn", "AddOn");
        this.menuItem = menuItem;
    }

    @Override
    public abstract void display();

    @Override
    public double getPrice() {
        return menuItem.getPrice() + price;
    }

    public String getBaseItemName() {
        return menuItem.getName();
    }
}

class SauceAddOn extends AddOnDecorator {
    private String sauceType;

    public SauceAddOn(MenuItem menuItem, String sauceType) {
        super(menuItem, "Extra " + sauceType + " Sauce", "Additional " + sauceType + " sauce", 0.5);
        this.sauceType = sauceType;
    }

    @Override
    public void display() {
        System.out.printf(" %s with Extra %s Sauce - Total: $%.2f%n",
                menuItem.getName(), sauceType, getPrice());
    }
}

class ToppingAddOn extends AddOnDecorator {
    private String toppingType;

    public ToppingAddOn(MenuItem menuItem, String toppingType, double price) {
        super(menuItem, "Extra " + toppingType, "Additional " + toppingType + " topping", price);
        this.toppingType = toppingType;
    }

    @Override
    public void display() {
        System.out.printf("%s with Extra %s - Total: $%.2f%n",
                menuItem.getName(), toppingType, getPrice());
    }
}