#include<bits/stdc++.h>
using namespace std;

int main(){
    ifstream file("input.txt");
    string line;

    if(file.is_open()){
        // step 1 is parsing the text file
        while(getline(file, line)){
            // maru kaam ahiya karvanu che
            // step 2 is converting them to numbers
            vector<int> numbers;
            for(char c: line){
                if(isalnum(c)){
                    numbers.push_back(c - '0'); 
                }
            }

            // step 3 is scissor cipher and saving as encrypted file
            for(int& num: numbers){
                num = (num + 3) % 26; 
            }

            // save the encrypted numbers to a new file
            ofstream outFile("encrypted.txt", ios::app);
            if(outFile.is_open()){
                for(int num: numbers){
                    outFile << num << " ";
                }
                outFile << endl;
                outFile.close();
            }
        }
        file.close();
    }

    // task 2 is to load that encrypted file and decrypt it
    ifstream encryptedFile("encrypted.txt");
    string line_;
    vector<int> decryptedNumbers;

    if(encryptedFile.is_open()){
        while(getline(encryptedFile, line_)){
            // so step 1 will be doing reverse of enryption in scissor cipher
            for(int num: line_){
                decryptedNumbers.push_back((num - 3 + 26) % 26);
            }

            // step 2 is converting numbers back to characters
            string decryptedLine;
            for(int num: decryptedNumbers){
                decryptedLine += (num + '0'); 
            }

            // step 3 is saving the decrypted line to a new file
            ofstream decryptedFile("decrypted.txt", ios::app);
        }
        encryptedFile.close();
    }
}