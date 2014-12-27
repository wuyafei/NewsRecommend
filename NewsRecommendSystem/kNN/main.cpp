#include<iostream>
#include<fstream>
#include<queue>
#include<functional>
using namespace std;

const int NUM_NEWS = 6183;
const int NUM_USER = 10000;
const float ALPHA = 200.;
const float DELTA = 25.0;
const float GAMMA = -2.5;
const float BETA = 86400.;
const int K = 10;

//function for loading training data to matrix Y
int loaddata(const char* trainfile, int** Y){
	ifstream fp(trainfile);
	if (fp.fail())
	{
		cout << "Cannot open training file!" << endl;
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

//function to genearte Co-Occurrence Matrix:M, Y is the rating matrix
void genMatrix(int** Y, int** M){
	int i, j, k;
	for (i = 0; i < NUM_NEWS; i++){
		for (j = 0; j < NUM_NEWS; j++){
			M[i][j] = 0;
		}
	}
	for (i = 0; i < NUM_USER; i++){
		for (j = 0; j < NUM_NEWS; j++){
			if (Y[i][j] != 0){
				M[j][j]++;
				for (k = j + 1; k < NUM_NEWS; k++){
					if (Y[i][k] != 0){
						M[j][k]++;
						M[k][j]++;
					}
				}
			}
		}
	}
}

inline int min(int x, int y){
	return x < y ? x : y;
}
inline float sigmoid(float x){
	return 1 / (1 + exp(-1 * x));
}

//function for generating similarity matrix from co-occureance matrix
void similarityMatrix_t(int** M, float** S, int* tm){
	for (int i = 0; i < NUM_NEWS; i++){
		for (int j = 0; j < NUM_NEWS; j++){
			int delta_t = 0;
			if (tm[i] == 0 || tm[j] == 0)
				delta_t = 150000;
			else
				delta_t = abs(tm[i] - tm[j]);

			int tmp = min(M[i][i], M[j][j]);
			float c = 0.;
			if (tmp != 0)
				c = M[i][j] * M[i][j] / float(tmp) / (M[i][j] + ALPHA);
			S[i][j] = sigmoid(DELTA * c * exp(-1 * delta_t / BETA) + GAMMA);
		}
	}
}

void similarityMatrix(int** M, float** S){
	for (int i = 0; i < NUM_NEWS; i++){
		for (int j = 0; j < NUM_NEWS; j++){
			int tmp = min(M[i][i], M[j][j]);
			if (tmp != 0)
				S[i][j] = M[i][j] * M[i][j] / float(tmp) / (M[i][j] + ALPHA);
			else
				S[i][j] = 0.;
		}
	}
}

//function for generating recommendation matrix; R=Y*S'=Y*S 
void matrixMultiply(float** S, int** Y, float** R){
	int i, j, k;
	for (i = 0; i < NUM_USER; i++){
		for (j = 0; j < NUM_NEWS; j++){
			R[i][j] = 0.;
		}
	}
	for (i = 0; i < NUM_USER; i++){
		for (j = 0; j < NUM_NEWS; j++){
			int t = Y[i][j];
			if (t == 0)
				continue;
			for (k = 0; k < NUM_NEWS; k++){
				R[i][k] += t * S[j][k];
			}
		}
	}

	//normalization
	/*float* C = new float[NUM_NEWS];
	for (i = 0; i < NUM_NEWS; i++){
		float tmp = 0.;
		for (j = 0; j < NUM_NEWS; j++){
			tmp += S[i][j];
		}
		C[i] = tmp;
	}
	for (i = 0; i < NUM_USER; i++){
		for (j = 0; j < NUM_NEWS; j++){
			if (C[j] != 0)
				R[i][j] /= C[j];
			else
				R[i][j] = 0;
		}
	}
	delete[] C;*/
}

//function for removing the read news
void rmReadNews(int** Y, float** R){
	int i, j;
	for (i = 0; i < NUM_USER; i++){
		for (j = 0; j < NUM_NEWS; j++){
			if (Y[i][j] != 0)
				R[i][j] = 0.;
		}
	}
}

//function to get news publish time
void publishTime(const char* timefile, int* tm){
	ifstream fp(timefile);
	if (fp.fail())
	{
		printf("Cannot open time file!\n");
		return;
	}
	int nid, t;
	while (!fp.eof()){
		fp >> nid >> t;
		tm[nid] = t;
	}
	fp.close();
}

//output the recommendation matrix
void outputMatrix(const char* matrixfile, float** R){
	ofstream fp(matrixfile);
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS - 1; j++){
			fp<<R[i][j]<<" ";
		}
		fp<<R[i][NUM_NEWS - 1]<<endl;
	}
	fp.close();
}

//get the top 1 recommendation
void topRecommendation(float** R, int* tr){
	for (int i = 0; i < NUM_USER; i++){
		float max = 0.;
		int idx = 0;
		for (int j = 0; j < NUM_NEWS; j++){
			if (R[i][j] > max){
				max = R[i][j];
				idx = j;
			}
		}
		tr[i] = idx;
	}
}

//test the recommedation
void testRecommendation(const char* testfile, int* tr){
	ifstream fp(testfile);
	int uid, nid;
	int tp = 0;
	if (fp.fail())
	{
		printf("Cannot open testing file!\n");
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

float get_k_th_element(float* x){
	priority_queue<float, vector<float>, greater<float> > q;   //minheap
	for (int i = 0; i < NUM_NEWS; i++){
		if (q.size() < K){
			q.push(x[i]);
		}
		else{
			if (x[i]>q.top()){
				q.pop();
				q.push(x[i]);
			}
		}
	}
	return q.top();
}

void getKNN(float** S, int** NN){
	for (int i = 0; i < NUM_NEWS; i++){
		float kth = get_k_th_element(S[i]);
		int k = 0;
		for (int j = 0; j < NUM_NEWS; j++){
			if (S[i][j]>kth){
				NN[i][k] = j;
				k++;
			}
		}
		while (k < K){
			NN[i][k] = i;
			k++;
		}
	}
}

void kNNMultiply(int** NN, float** S, int** Y, float** R){
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS; j++){
			float tmp = 0.;
			for (int k = 0; k < K; k++){
				tmp += Y[i][NN[j][k]] * S[j][NN[j][k]];
			}
			R[i][j] = tmp;
		}
	}
}

int main(){
	int** Y = new int*[NUM_USER];
	for (int i = 0; i < NUM_USER; i++){
		Y[i] = new int[NUM_NEWS];
	}
	loaddata("../data/remapped_uid_nid_train.txt", Y);
	cout << "load data Ok!" << endl;

	int** M = new int*[NUM_NEWS];
	for (int i = 0; i < NUM_NEWS; i++){
		M[i] = new int[NUM_NEWS];
	}
	genMatrix(Y, M);
	cout << "generate co-occurence matrix Ok!" << endl;

	float** S = new float*[NUM_NEWS];
	for (int i = 0; i < NUM_NEWS; i++){
		S[i] = new float[NUM_NEWS];
	}
	/*int* tm = new int[NUM_NEWS];
	publishTime("../data/remapped_nid_time.txt", tm);
	similarityMatrix_t(M, S, tm);*/
	similarityMatrix(M, S);
	for (int i = 0; i < NUM_NEWS; i++){
		delete [] M[i];
	}
	delete [] M;
	cout << "generate similarity matrix Ok!" << endl;

	int** NN = new int*[NUM_NEWS];
	for (int i = 0; i < NUM_NEWS; i++){
		NN[i] = new int[K];
	}
	getKNN(S, NN);
	cout << "get kNN oK!" << endl;

	float** R = new float*[NUM_USER];
	for (int i = 0; i < NUM_USER; i++){
		R[i] = new float[NUM_NEWS];
	}
	//matrixMultiply(S, Y, R);
	kNNMultiply(NN, S, Y, R);
	rmReadNews(Y, R);
	cout << "generate recommendation matrix Ok!" << endl;

	outputMatrix("../data/matrix_knn_train.txt", R);
	cout << "output recommendation matrix Ok!" << endl;

	int* tr = new int[NUM_USER];
	topRecommendation(R, tr);
	testRecommendation("../data/remapped_uid_nid_test.txt", tr);
	cout << "finished!!!" << endl;

	delete [] tr;

	for (int i = 0; i < NUM_NEWS; i++){
		delete[] NN[i];
	}
	delete[] NN;
	for (int i = 0; i < NUM_USER; i++){
		delete [] R[i];
	}
	delete [] R;
	for (int i = 0; i < NUM_NEWS; i++){
		delete [] S[i];
	}
	delete [] S;
	for (int i = 0; i < NUM_USER; i++){
		delete [] Y[i];
	}
	delete [] Y;
	getchar();
	return 1;

}