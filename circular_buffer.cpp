// circular_buffer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include<mutex>
#include<condition_variable>
#include<algorithm>


using namespace std;

#define BUFF_SIZE 4086   // buffer size
#define MAX_WRITES 512   // maximun number of writes at each try 
#define MAX_READS 512    // maximun number of reades at each try

int buffer[BUFF_SIZE];  // circular buffer

int write_ptr = 0;    // write pointer (index)
int read_ptr = 0;     // read pointer (index)


int write_num = 0;    // number of writes at each try
int read_num = 0;    // number of reades at each try


mutex mmutex;
condition_variable okToWrite;
condition_variable okToRead;


//void initial_buffer() {
//
	//for (int i = 0; i < 100; i++) buffer[(write_ptr + i) % BUFF_SIZE] = 100;
//	write_ptr = (write_ptr + 100) % BUFF_SIZE;
	//write_num = (write_num + 100) % (2 * BUFF_SIZE);

//}


void Writer(){
		while (true) {
		unique_lock<mutex> lck{ mmutex }; // protect operations
		while (write_ptr == read_ptr && (write_num - read_num) % (2 * BUFF_SIZE) == BUFF_SIZE) {
			okToWrite.wait(lck);
			
		}
		for (int i = 0; i < MAX_WRITES; i++) buffer[(write_ptr + i) % BUFF_SIZE] = i;
		write_ptr = (write_ptr + MAX_WRITES) % BUFF_SIZE;
		write_num = (write_num + MAX_WRITES) % (2*BUFF_SIZE);
		okToRead.notify_one();
		lck.unlock();
		} // release lock (at end of scope)
}


void Reader() {
		while (true) {
		unique_lock<mutex> lck{ mmutex }; // protect operations
		while (write_ptr == read_ptr && (write_num - read_num) % (2 * BUFF_SIZE) == 0) {
			okToRead.wait(lck);
			
		}
		for (int i = 0; i < MAX_READS; i++) cout<< buffer[(read_ptr + i) % BUFF_SIZE] <<endl;
		read_ptr = (read_ptr + MAX_READS) % BUFF_SIZE;
		read_num = (read_num + MAX_READS) % (2 * BUFF_SIZE);
		okToWrite.notify_one();
		lck.unlock();
	} // release lock (at end of scope)
}


int main() {

	//initial_buffer();
	thread t1{ Writer}; // f(some_vec) executes in a separate thread
	thread t2{ Reader }; // F(vec2)() executes in a separate thread
	t1.join();
	t2.join();

	return 0;
	
}






