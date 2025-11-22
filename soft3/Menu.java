import java.util.List;


public interface Menu {

    String getName();
    List<MenuItem> getItemsByCategory(Category category);
    List<Category> getAvailableCategories();
}
