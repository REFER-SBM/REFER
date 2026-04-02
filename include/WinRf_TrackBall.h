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

// TRACKBALL.H */
// IMPLEMENTATION OF A VIRTUAL TRACK-BALL, for Visual C++*/
// Author: Juan Vicente Andreu Hernandez   - Castellon'98 -

// THIS CODE IS AN ADAPTED VERSION OF:
// "A virtual trackball implementation"
// Written by Gavin Bell for Silicon Graphics, November 1988.

// Quaternions always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
// If they don't add up to 1.0, dividing by their magnitued will
// renormalize them.

// Note: See the following for more information on quaternions:

// - Shoemake, K., Animating rotation with quaternion curves,
//   Computer Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
// - Pletinckx, D., Quaternion calculus as a basic tool
//   in computer graphics, The Visual Computer 5, 2-13, 1989.


#ifndef TRACKBALL_VISITED
#define TRACKBALL_VISITED

	class Ctrackball
	{
		private:
			float tb_project_to_sphere(float r, float x, float y);
			void normalize_quat(float q[4]);
			void vzero(float *v);
			void vset(float *v, float x, float y, float z);
			void vsub(const float *src1, const float *src2, float *dst);
			void vcopy(const float *v1, float *v2);
			void vcross(const float *v1, const float *v2, float *cross);
			float vlength(const float *v);
			void vscale(float *v, float div);
			void vnormal(float *v);
			float vdot(const float *v1, const float *v2);
			void vadd(const float *src1, const float *src2, float *dst);


			void build_rotmatrix(float m[4][4], float q[4]);
			// A useful function, builds a rotation matrix in Matrix based on given quaternion.


			void axis_to_quat(float a[3], float phi, float q[4]);
			// This function computes a quaternion based on an axis (defined by
			// the given vector) and an angle about which to rotate.  The angle is
			// expressed in radians.  The result is put into the third argument.

		public:
			Ctrackball::Ctrackball();
			Ctrackball::~Ctrackball();

			void add_quats(float *q1, float *q2, float *dest);
				// Given two quaternions, add them together to get a third quaternion.
				// Adding quaternions to get a compound rotation is analagous to adding
				// translations to get a compound translation.  When incrementally
				// adding rotations, the first argument here should be the new
				// rotation, the second and third the total rotation (which will be
				// over-written with the resulting new total rotation).

			void CalcRotMatrix( POINT MousePrev , POINT MouseCurr, SIZE SizeWindow ,
								float curquat[4], float matRot[4][4] );
			// Calculate rotation matrix based on mouse movement


			void calc_quat(float q[4], float p1x, float p1y, float p2x, float p2y);
			// Pass the x and y coordinates of the last and current positions of
			// the mouse, scaled so they are from (-1.0 ... 1.0).
			//
			// if ox,oy is the window's center and sizex,sizey is its size, then
			// the proper transformation from screen coordinates (sc) to world
			// coordinates (wc) is:
			// wcx = (2.0 * (scx-ox)) / (float)sizex - 1.0
			// wcy = (2.0 * (scy-oy)) / (float)sizey - 1.0
			//
			// The resulting rotation is returned as a quaternion rotation in the
			// first paramater.

			void Euler_to_quat(float q[4], double heading, double attitude, double bank);

	};
#endif
