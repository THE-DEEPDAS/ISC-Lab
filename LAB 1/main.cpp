#include<bits/stdc++.h>
using namespace std;

int main(){
	string s;
	cin >> s;
	
	for (auto& x : s) {
        x = tolower(x);
    }

    cout << "Spaces are for better visualization" << "\n";
	string raw = "", ans = "";
	for(int i = 0; i < s.length(); i++){
		int j = int(s[i] - 'a');
		raw += to_string(j) + " ";
		
		char c = ((j + 6) % 26) + 97;
		ans += c;
		ans += " ";
	}
	
	cout << raw << "\n";
	cout << ans << "\n";
}