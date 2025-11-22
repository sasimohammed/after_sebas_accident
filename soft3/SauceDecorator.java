public class SauceDecorator extends MenuItemDecorator {

    public SauceDecorator(MenuItem item) {
        super(item);
    }

    @Override
    public String getName() {
        return item.getName() + " + Extra Sauce";
    }

    @Override
    public double getPrice() {
        return item.getPrice() + 5;
    }
}
