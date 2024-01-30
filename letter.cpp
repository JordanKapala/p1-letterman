// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68768FC9

#include <getopt.h>
#include <iostream>
// #include <iomanip>
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
    size_t b_idx = string::npos;
    size_t e_idx = string::npos;
    int cw_idx; 
    bool stack_mode = false;
    bool queue_mode = false;
    bool change_mode = false; // change letter
    bool length_mode = false; // insert or delete
    bool swap_mode = false;   // swap to adjacent letters
    bool output_W = true;     // word format is basic output mode
    bool solution = false;    // there is a solution

    char dict_type; // S or C
    int line_count;
    class dict_entry
    { // nested class to store dict entry info
    private:
        string word;
        int prev = -1;
        bool discovered = false;

    public:
        dict_entry(const string &word) : word(word) {}

        string getWord() const{
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
        void setPrev(const int &p) {
            prev = p;
        }
    };
    vector<dict_entry> dict_entries;

    void readSimpleDict()
    {
        string current_word;

        for (int i = 0; i < line_count; i++) {
            getline(cin, current_word);

            if (current_word[0] == '/')
            { // ignore word if comment
                i--;
            }
            else
            { // instantiate dict entry if a word
                checkBeginEnd(current_word);
                dict_entries.push_back(dict_entry(current_word));
            }
        }
    }

    void readComplexDict()
    {
        string current_word;
        
        for (int i = 0; i < line_count; i++) {
            getline(cin, current_word);

            // size_t reversal_ind = current_word.find("&");
            // size_t insert_ind1  = current_word.find("[");
            // size_t insert_ind2  = current_word.find("]");
            // size_t swap_ind     = current_word.find("!");
            // size_t double_ind   = current_word.find("?");
            size_t special_char_id = current_word.find_first_of("&[!?");

            if (current_word[0] == '/') { // ignore word if comment
                i--;
            }
            else if (special_char_id != string::npos) { //special character mods
                     if (current_word[special_char_id] == '&') { //reversal &
                    current_word.pop_back();
                    checkBeginEnd(current_word);
                    dict_entries.push_back(dict_entry(current_word));
                    for (size_t i = 0; i < current_word.length()/2; i++) {
                        swap(current_word[i], current_word[current_word.length()-i-1]);
                    }
                    checkBeginEnd(current_word);
                    dict_entries.push_back(dict_entry(current_word));
                }
                else if (current_word[special_char_id] == '[') { //insert  []
                    size_t special_char_id2  = current_word.find("]");
                    size_t word_count = special_char_id2 - special_char_id - 1;
                    string chars = current_word.substr(special_char_id+1, word_count);
                    current_word.erase(special_char_id, word_count + 2);
                    for (size_t i = 0; i < word_count; i++) {
                        current_word.insert(special_char_id, 1, chars[i]);
                        checkBeginEnd(current_word);
                        dict_entries.push_back(dict_entry(current_word));
                        current_word.erase(special_char_id, 1);
                    }
                }
                else if (current_word[special_char_id] == '!') { //swap     !
                    current_word.erase(special_char_id, 1);
                    checkBeginEnd(current_word);
                    dict_entries.push_back(dict_entry(current_word));
                    swap(current_word[special_char_id-1], current_word[special_char_id-2]);
                    checkBeginEnd(current_word);
                    dict_entries.push_back(dict_entry(current_word));
                }
                else if (current_word[special_char_id] == '?') { //double   ?
                current_word.erase(special_char_id, 1);
                checkBeginEnd(current_word);
                dict_entries.push_back(dict_entry(current_word));
                current_word.insert(special_char_id, 1, current_word[special_char_id-1]);
                checkBeginEnd(current_word);
                dict_entries.push_back(dict_entry(current_word));
            }
            }
            else {
                checkBeginEnd(current_word);
                dict_entries.push_back(dict_entry(current_word));
            }
        }
    }

    void checkBeginEnd(const string &current_word) 
    {
        if (current_word == begin) {
            b_idx = dict_entries.size();
        }
        if (current_word == end) {
            e_idx = dict_entries.size();
        }
    }

    void checkBeginEndinDict()
    {

        auto b_it = find_if(dict_entries.begin(),
                         dict_entries.end(),
                         [&](const dict_entry &de)
                         { return (de.getWord() == begin); });

        auto e_it = find_if(dict_entries.begin(),
                         dict_entries.end(),
                         [&](const dict_entry &de)
                         { return (de.getWord() == end); });

        if (b_it == dict_entries.end())
        {
            cerr << "Error: Beginning word does not exist in the dictionary" << endl;
            exit(1);
        }
        if (e_it == dict_entries.end())
        {
            cerr << "Error: Ending word does not exist in the dictionary" << endl;
            exit(1);
        }

        
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
                // printHelp(argv);
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
        cin.ignore();

        dict_entries.reserve(line_count);

        if (dict_type == 'S')
        { // simple dict
            readSimpleDict();
        }
        else
        {
            readComplexDict();
        }

        if (b_idx == string::npos) {
            cerr << "Error: Beginning word does not exist in the dictionary" << endl;
            exit(1);
        }
        if (e_idx == string::npos) {
            cerr << "Error: Ending word does not exist in the dictionary" << endl;
            exit(1);
        }

        //checkBeginEndinDict();
    }

    void search() {
        deque<int> sc;

        sc.push_front(static_cast<int>(b_idx));
        dict_entries[b_idx].discover();

        size_t shared_char_count;
        size_t diff_char_count;
        string word;
        string cw;
        size_t w_len;
        size_t cw_len;

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
            
            for (size_t i = 0; i < dict_entries.size(); i++) {
                dict_entry &de = dict_entries[i];
                if (!de.isDiscovered()) {
                    word      = de.getWord();
                    cw        = dict_entries[cw_idx].getWord();
                    w_len     = word.length();
                    cw_len    = cw.length();
                    isSimilar = false;
                    
                    if (change_mode && w_len == cw_len) {
                        diff_char_count   = 0;
                        isSimilar = true;
                        for (size_t j = 0; j < w_len; j++) {
                            if (word[j] != cw[j]) {
                                diff_char_count++;
                                if (diff_char_count == 2) {
                                    isSimilar = false;
                                    break;
                                }
                            }
                        }
                    }
                    else if (length_mode && !isSimilar && (w_len == cw_len+1 || w_len == cw_len-1)) {
                        shared_char_count = 0;
                        diff_char_count   = 0;
                        if (w_len == cw_len+1) {
                            for (size_t j = 0, k = 0; k < w_len; k++) {
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
                            for (size_t j = 0, k = 0; j < cw_len; j++) {
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
                    else if (swap_mode && !isSimilar && w_len == cw_len) {
                        shared_char_count = 0;
                        diff_char_count   = 0;
                        for (size_t j = 0; j < w_len; j++) {
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

                    if (isSimilar) {
                        de.discover();
                        de.setPrev(cw_idx);
                        sc.push_back(static_cast<int>(i));
                        if (word == end) {
                            solution = true;
                            break;
                        }
                    }
                }
            }
            if (sc.empty()) { //no path
                break;
                // int words_discovered = 0;
                // for (dict_entry &de : dict_entries) {
                //     if (de.getPrev() != -1) {
                //         words_discovered++;
                //     }
                // }
                // cout << "No solution"
            }
        }
    }

    void output() {
        //no solution
        //solution, Word mode
        //solution, Mod mode
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
            vector<int> word_order;
            int index = static_cast<int>(e_idx);
            while (index != static_cast<int>(b_idx)) {
                word_order.push_back(index);
                index = dict_entries[index].getPrev();
            }
            word_order.push_back(static_cast<int>(b_idx));
            size_t morph_count = word_order.size();
            cout << "Words in morph: " << morph_count << "\n";

            if (output_W) {
                for (size_t i = morph_count-1; i;i--) {
                    cout << dict_entries[word_order[i]].getWord() << "\n";
                }
                cout << end << endl;
            }
            else {
                //word_order end, ..., begin with size n
                //run n times
                //look at end word,
                size_t shared_char_count;
                string word;
                string cw;
                size_t w_len;
                size_t cw_len;

                cout << begin << "\n";
                for (size_t i = morph_count-1; i;i--) {
                    word      = dict_entries[word_order[i-1]].getWord();
                    cw        = dict_entries[word_order[i]].getWord();
                    w_len     = word.length();
                    cw_len    = cw.length();

                    if (w_len == cw_len) {
                        for (size_t j = 0; j < w_len; j++) {
                            if (word[j] != cw[j]) {
                                if (swap_mode && (cw[j] == word[j+1]) && (cw[j+1] == word[j])) {
                                    cout << "s," << j << "\n";
                                }
                                else {
                                    cout << "c," << j << "," << word[j] << "\n";
                                }
                                break;
                            }
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
    //std::ios_base::sync_with_stdio(false);
    Letterman object = Letterman();
    object.GetOptions(argc, argv);
    object.readDict();
    object.search();
    object.output();
}