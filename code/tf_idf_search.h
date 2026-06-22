#ifndef TF_IDF_SEARCH_H
#define TF_IDF_SEARCH_H

#include "structs.h"
#include <string>
#include <vector>
#include <unordered_map>

class TFIDFSearch {
private:
    Corpusdata corpus_stats;
    const std::unordered_map<std::string, std::vector<Posting>>& inverted_index;

    std::vector<std::string> tokenize(const std::string& orig_query);
    double calc_idf(int N, int nqi);
    double bm_25_score(double idf, int tf, int doclen, double avg_doc_len);
    double proximity_check(const Posting& term_a, const Posting& term_b);
    std::vector<SearchRes> get5(const std::unordered_map<int, double>& final_score);

public:
    TFIDFSearch(const std::unordered_map<std::string, std::vector<Posting>>& index, 
                const std::vector<Document>& registry);
    
    std::vector<SearchRes> execute_search(const std::string& query);
};

#endif