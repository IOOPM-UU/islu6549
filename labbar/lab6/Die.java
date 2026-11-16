import java.util.Scanner;

public class Die {
    private int numberOfSides;
    private int value;

    public PairDie() {
        this.numberOfSides = 6;
        this.roll();  
    }

    public Die(int numberOfSides) {
        if (numberOfSides < 1) {
            throw new IllegalArgumentException( numberOfSides + " is an invalid number of sides");
        }

        this.numberOfSides = numberOfSides;
        this.roll();
    }

    public int roll() {
        this.value = (int) (Math.random() * numberOfSides) + 1;
        return this.get();
    }

    public int get() {
        return value;
    }

    public String toString() {
        return "Die(" + value + ")";
    }

    public boolean equals(Die otherDie) {
        return this.value == otherDie.value;
    }

    public static void main(String [] args) {
        Scanner sc = new Scanner(System.in);
        System.out.print("Number of sides: ");
        int sides = sc.nextInt();
        Die d = new Die(sides);
        System.out.println("Alea iacta est: " + d.roll());
    }
}
