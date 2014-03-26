#ifndef RESPONSE_H
#define RESPONSE_H
union Response
{
	bool keyPressed;
	char keyCode;
	MouseResponse* mResponse;
};

typedef struct MouseResponse
{
	bool left;
	bool right;
	bool middle;
}

#endif