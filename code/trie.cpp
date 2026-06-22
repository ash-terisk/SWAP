#include "trie.h"

AutocompleteEngine::AutocompleteEngine(int capacity) : cache_capacity(capacity) {
    root = std::make_unique<TrieNode>();
}

void TrieNode::update_top_words(const std::string& word, int freq){
    for(auto &element : top_words){
        if(element.second == word){
            return;
        }
    }
    top_words.push_back({freq,word});

    sort(top_words.begin(),top_words.end(), [](const word_freq& a , const word_freq& b){
        return a.first>b.first;
    });

    if(top_words.size()>3){
        top_words.pop_back();
    }
}

void AutocompleteEngine::insert(const std::string& word, int frequency){
    TrieNode* current = root.get();

    for(char c: word){
        if(current->children.find(c) == current->children.end()){
            current->children[c] = std::make_unique<TrieNode>();
        }
        current = current->children[c].get();
        current->update_top_words(word,frequency);
    }
    current->is_end = true;
    current->frequency = frequency;
}

void AutocompleteEngine::search_recursive(TrieNode* node, char current_char, 
                                        const std::vector<int>& prev_row, const std::string& target_query,
                                        min_heap& top_k, std::unordered_set<std::string>& visited){
    int cols = target_query.length()+1;
    std::vector<int> curr_row(cols);
    curr_row[0] = prev_row[0]+1;
    
    for(int i=1;i<cols;i++){
        int insert_cost = curr_row[i-1] + 1;
        int delete_cost = prev_row[i]+1;
        int replace_cost = prev_row[i-1] + (target_query[i-1] == current_char ? 0:1);
        curr_row[i] = std::min({insert_cost,delete_cost,replace_cost});
    }

    if(curr_row.back()<=1){
        for(const auto& word_freqq : node->top_words){
            if(visited.find(word_freqq.second) == visited.end()){
                visited.insert(word_freqq.second);
                top_k.push(word_freqq);
                if(top_k.size()>3){
                    top_k.pop();
                }
            }
        }
    }

    if(*std::min_element(curr_row.begin(),curr_row.end()) >1){
        return;
    }

    for(auto const& pair : node->children){
        search_recursive(pair.second.get(), pair.first, curr_row, target_query, top_k, visited);
    }
}

std::vector<std::string> AutocompleteEngine::get_fuzzy_suggestions(const std::string& query){
    min_heap top_k;
    std::unordered_set<std::string> visited;
    std::vector<int> inital_row(query.length()+1);
    for(int i=0;i<query.length()+1;i++){
        inital_row[i] = i;
    }
    for(auto const& pair : root->children){
        search_recursive(pair.second.get(), pair.first, inital_row, query, top_k, visited);
    }
    std::vector<std::string> ans;
    while(!top_k.empty()){
        ans.push_back(top_k.top().second);
        top_k.pop();
    }
    std::reverse(ans.begin(),ans.end());
    return ans;
}

std::vector<std::string> AutocompleteEngine::get_suggestions(const std::string&query){
    if(lru_map.find(query) != lru_map.end()){
        lru_list.splice(lru_list.begin(),lru_list,lru_map[query]);
        return lru_map[query]->second;
    }

    std::vector<std::string> results = get_fuzzy_suggestions(query);

    lru_list.push_front({query,results});
    lru_map[query] = lru_list.begin();

    if(lru_list.size()>cache_capacity){
        lru_map.erase(lru_list.back().first);
        lru_list.pop_back();
    }
    return results;
}