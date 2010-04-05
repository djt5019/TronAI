#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
using namespace std;

#define RULEFILE "rulefile.txt"
#define KNOWLEDGEFILE "knowledge.txt"

struct rule
{
    string name;
    vector<string> antecedent;
    string consequent;
};


map<string, bool> knowledgeBase;
map<int, struct rule> ruleBase;

//! Rules will follow the form of IF <expr1> AND/OR <expr2> ... AND/OR <exprN> THEN consequent
//! The expressions can either be pulled from the knowledgeBase or can be functions
//! For example: IF near AND notTrapped() THEN floodFill()
//! The functions in the rulebase should return boolean.

void initalizeExpertSystem()
{
    //! Will read from the rule file, create a structure for each containing the consequent
    //! and the list of antecedents.
    ifstream input;
    
    initRules( input );
    initKnowledge( input);
}

void initRules(ifstream& ruleBaseInput)
{    
    int index = 0;
    string line;
    string temp;
    
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
	    tempRule.consequent = temp;
	

	  
	if( temp.find(';') != string::npos ) //!Found a semicolon delimiter 
	{
	    ++index;
	    
	    if( tempRule.antecedent.size() % 2 != 1 )
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

void initKnowledge(ifstream& knowledgeInput)
{
    string name;
    bool state;
    
    knowledgeInput.open( KNOWLEDGEFILE );
    
    if( !knowledgeInput.open() )
    {
       fprintf(stderr, "%s Error: Couldn't open the file %s\n",__func__, KNOWLEDGEFILE );
       exit(EXIT_FAILURE);
    }
    
    while( !knowledgeInput.eof() )
    {
	knowledgeInput >> name;
	knowledgeInput >> state;
	knowledgeBase.insert( std::pair<string, bool>(name,state) );
	name.clear();
    }
    
    knowledgeInput.close();
}

bool evaluateRule(struct rule theRule)
{
    //! This will take a function then parse it and proceed to call the
    //! following functions or poll the knowledgeBase.  If something is 
    //! proven as a result of the parsing process then it will be added
    //! to the knowledgeBase.  If the rule returns true then the parse
    //! function will return true as well.  This will be called multiple
    //! times to find a good rule to apply to the game.
    
    struct rule rules;
    bool fact;
    
    bool (*function)(void) = NULL;
    bool expressionValue = false;
    
    if( theRule.antecedent.size() < 2 )
    {
	//There is only one expression
	expressionValue = 
	
    }
    
    vector<string>::iterator prevItr = theRule.antecedent.begin() + 1;
    vector<string>::iterator currentItr = theRule.antecedent.begin() + 2;
    
    while( currentItr != theRule.antecedent.end() )
    {
	string tempString = *currentItr;
	string op  = *prevItr;
	bool result;
	
	if( tempString.find("()") != string::npos )
	{
	    //TODO: Add the functions here 
	    // Call the function, save the return value
	    // then use the operator and 
	}
	else
	{
	  //It's not a function so look it up in the knowledge base    
	  map<string,bool>::iterator itr = knowledgeBase.find( tempString );
	  
	  if( itr == map::end() )
	  {
	      //knowledge doesnt exist in the database so try to prove it
	      result = searchForRule( tempString );
	  }
	  
	}
	
	if( op == "AND" || op == "and" )
	{
	}
	else if( tempString == "OR" || tempString == "or" )
	{
	}
	else if( tempString == "NOT" || tempString == "not" )
	{
	}
	
	prevItr    += 2;
	currentItr += 2;
    }
    
    
    return expressionValue; 
}

bool searchForRule( string rulename )
{
    //! This will search for the rulename in the list of consequents stored in the
    //! rulebase in an effort to prove wether it is true or false
    
    
}

#ifdef DEBUG
int main()
{
  initalizeExpertSystem();
  return 0;
}
#endif

