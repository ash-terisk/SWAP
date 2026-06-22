#include"tf_idf_search.h"
#include<cmath>
#include<queue>
#include<algorithm>
#include<sstream>
#include<cctype>

TFIDFSearch::TFIDFSearch(const std::unordered_map<std::string, std::vector<Posting>>& index, 
                         const std::vector<Document>& registry) : inverted_index(index) {
    // Initialize Corpus Stats based on Person A's Document Registry
    corpus_stats.tot_doc=registry.size();
    
    double total_length=0;
    for (const auto& doc:registry) {
        corpus_stats.doc_len[doc.doc_id] = doc.token_length;
        total_length+=doc.token_length;
    }
    
    corpus_stats.avg_doc_len=corpus_stats.tot_doc>0?(total_length/corpus_stats.tot_doc) : 0.0;
}

std::vector<std::string>TFIDFSearch::tokenize(const std::string& orig_query) {
    std::vector<std::string> tokens;
    std::string cleanstr="";
    for (char c:orig_query) {
        if (std::isalnum(c))cleanstr+=std::tolower(c);
        else if (std::isspace(c)||c=='-'||c=='_') cleanstr+=' ';
    }
    std::stringstream ss(cleanstr);
    std::string token;
    while (ss>>token) {
        tokens.push_back(token);
    }
    return tokens;
}

double TFIDFSearch::calc_idf(int N, int nqi) {
    double num=N-nqi+0.5;
    double den=nqi+0.5;
    return std::log((num /den)+1);
}

double TFIDFSearch::bm_25_score(double idf, int tf, int doclen, double avg_doc_len) {
    double k1=1.2; double b=0.75;
    double num=tf*(k1+1.0);
    double den=tf+k1*(1.0- b+b*(static_cast<double>(doclen)/avg_doc_len));
    return idf*(num/den);
}

double TFIDFSearch::proximity_check(const Posting& term_a, const Posting& term_b) {
    double boost=0;
    size_t i=0, j=0;
    while (i<term_a.positions.size() && j<term_b.positions.size()) {
        int pos_a=term_a.positions[i]; int pos_b=term_b.positions[j];
        if (pos_b==pos_a+1 || pos_b==pos_a-1) {
            boost += 0.5; i++; j++;
        }
        else if (pos_a < pos_b) i++;
        else j++;
    }
    return boost;
}

std::vector<SearchRes> TFIDFSearch::get5(const std::unordered_map<int, double>& final_score) {
    std::priority_queue<SearchRes, std::vector<SearchRes>, std::greater<SearchRes>> min_heap;
    for (const auto& [doc_id, score]:final_score) {
        min_heap.push({doc_id, score});
        if (min_heap.size() > 5) min_heap.pop();
    }
    std::vector<SearchRes> res;
    while (!min_heap.empty()) {
        res.push_back(min_heap.top());
        min_heap.pop();
    }
    std::reverse(res.begin(), res.end());
    return res;
}

std::vector<SearchRes> TFIDFSearch::execute_search(const std::string& query) {
    std::vector<std::string> tokens = tokenize(query);
    std::unordered_map<int, double> document_scores;
    
    // Core TF-IDF search execution
    for (const std::string& token : tokens) {
        if (inverted_index.find(token)==inverted_index.end()) continue;
        
        const auto& postings=inverted_index.at(token);
        int nqi=postings.size();
        double idf=calc_idf(corpus_stats.tot_doc, nqi);
        
        for (const auto& posting:postings) {
            int doc_length=corpus_stats.doc_len[posting.doc_id];
            double score=bm_25_score(idf, posting.term_frequency, doc_length, corpus_stats.avg_doc_len);
            document_scores[posting.doc_id]+=score;
        }
    }
    
    return get5(document_scores);
}