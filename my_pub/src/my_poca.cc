#include "my_poca.hh"

my_poca::my_poca(MyTracker* up, MyTracker* down)
{
	tracker_up = up;
	tracker_down = down;
}
my_poca::~my_poca()
{

}
MyPocaResult my_poca::poca()
{
	TVector3 u = tracker_up->trackerSlope;
	TVector3 v = tracker_down->trackerSlope;
	TVector3 p = tracker_up->trackerPoint;
	TVector3 q = tracker_down->trackerPoint;

	double* theta = VectorAngle(u, v);
	MyPocaResult poca_result;

	// If the theta is not equal to 0, the two inject tracker and the emergence tracker are not parallel and D != 0
	if (abs(theta[0]) == 0)
	{
		//cout << "Small angle scatter" << endl;
		return poca_result;
	}
	poca_result.ScatterAngle = theta[0] * 1000;
	poca_result.PlaneAngleX = theta[1] * 1000;
	poca_result.PlaneAngleY = theta[2] * 1000;
	delete[] theta;

	TVector3 w = p - q;

	double uu = u * u;
	double uv = u * v;
	double vv = v * v;
	double uw = u * w;
	double vw = v * w;
	double D = uu * vv - uv * uv;

	// The calculation of  PoCA, please see https://www.yuque.com/wyu0725/mt_ustc_nda/poca_calculation (Need authority to open) 
	double s = (uu * vw - uw * uv) / D;
	double t = (vw * uv - vv * uw) / D;
	TVector3 pq = p + q;
	TVector3 ut = t * u;
	TVector3 vs = s * v;

	poca_result.ScatterPoint = (pq+ ut + vs) * 0.5;

	poca_result.t = t;
	poca_result.s = s;
	//poca_result.Show();
	return poca_result;
}

double* my_poca::VectorAngle(TVector3 u)
{
	TVector3 z(0, 0, 1);
	double angle = u.Angle(z);
	if (u(0) < 0)
	{
		angle = -angle;
	}
	/*else if (u(0) == 0)
	{
		if (u(1) < 0)
		{
			angle = -angle;
		}
	}*/
	double* plane_angle = PlaneAngle(u);
	double* total_angle = new double[3];
	total_angle[0] = angle;
	total_angle[1] = plane_angle[0];
	total_angle[2] = plane_angle[1];
	delete[] plane_angle;

	return total_angle;
}

double* my_poca::VectorAngle(TVector3 u, TVector3 v)
{
	TVector3 z(0, 0, 1);
	double theta_uz = u.Angle(z);
	if (theta_uz == 0)
	{
		return VectorAngle(v);
	}

	TVector3 u_cross_z = u.Cross(z);
	TRotation r;
	r.Rotate(theta_uz, u_cross_z);
	v.Transform(r);
	return VectorAngle(v);
}

double my_poca::PlaneAngle(TVector2 u)
{
	return u.Phi_mpi_pi(u.Phi());
}

double* my_poca::PlaneAngle(TVector3 p)
{
	double* plane_angle = new double[2];
	TVector2 x(p.Z(), p.X());
	TVector2 y(p.Z(), p.Y());
	plane_angle[0] = PlaneAngle(x);
	plane_angle[1] = PlaneAngle(y);
	return plane_angle;
}
