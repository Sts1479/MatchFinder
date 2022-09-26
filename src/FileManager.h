/*
 * FileManager.h
 *
 *  Created on: 7 сент. 2022 г.
 *      Author: stan
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include <filesystem>
#include "FileStrFinder.h"
namespace fs = std::filesystem;

class FileManager
{
public:
	FileManager() = default;
	virtual ~FileManager(){};
    /*
	@brief method for reading data from input file to struct
	@param filepath - path to input file 
	@param num_of_threads - set the nym of threads for searching
	*/
	void ReadFileToList(const std::string &filepath, int num_of_threads );
    /*
	@brief getter for _inlineList struct - here all data from input file
	@return &_inlineList
	*/
	auto GetInlineList ()
	{
		return &_inlineList;
	}
    /*
	@brief method for checking directory path of any file
	@param p - path to the file
	@return true if dir exists
	*/
	//bool DirExists(const fs::path& p, fs::file_status s = fs::file_status{});
	bool DirExists(const fs::path& p);
    /*
	@brief method for write result of searching to output file
	@param wlist - we've get data from this struct
	@param file -  file to write result
	*/
	void WriteOccursListToFile(const std::list<Matches> &wlist,std::ofstream &file);
	
    /*
	@brief method for write result of searching to console
	@param wlist - we've get data from this struct
	@return counter = size of wlist + substructures
	*/

	size_t WriteOccursListToConsole(const std::list<Matches> &wlist);

	 /*
	@brief method for get num of occurences
	@param wlist - we've get data from this struct
	@return counter = occurences in input file
	*/

	size_t NumOfOccurencesToFile(const std::list<Matches> &wlist);

	
private:
	std::vector<std::list<Line>> _inlineList;
	std::vector<std::string> _fileList;
};


#endif /* FILEMANAGER_H_ */
