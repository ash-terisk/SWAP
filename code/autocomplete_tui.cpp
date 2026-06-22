#include "autocomplete_tui.h"
#include <iostream>
#include <conio.h>    
#include <windows.h>  
#include <cctype>     

// Hidden helper function, only accessible in this file
void reset_cursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {0, 0};
    SetConsoleCursorPosition(hOut, pos);
}

AutocompleteTUI::AutocompleteTUI() : engine(50) {}

void AutocompleteTUI::add_word(const std::string& word, int freq) {
    engine.insert(word, freq);
}

std::string AutocompleteTUI::run() {
    std::string current_input = "";
    std::string error_message = ""; 
    bool needs_redraw = true;
    
    int selected_index = -1; 
    std::vector<std::string> current_results; 

    system("cls");

    while (true) {
        if (needs_redraw) {
            reset_cursor();
            
            std::cout << "=== Blazing Fast Fuzzy Search ===               \n";
            std::cout << "Type to search (Use Arrows & ENTER, 'ESC' to quit)\n";
            std::cout << "---------------------------------               \n";
            
            if (selected_index == -1) {
                std::cout << "> " << current_input << "_                      \n"; 
            } else {
                std::cout << "  " << current_input << "                       \n";
            }

            if (!error_message.empty()) {
                std::cout << error_message << "                                      \n";
            } else {
                std::cout << "                                                      \n";
            }

            if (!current_input.empty()) {
                current_results = engine.get_suggestions(current_input);
                
                std::cout << "Suggestions:                                  \n";
                if (current_results.empty()) {
                    std::cout << "  (no matches found)                          \n";
                    std::cout << "                                              \n";
                    std::cout << "                                              \n";
                } else {
                    for (int i = 0; i < 3; i++) {
                        if (i < current_results.size()) {
                            if (i == selected_index) {
                                std::cout << "  > [" << current_results[i] << "] <                              \n";
                            } else {
                                std::cout << "  -  " << current_results[i] << "                                 \n";
                            }
                        } else {
                            std::cout << "                                              \n";
                        }
                    }
                }
            } else {
                current_results.clear();
                std::cout << "                                              \n";
                std::cout << "                                              \n";
                std::cout << "                                              \n";
                std::cout << "                                              \n";
            }
            
            needs_redraw = false; 
        }

        int ch = _getch(); 
        
        if (!error_message.empty()) {
            error_message = "";
            needs_redraw = true;
        }
        
        if (ch == 224 || ch == 0) { 
            int arrow = _getch(); 
            if (arrow == 72) { 
                if (selected_index > -1) {
                    selected_index--;
                    needs_redraw = true;
                }
            } 
            else if (arrow == 80) { 
                if (selected_index < (int)current_results.size() - 1 && selected_index < 2) {
                    selected_index++;
                    needs_redraw = true;
                }
            }
        }
        else if (ch == 13) { 
            if (selected_index != -1 && selected_index < current_results.size()) {
                current_input = current_results[selected_index];
                selected_index = -1; 
                needs_redraw = true;
            } else {
                bool is_valid = false;
                for (const std::string& valid_word : current_results) {
                    if (current_input == valid_word) {
                        is_valid = true;
                        break;
                    }
                }

                if (is_valid) {
                    system("cls");
                    return current_input; 
                } else {
                    error_message = "[!] Invalid word. Please select a suggestion.";
                    needs_redraw = true;
                }
            }
        }
        else if (ch == 27) { 
            system("cls");
            return ""; 
        } 
        else if (ch == '\b') { 
            if (!current_input.empty()) {
                current_input.pop_back();
                selected_index = -1; 
                needs_redraw = true;
            }
        } 
        else if (std::isprint(ch)) { 
            current_input.push_back(std::tolower(ch)); 
            selected_index = -1; 
            needs_redraw = true;
        }
    }
}