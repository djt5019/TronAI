#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <string>
#include <vector>
#include "tron.cpp"

using std::vector;
using std::string;
using std::ifstream;

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

class djt5019 : public Player2{
  
  public:	
    int* myX;
    int* myY;
    int themX;
    int themY;
    int __debug__;
    
    djt5019() { initalizeExpertSystem(); }
    bool debug() { if(__debug__) return true; else return false; }
   
    void initalizeExpertSystem();
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
    bool checkSurroundings( const char board[MAX_Y][MAX_X] );
    bool shortestPath(const char board[MAX_Y][MAX_X]);

  private:
    map<string, bool> knowledgeBase;
    map<int, struct rule> ruleBase;
    bool moved;
    int zones_of_control[MAX_Y][MAX_X];
    
    bool compare_shortest_paths(const char G[MAX_Y][MAX_X], int p1x, int p1y, int p2x, int p2y);
    void initializeGraph(vertex_info G[MAX_Y][MAX_X], int sx, int sy);
};

#endif
