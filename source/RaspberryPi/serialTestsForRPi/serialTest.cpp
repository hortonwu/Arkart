#include <iostream>
#include <fstream>
using namespace std;

int main(){
	cout << "hello!" << endl;
	string text = "{F0}";
	ofstream fout ("/dev/ttyAMA0");
	
	fout << text << endl;
	
	fout.close();
	
	return 0;

}
