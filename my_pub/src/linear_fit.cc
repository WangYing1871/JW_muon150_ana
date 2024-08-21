#include "linear_fit.hh"

linear_fit::linear_fit(vector<double> xi, vector<double> yi)
{
	x_origin.clear();
	y_origin.clear();
	for (int i = 0; i < xi.size(); i++)
	{
		x_origin.push_back(xi[i]);
		y_origin.push_back(yi[i]);
	}
	len = xi.size();
	init();
}

linear_fit::linear_fit(double* xi, double* yi, int n)
{
	x_origin.clear();
	y_origin.clear();
	for (int i = 0; i < n; i++)
	{
		x_origin.push_back(xi[i]);
		y_origin.push_back(yi[i]);
	}
	len = n;
	init();
}

linear_fit::~linear_fit()
{
	delete[] x;
	delete[] y;
	delete linear_fun;
	delete gr;
}

void linear_fit::init()
{
	x = new double[len];
	y = new double[len];
	for (int i = 0; i < len; i++)
	{
		x[i] = x_origin[i];
		y[i] = y_origin[i];
	}
	double min_x = *min_element(x, x + len);
	double max_x = *max_element(x, x + len);
	linear_fun = new TF1("linear_fun", "[0]+[1]*x", min_x - 1, max_x + 1);
	gr = new TGraph(len, x, y);
}

void linear_fit::run()
{
	TFitResultPtr r = gr->Fit(linear_fun, "SQ");
	slope = linear_fun->GetParameter(1);
	intercept = linear_fun->GetParameter(0);
	corr = gr->GetCorrelationFactor();
	chi2 = linear_fun->GetChisquare();
	ndf = linear_fun->GetNDF();
	double calc;
	double nonl;
	max_nonl = 0;
	nonl_loc = 0;
	non_linear.clear();
	double mse = 0;
	for (int i = 0; i < len; i++)
	{
		mse += (y[i] - slope * x[i] - intercept) * (y[i] - slope * x[i] - intercept);
		calc = slope * x[i] + intercept;
		nonl = abs(calc - y[i]);
		non_linear.push_back(nonl);
		if (nonl > max_nonl)
		{
			max_nonl = nonl;
			nonl_loc = i;
		}
	}
	rmse = sqrt(mse / len);
}

bool linear_fit::run_best_fit(
    double chi2_max
    , double corr_min
    , int minimum_layer
    , bool is_chi2_set
    , bool is_corr_set) {
	run();
	// AB + !B = A + !B
	while (((abs(chi2) > chi2_max) || !is_chi2_set) 
			&& ((abs(corr) < corr_min) || !is_corr_set)
			 && len > minimum_layer)
	{
		//cout << cRED << "Chi2: " << chi2 << cRESET << endl;
		x_origin.erase(x_origin.begin() + nonl_loc);
		y_origin.erase(y_origin.begin() + nonl_loc);
		len--;
		delete x;
		delete y;
		delete linear_fun;
		delete gr;
		init();
		run();
	}
	if (((abs(chi2) > chi2_max) || !is_chi2_set) && ((abs(corr) < corr_min) || !is_corr_set))
	{
		return false;
	}
	return true;
	
}

bool linear_fit::least_square_fit()
{
	if (x_origin.size() == y_origin.size() && x_origin.size() >= 3)
	{
		double K = 0, B = 0, MSE = 0; // y = k*x+b
		double Mx = 0, My = 0;
		double Mxx = 0, Myy = 0, Mxy = 0;
		int n = x_origin.size();
		for (int i = 0; i < n; i++)
		{
			Mx += x_origin[i] / n;
			My += y_origin[i] / n;
			Mxx += x_origin[i] * x_origin[i] / n;
			Myy += y_origin[i] * y_origin[i] / n;
			Mxy += x_origin[i] * y_origin[i] / n;
		}
		if (Mxx - Mx * Mx == 0)
		{
			return false;
		}
		K = (Mxy - Mx * My) / (Mxx - Mx * Mx);
		B = My - K * Mx;
		for (int i = 0; i < n; i++)
		{
			MSE += (y_origin[i] - K * x_origin[i] - B) * (y_origin[i] - K * x_origin[i] - B) / n;
		}
		slope = K;
		intercept = B;
		rmse = sqrt(MSE);
		r2 = (Mxy - Mx * My) * (Mxy - Mx * My) / (Mxx - Mx * Mx) / (Myy - My * My); //correlation index R2
		return true;
	}
	else if (x_origin.size() == y_origin.size() && x_origin.size() == 2)
	{
		slope = (y_origin[1] - y_origin[0]) / (x_origin[1] - x_origin[0]);
		intercept = y_origin[0] - slope * x_origin[0];
		rmse = 0;
		r2 = 1;
		return true;
	}
	else if (x_origin.size() != y_origin.size())
	{
		cout << "Error: vectors is not the same size!" << endl;
		return false;
	}
	else
	{
		// cout << "Error: vectors' size is too small!" << endl;
		return false;
	}
}
