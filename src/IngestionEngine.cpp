#include <bits/stdc++.h>
#include <filesystem>
#include "IngestionEngine.h"

using namespace std;
namespace fs = std::filesystem;

IngestionEngine::IngestionEngine(string path) {
    directory_path = path;
}

string IngestionEngine::cleanToken(const string& raw_word) {
    string clean = "";
    for (char c : raw_word) {
        if (isalnum(c)) clean += tolower(c);
    }
    return clean;
}

vector<string> IngestionEngine::generateTrigrams(const string& word) {
    vector<string> trigrams;
    string padded = "$" + word + "$"; 
    if (padded.length() < 3) return trigrams; 
    for (size_t i = 0; i <= padded.length() - 3; i++) {
        trigrams.push_back(padded.substr(i, 3));
    }
    return trigrams;
}

void IngestionEngine::buildIndexes() {
    cout << "SWAP Ingestion Engine Starting...\n";
    int current_doc_id = 1;

    if (!fs::exists(directory_path) || !fs::is_directory(directory_path)) {
        cerr << "Fatal Error: Data directory '" << directory_path << "' does not exist!\n";
        return;
    }

    for (const auto& entry : fs::directory_iterator(directory_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            ifstream file(entry.path());
            if (!file.is_open()) continue;

            string word;
            int token_count = 0;

            while (file >> word) {
                string cleaned = cleanToken(word);
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
            
            file.close();
            document_registry.push_back({current_doc_id, entry.path().string(), token_count});
            current_doc_id++;
        }
    }

    for (const auto& pair : inverted_index) {
        // cout << pair.first << "\n";
        // cout << pair.second.size() << "\n";
        // cout << pair.second[0].doc_id << "\n";
        string vocab_word = pair.first;
        vector<string> chunks = generateTrigrams(vocab_word);
        for (const string& chunk : chunks) {
            trigram_index[chunk].push_back(vocab_word);
        }
    }

    cout << "Engine Built Successfully.\n";
    cout << " -> Documents: " << document_registry.size() << "\n";
    cout << " -> Vocabulary: " << inverted_index.size() << " words\n";
}

// Getters
const vector<Document>& IngestionEngine::getRegistry() const { return document_registry; }
const unordered_map<string, vector<Posting>>& IngestionEngine::getInvertedIndex() const { return inverted_index; }
const unordered_map<string, vector<string>>& IngestionEngine::getTrigramIndex() const { return trigram_index; }