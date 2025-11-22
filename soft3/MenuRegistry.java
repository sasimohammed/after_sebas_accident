import java.util.*;

public class MenuRegistry {

    private static final Map<String, Menu> menus = new HashMap<>();  // in order to not violate the open closed principle

    public static void register(Menu menu) {
        menus.put(menu.getName().toLowerCase(), menu);
    }

    public static Menu getMenu(String name) {
        return menus.get(name.toLowerCase());
    }

    public static Set<String> getMenuNames() {
        return menus.keySet();
    }
}
