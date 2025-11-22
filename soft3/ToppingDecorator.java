public class ToppingDecorator extends MenuItemDecorator {

    public ToppingDecorator(MenuItem item) {
        super(item);
    }

    @Override
    public String getName() {
        return item.getName() + " + Toppings";
    }

    @Override
    public double getPrice() {
        return item.getPrice() + 15;
    }
}
