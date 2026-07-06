#ifndef DATALOADER_HPP
#define DATALOADER_HPP

#include <string>
#include <vector>

/**
 * @struct Message
 * @brief Represents a single labeled text example from the dataset.
 */
struct Message {
    std::string label; ///< The class label (e.g., "spam" or "ham")
    std::string text;  ///< The raw text content of the message
};

/**
 * @class DataLoader
 * @brief A helper class to load and parse text classification datasets from TSV files.
 * 
 * This class reads tab-separated values (TSV) file line-by-line.
 * Each valid line is parsed into a Message object containing a label and text.
 */
class DataLoader {
public:
    /**
     * @brief Loads a dataset from a TSV file.
     * 
     * The file is expected to have one message per line, formatted as:
     * label <tab> text
     * 
     * @param filename The path to the TSV file.
     * @return A vector of parsed Message objects.
     * @throws std::runtime_error if the file cannot be opened.
     */
    std::vector<Message> loadDataset(const std::string& filename);
};

#endif // DATALOADER_HPP
