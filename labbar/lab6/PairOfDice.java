public class PairOfDice {
    private Die die1;
    private Die die2;

    public PairOfDice() {
        this.die1 = new Die(numberOfSides);
        this.die2 = new Die(numberOfSides);
    }

    public int roll() {
        int result1 = die1.roll();
        int result2 = die2.roll();
        return result1 + result2;
    }

    public int getDie1() {
        return die1.get;
    }

    public int getDie2() {
        return die2.get;
    }

    public String toString() {
        return "Dice(" + die1.get() +", " + die2.get() ")";
    }
}