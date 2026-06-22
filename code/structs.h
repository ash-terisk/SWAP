#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <vector>
#include <unordered_map>

struct Document {
    int doc_id;
    std::string filepath;
    int token_length; 
};

struct Posting {
    int doc_id;
    int term_frequency;
    std::vector<int> positions; 
};

struct Corpusdata {
    int tot_doc;
    double avg_doc_len;
    std::unordered_map<int, int> doc_len;
};

struct SearchRes {
    int doc_id; 
    double score;
    bool operator>(const SearchRes& other) const { return score > other.score; }
};

#endif