/**
 * hw1.cpp
 * Grant Ludwig
 * Based on idea from Matthew Flatt, Univ of Utah
 * Original version of code based on hw1_setup.cpp\
 */
#include <iostream>
#include "ThreadGroup.h"
using namespace std;

const int N_THREADS = 2;

/*
* Struct to hold shared data between the threads 
*/
struct ShareData {
	int *data;
	int length;
};

int encode(int v) {
	// do something time-consuming (and arbitrary)
	for (int i = 0; i < 500; i++)
		v = ((v * v) + v) % 10;
	return v;
}

int decode(int v) {
	// do something time-consuming (and arbitrary)
	return encode(v);
}

/**
* Struct that is passed to the ThreadGroup class
*/
struct EncodeThread {
	/**
	* Will encode values in the data array depending on the id of the thread running it
	*/
	void operator()(int id, void *sharedData){
		ShareData *ourData = (ShareData*)sharedData;
		int pieceSize = ourData->length / N_THREADS;
		int startIndex = id * pieceSize;
		int endIndex = id * pieceSize + pieceSize - 1;
		for (int i = startIndex; i < endIndex + 1; i++)
			ourData->data[i] = encode(ourData->data[i]);
	}
};

/**
* Struct that is passed to the ThreadGroup class
*/
struct DecodeThread {
	/**
	* Will decode values in the data array depending on the id of the thread running it
	*/
	void operator()(int id, void *sharedData){
		ShareData *ourData = (ShareData*)sharedData;
		int pieceSize = ourData->length / N_THREADS;
		int startIndex = id * pieceSize;
		int endIndex = id * pieceSize + pieceSize - 1;
		for (int i = startIndex; i < endIndex + 1; i++){
			ourData->data[i] = decode(ourData->data[i]);
		}
	}
};

void prefixSums(int *data, int length) {
	ShareData ourData;
	ourData.data = data;
	ourData.length = length;

	//Encode
	ThreadGroup<EncodeThread> encoders;
    encoders.createThread(0, &ourData);
    encoders.createThread(1, &ourData);
    encoders.waitForAll();

    //Get the encoded sum
	int encodedSum = 0;
	for (int i = 0; i < length; i++) {
		encodedSum += data[i];
		data[i] = encodedSum;
	}

	//Decode
    ThreadGroup<DecodeThread> decoders;
    decoders.createThread(0, &ourData);
    decoders.createThread(1, &ourData);
    decoders.waitForAll();
}

int main() {
	int length = 1000 * 1000;

	// make array
	int *data = new int[length];
	for (int i = 1; i < length; i++)
		data[i] = 1;
	data[0] = 6;

	// transform array into converted/deconverted prefix sum of original
	prefixSums(data, length);

	// printed out result is 6, 6, and 2 when data[0] is 6 to start and the rest 1
	cout << "[0]: " << data[0] << endl
			<< "[" << length/2 << "]: " << data[length/2] << endl 
			<< "[end]: " << data[length-1] << endl; 

    delete[] data;
	return 0;
}
