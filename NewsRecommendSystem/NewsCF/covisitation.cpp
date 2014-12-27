#include"newscf.h"
using namespace std;

inline double min(double x, double y){
	return x < y ? x : y;
}
//function to genearte Co-visitation Matrix:M, Y is the rating matrix
double** covisitationMatrix(int** Y){
	double** M = new double*[NUM_NEWS];
	for (int i = 0; i < NUM_NEWS; i++)
		M[i] = new double[NUM_NEWS];

	for (int i = 0; i < NUM_NEWS; i++)
		for (int j = 0; j < NUM_NEWS; j++)
			M[i][j] = 0.;

	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS; j++){
			if (Y[i][j] != 0){
				M[j][j] += 1.;
				for (int k = j + 1; k < NUM_NEWS; k++){
					if (Y[i][k] != 0){
						M[j][k] += 1.;
						M[k][j] += 1.;
					}
				}
			}
		}
	}
	for (int i = 0; i < NUM_NEWS; i++){
		for (int j = 0; j < NUM_NEWS; j++){
			double tmp = min(M[i][i], M[j][j]);
			if (tmp != 0.)
				M[i][j] = M[i][j] * M[i][j] / tmp / (M[i][j] + ALPHA);
			else
				M[i][j] = 0.;
		}
	}
	return M;
}


double** covisitationScore(int** Y, double** M){
	double** cs = new double*[NUM_USER];
	for (int i = 0; i < NUM_USER; i++)
		cs[i] = new double[NUM_NEWS];
	for (int i = 0; i < NUM_USER; i++)
		for (int j = 0; j < NUM_NEWS; j++)
			cs[i][j] = 0.;
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS; j++){
			int t = Y[i][j];
			if (t == 0)
				continue;
			for (int k = 0; k < NUM_NEWS; k++){
				cs[i][k] += M[j][k];
			}
		}
	}
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS; j++){
			if (Y[i][j] == 1)
				cs[i][j] = 0.;
		}
	}
	return cs;
}

int covisitation(){
	cout << "covisitation start" << endl;
	int** v = loadData("../data/remapped_uid_nid_train.txt");
	cout << "load data ok" << endl;
	double** cm = covisitationMatrix(v);
	cout << "Covisitation Matrix ok" << endl;
	double** cs = covisitationScore(v, cm);
	cout << "covisitation score ok" << endl;
	double* tr = new double[NUM_USER];
	topRecommendation(tr,cs);
	cout << "top recommendation ok" << endl;
	testRecommendation("../data/remapped_uid_nid_test.txt", tr);

	delete[] tr;
	for (int i = 0; i < NUM_USER; i++)
		delete[] cs[i];
	delete cs;
	for (int i = 0; i < NUM_NEWS; i++)
		delete[] cm[i];
	delete cm;
	for (int i = 0; i < NUM_USER; i++)
		delete[] v[i];
	delete v;
	//getchar();
	return 0;
}