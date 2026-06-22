#ifndef INGESTION_ENGINE_H
#define INGESTION_ENGINE_H

#include <string>
#include <vector>
#include <unordered_map>
#include "structs.h"

class IngestionEngine {
private:
    std::string directory_path;
    std::vector<Document> document_registry;
    std::unordered_map<std::string, std::vector<Posting>> inverted_index;
    std::unordered_map<std::string, std::vector<std::string>> trigram_index;

    std::string cleanToken(const std::string& raw_word);
    std::vector<std::string> generateTrigrams(const std::string& word);

public:
    IngestionEngine(std::string path);
    void buildIndexes();

    
    const std::vector<Document>& getRegistry() const;
    const std::unordered_map<std::string, std::vector<Posting>>& getInvertedIndex() const;
    const std::unordered_map<std::string, std::vector<std::string>>& getTrigramIndex() const;
};

#endif