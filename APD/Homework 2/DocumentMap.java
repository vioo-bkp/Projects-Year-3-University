// Nume: Mateescu Viorel-Cristian
// Grupa: 332CB
import java.util.List;
import java.util.Map;
import java.util.concurrent.CopyOnWriteArrayList;

// The Result of a Map Task.
public class DocumentMap {
    private String documentName;
    private CopyOnWriteArrayList<Map<Integer, Integer>> fragmentsWordLengths;
    private CopyOnWriteArrayList<List<String>> maxLengthWords;

    public DocumentMap() {
        this.fragmentsWordLengths = new CopyOnWriteArrayList<>();
        this.maxLengthWords = new CopyOnWriteArrayList<>();
    }

    public CopyOnWriteArrayList<Map<Integer, Integer>> getFragmentsWordLengths() {
        return fragmentsWordLengths;
    }

    public void setFragmentsWordLengths(CopyOnWriteArrayList<Map<Integer, Integer>> fragmentsWordLengths) {
        this.fragmentsWordLengths = fragmentsWordLengths;
    }

    public CopyOnWriteArrayList<List<String>> getMaxLengthWords() {
        return maxLengthWords;
    }

    public void setMaxLengthWords(CopyOnWriteArrayList<List<String>> maxLengthWords) {
        this.maxLengthWords = maxLengthWords;
    }

    public String getDocumentName() {
        return documentName;
    }

    public void setDocumentName(String documentName) {
        this.documentName = documentName;
    }
}
