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

#include <cmath>
#include <cassert>
#include <vector>
#include <algorithm>  //min, max

#include "Tools_Geometry.h"


POINT2D::POINT2D()
:x (0),
 y (0)
{
}


POINT2D::POINT2D(const POINT2D& DP)
:x (DP.x),
 y (DP.y)
{
}


POINT2D::POINT2D(double cx, double cy)
:x (cx),
 y (cy)
{
}


POINT2D::~POINT2D()
{
}


bool POINT2D::operator==(const POINT2D& right) const
//Overload of == operator for POINT2D
{
	return (this->x==right.x && this->y==right.y);
}


bool POINT2D::operator!=(const POINT2D& right) const
//Overload of != operator for POINT2D
{
	return (this->x!=right.x || this->y!=right.y);
}


POINT2D& POINT2D::operator+=(const POINT2D& right)
//Overload of += operator for POINT2D
{
	this->x+= right.x;
	this->y+= right.y;
	return *this;
}


POINT2D POINT2D::operator+(const POINT2D& right) const
//Overload of + operator for POINT2D
{
	POINT2D p(this->x+right.x, this->y+right.y);
	return p;
	//return POINT2D (*this) += right;
}


POINT2D& POINT2D::operator-=(const POINT2D& right)
//Overload of -= operator for POINT2D
{
	this->x-= right.x;
	this->y-= right.y;
	return *this;
}


POINT2D POINT2D::operator-(const POINT2D& right) const
//Overload of - operator for POINT2D
{
	POINT2D p(this->x-right.x, this->y-right.y);
	return p;
	//return POINT2D (*this) -= right;
}


POINT2D POINT2D::operator*(double scale) const
//Overload of * operator for POINT2D and double value
{
	POINT2D p(this->x*scale, this->y*scale);
	return p;
}


double POINT2D::operator*(const POINT2D& right) const
//Overload of * operator for POINT2D
//Returs the scalar product of both (left and right) points

//In fact, it is the scalar product of the vectors whose tail is the origin
//and whose tips are the points
{
	return (this->x*right.x + this->y*right.y);
}


POINT2D POINT2D::operator/(double divide) const
//Overload of / operator for POINT2D and double value
{
	assert(divide!=0);
	POINT2D p(this->x/divide, this->y/divide);
	return p;
}


//POINT2D& POINT2D::operator*=(const POINT2D& right)
////Overload of *= operator for POINT2D
//{
//	this->x*= right.x;
//	this->y*= right.y;
//	return *this;
//}


//POINT2D POINT2D::operator*(const POINT2D& right) const
////Overload of * operator for POINT2D
//{
//	POINT2D p(this->x*right.x, this->y*right.y);
//	return p;
//	//return POINT2D (*this) *= right;
//}


//POINT2D& POINT2D::operator/=(const POINT2D& right)
////Overload of /= operator for POINT2D
//{
//	assert(right.x!=0);
//	assert(right.y!=0);
//	this->x/= right.x;
//	this->y/= right.y;
//	return *this;
//}


//POINT2D POINT2D::operator/(const POINT2D& right) const
////Overload of / operator for POINT2D
//{
//	assert(right.x!=0);
//	assert(right.y!=0);
//	POINT2D p(this->x/right.x, this->y/right.y);
//	return p;
//	//return POINT2D (*this) /= right;
//}


void POINT2D::assign(double x_new, double y_new)
//Assigns the new coordinates to the point
{
	x= x_new;
	y= y_new;
}


POINT2D POINT2D::Rotate(POINT2D Center, double beta)
//Returns coordinates of the point rotated and angle "beta" around the point "Center"
{
	POINT2D PRot;
	PRot.x= Center.x+((x-Center.x)*cos(beta)-(y-Center.y)*sin(beta));
	PRot.y= Center.y+((x-Center.x)*sin(beta)+(y-Center.y)*cos(beta));
	return PRot;
}


POINT2D POINT2D::Rotate(double beta)
//Returns coordinates of the point rotated and angle "beta" around the origin
{
	POINT2D PRot;
	PRot.x= x*cos(beta)-y*sin(beta);
	PRot.y= x*sin(beta)+y*cos(beta);
	return PRot;
}


SEGMENT2D::SEGMENT2D()
:TipBegin (0,0),
 TipEnd (0,0),
 Length (0.0),
 Slope180 (0.0)
{
}


SEGMENT2D::SEGMENT2D(const SEGMENT2D& DS)
:TipBegin (DS.TipBegin),
 TipEnd (DS.TipEnd),
 Length (DS.Length),
 Slope180 (DS.Slope180)
{
}


SEGMENT2D::SEGMENT2D(POINT2D cTipBegin, POINT2D cTipEnd)
:TipBegin (cTipBegin),
 TipEnd (cTipEnd),
 Length (0.0),
 Slope180 (0.0)
{
}


SEGMENT2D::SEGMENT2D(NumPOINT2D cTipBegin, NumPOINT2D cTipEnd)
:TipBegin (cTipBegin.Point),
 TipEnd (cTipEnd.Point),
 Length (0.0),
 Slope180 (0.0)
{
}


SEGMENT2D::SEGMENT2D(double cTipBeginx, double cTipBeginy, double cTipEndx, double cTipEndy)
:TipBegin (cTipBeginx, cTipBeginy),
 TipEnd (cTipEndx, cTipEndy),
 Length (0.0),
 Slope180 (0.0)
{
}


SEGMENT2D::~SEGMENT2D()
{
}


void SEGMENT2D::clear()
{
	TipBegin.assign(0,0);
	TipEnd.assign(0,0);
	Length= 0.0;
	Slope180= 0.0;
}


void SEGMENT2D::Rotate(POINT2D Center, double beta)
//Returns coordinates of the point rotated and angle "beta" around the point "Center"
{
	TipBegin= TipBegin.Rotate(Center, beta);
	TipEnd= TipEnd.Rotate(Center, beta);
}


bool SEGMENT2D::operator==(const SEGMENT2D& right) const
//Overload of == operator for SEGMENT2D
{
	return (this->TipBegin==right.TipBegin && this->TipEnd==right.TipEnd  &&
			this->Length==right.Length && this->Slope180==right.Slope180);
}


bool SEGMENT2D::operator!=(const SEGMENT2D& right) const
//Overload of != operator for SEGMENT2D
{
	return (this->TipBegin!=right.TipBegin || this->TipEnd!=right.TipEnd);
			//this->Length!=right.Length || this->Slope180!=right.Slope180);
}

bool SEGMENT2D::intersects (const SEGMENT2D & q) const
//Given two segments (TipBegin,Tipend) and (q.TipBegin,q.TipEnd), 
//they intersect if and only if the orientation of the triplet (TipBegin,p1,qTipBegin)
//is different from the orientation of the triplet (TipBegin,TipEnd,qTipEnd) 
//and the orientation of the triplet (qTipBegin,qTipEnd,TipBegin)
//is different from the orientation of the triplet (qTipBegin,qTipEnd,TipEnd).
//The first condition means that qTipBegin is on one side of the segment (TipBegin,TipEnd),
//and qTipEnd is on the other side. 
//The second condition means that TipBegin is on one side of the segment (qTipBegin,qTipEnd), 
//and TipEnd is on the other side.

//Clearly, the segments intersect if and only if both conditions are met
{
	return turn (TipBegin, TipEnd, q.TipBegin) != turn (TipBegin, TipEnd, q.TipEnd) &&
			turn (q.TipBegin, q.TipEnd, TipBegin) != turn (q.TipBegin, q.TipEnd, TipEnd);
}


bool SEGMENT2D::contains (const POINT2D & p) const
// If the scalar product is negative, it means 
// TipBegin-p and Tipend-p are in opposite senses;  therefore, 
// p is between TipBegin and TipEnd
{
	const int collinear = 0;
	return turn (TipBegin, TipEnd, p) == collinear &&
				((p-TipBegin) * (p-TipEnd) < 0);
}


bool SEGMENT2D::contains (const NumPOINT2D & p) const
// If the scalar product is negative, it means 
// TipBegin-p and TipEnd-p are in opposite senses;  therefore, 
// p is between TipBegin and TipEnd
{
	const int collinear = 0;
	return turn (TipBegin, TipEnd, p.Point) == collinear &&
				((p.Point-TipBegin) * (p.Point-TipEnd) < 0);
}


TRIANGLE2D::TRIANGLE2D (const NumPOINT2D & p1 = NumOrigin, 
						const NumPOINT2D & p2 = NumOrigin, 
						const NumPOINT2D & p3 = NumOrigin)
: v1 (p1),
  v2 (p2),
  v3 (p3)
{
}


TRIANGLE2D::~TRIANGLE2D()
{
}


bool TRIANGLE2D::operator== (const TRIANGLE2D & t) const
{
	return v1.Point == t.v1.Point && v2.Point == t.v2.Point && v3.Point == t.v3.Point;
}


bool TRIANGLE2D::operator!= (const TRIANGLE2D & t) const
{
	return !(*this == t);
}


double TRIANGLE2D::signed_2area () const
//Given three vertices of a triangle v1,v2,v3 in the x-y plane,
//the z-coordinate of the vector product between v2-v1 (v2 minus v1) and v3-v2
//is the signed area, which is:
//	(positive if vector v2 is "at the left" of vector v1, 
//	zero if v1 and v2 are parallel, 
//	negative if v2 is "at the right" of v1.

//The magnitude of z is twice the area of the triangle v1,v2,v3.

//The sign of z tells whether the triplet v1,v2,v3 represents:
//	a right-turn or left-turn
//	(that is, if the point v3 is at the right
//	or at the left of the oriented segment from v1 to v2)
{
	return (v1.Point.x * (v2.Point.y - v3.Point.y) +
			v2.Point.x * (v3.Point.y - v1.Point.y) +
			v3.Point.x * (v1.Point.y - v2.Point.y));
}


bool TRIANGLE2D::contains (const NumPOINT2D & p) const
//Given a triangle (v1,v2,v3) and a point p, 
//the test for p's inclusion in the triangle is explained as follows.
//If we traverse the points v1,v2,v3 and the point is inside, 
//we will always see the point on the same side of the segment we are visiting. 
//If v1,v2,v3 are arranged in a counter-clockwise sense, 
//the points inside it are always at the left of the segments. 
//If the point is outside, for at least one of the segments the point will be at the right 
//If the vertices are arranged in a clockwise sense, 
//the reasoning is identical, except that a point that is inside the triangle 
//will always be at the right of the segment we are visiting.

//Thus, to determine if the point p is inside the triangle (v1,v2,v3),
//we need only obtain the directions of rotation along the triplets (v1,v2,p), (v2,v3,p) and (v3,v1,p).
//The point is inside if and only if the three directions are equal.
{
	return turn (v1.Point, v2.Point, p.Point) == turn (v2.Point, v3.Point, p.Point) &&
			turn (v2.Point, v3.Point, p.Point) == turn (v3.Point, v1.Point, p.Point);
}


double TRIANGLE2D::area () const
{
	return std::fabs (signed_2area()/2.);
}


int TRIANGLE2D::orientation () const
{
	double s_a = signed_2area();

	return s_a > 0 ? 1 : (s_a < 0 ? -1 : 0);
}



RECTANGLE2D::RECTANGLE2D()
:left (0),
 right (0),
 bottom (0),
 top (0)
{
}


RECTANGLE2D::RECTANGLE2D(const RECTANGLE2D& DR)
:left (DR.left),
 right (DR.right),
 bottom (DR.bottom),
 top (DR.top)
{
}


RECTANGLE2D::RECTANGLE2D(double cleft, double cright, double cbottom, double ctop)
:left (cleft),
 right (cright),
 bottom (cbottom),
 top (ctop)
{
}


RECTANGLE2D::~RECTANGLE2D()
{
}


POLYGON2D::POLYGON2D ()
{
	this->clear();
}


POLYGON2D::POLYGON2D (std::vector<NumPOINT2D> &p)
{
	for(long i = 1; i < (long)p.size(); i++){
		this->push_back(p[i]);
	}
}


POLYGON2D::~POLYGON2D()
{
}


POLYGON2D::iterator POLYGON2D::begin()
{
	return iterator (vertices.begin(), vertices.begin(), vertices.end());
}


POLYGON2D::const_iterator POLYGON2D::begin() const
{
	return const_iterator (vertices.begin(), vertices.begin(), vertices.end());
}


void POLYGON2D::push_back (const NumPOINT2D & p)
{
	vertices.push_back (p);
}


void POLYGON2D::push_front (const NumPOINT2D & p)
{
	vertices.push_front (p);
}


void POLYGON2D::insert (const iterator & i, const NumPOINT2D & p)
{
	vertices.insert (i.get_i(), p);
}


POLYGON2D::iterator POLYGON2D::remove (const iterator & target)
{
	if (!(vertices.empty()))
	{
		std::list<NumPOINT2D>::iterator ret_value;

		ret_value = vertices.erase (target.get_i());
		if (ret_value == vertices.end())
		{
			return iterator (vertices.begin(), vertices.begin(), vertices.end());
		}
		else
		{
			return iterator (ret_value, vertices.begin(), vertices.end());
		}
	}
	else
	{
		return iterator();	// return "null" iterator
	}
}


void POLYGON2D::clear ()
{
	do
	{
		this->remove(this->begin());
	}
	while (!vertices.empty());
}


int POLYGON2D::size ()
{
	return (long)vertices.size();
}


NumPOINT2D POLYGON2D::get (iterator current)
{
	NumPOINT2D v = *current;
	return v;
}


//bool POLYGON2D::containsPoint (const NumPOINT2D & p) const
////Ray casting approach:
////To test if a point p is inside a polygon P, 
////we need find a point that is outside the polygon, 
////and count how many times the line L from p to the point outside intersects the polygon.
////The point is inside the polygon if the number of intersections is odd.
////If it's even, it's out.
//
////To find a point outside the polygon, 
////we obtain the vertex with highest x-coordinate and we add one to that x-coordinate.
//
////Based on:
////	Hormann K., Agathas A. (2001)
////	The point in polygon problem for arbitrary polygons
////	Computational Geometry. 20(3), pp 131-144.
////	DOI: 10.1016/S0925-7721(01)00012-8.
//{
//	// Find a point outside the polygon
//	//(point with max x + (1,0))
//	const_iterator current = begin();
//	NumPOINT2D p_max_x = *current++;
//	do
//	{
//		if ((*current).Point.x > p_max_x.Point.x)
//		{
//			p_max_x = *current;
//		}
//		current++;
//	}
//	while (current != begin());
//
//	NumPOINT2D outside = p_max_x;
//	outside.Point.x += 1.;
//	outside.Point.y += 0.;
//	SEGMENT2D line (p, outside);
//
//	// Now check the number of intersections of segment 
//	// p - outside with the edges of the polygon
//
//	// The loop must start in a vertex that is not exactly
//	// on the segment 'line'.
//	int intersections = 0;
//	current = begin();
//
//	//while ((*current).is_on (line))  current++;
//	while (line.contains(*current))  current++;
//		POLYGON2D::const_iterator first_point = current;
//
//	do
//	{
//		NumPOINT2D v = *current++;
//		if (SEGMENT2D (v, *current).intersects (line))
//		{
//			intersections++;
//		}
//
//		//if ((*current).is_on (line))
//		if (line.contains(*current))
//		{
//			//while ((*++current).is_on (line));
//			while (line.contains(*++current));
//				// Skip all the vertices that are on the line
//
//			if (turn(line.TipBegin, line.TipEnd, v.Point) == 
//				turn (line.TipBegin, line.TipEnd, (*current).Point))
//			{
//				intersections--;
//			}
//			// If previous and next points are on the same 
//			// side of line, then line doesn't intersect the 
//			// polygon;  it merely touches it at the vertex.
//			// Since one intersection was counted by the test 
//			// with the first edge, decrement the count.
//		}
//	}
//	while (current != first_point);
//
//	if (intersections % 2)  // if odd number of intersections
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}


int POLYGON2D::polygonType()
//Classifies a polygon:
//	Returns 0 if the polygon is complex (has intersecting edges)
//	Returns +-1 if it is simple and convex
//	Returns +-2 if it is simple and concave

//The sign of the returned value indicates whether the polygon is:
//	+ CCW (counter clockwise) polygon,
//	- CW (clockwise) polygon,

//Copyright from:
//	W.H. Press, S.A. Teukolsky,, W.T. Vetterling, B.P. Flannery
//	Numerical recipes
//	Section 21.4.3 Polygons
//	Page 1125. Third edition.
{
	int schg= 0, wind= 0;
	double crsp= 0.0;

	POLYGON2D::iterator current;
	current = this->begin();
	NumPOINT2D pBegin= this->get(current);
	double pB0= pBegin.Point.x;
	double pB1= pBegin.Point.y;
	NumPOINT2D pEnd= this->get(current-1);
	double pE0= pEnd.Point.x;
	double pE1= pEnd.Point.y;

	double p0= pB0 - pE0;
	double p1= pB1 - pE1;

	for(int i=0; i<this->size(); i++) {
		NumPOINT2D pi= this->get(current);
		NumPOINT2D pii= this->get(current+1);
		double d0= pii.Point.x - pi.Point.x;
		double d1= pii.Point.y - pi.Point.y;

		double crs= p0*d1 - p1*d0; //cross product at current vertex
		if(crs*crsp < 0) schg= 1;  //Sign change (i.e. concavity) found.

		if(p1 <= 0.0){
			if(d1 > 0.0 && crs > 0.0) wind++;
		}
		else{
			if(d1 <= 0.0 && crs < 0.0) wind--;
		}

		//Current vertex becomes previous one.
		p0= d0;
		p1= d1;
		if(crs != 0.0) crsp= crs;
		current++;
	}

	if(std::abs(wind) != 1) return 0;  //Can already conclude polygon is complex
	if(schg == 0) return (wind>0? 1 : -1);  //Polygon is simple and convex

	//We have exhausted all the quick tricks and how have to check all pairs of edges for intersections
	POLYGON2D::iterator currenti;
	currenti = this->begin();
	for(int i=0; i<this->size(); i++) {
		NumPOINT2D pi= this->get(currenti);
		double p0= pi.Point.x;
		double p1= pi.Point.y;
		NumPOINT2D pii= this->get(currenti+1);
		double d0= pii.Point.x;
		double d1= pii.Point.y;

		double tp= 0;
		POLYGON2D::iterator currentj;
		currentj = currenti+1;
		for(int j= i+1; j<this->size(); j++) {
			if((j+1) == this->size()) {if (i==0) break;}
			NumPOINT2D pjj= this->get(currentj+1);
			double dd0= pjj.Point.x;
			double dd1= pjj.Point.y;

			double t= (dd0-d0)*(p1-d1) - (dd1-d1)*(p0-d0);
			if(t*tp <= 0.0 && j > i+1){
				NumPOINT2D pj= this->get(currentj);
				double pp0= pj.Point.x;
				double pp1= pj.Point.y;
				double t1= (p0-dd0)*(pp1-dd1) - (p1-dd1)*(pp0-dd0);
				double t2= (d0-dd0)*(pp1-dd1) - (d1-dd1)*(pp0-dd0);
				if(t1*t2 <= 0.0) return 0;   //Found intersection, so done
			}
			tp= t;
			currentj++;
		}
		currenti++;
	}
	return (wind>0? 2 : -2); // No intersection found, so simple concave
}


std::vector<long> POLYGON2D::ConvexHull()
//Melkman proposed an extremely elegant and straightforward algorithm 
//to compute (in linear time) the convex hull of a simple polygonal chain 
//(which can obviously be used for a simple polygon). 
//The key idea in this algorithm is to maintain an updated version of the convex hull, 
//and for each new point encountered in the input polygonal chain, 
//we test if the convex hull needs to be updated.

//The algorithm starts by sending to the output
//the first three points of the input polygon (with the appropriate orientation).

//At the step n of the algorithm, we test if the new point 
//(point number n+1 of the input polygon) is outside the convex hull.
//If it is, then the convex hull needs to be updated to account for the new point. 
//If not, the new point is discarded and the output remains the same.
{
	POLYGON2D hull;
	POLYGON2D::const_iterator input_iter = this->begin();
	POLYGON2D::iterator hull_iter;

	//The first three points go directly to the output
	NumPOINT2D p1 = *input_iter++;
	NumPOINT2D p2 = *input_iter++;
	NumPOINT2D p3 = *input_iter++;

	int left_turn = +1;
	int right_turn = -1;

	//Make sure the output is in counter-clockwise sense
	//if (turn (p1, p2, p3) == left_turn){
		hull.push_back (p1);
		hull.push_back (p2);
		hull.push_back (p3);
		hull.push_front (p3); //The last point has to be always inserted at both ends
	//}
	//else{
	////WARNING: this works bad for concave vertices!!!
	//	hull.push_back (p2);
	//	hull.push_back (p1);
	//	hull.push_back (p3);
	//	hull.push_front (p3); //The last point has to be always inserted at both ends
	//}

	do{
		POLYGON2D::iterator first = hull.begin();
		POLYGON2D::iterator last = first - 1; // The list is circular

		NumPOINT2D p = *input_iter++;

		if (turn (*(first+1), *first, p) == left_turn ||
			turn (*(last-1), *last, p) == right_turn){
			hull.push_back (p);
			hull.push_front (p);

			//Now fix the possible concavities after insertion 
			//of new point (on both ends of the output list)

			first = hull.begin();
			last = first-1;

			while (turn (*first, *(first+1), *(first+2)) == right_turn){
				hull.remove (first+1);
			}

			while (turn (*last, *(last-1), *(last-2)) == left_turn){
				hull.remove (last-1);
			}
		}
	}
	while (input_iter != begin());

	//Remove repeated point  (1st pt. = last pt.)
	hull.remove (hull.begin());

	//Check if original orientation has been altered while calculating the hull
	if(hull.polygonType() * this->polygonType() < 0)
		hull.ReversePolygon();

	//Save the numbers of the vertices that belong to the convex hull
	std::vector<long> Hull;
	iterator current = hull.begin();
	for(long i = 0; i < size(); i++){
		NumPOINT2D p= *current;
		Hull.push_back((*current).Number);
		current++;
	}

	return Hull;
}


void POLYGON2D::ReversePolygon()
// Reverses the order of the vertices of the polygon

//This is typically done to prevent attempts to triangulate CW polygons
{
	std::list<NumPOINT2D> ReversedVertices;

	const_iterator current = begin();
	current--;
	for(long i = 0; i < size(); i++){
		NumPOINT2D p= *current--;
		ReversedVertices.push_back(p);
	}

	vertices= ReversedVertices;

	return;
}


bool POLYGON2D::triangulate (std::vector<TRIANGLE2D> & output)
//Triangulate simple polygons (both convex or concave),
//located in a two-dimensional plane, with vertices defined by two-dimensional points

//The approach requires the vertices of the polygon arranged in counter clockwise order (CCW)

//Adapted from:
//Author: Carlos Moreno
//		https://mochima.com/articles/cuj_geometry_article/cuj_geometry_article.html
//
//		Carlos Moreno (1998) Efficient 2-D Geometric Operations, part 1.
//		C/C++ Users Journal, Volume 16, Issue 11, Nov. 1998, pp 25–36. 
//		https://dl.acm.org/doi/10.5555/297521.297525
//
//		Carlos Moreno (1998) Efficient 2-D Geometric Operations, part 2.
//		C/C++ Users Journal, Volume 16, Issue 12, Dec. 1998, pp 21–35. 
//		https://dl.acm.org/doi/10.5555/306490.306492
//
//		(modified from Herbert Schildt's 
//		 "Windows NT 4 Programming from the Ground Up"
//		 ISBN-13: 978-0078825736)
{
	POLYGON2D P = *this;    // Make a working copy

	output.clear ();	// Only the first time.  Recursive 
						// calls should not clear the list 
						// of triangles

	return recursive_triangulation (P, output);
}


static bool recursive_triangulation (POLYGON2D & P, std::vector<TRIANGLE2D> & output)
//The algorithm consists of finding convex vertices in the input polygon. 
//For each convex vertex that we find, 
//we consider the triangle Vprev-Vcurr-Vnext such that 
	//Vprev is the vertex previous to the convex vertex; 
	//Vcurr is the convex vertex; 
	//and Vnext is the vertex succeding the convex vertex. 

//We now test if this triangle contains any other vertices of the input polygon. 
//If the triangle does not contain any other vertex of the input polygon, 
//then it is guaranteed that the segment Vprev,Vnext does not intersect any edge of the polygon, 
//therefore, the triangle is sent to the output
//and the vertex Vcurr is removed from the polygon. 
//If the triangle does contain other vertices of the input polygon, 
//we find one of these vertices such that it can be joined to the vertex Vcurr 
//without intersecting any edges of the polygon. 
//If we call that vertex Vs, 
//then the segment (Vcurr,Vs) divides the input polygon into two simple polygons. 
//At this point, we recursively call the function to triangulate each half of the polygon. 

//The stop condition of the recursive process is, of course, 
//when the input polygon has three vertices (i.e., when it is already a triangle).

//The step of finding a vertex that can be joined to the vertex Vcurr in the triangle
//is not as obvious as it looks like. 
//Many wrong approaches have been used in the past, 
//including the most intuitive, which is finding the nearest vertex to Vcurr. 
//This approach fails for triangles that intersect the input polygon. 
//The appropriate way to find such vertex is to find the point with
//highest perpendicular distance to the straight line containing the segment Vprev, Vnext.

//Naturally, to obtain this point the algorithm does not directly compute a single distance 
//(it would be too hard, since we are talking about perpendicular distance). 
//Instead, the algorithm finds the point P for which the area of the triangle (Vprev,Vnext,P) is maximized.
{
	POLYGON2D::iterator current = P.begin();

	//Detect triangular polygons
	if ((current+3) == current)   // If polygon contains 3 vertices
	{
		output.push_back (TRIANGLE2D (*current, *(current+1), *(current+2)));
		return true;
	}

	//Triangulate polygon
	int left_turn = +1;
	int right_turn = -1;
	int counter_clockwise = left_turn;
	int clockwise = right_turn;

	long counter= 0;
	do
	{
		TRIANGLE2D t (*(current-1), *current, *(current+1));
		if (t.orientation() == counter_clockwise)   // if convex vertex
		{
			bool empty_triangle = true;
			POLYGON2D::const_iterator i = current+2, 
									farthest_point = current-1;
			do
			{
				if (t.contains (*i))
				{
					empty_triangle = false;
					if (TRIANGLE2D(*(current+1), *(current-1), *i).area() > 
						TRIANGLE2D(*(current+1), *(current-1), *farthest_point).area())
					{
						farthest_point = i;
					}
				}
				i++;
			}
			while (i != current-1);

			if (empty_triangle)     // send it to the triangulation
			{
				output.push_back (t);
				current = P.remove (current);
				current--;
				if ((current+3) == current)     // If P became a triangle, done
				{
					output.push_back (TRIANGLE2D (*current, *(current+1), *(current+2)));
					return true;
				}
			}

			else    // t not empty ==> split the polygon and triangulate both halves
			{
				POLYGON2D first_half, second_half;

				for (i = current; i != farthest_point; i++)
				{
					first_half.push_back (*i);
				}
				first_half.push_back (*farthest_point);

				recursive_triangulation (first_half, output);

				for (i = farthest_point; i != current; i++)
				{
					second_half.push_back (*i);
				}
				second_half.push_back (*current);

				recursive_triangulation (second_half, output);

				return true;
			}
		}
		else{
			counter++;  //To prevent infinite loop (that may arise for CW polygons)
		}
		current++;
	}
	while (counter < 1000);   //Arbitrary limit: Faces with more than 1000 vertices could be incompletely processed

	return false;
}


//double det3x3 (double v[])
//// Treat a row of doubles as a 3x3 matrix and return the determinant
//{
//   return v[0]*(v[4]*v[8]-v[5]*v[7])-v[1]*(v[3]*v[8]-v[5]*v[6])+v[2]*(v[3]*v[7]-v[4]*v[6]);
//}


//void invert3x3 (double input[], double output[])
// Invert a 3x3 matrix
//{
//   double a = input[0];
//   double b = input[1];
//   double c = input[2];
//   double d = input[3];
//   double e = input[4];
//   double f = input[5];
//   double g = input[6];
//   double h = input[7];
//   double i = input[8];
//   double det = a*(e*i-f*h)-b*(d*i-f*g)+c*(d*h-e*g);
//   output[0] = (e*i-f*h)/det;
//   output[1] = (c*h-b*i)/det;
//   output[2] = (b*f-c*e)/det;
//   output[3] = (f*g-d*i)/det;
//   output[4] = (a*i-c*g)/det;
//   output[5] = (c*d-a*f)/det;
//   output[6] = (d*h-e*g)/det;
//   output[7] = (b*g-a*h)/det;
//   output[8] = (a*e-b*d)/det;
//}
//
//
//double cofactor3x3 (double values[], long index)
// Treat a row of doubles as a 3x3 matrix and return the cofactor of a particular element
//{
//   double a = values[0];
//   double b = values[1];
//   double c = values[2];
//   double d = values[3];
//   double e = values[4];
//   double f = values[5];
//   double g = values[6];
//   double h = values[7];
//   double i = values[8];
//
//   switch (index)
//   {
//      case 0: return e*i-f*h;
//      case 3: return f*g-d*i;
//      case 6: return d*h-e*g;
//      case 1: return h*c-b*i;
//      case 4: return i*a-c*g;
//      case 7: return g*b-h*a;
//      case 2: return b*f-e*c;
//      case 5: return c*d-a*f;
//      case 8: return a*e-b*d;
//      default: return 0;
//   }
//
//
//void ExpressDAsSumOfABC (VECTOR3D *A, VECTOR3D *B, VECTOR3D *C, VECTOR3D *D, VECTOR3D *E)
//// Although this is 2D geometry, it uses vector3s for convenience because that's
//// what the calling program uses
//{
//   double   Mat[9];
//   double   Det;
//   Mat[0] = A->x;
//   Mat[1] = B->x;
//   Mat[2] = C->x;
//   Mat[3] = A->y;
//   Mat[4] = B->y;
//   Mat[5] = C->y;
//   Mat[6] = 1;
//   Mat[7] = 1;
//   Mat[8] = 1;
//   Det = det3x3(Mat);
//
//	Det= A->x*(B->y*1-C->y*1)-B->x*(A->y*1-C->y*1)+C->x*(A->y*1-B->y*1);
//
//   E->x = (cofactor3x3(Mat,0)*D->x + cofactor3x3(Mat,1)*D->y + cofactor3x3(Mat,2))/Det;
//   E->y = (cofactor3x3(Mat,3)*D->x + cofactor3x3(Mat,4)*D->y + cofactor3x3(Mat,5))/Det;
//   E->z = (cofactor3x3(Mat,6)*D->x + cofactor3x3(Mat,7)*D->y + cofactor3x3(Mat,8))/Det;
//}


//double ZToMakeDCoplanarWithABC (VECTOR3D *A, VECTOR3D *B, VECTOR3D *C, VECTOR3D *D)
//{
//   double   Mat[9];
//   double   Det;
//   Mat[0] = A->x;
//   Mat[1] = B->x;
//   Mat[2] = C->x;
//   Mat[3] = A->y;
//   Mat[4] = B->y;
//   Mat[5] = C->y;
//   Mat[6] = 1;
//   Mat[7] = 1;
//   Mat[8] = 1;
//   Det = det3x3(Mat);
//   return ((cofactor3x3(Mat,0)*D->x + cofactor3x3(Mat,1)*D->y + cofactor3x3(Mat,2))*A->z + (cofactor3x3(Mat,3)*D->x + cofactor3x3(Mat,4)*D->y + cofactor3x3(Mat,5))*B->z + (cofactor3x3(Mat,6)*D->x + cofactor3x3(Mat,7)*D->y + cofactor3x3(Mat,8))*C->z)/Det;
//}
//
//
//bool QuadrilateralIsConvex (POINT2Dv *A, POINT2Dv *B, POINT2Dv *C, POINT2Dv *D)
//{
//   IMPLICIT_LINE2 Lac, Lbd;
//   POINT2Dv Cross;
//   FindImplicitLine(&Lac,A,C);
//   FindImplicitLine(&Lbd,B,D);
//   if (!Intercept(&Lac,&Lbd,&Cross))  return false;
//   if (DistanceFromPoint2ToPoint2(&Cross,A) >= DistanceFromPoint2ToPoint2(A,C))  return false;
//   if (DistanceFromPoint2ToPoint2(&Cross,B) >= DistanceFromPoint2ToPoint2(B,D))  return false;
//   if (DistanceFromPoint2ToPoint2(&Cross,C) >= DistanceFromPoint2ToPoint2(A,C))  return false;
//   if (DistanceFromPoint2ToPoint2(&Cross,D) >= DistanceFromPoint2ToPoint2(B,D))  return false;
//   return true;
//}
//
//
//bool LineFromAtoBPassesBetweenCandD (POINT2Dv *A, POINT2Dv *B, POINT2Dv *C, POINT2Dv *D)
//{
//   return QuadrilateralIsConvex(A,C,B,D);
//}
//
//
//bool PointsAreSameSideOfLine (IMPLICIT_LINE2 *Line, POINT2Dv *A, POINT2Dv *B)
//{
//   double Xa = Line->Normal.x*A->x+Line->Normal.y*A->y+Line->Distance;
//   double Xb = Line->Normal.x*B->x+Line->Normal.y*B->y+Line->Distance;
//   return (Xa*Xb) > 0.0;
//}
//
//
//bool PointsAreOppositeSideOfLine (IMPLICIT_LINE2 *Line, POINT2Dv *A, POINT2Dv *B)
//{
//   double Xa = Line->Normal.x*A->x+Line->Normal.y*A->y+Line->Distance;
//   double Xb = Line->Normal.x*B->x+Line->Normal.y*B->y+Line->Distance;
//   return (Xa*Xb) < 0.0;
//}



//***********************************************************************

 /* Code to find the equation of a conic */
 /*               Tom Davis              */
 /*             April 12, 1996           */


//#include <stdio.h>
//#define FLOAT double

//#define cross(a, b, ab) ab[0] = a[1]*b[2] - a[2]*b[1]; \
//			ab[1] = a[2]*b[0] - a[0]*b[2]; \
//			ab[2] = a[0]*b[1] - a[1]*b[0];

//#define std::fabs(x) ((x > 0.0) ? x : (-x))   // WARNING!!  This define will replace calls to stdlib std::fabs!!!

/* toconic takes five points in homogeneous coordinates, and returns the
 * coefficients of a general conic equation in a, b, c, ..., f:
 * 
 * a*x*x + b*x*y + c*y*y + d*x + e*y + f = 0.
 * 
 * The routine returns 1 on success; 0 otherwise.  (It can fail, for
 * example, if there are duplicate points.
 * 
 * Typically, the points will be finite, in which case the third (w)
 * coordinate for all the input vectors will be 1, although the code
 * deals cleanly with points at infinity.
 * 
 * For example, to find the equation of the conic passing through (5, 0), 
 * (-5, 0), (3, 2), (3, -2), and (-3, 2), set:
 * 
 * p0[0] =  5, p0[1] =  0, p0[2] = 1, 
 * p1[0] = -5, p1[1] =  0, p1[2] = 1, 
 * p2[0] =  3, p2[1] =  2, p2[2] = 1, 
 * p3[0] =  3, p3[1] = -2, p3[2] = 1, 
 * p4[0] = -3, p4[1] =  2, p4[2] = 1.
 *
 * But if you want the equation of the hyperbola that is tangent to the
 * line 2x=y at infinity,  simply make one of the points be the point at
 * infinity along that line, for example:
 *
 * p0[0] = 1, p0[1] = 2, p0[2] = 0.
 */

//int toconic(FLOAT p0[3], FLOAT p1[3], FLOAT p2[3], FLOAT p3[3], FLOAT p4[3], 
//	FLOAT *a, FLOAT *b, FLOAT *c, FLOAT *d, FLOAT *e, FLOAT *f)
//{
//	FLOAT L0[3], L1[3], L2[3], L3[3];
//	FLOAT A, B, C, Q[3];
//	FLOAT a1, a2, b1, b2, c1, c2;
//	FLOAT x0, x4, y0, y4, w0, w4;
//	FLOAT aa, bb, cc, dd, ee, ff;
//	FLOAT y4w0, w4y0, w4w0, y4y0, x4w0, w4x0, x4x0, y4x0, x4y0;
//	FLOAT a1a2, a1b2, a1c2, b1a2, b1b2, b1c2, c1a2, c1b2, c1c2;
//
//	cross(p0, p1, L0)
//	cross(p1, p2, L1)
//	cross(p2, p3, L2)
//	cross(p3, p4, L3)
//	cross(L0, L3, Q)
//	A = Q[0]; B = Q[1]; C = Q[2];
//	a1 = L1[0]; b1 = L1[1]; c1 = L1[2];
//	a2 = L2[0]; b2 = L2[1]; c2 = L2[2];
//	x0 = p0[0]; y0 = p0[1]; w0 = p0[2];
//	x4 = p4[0]; y4 = p4[1]; w4 = p4[2];
//
//	y4w0 = y4*w0;
//	w4y0 = w4*y0;
//	w4w0 = w4*w0;
//	y4y0 = y4*y0;
//	x4w0 = x4*w0;
//	w4x0 = w4*x0;
//	x4x0 = x4*x0;
//	y4x0 = y4*x0;
//	x4y0 = x4*y0;
//	a1a2 = a1*a2;
//	a1b2 = a1*b2;
//	a1c2 = a1*c2;
//	b1a2 = b1*a2;
//	b1b2 = b1*b2;
//	b1c2 = b1*c2;
//	c1a2 = c1*a2;
//	c1b2 = c1*b2;
//	c1c2 = c1*c2;
//
//	aa = -A*a1a2*y4w0
//		 +A*a1a2*w4y0 
//		 -B*b1a2*y4w0
//		 -B*c1a2*w4w0
//		 +B*a1b2*w4y0
//		 +B*a1c2*w4w0
//		 +C*b1a2*y4y0
//		 +C*c1a2*w4y0
//		 -C*a1b2*y4y0
//		 -C*a1c2*y4w0;
//
//	cc =  A*c1b2*w4w0
//		 +A*a1b2*x4w0
//		 -A*b1c2*w4w0
//		 -A*b1a2*w4x0
//		 +B*b1b2*x4w0
//		 -B*b1b2*w4x0
//		 +C*b1c2*x4w0
//		 +C*b1a2*x4x0
//		 -C*c1b2*w4x0
//		 -C*a1b2*x4x0;
//
//	ff =  A*c1a2*y4x0
//		 +A*c1b2*y4y0
//		 -A*a1c2*x4y0
//		 -A*b1c2*y4y0
//		 -B*c1a2*x4x0
//		 -B*c1b2*x4y0
//		 +B*a1c2*x4x0
//		 +B*b1c2*y4x0
//		 -C*c1c2*x4y0
//		 +C*c1c2*y4x0;
//
//	bb =  A*c1a2*w4w0
//		 +A*a1a2*x4w0
//		 -A*a1b2*y4w0
//		 -A*a1c2*w4w0
//		 -A*a1a2*w4x0
//		 +A*b1a2*w4y0
//		 +B*b1a2*x4w0
//		 -B*b1b2*y4w0
//		 -B*c1b2*w4w0
//		 -B*a1b2*w4x0
//		 +B*b1b2*w4y0
//		 +B*b1c2*w4w0
//		 -C*b1c2*y4w0
//		 -C*b1a2*x4y0
//		 -C*b1a2*y4x0
//		 -C*c1a2*w4x0
//		 +C*c1b2*w4y0
//		 +C*a1b2*x4y0
//		 +C*a1b2*y4x0
//		 +C*a1c2*x4w0;
//
//	dd = -A*c1a2*y4w0
//		 +A*a1a2*y4x0
//		 +A*a1b2*y4y0
//		 +A*a1c2*w4y0
//		 -A*a1a2*x4y0
//		 -A*b1a2*y4y0
//		 +B*b1a2*y4x0
//		 +B*c1a2*w4x0
//		 +B*c1a2*x4w0
//		 +B*c1b2*w4y0
//		 -B*a1b2*x4y0
//		 -B*a1c2*w4x0
//		 -B*a1c2*x4w0
//		 -B*b1c2*y4w0
//		 +C*b1c2*y4y0
//		 +C*c1c2*w4y0
//		 -C*c1a2*x4y0
//		 -C*c1b2*y4y0
//		 -C*c1c2*y4w0
//		 +C*a1c2*y4x0;
//
//	ee = -A*c1a2*w4x0
//		 -A*c1b2*y4w0
//		 -A*c1b2*w4y0
//		 -A*a1b2*x4y0
//		 +A*a1c2*x4w0
//		 +A*b1c2*y4w0
//		 +A*b1c2*w4y0
//		 +A*b1a2*y4x0
//		 -B*b1a2*x4x0
//		 -B*b1b2*x4y0
//		 +B*c1b2*x4w0
//		 +B*a1b2*x4x0
//		 +B*b1b2*y4x0
//		 -B*b1c2*w4x0
//		 -C*b1c2*x4y0
//		 +C*c1c2*x4w0
//		 +C*c1a2*x4x0
//		 +C*c1b2*y4x0
//		 -C*c1c2*w4x0
//		 -C*a1c2*x4x0;
//
//	if (aa != 0.0) {
//		bb /= aa; cc /= aa; dd /= aa; ee /= aa; ff /= aa; aa = 1.0;
//	}
//	else if (bb != 0.0) {
//		cc /= bb; dd /= bb; ee /= bb; ff /= bb; bb = 1.0;
//	}
//	else if (cc != 0.0) {
//		dd /= cc; ee /= cc; ff /= cc; cc = 1.0;
//	}
//	else if (dd != 0.0) {
//		ee /= dd; ff /= dd; dd = 1.0;
//	}
//	else if (ee != 0.0) {
//		ff /= ee; ee = 1.0;
//	}
//	else {
//		return 0;
//	}
//	*a = aa;
//	*b = bb;
//	*c = cc;
//	*d = dd;
//	*e = ee;
//	*f = ff;
//	return 1;
//}


//////FLOAT p0[3] = {0, 0, 1};
//////FLOAT p1[3] = {1, 1, 1};
//////FLOAT p2[3] = {-1, -1, 1};
//////FLOAT p3[3] = {2, 2, 1};
//////FLOAT p4[3] = {3, 3, 1};
//////
//////main()
//////{
//////    FLOAT a, b, c, d, e, f, s0, s1, s2, s3, s4;
//////    FLOAT x, y, w;
//////    int i, ret;
//////
///////*
//////    ret = toconic(p0, p1, p2, p3, p4, &a, &b, &c, &d, &e, &f);
//////    if (ret == 1) {
//////	printf("success\n");
//////	printf("%g %g %g %g %g %g\n", a, b, c, d, e, f);
//////	x = p0[0]; y = p0[1]; w = p0[2];
//////	printf("%g ", a*x*x+b*x*y+c*y*y+d*x*w+e*y*w+f*w*w);
//////	x = p1[0]; y = p1[1]; w = p1[2];
//////	printf("%g ", a*x*x+b*x*y+c*y*y+d*x*w+e*y*w+f*w*w);
//////	x = p2[0]; y = p2[1]; w = p2[2];
//////	printf("%g ", a*x*x+b*x*y+c*y*y+d*x*w+e*y*w+f*w*w);
//////	x = p3[0]; y = p3[1]; w = p3[2];
//////	printf("%g ", a*x*x+b*x*y+c*y*y+d*x*w+e*y*w+f*w*w);
//////	x = p4[0]; y = p4[1]; w = p4[2];
//////	printf("%g\n", a*x*x+b*x*y+c*y*y+d*x*w+e*y*w+f*w*w);
//////    } else {
//////	printf("toconic failed\n");
//////    }
//////*/
//////
//////    for (i = 0; i < 100000; i++) {
//////	p0[0] = (FLOAT) (rand()%30);
//////	p0[1] = (FLOAT) (rand()%30);
//////	p0[2] = (FLOAT) (rand()%30);
//////	p1[0] = (FLOAT) (rand()%30);
//////	p1[1] = (FLOAT) (rand()%30);
//////	p1[2] = (FLOAT) (rand()%30);
//////	p2[0] = (FLOAT) (rand()%30);
//////	p2[1] = (FLOAT) (rand()%30);
//////	p2[2] = (FLOAT) (rand()%30);
//////	p3[0] = (FLOAT) (rand()%30);
//////	p3[1] = (FLOAT) (rand()%30);
//////	p3[2] = (FLOAT) (rand()%30);
//////	p4[0] = (FLOAT) (rand()%30);
//////	p4[1] = (FLOAT) (rand()%30);
//////	p4[2] = (FLOAT) (rand()%30);
//////	if (toconic(p0, p1, p2, p3, p4, &a, &b, &c, &d, &e, &f)) {
//////	    x = p0[0]; y = p0[1]; w = p0[2];
//////	    s0=a*x*x+b*x*y+c*y*y+d*x*w+e*y*w+f*w*w;
//////	    x = p1[0]; y = p1[1]; w = p1[2];
//////	    s1=a*x*x+b*x*y+c*y*y+d*x*w+e*y*w+f*w*w;
//////	    x = p2[0]; y = p2[1]; w = p2[2];
//////	    s2=a*x*x+b*x*y+c*y*y+d*x*w+e*y*w+f*w*w;
//////	    x = p3[0]; y = p3[1]; w = p3[2];
//////	    s3=a*x*x+b*x*y+c*y*y+d*x*w+e*y*w+f*w*w;
//////	    x = p4[0]; y = p4[1]; w = p4[2];
//////	    s4=a*x*x+b*x*y+c*y*y+d*x*w+e*y*w+f*w*w;
//////	    if (std::fabs(s0) > .00001 || std::fabs(s1) > .00001 || std::fabs(s2) > .00001
//////		|| std::fabs(s3) > .00001 || std::fabs(s4) > .00001) {
//////		    printf("%g %g %g %g %g\n", s0, s1, s2, s3, s4);
//////		    printf("\t%g %g %g\n", p0[0], p0[1], p0[2]);
//////		    printf("\t%g %g %g\n", p1[0], p1[1], p1[2]);
//////		    printf("\t%g %g %g\n", p2[0], p2[1], p2[2]);
//////		    printf("\t%g %g %g\n", p3[0], p3[1], p3[2]);
//////		    printf("\t%g %g %g\n", p4[0], p4[1], p4[2]);
//////		    printf("%g %g %g %g %g %g\n", a, b, c, d, e, f);
//////		}
//////	}
//////    }
//////}



CIRCLE2D::CIRCLE2D()
:centerX (0),
 centerY (0),
 radius (0)
{
}


CIRCLE2D::CIRCLE2D(const CIRCLE2D& CR)
:centerX (CR.centerX),
 centerY (CR.centerY),
 radius (CR.radius)
{
}

CIRCLE2D::CIRCLE2D(POINT2D C, double R)
:centerX (C.x),
 centerY (C.y),
 radius (R)
{
}


CIRCLE2D::CIRCLE2D(double Cx, double Cy, double R)
:centerX (Cx),
 centerY (Cy),
 radius (R)
{
}


CIRCLE2D::~CIRCLE2D()
{
}


double CIRCLE2D::MeritCirclesIntersect(CIRCLE2D C1, CIRCLE2D C2)
//Returns a merit in the range [0,1] if circles intersect
//Merit is 1 of bot circles are concentric
//Merit is 0 if both circles are tangent or exterior to each other
{

	if(C2.centerX == C1.centerX && C2.centerY == C1.centerY){
		//Circles trivially intersect, as they are concentric
		return 1;
	}

	if(C2.radius == 0. && C2.radius == 0.){
		//Circles cannot interect, as their radius are null
		//(while they are not concentric)
		return 0;
	}

	double distance= MODULE2D(C1.centerX, C1.centerY, C2.centerX, C2.centerY);
	if(distance <= C1.radius+C2.radius)
		return (1 - (distance/(C1.radius+C2.radius)));
	else
		return 0;

}


bool CIRCLE2D::CirclesIntersect(CIRCLE2D C1, CIRCLE2D C2)
//Returns true if circles intersect or are tangent,
//false otherwise

//C1 is the first circle
//C2 is the second circle

//Based on:
//	Algorithms for Competitive Programming (2022)
//	Circle-Circle Intersection
//	https://cp-algorithms.com/geometry/circle-circle-intersection.html
{

	if(C2.centerX == C1.centerX && C2.centerY == C1.centerY){
		//Circles trivially intersect, as they are concentric
		return true;
	}

	if(C2.radius == 0. && C2.radius == 0.){
		//Circles cannot interect, as their radius are null
		//(while they are not concentric)
		return false;
	}

	double distance= MODULE2D(C1.centerX, C1.centerY, C2.centerX, C2.centerY);
	if(distance <= C1.radius+C2.radius)
		return true;
	else
		return false;


	//---------------Calculation of intersect points follows-------------------

	//Translate origins to make C1 centered at origin
	C2.centerX-= C1.centerX;
	C2.centerY-= C1.centerY;
	//C1.centerX= 0;
	//C1.centerY= 0;

	//Reduce the original problem to the problem of 
	//finding intersections of the first circle and a line:
	double a= -2.*C2.centerX;
	double b= -2.*C2.centerY;
	double c= C2.centerX*C2.centerX + C2.centerY*C2.centerY + C1.radius*C1.radius - C2.radius*C2.radius;

	double r= C1.radius;

	//Calculate intersection between line a, b, c and circle R (centered at origin)
	double x0 = -a*c/(a*a+b*b), y0 = -b*c/(a*a+b*b);

	//if (c*c > r*r*(a*a+b*b)+EPS){
	if (c*c > r*r*(a*a+b*b)+Accuracy){
		//No points in common
		return false;
	}

	//else if (abs (c*c - r*r*(a*a+b*b)) < EPS) {
	else if (abs (c*c - r*r*(a*a+b*b)) < Accuracy) {
		//Circles are tangent at (x0, y0)
		return true;
	}
	else {
		//Circles intersect at points (ax, ay) and (bx, by)

		//double d = r*r - c*c/(a*a+b*b);
		//double mult = sqrt (d / (a*a+b*b));
		//double ax, ay, bx, by;
		//ax = x0 + b * mult;
		//ay = y0 - a * mult;

		//bx = x0 - b * mult;
		//by = y0 + a * mult;

		return true;
	}
}


double GetAngle2D_PI(POINT2D P0, POINT2D P1)
//Gets the angle (in radians) between
//	the non-directed segment defined by P0 and P1
//	and the X axis

//Since the edge is not oriented,
//returns a value in the range [0, PI]
{
	//assert(MODULE_2D(P0, P1) > 0);

	double angle= atan2(P1.y-P0.y, P1.x-P0.x);  //range [-PI, PI]
	if(angle<0) angle+= NUMBER_PI; //range [0, PI]
	//if (std::fabs(angle-NUMBER_PI)<Accuracy) angle= 0.0;  //range [0, PI)

	return angle;
}


double GetAngle2D_PI(POINT2D P1, POINT2D Pivot, POINT2D P2)
//Gets the angle (in radians) between the two segments
//defined by three 2D points 

//Segments are P1-Pivot and Pivot-P2, as Pivot is the vertex, of the angle
//This means that the function returns the minimum angle between two consecutive segments

//The order of the points P1 and P2 is irrelevant,
//as the function returns a value in the range [0, PI]
{
	P1-= Pivot;
	P2-= Pivot;
	double norm_cross= P1.x*P2.y-P1.y*P2.x;
	double dot= P1.x*P2.x+P1.y*P2.y;

	//Return the absolute value of the minor
	//of the two angles between segments P1-Pivot and Pivot-P2
	return std::fabs(atan2(norm_cross, dot));
}


double GetAngle2D_PI(POINT2D T1, POINT2D H1,   //Segment 1
					 POINT2D T2, POINT2D H2)   //Segment 2
//Gets the angle (in radians) between the two segments defined by:
//       segment 1 endpoints are T1 and H1
//       segment 2 endpoints are T2 and H2

//Segments orientation (from Tail to Head) is irrelevant,
//function returns a value in the range [0, PI],
//which is the smallest angle between both segments
{
	POINT2D HH2((H2.x+T1.x-T2.x), (H2.y+T1.y-T2.y));  //Translate H2-T2 to superimpose T2 on T1
	return (GetAngle2D_PI(H1, T1, HH2));
}


//double GetAngle2D_PI_half(POINT2D P0, POINT2D P1)
////Gets the angle (in radians) between
////	the non-directed segment defined by P0 and P1
////	and the X axis
//
////Since the edge is not oriented,
////returns a value in the range [0, PI/2)
//{
//	//assert(MODULE_2D(P0, P1) > 0);
//	double angle= GetAngle2D_PI(P0, P1);  //range [0, PI)
//	if (angle>(NUMBER_PI/2)) angle = NUMBER_PI - angle;  //Angle in the range [0, PI/2]
//
//	return angle;
//}


double GetDistancePointToLine(POINT2D V, POINT2D p0, POINT2D p1, 
							  POINT2D &Intersection)
//Gets the distance from an external point "V"
//to a line that contains the segment defined by p0 and p1
//Gets also the intersection of line defined by segment p0-p1
//with the minimum distance segment

//Returns the distance and the intersection point

//Note that the distance and the intersection points 
//are returned regardless of whether
//the perpendicular projection of V onto the line 
//belongs to the segment or not
{
	if(MODULE_2D(p0, p1) == 0)
		return MODULE_2D(p0, V);

	//Get the intersection point
	double u= (V.x - p0.x)*(p1.x - p0.x) + (V.y - p0.y)*(p1.y - p0.y);
	u/= ((p1.x - p0.x)*(p1.x - p0.x) + (p1.y - p0.y)*(p1.y - p0.y));
	Intersection.x = p0.x + ((p1.x - p0.x)*u);
	Intersection.y = p0.y + ((p1.y - p0.y)*u);

	//Get the distance from the external point to the intersection point
	return MODULE_2D(Intersection, V);
}


double GetDistancePointToLine(POINT2D V, POINT2D p0, POINT2D p1)
//Gets the distance from an external point "V" 
//to the line that contains the segment defined by p0 and p1

//Note that the distance is returned regardless of whether
//the perpendicular projection of V onto the line belongs to the segment or not

//The distance between point V and line segment P0-P1 can be calculated as
//the height of parallelogram defined by vectors P0-P1 and P0-V 
//(i.e. the area of the parallelogram divided by the length of P0-P1).
//So it can be written as simple as:
//	distance = |P0P1 X P0V| / sqrt(P0P1 * P0P1)
{
	double mod= MODULE_2D(p0, p1);
	if(mod == 0)
		return MODULE_2D(p0, V);

	return std::fabs(((V.x - p0.x)*(p1.y - p0.y) - (V.y - p0.y)*(p1.x - p0.x)) / mod);
}


double GetDistancePointIntoSegment(double pointX, double pointY,
								   double x1, double y1,
								   double x2, double y2,
								   double &CloseX, double &CloseY)
//Returns shortest distance between point (pointX, pointY) 
//and segment (x1,y1)-(x2,y2)

//Also returns the coordinates of the closest point (CloseX, CloseY)

//WARNING!!
//The closest point MUST be in the segment
//If the shortest distance occurs with an external point
//The distance to the closer tip is used instead
{
	double dX = x2 - x1;
	double dY = y2 - y1;
	if (dX==0 && dY==0){
		//The line is a point
		CloseX= x1;
		CloseY= y1;
		dX = pointX - x1;
		dY = pointY - y1;
		return sqrt(dX*dX + dY*dY);
	}

	double t = ((pointX - x1) * dX + (pointY - y1) * dY) / (dX*dX + dY*dY);

	if (t < 0){
		//Closer point is outside the segment:
		//Select closer point IN the segment
		CloseX= x1;
		CloseY= y1;
	}
	else if (t > 1){
		//Closer point is outside the segment:
		//Select closer point IN the segment
		CloseX= x2;
		CloseY= y2;
	}
	else{
		//Perpendicular line intersects the line segment
		CloseX= (x1 + t * dX);
		CloseY= (y1 + t * dY);
	}

	dX = pointX - CloseX;
	dY = pointY - CloseY;

	return sqrt(dX*dX + dY*dY);
}


RECTANGLE2D GetBounds(std::vector <POINT2D> Points)
// Finds the bounding rectangle of the set of Points
{
	RECTANGLE2D bound;
	bound.left  = Points[0].x;
	bound.right = Points[0].x;
	bound.bottom= Points[0].y;
	bound.top   = Points[0].y;

	for (long ipoint=1; ipoint<(long)Points.size(); ipoint++){
		bound.left  = std::min(bound.left, Points[ipoint].x);
		bound.right = std::max(bound.right, Points[ipoint].x);
		bound.bottom=  std::min(bound.bottom, Points[ipoint].y);
		bound.top   =  std::max(bound.top, Points[ipoint].y);
	}

	return bound;
}


//bool NearlyEquals(double A,double B,double percent)
////Returns true if A and B differ less than "percent" (in %),
////returns false otherwise
//{
//	if (A==B)
//		return true;
//	else{
//		if (std::fabs(A-B) <= std::fabs(std::min(A,B)*(1.-percent/100.)))
//			return true;
//		else
//			return false;
//	}
//}


double NearlyParallel(POINT2D A1, POINT2D B1,  //Segment 1
					  POINT2D A2, POINT2D B2,  //Segment 2
					  double Tolerance)
//Returns true if segments 1 and 2 are parallel or nearly parallel (whithin a tolerance)
//False otherwise

//Input: segment 1 endpoints are A1 and B1
//       segment 2 endpoints are A2 and B2
{
	POINT2D BB2((B2.x+A1.x-A2.x), (B2.y+A1.y-A2.y));  //Translate A2-B2 to superimpose A2 on A1
	double angle= GetAngle2D_PI(B1, A1, BB2);

	//Determine whether the angle is below an angular threshold
	double meritParallel= std::max(std::max((1. - angle/Tolerance), 0.),
								   std::max((1. - std::fabs(angle-NUMBER_PI)/Tolerance), 0.));
	return meritParallel;
}


double NearlyParallel(double a0x, double a0y,
					  double a1x, double a1y,
					  double b0x, double b0y,
					  double b1x, double b1y,
					  double ThresholdAngle)
//Returns true if two segments are parallel, within a "treshold"
//(a0x,a0y) and (a1x,a1y) are the coordinates of the endpoints of the first segment
//(b0x,b0y) and (b1x,b1y) are the coordinates of the endpoints of the second segment
{
	POINT2D A0(a0x, a0y);
	POINT2D A1(a1x, a1y);

	POINT2D B0(b0x, b0y);
	POINT2D B1(b1x, b1y);

	return NearlyParallel(A0, A1, B0, B1, ThresholdAngle);
}


double NearlyCollinear(double a0x, double a0y,
					   double a1x, double a1y,
					   double b0x, double b0y,
					   double b1x, double b1y,
					   double ThresholdAngle, double ThresholdOffset)
//Returns true if two segments are collinear, within both angular and offset thresholds

//(a0x,a0y) and (a1x,a1y) are the coordinates of the endpoints of the first segment
//(b0x,b0y) and (b1x,b1y) are the coordinates of the endpoints of the second segment
{
	POINT2D A0(a0x, a0y);
	POINT2D A1(a1x, a1y);

	POINT2D B0(b0x, b0y);
	POINT2D B1(b1x, b1y);

	//Check orientations
	//------------------
	//(Return false if differences between both orientations are over the threshold)
	double difAngle= GetAngle2D_PI(A0, A1, B0, B1);
	difAngle= std::min(difAngle, std::fabs(difAngle-NUMBER_PI)); //angle in the range [0, PI/2]
	double meritAngle= std::max((1. - difAngle/ThresholdAngle), 0.);

	//Check offset
	//------------
	double OffsetMax= std::min(MODULE_2D(A0,A1), MODULE_2D(B0,B1)) * ThresholdOffset; //% of the legth of the shortest segment
	if(OffsetMax <= 0.){  //Ignore check if threshold is non-positive
		return meritAngle;
	}

	//Check if segments are trivially nearly coincident
	//if((MODULE_2D(A0,B0)<OffsetMax && MODULE_2D(A1,B1)<OffsetMax) ||
	//   (MODULE_2D(A0,B1)<OffsetMax && MODULE_2D(A1,B0)<OffsetMax))
	//	return true; //Lines nearly overlap (are trivially collinear)

	double meritOffset;
	double offsetVer= std::min(std::max(MODULE_2D(A0,B0), MODULE_2D(A1,B1)), 
							   std::max(MODULE_2D(A0,B1), MODULE_2D(A1,B0)));
	if(offsetVer < OffsetMax){
		//Lines nearly overlap (are trivially collinear)
		meritOffset= std::max((1. - offsetVer/OffsetMax), 0.);
	}

	else{
		//Check if closest point of each segment is not too distant 
		//from the line defined by the other segment
		//Measure the minimum offset between lines that contain segments
		double minDist= std::min(std::min(GetDistancePointToLine(B0, A0, A1),
										  GetDistancePointToLine(B1, A0, A1)),
								 std::min(GetDistancePointToLine(A0, B0, B1),
										  GetDistancePointToLine(A1, B0, B1)));

		meritOffset= std::max((1. - minDist/OffsetMax), 0.);
	}

	return (std::min(meritAngle, meritOffset));
}


bool SegmentsIntersect(POINT2D a1, POINT2D b1,
					   POINT2D a2, POINT2D b2,
					   double &x, double &y)
//Return true if segments intersect, false otherwise

//Input: edge 1 endpoints (a1.x,a1.y) (b1.x,b1.y)
//       edge 2 endpoints (a2.x,a2.y) (b2.x,b2.y)

//Also returns the intersection point (x, y)

//Source: Paul Bourke, April 1989 (http://paulbourke.net/geometry/lineline2d/)
{
	x = 0;
	y = 0;

	double denom  = (b2.y-a2.y) * (b1.x-a1.x) - (b2.x-a2.x) * (b1.y-a1.y);
	double numera = (b2.x-a2.x) * (a1.y-a2.y) - (b2.y-a2.y) * (a1.x-a2.x);
	double numerb = (b1.x-a1.x) * (a1.y-a2.y) - (b1.y-a1.y) * (a1.x-a2.x);

	//Are the lines coincident?
	if (std::fabs(numera) < Accuracy && std::fabs(numerb) < Accuracy && std::fabs(denom) < Accuracy) {
		//x = (a1.x + b1.x) / 2;
		//y = (a1.y + b1.y) / 2;
		//return true;
		return false;
	}

	//Are the lines parallel?
	if (std::fabs(denom) < Accuracy) {
		return false;
	}

	//Find the intersection
	double mua = numera / denom;
	double mub = numerb / denom;
	if (mua < 0 || mua > 1 || mub < 0 || mub > 1) {
		//The intersection is outside the segments
		return false;
	}

	x = a1.x + mua * (b1.x - a1.x);
	y = a1.y + mua * (b1.y - a1.y);
	return true;
}


POINT2D GetCentroid(std::vector<POINT2D> Polygon)
//Returns the centroid of a polygon
//defined by the sequence of its vertices

//The signed area is calculated through the Shoelace algorithm
{
	POINT2D Centroid;

	long N = (long)Polygon.size();
	double SignedArea= 0;

	for (long i=0; i<N; i++){
		long ii= (i+1) % N;
		Centroid.x += ((Polygon[i].x + Polygon[ii].x)*
					   (Polygon[i].x * Polygon[ii].y - Polygon[ii].x * Polygon[i].y));
		Centroid.y += ((Polygon[i].y + Polygon[ii].y)*
					   (Polygon[i].x * Polygon[ii].y - Polygon[ii].x * Polygon[i].y));

		SignedArea += (Polygon[i].x * Polygon[ii].y);
		SignedArea -= (Polygon[i].y * Polygon[ii].x);
	}

	SignedArea/= 2;

	Centroid.x /= (6*SignedArea);
	Centroid.y /= (6*SignedArea);

	return Centroid;
}


double crossProduct(const POINT2D p1, const POINT2D p2, const POINT2D p3)
// Function to compute the cross product of vectors (p1p2) and (p1p3)
{
	return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}


bool isPointOnSegment(const POINT2D p, const POINT2D s1, const POINT2D s2)
// Function to check if point p lies on segment s1s2

// Checks if point p lies on the line segment s1s2 and
// within the bounding box of s1s2
{
	return crossProduct(s1, s2, p) == 0
						&& p.x >= std::min(s1.x, s2.x)
						&& p.x <= std::max(s1.x, s2.x)
						&& p.y >= std::min(s1.y, s2.y)
						&& p.y <= std::max(s1.y, s2.y);
}


bool isPointInPolygon(const std::vector<POINT2D> polygon, const POINT2D point)
// Function to check if a point is inside a polygon using
// the winding number algorithm.

// Compute the winding number for the point with respect to the polygon

//The Winding Number algorithm counts the number of times 
//the polygon rotates around the point. 
//If the number of turns is not zero, the point is inside; otherwise it is out.

//Based on:
//	Hormann K., Agathas A. (2001)
//	The point in polygon problem for arbitrary polygons
//	Computational Geometry. 20(3), pp 131-144.
//	DOI: 10.1016/S0925-7721(01)00012-8.
{
	long n = (long)polygon.size();
	long windingNumber = 0;

	// Iterate through each edge of the polygon
	for (long i= 0; i<n; i++){
		POINT2D p1 = polygon[i];
		POINT2D p2 = polygon[(i + 1) % n]; // Next vertex in the polygon

		// Check if the point lies on the current edge
		if (isPointOnSegment(point, p1, p2)){
			// Point is on the polygon boundary
			return 0;
		}

		// Calculate the cross product to determine winding direction
		if (p1.y <= point.y) {
			if (p2.y > point.y && crossProduct(p1, p2, point) > 0) {
				windingNumber++;
			}
		}
		else {
			if (p2.y <= point.y && crossProduct(p1, p2, point) < 0) {
				windingNumber--;
			}
		}
	}

	return (windingNumber != 0);
}


void equation_3_points_plane(double x1, double y1, double z1, //Point1
							 double x2, double y2, double z2, //Point2
							 double x3, double y3, double z3, //Point3
							 double &a, double &b, double &c, double &d) //Plane's parameters
//Returns the parameters (a, b, c, d) of the equation
//of the plane passing through three points
{
	//V21= P2 - P1
	double a1 = x2 - x1;
	double b1 = y2 - y1;
	double c1 = z2 - z1;

	//V31= P3 - P1
	double a2 = x3 - x1;
	double b2 = y3 - y1;
	double c2 = z3 - z1;

	//Normal= V21 x V31
	a = b1 * c2 - b2 * c1;
	b = a2 * c1 - a1 * c2;
	c = a1 * b2 - b1 * a2;

	//Distance= normal . Point1
	d = (- a * x1 - b * y1 - c * z1);
}
