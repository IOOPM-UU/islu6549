public class Store {

    private Register registers[];

    public Store() {
        registers = new Register[3];

        for (int i = 0; i < registers.length; i++) {
            registers[i] = new Register(new Queue());
        }

        registers[0].open();
    }

    public int getAverageQueueLength() {
        int registerLength = getRegisterLength();
        int median = 0;
        
        if (getRegisterLength() == 0) {
            return median;
        }

        for (int i = 0; i < registerLength; i++) {
            median += registers[i].getQueueLength();
        }

        return (median / registerLength);
    }

    public void newCustomer(Customer c) {

        int shortestQueue = registers[0].getQueueLength();
        Register shortesRegister = registers[0];

        for(int i = 0; i < getRegisterLength(); i++) {

            if (registers[i].getQueueLength() < shortestQueue) {

                shortestQueue = registers[i].getQueueLength();
                shortesRegister = registers[i];
            }
        }

        shortesRegister.addToQueue(c);
    }

    private int getRegisterLength() {
        int count = 0;

        while (count < registers.length && registers[count].isOpen()) {
            count++;
        }

        return count;
    }

    public void openNewRegister() {
    int count = getRegisterLength();

        if (count < registers.length) {
            registers[count].open();
        }
    }

    public Customer[] getDoneCustomers() {

        Customer doneCustomers[] = new Customer[registers.length];
        int index = 0;

        for (int i = 0; i < getRegisterLength(); i++) {

            if(registers[i].hasCustomers()) {

                if(registers[i].getCurrentCustomer().isDone()) {
                    // dequeue and to be returned
                    doneCustomers[index] = registers[i].removeCurrentCustomer();
                    index++;
                }
            }
        }
        return doneCustomers;
    }

    public void step() {
        for (int i = 0; i < getRegisterLength(); i++) {
            registers[i].step();
        }
    }
} 