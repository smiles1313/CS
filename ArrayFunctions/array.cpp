//
//  main.cpp
//  Project4
//
//  Created by Sophia Miles on 11/4/23.
//

#include <iostream>
#include <string>
#include <cassert>
using namespace std;


int reduplicate(string a[], int n)
{
    if (n < 0)  //returns error if n is negative
        return -1;
    
    for (int i = 0; i < n; i++) //does nothing if n is 0
    {
        a[i].append(a[i]);  //appends the string at index i of the array to the end of that same element
    }
    return n; //returns if n is not negative (including 0)
}


int locate(const string a[], int n, string target)
{
    if (n < 0)  //returns error if n is negative
        return -1;
    
    for (int i = 0; i < n; i++) //for all the n elements we are looking at in the array
    {
        if (a[i] == target)  //if string at index i equals target
            return i;        //returns the position/index of the string
    }
    
    return -1; //only does if no string in array matched target (and n >= 0)
}

int locationOfMax(const string a[], int n)
{
    if (n <= 0) //if n is negative OR if n is 0
        return -1;
    
    string maxS = a[0];
    int maxIndex = 0;
    
    for (int i = 1; i < n; i++)
    {
        if (a[i] > maxS) //if equal to maxS, then do not want to change maxS in order to keep smallest index
        {
            maxS = a[i];
            maxIndex = i;
        }
    }
    return maxIndex;
}

int circleLeft(string a[], int n, int pos)
{
    if (n <= 0) //if passed negative or no elements, gives error
        return -1;
    
    if (pos < 0 || pos > n - 1) //if pos is negative or greater than last known element, then it is invalid
        return -1;
    
    if (pos == (n - 1)) //if n is the last element in array, no change to array
        return pos;
    
    string temp = a[pos]; //sets temp to element at pos
    for (int i = pos; i < n; i++) //from index pos to last element
    {
        if (i != n - 1) //as long as i is not at the last element in array
        {
            a[i] = a[i+1]; //set element at i to the string after it (shifting down elements by 1)
        }
    }
    a[n-1] = temp; //then, set element at pos to last element
    return pos;
}

int enumerateRuns(const string a[], int n)
{
    if (n < 0) //if passed negative elements, gives error
        return -1;
    
    if (n == 0 || n == 1)
        return n; //returns 0 or 1, because if n is one of those nums then array essentially goes unchanged
        
    int count = 1; //counter for sequence of consecutive items - starts at 1 because previous is at an element in array
    string previous = a[0]; //array a at this point has 1 or more elements in it, so this initialization is ok
    for (int i = 1; i < n; i++) //loop starts at 2nd element
    {
        if (previous != a[i])
        {
            count++;
        }
        previous = a[i]; //updates previous to the current string for next loop comparison
    }
    return count;
}

int flip(string a[], int n)
{
    if (n < 0) //if passed negative elements, gives error
        return -1;
    
    for (int i = 0; i < (n/2); i++) //switches elements at a[i] and a[n-1], only do half the number of times as there are indexes
    {
        string temp = a[i];
        a[i] = a[n-1];
        a[n-1] = temp;
    }
    return n;
}

int locateDifference(const string a1[], int n1, const string a2[], int n2)
{
    if (n1 < 0 || n2 < 0)  //if passed negative elements in EITHER array, gives error
        return -1;

    int minSize = n1; //initialize minSize - used to ensure no out-of-bounds behavior in for loop
    if (n2 < n1)      //if n2 has less elements than n1, than reassign minSize
        minSize = n2;
    
    for (int i = 0; i < minSize; i++)
    {
        if (a1[i] != a2[i])
            return i;
    }
    
    return minSize; //returns 0 if n1 or n2 was 0, OR returns smaller element num (n1 or n2) if one array ends first, or no dif elements found
}


int subsequence(const string a1[], int n1, const string a2[], int n2)
{
    if (n1 >= 0 && n2 == 0) //if n2 is 0, then returns 0 as long as n1 has positive or no elements (invalid if negative)
       return 0;
    
    if (n1 <= 0 || n2 < 0 || n2 > n1) //if n2 > n1, then all elements of a2 can not appear in the elements we see in a1
        return -1;
    
    
    for (int i = 0; i < n1; i++)
    {
       if (a2[0] == a1[i] && (n1 -1 - i >= (n2 - 1)))
        {
                int count = 0; //used to count the num of conseq elements that match in a1 and a2
                for (int k = 0; k < n2; k++)
                {
                      if (a1[i + k] != a2[k])
                          break; //if subsequence not match anymore, breaks out of this inner for loop and looks through rest of a1
                      count++;
                }
                //cerr << count << endl;
            
                if (count == n2) //if the num of elements that matched conseq equals n2
                {
                    //cerr << i << endl;
                    return i;
                }
        }
    }
     return -1; //if did not find any valid subsequences when exit for loop/after looking through all elements of a1
}

int locateAny(const string a1[], int n1, const string a2[], int n2)
{
    if (n1 <= 0 || n2 <= 0) //if passed negative or no elements, gives error
        return -1;
    
    for (int i = 0; i < n1; i++) //checks at each index in a1
    {
        for (int k = 0; k < n2; k++) //for one element in a1, checks all the elements in a2 to see if matches it
        {
            if (a1[i] == a2[k])
            {
                //cerr << "smallest pos in a1 that matches a element in a2: " << i << endl;
                return i;
            }
        }
    }
    //cerr << "No matches." << endl;
    return -1; //returns this if go through all possible combinations and do not see any elements in a1 equal to a2
}

int divide(string a[], int n, string divider)
{
    if (n < 0) //if passed negative elements, gives error
        return -1;
    
    if (n == 0)
        return n;
    
    string temp; //temporarily holds the string that is being replaced at their index in the array
    int toReturn = n; //index to return initialized to n (if no element in array is >= divider).
    
    for (int i = 0; i < n/2 ; i++) //i < n/2 to avoid unneccessary repetitiveness when sorting array least to greatest
    {
        for (int k = 0; k < n - 1; k++) //k < n - 1 because checking at a[k+1], so do not want to try and access out of range
        {
            if (a[k] > a[k + 1])
            {
                temp = a[k]; //switches elements in a[k] and a[k + 1]
                a[k] = a[k + 1];
                a[k + 1] = temp;
            }
        }

    }
    
/*-------------------------
    //DEBUGGING PURPOSES
    for (int d = 0; d < n; d++)
    {
        cerr << "Element at " << d << " is: " << a[d] << endl;
    }
 --------------------------*/
    
    for (int p = 0; p < n; p++)
    {
        if (a[p] >= divider)
        {
            toReturn = p;
            break; //because want to return the first index where this condition is met
        }
    }
    return toReturn;

}



int main()
{
    string h[7] = { "nikki", "ron", "asa", "vivek", "", "chris", "donald" };
    assert(locate(h, 7, "chris") == 5);
    assert(locate(h, 7, "asa") == 2);
    assert(locate(h, 2, "asa") == -1);
    assert(locationOfMax(h, 7) == 3);

    
    string g[4] = { "nikki", "ron", "chris", "tim" };
    assert(locateDifference(h, 4, g, 4) == 2);
    assert(circleLeft(g, 4, 1) == 1 && g[1] == "chris" && g[3] == "ron");
    

    string c[4] = { "ma", "can", "tu", "do" };
    assert(reduplicate(c, 4) == 4 && c[0] == "mama" && c[3] == "dodo");

    
    string e[4] = { "asa", "vivek", "", "chris" };
    assert(subsequence(h, 7, e, 4) == 2);

    string d[5] = { "ron", "ron", "ron", "chris", "chris" };
    assert(enumerateRuns(d, 5) == 2);
        
    string f[3] = { "vivek", "asa", "tim" };
    assert(locateAny(h, 7, f, 3) == 2);
    assert(flip(f, 3) == 3 && f[0] == "tim" && f[2] == "vivek");
        
    assert(divide(h, 7, "donald") == 3);
    
        
    cerr << "All tests succeeded" << endl;
}
