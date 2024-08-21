#ifndef MyPocaResult_hh
#define MyPocaResult_hh 1

//#include "MyVector.hh"
#include "TVector3.h"
#include <iostream>

using namespace std;

class MyPocaResult
{
public:
	MyPocaResult() {};
	TVector3 ScatterPoint{ 0, 0, 0 }; // Unit is same as the input data
	double ScatterAngle{ 0 }; // Unit mrad
	double PlaneAngleX{ 0 }; // Unit mrad
	double PlaneAngleY{ 0 }; // Unit mrad
	int is_hit{ 0 };
	void Show()
	{
		cout << "Point: ";
		ScatterPoint.Print();
		cout << ", space angle: " << ScatterAngle
			<< ", X plane angle: " << PlaneAngleX << ", Y plane angle: " << PlaneAngleY
			<< " (mrad)" << endl;
	}
	double t = 0.; // Please see https://www.yuque.com/wyu0725/mt_ustc_nda/poca_calculation (Need authority to open) 
	double s = 0.;
};
#endif // !MyPocaResult_hh