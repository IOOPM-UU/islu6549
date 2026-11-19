public class Customer {

    private int bornTime;
    private int groceries;

    public Customer(int groceries, int bornTime) {
        if (groceries < 1) {
            throw new IllegalArgumentException("groceries must be at least 1, was " + groceries);
        }
        this.groceries = groceries;
        this.bornTime = bornTime;
    }

    public int getBornTime() {
        return bornTime;
    }

    public int serve() {
        return this.groceries--;
    }

    public boolean isDone() {
        return this.groceries == 0;
    }

    public static void main(String [] args) {
        Customer steve = new Customer(5, 0);
        while(!steve.isDone()) {
            steve.serve();
        }
    }
}