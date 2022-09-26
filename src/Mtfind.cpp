//============================================================================
// Name        : Mtfind.cpp
// Author      : stan
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "HThreadPool.h"
#include "FileManager.h"
#include "FileStrFinder.h"
#include <chrono>
#include <fstream>
#include <string.h>
#ifdef __linux__ 
#include <termios.h>
#include <unistd.h>
#endif

#define NUM_OF_CONSOLE_STRINGS_OUT 10U

#ifdef __linux__ 
int getch( ) {
   struct termios oldt,
   newt;
   int ch;
   tcgetattr( STDIN_FILENO, &oldt );
   newt = oldt;
   newt.c_lflag &= ~( ICANON | ECHO );
   tcsetattr( STDIN_FILENO, TCSANOW, &newt );
   ch = getchar();
   tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
   return ch;
}
#endif

bool checkMaskParam (const std::string & mask)
{
	if (mask.find("?") == 0)
	{
	   return true;
	}
	return false;   
}

int main(int argc, char **argv) {
    const char * using_ans = " <path>/file_name.txt \"?mask\".Example - mtfind test.txt \"?ad\"";
	if (argc <= 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
	{
		cout << "Using:\n  " << argv[0] << using_ans << endl;
		cout << "Additional parameters:\n  " 
		<< "Path to out result file. Example - mtfind test.txt \"?ad\" out.txt \n " 
		<< "Set the num of threads (int). Example - mtfind test.txt \"?ad\" out.txt 4 " << std::endl;
		return 0;
	}

	if (argc < 3)
	{
		std::cout << "Program terminate: no input args or broken\n" << "Using:\n  " << argv[0] << using_ans << std::endl;
		return 0;
	}
    const std::string path_to_in_file = argv[1];
  	const std::string mask = argv[2];
  	const size_t mask_length = mask.size();
	std::string search_mask;
	std::string path_to_out_file = "out.txt"; // ..here searching results will be
    int cores_all = std::thread::hardware_concurrency(); //get threads of CPU cores..
    int using_threads = cores_all/4; //..by default
	if (mask_length == 0 || mask_length > 100)
	{
		cout << "Program terminate: search mask length should be less than 100 character\n" << "Using:\n  " << argv[0] << using_ans << endl;
		return 0;
	}
	if (checkMaskParam(mask) == false)
	{
		cout << "Program terminate: wrong search mask parameter format - ? missing. Example - \"?xx\"\n" << "Using:\n  " << argv[0] << using_ans << endl;
		return 0;
	}
	else
	{
		search_mask = mask.substr(1,mask.length());
	}
	if (argc >= 4)
	{
		path_to_out_file = argv[3];
	}
	if (argc == 5)
	{
        int threads = atoi(argv[4]);
		if (threads <= cores_all)
		{
			using_threads = threads;
		}
		else
		{
			cout << "Wrong input num of cores/threads: using default settings " << "Using:\n  " << argv[0] << using_ans << endl;
		}
	}
    auto start_time = std::chrono::steady_clock::now();
	auto start_time_reading = std::chrono::steady_clock::now();

    std::unique_ptr<FileManager> fileManagerInst = std::make_unique<FileManager>();    
    cout << "Searching in "<< path_to_in_file <<" starts" << endl; 

    //base thread level - set num of threads, read all lines from file to struct
    cout <<"Num of used system threads: " <<using_threads << endl;
    fileManagerInst.get()->ReadFileToList(path_to_in_file, using_threads);
	auto end_time_reading = std::chrono::steady_clock::now();

    // start threads to find all occurs
    vector<unique_ptr<FileStrFinder>> vecFileStrFinder; //put instances of the finder class to a vector..
	HThreadPool pool(using_threads); //set threads of pool = using_threads_count: may be better /2 ?
	pool.Start(); // start pool
	for (int i = 0; i < using_threads; i++)
	{   
		vecFileStrFinder.push_back(make_unique<FileStrFinder>()); //init of instances finder class
		pool.AddTask([&,i](){vecFileStrFinder[i].get()->FindOccurences(fileManagerInst.get()->GetInlineList()->at(i),search_mask);}); //put to pool main func to find occurs in lines
	}
	pool.Wait();
	pool.Stop(); //stop all threads
    // base thread level - write results to out file
	if(!fileManagerInst.get()->DirExists(path_to_out_file))
	{
		path_to_out_file = "out.txt";
		cout << "Dir for output file does not exist, write results to default file " << path_to_out_file << endl;
	}
	std::ofstream file;
	file.open(path_to_out_file);
	if (!file.is_open())
	{
		cout << "Program terminate: error open file " << path_to_out_file << " to write results" <<endl;
		exit(0);
	}

	auto end_time = std::chrono::steady_clock::now();
	auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time);
    auto elapsed_reading_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time_reading-start_time_reading);
    auto elapsed_searching_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time-end_time_reading);

	size_t numOfOccur = 0;
	
	for (int i = 0; i < using_threads; i ++)
	{
		numOfOccur+=fileManagerInst.get()->NumOfOccurencesToFile(vecFileStrFinder[i].get()->GetOccurences());
	}
	file << numOfOccur << endl;
	for (int i = 0; i < using_threads; i++)
	{
		fileManagerInst.get()->WriteOccursListToFile(vecFileStrFinder[i].get()->GetOccurences(),file);
	}
	file.close();
	cout << numOfOccur << endl;
	size_t cons_counter = 0;
	int key_space = 0;
	for (int i = 0; i < using_threads; i++)
	{
		cons_counter+=fileManagerInst.get()->WriteOccursListToConsole(vecFileStrFinder[i].get()->GetOccurences());
#ifdef __linux__ 
		if (cons_counter >= NUM_OF_CONSOLE_STRINGS_OUT)
		{
			while (key_space!=' ') 
			{
				cout<< "Press SPACE to continue.. " << endl;
				key_space = getch();
			}
			key_space = 0;
			cons_counter = 0;
		}
#endif
	}
    cout << "Average time of all processing: " << elapsed_us.count() << " us" << endl;
	cout << "Average time of reading file: " << elapsed_reading_us.count() << " us" << endl;
    cout << "Average time of searching: " << elapsed_searching_us.count() << " us" << endl;

	cout << "The program completed successfully. Results in file " << path_to_out_file << endl;
	return 0;
}