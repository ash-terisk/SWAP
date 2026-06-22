#include "autocomplete_tui.h"
#include "IngestionEngine.h"
#include "tf_idf_search.h"
#include <iostream>
#include <conio.h>    
#include <vector>
#include <string>

int main() {
    std::cout << "Initializing System... Loading Documents...\n";
    
    
    std::string data_path = "../data"; 
    IngestionEngine data_engine(data_path);
    data_engine.buildIndexes();

    const auto& registry = data_engine.getRegistry();
    const auto& inv_index = data_engine.getInvertedIndex();

    
    AutocompleteTUI tui;
    for (const auto& pair : inv_index) {
        const std::string& word = pair.first;
        
        int total_freq = 0;
        for (const auto& posting : pair.second) {
            total_freq += posting.term_frequency;
        }
        
        tui.add_word(word, total_freq);
    }

   
    TFIDFSearch tfidf_engine(inv_index, registry);

   
    while (true) {
        std::string final_query = tui.run();

        if (final_query.empty()) {
            std::cout << "Exiting application. Goodbye!\n";
            break; 
        }

        system("cls");
        std::cout << "Executing TF-IDF Search for: [" << final_query << "]...\n\n";
        
        std::vector<SearchRes> top_docs = tfidf_engine.execute_search(final_query);

        std::cout << "=== TOP RELEVANT DOCUMENTS ===\n";
        if (top_docs.empty()) {
            std::cout << "  No matching documents found.\n";
        } else {
            for (size_t i = 0; i < top_docs.size(); i++) {
                std::string filepath = "Unknown File";
                for (const auto& doc : registry) {
                    if (doc.doc_id == top_docs[i].doc_id) {
                        filepath = doc.filepath;
                        break;
                    }
                }
                
                std::cout << "  " << (i + 1) << ". " << filepath 
                          << " (Score: " << top_docs[i].score << ")\n";
            }
        }

        std::cout << "\n-------------------------------------\n";
        std::cout << "Press 'ENTER' to search for another word.\n";
        std::cout << "Press 'ESC' to exit the application.\n";

        bool keep_running = true;
        while (true) {
            int ch = _getch();
            if (ch == 27) { 
                keep_running = false;
                break; 
            }
            if (ch == 13) { 
                break; 
            }
        }

        if (!keep_running) break; 
    }

    return 0;
}