public abstract class MenuItemDecorator {

    protected final MenuItem item;

    public MenuItemDecorator(MenuItem item) {
        this.item = item;
    }

    public String getName() {
        return item.getName();
    }

    public double getPrice() {
        return item.getPrice();
    }

    public Category getCategory() {
        return item.getCategory();
    }
}
