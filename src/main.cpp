#include <iostream>
#include <string>
#include "IngestionEngine.h"
#include "tf_idf_search.h"

int main() {
    // Change this to the path where your .txt files are located
    std::string data_path = "C:\\Users\\sahua\\OneDrive\\Desktop\\SWAP-Engine\\data"; 
    
    // 1. Build the index
    IngestionEngine engine(data_path);
    engine.buildIndexes();

    const auto& registry = engine.getRegistry();
    const auto& inv_index = engine.getInvertedIndex();

    // 2. Initialize the TF-IDF searcher with Person A's data
    TFIDFSearch searcher(inv_index, registry);

    std::cout << "\n=== TF-IDF Testing Environment ===\n";
    std::cout << "Type 'exit' to quit.\n\n";

    // 3. Simple text loop to test the search
    while (true) {
        std::string query;
        std::cout << "Enter search query: ";
        std::getline(std::cin, query);

        if (query == "exit") break;
        if (query.empty()) continue;

        std::vector<SearchRes> results = searcher.execute_search(query);

        std::cout << "\n--- Top Results ---\n";
        if (results.empty()) {
            std::cout << "No matching documents found.\n";
        } else {
            for (size_t i = 0; i < results.size(); ++i) {
                // Find filepath from registry
                std::string filepath = "Unknown";
                for (const auto& doc : registry) {
                    if (doc.doc_id == results[i].doc_id) {
                        filepath = doc.filepath;
                        break;
                    }
                }
                
                std::cout << (i + 1) << ". Doc ID: " << results[i].doc_id 
                          << " | File: " << filepath 
                          << " | Score: " << results[i].score << "\n";
            }
        }
        std::cout << "-------------------\n\n";
    }

    return 0;
}