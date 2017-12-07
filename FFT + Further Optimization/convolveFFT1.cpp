#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <ctime>

#define SWAP(a,b)  tempr=(a);(a)=(b);(b)=tempr

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
void four1(float data[], int nn, int isign);
void four1Scale (float signal[], int N);
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
		sig[i] = (sam*1.0) / (32767);
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

	out.write((char*)&wavSize, 4);
	
	
	int16_t sample;
	for (int i = 0; i < sigSize; i++){
		sample = (int16_t)(outputSig[i] * (pow(2.0, 15.0) - 1));
		out.write((char*)&sample, 2);
	
	}
	
	out.close();
	
	
	
}

// From notes in class
void four1Scale (float signal[], int N)
{
	int i;
	int j;
	for (i = 0, j = 0; i < N; i++, j+=4) {
		signal[j] /= (float)N;
		signal[j+1] /= (float)N;
		signal[j+2] /= (float)N;
		signal[j+3] /= (float)N;
	}
}




//  The four1 FFT from Numerical Recipes in C,
//  p. 507 - 508.
//  Note:  changed float data types to double.
//  nn must be a power of 2, and use +1 for
//  isign for an FFT, and -1 for the Inverse FFT.
//  The data is complex, so the array size must be
//  nn*2. This code assumes the array starts
//  at index 1, not 0, so subtract 1 when
//  calling the routine (see main() below).
//	Sourced from 501 course slides on d2l
// 	I Changed the first parameter to float instead of double
void four1(float data[], int nn, int isign)
{
    unsigned long n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta;
    double tempr, tempi;

    n = nn << 1;
    j = 1;

    for (i = 1; i < n; i += 2) {
	if (j > i) {
	    SWAP(data[j], data[i]);
	    SWAP(data[j+1], data[i+1]);
	}
	m = nn;
	while (m >= 2 && j > m) {
	    j -= m;
	    m >>= 1;
	}
	j += m;
    }

    mmax = 2;
    while (n > mmax) {
	istep = mmax << 1;
	theta = isign * (6.28318530717959 / mmax);
	wtemp = sin(0.5 * theta);
	wpr = -2.0 * wtemp * wtemp;
	wpi = sin(theta);
	wr = 1.0;
	wi = 0.0;
	for (m = 1; m < mmax; m += 2) {
	    for (i = m; i <= n; i += istep) {
		j = i + mmax;
		tempr = wr * data[j] - wi * data[j+1];
		tempi = wr * data[j+1] + wi * data[j];
		data[j] = data[i] - tempr;
		data[j+1] = data[i+1] - tempi;
		data[i] += tempr;
		data[i+1] += tempi;
	    }
	    wr = (wtemp = wr) * wpr - wi * wpi + wr;
	    wi = wi * wpr + wtemp * wpi + wi;
	}
	mmax = istep;
    }
}




void convolve(float x[], int N, float h[], int M, float y[], int P) 
{
	int i = 0;
	int newArrSize = 1;
	float *newInput = NULL;
	float *newIR = NULL;
	float *newOutput = NULL;
	
	//Keep doubling array size
	
	while (newArrSize < P) {
		newArrSize *= 2;
	}
	
	
	newInput = new float[2 * newArrSize];
	for (i = 0; i < (N * 2); i+=2) {
		newInput[i] = x[i/2];
		newInput[i+1] = 0;
	}
	while (i < newArrSize){
		newInput[i] = 0;
		newInput[i+1] = 0;
		i+=2;
	}	
	
	
	
	newIR = new float[2 * newArrSize];
	for (i = 0; i < (M * 2); i+=2) {
		newIR[i] = h[i/2];
		newIR[i+1] = 0;
	}
	while (i < newArrSize){
		newIR[i] = 0;
		newIR[i+1] = 0;
		i+=2;
	}
	
	newOutput = new float[2 * newArrSize];
	for (i = 0; i < newArrSize; i+=2) {
		newOutput[i] = 0;
		newOutput[i+1] = 0;
	}
	four1((newIR - 1), newArrSize, 1);
	four1((newInput - 1), newArrSize, 1);

	// multiplying complex  numbers
	for (i = 0; i < (newArrSize * 2); i+=2) {
		newOutput[i] = (newInput[i] * newIR[i]) - (newInput[i+1] * newIR[i+1]);
		newOutput[i+1] = (newInput[i+1] * newIR[i]) + (newInput[i] * newIR[i+1]);
	}

	// Inverse FFT
	four1((newOutput - 1), newArrSize, -1);
	
	four1Scale(newOutput, newArrSize);
	
	for (i = 0; i < P; i++) {
		y[i] = newOutput[i*2];
	}
}


int main(int argc, char* args[]){
	std::clock_t startTime = std::clock();
	if (argc != 4){
		cout << "Usage: convolve.exe <input wav> <impulse wav> <output wav>\n" << endl;
		return 1;
	}
	
	char *inputFileName 	= args[1];
	char *irFileName 		= args[2];
	
	
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
	convolve(inputFileSig, inputSize, irFileSig, irSize, outputFileSig, outputSize);
	cout << "Convolution Complete\n" << endl;
	
	scale(outputFileSig, outputSize);
	wavWriter(outputFileSig, outputSize, args[3]);
	
	double timeElapsed = (std::clock() - startTime ) / (double) CLOCKS_PER_SEC;
	
	cout << "Program Terminated with a run time of " << timeElapsed << "\n" << endl;
	
	return 0;
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



