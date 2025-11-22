public class CheeseDecorator extends MenuItemDecorator {

    public CheeseDecorator(MenuItem item) {
        super(item);
    }

    @Override
    public String getName() {
        return item.getName() + " + Cheese";
    }

    @Override
    public double getPrice() {
        return item.getPrice() + 10;
    }
}
