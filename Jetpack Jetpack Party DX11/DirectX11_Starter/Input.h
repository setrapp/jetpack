#ifndef INPUT_H
#define INPUT_H
#include <WinUser.h>
#include "Response.h"

static class Input
{
protected:  
	static Response currentResponse;
	static Response previousResponse;

public: 
	virtual Response GetKey(int key) = 0;

};


#endif