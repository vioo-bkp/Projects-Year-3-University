// Nume: Mateescu Viorel-Cristian
// Grupa: 332CB
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.*;

// The Map Task.
public class MapRunnable implements Runnable {
    private static final String separators = "[;:/?~.,><`\\[\\]{}()!@#$%^&\\-_+'=*\"| \t\r\n\0]";

    private final String documentName;
    private final DocumentMap documentInfo;
    private int offset;
    private int fragmentSize;

    public MapRunnable(String documentName, DocumentMap documentInfo, int offset, int fragmentSize) {
        this.documentName = documentName;
        this.documentInfo = documentInfo;
        this.offset = offset;
        this.fragmentSize = fragmentSize;
    }

    @Override
    public void run() {
        String fragment;

        Map<Integer, Integer> fragmentWordDictionary = new HashMap<>();
        List<String> fragmentMaxLengthWords = new ArrayList<>();

        try {
            RandomAccessFile file = new RandomAccessFile(documentName, "r");
            file.seek(offset);

            // Read fragment size characters from file + offset.
            byte[] bytes = new byte[fragmentSize];
            file.read(bytes);
            fragment = new String(bytes);

            // Check if the fragment starts in the middle of a word and remove it if necessary.
            fragment = limitFragmentLeft(fragment, file);

            // If there are any characters left in the fragment after adjusting its left margin.
            if (fragmentSize != 0) {
                // Check if the fragment end in the middle of a word and remove it if necessary.
                fragment = limitFragmentRight(fragment, file);

                // Split the fragment into words and remove the null or 0 length words.
                List<String> words = new ArrayList<>(Arrays.asList(fragment.split(separators)));
                words.removeAll(Collections.singleton(null));
                words.removeAll(Collections.singleton(""));

                // Generate the word dictionary and determine the maximum length of a word and the words with this
                // length in the fragment.
                mapFragment(fragmentWordDictionary, fragmentMaxLengthWords, words);
            }

            // Add the Map Phase results of the current Task to the list of map results of the Current Document.
            documentInfo.getFragmentsWordLengths().add(fragmentWordDictionary);
            documentInfo.getMaxLengthWords().add(fragmentMaxLengthWords);
            documentInfo.setDocumentName(documentName);

            file.close();
        } catch (IOException e) {
            System.err.println("Encountered an error while reading a fragment from document " + documentName + ".");
            System.exit(-1);
        }
    }

    // Function for checking if the fragment is starting with an incomplete word and removing it if necessary.
    private String limitFragmentLeft(String fragment, RandomAccessFile file) throws IOException {
        char character;

        // Check if it isn't the first fragment in the document (it is impossible to start with an incomplete word if
        // it is) and if the fragment starts with a character which is not a separator.
        if (offset > 0 && separators.indexOf(fragment.charAt(0)) == -1) {
            // Move to the previous character in the document.
            file.seek(offset - 1);
            character = (char) file.readByte();

            // If it is not a separator then the ending of the word should be removed from the fragment.
            if (separators.indexOf(character) == -1) {
                // As long as there are any characters left in the fragment and the current character is not a
                // separator remove it from the fragment.
                while (fragmentSize > 0 && separators.indexOf(fragment.charAt(0)) == -1) {
                    fragment = fragment.substring(1);
                    fragmentSize--;
                    offset++;
                }
            }
        }

        return fragment;
    }

    // Function for checking if the fragment is ending with an incomplete word and removing it if necessary.
    private String limitFragmentRight(String fragment, RandomAccessFile file) throws IOException {
        char character;

        StringBuilder auxFragment = new StringBuilder();
        auxFragment.append(fragment);
        file.seek(fragmentSize + offset);

        // While the last character in the fragment is not a separator and the ending of the document hasn't been
        // reached add the next character to the fragment and check again.
        while (separators.indexOf(auxFragment.charAt(fragmentSize - 1)) == -1 &&
                fragmentSize + offset < file.length()) {
            character = (char) file.readByte();
            fragmentSize++;
            auxFragment.append(character);
        }

        fragment = auxFragment.toString();

        return fragment;
    }

    // Function for generating the word dictionary and determining the maximum length of a word and the words with this
    // length in the fragment.
    private void mapFragment(Map<Integer, Integer> fragmentWordDictionary, List<String> fragmentMaxLengthWords,
                             List<String> words) {
        int maxLength = 0;

        for (String word : words) {
            int wordLength = word.length();

            // Update the word length dictionary depending on whether the current length is already in the dictionary.
            if (fragmentWordDictionary.containsKey(wordLength)) {
                fragmentWordDictionary.put(wordLength, fragmentWordDictionary.get(wordLength) + 1);
            } else {
                fragmentWordDictionary.put(word.length(), 1);
            }

            // Check if the current word's length is greater or equal to the current maximum word length in the
            // fragment and update it and the list of maximum length words in this fragment if necessary.
            if (word.length() > maxLength) {
                maxLength = word.length();
                fragmentMaxLengthWords.clear();
                fragmentMaxLengthWords.add(word);
            } else if (word.length() == maxLength) {
                fragmentMaxLengthWords.add(word);
            }
        }
    }
}