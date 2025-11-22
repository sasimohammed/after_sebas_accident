import java.util.*;

public class VegeterianMenu implements Menu {

    private final List<MenuItem> items = List.of(
            new MenuItem("Green Salad", 40, Category.STARTER),
            new MenuItem("Veg Soup", 35, Category.STARTER),
            new MenuItem("Veg Pizza", 120, Category.MAIN),
            new MenuItem("Pasta Alfredo", 100, Category.MAIN),
            new MenuItem("Orange Juice", 30, Category.DRINK)
    );

    @Override
    public String getName() { return "Veg"; }

    @Override
    public List<Category> getAvailableCategories() {
        return List.of(Category.STARTER, Category.MAIN, Category.DRINK);
    }

    @Override
    public List<MenuItem> getItemsByCategory(Category category) {
        return items.stream().filter(i -> i.getCategory() == category).toList();
    }
}
