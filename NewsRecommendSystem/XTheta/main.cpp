#include<iostream>
#include<fstream>
#include<time.h>
#include<math.h>
#define NUM_NEWS 6183
#define NUM_USER 10000
#define NUM_FEATURE 10
#define nDim (((NUM_USER)+(NUM_NEWS))*(NUM_FEATURE))
#define LAMBDA 10
#define COST_FUNC_DATATYPE float

using namespace std;

int fmincg(void(*costFunc)(COST_FUNC_DATATYPE*, COST_FUNC_DATATYPE*, COST_FUNC_DATATYPE*), COST_FUNC_DATATYPE* xVector, int maxCostFuncCalls, int dim);
int** Y;

//fucntion for loading traing data: uid_nid_train.txt
int loaddata(const char* trainfile, int** Y){
	ifstream fp(trainfile);
	if (fp.fail())
	{
		cout<<"Cannot open file!"<<endl;
		return 0;
	}
	int i, j;
	for (i = 0; i < NUM_USER; i++)//row:user; col:news
		for (j = 0; j < NUM_NEWS; j++)
			Y[i][j] = 0;
	while (!fp.eof()){
		fp >> i >> j;
		Y[i][j] = 1;
	}
	fp.close();
	return 1;
}

void initParam(float* vec){//X: attribute of news; Theta:attribute of user
	srand((unsigned)time(0));
	for (int i = 0; i < nDim; i++){
		vec[i] = rand() / (float)(RAND_MAX);
	}
}

inline float sigmoid(float x){
	return 1 / (1 + exp(-1 * x));
}
inline float logcost(int y, float x){
	if (x == 0){
		return (y == 0 ? 0 : 1e15);
	}
	if (x == 1){
		return (y == 1 ? 0 : 1e15);
	}
	return (-y*log(x) - (1 - y)*log(1 - x));
}

void costfunction(float* vec, float* cost, float* grad)
{
	int i, j, k, kk;
	float cost_y = 0.;
	float cost_x = 0.;
	float cost_theta = 0.;
	float tmp = 0.;
	int sizeofX = NUM_NEWS*NUM_FEATURE;

	//calculate 1st part of CostFuction
	for (i = 0; i<NUM_NEWS; i++){
		for (j = 0; j<NUM_USER; j++){
			for (k = 0; k<NUM_FEATURE; k++){
				tmp = tmp + vec[i*NUM_FEATURE + k] * vec[sizeofX + j*NUM_FEATURE + k];
			}
			cost_y = cost_y + logcost(Y[j][i], sigmoid(tmp));
			tmp = 0;
		}
	}
	//calculate cost of X part
	for (i = 0; i<NUM_NEWS; i++){
		for (j = 0; j<NUM_FEATURE; j++){
			cost_x = cost_x + vec[i*NUM_FEATURE + j] * vec[i*NUM_FEATURE + j];
		}
	}
	//calculate cost of Theta part
	for (i = 0; i<NUM_USER; i++){
		for (j = 0; j<NUM_FEATURE; j++){
			cost_theta = cost_theta + vec[sizeofX + i*NUM_FEATURE + j] * vec[sizeofX + i*NUM_FEATURE + j];
		}
	}
	*cost = cost_y / 2 + (cost_x + cost_theta)*LAMBDA / 2;

	//calculate gradient of X
	for (i = 0; i<NUM_NEWS; i++){
		for (kk = 0; kk<NUM_FEATURE; kk++){
			grad[i*NUM_FEATURE + kk] = LAMBDA*vec[i*NUM_FEATURE + kk];
			for (j = 0; j<NUM_USER; j++){
				for (k = 0; k<NUM_FEATURE; k++){
					tmp = tmp + vec[i*NUM_FEATURE + k] * vec[sizeofX + j*NUM_FEATURE + k];
				}
				grad[i*NUM_FEATURE + kk] += (sigmoid(tmp) - Y[j][i])*vec[sizeofX + j*NUM_FEATURE + kk];
				tmp = 0.0;
			}
		}
	}
	//calculate gradient of Theta
	for (j = 0; j<NUM_USER; j++){
		for (kk = 0; kk<NUM_FEATURE; kk++){
			grad[sizeofX + j*NUM_FEATURE + kk] = LAMBDA*vec[sizeofX + j*NUM_FEATURE + kk];
			for (i = 0; i<NUM_NEWS; i++){
				for (k = 0; k<NUM_FEATURE; k++){
					tmp = tmp + vec[i*NUM_FEATURE + k] * vec[sizeofX + j*NUM_FEATURE + k];
				}
				grad[sizeofX + j*NUM_FEATURE + kk] += (sigmoid(tmp) - Y[j][i])*vec[i*NUM_FEATURE + kk];
				tmp = 0.0;
			}
		}
	}
}

void getResult(float* vec, int** R){
	int sx = NUM_NEWS*NUM_FEATURE;
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS; j++){
			float tmp = 0.;
			for (int k = 0; k < NUM_FEATURE; k++){
				tmp += vec[sx + i*NUM_FEATURE + k] * vec[j*NUM_FEATURE + k];
			}
			R[i][j] = int(sigmoid(tmp)*10000);	
		}
	}
}

void rmReadNews(int** Y, int** R){
	int i, j;
	for (i = 0; i < NUM_USER; i++){
		for (j = 0; j < NUM_NEWS; j++){
			if (Y[i][j] != 0)
				R[i][j] = 0;
		}
	}
}

void outputMatrix(const char* file, int** R){
	ofstream fp(file);
	if (fp.fail()){
		cout << "open file failed!" << endl;
		return;
	}
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS - 1; j++){
			fp << R[i][j] << ',';
		}
		fp<<R[i][NUM_NEWS - 1]<<"\n";
	}
	fp.close();
}

void topRecommendation(int** R, int* tr){
	int i, j;
	for (i = 0; i < NUM_USER; i++){
		int max = 0, idx = 0;
		for (j = 0; j < NUM_NEWS; j++){
			if (R[i][j] > max){
				max = R[i][j];
				idx = j;
			}
		}
		tr[i] = idx;
	}
}

void testRecommendation(const char* testfile, int* tr){
	ifstream fp(testfile);
	int uid, nid;
	int tp = 0;
	if (fp.fail())
	{
		printf("Cannot open file!\n");
		return;
	}
	while (!fp.eof()){
		fp >> uid >> nid;
		if (tr[uid] == nid)
			tp++;
	}
	cout << "true positive: " << tp << endl;
	fp.close();
}
void loadMatrix(const char* matrixfile, int** R){
	ifstream fp(matrixfile);
	char ch;
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS-1; j++){
			fp >> R[i][j];
			fp >> ch;
		}
		fp >> R[i][NUM_NEWS - 1];
	}
}

int main(){
	Y=new int*[NUM_USER];
	for (int i = 0; i < NUM_USER; i++){
		Y[i] = new int[NUM_NEWS];
	}
	loaddata("../data/remapped_uid_nid_train.txt",Y);
	cout << "load data Ok!" << endl;

	float* vec = new float[nDim];
	initParam(vec);
	cout << "initialize parameter Ok!" << endl;

	//fmincg(&costfunction, vec, 1000, nDim);
	//cout << "training data OK!" << endl;

	int** R = new int*[NUM_USER];
	for (int i = 0; i < NUM_USER; i++){
		R[i] = new int[NUM_NEWS];
	}
	getResult(vec, R);
	delete vec;
	rmReadNews(Y, R);
	cout << "generate result matrix Ok!" << endl;

	//outputMatrix("../data/matrix_svd.txt", R);
	//cout << "output matrix OK!" << endl;

	//loadMatrix("../data/matrix_svd.txt", R);

	int* tr = new int[NUM_USER];
	topRecommendation(R, tr);
	testRecommendation("../data/remapped_uid_nid_test.txt", tr);

	for (int i = 0; i < NUM_USER; i++){
		delete R[i];
	}
	delete R;
	for (int i = 0; i < NUM_USER; i++){
		delete Y[i];
	}
	delete Y;
	getchar();
	return 1;
}