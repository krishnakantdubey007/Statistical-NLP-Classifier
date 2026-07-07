#include "NaiveBayes.hpp"

#include <cmath>
#include <unordered_set>

// ============================================================================
// Constructor
// ============================================================================

NaiveBayes::NaiveBayes()
    : spamMessageCount(0),
      hamMessageCount(0),
      totalSpamWords(0),
      totalHamWords(0),
      vocabularySize(0)
{
}

// ============================================================================
// Training
// ============================================================================

void NaiveBayes::train(const std::vector<Message>& dataset)
{
    // Reset model if train() is called again
    spamWordCounts.clear();
    hamWordCounts.clear();

    spamMessageCount = 0;
    hamMessageCount = 0;

    totalSpamWords = 0;
    totalHamWords = 0;

    vocabularySize = 0;

    for (const Message& msg : dataset)
    {
        if (msg.label == "spam")
        {
            ++spamMessageCount;
        }
        else if (msg.label == "ham")
        {
            ++hamMessageCount;
        }
        else
        {
            continue;
        }

        std::vector<std::string> tokens = tokenizer.tokenize(msg.text);

        for (const std::string& word : tokens)
        {
            if (msg.label == "spam")
            {
                spamWordCounts[word]++;
                ++totalSpamWords;
            }
            else
            {
                hamWordCounts[word]++;
                ++totalHamWords;
            }
        }
    }

    std::unordered_set<std::string> vocabulary;

    for (const auto& entry : spamWordCounts)
    {
        vocabulary.insert(entry.first);
    }

    for (const auto& entry : hamWordCounts)
    {
        vocabulary.insert(entry.first);
    }

    vocabularySize = static_cast<int>(vocabulary.size());
}

// ============================================================================
// Prediction
// ============================================================================

std::string NaiveBayes::predict(const std::string& text)
{
    std::vector<std::string> tokens = tokenizer.tokenize(text);

    int totalMessages = spamMessageCount + hamMessageCount;

    double logScoreSpam =
        std::log(static_cast<double>(spamMessageCount) / totalMessages);

    double logScoreHam =
        std::log(static_cast<double>(hamMessageCount) / totalMessages);

    for (const std::string& word : tokens)
    {
        int spamCount = 0;

        auto spamIt = spamWordCounts.find(word);

        if (spamIt != spamWordCounts.end())
        {
            spamCount = spamIt->second;
        }

        double spamProbability =
            static_cast<double>(spamCount + 1) /
            (totalSpamWords + vocabularySize);

        logScoreSpam += std::log(spamProbability);

        int hamCount = 0;

        auto hamIt = hamWordCounts.find(word);

        if (hamIt != hamWordCounts.end())
        {
            hamCount = hamIt->second;
        }

        double hamProbability =
            static_cast<double>(hamCount + 1) /
            (totalHamWords + vocabularySize);

        logScoreHam += std::log(hamProbability);
    }

    return (logScoreSpam > logScoreHam) ? "spam" : "ham";
}

// ============================================================================
// Statistics Getters
// ============================================================================

int NaiveBayes::getSpamMessageCount() const
{
    return spamMessageCount;
}

int NaiveBayes::getHamMessageCount() const
{
    return hamMessageCount;
}

int NaiveBayes::getVocabularySize() const
{
    return vocabularySize;
}