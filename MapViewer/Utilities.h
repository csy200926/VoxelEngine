#pragma once
#include <glm.hpp>
#include <string>
#include <sstream>
#include <vector>
namespace Utilities
{
	using namespace glm;
	using namespace std;

	int signum(int i);
	void swap(int &x, int &y);
	void SwapVec2(vec2 &v1, vec2 &v2);

	int signum(int i)
	{
		return (i > 0) - (i < 0);
	}
	void swap(int &x, int &y)
	{
		int temp = x;
		x = y;
		y = temp;
	}
	void SwapVec2(vec2 &v1, vec2 &v2)
	{
		vec2 temp = v1;
		v1 = v2;
		v2 = temp;
	}

	void BarycentricCoord(float &a,float &b, float &c, vec2 &v1,vec2 &v2,vec2 &v3,int x,int y)
	{
		a = ( (v2.y-v3.y)*(x-v3.x)+(v3.x-v2.x)*(y-v3.y) ) / ( (v2.y-v3.y)*(v1.x-v3.x) + (v3.x-v2.x)*(v1.y-v3.y) );
		b = ( (v3.y-v1.y)*(x-v3.x)+(v1.x-v3.x)*(y-v3.y) ) / ( (v2.y-v3.y)*(v1.x-v3.x) + (v3.x-v2.x)*(v1.y-v3.y) );
		c = 1 - a - b;
	}

	vector<string> &split(const string &s, char delim, vector<string> &elems) {
		stringstream ss(s);
		string item;
		while (getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}


	vector<string> split(const string &s, char delim) {
		vector<string> elems;
		split(s, delim, elems);
		return elems;
	}
}