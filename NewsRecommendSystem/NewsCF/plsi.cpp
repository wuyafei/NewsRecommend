#include"newscf.h"
#include<random>
using namespace std;

void random_param(float** x, int m, int n){
	default_random_engine generator;
	normal_distribution<float> distribution(0.5, 1.0);
	for (int i = 0; i < m; i++){
		int j = 0;
		while (j < n){
			float number=distribution(generator);
			if (number>1.0 || number < 0.)
				continue;
			x[i][j] = number;
			j++;
		}
	}
}

void getQ(float*** q, float** psz, float** puz){
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS; j++){
			float tmp = 0.;
			for (int k = 0; k < NUM_Z; k++){
				tmp += puz[i][k] * psz[j][k];
			}
			for (int k = 0; k < NUM_Z; k++){
				q[i][j][k] = puz[i][k] * psz[j][k] / tmp;
			}
		}
	}
}

void update_psz(float** psz,  float*** q, int** Y){
	for (int i = 0; i < NUM_NEWS; i++){
		for (int j = 0; j < NUM_Z; j++){
			psz[i][j] = 0.;
			for (int k = 0; k < NUM_USER; k++){
				if (Y[k][i] == 1){
					psz[i][j] += q[k][i][j];
				}
			}
		}
	}
	float* tmp = new float[NUM_Z];
	for (int i = 0; i < NUM_Z; i++){
		tmp[i] = 0.;
		for (int j = 0; j < NUM_USER; j++){
			for (int k = 0; k < NUM_NEWS; k++){
				if (Y[j][k] == 1){
					tmp[i] += q[j][k][i];
				}
			}
		}
	}
	for (int i = 0; i < NUM_NEWS; i++){
		for (int j = 0; j < NUM_Z; j++){
			psz[i][j] /= tmp[j];
		}
	}
	delete[]  tmp;
}
void update_puz(float** puz, float*** q, int** Y){
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_Z; j++){
			puz[i][j] = 0.;
			for (int k = 0; k < NUM_NEWS; k++){
				if (Y[i][k] == 1){
					puz[i][j] += q[i][k][j];
				}
			}
		}
	}

	float* tmp = new float[NUM_USER];
	for (int i = 0; i < NUM_USER; i++){
		tmp[i] = 0.;
		for (int j = 0; j < NUM_NEWS; j++){
			if (Y[i][j] == 1){
				for (int k = 0; k < NUM_Z; k++){
					tmp[i] += q[i][j][k];
				}
			}
			
		}
	}
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_Z; j++){
			puz[i][j] /= tmp[i];
		}
	}
	delete[]  tmp;

}

void probScore(double** ps, float** psz, float** puz){
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS; j++){
			double tmp = 0.;
			for (int k = 0; k < NUM_Z; k++){
				tmp += psz[j][k] * puz[i][k];
			}
			ps[i][j] = tmp;
		}
	}
}

int plsi(){   //can't work because of not enough memory
	cout << "plsi start" << endl;
	float** psz = new float*[NUM_NEWS];
	for (int i = 0; i < NUM_NEWS; i++)
		psz[i] = new float[NUM_Z];
	random_param(psz, NUM_NEWS, NUM_Z);
	cout << "random psz ok "<< endl;
	float** puz = new float*[NUM_USER];
	for (int i = 0; i < NUM_USER; i++)
		puz[i] = new float[NUM_Z];
	random_param(puz, NUM_USER, NUM_Z);
	cout << "random puz ok" << endl;

	double** ps = new double*[NUM_USER];
	for (int i = 0; i < NUM_USER; i++)
		ps[i] = new double[NUM_NEWS];

	probScore(ps, psz, puz);
	cout << "initial probability score ok" << endl;

	double* tr = new double[NUM_USER];
	topRecommendation(tr, ps);
	testRecommendation("../data/remapped_uid_nid_test.txt", tr);

	float*** q = new float**[NUM_USER];
	for (int i = 0; i < NUM_USER; i++){
		q[i] = new float*[NUM_NEWS];
		for (int j = 0; j < NUM_NEWS; j++)
			q[i][j] = new float[NUM_Z];
	}
	cout << "initialize q ok" << endl;
	int** Y = loadData("../data/remapped_uid_nid_train.txt");
	cout << "load data ok" << endl;
	getQ(q, psz, puz);
	cout << "get q ok" << endl;
	update_psz(psz, q, Y);
	cout << "update psz ok" << endl;
	update_puz(puz, q, Y);
	cout << "update puz ok" << endl;

	probScore(ps, psz, puz);
	cout << "probability score ok" << endl;
	topRecommendation(tr, ps);
	testRecommendation("../data/remapped_uid_nid_test.txt", tr);

	for (int i = 0; i < NUM_NEWS; i++)
		delete[] psz[i];
	delete [] psz;
	for (int i = 0; i < NUM_USER; i++)
		delete [] puz[i];
	delete [] puz;
	for (int i = 0; i < NUM_USER; i++)
		delete [] ps[i];
	delete[] ps;
	delete[] tr;
	for (int i = 0; i < NUM_USER; i++){
		for (int j = 0; j < NUM_NEWS; j++)
			delete [] q[i][j];
		delete[] q[i];
	}
	delete[] q;

	for (int i = 0; i < NUM_USER; i++)
		delete[] Y[i];
	delete[] Y;

	//getchar();
	return 0;
}
