#include "DataLoader.hpp"
#include <iostream>
#include <vector>
#include <cassert>

int main() {
    try {
        DataLoader loader;
        std::vector<Message> dataset = loader.loadDataset("tests/mock_data.tsv");

        std::cout << "Successfully loaded " << dataset.size() << " messages.\n";
        
        // Assertions to verify correctness
        assert(dataset.size() == 4);
        
        assert(dataset[0].label == "spam");
        assert(dataset[0].text == "Congratulations!!! You won FREE Lottery.");

        assert(dataset[1].label == "ham");
        assert(dataset[1].text == "Hey, what are you doing tonight?");

        assert(dataset[2].label == "spam");
        assert(dataset[2].text == "URGENT: Click here to claim your cash prize now!");

        assert(dataset[3].label == "ham");
        assert(dataset[3].text == "Let's meet for lunch tomorrow.");

        std::cout << "All assertions passed successfully!\n";

        // Print loaded messages
        for (const auto& msg : dataset) {
            std::cout << "[" << msg.label << "]: " << msg.text << "\n";
        }

    } catch (const std::exception& ex) {
        std::cerr << "Test failed with exception: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
