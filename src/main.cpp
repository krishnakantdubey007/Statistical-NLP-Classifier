// ============================================================================
// Statistical NLP Text Classifier Engine
// main.cpp
// Developer : Krishnakant Dubey
// Language  : C++17
// ============================================================================

#include <iostream>
#include <string>
#include <vector>
#include <limits>

#include "DataLoader.hpp"
#include "NaiveBayes.hpp"

// ============================================================================
// Banner
// ============================================================================

void printBanner()
{
    std::cout << "\n";
    std::cout << "=========================================================\n";
    std::cout << "        Statistical NLP Text Classifier Engine\n";
    std::cout << "         Multinomial Naive Bayes (C++17)\n";
    std::cout << "=========================================================\n";
}

// ============================================================================
// Menu
// ============================================================================

void printMenu()
{
    std::cout << "\n";
    std::cout << "=========================================================\n";
    std::cout << "1. Predict a Message\n";
    std::cout << "2. Show Model Statistics\n";
    std::cout << "3. About Project\n";
    std::cout << "4. Exit\n";
    std::cout << "=========================================================\n";
    std::cout << "Enter your choice : ";
}

// ============================================================================
// About Project
// ============================================================================

void showAbout()
{
    std::cout << "\n";
    std::cout << "================== ABOUT PROJECT ==================\n";
    std::cout << "Project Name : Statistical NLP Text Classifier\n";
    std::cout << "Algorithm    : Multinomial Naive Bayes\n";
    std::cout << "Language     : C++17\n";
    std::cout << "Dataset      : UCI SMS Spam Collection\n";
    std::cout << "Developer    : Krishnakant Dubey\n";
std::cout << "GitHub       : github.com/krishnakantdubey007\n";
    std::cout << "===================================================\n";
}
// ============================================================================
// Predict Message
// ============================================================================

void predictMessage(NaiveBayes& classifier)
{
    std::string message;

    std::cout << "\nEnter your message:\n";
    std::cout << "> ";

    std::getline(std::cin, message);

    if (message.empty())
    {
        std::cout << "\n[!] Message cannot be empty.\n";
        return;
    }

    std::string result = classifier.predict(message);

    std::cout << "\n";
    std::cout << "=========================================================\n";
    std::cout << "Input Message\n";
    std::cout << "---------------------------------------------------------\n";
    std::cout << message << "\n\n";

    std::cout << "Prediction\n";
    std::cout << "---------------------------------------------------------\n";

    if      (result == "spam")
    {
        std::cout << "SPAM\n";
    }
    else if (result == "ham")
    {
        std::cout << "HAM\n";
    }
    else if (result == "unknown")
    {
        std::cout << "[!] Input contained no recognisable words. Please try again.\n";
    }
    else if (result == "untrained")
    {
        std::cout << "[!] Model is not trained. Load a dataset first.\n";
    }
    else
    {
        std::cout << "[!] Unexpected result: " << result << "\n";
    }

    std::cout << "=========================================================\n";
}

// ============================================================================
// Show Statistics
// ============================================================================

void showStatistics(const NaiveBayes& classifier,
                    const std::vector<Message>& dataset)
{
    std::cout << "\n";
    std::cout << "================ MODEL STATISTICS =================\n";

    std::cout << "Total Messages : "
              << dataset.size() << "\n";

    std::cout << "Spam Messages  : "
              << classifier.getSpamMessageCount() << "\n";

    std::cout << "Ham Messages   : "
              << classifier.getHamMessageCount() << "\n";

    std::cout << "Vocabulary     : "
              << classifier.getVocabularySize() << "\n";

    std::cout << "===================================================\n";
}
// ============================================================================
// Main Function
// ============================================================================

int main()
{
    try
    {
        printBanner();

        const std::string datasetPath = "data/SMSSpamCollection";

        std::cout << "\nLoading dataset...\n";

        DataLoader loader;
        std::vector<Message> dataset =
            loader.loadDataset(datasetPath);

        std::cout << "[ok] Dataset Loaded Successfully\n";
        std::cout << "    Total Messages : "
                  << dataset.size()
                  << "\n";

        std::cout << "\nTraining model...\n";

        NaiveBayes classifier;
        classifier.train(dataset);

        std::cout << "[ok] Model Trained Successfully\n";

        int choice;

        while (true)
        {
            printMenu();

            if (!(std::cin >> choice))
            {
                std::cin.clear();

                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(),
                    '\n');

                std::cout << "\nInvalid input. Please enter a number.\n";

                continue;
            }

            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n');

            switch (choice)
            {                case 1:
                {
                    predictMessage(classifier);
                    break;
                }

                case 2:
                {
                    showStatistics(classifier, dataset);
                    break;
                }

                case 3:
                {
                    showAbout();
                    break;
                }

                case 4:
                {
                    std::cout << "\n";
                    std::cout << "=========================================================\n";
                    std::cout << "Thank you for using Statistical NLP Text Classifier.\n";
                    std::cout << "Goodbye!\n";
                    std::cout << "=========================================================\n";

                    return 0;
                }

                default:
                {
                    std::cout << "\nInvalid choice. Please enter 1 to 4.\n";
                    break;
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "\n=========================================================\n";
        std::cerr << "ERROR : " << ex.what() << "\n";
        std::cerr << "=========================================================\n";

        return 1;
    }

    return 0;
}