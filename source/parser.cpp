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
using namespace std;

#define RULEFILE "rulefile.txt"
#define KNOWLEDGEFILE "knowledge.txt"

map<string, bool> knowledgeBase;
map<int, struct rule> ruleBase;
static bool initalized = false;

int* myX;
int* myY;
int themX;
int themY;

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
      initalized = true;
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
	    
	    if( temp.find(";") == string::npos )
	        printf("%s Warning: Rule %d is missing a semicolon delimiter\n", __func__, index);
	    else
		temp.erase( temp.size() - 1);
	    
	    tempRule.consequent = temp;
	    tempRule.name = line;
	    
	    if( tempRule.antecedent.size() % 2 != 1 && tempRule.antecedent.size() != 1)
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
    int lineNumber = 0;
    string line;
    string name;
    ifstream knowledgeInput;
    
    knowledgeInput.open(KNOWLEDGEFILE);
    
    if( !knowledgeInput.is_open() )
    {
       fprintf(stderr, "%s Warning: Couldn't open the file %s, continuing anyway\n",__func__, KNOWLEDGEFILE );
       return;
    }
    
    
    while( ++lineNumber, getline(knowledgeInput, line) )
    {
	if( line == "" ) continue;
	
	if( line.find("##") != string::npos )
	{
	    //Found a comment
	    line.clear();
	    continue;
	}
	
	name = line.substr(0, line.find(" "));
	
	if(name.find("()") != string::npos)
	{
	    fprintf(stderr, "%s Warning: Line %d: %s seems to be a function declaration, skipping line\n", __func__, lineNumber, name.c_str());
	    continue;
	}
	
	string temp = line.substr(line.find(" ")+1, line.size()); 
	
	if( temp == "true" || temp == "1" )
	    knowledgeBase[name] = true;
	else if( temp == "false" || temp == "0")
	    knowledgeBase[name] = false;
	else
	{
	    fprintf(stderr, "%s Warning: Line %d: Syntax error \"%s\". Values can only be boolean... Defaulting %s to false\n", __func__, lineNumber,temp.c_str(), name.c_str());
	    knowledgeBase[name] = false;
	}
	      
    }
    knowledgeInput.close();
}


bool callFunction(const char board[MAX_Y][MAX_X], string functionName )
{
    bool (*function)(const char board[MAX_Y][MAX_X]) = NULL;
    
    printf("%s: calling function \'%s\'\n",__func__, functionName.c_str());
    
    if( functionName == "checkSurroundings()" )
	function = checkSurroundings;
    else
    {
	printf("%s: the function \'%s\' doesn't seem to exist\n", __func__, functionName.c_str());
	return false;
    }
      
    return function(board);
}


bool triggerRule(const char board[MAX_Y][MAX_X], struct rule theRule)
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
    bool tempResult = false;
    
    map<string, bool>::iterator itr;
    
    if( theRule.antecedent.size() == 1 )
    {
	//There is only one expression
	tempString = theRule.antecedent[0];
	
	if( tempString.find("()") == string::npos && (knowledgeBase.find( tempString ) == knowledgeBase.end()) )
	{
	    printf("Deriving the expression \'%s\' ...\n", tempString.c_str());
	    deriveExpression(board, tempString );
	}
	
	expressionValue = knowledgeBase[tempString];
	return expressionValue;
    } 
    else
    {
	expressionValue = knowledgeBase[ theRule.antecedent[0] ];
	
	for(size_t i = 1; i < theRule.antecedent.size(); i+=2)
	{
	    string op = theRule.antecedent[i];
	    string ex = theRule.antecedent[i-1];       
	    
	    if( (itr=knowledgeBase.find(ex)) == knowledgeBase.end() )
		deriveExpression( board, ex );
	    

	    if( ex.find("()") != string::npos ) //Function call
		tempResult = callFunction( board, ex );
	    else
		tempResult = knowledgeBase[ex];
	    
	    if( op == "AND" || op == "and" )
	    {
		expressionValue = tempResult && expressionValue;
	    }
	    else if(op == "OR" || op == "or" )
	    {		
		expressionValue = tempResult || expressionValue;
	    }
	}
    }
    
    
    return expressionValue; 
}

void deriveExpression( const char board[MAX_Y][MAX_X], string expression )
{
    //! This function will iterate through the list of consequents in an effort 
    //! to determine if the expression is true or false
    
    map<int,struct rule>::iterator itr = ruleBase.begin();
    
    printf("  +--->");
    
    for( ; itr != ruleBase.end(); ++itr )
    {
	if( itr->second.consequent == expression )
	{
	    printf("\t%s was proven true\n", expression.c_str());
	    bool value = triggerRule( board, itr->second );
	    knowledgeBase[expression] = value;
	    return;
	}
    }
    
    printf("\t%s was unable to proven\n", expression.c_str());
     
    //The rule could not be derived so add it and default it to false
    knowledgeBase[expression] = false;
}

void tryRules(const char board[MAX_Y][MAX_X], int& myX, int& myY)
{
    //Pointer to one of the four move functions
    void (*function)(int&) = NULL;
    
    map<int,struct rule>::iterator itr;
    
    for( itr=ruleBase.begin(); itr != ruleBase.end(); ++itr )
    {	  
	  // Iterate through each rule trying to see if it can be triggered
	  // if it can then observe the consequent action, set the function
	  // pointer to the relevent movement function then call the function
	  // The consequents will only be movement functions or proving knowledge facts
	  
	  if( triggerRule( board, itr->second ) )
	  {	      	    
	      string functionName = itr->second.consequent;
	      
	      if( functionName.find("()") == string::npos )
	      {
		  printf("%s: %s is now set to true\n", __func__, functionName.c_str());
		  knowledgeBase[ functionName ] = true;
		  break;
	      }

	      printf("%s: Action take is \'%s\'\n", __func__, functionName.c_str());
     	      
	      if( functionName ==  "moveLeft()" )
	      {
		  function = moveLeft;
		  function(myX);
	      }
	      else if( functionName ==  "moveRight()" )
	      {
		  function = moveRight;
		  function(myX);
	      }
	      else if( functionName == "moveUp()" )
	      {
		  function = moveUp;
		  function(myY);
	      }
	      else if( functionName == "moveDown()" )
	      {
		  function = moveDown;
		  function(myY);
	      }
	      else
	      {
		if( callFunction(board,functionName) == false )
		{
		  // The function call didn't succeed
		  // When in doubt, go down.
		  function = moveDown;
		  function(myY);
		}
	      }
	     	      
	      break;
	  }
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
    
    myX    = &me_x;
    myY    = &me_y;
    themX  = them_x;
    themY  = them_y;
    
    if( checkSurroundings(board) )
    {
	tryRules(board, me_x, me_y);
	cin.get();
	return true;
    }
    
    cin.get();
    return false;
}


///////////////////////
// AI FUNCTIONS BELOW
///////////////////////

bool checkSurroundings( const char board[MAX_Y][MAX_X] )
{
    //! This will check around the players piece looking for a place to move
    
    int x = *myX;
    int y = *myY;
    
    if( board[y-1][x] != ' ' )
    {
	knowledgeBase["upIsOpen"] = false;
    }
    
    if( board[y][x-1] != ' ' )
    {
	knowledgeBase["leftIsOpen"] = false;
    }
    
    if( board[y][x+1] != ' ' )
    {
	knowledgeBase["rightIsOpen"] = false;
    }
    
    if( board[y+1][x] != ' ' )
    {
	knowledgeBase["downIsOpen"] = false;
    }  
    
    
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

int SquaresReachable(const char board[MAX_Y][MAX_X], int& me_x, int& me_y, _square squaresReachable[MAX_Y*MAX_X])
{
    int counter = 0;
    
    for (int i = 1; i < MAX_X-1; i++)
       for (int j = 1; j < MAX_Y-1; j++)
	  if (board[j][i] == ' ')
	  {
	      if(Distance(board, me_x, me_y, i, j)>0)
	      {
		squaresReachable[counter].x = i;
		squaresReachable[counter++].y = j;
	      }
	  }
	    
    return counter;
}

int Distance(const char board[MAX_Y][MAX_X],const int start_x,const int start_y,const int end_x,const int end_y)
{
    _square movesList[MAX_Y*MAX_X];
    return ShortestPath(board,start_x, start_y, end_x, end_y, movesList, 0);
}


int ShortestPath(const char board[MAX_Y][MAX_X],const int start_x,const int start_y,const int end_x,const int end_y, _square movesList[MAX_Y*MAX_X], int stackCount)
{
    _square oldSquare[MAX_Y*MAX_X];
    _square newPath[MAX_Y*MAX_X];
    _square bestPath[MAX_Y*MAX_X];
    _square moves[3];
    int newPathLength = 0;
    int shortestPathLength = 0;
    int numNewPaths = 0;
    bool repeat = false;


    for (int i = 0; i < stackCount; i++)
    {
	oldSquare[i].x = movesList[i].x;
	oldSquare[i].y = movesList[i].y;
    }


    //if end is reached, set path returned to previous square to the coordinates of current square and length of path to 1.
    if(start_x == end_x && start_y == end_y)
    {
	movesList[0].x = start_x;
	movesList[0].y = start_y;
	return 1;
    }
    
	for(int i = -1; i<2; i+=2)
	    for(int j = -1; j<2; j+=2)
		if( (i != 0 || j!=0) && (j == 0 || i == 0) && board[start_y + j][start_x + i] == ' ')
		{
		    for(int k = 0; k < stackCount; k++)
			if(oldSquare[k].x == start_x + i && oldSquare[k].y == start_y + j)
			    repeat = true;
			
		    if(!repeat)
		    {
			moves[numNewPaths].x = start_x + i;
			moves[numNewPaths++].y = start_y + j;
		    }
		    repeat = false; 
		}
		
    for(int i = 0; i < stackCount; i++)
    {
	newPath[i].x = oldSquare[i].x;
	newPath[i].y = oldSquare[i].y;
    }
    
    stackCount++;
    
    if(numNewPaths == 0)
	return -1;
    
    //for each entry in 'moves' calculate the shortest path from that square to the end square        
    for(int i = 0; i < numNewPaths; i++)
    {
	//Add starting square to the path given to the next square.
	newPath[stackCount].x = start_x;
	newPath[stackCount].y = start_y;
	newPathLength = ShortestPath(board, moves[i].x, moves[i].y, end_x, end_y, newPath, stackCount);
	
	//save the shortest path
	if ((newPathLength < shortestPathLength || shortestPathLength == 0) && newPathLength > 0)
	{
	    for(int j = 0; j < newPathLength;j++)
	    {
		bestPath[j].x = newPath[j].x;
		bestPath[j].y = newPath[j].y;
	    }
	    shortestPathLength = newPathLength;
	}
    }
    
    if(shortestPathLength == 0)
    {
	return -1;
    }
    
    //add starting square as the first entry in best path
    movesList[0].x = start_x;
    movesList[0].y = start_y;
    
    //add squares in bestPath to movesList sent back to previous square
    for(int i = 0; i < shortestPathLength; i++)
    {
	movesList[i+1].x = bestPath[i].x;
	movesList[i+1].y = bestPath[i].y;
    }
    
    return ++shortestPathLength;
}


bool FillPoly(const char board[MAX_Y][MAX_X], int& me_x, int& me_y)
{
    int numSquares;
    _square fillSquare[MAX_Y*MAX_X];
    numSquares = SquaresReachable(board, me_x, me_y, fillSquare);
    return true;
}
