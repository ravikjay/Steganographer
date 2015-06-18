#include "provided.h"
#include "HashTable.h"
//#include "anotherHashTable.h"
//#include "substituteHashTable.h"
//#include "saveMyHashTable.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

// ComputeHash function
unsigned int computeHash (string x)
{
    int hash = 9;
    for (size_t i = 0; i < x.size(); i++)
    {
        hash = hash * 131 + x[i];
    }
    return hash;
}

unsigned int computeHash (unsigned short x){
    return x % 10;
}


void Compressor::compress(const string& s, vector<unsigned short>& numbers)
{
    // Make sure the vector we're working with is empty
    numbers.clear();
    unsigned int n = s.size();
    unsigned int myCapacity = 0;
    // Calculate the capacity as defined by the algorithm
    if ((n/2)+512 < 16384)
    {
        myCapacity = (n/2)+512;
    }
    else {
        myCapacity = 16384;
    }
    // Create a hash table based on the capacity we defined earlier
    HashTable<string, unsigned short> myHash ((myCapacity/0.5), myCapacity);
    
    // For the first 256 buckets, we will make their permanence true (can't be altered) and place in our characters
    for (int j = 0; j < 256; j++)
    {
        string str2(1, static_cast<char>(j)); // static_cast so that the char can be converted and inputted in as a string
        myHash.set(str2, j, true);
    }
    int nextFreeID = 256; // This is where we begin when we start adding new values later
    string runSoFar = "";
    vector<unsigned short> v;
    for (int i = 0; i < s.size(); i++) // Loop through each character in the string
    {
        string str(1, static_cast<char>(s[i])); // convert each char to a one value string
        string expandedRun = runSoFar + str; // append this string with our running total to create our appended total
        unsigned short val = 0;
        if (myHash.get(expandedRun, val)) // See if the value of expandedRun exists in our hash table
        {
            runSoFar = expandedRun; // If so, make our running total the appended total (current char added)
            continue;
        }
        myHash.get(runSoFar, val); // otherwise, see if the non-appended string exists and then push it
        unsigned short x = val;
        v.push_back(x);
        myHash.touch(runSoFar); // make this the most recently added
        runSoFar = ""; // reset our running total string
        myHash.get(str, val); // Look for a specific string
        unsigned short cv = val;
        v.push_back(cv);
        if (!myHash.isFull()) // if the hash table is not full, add this appended total
        {
            myHash.set(expandedRun, nextFreeID);
            nextFreeID++; // move our current location
        }
        else // get rid of the least recently added value to make room for this
        {
            string discardKey = "";
            unsigned short discardVal = 0;
            myHash.discard(discardKey, discardVal);
            myHash.set(expandedRun, discardVal);
        }
    }
    if (runSoFar != "") // if the current string question is not empty
    {
        unsigned short checkVal = 0;
        myHash.get(runSoFar, checkVal);
        unsigned short x = checkVal;
        v.push_back(x); // find the current string's value in the hash table and push it
    }
    v.push_back(myCapacity);
    numbers = v; // all our compressed numbers that represent string combinations are saved into a vector
}

bool Compressor::decompress(const vector<unsigned short>& numbers, string& s)
{
    //unsigned short myCapacity = *(numbers.size()-1);
    unsigned short myCapacity = numbers[numbers.size()-1];
    HashTable<unsigned short, string> myHash ((myCapacity/0.5), myCapacity); // Create another hash table with the specified capacity
    
    // loop through all the 256 values and set the characters
    for (int j = 0; j < 256; j++)
    {
        // static_cast so that each character can be used as a one value string
        string str(1, static_cast<char>(j));
        // set each char with a permanence of true
        myHash.set(j, str, true);
    }
    // start creating new values in the hash table at a position at 256
    int nextFreeID = 256;
    string runSoFar = "";
    string output = "";
    // loop through all of the numbers in the passed in vector
    for (int i = 0; i < numbers.size()-1; i++)
    {
        // If the value we're looking at in the numbers array has a value that already exists in the hash table
        if (numbers[i] <= 255)
        {
            // get that value thats been compressed and append it to the string being examined
            string tempString = "";
            myHash.get(numbers[i], tempString);
            output.append(tempString);
            // after being appended, if the string being examined is empty
            if (runSoFar == "")
            {
                // make the running total string equal to the recovered value
                runSoFar = tempString;
                continue; // Will this enter the next iteration of the for loop and not run the rest of the code??
            }
            
            // now create another running total string that will be appended to the expanded string
            string expandedRun = runSoFar + tempString;
            
            // as long as the hashtable is not full
            if (!myHash.isFull())
            {
                // add or update the value into the next free spot
                myHash.set(nextFreeID, expandedRun);
                nextFreeID++;
            }
            else // if its full, discard a key -- value and then set the expanded run
            {
                string discardKey = "";
                unsigned short discardVal = 0;
                myHash.discard(discardVal, discardKey);
                myHash.set(discardVal, expandedRun);
            }
            runSoFar = "";
            continue;
        }
        else // if this value we're examining does not already exist in the hash table
        {
            string tempString = "";
            // if the value of tempstring does not exist in the table at that bucket
            if (!myHash.get(numbers[i], tempString))
                return false;
            // make it the most recent in history
            myHash.touch(numbers[i]);
            // and add it to the final output
            output.append(tempString);
            runSoFar = tempString;
            continue;
        }
    }
    // finally, set the output equal to the value by const reference
    s = output;
	return true;
}
