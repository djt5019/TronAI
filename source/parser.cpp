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
    bool state;
    ifstream knowledgeInput;
    
    knowledgeInput.open(KNOWLEDGEFILE);
    
    if( !knowledgeInput.is_open() )
    {
       fprintf(stderr, "%s Error: Couldn't open the file %s\n",__func__, KNOWLEDGEFILE );
       exit(EXIT_FAILURE);
    }
    
    
    while( getline(knowledgeInput, line) )
    {
	cout << line << endl;
	name = line.substr(0, line.find(" "));
	if( line.substr(line.find(" "), line.size() ) == "true" )
	  state = true;
	else
	  state = false;
	cout << name << "=" << state <<endl;
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
    
    if( theRule.antecedent.size() < 2 )
    {
	//There is only one expression
	tempString = theRule.antecedent[0];
	expressionValue = knowledgeBase.find( tempString )->second;
	
	return expressionValue;
    } 
    
    return expressionValue; 
}

void moveLeft(const char board[MAX_X][MAX_Y], int& x, int& y)
{
}

void moveRight(const char board[MAX_X][MAX_Y],int& x, int& y)
{
}

void moveUp(const char board[MAX_X][MAX_Y],int& x, int& y)
{
}

void moveDown(const char board[MAX_X][MAX_Y],int& x, int& y)
{
}

void tryRules()
{
    void (*function)(const char[MAX_X][MAX_Y], int&, int&) = NULL;
    map<int,struct rule>::iterator itr; //Rule iterator
    
    for( itr=ruleBase.begin(); itr != ruleBase.end(); ++itr )
    {
	  if( triggerRule( itr->second ) )
	  {
	      string functionName = itr->second.consequent;
	      
	      if( functionName ==  "moveLeft" )
		  function = moveLeft;
	      else if( functionName ==  "moveRight" )
		  function = moveRight;
	      else if( functionName == "moveUp" )
		  function = moveUp;
	      else
		  function = moveDown;
	  }
    }
}



void checkSurroundings(const char board[MAX_Y][MAX_X], int x, int y)
{
    //! This will check around the players piece looking for a place to move
    //! it will first try to move right then left.  If not it will default to 
    //! moving forward
    
    if( board[x][y+1] == ' ' )
    {
    }
    else if( board[x][y-1] == ' ' )
    {
    }
    else if( board[x+1][y] == ' ' )
    {
    }
    else if( board[x-1][y] == ' ' )
    {
    }
}

bool Player2 :: Move(const char board[MAX_Y][MAX_X], int& me_x, int& me_y, int them_x, int them_y)
{
    initalizeExpertSystem();
    checkSurroundings(board, me_x, me_y);
    
    return false;
}



