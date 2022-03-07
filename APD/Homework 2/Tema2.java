// Nume: Mateescu Viorel-Cristian
// Grupa: 332CB
import java.io.*;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

public class Tema2 {
    public static void main(String[] args) {
        checkUsage(args);

        String data;
        List<DocumentMap> documentsInfo = null;
        List<String> documentNames = new ArrayList<>();

        int noWorkers, fragmentSize, noDocuments = -1;

        // Parse the number of workers.
        noWorkers = getAnInt(args[0]);

        // Executor Service for concurrent asynchronous Map tasks.
        AtomicInteger inQueue = new AtomicInteger(0);
        ExecutorService tpe = Executors.newFixedThreadPool(noWorkers);

        try {
            BufferedReader reader = new BufferedReader(new FileReader(args[1]));

            // The fragment Size.
            data = reader.readLine();
            fragmentSize = getAnInt(data);

            // The number of documents.
            data = reader.readLine();
            noDocuments = getAnInt(data);

            documentsInfo = new ArrayList<>(noDocuments);

            for (int i = 0; i < noDocuments; i++) {
                documentsInfo.add(i, new DocumentMap());

                // Parse document name.
                data = reader.readLine();
                String[] name = data.split("/");
                documentNames.add(name[name.length - 1]);

                // Create the Tasks for the Map Phase of the current Document.
                mapDocument(data, documentsInfo.get(i), fragmentSize, inQueue, tpe);
            }

            reader.close();
        } catch (FileNotFoundException e) {
            System.err.println("Input file " + args[1] + " can't be opened.");
            System.exit(-1);
        } catch (IOException e) {
            System.err.println("Encountered an error while working with a file.");
            System.exit(-1);
        }

        // Wait for the Map tasks to finish their execution.
        tpe.shutdown();
        try {
            tpe.awaitTermination(10, TimeUnit.MINUTES);
        } catch (InterruptedException e) {
            System.err.println("Encountered an error while waiting for the Executor to finish.");
            System.exit(-1);
        }

        // Create the Tasks for the Reduce Phase.
        Map<String, MapReduceResult> results = reducePhase(documentsInfo, noWorkers, noDocuments, documentNames);

        // Print the results.
        printMapReduceResults(args[2], results, documentNames);
    }

    // Function for creating the Map Tasks for a Document.
    private static void mapDocument(String data, DocumentMap documentInfo, int fragmentSize,
                                    AtomicInteger inQueue, ExecutorService tpe) throws IOException {
        // Get document size.
        RandomAccessFile file = new RandomAccessFile(data, "r");
        long documentSize = file.length();
        int offset = 0;

        // Create a Map Task for each fragment of the specified size, except the last fragment which may have a
        // shorter size.
        while (documentSize > 0) {
            inQueue.incrementAndGet();
            tpe.submit(new MapRunnable(data, documentInfo, offset, (int) Math.min(documentSize,
                    fragmentSize)));

            documentSize -= fragmentSize;
            offset += fragmentSize;
        }

        file.close();
    }

    // Function for creating the Reduce Tasks for a Document.
    private static Map<String, MapReduceResult> reducePhase(List<DocumentMap> documentsInfo, int noWorkers,
                                                            int noDocuments, List<String> documentNames) {
        ExecutorService tpe;
        AtomicInteger inQueue;
        Map<String, MapReduceResult> maps = new ConcurrentHashMap<>();

        // Executor Service for concurrent asynchronous Reduce tasks.
        inQueue = new AtomicInteger(0);
        tpe = Executors.newFixedThreadPool(noWorkers);

        // Create a Reduce Task for each document.
        for (int i = 0; i < noDocuments; i++) {
            inQueue.incrementAndGet();
            tpe.submit(new ReduceRunnable(documentsInfo.get(i), maps, documentNames.get(i)));
        }

        // Wait for the Reduce tasks to finish their execution.
        tpe.shutdown();
        try {
            tpe.awaitTermination(10, TimeUnit.MINUTES);
        } catch (InterruptedException e) {
            System.err.println("Encountered an error while waiting for the Executor to finish.");
            System.exit(-1);
        }

        return maps;
    }

    // Function for converting a String to an Integer.
    private static int getAnInt(String valueToParse) {
        int value = 0;
        try {
            value = Integer.parseInt(valueToParse);
        } catch (NumberFormatException e) {
            System.err.println("Argument " + valueToParse + " must be an integer.");
            System.exit(1);
        }
        return value;
    }

    // Function for checking if the program has been run correctly.
    private static void checkUsage(String[] args) {
        if (args.length < 3) {
            System.err.println("Usage: Tema2 <workers> <in_file> <out_file>");
            System.exit(-1);
        }
    }

    // Function for printing the results of the Map Reduce Algorithm.
    private static void printMapReduceResults(String outputFileName, Map<String, MapReduceResult> documentMaps,
                                              List<String> documentNames) {
        // Sort the documents in descending order by their ranks, if their ranks are equal their order should be the one
        // from the input file.
        List<Map.Entry<String, MapReduceResult>> results = new ArrayList<>(documentMaps.entrySet());
        results.sort((o1, o2) -> {
            int value = Double.compare(o2.getValue().getRank(), o1.getValue().getRank());
            return value == 0 ? (documentNames.indexOf(o1.getKey()) - documentNames.indexOf(o2.getKey())) : value;
        });

        BufferedWriter writer;
        try {
            writer = new BufferedWriter(new FileWriter(outputFileName));

            // Print the document name, the rank, the maximum length of a word and the number of words with the maximum
            // length for the current document.
            for (Map.Entry<String, MapReduceResult> documentResults : results) {
                String outputString = documentResults.getKey() + "," +
                        String.format("%.2f", documentResults.getValue().getRank()) + "," +
                        documentResults.getValue().getMaxLength() + "," +
                        documentResults.getValue().getNoMaxLengthWords() + "\n";

                writer.write(outputString);
            }

            writer.close();
        } catch (IOException e) {
            System.err.println("Encountered an error while printing the output.");
            System.exit(-1);
        }
    }
}
