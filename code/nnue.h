// NNUE

#pragma once


#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

const int inputSize=64*12,hiddenLayerSize=32;
const int QA=255,QB=64,SCALE=400;

struct NNUEevaluator{

	bool initialized=false;
	int w0[inputSize][hiddenLayerSize],b0[hiddenLayerSize];
	int w1[hiddenLayerSize*2],b1;

	int hlSumW[hiddenLayerSize],hlSumB[hiddenLayerSize];

	NNUEevaluator(){
		for(int i=0;i<hiddenLayerSize;i++)
			hlSumW[i]=hlSumB[i]=b0[i];
	}

	void clear(){
		for(int i=0;i<hiddenLayerSize;i++)
			hlSumW[i]=hlSumB[i]=b0[i];
	}

	void set0(pair<int,int>neuronIdx){
		for(int i=0;i<hiddenLayerSize;i+=1){

			// _mm256_storeu_si256((__m256i*)&hlSumW[i], 
			// 	_mm256_sub_epi16(_mm256_loadu_si256((__m256i*)&hlSumW[i]),
			// 					 _mm256_loadu_si256((__m256i*)&w0[neuronIdx.F][i])));


			// _mm256_storeu_si256((__m256i*)&hlSumB[i], 
			// 	_mm256_sub_epi16(_mm256_loadu_si256((__m256i*)&hlSumB[i]),
			// 					 _mm256_loadu_si256((__m256i*)&w0[neuronIdx.S][i])));

			hlSumW[i]-=w0[neuronIdx.F][i];
			hlSumB[i]-=w0[neuronIdx.S][i];
		}
	}

	void set1(pair<int,int>neuronIdx){
		// cout<<neuronIdx.F<<' '<<neuronIdx.S<<'\n';
		for(int i=0;i<hiddenLayerSize;i++){
			hlSumW[i]+=w0[neuronIdx.F][i];
			hlSumB[i]+=w0[neuronIdx.S][i];
		}
		// for(ll i=0;i<hiddenLayerSize;i++)
		// 	cout<<hlSumW[i]<<' ';
		// cout<<'\n';
		// for(ll i=0;i<hiddenLayerSize;i++)
		// 	cout<<hlSumB[i]<<' ';
		// cout<<'\n';
	}

	int screlu(int x){
		x=clamp(x,0,QA);
		return x*x;
	}

	void printAccum(){
		for(ll i=0;i<hiddenLayerSize;i++)
			cout<<hlSumW[i]<<' ';
		cout<<'\n';
		for(ll i=0;i<hiddenLayerSize;i++)
			cout<<hlSumB[i]<<' ';
		cout<<'\n';
	}

	int evaluate(int color){
		int output=0;

		if(color==WHITE){
			for(int i=0;i<hiddenLayerSize;i++){
				output+=screlu(hlSumW[i])*w1[i];
				output+=screlu(hlSumB[i])*w1[i+hiddenLayerSize];
			}
		}else{
			for(int i=0;i<hiddenLayerSize;i++){
				output+=screlu(hlSumB[i])*w1[i];
				output+=screlu(hlSumW[i])*w1[i+hiddenLayerSize];
			}
		}

		output/=QA;
		output+=b1;
		output*=SCALE;
		output/=(QA*QB);
		return output;
	}

	void initFromFile(string path){
		ifstream file(path,ios::binary);

		if(!file){
			cout<<"Failed to open NNUE file\n";
			return;
		}

		initialized=true;

		vector<int16_t> data;
		file.seekg(0, ios::end);
		size_t file_size = file.tellg();
		file.seekg(0, ios::beg);
		size_t num_elements = file_size / sizeof(int16_t);
		// cout<<num_elements<<'\n';
    	data.resize(num_elements);
    	file.read(reinterpret_cast<char*>(data.data()), file_size);

    	int iter=0;
    	for(int i=0;i<inputSize;i++)
    		for(int j=0;j<hiddenLayerSize;j++)
    			w0[i][j]=data[iter++];

		for(int j=0;j<hiddenLayerSize;j++){
			b0[j]=data[iter++];
			// cout<<b0[j]<<' ';
		}
		// cout<<'\n';

		for(int j=0;j<hiddenLayerSize*2;j++)
			w1[j]=data[iter++];

		b1=data[iter++];
		// cout<<b1<<'\n';

		clear();
	}
};

NNUEevaluator nnueEvaluator;