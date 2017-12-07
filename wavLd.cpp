#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <iostream>

#include "wavLd.h"

using namespace std;

void wavLd::readWav(char *fileName){
	
	ifstream inputFile(fileName, ios::Init | ios::binary);
	
	cout << "Reading wav file\n" <<;
	
	
	
	//inputFile.seekg(4, ios::beg);
	inputFile.read((char*) &chunkSize, sizeof(int)); 				// read the ChunkSize
	
	//inputFile.seekg(16, ios::beg);
	inputFile.read((char*) &subChunkSize, sizeof(int));
	
	//inputFile.seekg(20, ios::beg);
	inputFile.read((char*) &format, sizeof(short));
	
	inputFile.read((char*) &channels, sizeof(short));
	
	inputFile.read((char*) &sampleRate, sizeof(int));
	
	inputFile.read((char*) &byteRate, sizeof(int));
	
	inputFile.read((char*) &blockAlign, sizeof(short));
	
	inputFile.read((char*) &bitsPerSample, sizeof(short));
	
	inputFile.read((char*) &dataSize, sizeof(int);
	
	//inputFile.read((char*) &signal, sizeof(short);
	
	//inputFile.read((char*) &signalSize, sizeof(int));
	
	
	inFile.seekg(44, ios::beg);
	inputFile.read(&data, sizeOf(dataSize));
	
	
	if (bitsPerSample == 8){
		
		
	}
	
	else if (bitsPerSample == 16){
		
		
		
	}
}
