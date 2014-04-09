#include "GUParseUtility.h"
#include "GUMathUtility.h"

//attempt to open a file and check if the file extension matches the desired externsion ('.' can be included or excluded), return success or failure
int guAttemptFileOpen(FILE** file, char const* filename, char const* desiredExtension, int filenameMaxSize, int extensionMaxSize)
{	
    //declare counter
    int i = 0;

	//declare file opening varibles (define what can be)
	int filenameSize = -1;
	int dotIncluded;
	int dotLocation = -1;
	int extensionSize = -1;
	char* fileExtension;
	
	//--- Compare file extension to desired extension if the latter was provided ---//
	if(desiredExtension != NULL)
	{
		dotIncluded = desiredExtension[0] == '.';

		//allow the '.' to be a part of the extension length
		if(dotIncluded)
			extensionMaxSize++;

		//determine the size of desiredExtension
		for(i = 0; i < extensionMaxSize+1 && extensionSize < 0; i++)
			if(desiredExtension[i] == '\0')
				extensionSize = i;
		//return a failure if the desired extension does not terminate properly
		if(extensionSize < 0)
		{
			printf("The termination of the string provided as the desired extension was not found. %s was not opened\n.", filename);
			return 0;
		}

		//determine the size of the filename and find the required '.'
		for(i = 0; i < filenameMaxSize+1 && filenameSize < 0; i++)
		{
			if(filename[i] == '.')
				dotLocation = i;
			if(filename[i] == '\0')
				filenameSize = i;
		}
		//return a failure if the filename does not terminate properly
		if(filenameSize < 0)
		{
			printf("The termination of the string provided as the filename was not found. %s was not opened\n.", filename);
			return 0;
		}
		//return failure if '.' was not found or if the filename is too short to contain the extention string
		if(dotLocation < 0 || filenameSize < extensionSize + (dotIncluded ? 0 : 1))
			return 0;

		//retrive the possible extension of the file being opened
		fileExtension = guScanLineRaw(filename, NULL, dotLocation, GU_STRING_END, NULL);

		//compare the extenions
		if(guCompare(fileExtension, desiredExtension, 0, extensionMaxSize) != 0)
        {
            free(fileExtension);
			return 0;
        }
        free(fileExtension);
	}
	
	//attempt to open file
	if((*file = fopen(filename, "r")) == 0)
	{
		printf("%s could not be opened\n", filename);
		return 0;
	}

	return 1;
}

//extract the directory path specified in the filename
char* guExtractFilePath(char const* filename, char** filePathOut, int* pathSize, char const directoryDelimiter)
{
	//find locations of directory delimiter
	int delimiterCount;
	int* delimiters = guFindDelimiters(filename, directoryDelimiter, &delimiterCount, GU_DEFAULT_BUFFER_SIZE);
	//substring the filename from the beginning to the last directory delimiter
	return guScanLineRaw(filename, filePathOut, 0, delimiters[delimiterCount-2], pathSize);
}

//extract the pathless filename specified in the filename
char* guExtractFilename(char const* filename, char** filenameOut, int* pathlessFilenameSize, char const directoryDelimiter)
{
	//find locations of directory delimiter
	int delimiterCount;
	int* delimiters = guFindDelimiters(filename, directoryDelimiter, &delimiterCount, GU_DEFAULT_BUFFER_SIZE);
	//substring the filename from the beginning to the last directory delimiter
	return guScanLineRaw(filename, filenameOut, delimiters[delimiterCount-2]+1, delimiters[delimiterCount-1], pathlessFilenameSize);
}

//get a portion of a string from first index to last index (inclusive)
char* guSubstring(char const* fullString, char** substringOut, int firstIndexInString, int lastIndexInString, int fullStringSize, int* sizeOut)
{
    //declare counter
    int i;
    
    //declare substring buffer and size of substring
    char* substring;
    int size;
    
    //if the start index is beyond the number of the valid character in the full string, return a failure
    if(firstIndexInString >= ((fullStringSize >= 0) ? fullStringSize-1 : guStringLength(fullString, GU_MULTIPLE_LINES, GU_NO_BUFFER_LIMIT)-1))
        return NULL;
	
    //calculate a size large enough to hold all substring characters and '\0'
	size = lastIndexInString-firstIndexInString+2;
    
    //if a buffer was given to hold the substring, address it
    if(substringOut && *substringOut)
        substring = *substringOut;
    //otherwise allocate space for a new buffer
    else
        substring = (char*)malloc(sizeof(char) * (size));	
    
    //copy characters to substring
	for(i = firstIndexInString; i <= lastIndexInString && fullString[i] != '\0' ; i++)
		substring[i-firstIndexInString] = fullString[i];
    
    //add termination character at the end of the substring
	substring[lastIndexInString-firstIndexInString+1] = '\0';	
    
    //if an address was given to store the substring size, store the size
    if(sizeOut)
        *sizeOut = size;
    
	return substring;
}

//scan line from one index to another, inclusive (clipped to 0 and end of line)
//passing in a number below 0 for the end parameter allows the line to be scanned to its own end
//sizeOut can then be used to track the size of the substring
char* guScanLineRaw(char const* line, char** substringOut, int start, int end, int* sizeOut)
{
    //declare counter and size of string
	int i = 0;
	int size;

	//substring of line
	char* subLine;
	
	//find end of line
	int lineLength = guStringLength(line, GU_SINGLE_LINE, GU_NO_BUFFER_LIMIT);
    
	//clip start to 0
	if(start < 0)
		start = 0;
	//clip end to line length - 2, and allow negative end to mean the end of the line
	if(end < 0 || end > lineLength-2)
		end = lineLength-2;

	//if the substring starts from an index greater than or equal to the ending index, return an empty string
	if(start > end)
	{
		if(sizeOut != NULL)
			*sizeOut = 1;
		return guEmptyString(&subLine);
	}
    
    //calculate a size large enough to hold all substring characters and '\0'
	size = end-start+2;
    
    //if a buffer was given to hold the substring, address it
    if(substringOut && *substringOut)
        subLine = *substringOut;
    //otherwise allocate space for a new buffer
    else
        subLine = (char*)malloc(sizeof(char) * (size));
    
    //copy characters to substring
	for(i = start; i <= end; i++)
		subLine[i-start] = line[i];
    
    //add termination character at the end of the substring
	subLine[end-start+1] = '\0';							

    //if an address was given to store the substring size, store the size
	if(sizeOut != NULL)
		*sizeOut = size;
    
	return subLine;
}

//scan line for one element between delimiters that have not yet been located
char* guScanLineWithDelimiter(char const* line, char** substringOut, char delimiter, int delimiterIndex)
{
	//find delimiters in line
	int delimiterCount = 0;
	int* delimiterIndices = guFindDelimiters(line, delimiter, &delimiterCount, GU_DEFAULT_BUFFER_SIZE);

	//call scanLine that uses delimiter data to generate output string
	return guScanLine(line, substringOut, delimiterIndices, delimiterCount, delimiterIndex);
}

//scan line for one element between delimiters
char* guScanLine(char const* line, char** substringOut, int* delimiterIndices, int delimiterCount, int delimiterIndex)
{
    //declare counter, chunk, size of chuck, and the index within the line to be used
	int i = 0;
	char* chunk;
    int chunkSize;
	int lineIndex;

	//ensure that delimiter index is within bounds
	if(delimiterIndex < 0)							//set negative to zero
		delimiterIndex = 0;
	else if(delimiterIndex >= delimiterCount-1)		//return NULL if the index at or beyond the end of the line
		return NULL;

    //calculate size of chunk between delimiters
	chunkSize = delimiterIndices[delimiterIndex+1] - delimiterIndices[delimiterIndex];
    
    //if a buffer was given to hold the chunk, address it
    if(substringOut && *substringOut)
        chunk = *substringOut;
    //otherwise allocate space for a new buffer
    else
        chunk = (char*)malloc(sizeof(char) * (chunkSize));

	//retrieve characters
	lineIndex = delimiterIndices[delimiterIndex];
	for(i = 1; i < chunkSize; i++)
		chunk[i-1] = line[lineIndex + i];
	chunk[chunkSize-1] = '\0';			//make sure c-string terminates

	return chunk;
}

//return an empty string
char* guEmptyString(char** toEmpty)
{
	//set up empty string with only a terminating character
	char* outEmpty = (char*)malloc(sizeof(char)*1);
	outEmpty[0] = '\0';
	//free argument string and set readdress it, if the address to the string address is not NULL
	if(toEmpty != NULL)
	{
		if(*toEmpty != NULL)
			free(toEmpty);
		*toEmpty = outEmpty;
	}
	return *toEmpty;
}

//return character length of string, limit to maxSize
int guStringLength(char const* cString, int singleOrMultipleLines, int maxSize)
{
	int length;

	if(cString == NULL)
		return 0;
	//loop through string until end of string allowing for one character beyond max for termination character
	//end at '\n' o r '\r' if single line is being read
	for(length = 0; (maxSize < 0 || length < maxSize+1) && cString[length] != '\0' && (singleOrMultipleLines == GU_SINGLE_LINE ? cString[length] != '\n' && cString[length] != '\r' : 1); length++);	
	//return 0 if the string is not terminated properly
	if(maxSize >= 0 && length > maxSize)	
		return 0;
	return length+1;
}

//find the first index of the given character starting at the start index
int guIndexOf(char const* searchString, char goalCharacter, int startIndex, int maxDistanceFromStart)
{
    //declare counter and define max index
    int i;
    int maxIndex = startIndex + maxDistanceFromStart;
    for(i = startIndex; (maxDistanceFromStart < 0 || i < maxIndex) && searchString[i] != '\0'; i++)
        if(searchString[i] == goalCharacter)
            return i;
    return -1;
}

//compare two c-strings using the ASCII values of their characters, if they are equal then return zero, if the first is greater then return 1, if the first is less return -1
//the case sensitive flag allows uppercase and lowercase to be treated as either equal or not
//note that passing in c-strings that are not terminated by '\0' is dangerous, for this reason the function will terminate early if no such termination is found before the maximum size is reached
int guCompare(char const* cString1, char const* cString2, int caseSensitive_bool, int maxSize)
{
    //declare counter
	int i = 0;
	
	//--- Find the terminating indices of both strings ---//
	int term1 = -1, term2 = -1;	//terminating index of cString1 and cString2, respectively
	maxSize += 1;				//allow maxSize to specify how many characters appear before the termination character
	for(i = 0; (maxSize < 1 || i < maxSize) && (term1 < 0 || term2 < 0); i++)
	{
		if(term1 < 0)
			if(cString1[i] == '\0')
				term1 = i;
		if(term2 < 0)
			if(cString2[i] == '\0')
				term2 = i;
	}
	//return early if either string has no termination
	if(term1 < 0 || term2 < 0)
		return GU_NO_TERMINATION;

	//--- Compare Strings ---//
	for(i = 0; i < term1 && i < term2; i++)
	{
		char c1 = cString1[i], c2 = cString2[i];	//current characters being compared
		//cast both characters as uppercase if not case sensitive
		if(!caseSensitive_bool)
		{
			if(c1 >= 97 && c1 <= 122)
				c1 -= 32;
			if(c2 >= 97 && c2 <= 122)
				c2 -= 32;
		}
		//return if either character is greater
		if(c1 > c2)
			return GU_FIRST_GREATER;
		else if(c1 < c2)
			return GU_SECOND_GREATER;
	}
	//loop terminated becuase it reached the end of at least one of the string
	//to reach this point the strings must be equal so far, thus the comparison will be determined by which string has fewer characters
	if(term1 > term2)
		return GU_FIRST_GREATER;
	else if(term1 < term2)
		return GU_SECOND_GREATER;
	else
		return GU_EQUAL;
}

//concatenate two strings
char* guConcat(char const* cString1, char const* cString2, char** concatOut, int* resultSize, int inputMaxSize)
{
    //declare counter
	int i = 0;

	//set up tracking varibles
	char* result;
	int size = 0;

	//if the first string is NULL, don't count it
	if(cString1 == NULL)
	{
		//if both strings are NULL, return empty string 
		if(cString2 == NULL)
			return guEmptyString(&result);
		//only count second string
		size = guStringLength(cString2, GU_SINGLE_LINE, inputMaxSize);
		result = (concatOut && *concatOut) ? *concatOut : (char*)malloc(sizeof(char)*size);
		for(i = 0; i < size; i++)
			result[i] = cString2[i];

		//add termination character
		result[size-1] = '\0';
	}
	//if the second string is NULL, don't count it
	else if(cString2 == NULL)
	{
		//only count cString2
		size = guStringLength(cString1, GU_SINGLE_LINE, inputMaxSize);
		result = (concatOut && *concatOut) ? *concatOut : (char*)malloc(sizeof(char)*size);
		for(i = 0; i < size; i++)
			result[i] = cString1[i];

		//add termination character
		result[size-1] = '\0';										
	}
	//count both strings
	else
	{
		int string1Size;
		size = guStringLength(cString1, GU_SINGLE_LINE, GU_DEFAULT_BUFFER_SIZE);
		string1Size = size;
        //add an extra count to size if the first string's size was zero (done to handle the next line's removal of a character)
		size = (size > 0 ? size : 1) + guStringLength(cString2, GU_SINGLE_LINE, inputMaxSize);				
        //remove the space for first terminating character
        result = (concatOut && *concatOut) ? *concatOut : (char*)malloc(sizeof(char)*size-1);                    
		for(i = 0; i < size-1; i++)
			if(i < string1Size-1)									//add characters from first string until no non-terminating characters remain
				result[i] = cString1[i];
			else													
				result[i] = cString2[i-(string1Size-1)];			//add characters from second string until no non-terminating characters remain
		
		//add termination character
		result[size-2] = '\0';
	}
	
	//if the resulting size was requested, store it
	if(resultSize != NULL)
		*resultSize = size;
	
	return result;
}

//create a copy of the input string
//char* guCopyString(char const* original, char** duplicateOut, const int maxSize)
char* guCopyString(char const* original, char** duplicateOut, const int maxSize)
{
	int i = 0;
	int size = guStringLength(original, 0, maxSize);
	char* duplicate = (duplicateOut && *duplicateOut) ? *duplicateOut : (char*)malloc(sizeof(char)*size);
	for(i = 0; i < size; i++)
		duplicate[i] = original[i];
	return duplicate;
}

//find the where the specified delimiter appears in the input string
int* guFindDelimiters(char const* inputString, char delimiter, int* delimiterCountToFind, int maxCharacters)
{
    //declare counter, indices of delimeters array, and place of delimter to be used
	int i = 0;
	int* delimiterIndices;
	int delimiterPlace;
	
	//determine length of line and count delimiters, stop counting at either a new line or termination character
	int size;
	int delimiterCount = 0;
	for(size = 0; (maxCharacters < 0 || size < maxCharacters+1) && inputString[size] != '\n' && inputString[size] != '\r' && inputString[size] != '\0'; size++)
		if(inputString[size] == delimiter)
			delimiterCount++;

	//allocate memory for delimter array, add two for beginning and end of array
	delimiterCount += 2;
	delimiterIndices = (int*)malloc(sizeof(int)*(delimiterCount));
	delimiterIndices[0] = -1;
	delimiterIndices[delimiterCount-1] = size;

	//find delimiter throughout string
	delimiterPlace = 1;
	for(i = 0; i < size; i++)
	{
		if(inputString[i] == delimiter)
		{
			delimiterIndices[delimiterPlace] = i;
			delimiterPlace++;
		}
	}

	//set delimiterCount to number of delimiters if not passed in as NULL
	*delimiterCountToFind = delimiterCount;

	return delimiterIndices;
}

//parse a string into a decimal value, return 1 if successful or 0 if not
//numerical base can be specified using the NumBases enum and will only work with these predefined bases
//this will not check if the returned number exceeds the maximum float value
int guCheckNumerical(char* inputString, int* sizeOut, int* radixPoint, int* xPoint, unsigned int numericalBaseIn, int maxDigits)
{
	int i = 0;                                                                  //counter
	int size = 0;                                                               //size of string being parsed
	int radixValid = 1;                                                         //only allow onee '.'
	int radixIndex = -1;                                                        //where does '.' appear if anywhere
	int xValid = numericalBaseIn == GU_BASE_HEXADECIMAL && xPoint != NULL;		//allow one 'x' if in a hexidecimal base and there is a place to store its location
	int xIndex = -1;                                                            //where does 'x' appear if anywhere
	
	//fail if input is empty
	if(inputString == NULL)
		return 0;
	//fail if no location was given for storing radix point
	if(radixPoint == NULL)
		return 0;

	//fail if numerical base is does not fit predefined systems
	if(numericalBaseIn != GU_BASE_BINARY && numericalBaseIn != GU_BASE_OCTAL && numericalBaseIn != GU_BASE_DECIMAL && numericalBaseIn != GU_BASE_HEXADECIMAL)
		return 0;
	
	//--- Step through string once to find any invalid characters and determine placement of special characters ---//
	for(i = 0; inputString[i] != '\0' && i < maxDigits + 5; i++)			//add 3 to max digits to account for  '-' "0x" "." '\0'
	{
		//track size
		size++;

		//x will not be valid beyond the third character in hexadecimal
		if(i > 2 && xValid)
			xValid = 0;

		//track places of '.' and 'x' in valid string
		if(radixValid && inputString[i] == '.')
			radixIndex = i;
		if(xValid && inputString[i] == 'x')
			xIndex = i;
		
		//check binary characters
		if(numericalBaseIn < GU_BASE_BINARY || (inputString[i] != '0' && inputString[i] != '1'))
		{
			//check octal characters
			if(numericalBaseIn < GU_BASE_OCTAL || (inputString[i] != '2' && inputString[i] != '3' && inputString[i] != '4' && inputString[i] != '5' && inputString[i] != '6' && inputString[i] != '7'))
			{
				//check decimal characters
				if(numericalBaseIn < GU_BASE_DECIMAL || (inputString[i] != '8' && inputString[i] != '9'))
				{
					//check hexadecimal characters
					if(numericalBaseIn < GU_BASE_HEXADECIMAL || (inputString[i] != 'A' && inputString[i] != 'a' && inputString[i] != 'B' && inputString[i] != 'b' && inputString[i] != 'C' && inputString[i] != 'c' && inputString[i] != 'D' && inputString[i] != 'd' && inputString[i] != 'E' && inputString[i] != 'e' && inputString[i] != 'F' && inputString[i] != 'f'))
					{
						//check if character is the leading sign
						if(i > 0 || !(inputString[i] == '+' || inputString[i] == '-'))
						{
							//check if character is the first '.'
							if(!radixValid || inputString[i] != '.')
							{
								//check if character is the first 'x' in a hexadecimal base (hexadecimal base is checked when xValid is first defined)
								if(!xValid || inputString[i] != 'x')
								{
									//fail due to invalid character
									return 0;
	}	}	}	}	}	}	}	}	//end of validation loop

	//set size, radix and x positions for output
	if(sizeOut != NULL)
		*sizeOut = size;
	if(radixPoint != NULL)
		*radixPoint = radixIndex;
	if(xPoint != NULL)
		*xPoint = xIndex;

	return 1;
}

//prepare string to be parsed into number
//this function is the same for all types of numbers and is used to stream line the addition of new number types
int guPrepareStringToNumber(char* inputString, unsigned int numericalBaseIn, int maxDigits, int* startIndexOut, int* signOut, int* sizeOut, int* radixIndexOut, int* xIndexOut)
{	
	int size = 0;			//character count of number (may not all be digits)
	int radixIndex = -1;	//where does '.' appear if anywhere
	int xIndex = -1;		//where does 'x' appear if anywhere
	int startIndex = 0;		//where to start parsing
	int sign = 1;			//sign multiplier
	
	//check if string is numerical and find the location of the '.' and 'x' if they exist, fail if string is not numerical
	if(!guCheckNumerical(inputString, &size, &radixIndex, &xIndex, numericalBaseIn, maxDigits))
		return 0;

	//determine sign and starting location of digits
	if(inputString [0] == '-')
	{
		sign = -1;
		startIndex = 1;
	}
	else if(inputString [0] == '+')
		startIndex = 1;

	//read value after possible 'x'
	if(xIndex >= 0)
		startIndex = xIndex;

	//pretend radix point is at the end if it does not actually appear
	if(radixIndex < 0)
		radixIndex = size;

	//set outputs
	if(startIndexOut != NULL)
		*startIndexOut = startIndex;
	if(signOut != NULL)
		*signOut = sign;
	if(sizeOut != NULL)
		*sizeOut = size;
	if(radixIndexOut != NULL)
		*radixIndexOut = radixIndex;
	if(xIndexOut != NULL)
		*xIndexOut = xIndex;

	return 1;
}

//--- Number Parsers ---//

//parse a c-string into an int if possible
//return 1 if possible and 0 if not
int guParseInt(char* inputString, int* outputNum, unsigned int numericalBaseIn, int maxDigits)
{
	//establish varibles needed for parsing
	int i = 0;              //counter
	int size = 0;			//character count of number (may not all be digits)
	int radixIndex = -1;	//where does '.' appear if anywhere
	int xIndex = -1;		//where does 'x' appear if anywhere
	int startIndex = 0;		//where actual digits start
	int sign = 1;			//sign of number + or -
	int result = 0;			//resulting number to be returned
	
	//return failure if no location was provided to store outputNum
	if(outputNum == NULL)
		return 0;
	
	//use zero is an initial value to avoid errors
	*outputNum = 0;

	//read through string and find important locations to streamline parsing
	if(!guPrepareStringToNumber(inputString, numericalBaseIn, maxDigits, &startIndex, &sign, &size, &radixIndex, &xIndex))
		return 0;

	//add numerical values based on placement in the string
	for(i = startIndex; i < size; i++)
	{
		if(i < radixIndex)
		{
			//standard decimal characters
			if(inputString[i] >= '0' && inputString[i] <= '9')
				result += (inputString[i] - '0') * (int)guPowUInt(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (uppercase)
			if(inputString[i] >= 'A' && inputString[i] <= 'F')
				result += (inputString[i] - 'A' + 10) * (int)guPowUInt(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (lowercase)
			if(inputString[i] >= 'a' && inputString[i] <= 'f')
				result += (inputString[i] - 'a' + 10) * (int)guPowUInt(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));
		}
	}

	//account for sign and return a success
	*outputNum = result * sign;
	return 1;
}

//parse a c-string into an unsigned int if possible
//return 1 if possible and 0 if not
int guParseUInt(char* inputString, unsigned int* outputNum, unsigned int numericalBaseIn, int maxDigits)
{
	//establish varibles needed for parsing
	int i = 0;                  //counter
	int size = 0;				//character count of number (may not all be digits)
	int radixIndex = -1;		//where does '.' appear if anywhere
	int xIndex = -1;			//where does 'x' appear if anywhere
	int startIndex = 0;			//where actual digits start
	int sign = 1;				//sign of number + or -
	unsigned int result = 0;	//resulting number to be returned
	
	//return failure if no location was provided to store outputNum
	if(outputNum == NULL)
		return 0;
	
	//use zero is an initial value to avoid errors
	*outputNum = 0;

	//read through string and find important locations to streamline parsing
	if(!guPrepareStringToNumber(inputString, numericalBaseIn, maxDigits, &startIndex, &sign, &size, &radixIndex, &xIndex))
		return 0;

	//add numerical values based on placement in the string
	for(i = startIndex; i < size; i++)
	{
		if(i < radixIndex)
		{
			//standard decimal characters
			if(inputString[i] >= '0' && inputString[i] <= '9')
				result += (inputString[i] - '0') * guPowUInt(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (uppercase)
			if(inputString[i] >= 'A' && inputString[i] <= 'F')
				result += (inputString[i] - 'A' + 10) * guPowUInt(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (lowercase)
			if(inputString[i] >= 'a' && inputString[i] <= 'f')
				result += (inputString[i] - 'a' + 10) * guPowUInt(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));
		}
	}

	//do not account for sign and return a success
	*outputNum = result;
	return 1;
}

//parse a c-string into a short if possible
//return 1 if possible and 0 if not
int guParseShort(char* inputString, short* outputNum, unsigned int numericalBaseIn, int maxDigits)
{
	//establish varibles needed for parsing
	int i = 0;              //coutner
	int size = 0;			//character count of number (may not all be digits)
	int radixIndex = -1;	//where does '.' appear if anywhere
	int xIndex = -1;		//where does 'x' appear if anywhere
	int startIndex = 0;		//where actual digits start
	int sign = 1;			//sign of number + or -
	short result = 0;		//resulting number to be returned
	
	//return failure if no location was provided to store outputNum
	if(outputNum == NULL)
		return 0;
	
	//use zero is an initial value to avoid errors
	*outputNum = 0;

	//read through string and find important locations to streamline parsing
	if(!guPrepareStringToNumber(inputString, numericalBaseIn, maxDigits, &startIndex, &sign, &size, &radixIndex, &xIndex))
		return 0;

	//add numerical values based on placement in the string
	for(i = startIndex; i < size; i++)
	{
		if(i < radixIndex)
		{
			//standard decimal characters
			if(inputString[i] >= '0' && inputString[i] <= '9')
				result += (inputString[i] - '0') * (short)guPowUInt(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (uppercase)
			if(inputString[i] >= 'A' && inputString[i] <= 'F')
				result += (inputString[i] - 'A' + 10) * (short)guPowUInt(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (lowercase)
			if(inputString[i] >= 'a' && inputString[i] <= 'f')
				result += (inputString[i] - 'a' + 10) * (short)guPowUInt(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));
		}
	}

	//account for sign and return a success
	*outputNum = result * sign;
	return 1;
}

//parse a c-string into an unsigned short if possible
//return 1 if possible and 0 if not
int guParseUShort(char* inputString, unsigned short* outputNum, unsigned int numericalBaseIn, int maxDigits)
{
	//establish varibles needed for parsing
	int i = 0;                  //counter
	int size = 0;				//character count of number (may not all be digits)
	int radixIndex = -1;		//where does '.' appear if anywhere
	int xIndex = -1;			//where does 'x' appear if anywhere
	int startIndex = 0;			//where actual digits start
	int sign = 1;				//sign of number + or -
	unsigned short result = 0;	//resulting number to be returned
	
	//return failure if no location was provided to store outputNum
	if(outputNum == NULL)
		return 0;
	
	//use zero is an initial value to avoid errors
	*outputNum = 0;

	//read through string and find important locations to streamline parsing
	if(!guPrepareStringToNumber(inputString, numericalBaseIn, maxDigits, &startIndex, &sign, &size, &radixIndex, &xIndex))
		return 0;

	//add numerical values based on placement in the string
	for(i = startIndex; i < size; i++)
	{
		if(i < radixIndex)
		{
			//standard decimal characters
			if(inputString[i] >= '0' && inputString[i] <= '9')
				result += (inputString[i] - '0') * (unsigned short)guPowUInt(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (uppercase)
			if(inputString[i] >= 'A' && inputString[i] <= 'F')
				result += (inputString[i] - 'A' + 10) * (unsigned short)guPowUInt(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (lowercase)
			if(inputString[i] >= 'a' && inputString[i] <= 'f')
				result += (inputString[i] - 'a' + 10) * (unsigned short)guPowUInt(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));
		}
	}

	//do not account for sign and return a success
	*outputNum = result;
	return 1;
}

//parse a c-string into a long if possible
//return 1 if possible and 0 if not
int guParseLong(char* inputString, long* outputNum, unsigned int numericalBaseIn, int maxDigits)
{
	//establish varibles needed for parsing
	int i = 0;              //counter
	int size = 0;			//character count of number (may not all be digits)
	int radixIndex = -1;	//where does '.' appear if anywhere
	int xIndex = -1;		//where does 'x' appear if anywhere
	int startIndex = 0;		//where actual digits start
	int sign = 1;			//sign of number + or -
	long result = 0;		//resulting number to be returned
	
	//return failure if no location was provided to store outputNum
	if(outputNum == NULL)
		return 0;
	
	//use zero is an initial value to avoid errors
	*outputNum = 0;

	//read through string and find important locations to streamline parsing
	if(!guPrepareStringToNumber(inputString, numericalBaseIn, maxDigits, &startIndex, &sign, &size, &radixIndex, &xIndex))
		return 0;

	//add numerical values based on placement in the string
	for(i = startIndex; i < size; i++)
	{
		if(i < radixIndex)
		{
			//standard decimal characters
			if(inputString[i] >= '0' && inputString[i] <= '9')
				result += (inputString[i] - '0') * (long)guPowULong(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (uppercase)
			if(inputString[i] >= 'A' && inputString[i] <= 'F')
				result += (inputString[i] - 'A' + 10) * (long)guPowULong(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (lowercase)
			if(inputString[i] >= 'a' && inputString[i] <= 'f')
				result += (inputString[i] - 'a' + 10) * (long)guPowULong(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));
		}
	}

	//account for sign and return a success
	*outputNum = result * sign;
	return 1;
}

//parse a c-string into an unsigned long if possible
//return 1 if possible and 0 if not
int guParseULong(char* inputString, unsigned long* outputNum, unsigned int numericalBaseIn, int maxDigits)
{
	//establish varibles needed for parsing
	int i = 0;                  //counter
	int size = 0;				//character count of number (may not all be digits)
	int radixIndex = -1;		//where does '.' appear if anywhere
	int xIndex = -1;			//where does 'x' appear if anywhere
	int startIndex = 0;			//where actual digits start
	int sign = 1;				//sign of number + or -
	unsigned long result = 0;	//resulting number to be returned
	
	//return failure if no location was provided to store outputNum
	if(outputNum == NULL)
		return 0;
	
	//use zero is an initial value to avoid errors
	*outputNum = 0;

	//read through string and find important locations to streamline parsing
	if(!guPrepareStringToNumber(inputString, numericalBaseIn, maxDigits, &startIndex, &sign, &size, &radixIndex, &xIndex))
		return 0;

	//add numerical values based on placement in the string
	for(i = startIndex; i < size; i++)
	{
		if(i < radixIndex)
		{
			//standard decimal characters
			if(inputString[i] >= '0' && inputString[i] <= '9')
				result += (inputString[i] - '0') * guPowULong(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (uppercase)
			if(inputString[i] >= 'A' && inputString[i] <= 'F')
				result += (inputString[i] - 'A' + 10) * guPowULong(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (lowercase)
			if(inputString[i] >= 'a' && inputString[i] <= 'f')
				result += (inputString[i] - 'a' + 10) * guPowULong(numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));
		}
	}

	//do not account for sign and return a success
	*outputNum = result;
	return 1;
}

//parse a c-string into a float if possible
//return 1 if possible and 0 if not
int guParseFloat(char* inputString, float* outputNum, unsigned int numericalBaseIn, int maxDigits)
{
	//establish varibles needed for parsing
	int i = 0;              //counter
	int size = 0;			//character count of number (may not all be digits)
	int radixIndex = -1;	//where does '.' appear if anywhere
	int xIndex = -1;		//where does 'x' appear if anywhere
	int startIndex = 0;		//where actual digits start
	int sign = 1;			//sign of number + or -
	float result = 0;		//resulting number to be returned
	
	//return failure if no location was provided to store outputNum
	if(outputNum == NULL)
		return 0;
	
	//use zero is an initial value to avoid errors
	*outputNum = 0;

	//read through string and find important locations to streamline parsing
	if(!guPrepareStringToNumber(inputString, numericalBaseIn, maxDigits, &startIndex, &sign, &size, &radixIndex, &xIndex))
		return 0;

	//add numerical values based on placement in the string
	for(i = startIndex; i < size; i++)
	{
		if(i != radixIndex)
		{
			//standard decimal characters
			if(inputString[i] >= '0' && inputString[i] <= '9')
				result += (inputString[i] - '0') * (float)pow((double)numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (uppercase)
			if(inputString[i] >= 'A' && inputString[i] <= 'F')
				result += (inputString[i] - 'A' + 10) * (float)pow((double)numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (lowercase)
			if(inputString[i] >= 'a' && inputString[i] <= 'f')
				result += (inputString[i] - 'a' + 10) * (float)pow((double)numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));
		}
	}

	//account for sign and return a success
	*outputNum = result * sign;
	return 1;
}

//parse a c-string into a double if possible
//return 1 if possible and 0 if not
int guParseDouble(char* inputString, double* outputNum, unsigned int numericalBaseIn, int maxDigits)
{
	//establish varibles needed for parsing
	int i = 0;              //counter
	int size = 0;			//character count of number (may not all be digits)
	int radixIndex = -1;	//where does '.' appear if anywhere
	int xIndex = -1;		//where does 'x' appear if anywhere
	int startIndex = 0;		//where actual digits start
	int sign = 1;			//sign of number + or -
	double result = 0;		//resulting number to be returned

	//return failure if no location was provided to store outputNum
	if(outputNum == NULL)
		return 0;
	
	//use zero is an initial value to avoid errors
	*outputNum = 0;

	//read through string and find important locations to streamline parsing
	if(!guPrepareStringToNumber(inputString, numericalBaseIn, maxDigits, &startIndex, &sign, &size, &radixIndex, &xIndex))
		return 0;

	//add numerical values based on placement in the string
	for(i = startIndex; i < size; i++)
	{
		if(i != radixIndex)
		{
			//standard decimal characters
			if(inputString[i] >= '0' && inputString[i] <= '9')
				result += (inputString[i] - '0') * pow((double)numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (uppercase)
			if(inputString[i] >= 'A' && inputString[i] <= 'F')
				result += (inputString[i] - 'A' + 10) * pow((double)numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));

			//additional hexadecimal characters (lowercase)
			if(inputString[i] >= 'a' && inputString[i] <= 'f')
				result += (inputString[i] - 'a' + 10) * pow((double)numericalBaseIn, radixIndex - i - (i < radixIndex ? 1 : 0));
		}
	}

	//account for sign and return a success
	*outputNum = result * sign;
	return 1;
}

//---System Dependent---//

#ifdef __COREFOUNDATION_COREFOUNDATION__
#ifdef GU_USING_BUNDLES
char* guWrapFilenameInBundle(CFStringRef filename, char** wrappedFilenameOut)
{
	//get application's bundle
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	
	//get filesystem URL of file
	CFURLRef fileURL = CFBundleCopyResourceURL(mainBundle, filename, NULL, NULL);
	
	//get file location in format that can be cast as c-string
	UInt8* bundledFilename = (wrappedFilenameOut && *wrappedFilenameOut) ? (UInt8*)(*wrappedFilenameOut) : (UInt8*)malloc(sizeof(char)*GU_DEFAULT_BUFFER_SIZE);
	CFURLGetFileSystemRepresentation(fileURL, true, bundledFilename, GU_DEFAULT_BUFFER_SIZE);
	
	//deallocate memory that is no longer needed
	CFRelease(fileURL);
	
	return (char*)bundledFilename;
}
#endif
#endif
