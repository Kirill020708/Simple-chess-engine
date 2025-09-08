// #include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
#include <set>
#include <cmath>
#include <deque>
#include <map>
#include <cassert>
#include <queue>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <random>
#include <mutex>


using namespace std;
 
typedef long long ll;
typedef unsigned long long ull;
typedef __int128 lll;
typedef long double ld;
const ll mod = 1e9+7, inf = 2e15 + 1;
const ll noneCode=inf/173;
const ld eps = 1e-9, pi = 3.1415926;
#define all(a) a.begin(), a.end()
#define mpp make_pair
#define F first
#define S second
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());


const int EMPTY=-1,WHITE=0,BLACK=1,ERROR=-2;
const int NOPIECE=0,PAWN=1,KNIGHT=2,BISHOP=3,ROOK=4,QUEEN=5,KING=6;


vector<string>splitStr(string s,string c){//splits a string by (c) substrings (for FEN parsing)
	s+=c;
	vector<string>an;
	string t;
	for(auto i:s){
		t.push_back(i);
		if(t.length()>=c.length()&&t.substr(t.length()-c.length(),c.length())==c){
			t=t.substr(0,t.length()-c.length());
			if(t!="")
				an.push_back(t);
			t="";
		}
	}
	return an;
}

string squareNumberToUCI(int square){
    string s;
    s.push_back(char('a'+(square&7)));
    s.push_back(char('0'+8-(square>>3)));
    return s;
}