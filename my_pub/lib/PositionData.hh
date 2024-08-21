#ifndef PositionData_h
#define PositionData_h 1

#include <iostream>
class PositionData{
	public:
		PositionData(){
			sig = 0;
			sig_x = 0;
			sig_y = 0;
			x = -1000;
			y = -1000;
			z = -2000;
			hit_strip_num_x = 0;
			hit_strip_num_y = 0;
			hit_amp_x = 0;
			hit_amp_y = 0;
			hit_chn_num_x = 0;
			hit_chn_num_y = 0;
			x_nhits = 0;
			y_nhits = 0;
			for (int i = 0; i < 10; i++)
			{
				x_other[i] = 0;
				y_other[i] = 0;
			}
		}
		~PositionData(){}
		long sig;
		long sig_x;
		long sig_y;
		double x;
		double y;
		double z;
		long hit_strip_num_x;
		long hit_strip_num_y;
		double hit_amp_x;
		double hit_amp_y;
		long hit_chn_num_x;
		long hit_chn_num_y;
		long x_nhits;
		double x_other[10];
		double x_other_amp[10];
		long y_nhits;
		double y_other[10];
		double y_other_amp[10];
		void Show() const
		{
			std::cout << "(" << x << " ," << y << " ," << z << ")" << std::endl;
		}
	private:
};

#endif
