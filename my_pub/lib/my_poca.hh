#ifndef my_poca_hh
#define my_poca_hh 1

#include<TVector3.h>
#include <TVector2.h>
#include <TRotation.h>
#include "MyPocaResult.hh"
#include "MyTracker.hh"

class my_poca
{
public:
	my_poca(MyTracker* up, MyTracker* down);
	~my_poca();
	MyPocaResult poca();

	double* VectorAngle(TVector3);
	double* VectorAngle(TVector3, TVector3);
	double PlaneAngle(TVector2);
	double* PlaneAngle(TVector3);

private:
	MyTracker* tracker_up;
	MyTracker* tracker_down;
};
#endif // !my_poca_hh
