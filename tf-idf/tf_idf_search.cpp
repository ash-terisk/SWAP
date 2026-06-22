#include<string>
#include<vector>
#include<unordered_map>
#include<cmath>
#include<queue>
#include<algorithm>
#include<sstream>
#include<iostream>
#include<unordered_map>
#include<cctype>
#include<iomanip>

#ifndef STRUCTS_H
#define STRUCTS_H
#include<string>
#include<vector>
struct Docdata{
    int doc_id;
    int term_frequency;
    std::vector<int> posn;
};
struct Corpusdata{
    int tot_doc;
    double avg_doc_len;
    std::unordered_map<int,int> doc_len;
};
#endif

//Simulation of Part 1 result
std::unordered_map<std::string, std::vector<Docdata>> test_inv_idx;
Corpusdata test_data;
void set_mock_data(){
    test_data.tot_doc=3;
    test_data.avg_doc_len=6.0;
    test_data.doc_len[1]=5; test_data.doc_len[2]=7; test_data.doc_len[3]=6;
    test_inv_idx["search"]={
        {1,1,{1}}, {2,2,{0,4}}
    };
    test_inv_idx["engine"]={
        {1,1,{2}}, {2,1,{1}}
    };
};

//Lowercasing, removing punctuations etc
std::vector<std::string> tokenize(const std::string& orig_query){
    std::vector<std::string> tokens;
    std::string cleanstr;
    cleanstr.reserve(orig_query.size());
    cleanstr="";
    for (char c : orig_query){
        if (std::isalnum(c)) cleanstr+=std::tolower(c);
        else if (std::isspace(c)) cleanstr+=' ';
        else if (c=='-'||c=='_')cleanstr+=' ';
    }
    std::stringstream ss(cleanstr);
    std::string token;
    while(ss>>token){
        tokens.push_back(token);
    }
    return tokens;
}

//IDF
double calc_idf(int N, int nqi){
    double num=N-nqi+0.5;
    double den=nqi+0.5;
    return std::log((num/den)+1);
}

//Okapi BM25 Ranking Engine
double bm_25_score(double idf, int tf, int doclen, double avg_doc_len){
    double k1=1.2; double b=0.75;
    double num=tf*(k1+1.0);
    double den=tf+k1*(1.0-b+b*(static_cast<double>(doclen)/avg_doc_len));
    return idf*(num/den);
}

//Issue: Bag of Words
double proximity_check(const Docdata& term_a, const Docdata& term_b){
    double boost=0;
    size_t i=0, j=0;
    while (i<term_a.posn.size() && j<term_b.posn.size()){
        int pos_a=term_a.posn[i]; int pos_b=term_b.posn[j];
        if (pos_b==pos_a+1 || pos_b==pos_a-1){
            boost+=0.5;
            i++; j++;
        }
        else if (pos_a<pos_b) i++;
        else j++;
    }
    return boost;
}

//Top-5 consideration
struct SearchRes{
    int doc_id; double score;
    bool operator>(const SearchRes& other) const{
        return score>other.score;
    }
};
std::vector<SearchRes> get5(const std::unordered_map<int,double>& final_score){
    std::priority_queue<SearchRes, std::vector<SearchRes>, std::greater<SearchRes>> min_heap;

    for (const auto&[doc_id, score]:final_score){
        min_heap.push({doc_id, score});
        if (min_heap.size()>5){
            min_heap.pop();
        }
    }
    std::vector<SearchRes> res;
    while(!min_heap.empty()){
        res.push_back(min_heap.top());
        min_heap.pop();
    }
    std::reverse(res.begin(), res.end());
    return res;
    }
