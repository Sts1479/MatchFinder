#include "FileManager.h"
#include <iostream>
#include <fstream>
#include <numeric>

using namespace std;


void FileManager::ReadFileToList(const std::string &filepath, int num_of_threads )
{
    try
    {
        std::runtime_error file_open_error("Program terminate: input file not exists"); 
        std::fstream file;
        file.open(filepath);
        if (!file.is_open())
            throw file_open_error;
        std::string readLine;
        Line line;
        line.linenum = 0;
        size_t line_counter = 0;
        while(file.ignore((numeric_limits<streamsize>::max)(), '\n')) //calc num of lines in file
            line_counter++;
        file.close();
        int n;
        size_t lines_on_thread = line_counter/num_of_threads; //calc how lines on one thread
        for (n = 0; n < num_of_threads;n++)
        {
            _inlineList.push_back(std::list<Line>()); //init List (size = num_of_threads) of lines
        }
        line_counter = 0;
        n = 0;
        file.open(filepath);
        while(getline(file, readLine)) //begin reading lines and pushing to the struct
        {
            line.linenum++;
            line_counter++;
            line.linestr = readLine;
            _inlineList[n].push_back(line);
            
            if ((lines_on_thread - line_counter) == 0 && (n < num_of_threads-1)) //
            {
                line_counter = 0;
                n++;
            }
        }
        file.close();
    }
    catch(std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        exit(0);
    }
}

bool FileManager::DirExists(const fs::path& p)
{   
    fs::file_status s = fs::file_status{};
    if (p.parent_path() == "")
        return true;
    if(!(fs::status_known(s) ? fs::exists(s) : fs::exists(p.parent_path())))
    {
        return false;
    }
    return true;
}

void FileManager::WriteOccursListToFile(const std::list<Matches> &wlist,std::ofstream &file)
{
    try
    {
        for (auto wl: wlist)
        {
            for (auto w: wl.poswords)
            {
                file << wl.linenum << " " << w.position << " " << w.word << " " << endl;
            }
        }
    }
    catch(std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

size_t FileManager::WriteOccursListToConsole(const std::list<Matches> &wlist)
{
    size_t counter = 0;
    try
    {
        for (auto wl: wlist)
        {
            for (auto w: wl.poswords)
            {
                counter++;
                cout << wl.linenum << " " << w.position << " " << w.word << " " << endl;
            }
        }
        return counter;
    }
    catch(std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return counter;
    }
}

size_t FileManager::NumOfOccurencesToFile(const std::list<Matches> &wlist)
{
    size_t numOfOccur = 0;
    for (auto wl: wlist)
    {
        numOfOccur+=wl.poswords.size();
    }
    return numOfOccur;
}