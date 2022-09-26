#include "FileStrFinder.h"
#include <iostream>
#include <fstream>
#include <numeric>
#include <boost/algorithm/string.hpp>

using namespace std;

const std::string  delims = " ,:;." ;

using StringRange = boost::iterator_range<std::string::const_iterator>;

std::string FileStrFinder::FindWordByPosition(const std::string &line,int start_pos,const std::string &delim)
{
    size_t found_pos_right;
    size_t found_pos_left;
    std::string substr_left;
    std::string substr_right;
    try
    {
        substr_right = line.substr(start_pos);
        substr_left = line.substr(0,start_pos);
        found_pos_left = substr_left.find_last_of(delim);
        if (found_pos_left >= INT_MIN)
            found_pos_left = 0;
        found_pos_right = substr_right.find_first_of(delim)+start_pos;
        return line.substr(found_pos_left,found_pos_right - found_pos_left);
    }
    catch (exception &e)
    {
        cout << "Error: " << e.what() << endl;
        return "";
    }
};

std::string FileStrFinder::FindWordByPosition(const std::string &line,int start_pos,const std::string &delim, const std::string& mask)
{
    size_t found_pos_left;
    std::string substr_left;
    try
    {
        substr_left = line.substr(0,start_pos);
        found_pos_left = substr_left.find_last_of(delim);
        if (found_pos_left >= INT_MIN)
            found_pos_left = 0;
        return line.substr(found_pos_left, start_pos - found_pos_left + mask.size());
    }
    catch (exception &e)
    {
        cout << "Error: " << e.what() << endl;
        return "";
    }
};


void FileStrFinder::GetOccurencesFromLine(const Line& line,const std::string &mask)
{
    auto linestr = line.linestr;
    auto linenum = line.linenum;
    std::vector<StringRange> matches;
    boost::find_all(matches, linestr, mask);
    Matches matchesInst;
    matchesInst.linenum = linenum;
    std:string old_word ="";
    int old_pos = 0;
    if (matches.size() != 0)
    {
        for (auto match : matches) {
            int pos = match.begin() - linestr.begin();
            bool delims_flag = false;
            auto word = FindWordByPosition(linestr,pos,delims,mask);
            if (old_word == word)
            {
                auto left_subs = linestr.substr(old_pos);
                if (left_subs.find_first_of(delims))
                    matchesInst.poswords.push_back({pos,word});
            }
            if (old_word != word)
                matchesInst.poswords.push_back({pos,word});
            old_word = word;
            old_pos = pos;
    }
        _occurencesList.push_back(matchesInst);
    }
}

void FileStrFinder::FindOccurences(const std::list<Line> &inlineList,const std::string &mask)
{
    for (auto line : inlineList)
    {
       GetOccurencesFromLine(line, mask);
    }
}