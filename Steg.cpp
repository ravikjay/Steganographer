#include "provided.h"
#include <string>
#include <iostream>
using namespace std;

bool Steg::hide(const string& hostIn, const string& msg, string& hostOut) 
{
    if (hostIn.empty())
        return false;
    hostOut = "";
    
    vector<string> subStringVec;
    string subString = "";
    for (int i = 0; i < hostIn.size(); i++) // Loop through the entire hostIn string
    {
        if (hostIn[i] != ' ' && hostIn[i] != '\t') // If there are any existing spaces or tabs, ignore them
        {
            if (hostIn[i] == '\n' || i == hostIn.size()-1) // If a newline is found or you're at the end of the string
            {
                subStringVec.push_back(subString); // Push the substring that we've been adding to our substring vector
                subString = "";
            }
            subString += hostIn[i]; // Add each desired character to the substring
        }
    }
    // Break up the hostIn string into its substrings
    
    vector<unsigned short> vec;
    Compressor::compress(msg, vec);
    string encodedResult = BinaryConverter::encode(vec); // The string is comprised of tabs and space
    
    unsigned long l = encodedResult.size();
    unsigned long n = subStringVec.size();
    unsigned long cap = l%n;
    int capCount = 0;
    vector<string> tabsAndSpacesVec;
    string subString2 = "";
    for (int i = 0; i < encodedResult.size(); i++) // Go through the string that has been successfully encoded
    {
        if (capCount < cap) // If our current capCount is less than the cap calculated by encodedResult's size and the subString vector's number of values
        {
            subString2 += encodedResult[i]; // Add each character from the encodedResult to the second subString being created
            if (subString2.size() == (l/n)+1) // Check if the substring's size is a specific value calculated
            {
                tabsAndSpacesVec.push_back(subString2); // If the size of the substring being created is of a certain value, add it to the vector
                subString2 = ""; // Reset the substring to empty
                capCount++; // Increase the number value of how many are being added to the tabsAndSpacesVector
                continue; // Enter the next iteration of the for loop (look at the next character)
            }
        }
        else
        {
            subString2 += encodedResult[i]; // Otherwise, add the character being examined to the current substring
            if (subString2.size() == (l/n)) // Check the substring's size
            {
                tabsAndSpacesVec.push_back(subString2); // Add it to the vector
                subString2 = ""; // Reset the substring to empty
                continue; // Enter the next iteration of the for loop (look at the next character)
            }
        }
    }
    // Break up the encodedResult into 9 substrings
    
    // Go through and add each tab and space to the vector in question
    for (int i = 0; i < subStringVec.size(); i++)
    {
        subStringVec[i] += tabsAndSpacesVec[i];
        hostOut += subStringVec[i]; // Add the substring in question to our output hostout string
    }
    
    return true;
}

bool Steg::reveal(const string& hostIn, string& msg) 
{
    if (hostIn.empty()) // Check if the string passed in is invalid
        return false;
    
    vector<string> subStringVec;
    string subString = "";
    for (int i = 0; i < hostIn.size(); i++) // Loop through the passed in string
    {
        if (hostIn[i] == '\n' || i == hostIn.size()-1) // If the current char is newline or we are looking at the last char in the passed in string
        {
            // If we're at the last character, make sure we still add it to the substring
            if(i == hostIn.size()-1){
                subString += hostIn[i];
            }
            subStringVec.push_back(subString); // Add the current substring to the vector
            subString = "";
            continue;
        }
        if (hostIn[i] == ' ' || hostIn[i] == '\t') // If the string passed in is a space or tab, add them to the substring
        {
            subString += hostIn[i];
        }
    }
    
    string spacesAndTabsString;
    for (int i = 0; i < subStringVec.size(); i++) // Loop through the vector of substrings
    {
        for (int j = 0; j < subStringVec[i].size(); j++) // In each substring, loop through each character
        {
            if (subStringVec[i][j] == ' ' || subStringVec[i][j] == '\t') // If the character in question is a space or tab
            {
                spacesAndTabsString += subStringVec[i][j]; // Add it to the string of counting spaces and tabs
            }
        }
    }
    vector<unsigned short> vec;
    string finalResult = "";
    
    // If the decode doesn't run properly, return reveal false. If not, it will decode our string
    if (!BinaryConverter::decode(spacesAndTabsString, vec)){
        return false;
    }
    
    // Decompress the decoded message
    if (!Compressor::decompress(vec, finalResult))
        return false;
    
    // Set our message equal to the finalResult
    msg = finalResult;
	return true;  
}












