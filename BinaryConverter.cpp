#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

const size_t BITS_PER_UNSIGNED_SHORT = 16;

string convertNumberToBitString(unsigned short number);
bool convertBitStringToNumber(string bitString, unsigned short& number);

string BinaryConverter::encode(const vector<unsigned short>& numbers)
{
    string encodedResult = "";
    string finalResult = "";
    // iterate through the vector of numbers we wish to encode
    for (vector<unsigned short>::const_iterator it = numbers.begin(); it < numbers.end(); it++)
    {
        // convert them to 0 or 1s for encoded binary numerations
        encodedResult += convertNumberToBitString(*it);
    }
    // now go through our entire binary string
    for (int i = 0; i < encodedResult.size(); i++)
    {
        // if the value is a 0, give it the value of space
        if (encodedResult[i] == '0')
            finalResult += ' ';
        else // otherwise set it to tab
            finalResult += '\t';
    }
    return finalResult;
}

bool BinaryConverter::decode(const string& bitString, vector<unsigned short>& numbers)
{
    // make sure that the string passed in is valid and is a multiple of 16
    if ((bitString.size() % 16) != 0){
        return false;
    }
    string s = "";
    // loop through the whole string
    for (int i = 0; i < bitString.size(); i++)
    {
        // if whats in the string is not a tab or space, disregard it
        if (bitString[i] != ' ' && bitString[i] != '\t')
            return false;
        else
        {
            // so now, knowing that its either a space or tab set it accordingly
            if (bitString[i] == ' ')
                s += '0';
            else
                s += '1';
        }
        // if the substrings being appended to are not empty and are of length (multiple of 16)
        if (i != 0 && s.length() % 16 == 0)
        {
            // convert it back and push the result back into numbers
            unsigned short temp = 0;
            convertBitStringToNumber(s, temp);
            numbers.push_back(temp);
            s = ""; // reset the string for future usage
        }
    }
    
	return true; 
}

string convertNumberToBitString(unsigned short number)
{
	string result(BITS_PER_UNSIGNED_SHORT, '0');
	for (size_t k = BITS_PER_UNSIGNED_SHORT; number != 0; number /= 2)
	{
		k--;
		if (number % 2 == 1)
			result[k] = '1';
	}
	return result;
}

bool convertBitStringToNumber(string bitString, unsigned short& number)
{
	if (bitString.size() != BITS_PER_UNSIGNED_SHORT)
		return false;
	number = 0;
	for (size_t k = 0; k < bitString.size(); k++)
	{
		number *= 2;
		if (bitString[k] == '1')
			number++;
		else if (bitString[k] != '0')
			return false;
	}
	return true;
}
