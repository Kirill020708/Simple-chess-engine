// NNUE

#pragma once

#ifndef DECLARS
#define DECLARS

#include "declars.h"

#endif /* DECLARS */

#define INCBIN_SILENCE_BITCODE_WARNING
#include "incbin.h"
INCBIN(NETWORK, "code/quantised256.bin");

const int inputSize = 64 * 12, hiddenLayerSize = 256;
const int QA = 255, QB = 64, SCALE = 400;

struct NNUEevaluator {

    bool initialized = false;
    __int16_t w0[inputSize][hiddenLayerSize], b0[hiddenLayerSize];
    __int16_t w1[hiddenLayerSize * 2], b1;

    __int16_t hlSumW[hiddenLayerSize], hlSumB[hiddenLayerSize];

    __int16_t hlSumWStack[maxDepth][hiddenLayerSize], hlSumBStack[maxDepth][hiddenLayerSize];

    int curDepth = 0, lastCalculatedState = 0;

    int diffsW[maxDepth][4], diffsB[maxDepth][4], diffsIter[maxDepth];

    NNUEevaluator() {
    	curDepth = 0, lastCalculatedState = 0;

        for (int i = 0; i < maxDepth; i++){
        	diffsIter[i] = 0;
        	for(int j = 0; j < 4; j++)
        		diffsW[i][j] = diffsB[i][j] = 0;
        	for(int j = 0; j < hiddenLayerSize; j++)
        		hlSumWStack[i][j] = hlSumBStack[i][j] = 0;
        }

        for (int i = 0; i < hiddenLayerSize; i++)
            hlSumW[i] = hlSumB[i] = hlSumWStack[0][i] = hlSumBStack[0][i] = b0[i];
    }

    void clear() {
    	curDepth = 0, lastCalculatedState = 0;

        for (int i = 0; i < maxDepth; i++){
        	diffsIter[i] = 0;
        	for(int j = 0; j < 4; j++)
        		diffsW[i][j] = diffsB[i][j] = 0;
        	for(int j = 0; j < hiddenLayerSize; j++)
        		hlSumWStack[i][j] = hlSumBStack[i][j] = 0;

        for (int i = 0; i < hiddenLayerSize; i++)
            hlSumW[i] = hlSumB[i] = hlSumWStack[0][i] = hlSumBStack[0][i] = b0[i];
        }
    }

    void makeCopy() {
    	if (lastCalculatedState > curDepth)
    		lastCalculatedState = curDepth;

    	diffsIter[curDepth + 1] = 0;
    }

    void makeCopyLayer(int to) {

        for (int i = 0; i < hiddenLayerSize; i += 16) {

            _mm256_storeu_si256((__m256i *)&hlSumWStack[to][i], _mm256_loadu_si256((__m256i *)&hlSumWStack[to - 1][i]));

            _mm256_storeu_si256((__m256i *)&hlSumBStack[to][i], _mm256_loadu_si256((__m256i *)&hlSumBStack[to - 1][i]));
        }
    }

    void set0(pair<int, int> neuronIdx){
    	int i = diffsIter[curDepth + 1];
    	diffsW[curDepth + 1][i] = -neuronIdx.F - 1;
    	diffsB[curDepth + 1][i] = -neuronIdx.S - 1;
    	diffsIter[curDepth + 1]++;
    }

    void set1(pair<int, int> neuronIdx){
    	int i = diffsIter[curDepth + 1];
    	diffsW[curDepth + 1][i] = neuronIdx.F + 1;
    	diffsB[curDepth + 1][i] = neuronIdx.S + 1;
    	diffsIter[curDepth + 1]++;
    }

    void set0calc(int layerIdx, int idxW, int idxB) {
        for (int i = 0; i < hiddenLayerSize; i += 16) {

            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxW][i])));

            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxB][i])));

            // hlSumW[i]-=w0[neuronIdx.F][i];
            // hlSumB[i]-=w0[neuronIdx.S][i];
        }
    }

    void set1calc(int layerIdx, int idxW, int idxB) {
        // cout<<neuronIdx.F<<' '<<neuronIdx.S<<'\n';
        for (int i = 0; i < hiddenLayerSize; i += 16) {

            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxW][i])));

            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxB][i])));

            // hlSumW[i]+=w0[neuronIdx.F][i];
            // hlSumB[i]+=w0[neuronIdx.S][i];
        }
    }

    void set1init(int layerIdx, int idxW, int idxB) {
        for (int i = 0; i < hiddenLayerSize; i += 16) {

            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxW][i])));

            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxB][i])));

        }
    }

    void sortUpdLayer(int i){
    	for(int j = 1; j < diffsIter[i]; j++) {
    		int k = j;
    		while(k >= 1 && diffsW[i][k] < 0 && diffsW[i][k - 1] > 0) {
    			swap(diffsW[i][k], diffsW[i][k - 1]);
    			swap(diffsB[i][k], diffsB[i][k - 1]);
    			k--;
    		}
    	}
    }

    void set(int layerIdx) {
    	
        for (int i = 0; i < hiddenLayerSize; i += 16) {
            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i], _mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx - 1][i]));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i], _mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx - 1][i]));
        }
    }

    void set01(int layerIdx, int idxW0, int idxB0, int idxW1, int idxB1) {
    	
        for (int i = 0; i < hiddenLayerSize; i += 16) {
            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i], _mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx - 1][i]));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i], _mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx - 1][i]));
}       for (int i = 0; i < hiddenLayerSize; i += 16) {
            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxW0][i])));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxB0][i])));
}       for (int i = 0; i < hiddenLayerSize; i += 16) {
            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxW1][i])));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxB1][i])));

        }
    }

    void set001(int layerIdx, int idxW0, int idxB0, int idxW00, int idxB00, int idxW1, int idxB1) {
    	
        for (int i = 0; i < hiddenLayerSize; i += 16) {
            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i], _mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx - 1][i]));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i], _mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx - 1][i]));
}       for (int i = 0; i < hiddenLayerSize; i += 16) {
            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxW0][i])));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxB0][i])));
}       for (int i = 0; i < hiddenLayerSize; i += 16) {
            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxW00][i])));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxB00][i])));
}       for (int i = 0; i < hiddenLayerSize; i += 16) {
            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxW1][i])));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxB1][i])));

        }
    }

    void set0011(int layerIdx, int idxW0, int idxB0, int idxW00, int idxB00, int idxW1, int idxB1, int idxW11, int idxB11) {
    	
        for (int i = 0; i < hiddenLayerSize; i += 16) {
            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i], _mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx - 1][i]));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i], _mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx - 1][i]));

            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxW0][i])));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxB0][i])));

            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxW00][i])));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i],
                                _mm256_sub_epi16(_mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxB00][i])));

            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxW1][i])));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxB1][i])));

            _mm256_storeu_si256((__m256i *)&hlSumWStack[layerIdx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumWStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxW11][i])));
            _mm256_storeu_si256((__m256i *)&hlSumBStack[layerIdx][i],
                                _mm256_add_epi16(_mm256_loadu_si256((__m256i *)&hlSumBStack[layerIdx][i]),
                                                 _mm256_loadu_si256((__m256i *)&w0[idxB11][i])));

        }
    }

    void recalculateAccumulators() {
    	/*
		Possible upd types (sz = diffsIter[i]):
		sz == 0: no upd
		sz == 2: [set0, set1] (simple move)
		sz == 3: [set0, set0, set1] (capture)
		sz == 4: [set0, set0, set1, set1] (castling)
    	*/

    	for (int i = lastCalculatedState + 1; i <= curDepth; i++) {
    		sortUpdLayer(i);

    		int sz = diffsIter[i];

    		if(sz == 0)
    			set(i);

    		if(sz == 2)
    			set01(i, -(diffsW[i][0] + 1), -(diffsB[i][0] + 1), diffsW[i][1] - 1, diffsB[i][1] - 1);

    		if(sz == 3)
    			set001(i, -(diffsW[i][0] + 1), -(diffsB[i][0] + 1), -(diffsW[i][1] + 1), -(diffsB[i][1] + 1), diffsW[i][2] - 1, diffsB[i][2] - 1);

    		if(sz == 4)
    			set0011(i, -(diffsW[i][0] + 1), -(diffsB[i][0] + 1), -(diffsW[i][1] + 1), -(diffsB[i][1] + 1), diffsW[i][2] - 1, diffsB[i][2] - 1, diffsW[i][3] - 1, diffsB[i][3] - 1);

    		// makeCopyLayer(i);
    		// for (int j = 0; j < diffsIter[i]; j++) {
    		// 	if (diffsW[i][j] < 0)
    		// 		set0calc(i, -(diffsW[i][j] + 1), -(diffsB[i][j] + 1));
    		// 	else
    		// 		set1calc(i, diffsW[i][j] - 1, diffsB[i][j] - 1);
    		// }
    	}
    	lastCalculatedState = curDepth;
    }

    int screlu(int x) {
        x = clamp(x, 0, QA);
        return x * x;
    }

    void printAccum() {
        for (ll i = 0; i < hiddenLayerSize; i++)
            cout << hlSumW[i] << ' ';
        cout << '\n';
        for (ll i = 0; i < hiddenLayerSize; i++)
            cout << hlSumB[i] << ' ';
        cout << '\n';
    }

    int evaluate(int color) {
    	recalculateAccumulators();

        int output = 0;

        __m256i outputV = _mm256_setzero_si256();

        __m256i zerosm = _mm256_set1_epi16(-1);
        __m256i qas = _mm256_set1_epi16(QA);

        // for(int i=0;i<hiddenLayerSize;i++)
        // 	cout<<hlSumWStack[curDepth][i]<<' ';
        // cout<<"\n";
        // for(int i=0;i<hiddenLayerSize;i++)
        // 	cout<<hlSumBStack[curDepth][i]<<' ';
        // cout<<"\n\n";

        for (int i = 0; i < hiddenLayerSize; i += 16) {
            __m256i hl = _mm256_loadu_si256((__m256i *)&hlSumWStack[curDepth][i]);
            hl = _mm256_and_si256(hl, _mm256_cmpgt_epi16(hl, zerosm));
            hl = _mm256_blendv_epi8(hl, qas, _mm256_cmpgt_epi16(hl, qas));
            __m256i hl0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 0));
            hl0 = _mm256_mullo_epi32(hl0, hl0);
            __m256i hl1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 1));
            hl1 = _mm256_mullo_epi32(hl1, hl1);
            __m256i w1v = _mm256_loadu_si256((__m256i *)&w1[i + hiddenLayerSize * (color == BLACK)]);
            outputV = _mm256_add_epi32(
                outputV, _mm256_mullo_epi32(hl0, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 0))));
            outputV = _mm256_add_epi32(
                outputV, _mm256_mullo_epi32(hl1, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 1))));

            hl = _mm256_loadu_si256((__m256i *)&hlSumBStack[curDepth][i]);
            hl = _mm256_and_si256(hl, _mm256_cmpgt_epi16(hl, zerosm));
            hl = _mm256_blendv_epi8(hl, qas, _mm256_cmpgt_epi16(hl, qas));
            hl0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 0));
            hl0 = _mm256_mullo_epi32(hl0, hl0);
            hl1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(hl, 1));
            hl1 = _mm256_mullo_epi32(hl1, hl1);
            w1v = _mm256_loadu_si256((__m256i *)&w1[i + hiddenLayerSize * (color == WHITE)]);
            outputV = _mm256_add_epi32(
                outputV, _mm256_mullo_epi32(hl0, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 0))));
            outputV = _mm256_add_epi32(
                outputV, _mm256_mullo_epi32(hl1, _mm256_cvtepi16_epi32(_mm256_extracti128_si256(w1v, 1))));

            // output+=screlu(hlSumWStack[curDepth][i])*w1[i];
            // output+=screlu(hlSumBStack[curDepth][i])*w1[i+hiddenLayerSize];
        }

        __m256i hadd1 = _mm256_hadd_epi32(outputV, outputV);
        __m256i hadd2 = _mm256_hadd_epi32(hadd1, hadd1);
        __m128i sum128 = _mm_add_epi32(_mm256_castsi256_si128(hadd2), _mm256_extractf128_si256(hadd2, 1));
        output = _mm_extract_epi32(sum128, 0);

        output /= QA;
        output += b1;
        output *= SCALE;
        output /= (QA * QB);
        return output;
    }

    void initFromFile(string path) {
        // ifstream file(path,ios::binary);

        // if(!file){
        // 	cout<<"Failed to open NNUE file\n";
        // 	return;
        // }

        initialized = true;

        vector<int16_t> data;
        size_t file_size = gNETWORKSize;
        size_t num_elements = file_size / sizeof(int16_t);

        // cout<<num_elements<<endl;
        data.resize(num_elements);
        // file.read(reinterpret_cast<char*>(data.data()), file_size);

        const int16_t *src_data = reinterpret_cast<const int16_t *>(gNETWORKData);
        std::copy(src_data, src_data + num_elements, data.begin());

        int iter = 0;
        for (int i = 0; i < inputSize; i++)
            for (int j = 0; j < hiddenLayerSize; j++)
                w0[i][j] = data[iter++];

        for (int j = 0; j < hiddenLayerSize; j++) {
            b0[j] = data[iter++];
            // cout<<b0[j]<<' ';
        }
        // cout<<'\n';

        for (int j = 0; j < hiddenLayerSize * 2; j++)
            w1[j] = data[iter++];

        b1 = data[iter++];
        // cout<<b1<<'\n';

        clear();
    }
};

NNUEevaluator mainNnueEvaluator;