import java.util.*;

public class non_VegeterianMenu implements Menu {

    private final List<MenuItem> items = List.of(
            new MenuItem("Chicken Strips", 60, Category.STARTER),
            new MenuItem("Beef Burger", 140, Category.MAIN),
            new MenuItem("Coca Cola", 20, Category.DRINK)
    );

    @Override
    public String getName() { return "Non-Veg"; }

    @Override
    public List<Category> getAvailableCategories() {
        return List.of(Category.STARTER, Category.MAIN, Category.DRINK);
    }

    @Override
    public List<MenuItem> getItemsByCategory(Category category) {
        return items.stream().filter(i -> i.getCategory() == category).toList();
    }
}
