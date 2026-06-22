#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <vector>

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

#endif