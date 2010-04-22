/*
@Name: Dan Tracy, Jerry Zimmer, Scott Dubnoff
@Prgm: parser.cpp
*/

#ifndef DJT5019_H
#define DJT5019_H
#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <queue>
#include <map>
#include <climits>
using namespace std;

#define RULEFILE "files/djt5019.dat1"
#define KNOWLEDGEFILE "files/djt5019.dat2"
#define MAXDEPTH 10

struct rule
{
    string name;
    vector<string> antecedent;
    string consequent;
};

struct vertex_info
{
    int x, y; 
    int dist;    
    bool visited;
};

class djt5019{
  
  public:	
    int* myX;
    int* myY;
    int themX;
    int themY;
    int __debug__;
 
    bool debug() { if(__debug__) return true; else return false; }
    bool Move(const char board[MAX_Y][MAX_X], int& me_x, int& me_y, int them_x, int them_y);
   
    void Initialize(struct _playerData* data);
    
    void initRules();
    void initKnowledge();
    void deriveExpression( const char board[MAX_Y][MAX_X], string expression );
    void tryRules(const char board[MAX_Y][MAX_X], int& myX, int& myY);
    bool triggerRule(const char board[MAX_Y][MAX_X], struct rule theRule);
    bool callFunction(const char board[MAX_Y][MAX_X], string functionName );

    //! Movement Functions
    void moveLeft( int& x){ --x;}
    void moveRight(int& x){ ++x;}
    void moveUp(int& y){ --y;}
    void moveDown(int& y){ ++y;}
    void resetDirections();
    void fill(int& myX, int& myY);

    //! Decision Functions
    void lookAhead();
    bool checkSurroundings( const char board[MAX_Y][MAX_X] );
    bool shortestPath(const char board[MAX_Y][MAX_X]);
    bool enemyAbove();
    bool enemyBelow();
    bool enemyRight();
    bool enemyLeft();
    bool enemyDiagonalUpLeft();
    bool enemyDiagonalUpRight();
    bool enemyDiagonalDownRight();
    bool enemyDiagonalDownLeft();

  private:
    map<string, bool> knowledgeBase;
    map<int, struct rule> ruleBase;
    bool moved;
    int zones_of_control[MAX_Y][MAX_X];
    
    bool compare_shortest_paths(const char G[MAX_Y][MAX_X], int p1x, int p1y, int p2x, int p2y);
    void initializeGraph(vertex_info G[MAX_Y][MAX_X], int sx, int sy);
};


//! Rules will follow the form of IF <expr1> AND/OR <expr2> ... AND/OR <exprN> THEN consequent
//! The expressions can either be pulled from the knowledgeBase or can be functions
//! For example: IF near AND notTrapped() THEN floodFill()
//! The functions in the rule base should return boolean.

void djt5019 :: Initialize(struct _playerData* data)
{
    //! Will read from the rule file, create a structure for each containing the consequent
    //! and the list of antecedents.
      printf("Initalizing the Game as Player %d\n", data->playerNum);
      #if DEBUG
      __debug__ = 1;
      #else
      __debug__ = 0;
      #endif
      
      *myX = data->x;
      *myY = data->y;
      
      initRules();    
      initKnowledge();
}

void djt5019 :: initRules()
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
	fprintf(stderr,"initRules Error: The Rule File could not be opened\n");
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
	        printf("initRules Warning: Rule %d is missing a semicolon delimiter\n", index);
	    else
		temp.erase( temp.size() - 1);
	    
	    tempRule.consequent = temp;
	    tempRule.name = line;
	    
	    if( tempRule.antecedent.size() % 2 != 1 && tempRule.antecedent.size() != 1)
	    {
			fprintf(stderr,"initRules Warning: Rule %d may be malformed\n", index);
	    }
	    
	    ruleBase.insert( std::pair<int,struct rule>(index,tempRule));
	    
	    line.clear();
	    tempRule.antecedent.clear();
	}
	
    }
    ruleBaseInput.close();
}

void djt5019 :: initKnowledge()
{
    int lineNumber = 0;
    string line;
    string name;
    ifstream knowledgeInput;
    
    knowledgeInput.open(KNOWLEDGEFILE);
    
    if( !knowledgeInput.is_open() )
    {
       fprintf(stderr, "initKnowledge Warning: Couldn't open the file %s, continuing anyway\n", KNOWLEDGEFILE );
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
			fprintf(stderr, "initKnowledge Warning: Line %d: %s seems to be a function declaration, skipping line\n", lineNumber, name.c_str());
			continue;
		}
		
		string temp = line.substr(line.find(" ")+1, line.size()); 
		
		if( temp == "true" )
		{
			knowledgeBase[name] = true;
		}
		else if( temp == "false")
		{
			knowledgeBase[name] = false;
		}
		else
		{
			fprintf(stderr, "initKnowledge Warning: Line %d: Syntax error \"%s\", defaulting %s to false\n", lineNumber,temp.c_str(), name.c_str());
			knowledgeBase[name] = false;
		}
			  
    }
    knowledgeInput.close();
}


bool djt5019 :: callFunction(const char board[MAX_Y][MAX_X], string functionName )
{
    if( functionName == "checkSurroundings()" ) return checkSurroundings(board);
    else if( functionName == "fill()" ){
      knowledgeBase["trapped"] = true;
      return true;
    }
    else if( functionName == "enemyAbove()" ) return enemyAbove();
    else if( functionName == "enemyBelow()" ) return enemyBelow();
    else if( functionName == "enemyRight()" ) return enemyRight();
    else if( functionName == "enemyLeft()" )  return enemyLeft();
    else if( functionName == "enemyDiagonalUpRight()" ) return enemyDiagonalUpRight();
    else if( functionName == "enemyDiagonalDownRight()" ) return enemyDiagonalDownRight();
    else if( functionName == "enemyDiagonalUpLeft()" ) return enemyDiagonalUpLeft();
    else if( functionName == "enemyDiagonalDownLeft()" ) return enemyDiagonalDownLeft();
    else
    {
	printf("callFunction: the function \'%s\' doesn't seem to exist\n", functionName.c_str());
	return false;
    }
}


bool djt5019 :: triggerRule(const char board[MAX_Y][MAX_X], struct rule theRule)
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

	if( tempString[0] == '!' )
	    expressionValue = !(knowledgeBase[tempString]);
	else
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
	    {
		tempResult = callFunction( board, ex );
		
		(ex[0] == '!') ? tempResult =  !tempResult : 0;
	    }
	    else
	    {
		tempResult = knowledgeBase[ex];
		
		(ex[0] == '!') ? tempResult = !tempResult : 0;
	    }

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


void djt5019 :: deriveExpression( const char board[MAX_Y][MAX_X], string expression )
{
    //! This function will iterate through the list of consequents in an effort 
    //! to determine if the expression is true or false
    
    map<int,struct rule>::iterator itr = ruleBase.begin();
    static int currentDepth = 0;
    
    printf("  +--->");
    
    if( currentDepth == MAXDEPTH )
    {
      knowledgeBase[expression] = false;
      currentDepth = 0;
      return;
    }
    else
      ++currentDepth;
      
    for( ; itr != ruleBase.end(); ++itr )
    {
	if( itr->second.consequent == expression )
	{
	    printf("\t%s was proven true\n", expression.c_str());
	    bool value = triggerRule( board, itr->second );
	    knowledgeBase[expression] = value;
	    currentDepth = 0;
	    return;
	}
    }
    
    printf("\t%s was unable to proven\n", expression.c_str());
     
    //The rule could not be derived so add it and default it to false
    knowledgeBase[expression] = false;
}

void djt5019 :: tryRules(const char board[MAX_Y][MAX_X], int& myX, int& myY)
{
   
    map<int,struct rule>::iterator itr;
    
    if( knowledgeBase["trapped"] == true || knowledgeBase["enemyTrapped"] == true)
	fill(myX,myY);
    
    for( itr=ruleBase.begin(); itr != ruleBase.end(); ++itr )
    {	  
	  // Iterate through each rule trying to see if it can be triggered
	  // if it can then observe the consequent action, set the function
	  // pointer to the relevent movement function then call the function
	  // The consequents will only be movement functions or proving knowledge facts
	  
	  if( triggerRule( board, itr->second ) && moved != true)
	  {	      	    
	      string functionName = itr->second.consequent;
	      
	      if( functionName.find("()") == string::npos )
	      {
		  printf("tryRules: %s is now set to true\n", functionName.c_str());
		  knowledgeBase[ functionName ] = true;
		  break;
	      }

	      printf("tryRules: Action take is \'%s\'\n", functionName.c_str());
     	      
	      if( functionName ==  "moveLeft()" )
	      {
		  moveLeft(myX);
		  moved = true;
	      }
	      else if( functionName ==  "moveRight()" )
	      {
		  moveRight(myX);		  
		  moved = true;
	      }
	      else if( functionName == "moveUp()" )
	      {
		  moveUp(myY);
		  moved = true;		  
	      }
	      else if( functionName == "moveDown()" )
	      {
		  moveDown(myY);
		  moved = true;
	      }
	      else
	      {
		if( callFunction(board,functionName) == false )
		{
		  // The function call didn't succeed
		  // When in doubt, go down.
		  moveDown(myY);
		  moved = true;
		}
	      }
	     	      
	      break;
	  }
    }
}

void djt5019 :: fill(int& myX, int& myY)
{
    if( moved == true ) return;
    
    if( knowledgeBase["upIsOpen"] == true )
    {
	moveUp(myY);
	moved = true;
    }
    else if( knowledgeBase["leftIsOpen"] == true )
    {
	moveLeft(myX);		  
	moved = true;
    }
    else if( knowledgeBase["rightIsOpen"] == true )
    {
	moveRight(myX);
	moved = true;		  
    }
    else if( knowledgeBase["downIsOpen"] == true )
    {
	moveDown(myY);
	moved = true;
    }
}

void djt5019 :: resetDirections()
{
    knowledgeBase["rightIsOpen"] = false;
    knowledgeBase["leftIsOpen"]  = false;
    knowledgeBase["downIsOpen"]  = false;
    knowledgeBase["upIsOpen"]    = false;
    knowledgeBase["up"] 	 = false;
    knowledgeBase["left"]	 = false;
    moved = false;
}

bool djt5019 :: Move(const char board[MAX_Y][MAX_X], int& me_x, int& me_y, int them_x, int them_y)
{
    resetDirections();
    
    myX    = &me_x;
    myY    = &me_y;
    themX  = them_x;
    themY  = them_y;
    
    if( checkSurroundings(board) )
    {
		shortestPath(board);
		lookAhead();
		tryRules(board, me_x, me_y);
		fill(me_x,me_y);
		(debug()) ? cin.get() : 0;
		return true;
    }
    
    (debug())? cin.get() : 0;
	return false;
}


bool djt5019 :: enemyAbove()
{
    return ((themY < *myY ) && (themX == *myX));
}

bool djt5019 :: enemyBelow()
{
    return ((themY > *myY ) && (themX == *myX));
}

bool djt5019 :: enemyRight()
{
    return ((themX > *myX ) && (themY == *myY)) ;
}

bool djt5019 :: enemyLeft()
{
    return ((themX < *myX ) && (themY == *myY));
}

bool djt5019 :: enemyDiagonalUpLeft()
{
    return ( themY < *myY ) && (themX < *myX);
}

bool djt5019 :: enemyDiagonalUpRight()
{
    return ( themY < *myY ) && (themX > *myX);
}

bool djt5019 :: enemyDiagonalDownLeft()
{
    return ( themY > *myY ) && (themX < *myX);
}

bool djt5019 :: enemyDiagonalDownRight()
{
    return ( themY > *myY ) && (themX > *myX);
}

void djt5019 :: lookAhead()
{    
    int countU = 0;
    int countD = 0;
    int countL = 0;
    int countR = 0;
    
    int X = *myX;
    int Y = *myY;
    
    //lookRight
    for(int i = X; i < MAX_X; ++i)
    {
	(zones_of_control[Y][i] > 0) ? ++countR : 0;
    }  

    for(int i = *myX; i > 0; --i)
    {
	(zones_of_control[Y][i] > 0) ? ++countL : 0; 
    }

    for(int i = *myY; i< MAX_Y; ++i)
    {
	(zones_of_control[i][X] > 0) ? ++countD: 0;
    }
    
    //lookUp
    for(int i = *myY; i > 0; --i)
    {
      (zones_of_control[i][X] > 0) ? ++countU : 0;
    }
    
    if( countL > countR ) knowledgeBase["left"] = true;
    if( countL <= countR )knowledgeBase["left"] = false;
    if( countU > countD ) knowledgeBase["up"] 	= true;
    if( countU <=countD ) knowledgeBase["up"] 	= false;
}

///////////////////////
// AI FUNCTIONS BELOW
///////////////////////

bool djt5019 :: checkSurroundings( const char board[MAX_Y][MAX_X] )
{
    //! This will check around the players piece looking for a place to move
    
    int x = *myX;
    int y = *myY;
    int blockedCount = 0;
    
    if( board[y-1][x] != ' ' )
    {
	knowledgeBase["upIsOpen"] = false;
	++blockedCount;
    }
    else
	knowledgeBase["upIsOpen"] = true;
    
    
    if( board[y][x-1] != ' ' )
    {
	knowledgeBase["leftIsOpen"] = false;
	++blockedCount;
    }
    else
	knowledgeBase["leftIsOpen"] = true;
    
    
    if( board[y][x+1] != ' ' )
    {
	knowledgeBase["rightIsOpen"] = false;
	++blockedCount;
    }
    else
	knowledgeBase["rightIsOpen"] = true;
    
    
    if( board[y+1][x] != ' ' )
    {
	knowledgeBase["downIsOpen"] = false;
	++blockedCount;
    } 
    else
	knowledgeBase["downIsOpen"] = true;
    
    if( blockedCount == 4 )
    {
	knowledgeBase["trapped"] = true;
	return false;
    }
   
    return true;
}

bool djt5019 :: shortestPath( const char G[MAX_Y][MAX_X] )
{
    return compare_shortest_paths(G, *myX, *myY, themX, themY);
}

void djt5019 :: initializeGraph(vertex_info G[MAX_Y][MAX_X], int sx, int sy)
{
    for (int i = 0; i < MAX_Y; i++)
    {
        for (int j = 0; j < MAX_X; j++)
        {
            G[i][j].x = j;
            G[i][j].y = i;
            G[i][j].dist    = -999;
            G[i][j].visited = false;
        }
    }
    G[sy][sx].dist    = 0;
    G[sy][sx].visited = false;
}

bool djt5019 :: compare_shortest_paths(const char board[MAX_Y][MAX_X], int p1x, int p1y, int p2x, int p2y)
{
    int wall = -999;
    vertex_info p1graph[MAX_Y][MAX_X];
    vertex_info p2graph[MAX_Y][MAX_X];
    
    initializeGraph(p1graph, p1x, p1y);
    initializeGraph(p2graph, p2x, p2y);

    queue<vertex_info> q1;
    queue<vertex_info> q2;
    
    q1.push( p1graph[p1y][p1x] );
    q2.push( p2graph[p2y][p2x] );
    
    while( !q1.empty() )
    {
	vertex_info u = q1.front();
	int x = u.x;
	int y = u.y;
	int distance = u.dist;
	
	if( p1graph[y][x].visited == false )
	{
	    if( p1graph[y][x+1].visited == false && board[y][x+1] == ' ' )
	    {
		p1graph[y][x+1].dist = distance+1;
		q1.push( p1graph[y][x+1] );
	    }
	    if( p1graph[y][x-1].visited == false && board[y][x-1] == ' ' )
	    {
		p1graph[y][x-1].dist = distance+1;
		q1.push( p1graph[y][x-1] );
	    }
	    if( p1graph[y+1][x].visited == false && board[y+1][x] == ' ' )
	    {
		p1graph[y+1][x].dist = distance+1;
		q1.push( p1graph[y+1][x] );
	    }
	    if( p1graph[y-1][x].visited == false && board[y-1][x] == ' ' )
	    {
		p1graph[y-1][x].dist = distance+1;
		q1.push( p1graph[y-1][x] );
	    }
	}
	p1graph[y][x].visited = true;
	q1.pop();
    }
    
    while( !q2.empty() )
    {
	vertex_info u = q2.front();
	int x = u.x;
	int y = u.y;
	int distance = u.dist;
	
	if( p2graph[y][x].visited == false )
	{
	    if( p2graph[y][x+1].visited == false && board[y][x+1] == ' ' )
	    {
		p2graph[y][x+1].dist = distance+1;
		q2.push( p2graph[y][x+1] );
	    }
	    if( p2graph[y][x-1].visited == false && board[y][x-1] == ' ' )
	    {
		p2graph[y][x-1].dist = distance+1;
		q2.push( p2graph[y][x-1] );
	    }
	    if( p2graph[y+1][x].visited == false && board[y+1][x] == ' ' )
	    {
		p2graph[y+1][x].dist = distance+1;
		q2.push( p2graph[y+1][x] );
	    }
	    if( p2graph[y-1][x].visited == false && board[y-1][x] == ' ' )
	    {
		p2graph[y-1][x].dist = distance+1;
		q2.push( p2graph[y-1][x] );
	    }
	}
	p2graph[y][x].visited = true;
	q2.pop();
    }
    
    for (int i = 0; i < MAX_Y; i++)
      for (int j = 0; j < MAX_X; j++)
      {
	if (p1graph[i][j].dist == wall)
	  zones_of_control[i][j] = wall;
	else if (p1graph[i][j].dist != wall && p2graph[i][j].dist == wall)
	  zones_of_control[i][j] = p1graph[i][j].dist;
	else
	{
	  zones_of_control[i][j] = p2graph[i][j].dist - p1graph[i][j].dist;
	}
      }
    
    if( zones_of_control[p2y-1][p2x] == wall && zones_of_control[p2y+1][p2x] == wall && zones_of_control[p2y][p2x-1] == wall && zones_of_control[p2y][p2x+1] == wall )
    {
	knowledgeBase["trapped"] = true;
	knowledgeBase["enemyTrapped"] = true;
	return false;
    }
    
    return true;
} 
#endif