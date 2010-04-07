#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <string>
#include <vector>

using std::vector;
using std::string;
using std::ifstream;

struct rule
{
    string name;
    vector<string> antecedent;
    string consequent;
};

void initalizeExpertSystem();
void initRules();
void initKnowledge();
void deriveExpression(string expresson);
bool evaluateRule(struct rule theRule);
bool searchForRule( string rulename );
bool checkArea();
bool lookupKnowledge( string name );
bool triggerRule( struct rule theRule );

void moveLeft( int& x, int& y);
void moveRight(int& x, int& y);
void moveUp(int& x, int& y);
void moveDown(int& x, int& y);

#endif
