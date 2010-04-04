#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
using namespace std;

#define RULEFILE "rulefile.txt"
const string AND =  "&&";
const string OR  =  "||";

struct knowledge
{
    bool state;
    string name;
};

struct rule
{
    string name;
    vector<string> antecedent;
    string consequent;
};

ifstream ruleBaseInput;
map<int, struct knowledge> knowledgeBase;
map<int, struct rule> ruleBase;

//! Rules will follow the form of IF <expr1> AND/OR <expr2> ... AND/OR <exprN> THEN consequent
//! The expressions can either be pulled from the knowledgeBase or can be functions
//! For example: IF near AND notTrapped() THEN floodFill()
//! The functions in the rulebase should return boolean.

void initalizeExpertSystem()
{
    //! Will read from the rule file, create a structure for each containing the consequent
    //! and the list of antecedents.
    
    int index = 0;
    string line;
    string temp;
    
    bool antecedentFlag = false;
    bool consequentFlag = false;
    
    struct rule tempRule;
    
    ruleBaseInput.open( RULEFILE );
    
    if( !ruleBaseInput.is_open() )
    {
	printf("%s Error: The Rule File could not be opened\n", __func__);
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
	}
	if( temp == "AND" || temp == "and" )
	   temp = "&&";
	else if (temp == "OR" || temp == "or" )
	   temp = "||";
	else if (temp == "NOT" || temp == "not")
	   temp = "!";
	
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
	    ruleBase.insert( std::pair<int,struct rule>(++index,tempRule));
	    line.clear();
	    tempRule.antecedent.clear();
	}
	
    }
    
    ruleBaseInput.close();
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
    struct knowledge fact;
    
    
    return false;
    
}
#ifdef DEBUG

int main()
{
  initalizeExpertSystem();
  return 0;
}

#endif

