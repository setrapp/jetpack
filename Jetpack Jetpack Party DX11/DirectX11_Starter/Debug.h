#pragma once 
#ifdef _DEBUG
#include <string>
#include <string.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <sstream>
#include <iostream>

using namespace std;
using namespace DirectX;

//Runtime values can be debugged, at the cost of FPS. So this is a Debug only. 
class Debug
{
public: 

	inline static string ToString(const float t)
	{		
		std::stringstream ss;
		ss << t;
		return ss.str();
	}

	inline static string ToString(const double t)
	{		
		std::stringstream ss;
		ss << t;
		return ss.str();
	}

	inline static string ToString(const short t)
	{		
		std::stringstream ss;
		ss << t;
		return ss.str();
	}

	inline static string ToString(const unsigned short t)
	{		
		std::stringstream ss;
		ss << t;
		return ss.str();
	}

	inline static string ToString(const XMFLOAT4 v)
	{
		std::stringstream ss;
		ss << "x : " << v.x << "\ny : " << v.y << "\nz : " << v.z << "\nw : " << v.w;
		return ss.str();
	}

	inline static string ToString(XMFLOAT4X4 v)
	{
		std::stringstream ss;
		int four = 0;
		for( float* f : v.m)
		{
			for( int i = 0 ; i < 4; i++)
			{
				ss << four + 1 << i + 1 << " : ";
				ss << f[i];
				ss << "\t";
			}
			ss << "\n";
			four ++;
		}
		return ss.str();
	}

	inline static string ToString(XMFLOAT3X3 v)
	{
		std::stringstream ss;
		int four = 0;
		for( float* f : v.m)
		{
			for( int i = 0 ; i < 3; i++)
			{
				ss << four + 1 << i + 1 << " : ";
				ss << f[i];
				ss << "\t";
			}
			ss << "\n";
			four ++;
		}
		return ss.str();
	}

	inline static void Log(const string s)
	{
		string line = s + "\n";
		OutputDebugStringA(line.c_str());
	}

	inline static string ToString(const void * ptr)
	{
		std::stringstream ss;
		if(ptr)
		{
			ss << ptr;
		}
		else
			ss << "Pointer is NULL";
		return ss.str();
	}


	
};

#endif