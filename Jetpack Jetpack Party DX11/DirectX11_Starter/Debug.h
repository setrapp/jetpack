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

	static string ToString(float t)
	{		
		return to_string(t);
	}

	static string ToString(double t)
	{		
		return to_string(t);
	}

	static string ToString(XMFLOAT4 v)
	{
		std::stringstream ss;
		ss << "\n";
		ss << "x : " << v.x << "\ny : " << v.y << "\nz : " << v.z << "\nw : " << v.w;
		ss << "\n";
		return ss.str();
	}

	static string ToString(XMFLOAT4X4 v)
	{
		std::stringstream ss;
		int four = 0;
		ss << "\n";
		ss << "XMFLOAT4X4"<< "\n";
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
		ss << "\n";
		return ss.str();
	}

	static string ToString(XMFLOAT3X3 v)
	{
		std::stringstream ss;
		int four = 0;
		ss << "\n";
		ss << "XMFLOAT3X3"<< "\n";
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
		ss << "\n";
		return ss.str();
	}

	static void Log(string s)
	{
		OutputDebugStringA(s.c_str());
	}

	static string ToString(void * ptr)
	{
		std::stringstream ss;
		ss << "\n";
		if(ptr)
		{
			ss << ptr;
		}
		else
			ss << "Pointer is NULL";
		ss << "\n";
		return ss.str();
	}
};

#endif