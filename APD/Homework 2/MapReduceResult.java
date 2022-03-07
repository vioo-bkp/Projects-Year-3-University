// Nume: Mateescu Viorel-Cristian
// Grupa: 332CB
// The result of a Reduce Task.
public class MapReduceResult {
    private double rank;
    private int maxLength;
    private int noMaxLengthWords;

    public MapReduceResult(double rank, int maxLength, int noMaxLengthWords) {
        this.rank = rank;
        this.maxLength = maxLength;
        this.noMaxLengthWords = noMaxLengthWords;
    }

    public double getRank() {
        return rank;
    }

    public void setRank(double rank) {
        this.rank = rank;
    }

    public int getMaxLength() {
        return maxLength;
    }

    public void setMaxLength(int maxLength) {
        this.maxLength = maxLength;
    }

    public int getNoMaxLengthWords() {
        return noMaxLengthWords;
    }

    public void setNoMaxLengthWords(int noMaxLengthWords) {
        this.noMaxLengthWords = noMaxLengthWords;
    }
}
