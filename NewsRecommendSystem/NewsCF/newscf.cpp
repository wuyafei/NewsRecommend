#include"newscf.h"
using namespace std;


void linearCombination(double** os, double** rs, double** cs, double p, double q){
	
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS; j++){
			os[i][j] = p*rs[i][j] + q*cs[i][j];
		}
	}
}

int main(){

	cout << "start" << endl;
	int** v = loadData("../data/remapped_uid_nid_train.txt");
	cout << "load data ok" << endl;

	double** jm = JaccardMatrix(v);
	cout << "Jaccard Matrix ok" << endl;
	double** rs = recommendationScore(v, jm);
	cout << "recommendation score ok" << endl;
	for (int i = 0; i < NUM_USER; i++)
		delete[] jm[i];
	delete jm;

	double** cm = covisitationMatrix(v);
	cout << "Covisitation Matrix ok" << endl;
	double** cs = covisitationScore(v, cm);
	cout << "covisitation score ok" << endl;
	for (int i = 0; i < NUM_NEWS; i++)
		delete[] cm[i];
	delete cm;

	for (int i = 0; i < NUM_USER; i++)
		delete[] v[i];
	delete v;

	double** os = new double*[NUM_USER];
	for (int i = 0; i < NUM_USER; i++)
		os[i] = new double[NUM_NEWS];
	double* tr = new double[NUM_USER];
	//for (int i = 0; i < 101; i++){
	double p = 0.73;// + double(i) / 10000;
		double q = 1 - p;
		linearCombination(os, rs, cs,p,q);
		topRecommendation(tr,os);
		cout << "top recommendation ok, p=" <<p << endl;
		testRecommendation("../data/remapped_uid_nid_test.txt", tr);	
	//}
	delete[] tr;
	for (int i = 0; i < NUM_USER; i++)
		delete[] os[i];
	delete os;
	
	for (int i = 0; i < NUM_USER; i++)
		delete[] rs[i];
	delete rs;
	for (int i = 0; i < NUM_USER; i++)
		delete[] cs[i];
	delete cs;

	getchar();
	return 0;
}