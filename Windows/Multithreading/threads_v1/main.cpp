// threads_v1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <string>
#include <iostream>
#include <thread>

void task1(std::string msg);

// https://stackoverflow.com/questions/266168/simple-example-of-threading-in-c
int main()
{
	// Constructs the new thread object and runs it. Does not block execution.
	// The constructor's arguments are the function the thread will execute, 
	//    followed by the function's parameters. 
	// The thread is automatically started upon construction.
	std::thread t1(task1, "Hello");
	std::thread t2(task1, "Goodbye");

	// Do other things...
	std::cout << "Doing other stuff\n";
	std::cin.get();
	std::cout << "Finished other stuff\n";

	// Makes the main thread wait for the new thread to finish execution, therefore blocks its own execution.
	// If later on you want to wait for the thread to be done executing the function, call:
	//t1.join();
	//t2.join();
	// Joining means that the thread who invoked the new thread will wait for the new thread to finish execution, 
	//    before it will continue its own execution).
	return 0;
}

// The function we want to execute on the new thread.
void task1(std::string msg)
{
	std::string input;
	std::cout << "task1 says: " << msg;
	std::cin >> input;
	std::cout << "You typed " << input << std::endl;
	return;
}



