#include <bits/stdc++.h>
using namespace std;

int main()
{
    ifstream infile("input.txt"); 
    string line;

    if (infile.is_open())
    {
        // step 1 is parsing the text file
        string str = "";
        while (getline(infile, line))
        {
            // maru kaam ahiya karvanu che
            // step 2 is converting them to numbers and then do scissor cipher
            // and then do converting back to chars and store it in an output file
            for (char c : line)
            {
                if (isalpha(c)) 
                {
                    char base = isupper(c) ? 'A' : 'a'; 
                    int num = c - base;
                    num = (num + 3) % 26;
                    str += base + num;
                }
                else
                {
                    str += c;
                }
            }
            str += "\n";
        }

        ofstream outfile("encrypted.txt");
        str += "\n";  
        if (outfile.is_open())
        {
            outfile << str;
            outfile.close();
        }

        // also i want to save the encrypted things as numbers
        // will be using -1 as the delimeter for spaces
        string nums = "";
        for(char c: str){
            if(c == ' ') nums += "  ";
            else{
                if(isalpha(c)){
                    // then i will convert to number
                    char base = isupper(c) ? 'A' : 'a';
                    int num = c - base;
                    nums += to_string(num) + " ";
                }
                else nums += c;
            }
        }

        ofstream numfile("Encrypted_Numbers.txt");
        while(numfile.is_open()){
            numfile << nums;
            numfile.close();
        }
        infile.close();  
    }

    // task 2 is to load that encrypted file and decrypt it
    ifstream encryptedFile("encrypted.txt");
    string line_;
    vector<int> decryptedNumbers;

    if (encryptedFile.is_open())
    {
        string decrypted_str = "";
        while (getline(encryptedFile, line_))
        {
            // so step 1 will be doing reverse of enryption in scissor cipher
            for(char c: line_){
                if(isalpha(c)){
                    char base = isupper(c) ? 'A' : 'a';
                    int num = c - base;
                    num = (num - 3 + 26) % 26;
                    decrypted_str += base + num;
                }
                else decrypted_str += c;
            }
            decrypted_str += "\n";
        }

        ofstream decout("decrypted.txt");
        decrypted_str += "\n";
        if(decout.is_open()){
            decout << decrypted_str;
            decout.close();
        }
        encryptedFile.close();
    }
}