public class Register {
    private boolean open;
    private Queue queue;
    public Register(Queue queue) {
        this.open = false;
        this.queue = queue;
    }

    public boolean open() {
        open = true;
        return open;
    }

    public boolean closed() {
        open = false;
        return open;
    }

    public boolean isOpen() {
        return open;
    }

    public void step() {
        if (hasCustomers()) {
            queue.first().serve();
        }
    }

    public boolean hasCustomers() {
        return !queue.isEmpty();
    }

    public void addToQueue(Customer c) {
        queue.enqueue(c);
    }

    public Customer getCurrentCustomer() {
        return queue.first();
    }

    public Customer removeCurrentCustomer() {
        return queue.dequeue();
    }

    public int getQueueLength() {
        return queue.length();
    }
}