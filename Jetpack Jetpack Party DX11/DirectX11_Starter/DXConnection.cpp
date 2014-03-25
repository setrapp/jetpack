#include "DXConnection.h"

DXConnection* DXConnection::instance = NULL;
DXConnection* DXConnection::Instance()
{
	return instance;
}

DXConnection::DXConnection() 
{
	if (!instance)
	{
		instance = this;
	}
}

DXConnection::~DXConnection()
{
	if (instance == this)
	{
		instance = NULL;
	}
}
