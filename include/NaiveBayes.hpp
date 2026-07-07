#ifndef NAIVEBAYES_HPP
#define NAIVEBAYES_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "Tokenizer.hpp"
#include "DataLoader.hpp"

class NaiveBayes
{
private:

    Tokenizer tokenizer;

    std::unordered_map<std::string, int> spamWordCounts;
    std::unordered_map<std::string, int> hamWordCounts;

    int spamMessageCount;
    int hamMessageCount;

    int totalSpamWords;
    int totalHamWords;

    int vocabularySize;

public:

    NaiveBayes();

    void train(const std::vector<Message>& dataset);

    std::string predict(const std::string& text);

    // Statistics
    int getSpamMessageCount() const;
    int getHamMessageCount() const;
    int getVocabularySize() const;
};

#endif