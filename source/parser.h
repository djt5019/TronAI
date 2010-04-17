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
    
    djt5019() { initalizeExpertSystem(); }
    
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
    int SquaresReachable(const char board[MAX_Y][MAX_X], int& me_x, int& me_y, _square squaresReachable[MAX_Y*MAX_X]);
    int Distance(const char board[MAX_Y][MAX_X],const int start_x,const int start_y,const int end_x,const int end_y);
    int ShortestPath(const char board[MAX_Y][MAX_X], const int start_x,const int start_y,const int end_x,const int end_y, _square movesList[MAX_Y*MAX_X], int stackCount);
    bool FillPoly(const char board[MAX_Y][MAX_X], int& me_x, int& me_y);

  private:
    map<string, bool> knowledgeBase;
    map<int, struct rule> ruleBase;
    bool moved;
};

#endif
