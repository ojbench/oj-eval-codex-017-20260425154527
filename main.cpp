#include <iostream>
#include <string>
using namespace std;
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string line;
    while (getline(cin, line)){
        if (line.size()==4 && line[0]=='e' && line[1]=='x' && line[2]=='i' && line[3]=='t'){
            cout.put('b'); cout.put('y'); cout.put('e'); cout.put('\n');
            break;
        }
        cout << -1; cout.put('\n');
    }
    return 0;
}
