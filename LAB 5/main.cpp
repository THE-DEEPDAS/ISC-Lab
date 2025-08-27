#include <bits/stdc++.h>
using namespace std;
/*
Playfair Cipher - Starter Skeleton (NO SOLUTION CODE)
----------------------------------------------------
Complete the TODOs below. Do NOT change function signatures.
You must implement:
2
- buildKeySquare
- toDigraphs
- encrypt
- decrypt
CLI usage (choose one style):
1) Flags:
./playfair --mode enc --key "MONARCHY" --text "INSTRUMENTS"
2) Stdin (three lines):
enc
MONARCHY
INSTRUMENTS
*/
// --- Utilities you may use as-is ---
string normalize(const string &s)
{
    string r;
    for (char ch : s)
    {
        if ('a' <= ch && ch <= 'z')
            r.push_back(ch - 'a' + 'A');
        else if ('A' <= ch && ch <= 'Z')
            r.push_back(ch);
    }
    for (char &ch : r)
        if (ch == 'J')
            ch = 'I'; // J -> I convention
    return r;
}
// --- TODO #1: Build the 5x5 key square and fill ’pos’ map ---
// Requirements:
// - Use alphabet A..Z without ’J’ (merge J->I already handled by normalize

// - Remove duplicate letters from keyword preserving order.
// - Fill remaining letters of alphabet (without J) afterwards.
// - Return a 5x5 array<char,5> grid, and fill ’pos’ with positions for quick lookup.
array<array<char,5>,5> buildKeySquare(const string &keyword, map<char,pair<int,int>> &pos)
{
    // TODO: construct grid and fill ’pos’
    // so me logic em lagaivu ke jem jem aavta jaay tem tem bharto javu and pachi je ni aavya e hari devana 
    // as 25 chars bharvana hata so this seems most appropriate to me.
    array<array<char,5>,5> grid;
    unordered_set<char> used;

    int row = 0, col = 0;
    for(char c: keyword){
        if(used.count(c) == 0){
            used.insert(c);
            grid[row][col] = c;
            pos[c] = {row, col};
            col++;
            if(col == 5){
                col = 0;
                row++;
            }
        }
    }
    for(char c = 'A'; c <= 'Z'; c++){
        if(c != 'J' && used.count(c) == 0){
            grid[row][col] = c;
            pos[c] = {row, col};
            col++;
            if(col == 5){
                col = 0;
                row++;
            }
        }
    }
    return grid;
}
// --- TODO #2: Split plaintext into digraphs with Playfair rules ---
// Rules:
// - Work on normalized text (A-Z, J->I, no non-letters).
// - If a pair would be ’AA’, insert ’X’ between them: ’A X A’ (thencontinue).
// - If final letter is alone, pad with ’X’.
vector<pair<char,char>> toDigraphs(const string &plaintext)
{
    // TODO: create digraphs as per rules
    string norm = normalize(plaintext);
    vector<pair<char,char>> digraphs;
    size_t i = 0;
    while (i < norm.size()) {
        char a = norm[i];
        char b;
        if (i + 1 == norm.size()) {          // last single
            b = 'X';
            ++i;
        } else if (a == norm[i + 1]) {       // double letter: insert X
            b = 'X';
            ++i;                             // only advance one, second char reprocessed
        } else {
            b = norm[i + 1];
            i += 2;                          // consumed both
        }
        digraphs.emplace_back(a, b);
    }
    return digraphs;
}
// --- TODO #3: Encrypt using Playfair rules ---
// For each pair (a,b) at positions (ra,ca), (rb,cb):
// - Same row: replace with letters to the RIGHT (wrap around).
// - Same column: replace with letters BELOW (wrap around).
// - Rectangle: replace a with (ra,cb) and b with (rb,ca).
string encrypt(const string &plaintext, const array<array<char, 5>, 5> &grid,
               map<char, pair<int, int>> &pos)
{
    // TODO: implement encryption using toDigraphs(...) and ’grid’/’pos’
    vector<pair<char,char>> digraphs = toDigraphs(plaintext);
    string ciphertext;
    for(const auto &it: digraphs){
        auto a = it.first, b = it.second;
        int ra = pos[a].first, ca = pos[a].second;
        int rb = pos[b].first, cb = pos[b].second;
        if(ra == rb){ // same row
            ciphertext.push_back(grid[ra][(ca + 1) % 5]);
            ciphertext.push_back(grid[rb][(cb + 1) % 5]);
        }
        else if(ca == cb){ // same column
            ciphertext.push_back(grid[(ra + 1) % 5][ca]);
            ciphertext.push_back(grid[(rb + 1) % 5][cb]);
        }
        else{ // rectangle
            ciphertext.push_back(grid[ra][cb]);
            ciphertext.push_back(grid[rb][ca]);
        }
    }
    return ciphertext;
}
// --- TODO #4: Decrypt using inverse rules ---
// Same row -> shift LEFT; same column -> shift UP; rectangle rule is
// symmetric.
string decrypt(const string &ciphertext, const array<array<char, 5>, 5> &grid, map<char, pair<int, int>> &pos)
{
    // TODO: implement decryption
    string norm = normalize(ciphertext);
    string plaintext;
    if (norm.size() % 2) norm.pop_back(); // safety
    for (size_t i = 0; i + 1 < norm.size(); i += 2) {
        char a = norm[i], b = norm[i+1];
        int ra = pos[a].first, ca = pos[a].second;
        int rb = pos[b].first, cb = pos[b].second;
        if(ra == rb){
            plaintext.push_back(grid[ra][(ca + 5 - 1) % 5]);
            plaintext.push_back(grid[rb][(cb + 5 - 1) % 5]);
        }
        else if(ca == cb){
            plaintext.push_back(grid[(ra + 5 - 1) % 5][ca]);
            plaintext.push_back(grid[(rb + 5 - 1) % 5][cb]);
        }
        else{
            plaintext.push_back(grid[ra][cb]);
            plaintext.push_back(grid[rb][ca]);
        }
    }
    return plaintext;
}
int main(int argc, char **argv)
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string mode, key, text;
    // --- Simple CLI parsing ---
    if (argc > 1)
    {
        for (int i = 1; i < argc; ++i)
        {
            string a = argv[i];
            if (a == "--mode" && i + 1 < argc)
                mode = argv[++i];
            else if (a == "--key" && i + 1 < argc)
                key = argv[++i];
            else if (a == "--text" && i + 1 < argc)
                text = argv[++i];
        }
    }
    if (mode.empty())
    {
        // Fallback: read three lines from stdin
        if (!getline(cin, mode))
            return 0;
        getline(cin, key);
        getline(cin, text);
    }
    try
    {
        map<char, pair<int, int>> pos;
        auto grid = buildKeySquare(key, pos);
        if (mode == "enc")
        {
            cout << encrypt(text, grid, pos) << "\n";
        }
        else if (mode == "dec")
        {
            cout << decrypt(text, grid, pos) << "\n";
        }
        else
        {
            cerr << "Invalid mode. Use enc|dec.\n";
            return 1;
        }
    }
    catch (const exception &e)
    {
        cerr << "[ERROR] " << e.what() << "\n";
        cerr << "Hint: Complete the TODOs in buildKeySquare / toDigraphs / encrypt / decrypt.\n";
        return 2;
    }
    return 0;
}