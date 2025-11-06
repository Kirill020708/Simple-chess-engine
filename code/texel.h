#ifndef EVALUATOR
#define EVALUATOR

#include "evaluation.h"

#endif /* EVALUATOR */

#ifndef SEARCHER
#define SEARCHER

#include "searcher.h"

#endif /* SEARCHER */



float rngF(float l,float r){
	return (rng()%int(1e6))/1e6*(r-l)-l;
}

struct TexelTuner{
	float delta=1;
	const float K=1.6;
	float rate=500;

	float sigmoid(float ev){
		return 1.0/(1+exp(-K*ev/400));
	}

	float err=0;
	string curFen;


	float fitOneParam(float result,int i,float eval){
		vector<float>weights=evaluator.writeToVector();
		int n=weights.size();
		// float eval=searcher.texelSearch(board.boardColor,-inf*10,inf*10,0);
		// if(board.boardColor==BLACK)
		// 	eval=-eval;
		float error=pow(sigmoid(eval)-result,2);
		// err+=error;
		weights[i]+=delta;
		evaluator.initFromVector(weights);

		float newEval=evaluator.evaluatePositionDeterministic();
		// float newEval=searcher.texelSearch(board.boardColor,-inf*10,inf*10,0);
		// if(board.boardColor==BLACK)
		// 	newEval=-newEval;

		float newError=pow(sigmoid(newEval)-result,2);
		weights[i]-=delta;
		float grad=(newError-error)/delta;
		// if(abs(grad)>0)
		// 	grad=grad/abs(grad);
		evaluator.initFromVector(weights);
		return grad;
	}

	vector<float> fit(float result){
		vector<float>weights=evaluator.writeToVector();
		int n=weights.size();
		// float eval=searcher.texelSearch(board.boardColor,-inf*10,inf*10,0);
		// if(board.boardColor==BLACK)
		// 	eval=-eval;
		float eval=evaluator.evaluatePositionDeterministic();
		// if(abs(eval)>=1e3)
		// 	return vector<float>(n);
		float error=pow(sigmoid(eval)-result,2);
		// if(error>=100)
		// 	cout<<eval<<' '<<result<<' '<<curFen<<'\n';
		err+=error;
		vector<float>gradient(n);

		// for(int i=0;i<n;i++){
		// 	gradient[i]=rngF(-1/rate,1/rate);
		// 	weights[i]+=gradient[i];
		// }
		// evaluator.initFromVector(weights);
		// float newEval=searcher.texelSearch(board.boardColor,-inf*10,inf*10,0);
		// if(board.boardColor==BLACK)
		// 	newEval=-newEval;
		// float newError=pow(sigmoid(newEval-result),2);
		// float grad=(newError-error)/delta;
		// for(int i=0;i<n;i++){
		// 	weights[i]-=gradient[i];
		// 	gradient[i]*=grad;
		// }

		for(int i=0;i<n;i++)
			gradient[i]=fitOneParam(result,i,eval);

		return gradient;
	}

	void checkData(){

		int nmb=0;
		ifstream in("/Users/Apple/Desktop/projects/chessEngv2/texelBigData.txt");
		ofstream out("/Users/Apple/Desktop/projects/chessEngv2/texelBigGoodData.txt");
		string pos;
		int numberOfPositions=0;
		while(getline(in,pos)){
			numberOfPositions++;
			if(numberOfPositions%100'000==0)
				cout<<numberOfPositions<<' '<<nmb<<'\n';
			string evalS;
			while(pos.back()!=' '){
				evalS.push_back(pos.back());
				pos.pop_back();
			}
			pos.pop_back();
			reverse(all(evalS));
			board.initFromFEN(pos);

			int staticEval=evaluator.evaluatePosition(board.boardColor);
			int qEval=searcher.texelSearch(board.boardColor,-inf*10,inf*10,0);
			if(abs(qEval-staticEval)>=100)
				continue;
			nmb++;

			out<<pos<<' '<<evalS<<'\n';
		}
		cout<<"total pos: "<<nmb<<'\n';
	}

	void train(string path){
		ifstream in(path);
		string pos;
		vector<pair<string,float>>database,databaseValid;
		int numberOfPositions=0;
		while(getline(in,pos)){
			numberOfPositions++;
			if(numberOfPositions%100'000==0)
				cout<<numberOfPositions<<'\n';
			string evalS;
			while(pos.back()!=' '){
				evalS.push_back(pos.back());
				pos.pop_back();
			}
			pos.pop_back();
			reverse(all(evalS));
			if(database.size()<4'800'000)
				database.push_back({pos,stof(evalS)});
			else
				databaseValid.push_back({pos,stof(evalS)});
		}

		string file="/Users/Apple/Desktop/projects/chessEngv2/apps/evalnew.txt";

		vector<float>weights=evaluator.writeToVector();
		int n=weights.size();

		// for(int param=0;param<n;param++){
		// 	if(weights[param]==0)
		// 		continue;
		// 	for(int epoch=1;epoch<=10;epoch++){
		// 		cout<<"Param "<<param+1<<"/"<<n<<" Epoch "<<epoch<<'\n';

		// 		numberOfPositions=0;
		// 		err=0;
		// 		int batchSize=1000;
		// 		for(int i=0;i<database.size();i+=batchSize){
		// 			float grad=0;

		// 			for(int j=i;j<min(int(database.size()),i+batchSize);j++){
		// 				board.initFromFEN(database[j].first);
		// 				curFen=database[j].first;
		// 				auto newGrad=fitOneParam(database[j].second,param);

		// 				grad+=newGrad;

		// 				numberOfPositions++;
		// 			}

		// 			grad/=batchSize;
		// 			weights[param]-=grad*rate;

		// 			evaluator.initFromVector(weights);
		// 			if(numberOfPositions%100'000==0)
		// 				cout<<numberOfPositions<<' '<<(err/numberOfPositions)<<' '<<grad*rate<<'\n';
		// 		}
		// 		cout<<(err/numberOfPositions)<<'\n';
		// 		evaluator.writeToFile("/Users/Apple/Desktop/projects/chessEngv2/apps/evalnew1.txt");
		// 	}
		// }

		vector<float>errors,errValid;

		for(int epoch=1;;epoch++){
			cout<<"Epoch "<<epoch<<'\n';

			numberOfPositions=0;
			err=0;
			int batchSize=10000;
			for(int i=0;i<database.size();i+=batchSize){
				vector<float>weights=evaluator.writeToVector();
				int n=weights.size();
				vector<float>grad(n);
				for(int j=i;j<min(int(database.size()),i+batchSize);j++){
					board.initFromFEN(database[j].first);
					curFen=database[j].first;
					auto newGrad=fit(database[j].second);
					for(int p=0;p<n;p++)
						grad[p]+=newGrad[p];
				}
					numberOfPositions+=batchSize;
				for(int p=0;p<n;p++){
					grad[p]/=batchSize;
					weights[p]-=grad[p]*rate;
					// cout<<weights[p]<<' ';
				}
				// cout<<'\n';
				evaluator.initFromVector(weights);
				if(numberOfPositions%100'00==0)
					cout<<numberOfPositions<<' '<<(err/numberOfPositions)<<' '<<grad[5]*rate<<'\n';
				evaluator.writeToFile(file);
			}
			cout<<(err/numberOfPositions)<<'\n';
			errors.push_back(err/numberOfPositions);
			cout<<'[';
			for(int i=0;i<errors.size();i++){
				cout<<errors[i];
				if(i+1<errors.size())
					cout<<',';
			}
			cout<<"]\n";

			float errV=0;
			for(int i=0;i<databaseValid.size();i++){
				board.initFromFEN(databaseValid[i].first);
				float eval=evaluator.evaluatePositionDeterministic();
				errV+=pow(sigmoid(eval)-databaseValid[i].second,2);
			}
			errV/=databaseValid.size();

			cout<<errV<<'\n';
			errValid.push_back(errV);
			cout<<'[';
			for(int i=0;i<errValid.size();i++){
				cout<<errValid[i];
				if(i+1<errValid.size())
					cout<<',';
			}
			cout<<"]\n";


			evaluator.writeToFile(file);
		}
	}
};

TexelTuner texelTuner;