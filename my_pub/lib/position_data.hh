#pragma once
#include "constant.hh"

class position_data
{
public:
	position_data() 
	{
		x_nhits = 0;
		y_nhits = 0;
		z = 0;
		for (int i = 0; i < cMAX_CLUSTER; i++)
		{
			x[i] = 0;
			y[i] = 0;
			x_amp[i] = 0;
			y_amp[i] = 0;			
			x_chn_num[i] = 0;
			y_chn_num[i] = 0;
			x_cluster_hole[i] = 0;
			y_cluster_hole[i] = 0;
		}
	};
	~position_data() {};
	int x_nhits;
	int y_nhits;
	double x[cMAX_CLUSTER];
	double y[cMAX_CLUSTER];
	double z;
	double x_amp[cMAX_CLUSTER];
	double y_amp[cMAX_CLUSTER];
	int x_chn_num[cMAX_CLUSTER];
	int y_chn_num[cMAX_CLUSTER];
	int x_cluster_hole[cMAX_CLUSTER];
	int y_cluster_hole[cMAX_CLUSTER];

private:

};

