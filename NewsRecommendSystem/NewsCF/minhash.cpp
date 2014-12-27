#include"newscf.h"
using namespace std;


int**  loadData(const char* data_file){
	ifstream fp(data_file);
	if (fp.fail()){
		cout << "Cannot open training file!" << endl;
		return NULL;
	}
	int i, j;
	int** v = new int*[NUM_USER];
	for (int i = 0; i < NUM_USER; i++){
		v[i] = new int[NUM_NEWS];
	}
	for (i = 0; i < NUM_USER; i++)//row:user; col:news
		for (j = 0; j < NUM_NEWS; j++)
			v[i][j] = 0;
	while (!fp.eof()){
		fp >> i >> j;
		v[i][j] = 1;
	}
	fp.close();
	return v;
}

double** JaccardMatrix(int** v){
	double** jm = new double*[NUM_USER];
	for (int i = 0; i < NUM_USER; i++)
		jm[i] = new double[NUM_USER];
	for (int i = 0; i < NUM_USER; i++)
		for (int j = 0; j < NUM_USER; j++)
			jm[i][j] = 0.;

	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS; j++){
			if (v[i][j] == 1){
				for (int k = i + 1; k < NUM_USER; k++){
					if (v[k][j] == 1)
						jm[i][k] += 1;
				}
			}
		}
	}

	int* tmp = new int[NUM_USER];
	for (int i = 0; i < NUM_USER; i++){
		int cnt = 0;
		for (int j = 0; j < NUM_NEWS; j++){
			if (v[i][j] == 1){
				cnt++;
			}
		}
		tmp[i] = cnt;
	}

	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_USER; j++){
			double x = jm[i][j];
			jm[i][j] = x / (tmp[i] + tmp[j] - x);
		}
	}
	delete[] tmp;
	

	return jm;
}

double** recommendationScore(int** Y, double** jm){
	double** rs = new double*[NUM_USER];
	for (int i = 0; i < NUM_USER; i++)
		rs[i] = new double[NUM_NEWS];
	for (int i = 0; i < NUM_USER; i++)
		for (int j = 0; j < NUM_NEWS; j++)
			rs[i][j] = 0.;
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_USER; j++){
			double sm = jm[i][j];
			if (sm == 0.)
				continue;
			for (int k = 0; k < NUM_NEWS; k++){
				if (Y[j][k] == 1){
					rs[i][k] += sm;
				}
			}
		}
	}
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS;j++){
			if (Y[i][j] == 1)
				rs[i][j] = 0.;
		}
	}

	return rs;
}

void topRecommendation(double* tr, double** rs){
	
	for (int i = 0; i < NUM_USER; i++){
		double max = 0.;
		int idx = 0;
		for (int j = 0; j < NUM_NEWS; j++){
			if (rs[i][j]>max){
				max = rs[i][j];
				idx = j;
			}
		}
		tr[i] = idx;
	}
}
void testRecommendation(const char* test_file, double* tr){
	ifstream fp(test_file);
	if (fp.fail()){
		cout << "open test file failed!!" << endl;
		return;
	}
	int i, j;
	int cnt = 0;
	while (!fp.eof()){
		fp >> i >> j;
		if (tr[i] == j)
			cnt++;
	}
	fp.close();
	cout << "precision=" << (double)cnt / NUM_USER << endl;
}


int minhash(){
	cout << "minhash start" << endl;
	int** v = loadData("../data/remapped_uid_nid_train.txt");
	cout << "load data ok" << endl;
	double** jm = JaccardMatrix(v);
	cout << "Jaccard Matrix ok" << endl;
	double** rs = recommendationScore(v, jm);
	cout << "recommendation score ok" << endl;
	double* tr = new double[NUM_USER];
	topRecommendation(tr,rs);
	cout << "top recommendation ok" << endl;
	testRecommendation("../data/remapped_uid_nid_test.txt", tr);

	delete [] tr;
	for (int i = 0; i < NUM_USER; i++)
		delete[] rs[i];
	delete rs;
	for (int i = 0; i < NUM_USER; i++)
		delete[] jm[i];
	delete jm;
	for (int i = 0; i < NUM_USER; i++)
		delete[] v[i];
	delete v;
	//getchar();
	return 0;
}
