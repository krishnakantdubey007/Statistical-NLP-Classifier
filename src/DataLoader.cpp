#include "DataLoader.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

/**
 * Loads a dataset from a TSV file.
 * 
 * Explanation of the implementation:
 * 1. Open the file using std::ifstream.
 * 2. Check if the file is successfully opened. If not, throw std::runtime_error.
 * 3. Read the file line-by-line using std::getline() inside a while loop.
 * 4. For each line, check if it's empty to gracefully skip blank lines.
 * 5. Wrap the line in a std::stringstream to facilitate parsing.
 * 6. Use std::getline() with a '\t' delimiter to extract the label first,
 *    and then read the rest of the stream as the text.
 * 7. Clean up Windows-style carriage return ('\r') from the end of the text
 *    if present (handles cross-platform line ending issues).
 * 8. Only store and push the Message if both label and text are non-empty.
 */
std::vector<Message> DataLoader::loadDataset(const std::string& filename) {
    std::vector<Message> dataset;
    std::ifstream infile(filename);

    // Step 1 & 2: Open file and verify success
    if (!infile.is_open()) {
        throw std::runtime_error("Error: Could not open dataset file: " + filename);
    }

    std::string line;
    // Step 3: Read line-by-line
    while (std::getline(infile, line)) {
        // Step 4: Skip completely empty lines
        if (line.empty()) {
            continue;
        }

        // Step 5: Wrap line in a stringstream
        std::stringstream lineStream(line);
        std::string label;
        std::string text;

        // Step 6: Parse tab-separated columns
        // std::getline(stream, string, delim) extracts up to the delimiter '\t'
        if (std::getline(lineStream, label, '\t')) {
            // Read the remaining part of the line as the text
            std::getline(lineStream, text);

            // Step 7: Handle carriage return '\r' at the end of the text
            // (common when files with CRLF are read on non-Windows platforms or vice versa)
            if (!text.empty() && text.back() == '\r') {
                text.pop_back();
            }

            // Step 8: Store valid Message objects
            if (!label.empty() && !text.empty()) {
                dataset.push_back(Message{label, text});
            }
        }
    }

    return dataset;
}
