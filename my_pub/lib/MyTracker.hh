#ifndef MyTracker_hh
#define MyTracker_hh

//#include "MyVector.hh"
#include "TVector3.h"
#include <iostream>

class MyTracker
{
public:
	TVector3 trackerPoint;
	TVector3 trackerSlope;
	double chi2_ndf[2] = { 0, 1 };
	double r2 = 0;
	MyTracker()
	{
		trackerPoint.SetXYZ(0., 0., 0.);
		trackerSlope.SetXYZ(0., 0., 0.);
	}
	MyTracker(TVector3 point, TVector3 slope)
	{
		trackerPoint = point;
		trackerSlope = slope;
	}
	~MyTracker(){}
	
};
#endif
