#include <iostream>
#include <fstream>
#include <filesystem>
#include "IngestionEngine.h"

using namespace std;
namespace fs = std::filesystem;

IngestionEngine::IngestionEngine(string path) {
    directory_path = path;
}

vector<string> IngestionEngine::cleanToken(const string& raw_word) {
    vector<string> tokens;
    string current_token = "";
    
    for (char c : raw_word) {
        if (isalnum(c)) {
            current_token += tolower(c);
        } 
        else if (c == '-' || c == '_') {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token = "";
            }
        }
    }
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    
    return tokens;
}

void IngestionEngine::buildIndexes() {
    cout << "Ingestion Engine Starting...\n";
    int current_doc_id = 1;

    if (!fs::exists(directory_path) || !fs::is_directory(directory_path)) {
        cerr << "Fatal Error: Data directory '" << directory_path << "' does not exist!\n";
        return;
    }

    for (const auto& entry : fs::directory_iterator(directory_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            ifstream file(entry.path());
            if (!file.is_open()) continue;

            string raw_word;
            int token_count = 0;

            while (file >> raw_word) {
                vector<string> cleaned_tokens = cleanToken(raw_word);
                
                for (const string& cleaned : cleaned_tokens) {
                    if (cleaned.empty()) continue;

                    bool found = false;
                    for (auto& p : inverted_index[cleaned]) {
                        if (p.doc_id == current_doc_id) {
                            p.term_frequency++;
                            p.positions.push_back(token_count);
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        Posting new_p;
                        new_p.doc_id = current_doc_id;
                        new_p.term_frequency = 1;
                        new_p.positions.push_back(token_count);
                        inverted_index[cleaned].push_back(new_p);
                    }
                    token_count++;
                }
            }
            
            file.close();
            document_registry.push_back({current_doc_id, entry.path().string(), token_count});
            current_doc_id++;
        }
    }

    cout << "Engine Built Successfully.\n";
    cout << " -> Documents: " << document_registry.size() << "\n";
    cout << " -> Vocabulary: " << inverted_index.size() << " words\n";
}

const vector<Document>& IngestionEngine::getRegistry() const { return document_registry; }
const unordered_map<string, vector<Posting>>& IngestionEngine::getInvertedIndex() const { return inverted_index; }