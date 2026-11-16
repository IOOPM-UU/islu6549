import java.util.Scanner;

public class NameOrder {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);

        System.out.print("Enter first name: ");
        String name1 = sc.nextLine();

        System.out.print("Enter second name: ");
        String name2 = sc.nextLine();

        if (name1.compareTo(name2) < 0) {
            System.out.println(name1);
            System.out.println(name2);
        } else {
            System.out.println(name2);
            System.out.println(name1);
        }
    }
}