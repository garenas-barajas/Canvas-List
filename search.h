#pragma once

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;


//cleans token by removing leading and trailing punctuation, and converts to lowercase
string cleanToken(string s) {
    //remove punctuation at the begining of string
    size_t start = 0;
    //while loop will iterate through string until it finds first char
    while(start < s.size() && ispunct(s[start])){
        start++;
    }

    //remove trailing punctuation
    int end = s.size() - 1;
    //iterate backwords through string until last char
    while(end >= 0 && ispunct(s[end])){
        end--;
    }

    //check if string has characters
    bool hasChar = false;
    for(int i = start; i <= end; ++i){
        if(isalpha(s[i])){
            //if string has char, continue cleaning token
            hasChar = true;
            break;
        }
    }

    //if no chars in string, return empty string
    if(!hasChar){
        return "";
    }

    //compile cleaned token as lowercase
    string cleanedToken;
    for(int i = start; i <= end; ++i){
        cleanedToken += tolower(s[i]);
    }

    //return cleaned token
    return cleanedToken;
}

//returns set of cleaned tokens
set<string> gatherTokens(string text) {
    set<string> gatheredTokens;
    string token;
    //bool ensures that token will not start with white space
    bool isChar = false;

    //iterate through each character in text
    for(char& c : text){
        //if c is not space, add to token
        if(!isspace(c)){
            token += tolower(c);
            isChar = true;
        }else{
            if(isChar){
                //if c is a space, add token to set
                string cleanedToken = cleanToken(token);
                if(!cleanedToken.empty()){
                    gatheredTokens.insert(cleanedToken);
                }
                token.clear();
                //ready for next token
                isChar = false;
            }
        }
    }

    //add last token if it is not empty
    if(isChar){
        string cleanedToken = cleanToken(token);
        if(!cleanedToken.empty()){
            gatheredTokens.insert(cleanedToken);
        }
    }
    
    //returns set of cleaned tokens
    return gatheredTokens;
}

//builds inverted index from data file.
int buildIndex(string filename, map<string, set<string>>& index) {
    //open file
    ifstream file(filename);
    //make sure file is open
    if(!file.is_open()){
        return 0;
    }
    
    //read from file, extract url and text seperately
    string line;
    int pagesProcessed = 0;
    while(getline(file, line)){
        //first line is url
        string url = line;

        //read next line, make sure it's not empty
        if(!getline(file, line)){
            break;
        }

        //second line is body
        string body = line;
        //clean and gather tokens in body
        set<string> cleanedText = gatherTokens(body);
        //add url to matching token in index
        for(const auto& token : cleanedText){
            index[token].insert(url);
        }

        //increase pages processed
        pagesProcessed++;
    }
    //return pagesProcessed, index returned via reference
    return pagesProcessed;
}

//searches through index using query and returns results
set<string> findQueryMatches(map<string, set<string>>& index, string sentence) {
    //variable keeps track of boolean operation, defualt to union
    char operation = 'u';

    //variables for tokenize sentence loop
    stringstream ss(sentence);
    string term;
    set<string> results;

    //loop will tokenize sentence
    while(ss >> term){
        //check if the term has a modifier
        if(term[0] == '+' || term[0] == '-'){
            //sets operation
            operation = term[0];
            //removes modifier
            term.erase(term.begin());
        }

        //clean term
        string cleanedTerm = cleanToken(term);

        //set will store url's matching the term
        set<string> termUrls = index[cleanedTerm];

        //if else if statments depending on operation
        if(operation == 'u'){
            //union
            results.insert(termUrls.begin(), termUrls.end());
        }else if(operation == '+'){
            //intersection operation
            if(index.count(cleanedTerm) == 0){
                termUrls.clear();
            }
            set<string> intersection;
            //search for intersecting terms and adds to results
            set_intersection(results.begin(), results.end(), termUrls.begin(), termUrls.end(), inserter(intersection, intersection.begin()));
            results = intersection;
        }else if(operation == '-'){
            //difference operation
            set<string> difference;
            //search for difference in terms and adds to results
            set_difference(results.begin(), results.end(), termUrls.begin(), termUrls.end(), inserter(difference, difference.begin()));
            results = difference;
        }
        //reset variable for next iteration
        operation = 'u';
    }
    return results;
}

//runs main command loop for program
void searchEngine(string filename) {
    //build index from file
    map<string, set<string>> index;

    //set number of web pages
    int numUrls = buildIndex(filename, index);

    //display error if no urls in index
    if(numUrls == 0){
        cout << "Invalid filename." << endl;
    }

    //display number of urls index size
    cout << "Stand by while building index..." << endl;
    cout << "Indexed " << numUrls << " pages containing " << index.size() << " unique terms" << endl << endl;

    string query;
    bool continueSearch = true;

    //while loop will continue to iterate until user enters an empty term
    while(continueSearch){
        cout << "Enter query sentence (press enter to quit): ";
        getline(cin, query);

        if(query.empty()){
            continueSearch = false;
            cout << "Thank you for searching!" << endl;
        }else{
            //call findQueryMatches
            set<string> results = findQueryMatches(index, query);
            cout << "Found " << results.size() << " matching pages" << endl;

            //display pages
            for(const auto& page : results){
                cout << page << endl;
            }
            cout << endl;
        }
    }

}
