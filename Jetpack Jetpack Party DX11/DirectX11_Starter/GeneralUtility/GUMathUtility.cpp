#include "GUMathUtility.h"

//calculate base raised to the power
int guPowInt(int base, int power)
{
    //declare counter and the result to be returned
	int i = 0;
	int result = 1;
    
	//unless base is one this becomes a fractional number less than 1 so return 0
	if(base != 1 && power < 0)
		return 0;

	//step through each power and multiply result by base
	for(i = 1; i <= power; i++)
		result *= base;
	return result;
}

//calculate base raised to the power
unsigned int guPowUInt(unsigned int base, int power)
{
	//declare counter and the result to be returned
    int i = 0;
	unsigned int result = 1;
    
	//unless base is one this becomes a fractional number less than 1 so return 0
	if(base != 1 && power < 0)
		return 0;

	//step through each power and multiply result by base
	for(i = 1; i <= power; i++)
		result *= base;
	return result;
}

//calculate base raised to the power
long guPowLong(long base, int power)
{
    //declare counter and the result to be returned
	int i = 0;
	long result = 1;
    
	//unless base is one this becomes a fractional number less than 1 so return 0
	if(base != 1 && power < 0)
		return 0;

	//step through each power and multiply result by base
	for(i = 1; i <= power; i++)
		result *= base;
	return result;
}

//calculate base raised to the power
unsigned long guPowULong(unsigned long base, int power)
{
    //declare counter and the result to be returned
	int i = 0;
	unsigned long result = 1;
    
	//unless base is one this becomes a fractional number less than 1 so return 0
	if(base != 1 && power < 0)
		return 0;

	//step through each power and multiply result by base
	for(i = 1; i <= power; i++)
		result *= base;
	return result;
}

//normalize a vector
void guNormalize(GUVector3D* vector)
{
	//calculate magnitude of vector
	float mag = (float)sqrt(vector->x*vector->x + vector->y*vector->y + vector->z*vector->z);
	//return if magnitude is zero
	if(mag == 0)
		return;
	//divide each dimension by the magnitude so that they sum to one
	vector->x = vector->x/mag;
	vector->y = vector->y/mag;
	vector->z = vector->z/mag;
}

//calculate dot product of two vectors
float guDotProduct(GUVector3D vector1, GUVector3D vector2)
{
	return (vector1.x * vector2.x) + (vector1.y * vector2.y) + (vector1.z * vector2.z);
}

//calculate cross product of two vectors
GUVector3D guCrossProduct(GUVector3D vector1, GUVector3D vector2)
{
	GUVector3D outVector;
	outVector.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
	outVector.y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
	outVector.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);
	return outVector;
}

//calculate the surface normal of the face defined by the passed in vertices and calculate normals of vertices 
//to work properly vertices must appear in counter-clockwise order
GUNormal3D guCalcPlaneNormal(GUPoint3D const* point1, GUPoint3D const* point2, GUPoint3D const* point3)
{
	GUVector3D edge1;
	GUVector3D edge2;
	GUNormal3D normal;
	
	//initialize normal as zero vector
	normal.x = 0;
	normal.y = 0;
	normal.z = 0;
	
	//if any of the points specified does not exist, return 0 normal
	if(!(point1 && point2 && point3))
		return normal;
	
	//define an edge of the triangle
	edge1.x = point2->x - point1->x;
	edge1.y = point2->y - point1->y;
	edge1.z = point2->z - point1->z;
	//define another edge of the triangle
	edge2.x = point3->x - point1->x;
	edge2.y = point3->y - point1->y;
	edge2.z = point3->z - point1->z;

	//get normal using the cross product of the two edges
	normal = guCrossProduct(edge1, edge2);

	//normalize
	guNormalize(&normal);

	return normal;
}