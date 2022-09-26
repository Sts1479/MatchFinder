/*
 * FileStrFinder.h
 *
 *  Created on: 7 сент. 2022 г.
 *      Author: stan
 */

#ifndef FILESTRFINDER_H_
#define FILESTRFINDER_H_


#include <list>
#include <string>
#include <vector>


struct Line{
    std::string linestr;
    size_t linenum;
};

struct PosWords{
	int position;
	std::string word;
};

struct Matches{
    size_t linenum;
    std::vector<PosWords> poswords;
};

class FileStrFinder {
public:
	FileStrFinder() = default;
	virtual ~FileStrFinder(){};
	 /*
	@brief method for find a word by start position
	@param line - input line from input file
	@param start_pos - start position for searching
	@param delim - delimiters around the word
	@return  word
	*/
	std::string FindWordByPosition(const std::string &line,int start_pos,const std::string &delim);

    /*
	@brief method for find a word by start position
	@param line - input line from input file
	@param start_pos - start position for searching
	@param delim - delimiters around the word
	@param mask - mask for searching matches in words
	@return  word
	*/
	std::string FindWordByPosition(const std::string &line,int start_pos,const std::string &delim, const std::string& mask);
    

	/*
	@brief method for get occurences from input line
	@param line - Line struct (see FileStrFinder.h)
	@param mask - mask for searching matches in words
	*/
	void GetOccurencesFromLine(const Line& line,const std::string &mask);

	/*
	@brief method for get occurences from input file
	@param inlineList - struct that contains all lines from input file
	@param mask - mask for searching matches in words
	*/
	void FindOccurences(const std::list<Line> &inlineList,const std::string &mask);
    
    /*
	@brief method for get struct contains all results
	@return struct
	*/

    auto GetOccurences ()
    {
    	return _occurencesList;
    }

private:
	std::vector<std::list<Line>> _inlineList;
	std::list<Matches> _occurencesList;

};

#endif /* FILESTRFINDER_H_ */
