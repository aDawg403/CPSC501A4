#ifndef wavLd
#define wavLd

class wavLd{
		
	public:
		int 	chunkSize;
		int 	subChunkSize;
		short 	format;
		short 	channels;
		int 	sampleRate;
		int 	byteRate;
		short 	blockAlign;
		short 	bitsPerSample;
		int 	dataSize;
		short 	signal;
		int 	signalSize;
		char* 	data;
		
	public: 
		void readWav(char *fileName);

};
#endif
