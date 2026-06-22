#ifndef AUTOCOMPLETE_TUI_H
#define AUTOCOMPLETE_TUI_H

#include "trie.h"
#include <string>
#include <vector>

class AutocompleteTUI {
private:
    AutocompleteEngine engine;

public:
    AutocompleteTUI();
    void add_word(const std::string& word, int freq);
    std::string run();
};

#endif