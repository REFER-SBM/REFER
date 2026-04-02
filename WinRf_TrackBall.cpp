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

// IMPLEMENTATION OF A VIRTUAL TRACK-BALL, for Visual C++ 5.0
// Author: Juan Vicente Andreu Hernandez   - Castellon'98 -

// This code is an adapted version of:
//  A virtual trackball implementation
//  Written by Gavin Bell for Silicon Graphics, November 1988

// A general explanation about trackballs can be found here:
// https://www.opengl.org/wiki/Object_Mouse_Trackball


#include <stdafx.h>

#include <cmath>
#include "WinRf_TrackBall.h"

// This size should really be based on the distance from the center of
// rotation to the point on the object underneath the mouse
// That point would then track the mouse as closely as possible
#define TRACKBALLSIZE  (0.8f)


Ctrackball::Ctrackball()
{
}


Ctrackball::~Ctrackball()
{
}


float Ctrackball::tb_project_to_sphere(float r, float x, float y)
// Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
// if we are away from the center of the sphere.
{
	float d, t, z;

	d = (float) sqrt(x*x + y*y);
	if (d < r * 0.70710678118654752440f)  // Inside sphere
		z = (float) sqrt(r*r - d*d);
	else {                                // On hyperbola
	  t = r / 1.41421356237309504880f;
	  z = t*t / d;
	}
	return z;
}


void Ctrackball::normalize_quat(float q[4])
// Quaternions always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
// If they don't add up to 1.0, dividing by their magnitued will
// renormalize them.
//
// Note: See the following for more information on quaternions:
//
// - Shoemake, K., Animating rotation with quaternion curves, Computer
//   Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
// - Pletinckx, D., Quaternion calculus as a basic tool in computer
//   graphics, The Visual Computer 5, 2-13, 1989.
{
	int i;
	float mag;

	mag = (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
	for (i = 0; i < 4; i++)
		q[i] /= mag;
}

//Some vector management

void Ctrackball::vzero(float *v)
{
	v[0] = 0.0f;
	v[1] = 0.0f;
	v[2] = 0.0f;
}

void Ctrackball::vset(float *v, float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

void Ctrackball::vsub(const float *src1, const float *src2, float *dst)
{
	dst[0] = src1[0] - src2[0];
	dst[1] = src1[1] - src2[1];
	dst[2] = src1[2] - src2[2];
}

void Ctrackball::vcopy(const float *v1, float *v2)
{
	for (int i = 0 ; i < 3 ; i++)
		v2[i] = v1[i];
}

void Ctrackball::vcross(const float *v1, const float *v2, float *cross)
{
	float temp[3];

	temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
	vcopy(temp, cross);
}

float Ctrackball::vlength(const float *v)
{
	return (float) sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void Ctrackball::vscale(float *v, float div)
{
	v[0] *= div;
	v[1] *= div;
	v[2] *= div;
}

void Ctrackball::vnormal(float *v)
{
	vscale(v,1.0f/vlength(v));
}

float Ctrackball::vdot(const float *v1, const float *v2)
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void Ctrackball::vadd(const float *src1, const float *src2, float *dst)
{
	dst[0] = src1[0] + src2[0];
	dst[1] = src1[1] + src2[1];
	dst[2] = src1[2] + src2[2];
}


void Ctrackball::add_quats(float q1[4], float q2[4], float dest[4])
// Given two rotations, e1 and e2, expressed as quaternion rotations,
// figure out the equivalent single rotation and stuff it into dest.

// This routine also normalizes the result every RENORMCOUNT times it is
// called, to keep error from creeping in.

// NOTE: This routine is written so that q1 or q2 may be the same
// as dest (or each other).
{
	long RENORMCOUNT= 97;

	static int count=0;
	float t1[4], t2[4], t3[4];
	float tf[4];

	vcopy(q1,t1); 
	vscale(t1,q2[3]);

	vcopy(q2,t2); 
	vscale(t2,q1[3]);

	vcross(q2,q1,t3);
	vadd(t1,t2,tf);
	vadd(t3,tf,tf);
	tf[3] = q1[3] * q2[3] - vdot(q1,q2);

	dest[0] = tf[0];
	dest[1] = tf[1];
	dest[2] = tf[2];
	dest[3] = tf[3];

	if (++count > RENORMCOUNT) {
		count = 0;
		normalize_quat(dest);
	}
}


void Ctrackball::build_rotmatrix(float m[4][4], float q[4])
// Build a rotation matrix, given a quaternion rotation
{
	m[0][0] = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
	m[0][1] = 2.0f * (q[0] * q[1] - q[2] * q[3]);
	m[0][2] = 2.0f * (q[2] * q[0] + q[1] * q[3]);
	m[0][3] = 0.0f;

	m[1][0] = 2.0f * (q[0] * q[1] + q[2] * q[3]);
	m[1][1]= 1.0f - 2.0f * (q[2] * q[2] + q[0] * q[0]);
	m[1][2] = 2.0f * (q[1] * q[2] - q[0] * q[3]);
	m[1][3] = 0.0f;

	m[2][0] = 2.0f * (q[2] * q[0] - q[1] * q[3]);
	m[2][1] = 2.0f * (q[1] * q[2] + q[0] * q[3]);
	m[2][2] = 1.0f - 2.0f * (q[1] * q[1] + q[0] * q[0]);
	m[2][3] = 0.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}


void Ctrackball::axis_to_quat(float a[3], float phi, float q[4])
// Given an axis a and angle phi, compute quaternion
{
	vnormal(a);
	vcopy(a,q);
	vscale(q,(float) sin(phi/2.0f));
	q[3] = (float) cos(phi/2.0f);
}


////////////////////////////////////////////////////////////////////////////


void Ctrackball::CalcRotMatrix(POINT MousePrev , POINT MouseCurr, SIZE SizeWindow ,
							   float curquat[4], float matRot[4][4])
// Calculate rotation matrix based on mouse movement
// If mouse has moved since button was pressed, change quaternion
{
	if (MousePrev.x != MouseCurr.x || MousePrev.y != MouseCurr.y) {
		float newquat[4];
		calc_quat(newquat,
				  -(2.0f * ( SizeWindow.cx - MousePrev.x ) / SizeWindow.cx - 1.0f),
				  -(2.0f * MousePrev.y / SizeWindow.cy - 1.0f),
				  -(2.0f * ( SizeWindow.cx - MouseCurr.x ) / SizeWindow.cx - 1.0f),
				  -(2.0f * MouseCurr.y / SizeWindow.cy - 1.0f) );

		add_quats(newquat, curquat, curquat);  //The quaternion changes incrementally
	};
	build_rotmatrix(matRot, curquat);
};


void Ctrackball::calc_quat(float q[4], float p1x, float p1y, float p2x, float p2y)
// Calculate the quaternion that represents the rotation
// from previous (p1) to current point (p2)

// Any rotation in three dimensions can be represented as a combination
// of a vector and an angle
// Quaternions encode this axis–angle representation in four numbers

// Project the points onto the virtual trackball,
// then figure out the axis of rotation "a", which is the cross
// product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
// Next, compute the angle phi encompassed by the arc segment from P1 to P2.
// Finally, compute the quaternion

// Note:  This is a deformed trackball-- is a trackball in the center,
// but is deformed into a hyperbolic sheet of rotation away from the
// center.

// It is assumed that the arguments are in the range (-1.0 ... 1.0)
{
	if (p1x == p2x && p1y == p2y) {
		// Zero rotation
		vzero(q);
		q[3] = 1.0f;
		return;
	}

	// First, figure out z-coordinates for projection of P1 and P2 to deformed sphere
	float p1[3], p2[3];
	vset(p1,p1x,p1y,tb_project_to_sphere(TRACKBALLSIZE,p1x,p1y));
	vset(p2,p2x,p2y,tb_project_to_sphere(TRACKBALLSIZE,p2x,p2y));

	// Now, we want the cross product of P1 and P2
	float a[3]; // Axis of rotation
	vcross(p2,p1,a);

	// Figure out how much to rotate around that axis
	float d[3];
	vsub(p1,p2,d);
	float t = vlength(d) / (2.0f*TRACKBALLSIZE);

	// Avoid problems with out-of-control values...
	if (t > 1.0f) t = 1.0f;
	if (t < -1.0f) t = -1.0f;

	// Calculate the rotation angle from P1 to P2
	float phi = 2.0f * (float) asin(t);

	//Compute the quaternion
	axis_to_quat(a,phi,q);
}


void Ctrackball::Euler_to_quat(float q[4], double heading, double attitude, double bank)
// Calculates the quaternion that corresponds to the Euler angles
// Assuming the angles are in radians

//The Euler angles are as follows:
//		Heading == Yaw: rotation about the Z-axis
//		Attitude== Pitch: rotation about the Y-axis
//		Bank    == Roll: rotation about the X-axis

{
	double c1 = cos(heading/2);
	double s1 = sin(heading/2);
	double c2 = cos(attitude/2);
	double s2 = sin(attitude/2);
	double c3 = cos(bank/2);
	double s3 = sin(bank/2);
	double c1c2 = c1*c2;
	double s1s2 = s1*s2;
	q[3] = (float) (c1c2*c3 - s1s2*s3);
	q[0] = (float) (c1c2*s3 + s1s2*c3);
	q[1] = (float) (s1*c2*c3 + c1*s2*s3);
	q[2] = (float) (c1*s2*c3 - s1*c2*s3);
}
