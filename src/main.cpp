#include <iostream>
#include "DataLoader.hpp"

int main()
{
    try
    {
        DataLoader loader;

        std::vector<Message> dataset =
            loader.loadDataset("data/SMSSpamCollection");

        std::cout << "Dataset Loaded Successfully!\n";
        std::cout << "Total Messages : "
                  << dataset.size() << "\n\n";

        std::cout << "First Message\n";
        std::cout << "-------------------------\n";
        std::cout << "Label : "
                  << dataset[0].label << "\n";

        std::cout << "Text : "
                  << dataset[0].text << "\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "Error : "
                  << e.what()
                  << std::endl;
    }

    return 0;
}