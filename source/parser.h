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

struct _square
{
    int x;
    int y;
    int playerNum;
    bool topedge;
    bool bottomedge;
    bool leftedge;
    bool rightedge;
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
    void move(int& myX, int& myY);

    //! Decision Functions
    bool checkSurroundings( const char board[MAX_Y][MAX_X] );
    

  private:
    map<string, bool> knowledgeBase;
    map<int, struct rule> ruleBase;
    bool moved;
};

#endif
