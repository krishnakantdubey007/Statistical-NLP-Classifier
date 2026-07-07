# 📌 Statistical NLP Text Classifier Engine

A modular C++17 implementation of a **Statistical Natural Language Processing (NLP)** based **SMS Spam Detection System** using the **Multinomial Naive Bayes Algorithm**.

---

## 📖 Project Overview

This project classifies SMS messages into **Spam** or **Ham (Normal)** using the **Multinomial Naive Bayes** algorithm.

The application is completely developed in **Modern C++17** without using any external Machine Learning or NLP libraries.

The classifier performs:

- Text Preprocessing
- Tokenization
- Word Frequency Analysis
- Bayesian Probability Calculation
- Laplace Smoothing
- Log Probability Transformation
- Interactive Console Prediction

The project uses the **UCI SMS Spam Collection Dataset** containing **5,574 real-world SMS messages**.

---

# ✨ Features

- ✅ Object-Oriented Design
- ✅ Modular Architecture
- ✅ Tokenizer Module
- ✅ Dataset Loader
- ✅ Multinomial Naive Bayes Classifier
- ✅ Laplace Smoothing
- ✅ Log Probability Calculation
- ✅ Interactive Console Interface
- ✅ Model Statistics
- ✅ GitHub Ready Project Structure

---

# 🏗️ Project Architecture

```

                 SMS Dataset
                       │
                       ▼
                +---------------+
                |  DataLoader   |
                +---------------+
                       │
                       ▼
                +---------------+
                |  Tokenizer    |
                +---------------+
                       │
                       ▼
                +---------------+
                | Naive Bayes   |
                +---------------+
                       │
                       ▼
                Spam / Ham Prediction

```

---

# 📁 Folder Structure

```text
Statistical-NLP-Classifier/

│

├── data/
│   └── SMSSpamCollection

│

├── include/
│   ├── Tokenizer.hpp
│   ├── DataLoader.hpp
│   └── NaiveBayes.hpp

│

├── src/
│   ├── Tokenizer.cpp
│   ├── DataLoader.cpp
│   ├── NaiveBayes.cpp
│   └── main.cpp

│

├── docs/
│   └── Project-Journal.md

│

├── LICENSE

├── README.md

└── CMakeLists.txt

```

---

# 💻 Technologies Used

- C++17
- STL
- Object-Oriented Programming
- Hash Tables (`std::unordered_map`)
- File Handling
- String Processing
- Git
- GitHub

---

# 📂 Dataset

**Dataset**

UCI SMS Spam Collection

**Total Messages**

5574

**Spam**

747

**Ham**

4827

---

# 🧠 Algorithm

This project implements the **Multinomial Naive Bayes Algorithm**, a probabilistic machine learning algorithm widely used for text classification.

For each incoming SMS, the classifier computes:

\[
P(Class \mid Message) \propto P(Class)\times\prod P(Word \mid Class)
\]

The class with the higher probability is selected as the final prediction.

To improve numerical stability and handle unseen words, the implementation includes:

- **Laplace Smoothing (Add-One Smoothing)**
- **Log Probability Transformation**

---

# ⚙️ How It Works

The complete workflow of the application is:

```text
Load Dataset
      │
      ▼
Train Naive Bayes Model
      │
      ▼
User Enters SMS
      │
      ▼
Tokenizer
      │
      ▼
Word Frequency Lookup
      │
      ▼
Probability Calculation
      │
      ▼
Spam / Ham Prediction
```

---

# 🚀 Build Instructions

Compile using MinGW GCC:

```bash
g++ src/main.cpp src/Tokenizer.cpp src/DataLoader.cpp src/NaiveBayes.cpp -Iinclude -o classifier
```

Run:

```bash
./classifier
```

On Windows PowerShell:

```powershell
.\classifier.exe
```

---

# 🖥️ Example Output

```text
=========================================================
        Statistical NLP Text Classifier Engine
         Multinomial Naive Bayes (C++17)
=========================================================

Loading dataset...
[OK] Dataset Loaded Successfully

Training model...
[OK] Model Trained Successfully

1. Predict a Message
2. Show Model Statistics
3. About Project
4. Exit
```
# 📸 Project Screenshots

## Startup Menu

!## Startup Menu

<img src="docs/screenshots/menu.png" width="800">
---

## Spam Prediction

![Prediction](docs/screenshots/prediction.png)

---

## Model Statistics

![Statistics](docs/screenshots/statistics.png)

---

## About Project

![About](docs/screenshots/about.png)
# 📈 Current Features

- Text preprocessing
- Lowercase normalization
- Punctuation removal
- Tokenization
- Dataset loading from file
- Word frequency counting
- Multinomial Naive Bayes classifier
- Laplace smoothing
- Log probability scoring
- Interactive console menu
- Model statistics display

---

# 🚀 Future Improvements

Some possible future enhancements include:

- Train/Test Split Evaluation
- Accuracy Calculation
- Precision, Recall and F1 Score
- Confusion Matrix
- CSV Dataset Support
- Email Spam Detection
- GUI Version
- Web API Version

---

# 👨‍💻 Developer

**Krishnakant Dubey**

- UGC NET Qualified (Computer Science)
- M.Sc. Computer Science
- GitHub: https://github.com/krishnakantdubey007

---

# 📄 License

This project is released under the **MIT License**.

---

## ⭐ If you found this project useful, consider giving it a Star on GitHub.