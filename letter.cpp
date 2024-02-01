// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68768FC9

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>
using namespace std;

class Letterman
{
private:
    string begin;
    string end;
    int b_idx = -1;
    int e_idx = -1;
    int cw_idx;
    int dict_size; 
    size_t b_len;
    bool stack_mode = false;
    bool queue_mode = false;
    bool change_mode = false; // change letter
    bool length_mode = false; // insert or delete
    bool swap_mode = false;   // swap to adjacent letters
    bool output_W = true;     // word format is basic output mode
    bool solution = false;    // there is a solution

    string dict_type; // S or C
    int line_count;
    class dict_entry
    { // nested class to store dict entry info
    private:
        string word;
        int prev = -1;
        bool discovered = false;

    public:
        dict_entry(const string &word) : word(word) {}

        const string& getWord() {
            return word;
        }
        bool isDiscovered() {
            return discovered;
        }
        int getPrev() {
            return prev;
        }

        void discover() {
            discovered = true;
        }
        void setPrev(int p) {
            prev = p;
        }
    };
    vector<dict_entry> dict_entries;

    void readSimpleDict(const string &w)
    {
        string current_word = w;

        for (int i = 0; i < line_count; i++) {
            if (!length_mode && current_word.length() != b_len) {
                cin >> current_word;
                continue;
            }
            checkBeginEnd(current_word);
            dict_entries.push_back(dict_entry(current_word));
            cin >> current_word;
        }
    }

    void readComplexDict(const string &w)
    {
        string current_word = w;
        size_t special_char_id;
        
        for (int i = 0; i < line_count; i++) {

            special_char_id = current_word.find_first_of("&[!?");

            if (special_char_id != string::npos) { //special character mods
                
                if (current_word[special_char_id] == '&') { //reversal &
                    if (!length_mode && current_word.length()-1 != b_len) {
                        cin >> current_word;
                        continue;
                    }
                        current_word.pop_back();
                        checkBeginEnd(current_word);
                        dict_entries.push_back(dict_entry(current_word));
                        for (size_t j = 0; j < current_word.length()/2; j++) {
                            swap(current_word[j], current_word[current_word.length()-j-1]);
                        }
                        checkBeginEnd(current_word);
                        dict_entries.push_back(dict_entry(current_word));
                }
                else if (current_word[special_char_id] == '[') { //insert  []
                    size_t special_char_id2  = current_word.find("]");
                    size_t word_count = special_char_id2 - special_char_id - 1;
                    if (!length_mode && (current_word.length()-word_count-1) != b_len) {
                        cin >> current_word;
                        continue;
                    }
                        string chars = current_word.substr(special_char_id+1, word_count);
                        current_word.erase(special_char_id, word_count + 2);
                        for (size_t j = 0; j < word_count; j++) {
                            current_word.insert(special_char_id, 1, chars[j]);
                            checkBeginEnd(current_word);
                            dict_entries.push_back(dict_entry(current_word));
                            current_word.erase(special_char_id, 1);
                        }
                }
                else if (current_word[special_char_id] == '!') { //swap     !
                    if (!length_mode && current_word.length()-1 != b_len) {
                        cin >> current_word;
                        continue;
                    }
                        current_word.erase(special_char_id, 1);
                        checkBeginEnd(current_word);
                        dict_entries.push_back(dict_entry(current_word));
                        swap(current_word[special_char_id-1], current_word[special_char_id-2]);
                        checkBeginEnd(current_word);
                        dict_entries.push_back(dict_entry(current_word));
                }
                else if (current_word[special_char_id] == '?') { //double   ?
                    current_word.erase(special_char_id, 1);
                    if (length_mode || current_word.length() == b_len) {
                        checkBeginEnd(current_word);
                        dict_entries.push_back(dict_entry(current_word));
                    }
                    if (length_mode || current_word.length()+1 == b_len) {
                        current_word.insert(special_char_id, 1, current_word[special_char_id-1]);
                        checkBeginEnd(current_word);
                        dict_entries.push_back(dict_entry(current_word));
                    }
                }
            }
            else {
                if (!length_mode && current_word.length() != b_len) {
                    cin >> current_word;
                    continue;
                }
                    checkBeginEnd(current_word);
                    dict_entries.push_back(dict_entry(current_word));
            }
            cin >> current_word;
        }
    }

    void checkBeginEnd(const string &current_word) 
    {
        if (b_idx == -1 && current_word == begin) {
            b_idx = static_cast<int>(dict_entries.size());
        }
        if (e_idx == -1 && current_word == end) {
            e_idx = static_cast<int>(dict_entries.size());
        }
    }

    void printHelp() {
        cout << "This program goes from a begin word and converts it to a different end word.\n";
        cout << "Pass in a begin word (-b <w>), end word (-e <e>),\n";
        cout << "whether you want to go through words like a stack or queue (-s OR -q),\n";
        cout << "and what modifications are allowed: swap (-p), insert/delete (-l), change (-c).\n";
        cout << "Swap: swap adjacent letters. Insert/Delete: insert/delete a character. Change: change one letter to any other" << endl;
    } 

public:
    // Letterman(const int &argc, char *argv[]) : {}

    void GetOptions(int argc, char *argv[])
    {
        // opterr = false; // Let us handle all error output for command line options
        int choice;
        int index = 0;
        option long_options[] = {
            {"begin", required_argument, nullptr, 'b'},
            {"end", required_argument, nullptr, 'e'},
            {"stack", no_argument, nullptr, 's'},
            {"queue", no_argument, nullptr, 'q'},
            {"change", no_argument, nullptr, 'c'},
            {"length", no_argument, nullptr, 'l'},
            {"swap", no_argument, nullptr, 'p'},
            {"output", required_argument, nullptr, 'o'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, 0, nullptr, '\0'},
        }; // long_options[]

        while ((choice = getopt_long(argc, argv, "b:e:o:sqclph", long_options, &index)) != -1)
        {
            switch (choice)
            {
            case 'h':
                printHelp();
                exit(0);

            case 'b':
            {
                string arg{optarg};
                if (arg == "")
                {
                    cerr << "Error: Beginning word not specified" << endl;
                    exit(1);
                }
                else
                {
                    begin = arg;
                    b_len = begin.length();
                }
                break;
            }

            case 'e':
            {
                string arg{optarg};
                if (arg == "")
                {
                    cerr << "Error: Ending word not specified" << endl;
                    exit(1);
                }
                else
                {
                    end = arg;
                }
                break;
            }

            case 's':
            {
                stack_mode = true;
                break;
            }

            case 'q':
            {
                queue_mode = true;
                break;
            }

            case 'c':
            {
                change_mode = true;
                break;
            }

            case 'l':
            {
                length_mode = true;
                break;
            }

            case 'p':
            {
                swap_mode = true;
                break;
            }

            case 'o':
            {
                string arg{optarg};
                if (arg == "M")
                {
                    output_W = false;
                }
                else if (arg != "W")
                {
                    cerr << "Error: Invalid output mode specified" << endl;
                    exit(1);
                }

                break;
            }

            default:
                cerr << "Error: Unknown command line option" << endl;
                exit(1);
            } // switch ..choice
        }

        if (stack_mode && queue_mode)   //conflicting stack and queue specified
        {
            cerr << "Error: Conflicting or duplicate stack and queue specified" << endl;
            exit(1);
        }
        if (!stack_mode && !queue_mode) //neither stack or queue specific
        {
            cerr << "Error: Must specify one of stack or queue" << endl;
            exit(1);
        }
        if (!change_mode && !length_mode && !swap_mode) //no mod mode selected
        {
            cerr << "Error: Must specify at least one modification mode (change length swap)" << endl;
            exit(1);
        }
        if (begin.length() != end.length() && !length_mode) //wrong length, no length mode
        {
            cerr << "Error: The first and last words must have the same length when length mode is off" << endl;
            exit(1);
        }
    }

    void readDict()
    { // fills vector<dict_entry> with the read in dictionary
        cin >> dict_type;
        cin >> line_count;
        dict_entries.reserve(line_count);

        string comment;
        cin >> comment;
        while (comment[0] == '/') {
            getline(cin, comment);
            cin >> comment;
        }

        if (dict_type == "S")
        { // simple dict
            readSimpleDict(comment);
        }
        else
        {
            readComplexDict(comment);
        }

        if (b_idx == -1) {
            cerr << "Error: Beginning word does not exist in the dictionary" << endl;
            exit(1);
        }
        if (e_idx == -1) {
            cerr << "Error: Ending word does not exist in the dictionary" << endl;
            exit(1);
        }

        dict_size = static_cast<int>(dict_entries.size());
    }

    void search() {
        deque<int> sc;

        sc.push_front(b_idx);
        dict_entries[b_idx].discover();

        int shared_char_count;
        int diff_char_count;

        bool isSimilar;
        while(!solution) {
            if (stack_mode) {
                cw_idx = sc.back();
                sc.pop_back();
            }
            else {
                cw_idx = sc.front();
                sc.pop_front();
            }
            const int &cw_len    = static_cast<int>(dict_entries[cw_idx].getWord().length());
            const string &cw      = dict_entries[cw_idx].getWord();
            for (int i = 0; i < dict_size; i++) {
                dict_entry &de = dict_entries[i];
                if (!de.isDiscovered()) {
                    const int &w_len     = static_cast<int>(de.getWord().length());
                    isSimilar = false;
                    
                    if (w_len == cw_len) {
                        const string &word = de.getWord();
                        if (change_mode) {
                            diff_char_count   = 0;
                            isSimilar = true;
                            for (int j = 0; j < w_len; j++) {
                                if (word[j] != cw[j]) {
                                    diff_char_count++;
                                    if (diff_char_count == 2) {
                                        isSimilar = false;
                                        break;
                                    }
                                }
                            }
                        }
                        if (swap_mode && !isSimilar) {
                            shared_char_count = 0;
                            diff_char_count = 0;
                            // swapped = false;
                            // isSimilar = true;
                            for (int j = 0; j < w_len; j++) {
                                // if (cw[j] != word[j]) {
                                //     if (swapped) {
                                //         isSimilar = false;
                                //         break;
                                //     }
                                //     if ((cw[j] == word[j+1]) && (cw[j+1] == word[j])) {
                                //         swapped = true;
                                //         j++;
                                //     }
                                //     else {
                                //         isSimilar = false;
                                //         break;
                                //     }
                                // }
                                if (cw[j] == word[j]) {
                                    shared_char_count++;
                                }
                                else {
                                    if ((cw[j] == word[j+1]) && (cw[j+1] == word[j])) {
                                        diff_char_count++;
                                        if (diff_char_count == 2) {
                                            break;
                                        }
                                        j++;
                                    }
                                }
                            }
                            if (shared_char_count == (w_len-2) && diff_char_count == 1) {
                                isSimilar = true;
                            }
                        }
                    }
                    else if (length_mode && !isSimilar && (w_len == cw_len+1 || w_len == cw_len-1)) {
                        const string &word    = de.getWord();
                        shared_char_count = 0;
                        diff_char_count   = 0;
                        if (w_len == cw_len+1) {
                            for (int j = 0, k = 0; k < w_len; k++) {
                                if (cw[j] == word[k]) {
                                    shared_char_count++;
                                    if (shared_char_count == cw_len) {
                                        isSimilar = true;
                                        break;
                                    }
                                    j++;
                                }
                                else {
                                    diff_char_count++;
                                    if (diff_char_count == 2) {
                                        break;
                                    }
                                }
                            }
                        }
                        else {
                            for (int j = 0, k = 0; j < cw_len; j++) {
                                if (cw[j] == word[k]) {
                                    shared_char_count++;
                                    if (shared_char_count == w_len) {
                                        isSimilar = true;
                                        break;
                                    }
                                    k++;
                                }
                                else {
                                    diff_char_count++;
                                    if (diff_char_count == 2) {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if (isSimilar) {
                        de.discover();
                        de.setPrev(cw_idx);
                        sc.push_back(static_cast<int>(i));
                        if (i == e_idx) {
                            solution = true;
                            return ;
                        }
                    }
                }
            }
            if (sc.empty()) { //no path
                break;
            }
        }
    }

    void output() {
        if (!solution) {
            int words_discovered = 0;
            for (dict_entry &de : dict_entries) {
                if (de.isDiscovered()) {
                    words_discovered++;
                }
            }
            cout << "No solution, " << words_discovered << " words discovered." << endl;
        
        }
        else {
            deque<int> word_order;
            int index = (e_idx);
            while (index != (b_idx)) {
                word_order.push_back(index);
                index = dict_entries[index].getPrev();
            }
            word_order.push_back((b_idx));
            size_t morph_count = word_order.size();
            cout << "Words in morph: " << morph_count << "\n";
            if (output_W) {
                while(!word_order.empty()) {
                    cout << dict_entries[word_order.back()].getWord() << "\n";
                    word_order.pop_back();
                }
            }
            else {
                size_t shared_char_count;

                cout << begin << "\n";
                while (word_order.size()-1) {
                //for (size_t i = morph_count-1; i;i--) {
                    const string &cw        = dict_entries[word_order.back()].getWord();
                    word_order.pop_back();
                    const string &word      = dict_entries[word_order.back()].getWord();
                    const size_t w_len     = word.length();
                    const size_t cw_len    = cw.length();

                    if (w_len == cw_len) {
                        int j = 0;
                        while (word[j] == cw[j]) {
                            j++;
                        }
                        if ((cw[j+1] != word[j+1])) {
                            cout << "s," << j << "\n";
                        }
                        else {
                            cout << "c," << j << "," << word[j] << "\n";
                        }
                    }
                    else {
                        shared_char_count = 0;
                        if (w_len == cw_len+1) {
                            for (; shared_char_count != cw_len; shared_char_count++) {
                                if (cw[shared_char_count] != word[shared_char_count]) {
                                    cout << "i," << shared_char_count << "," << word[shared_char_count] << "\n";
                                    break;
                                }
                            }
                            if (shared_char_count == cw_len) {
                                cout << "i," << shared_char_count << "," << word[shared_char_count] << "\n";
                            }
                        }
                        else {
                            for (; shared_char_count != w_len; shared_char_count++) {
                                if (cw[shared_char_count] != word[shared_char_count]) {
                                    cout << "d," << shared_char_count << "\n";
                                    break;
                                }
                            }
                            if (shared_char_count == w_len) {
                                cout << "d," << shared_char_count << "\n";
                            }
                        }
                    }
                }
            }
        }
    }
};

int main(int argc, char *argv[])
{
    std::ios_base::sync_with_stdio(false);
    Letterman object = Letterman();
    object.GetOptions(argc, argv);
    object.readDict();
    object.search();
    object.output();
}