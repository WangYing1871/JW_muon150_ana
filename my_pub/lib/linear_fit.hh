#ifndef linear_fit_hh
#define linear_fit_hh 1
#include <iostream>
#include <vector>
#include <TF1.h>
#include <TGraph.h>
#include "constant.hh"
using namespace std;
class linear_fit
{
public:
	linear_fit(vector<double> x, vector<double> y);
	linear_fit(double* xi, double* yi, int n);
	~linear_fit();
	void run();
	bool run_best_fit(double chi2_max, double corr_min, int minimum_layer, bool is_chi2_set = true, bool is_corr_set = true);
	bool least_square_fit();

	double slope;
	double intercept;
	double chi2;
	double ndf;
	double corr;

	double rmse;
	double r2;
private:
	double* x;
	double* y;
	vector<double> x_origin;
	vector<double> y_origin;
	vector<double> non_linear;
	double max_nonl = 0;
	int nonl_loc = 0;
	int len;
	TF1* linear_fun;
	TGraph* gr;
	void init();
	
};

#endif // !linear_fit_hh
