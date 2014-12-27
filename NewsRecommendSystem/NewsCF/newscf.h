#include<iostream>
#include<fstream>
#define NUM_USER 10000
#define NUM_NEWS 6183
#define ALPHA 200
#define NUM_Z 5

int**  loadData(const char*);
void topRecommendation(double*, double**);
void testRecommendation(const char*, double*);


int minhash();
double** recommendationScore(int**, double**);
double** JaccardMatrix(int**);


int covisitation();
double** covisitationScore(int**, double**);
double** covisitationMatrix(int**);