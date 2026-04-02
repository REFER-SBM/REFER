
#ifndef _CUEFACES_V_GEOMETRY_
	#define _CUEFACES_V_GEOMETRY_

	const double M_PI = 3.14159265359;

	typedef double DEGREES;

	class VECTOR2D { public: double x,y; };
	class VECTOR3D { public: double x, y, z; };

	void Normalise (VECTOR3D *Vector);
	//double VectorMagnitude (VECTOR3D *V);
	//void GetCrossProduct (VECTOR3D *CrossProduct, VECTOR3D *A, VECTOR3D *B);
	double DotProduct (VECTOR3D *Vector1, VECTOR3D *Vector2);

	DEGREES danglecwise (DEGREES Direction1, DEGREES Direction2);
	DEGREES dangleclock (DEGREES Direction1, DEGREES Direction2);
	bool clockwise (DEGREES A, DEGREES B);
#endif

