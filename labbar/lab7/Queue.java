public class Queue {
    private class Node {
        Customer element;
        Node next;

        Node(Customer element) {
            this.element = element;
            this.next = null;
        }
    }

    private Node first;
    private Node last;

    public boolean isEmpty() {
        return first == null;
    }

    public int length() {
        int count = 0;
        Node current = first;

        while(current != null) {
            count++;
            current = current.next;
        }

        return count;
    }

    public void enqueue(Customer c) {
        Node newNode = new Node(c);

        if (first == null) {
            first = newNode;
            last = newNode;
        } else {
            last.next = newNode;
            last = newNode;
        }
    }

    public Customer dequeue() {
        if (first == null) {
        return null;
        }

        Customer oldCustomer = first.element;
        first = first.next;

        if (first == null) {
            last = null;
        }

        return oldCustomer;
    }

    public Customer first() {
        if (first == null) {
            throw new java.util.NoSuchElementException("Queue is empty");
        }
        return this.first.element;
    }
}