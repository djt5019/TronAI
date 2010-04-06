/*
@Name: Dan Tracy, Jerry Zimmer, Scott Dubnoff
@Prgm: parser.cpp
*/

#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include "parser.h"
#include "tron.cpp"
using namespace std;

#define RULEFILE "rulefile.txt"
#define KNOWLEDGEFILE "knowledge.txt"

map<string, bool> knowledgeBase;
map<int, struct rule> ruleBase;
static bool initalized = false;

//! Rules will follow the form of IF <expr1> AND/OR <expr2> ... AND/OR <exprN> THEN consequent
//! The expressions can either be pulled from the knowledgeBase or can be functions
//! For example: IF near AND notTrapped() THEN floodFill()
//! The functions in the rule base should return boolean.

void initalizeExpertSystem()
{
    //! Will read from the rule file, create a structure for each containing the consequent
    //! and the list of antecedents.
  
    if( !initalized )
    {	
      initRules();    
      initKnowledge();
      initalized = false;
    }
}

void initRules()
{    
    int index = 0;
    string line;
    string temp;
    ifstream ruleBaseInput;
    
    bool antecedentFlag = false;
    bool consequentFlag = false;
    
    struct rule tempRule;
    
    ruleBaseInput.open( RULEFILE );
    
    if( !ruleBaseInput.is_open() )
    {
	fprintf(stderr,"%s Error: The Rule File could not be opened\n", __func__);
	exit(EXIT_FAILURE);
    }
    
    while( !ruleBaseInput.eof() )
    {
	ruleBaseInput >> temp;
	line += temp + " ";
	
	if( temp.find("##") != string::npos)
	{
	    string junk;
	    getline( ruleBaseInput, junk );
	    line.clear();
	    junk.clear();
	    continue;
	}

	if( temp == "IF" || temp == "if" ) 
	{
	    antecedentFlag = true;
	    continue;
	}
	
	if( temp == "THEN" || temp == "then" ) 
	{ 
	    consequentFlag = true; 
	    antecedentFlag = false; 
	    continue;
	}
	
	if( antecedentFlag )
	    tempRule.antecedent.push_back(temp);
	
	if( consequentFlag )
	{
	    consequentFlag = false;
	    
	    ++index;
	    
	    temp.erase( temp.size() - 1);
	    tempRule.consequent = temp;
	    tempRule.name = line;
	    
	    if( tempRule.antecedent.size() % 2 != 1 && tempRule.antecedent.size() < 2)
	    {
		fprintf(stderr,"%s Warning: Rule %d may be malformed\n", __func__, index);
	    }
	    
	    ruleBase.insert( std::pair<int,struct rule>(index,tempRule));
	    
	    line.clear();
	    tempRule.antecedent.clear();
	}
	
    }
    ruleBaseInput.close();
}

void initKnowledge()
{
    string line;
    string name;
    ifstream knowledgeInput;
    
    knowledgeInput.open(KNOWLEDGEFILE);
    
    if( !knowledgeInput.is_open() )
    {
       fprintf(stderr, "%s Warning: Couldn't open the file %s, continuing anyway\n",__func__, KNOWLEDGEFILE );
       return;
    }
    
    
    while( getline(knowledgeInput, line) )
    {
	if( line == "" ) break;
	
	name = line.substr(0, line.find(" "));
	if( line.substr(line.find(" "), line.size() ) == "true" )
	  knowledgeBase[name] = true;
	else
	  knowledgeBase[name] = false;
    }
    knowledgeInput.close();
}


bool triggerRule(struct rule theRule)
{
    //! This will take a function then parse it and proceed to call the
    //! following functions or poll the knowledgeBase.  If something is 
    //! proven as a result of the parsing process then it will be added
    //! to the knowledgeBase.  If the rule returns true then the parse
    //! function will return true as well.  This will be called multiple
    //! times to find a good rule to apply to the game.
    
    struct rule rules;
    string tempString;
    bool expressionValue = false;
    
    for( map<string,bool>::iterator itr = knowledgeBase.begin(); itr!=knowledgeBase.end(); ++itr)
    {
       cout << itr->first << "=" << itr->second << endl;
    }
    
    if( theRule.antecedent.size() < 2 )
    {
	//There is only one expression
	tempString = theRule.antecedent[0];
	expressionValue = knowledgeBase.find( tempString )->second;
	cout << "Expression Value of: " << tempString << "=" << expressionValue << endl;
	return expressionValue;
    } 
    
    return expressionValue; 
}


void tryRules(int& me_x, int& me_y)
{
    //Pointer to one of the four move functions
    void (*function)(int&, int&) = NULL;
    
    map<int,struct rule>::iterator itr;
    
    for( itr=ruleBase.begin(); itr != ruleBase.end(); ++itr )
    {	  
	  // Iterate through each rule trying to see if it can be triggered
	  // if it can then observe the consequent action, set the function
	  // pointer to the relevent movement function then call the function
	  
	  if( triggerRule( itr->second ) )
	  {	      
	    
	      string functionName = itr->second.consequent;
	      cout << __func__ << ": Consequent is: " << functionName << endl;
	      if( functionName ==  "moveLeft()" )
		  function = moveLeft;
	      else if( functionName ==  "moveRight()" )
		  function = moveRight;
	      else if( functionName == "moveUp()" )
		  function = moveUp;
	      else
		  function = moveDown;
	      
	      function(me_x, me_y);
	      
	      break;
	  }
    }
}



bool checkSurroundings(const char board[MAX_Y][MAX_X], int x, int y)
{
    //! This will check around the players piece looking for a place to move
    if( board[y-1][x] == ' ' )
    {
	knowledgeBase["upIsOpen"] = true;
	return true;
    }
    else if( board[y][x-1] == ' ' )
    {
	knowledgeBase["leftIsOpen"] = true;
	return true;
    }
    else if( board[y][x+1] == ' ' )
    {
	knowledgeBase["rightIsOpen"] = true;
	return true;
    }
    else if( board[y+1][x] == ' ' )
    {
	knowledgeBase["downIsOpen"] = true;
	return true;
    }
    else
    {
      return false;
    }
}

void resetDirections()
{
    knowledgeBase["rightIsOpen"] = false;
    knowledgeBase["leftIsOpen"]  = false;
    knowledgeBase["downIsOpen"]  = false;
    knowledgeBase["upIsOpen"]    = false;
}

bool Player2 :: Move(const char board[MAX_Y][MAX_X], int& me_x, int& me_y, int them_x, int them_y)
{
    resetDirections();
    initalizeExpertSystem();
    
    if( checkSurroundings(board, me_x, me_y) )
    {
      tryRules(me_x,me_y);
      cin.get();
      return true;
    }
    cin.get();
    return false;
}