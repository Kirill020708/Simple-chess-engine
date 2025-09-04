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



const int KNIGHT_PROM=1,BISHOP_PROM=2,ROOK_PROM=3,QUEEN_PROM=4;

ull generateMask(int start,int end){//mask with 1-s from start bit to end bit
	return ((1ull<<(end-start+1))-1)<<start;
}