//+-----------------------------------------------------------------------------+
//| Copyleft (C) 2025 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

#ifndef TOOLS_GEOMETRY_VISITED
	#define TOOLS_GEOMETRY_VISITED

	#include <list>    //Used for POLYGONS

	class POINT2D
	{
		public:
			double x, y;

			POINT2D();
			POINT2D(const POINT2D& DP);
			POINT2D(double cx, double cy);
			~POINT2D();
			bool operator==(const POINT2D& right) const;
			bool operator!=(const POINT2D& right) const;
			POINT2D& operator+=(const POINT2D& right);
			POINT2D operator+(const POINT2D& right) const;
			POINT2D& operator-=(const POINT2D& right);
			POINT2D operator-(const POINT2D& right) const;
			POINT2D operator*(double scale) const;  //Scale point
			double operator*(const POINT2D& right) const;  //scalar product
			POINT2D operator/(double divide) const;  //Divide point
			//POINT2D& operator*=(const POINT2D& right);
			//POINT2D operator*(const POINT2D& right) const;
			//POINT2D& operator/=(const POINT2D& right);
			//POINT2D operator/(const POINT2D& right) const;

			void POINT2D::assign(double x_new, double y_new);

			POINT2D Rotate(POINT2D Center, double Angle);
			POINT2D Rotate(double Angle);
	};


	const POINT2D origin (0,0);


	//Struct to save the label of the POINT2D together with its coordinates
	struct NumPOINT2D {long Number; POINT2D Point; };
	const NumPOINT2D NumOrigin = { 0, origin};


	class SEGMENT2D
	{
		public:
			POINT2D TipBegin, TipEnd;
			double Length;
			double Slope180;

			SEGMENT2D();
			SEGMENT2D(const SEGMENT2D& DS);
			SEGMENT2D(POINT2D cTipBegin, POINT2D cTipEnd);
			SEGMENT2D(NumPOINT2D cTipBegin, NumPOINT2D cTipEnd);
			SEGMENT2D(double cTipBeginx, double cTipBeginy, double cTipEndx, double cTipEndy);
			~SEGMENT2D();
			bool operator==(const SEGMENT2D& right) const;
			bool operator!=(const SEGMENT2D& right) const;

			void clear();

			void Rotate(POINT2D Center, double Angle);

			bool intersects (const SEGMENT2D &) const;
			bool contains (const POINT2D &) const;
			bool contains (const NumPOINT2D &) const;
	};


	class TRIANGLE2D
	{
		public:
			//Constructor
			TRIANGLE2D (const NumPOINT2D & p1,
						const NumPOINT2D & p2,
						const NumPOINT2D & p3);
			~TRIANGLE2D();

			//Geometric and miscelaneous operations
			bool contains (const NumPOINT2D &) const;
			double area () const;
			int orientation () const;

			// Comparison operators
			bool operator== (const TRIANGLE2D & t) const;
			bool operator!= (const TRIANGLE2D & t) const;

			bool operator> (const TRIANGLE2D &) const;
			bool operator< (const TRIANGLE2D &) const;

			NumPOINT2D v1, v2, v3;

	private:
			double signed_2area () const;
	};


	inline int turn (const NumPOINT2D & v1, const NumPOINT2D & v2, const NumPOINT2D & v3)
	{
		return TRIANGLE2D(v1,v2,v3).orientation();
	}


	inline int turn (const POINT2D & v1, const POINT2D & v2, const POINT2D & v3)
	{
		NumPOINT2D nv1; nv1.Number= 0; nv1.Point= v1;
		NumPOINT2D nv2; nv2.Number= 0; nv2.Point= v2;
		NumPOINT2D nv3; nv3.Number= 0; nv3.Point= v3;

		return turn(nv1,nv2,nv3);
	}


	class RECTANGLE2D
	{
		public:
			double left, right, bottom, top;

			RECTANGLE2D();
			RECTANGLE2D(const RECTANGLE2D& DR);
			RECTANGLE2D(double cleft, double cright, double cbottom, double ctop);
			~RECTANGLE2D();
	};


	class POLYGON2D {
		public:
			//Constructor
			POLYGON2D ();
			POLYGON2D (std::vector<NumPOINT2D> &p);
			~POLYGON2D();

			//Geometric operations
			//bool containsPoint (const NumPOINT2D &) const;
			int polygonType();
			std::vector<long> ConvexHull();
			void ReversePolygon();
			bool triangulate (std::vector<TRIANGLE2D> &);

			//Vertices manipulation
			class iterator;
			class const_iterator;

			iterator begin();
			const_iterator begin() const;
			void push_back (const NumPOINT2D &);
			void push_front (const NumPOINT2D &);
			void insert (const iterator &, const NumPOINT2D &);
			iterator remove (const iterator &);
			void clear ();
			int size ();
			NumPOINT2D get (iterator current);

		private:
			std::list<NumPOINT2D> vertices;

		public:
			class iterator 
			//The implementation of class Polygon uses the STL (Standard Template Library) list container. 

			//For convenience of use, an iterator for this class is provided, as the standard containers do. 
			//Thus, polygons can be manipulated in the same manner as most of the standard containers, as shown below:
			//	POLYGON2D P;
			//	POLYGON2D::iterator current = P.begin();
			//	do
			//	{
			//		// ...
			//	}
			//	while (++current != P.begin());
			{
				public:
					iterator (const std::list<NumPOINT2D>::iterator & _i = 
									std::list<NumPOINT2D>::iterator(), 
							  const std::list<NumPOINT2D>::iterator & _first = 
									std::list<NumPOINT2D>::iterator(),
							  const std::list<NumPOINT2D>::iterator & _last = 
									std::list<NumPOINT2D>::iterator())
						: i(_i), first(_first), last(_last) {}

					std::list<NumPOINT2D>::iterator get_i () const {
						return i;
					}

					std::list<NumPOINT2D>::iterator get_first () const {
						return first;
					}

					std::list<NumPOINT2D>::iterator get_last () const {
						return last;
					}

					const iterator & operator++ () {	// preincrement
						if (++i == last)
						{
							i= first;
						}

						return *this;
					}

					const iterator operator++ (int) {	// postincrement
						iterator original = *this;

						if (++i == last)
						{
							i= first;
						}

						return original;
					}

					const iterator operator+ (int n) const {
						iterator result = *this;

						for (int i = 0; i < n; i++)
						{
							result++;
						}

						return result;
					}

					const iterator & operator-- () {	// predecrement
						if(i == first)
							i= last;
						i--;

						return *this;
					}

					const iterator operator-- (int) {	// postdecrement
						iterator original = *this;
						if(i == first)
							i= last;
						i--;

						return original;
					}

					const iterator operator- (int n) const {
						iterator result = *this;

						for (int i = 0; i < n; i++)
						{
							result--;
						}

						return result;
					}

					NumPOINT2D & operator* () const {
						return *i;
					}

					bool operator== (const iterator & other) const {
						return i == other.i && last == other.last;
					}

					bool operator!= (const iterator & other) const {
						return !(*this == other);
					}

				private:
					std::list<NumPOINT2D>::iterator i;
					std::list<NumPOINT2D>::iterator first;
					std::list<NumPOINT2D>::iterator last;
			};

			class const_iterator
			//Definition of class Polygon::const_iterator
			{
				public:
					const_iterator (const std::list<NumPOINT2D>::const_iterator & _i = 
										  std::list<NumPOINT2D>::const_iterator(), 
									const std::list<NumPOINT2D>::const_iterator & _first =
										  std::list<NumPOINT2D>::const_iterator(),
									const std::list<NumPOINT2D>::const_iterator & _last = 
										  std::list<NumPOINT2D>::const_iterator())
						: i(_i), first(_i), last(_last) {}

					// conversion constructor (from iterator to const_iterator)
					const_iterator (const iterator & it)
						: i(it.get_i()), first(it.get_first()), last(it.get_last()) {}
							// Use conversion from list::iter to const_iter

					std::list<NumPOINT2D>::const_iterator get_i () const {
						return i;
					}

					const const_iterator & operator++ () {
						if (++i == last)
						{
							i= first;
						}

						return *this;
					}

					const const_iterator operator++ (int) {
						const_iterator original = *this;

						if (++i == last)
						{
							i= first;
						}

						return original;
					}

					const const_iterator operator+ (int n) const {
						const_iterator result = *this;

						for (int i = 0; i < n; i++) {
							result++;
						}

						return result;
					}

					const const_iterator & operator-- () {
						if(i == first)
							i= last;
						i--;

						return *this;
					}

					const const_iterator operator-- (int) {
						const_iterator original = *this;
						if(i == first)
							i= last;
						i--;

						return original;
					}

					const const_iterator operator- (int n) const {
						const_iterator result = *this;

						for (int i = 0; i < n; i++) {
							result--;
						}

						return result;
					}

					const NumPOINT2D & operator* () const {
						return *i;
					}

					bool operator== (const const_iterator & other) const {
						return i == other.i && last == other.last;
					}

					bool operator!= (const const_iterator & other) const {
						return !(*this == other);
					}

				private:
					std::list<NumPOINT2D>::const_iterator i;
					std::list<NumPOINT2D>::const_iterator first;
					std::list<NumPOINT2D>::const_iterator last;
			};
	};


	class CIRCLE2D
	{
		public:
			double centerX, centerY, radius;

			CIRCLE2D();
			CIRCLE2D(const CIRCLE2D& CR);
			CIRCLE2D(POINT2D C, double R);
			CIRCLE2D(double Cx, double Cy, double R);
			~CIRCLE2D();

			double MeritCirclesIntersect(CIRCLE2D C1, CIRCLE2D C2);
			bool CirclesIntersect(CIRCLE2D C1, CIRCLE2D C2);
	};


	///////////////////////////////////////////////////////////////////////////////////

	//public:
		static bool recursive_triangulation (POLYGON2D &, std::vector<TRIANGLE2D> &);

		const double Accuracy = 0.0000001;

		const double NUMBER_PI = 3.1415926535897932;
		#define DEGREES_TO_RADIANS(degrees) degrees*NUMBER_PI/180
		#define RADIANS_TO_DEGREES(radians) radians*180/NUMBER_PI

		#define MODULE_2D(A,B) sqrt((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y))
		#define MODULE2D(v1x, v1y, v2x, v2y) sqrt(pow(v2x-v1x,2)+pow(v2y-v1y,2))

		#define MODULE1_3D(A) sqrt(A.x*A.x+A.y*A.y+A.z*A.z)
		#define MODULE_3D(A,B) sqrt((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y)+(A.z-B.z)*(A.z-B.z))
		#define MODULE3D(v1x, v1y, v1z, v2x, v2y, v2z) sqrt(pow(v2x-v1x,2)+pow(v2y-v1y,2)+pow(v2z-v1z,2))

		#define DOT_PRODUCT_2D(A,B) ( A.x*B.x + A.y*B.y )
		#define DOT_PRODUCT2D(Ax, Ay, Bx, By) ( Ax*Bx + Ay*By )

		#define DOT_PRODUCT_3D(A,B)( A.x*B.x + A.y*B.y + A.z*B.z )
		#define DOT_PRODUCT3D(Ax, Ay, Az, Bx, By, Bz) ( Ax*Bx + Ay*By +Az*Bz )

		//#define CROSS_PRODUCT_2D(A,B) ( A.x*B.Y - A.y*B.X )  //Returns the determinant of the cross product

		//inline double SIGN(const double &a, const double &b)
		////Returns the value of "a" with the sign of "b"
		//{return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

		//inline double NORMALIZE(const double &a)
		////Returns the value of "a" within the range [-1, 1]
		//{
		//	return (SIGN(/*MIN*/ (1 < /*ABS*/ (a >= 0 ? a : -a) ? (1)  : /*ABS*/ (a >= 0 ? a : -a)),
		//			  a));}

		//	//double abs_a= (a >= 0 ? a : -a);
		//	//double min_a_1= (1 < abs_a ? (1)  : (abs_a) );
		//	//return ( a >= 0 ? (min_a_1) : (-min_a_1) );
		//}


		//double det3x3 (double values[]);
		//void invert3x3 (double input[], double output[]);
		//double cofactor3x3 (double values[], long index);

		//void ExpressDAsSumOfABC (VECTOR3D *A, VECTOR3D *B, VECTOR3D *C, VECTOR3D *D, VECTOR3D *E);
		//double ZToMakeDCoplanarWithABC (VECTOR3D *A, VECTOR3D *B, VECTOR3D *C, VECTOR3D *D);

		//bool QuadrilateralIsConvex (POINT2Dv *A, POINT2Dv *B, POINT2Dv *C, POINT2Dv *D);
		//bool LineFromAtoBPassesBetweenCandD (POINT2Dv *A, POINT2Dv *B, POINT2Dv *C, POINT2Dv *D);

		//bool PointsAreSameSideOfLine (IMPLICIT_LINE2 *Line, POINT2Dv *A, POINT2Dv *B);
		//bool PointsAreOppositeSideOfLine (IMPLICIT_LINE2 *Line, POINT2Dv *A, POINT2Dv *B);

		double GetAngle2D_PI(POINT2D P0, POINT2D P1);
		double GetAngle2D_PI(POINT2D P1, POINT2D Pivot, POINT2D P2);
		double GetAngle2D_PI(POINT2D T1, POINT2D H1,
							 POINT2D T2, POINT2D H2);
		//double GetAngle2D_PI_half(POINT2D P0, POINT2D P1);

		double GetDistancePointToLine(POINT2D V, POINT2D p0, POINT2D p1,
									  POINT2D &Intersection);
		double GetDistancePointToLine(POINT2D V, POINT2D p0, POINT2D p1);

		double GetDistancePointIntoSegment(double pointX, double pointY,
										   double x1, double y1,
										   double x2, double y2,
										   double &CloseX, double &CloseY);

		RECTANGLE2D GetBounds(std::vector <POINT2D> Points);

		//bool NearlyEquals(double A,double B,double percent);
		double NearlyParallel(POINT2D A1, POINT2D B1,
							  POINT2D A2, POINT2D B2, double Tolerance);
		double NearlyParallel(double a0x, double a0y,
							  double a1x, double a1y,
							  double b0x, double b0y,
							  double b1x, double b1y,
							  double ThresholdAngle);

		double NearlyCollinear(double a0x, double a0y,
							   double a1x, double a1y,
							   double b0x, double b0y,
							   double b1x, double b1y,
							   double ThresholdAngle, double ThresholdOffset);

		bool SegmentsIntersect(POINT2D a1, POINT2D b1,
							   POINT2D a2, POINT2D b2,
							   double &x, double &y);

		POINT2D GetCentroid(std::vector<POINT2D> Polygon);

		double crossProduct(const POINT2D p1, const POINT2D p2, const POINT2D p3);
		bool isPointOnSegment(const POINT2D p, const POINT2D p1, const POINT2D p2);
		bool isPointInPolygon(const std::vector<POINT2D> polygon, const POINT2D point);

		void equation_3_points_plane(double x1, double y1, double z1, //Point1
									 double x2, double y2, double z2, //Point2
									 double x3, double y3, double z3, //Point3
									 double &a, double &b, double &c, double &d);//Plane's parameters

#endif  // TOOLS_GEOMETRY