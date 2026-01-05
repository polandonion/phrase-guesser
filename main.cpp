#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#ifdef __linux__
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#elif _WIN32
#include <conio.h>
#endif

using namespace std;

struct Pair {
    string word;
    string category;
};

bool is_letter(char c) {
    return (c >= 'a' and c <= 'z');
}

bool is_vowel(char c) {
    return (c == 'a' or c == 'e' or c == 'i' or c == 'o' or c == 'u');
}

#ifdef __linux__
char getch(void) {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

void check(string& show_word, string real_word, char guess_letter, vector<char>& unused) {
    bool found = false;
    for (int i = 0; i < real_word.size(); i++) {
        if (real_word[i] == guess_letter) {
            show_word[i] = guess_letter;
            found = true;
        }
    }
    unused.erase(find(unused.begin(), unused.end(), guess_letter));
}

void generate_word(string& real_word, string& word_category, string& show_word, vector<char> unused) {
    fstream database;
    database.open("database.txt", ios::in);
    if (database.is_open() == false) {
        cout << "'database.txt' not found!" << endl;
        exit(0);
    }

    vector<Pair> words = {};
    string input;
    while (getline(database, input)) {
        if (input.size() == 0 or input.substr(0, 2) == "//")
            continue;
        int idx = input.find(',');
        words.push_back({input.substr(0, idx), input.substr(idx + 1)});
    }
    int random_index = rand() % words.size();
    real_word = words[random_index].word;
    word_category = words[random_index].category;
    for (char c : real_word) {
        if (is_letter(c) == true and is_vowel(c) == false)
            show_word += '_';
        else if (is_letter(c) == true)
            show_word += c;
        else
            show_word += ' ';
    }
    if (real_word.front() == 'y' or real_word.back() == 'y') {
        unused.erase(find(unused.begin(), unused.end(), 'y'));
        show_word.front() = (real_word.front() == 'y' ? 'y' : show_word.front());
        show_word.back() = (real_word.back() == 'y' ? 'y' : show_word.back());
    }
    words.clear();
    database.close();
}

int main() {
    srand(time(nullptr));
    while (true) {
        string real_word, word_category, show_word;
        int attempts = 0;
        vector<char> unused = {'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z' };
        generate_word(real_word, word_category, show_word, unused);

        bool advanced_options = false;
        while (true) {
            #ifdef __linux__
            system("clear");
            #elif _WIN32
            system("cls");
            #endif

            cout << "word category: " << word_category << endl;
            cout << "total attempts: " << attempts << endl;
            cout << "progress: " << show_word << endl;

            if (show_word == real_word)
                break;

            cout << "unused letters: ";
            for (char c : unused)
                cout << c << ' ';
            cout << endl;

            cout << "-------------------------------" << endl;
            if (advanced_options == false) {
                cout << "space - show advanced menu" << endl;
                cout << "guess the letter... ";
            }
            else {
                cout << "space - hide advanced menu" << endl;
                cout << "l - guess the letter" << endl;
                cout << "w - guess the word" << endl;
                cout << "g - give up" << endl;
                cout << "q - quit" << endl;
                cout << "enter your option... ";
            }

            char opt = getch();
            if (advanced_options == false)
                while ((opt == ' ' or (is_letter(opt) == true and find(unused.begin(), unused.end(), opt) != unused.end())) == false)
                    opt = getch();
            else {
                if (opt == 'q') {
                    cout << endl;
                    return 0;
                }
                while ((opt == ' ' or opt == 'l' or opt == 'w' or opt == 'g' or opt == 'q') == false)
                    opt = getch();
                if (opt == 'q') {
                    cout << endl;
                    return 0;
                }
            }


            if (opt == ' ')
                advanced_options = !advanced_options;
            else {
                if (advanced_options == false) {
                    char guess_letter = opt;
                    check(show_word, real_word, guess_letter, unused);
                }
                else {
                    cout << endl << "-------------------------------" << endl;
                    if (opt == 'l') {
                        cout << "guess the letter... ";
                        char guess_letter = getch();
                        while ((is_letter(guess_letter) == true and find(unused.begin(), unused.end(), guess_letter) != unused.end()) == false)
                            guess_letter = getch();
                        check(show_word, real_word, guess_letter, unused);
                    }
                    else if (opt == 'w') {
                        cout << "guess the word: ";
                        string guess_word;
                        getline(cin, guess_word);
                        if (guess_word == real_word)
                            show_word = real_word;
                    }
                    else if (opt == 'g') {
                        show_word = real_word;
                        attempts = -1;
                    }
                    else {
                        cout << endl;
                        return 0;
                    }
                }
                attempts++;
            }
        }
        cout << "-------------------------------" << endl;
        cout << "p - play again" << endl;
        cout << "q - quit" << endl;
        cout << "enter your option... ";
        char opt = getch();
        while ((opt == 'p' or opt == 'q') == false)
            opt = getch();
        if (opt == 'q') {
            cout << endl;
            return 0;
        }
    }
    return 0;
}
