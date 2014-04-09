#ifndef GENERAL_UTILITY_PARSE_UTILITY_H
#define GENERAL_UTILITY_PARSE_UTILITY_H

#ifdef _WIN32
	#define GU_DIRECTORY_DELIMITER 92 
#elif __APPLE__
	#include "TargetConditionals.h"
	#include <CoreFoundation/CoreFoundation.h>
	#define GU_DIRECTORY_DELIMITER 47
	#define GU_USING_BUNDLES
	#ifdef TARGET_OS_IPHONE
	#elif TARGET_OS_IPHONE_SIMULATOR
	#elif TARGET_OS_MAC
	#endif
#elif __linux__
	#define GU_DIRECTORY_DELIMITER 47
#endif

// Disable warning about unsafe iterators (we aren't multi-threading)
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define GU_NO_BUFFER_LIMIT -1
#define GU_DEFAULT_BUFFER_SIZE 256
#define GU_DEFAULT_DIGITS_MAX 10

    //enumeration used when parsing decimals
    typedef enum
    {
        GU_BASE_BINARY			= 2,
        GU_BASE_OCTAL           = 8,
        GU_BASE_DECIMAL         = 10,
        GU_BASE_HEXADECIMAL     = 16

    }GUNumBases;

    //enumeration used when comparing two objects
    typedef enum
    {
        GU_EQUAL			= 0,
        GU_FIRST_GREATER	= 1,
        GU_SECOND_GREATER	= -1,
        GU_NO_TERMINATION	= -100
    }GUCompareStats;

    //enumeration for string reading
    typedef enum
    {
        GU_STRING_BEGIN = 0,		
        GU_STRING_END = -1
    }GUStringPositions;

    //enumeration of data types that could be read from a line in a file, arrays of size 1 are used for single values
    typedef enum
    {
        GU_NO_TYPE,
        GU_INT_ARRAY,
        GU_UNSIGNED_INT_ARRAY,
        GU_SHORT_ARRAY,
        GU_UNSIGNED_SHORT_ARRAY,
        GU_LONG_ARRAY,
        GU_UNSIGNED_LONG_ARRAY,
        GU_FLOAT_ARRAY,
        GU_DOUBLE_ARRAY,
        GU_CHAR_ARRAY
    }GUReadDataTypes;

    //
    typedef enum
    {
        GU_MULTIPLE_LINES = 0,
        GU_SINGLE_LINE
    }GULinePlurality;

    //--- File Related ---//
    int guAttemptFileOpen(FILE** file, char const* filename, char const* desiredExtension, int filenameMaxSize, int extensionMaxSize);
    char* guExtractFilePath(char const* filename, char** filePathOut, int* pathSize, char const directoryDelimiter);
    char* guExtractFilename(char const* filename, char** filenameOut, int* pathlessFilenameSize, char const directoryDelimiter);
    //--- Line Related ---//
    char* guSubstring(char const* fullString, char** substringOut, int firstIndexInString, int lastIndexInString, int fullStringSize, int* sizeOut);
    char* guScanLineRaw(char const* line, char** substringOut, int start, int end, int* sizeOut);
    char* guScanLineWithDelimiter(char const* line, char** substringOut, char delimiter, int delimiterIndex);
    char* guScanLine(char const* line, char** substringOut, int* delimiterIndices, int delimiterCount, int delimiterIndex);
    //--- String Related ---//
    char* guEmptyString(char** toEmpty);
    int guStringLength(char const* cString, int singleOrMultipleLines, int maxSize);
    int guIndexOf(char const* searchString, char goalCharacter, int startIndex, int maxDistanceFromStart);
    int guCompare(char const* cString1, char const* cString2, int caseSensitive_bool, int maxSize);
    char* guConcat(char const* cString1, char const* cString2, char** concatOut, int* resultSize, int inputMaxSize);
    char* guCopyString(char const* original, char** duplicateOut, const int maxSize);
    //---Number Related ---//
    int* guFindDelimiters(char const* inputString, char delimiter, int* delimiterCountToFind, int maxCharacters);
    int guCheckNumerical(char* inputString, int* sizeOut, int* radixPoint, int* xPoint, unsigned int numericalBaseIn, int maxDigits);
    int guParseInt(char* inputString, int* outputNum, unsigned int numericalBase, int maxDigits);
    int guParseUInt(char* inputString, unsigned int* outputNum, unsigned int numericalBase, int maxDigits);
    int guParseShort(char* inputString, short* outputNum, unsigned int numericalBase, int maxDigits);
    int guParseUShort(char* inputString, unsigned short* outputNum, unsigned int numericalBase, int maxDigits);
    int guParseLong(char* inputString, long* outputNum, unsigned int numericalBase, int maxDigits);
    int guParseULong(char* inputString, unsigned long* outputNum, unsigned int numericalBase , int maxDigits);
    int guParseFloat(char* inputString, float* outputNum, unsigned int numericalBase, int maxDigits);
    int guParseDouble(char* inputString, double* outputNum, unsigned int numericalBase, int maxDigits);

//---System Dependent---//
#ifdef __COREFOUNDATION_COREFOUNDATION__
#ifdef GU_USING_BUNDLES
    char* guWrapFilenameInBundle(CFStringRef filename, char** wrappedFilenameOut);
#endif
#endif
    
#ifdef __cplusplus
}
#endif

#endif
