# Document Retrieval Engine

A dependency-free document retrieval engine implemented in C++11. This project builds a complete local search pipeline from scratch, utilizing core data structures to implement an inverted index, real-time fuzzy autocomplete, and classical Information Retrieval (IR) ranking algorithms. Designed to parse thousands of local text files, it demonstrates fundamental search architecture without relying on external libraries.

---

## Architecture Overview

The system architecture consists of three primary modules:

### 1. Ingestion Engine
On initialization, the ingestion module scans the `data/` directory to process and index the document corpus.

* **Tokenization:** Performs custom string processing beyond whitespace separation. It handles punctuation removal, case normalization, and hyphenated word splitting (e.g., `state-of-the-art` is parsed into three distinct tokens).
* **Inverted Index:** Utilizes an `std::unordered_map` to map validated tokens to document IDs, storing exact term frequencies and absolute positional data to support downstream proximity queries.

### 2. Autocomplete TUI
A Terminal User Interface (TUI) implemented via Windows console APIs (`<conio.h>`, `<windows.h>`) for real-time, flicker-free state updates.

* **Fuzzy Matching:** Utilizes a Prefix Trie combined with a Levenshtein Distance algorithm to resolve typographical errors (e.g., seamlessly matching `algrithm` to `algorithm`).
* **LRU Cache:** Mitigates the computational cost of autocomplete queries. A Least Recently Used (LRU) cache guarantees constant-time resolution for repeated keystrokes and backspaces.
* **Query Validation:** Restricts search execution to terms present within the ingested vocabulary, preventing invalid query processing.

### 3. Ranking Engine
Executes upon query submission to mathematically evaluate document relevance.

* **BM25 TF-IDF Scoring:** Calculates Inverse Document Frequency (IDF) dynamically based on corpus size. Applies the Okapi BM25 ranking function to normalize document length and penalize term frequency saturation.
* **Proximity Boosting:** Leverages positional data from the inverted index to apply a score multiplier when multiple query terms appear adjacently within a document.
* **Min-Heap Selection:** Uses a priority queue (min-heap) to efficiently extract the top 5 most relevant results, bypassing the overhead of sorting the entire document dataset.

---

## Repository Structure

```text
.
├── code/
│   ├── main.cpp                 # Central pipeline controller
│   ├── autocomplete_tui.h/cpp   # Screen rendering and key trapping
│   ├── trie.h/cpp               # Prefix Trie and Levenshtein logic
│   ├── IngestionEngine.h/cpp    # Tokenizer and index builder
│   ├── tf_idf_search.h/cpp      # BM25 math and proximity scoring
│   └── structs.h                # Shared memory structures
├── data/                        # Drop .txt corpus files here
└── README.md
