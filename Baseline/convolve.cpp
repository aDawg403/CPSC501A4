#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fstream>
#include <iostream>

using namespace std;

char chunkId[4];
int chunkSize;
char wavFormat[2];

char firstSubChunkId[4];
int firstSubChunkSize;

int16_t aFormat;
int16_t channels;

int sampleRate;
int byteRate;
int16_t blockAlignment;
int16_t bitsPerSample;

char secondSubChunkId[4];
int secondSubChunkSize;
short* fileData;

int sizeOfOutput;
int wavSize;


float *wavReader(float *sig, char *inputFile, int *inputSize);
void wavWriter(float *outputSig, int sigSize, char *outputFile);
void convolveSigs(float x[], int N, float h[], int M, float y[], int P);
void scale(float *outputSignal, int size);



float* wavReader(float *sig, char *inputFile, int *inputSize){
	
	//Load header values
	ifstream inFile(inputFile, ios::in | ios::binary);
	inFile.seekg(ios::beg);
	inFile.read(chunkId, 4);
	inFile.read((char*) &chunkSize, 4);
	inFile.read(wavFormat, 4);
	inFile.read(firstSubChunkId, 4);
	inFile.read((char*) &firstSubChunkSize, 4);
	inFile.read((char*) &aFormat, 2);
	inFile.read((char*) &channels, 2);
	inFile.read((char*) &sampleRate, 4);
	inFile.read((char*) &byteRate, 4);
	inFile.read((char*) &blockAlignment, 2);
	inFile.read((char*) &bitsPerSample, 2);
	if (firstSubChunkSize == 18){	
		char *trash = new char[2];
		inFile.read(trash, 2);
	}
	inFile.read(secondSubChunkId, 4); 
	inFile.read((char*)&secondSubChunkSize, 4);
	
	
	//Load data 
	*inputSize = secondSubChunkSize/2;
	wavSize = secondSubChunkSize/2;
	short *wavData = new short[wavSize];
	for (int i = 0; i < wavSize; i++){
		inFile.read((char *) &wavData[i], 2);
	}
	inFile.close();
	
	short sam;
	sig = new float[wavSize];
	for (int i = 0; i < wavSize; i++){
		sam = wavData[i];
		sig[i] = (sam*1.0) / (pow(2.0, 15.0) -1);
		if (sig[i] < -1.0){
			sig[i] = -1.0;
		}		
	}
	cout << "Input file loaded" << endl;
	return sig;
}


void wavWriter(float *outputSig, int sigSize, char *outputFile){
	int chunkSize = channels * sigSize * (bitsPerSample / 8);
	firstSubChunkSize = 16;
	wavSize = sigSize * 2;
	
	char *chunkId = "RIFF";
	char *wavFormat = "WAVE";
	ofstream out(outputFile, ios::out| ios::binary);
	
	out.write(chunkId, 4);
	cout << "test1" << endl;
	out.write((char*) &chunkSize, 4);
	chunkSize = chunkSize - 36;
	out.write(wavFormat, 4);
	out.write(firstSubChunkId, 4);
	out.write((char*) &firstSubChunkSize, 4);
	out.write((char*) &aFormat, 2);
	out.write((char*) &channels, 2);
	out.write((char*) &sampleRate, 4);
	out.write((char*) &byteRate, 4);
	out.write((char*) &blockAlignment, 2);
	out.write((char*) &bitsPerSample, 2);
	out.write(secondSubChunkId, 4);
	cout << "test2" << endl;
	out.write((char*)&wavSize, 4);
	
	
	int16_t sample;
	for (int i = 0; i < sigSize; i++){
		sample = (int16_t)(outputSig[i] * (pow(2.0, 15.0) - 1));
		out.write((char*)&sample, 2);
	
	}
	
	out.close();
	
	
	
}

/*****************************************************************************
*
*    Function:     convolve
*
*    Description:  Convolves two signals, producing an output signal.
*                  The convolution is done in the time domain using the
*                  "Input Side Algorithm" (see Smith, p. 112-115).
*
*    Parameters:   x[] is the signal to be convolved
*                  N is the number of samples in the vector x[]
*                  h[] is the impulse response, which is convolved with x[]
*                  M is the number of samples in the vector h[]
*                  y[] is the output signal, the result of the convolution
*                  P is the number of samples in the vector y[].  P must
*                       equal N + M - 1
*
*	Source:			Leonard Manzara's convolution demo program
*****************************************************************************/
void convolve(float x[], int N, float h[], int M, float y[], int P)
{
	int n, m;
	 cout << "START ALREADY" << endl;
	 /*  Make sure the output buffer is the right size: P = N + M - 1  */
	if (P != (N + M - 1)) {
		printf("Output signal vector is the wrong size\n");
		printf("It is %-d, but should be %-d\n", P, (N + M - 1));
		printf("Aborting convolution\n");
		return;
	}
	
	/*  Clear the output buffer y[] to all zero values  */  
	for (n = 0; n < P; n++)
		y[n] = 0.0;
		
	  /*  Do the convolution  */
  /*  Outer loop:  process each input value x[n] in turn  */	
	for (n = 0; n < N; n++) {
		cout << "Cycle " << n << " out of " << N << "\n" << endl;
		/*  Inner loop:  process x[n] with each sample of h[]  */
		for (m = 0; m < M; m++)
			y[n+m] += x[n] * h[m];
	}
}

void scale(float *outputSignal, int size){
	float minVal = 0, maxVal = 0;
 	for(int i = 0; i < size; i++)
 	{
 		if(outputSignal[i] > maxVal)
			maxVal = outputSignal[i];
		if(outputSignal[i] < minVal)
			minVal = outputSignal[i];
 	}
 
 	minVal = minVal * -1;
 	if(minVal > maxVal)
 		maxVal = minVal;
 	for(int i = 0; i < size; i++)
 	{
 		outputSignal[i] = outputSignal[i] / maxVal;
 	}
}





int main(int argc, char* args[]){
	//std::clock_t startTime;
	//startTime = std::clock();
	if (argc != 4){
		cout << "Usage: convolve.exe <input wav> <impulsve wav> <output wav>\n" << endl;
		return 1;
	}
	cout << "HELLO" << endl;
	
	char *inputFileName 	= args[1];
	char *irFileName 		= args[2];
//	char *outputFileName	= args[3];
	
	
	int inputSize;
	int irSize;
	
	
	cout << "Reading Input file\n" << endl;
	float *inputFileSig = wavReader(inputFileSig, inputFileName, &wavSize);
	inputSize = wavSize;
	cout << "Input size " << inputSize << " \n" << endl;
	
	cout << "Reading IR file\n" << endl;
	float *irFileSig = wavReader(irFileSig, irFileName, &wavSize);
	irSize = wavSize;
	cout << "IR SIZE " << irSize << " \n" << endl;
	int outputSize = inputSize + irSize - 1;
	float *outputFileSig = new float[outputSize];
	
	cout << "Convolving data....\n" << endl;
	cout << "ALMOST START" << endl;
	convolve(inputFileSig, inputSize, irFileSig, irSize, outputFileSig, outputSize);
	cout << "Convolution Complete\n" << endl;
	
	cout << "Scaling wav File\n" << endl;
	
	scale(outputFileSig, outputSize);
	cout << "Saving Output file\n" << endl;
	
	wavWriter(outputFileSig, outputSize, args[3]);
	
	//double timeElapsed = (clock() - startTime ) / (double) CLOCKS_PER_SEC;
	
	//cout << "Program Terminated with a run time of " << timeElapsed << "\n" << endl;
	
	return 0;
}

