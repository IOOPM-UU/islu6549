import java.util.Random;

public class Simulation {
    private Store store;
    private int time;
    private int intensity;
    private int maxGroceries;
    private int thresholdForNewRegister;

    private int customersServed;
    private int totalTimeSpent;

    private Random random;

    public Simulation(int intensity, int maxGroceries,
                    int thresholdForNewRegister) {
        this.store = new Store();
        this.time = 0;
        this.intensity = intensity;
        this.maxGroceries = maxGroceries;
        this.thresholdForNewRegister = thresholdForNewRegister;
        this.random = new Random();

        this.customersServed = 0;
        this.totalTimeSpent = 0;
    }

    @Override
    public String toString() {
        return "Time: " + time
                + "\nCustomers served: " + customersServed
                + "\nAverage time spent: " + getAverageTimeSpent();
    }

    public double getAverageTimeSpent() {
        if (customersServed == 0) {
            return 0;
        }

        return (double) totalTimeSpent / customersServed;
    }

    public int getCustomersServed() {
        return customersServed;
    }

    public int getTotalTimeSpent() {
        return totalTimeSpent;
    }

    public int getTime() {
        return time;
    }
    
    public void step() {

        store.step();

        if (random.nextInt(100) < intensity) {
            int groceries = random.nextInt(maxGroceries) + 1;
            store.newCustomer(new Customer(groceries, time));
        }

        if (store.getAverageQueueLength() > thresholdForNewRegister) {
            store.openNewRegister();
        }

        Customer[] done = store.getDoneCustomers();

        for (Customer c : done) {
            if (c != null) {
                customersServed++;
                totalTimeSpent += (time + 1) - c.getBornTime();
            }
        }

        time++;
    }
} 