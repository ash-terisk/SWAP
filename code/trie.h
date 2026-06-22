#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <list>

using word_freq = std::pair<int, std::string>;
using min_heap = std::priority_queue<word_freq, std::vector<word_freq>, std::greater<word_freq>>;
using cache_list = std::list<std::pair<std::string, std::vector<std::string>>>;

struct TrieNode {
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    bool is_end = false;
    int frequency = 0;
    std::vector<word_freq> top_words;

    void update_top_words(const std::string& word, int freq);
};

class AutocompleteEngine {
private:
    std::unique_ptr<TrieNode> root;
    int cache_capacity;
    cache_list lru_list;
    std::unordered_map<std::string, cache_list::iterator> lru_map;
    
    void search_recursive(TrieNode* node, char current_char, 
                        const std::vector<int>& prev_row, const std::string& target_query,
                        min_heap& top_k, std::unordered_set<std::string>& visited);

public:
    AutocompleteEngine(int capacity = 50);
    void insert(const std::string& word, int frequency = 0);
    std::vector<std::string> get_suggestions(const std::string& query);
    std::vector<std::string> get_fuzzy_suggestions(const std::string& query);
};