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
        // R-BUG-03 FIX: compute both booleans first so msg.label is compared
        // at most once per class (isSpam) and once for the ham check (isHam).
        // The previous version compared msg.label once for the counter, then
        // again to set isSpam — two comparisons per message plus one per token.
        const bool isSpam = (msg.label == "spam");
        const bool isHam  = !isSpam && (msg.label == "ham");
        if (!isSpam && !isHam) continue;

        if (isSpam) ++spamMessageCount;
        else        ++hamMessageCount;

        std::vector<std::string> tokens = tokenizer.tokenize(msg.text);

        for (const std::string& word : tokens)
        {
            if (isSpam)
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
    // BUG-02: guard against calling predict() before train() has been called.
    // Division by zero would occur computing class priors if counts are 0.
    if (spamMessageCount == 0 || hamMessageCount == 0)
        return "untrained";

    std::vector<std::string> tokens = tokenizer.tokenize(text);

    // LOGIC-02: guard against empty token list (whitespace-only input).
    // Without tokens the loop never runs and the model always returns
    // whichever class has the higher prior (ham for UCI dataset), giving
    // a silently incorrect result.
    if (tokens.empty())
        return "unknown";

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

        // operator[] zero-initialises missing keys so the +1 Laplace term
        // is applied even for words unseen during training.
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