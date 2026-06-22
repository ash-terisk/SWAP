#include <iostream>
#include "IngestionEngine.h"

using namespace std;

int main() {
    // 1. Turn on the Engine
    IngestionEngine engine("../data");
    engine.buildIndexes();

    cout << "\n--- TESTING DATA ACCESS ---\n";

    // 2. Safely grab the Inverted Index!
    // The 'const auto&' is CRITICAL here. It means we are just LOOKING at the 
    // original database. If you forget the '&', C++ will accidentally copy 
    // the entire massive dictionary into new memory!
    const auto& my_index = engine.getInvertedIndex();

    // 3. Let's test if we can find a specific word
    string test_word = "search";

    // Check if the word actually exists in the map
    if (my_index.count(test_word) > 0) {
        cout << "SUCCESS: The word '" << test_word << "' was found!\n";
        cout << "It appears in " << my_index.at(test_word).size() << " documents.\n";

        // Loop through the Postings for this specific word
        for (const auto& posting : my_index.at(test_word)) {
            cout << " -> Found in Doc ID: " << posting.doc_id 
                 << " | Times appeared: " << posting.term_frequency << "\n";
        }
    } else {
        cout << "The word '" << test_word << "' does not exist in the database.\n";
    }

    return 0;
}