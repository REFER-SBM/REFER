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

#include <stdafx.h>
#include <gl\freeglut.h>
#include <cmath>  // sin, cos, sqrt, atan2
#include <vector>
#include <algorithm>  //min, max

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "DB_Model_Maths.h"

#include "WinRf_TrackBall.h"
#include "WinRf_OpenGLViewModelBrep.h"

#define GL_MAX_NUMBER_OF_VERTICES_PER_FACE 1000

// Easier way to specify color for Windows bigots
#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)


CWinRf_OpenGLViewModelBrep::CWinRf_OpenGLViewModelBrep()
{
}


void CWinRf_OpenGLViewModelBrep::GLSetupRC_ModelBrep(void *pData)
// Do any initialization of the rendering context here,
// such as setting background colors, setting up lighting,
// or performing preliminary calculations.
{
	glShadeModel(GL_FLAT);    // Flat shading
//	glShadeModel(GL_SMOOTH);  // Smooth shading
	glEnable(GL_DEPTH_TEST);  // Z-buffer
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL); // Paint the polygons in front and behind


	// ACTIVACION DE LA ILUMINACION
	// Coordenadas y valores de las luces
	//---------------------- OJO AUMENTO LA LUZ AMBIENTE (SEGUNDA LINEA)--------------------
	GLfloat LuzAmbiente[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat LuzDifusa[] =   { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat PosicionLuz[] = { 1.0f, 1.0f, 1.0f, 0.0f };   // La posicion esta en el infinito
	//--------------------------------------------------------------------------------------

//	glEnable(GL_LIGHTING);
	// Defines light
	glLightfv(GL_LIGHT0, GL_AMBIENT, LuzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LuzDifusa);
	glLightfv(GL_LIGHT0, GL_POSITION, PosicionLuz);
	glEnable(GL_LIGHT0);

	// Activate color tracking
	glEnable(GL_COLOR_MATERIAL);

	// Select material properties to track values of glColor
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	//Adjust the quality of the representation
	//Activate the anti-aliasing
	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	glEnable(GL_LINE_SMOOTH);
	//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	//Deactivate the anti-aliasing
		glDisable(GL_BLEND);
		glDisable(GL_LINE_SMOOTH);
};


void CWinRf_OpenGLViewModelBrep::GLSetupBackground(COLORREF m_BackgroundColor)
// Adjust the clear color
{
	GLclampf Rojo, Verde, Azul;
	Rojo  = ((GLclampf) GetRValue(m_BackgroundColor)) / (GLclampf)255.0;
	Verde = ((GLclampf) GetGValue(m_BackgroundColor)) / (GLclampf)255.0;
	Azul  = ((GLclampf) GetBValue(m_BackgroundColor)) / (GLclampf)255.0;
	glClearColor( Rojo, Verde, Azul, 1.0f );
}


void CWinRf_OpenGLViewModelBrep::GLResize_ModelBrep(long width, long heigh)
// Updates the size of the window and fits the image to the viewport
{
	GLsizei w= width;
	GLsizei h= heigh;
	if(h == 0) h = 1; // Prevent a divide by zero

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Establish orthogonal projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	GLdouble dRangeFrame= std::max(VolumeDDBB.dRangeX,
								   std::max(VolumeDDBB.dRangeY,
											VolumeDDBB.dRangeZ));

	double dRangeDeep = 1.1 * dRangeFrame;   //10% margin

	dRangeFrame *= VolumeDDBB.dZoom;

	if (w <= h) 
		glOrtho(-dRangeFrame, dRangeFrame, -dRangeFrame*h/w, dRangeFrame*h/w, -dRangeDeep, dRangeDeep);
	else
		glOrtho(-dRangeFrame*w/h, dRangeFrame*w/h, -dRangeFrame, dRangeFrame, -dRangeDeep, dRangeDeep);
	
}


void CWinRf_OpenGLViewModelBrep::DrawMainAxes_ModelBrep(double Size, bool bMainAxes)
// Draws the coordinate axes

//Works in two modes:
//  - Inflation axes
//  - Main axes
{
	GLdouble dTam= Size;
	GLdouble dRadius = dTam / 20.0;
	GLdouble dLengthTip = dTam * 0.5;

	if(bMainAxes){
		GLUquadricObj *Axis = gluNewQuadric();
			glRGB(0, 0, 180);   //Blue
			glPushMatrix();
			gluCylinder( Axis, dRadius, dRadius, dTam-dLengthTip, 10, 2 );  // Axis Z
			glTranslated( 0, 0, dTam-dLengthTip-dRadius*2 );
			gluCylinder( Axis, dRadius*2, 0, dLengthTip, 10, 2 );  // Tip axis Z
			glPopMatrix();

			glRGB(180, 0, 0);   //Red
			glPushMatrix();
			glRotated( 90, 0.0f, 1.0f, 0.0f );
			gluCylinder( Axis, dRadius, dRadius, dTam-dLengthTip, 10, 2 );  // Axis X
			glTranslated( 0, 0, dTam-dLengthTip-dRadius*2 );
			gluCylinder( Axis, dRadius*2, 0, dLengthTip, 10, 2 );  // Tip axis X
			glPopMatrix();

			glRGB(0, 180, 0);   //Green
			glPushMatrix();
			glRotated( -90, 1.0f, 0.0f, 0.0f );
			gluCylinder( Axis, dRadius, dRadius, dTam-dLengthTip, 10, 2 );  // Axis Y
			glTranslated( 0, 0, dTam-dLengthTip-dRadius*2 );
			gluCylinder( Axis, dRadius*2, 0, dLengthTip, 10, 2 );  // Tip axis Y
			glPopMatrix();
		gluDeleteQuadric( Axis );
	}

	else{
		glBegin(GL_QUADS);
			// Plane XY; Z=0
			glRGB(180, 180, 180);   //Gray
			glVertex3d( 0.0, 0.0, 0.0 );
			glVertex3d( dTam, 0.0, 0.0 );
			glVertex3d( dTam, dTam, 0.0 );
			glVertex3d( 0.0, dTam, 0.0 );
			// Plane XZ; Y=0
			glRGB(90, 90, 90);   //Dark gray
			glVertex3d( 0.0, 0.0, 0.0 );
			glVertex3d( 0.0, 0.0, dTam );
			glVertex3d( dTam, 0.0, dTam );
			glVertex3d( dTam, 0.0, 0.0 );
			// Plane YZ; X=0
			glRGB(0, 0, 200);   //Blue
			glVertex3d( 0.0, 0.0, 0.0 );
			glVertex3d( 0.0, dTam, 0.0 );
			glVertex3d( 0.0, dTam, dTam );
			glVertex3d( 0.0, 0.0, dTam );
		glEnd();

		GLUquadricObj *Axis = gluNewQuadric();
			glPushMatrix();
			glTranslated( dRadius*2, dRadius*2, dRadius*2 );
			glRGB(255, 255, 255);   //White

			glPushMatrix();
			gluCylinder( Axis, dRadius, dRadius, dTam-dLengthTip, 10, 2 );  // Axis Z
			glTranslated( 0, 0, dTam-dLengthTip-dRadius*2 );
			gluCylinder( Axis, dRadius*2, 0, dLengthTip, 10, 2 );  // Tip axis Z
			glPopMatrix();

			glPushMatrix();
			glRotated( 90, 0.0f, 1.0f, 0.0f );
			gluCylinder( Axis, dRadius, dRadius, dTam-dLengthTip, 10, 2 );  // Axis X
			glTranslated( 0, 0, dTam-dLengthTip-dRadius*2 );
			gluCylinder( Axis, dRadius*2, 0, dLengthTip, 10, 2 );  // Tip axis X
			glPopMatrix();

			glPushMatrix();
			glRotated( -90, 1.0f, 0.0f, 0.0f );
			gluCylinder( Axis, dRadius, dRadius, dTam-dLengthTip, 10, 2 );  // Axis Y
			glTranslated( 0, 0, dTam-dLengthTip-dRadius*2 );
			gluCylinder( Axis, dRadius*2, 0, dLengthTip, 10, 2 );  // Tip axis Y
			glPopMatrix();

			glPopMatrix();
		gluDeleteQuadric( Axis );
	}
};


void CWinRf_OpenGLViewModelBrep::GLRenderScene(CDB_Model *DB_Aux,
											 bool bShowAxesModelBrep,
											 bool bMainAxes,
											 bool bShowEdges3D,
											 bool bThickEdges,
											 bool bShowFaces3D,
											 bool bShowTriangulatedFaces3D,
											 bool bShowFaceNormals3D)
// Renders the 3D model and its associated information

//Draws the scene in reverse order,
//drawing first what must be ocluded in case of overlapping
//
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Update trackball and camera
	float matRot[4][4];
	Ctrackball T;
	T.CalcRotMatrix( Camera.PosMousePrev, Camera.PosMouseCurrent,
					 Camera.SizeWindow, Camera.curquat, matRot );
	glMultMatrixf( &(matRot[0][0]) );
	Camera.PosMousePrev = Camera.PosMouseCurrent;

	// Draw coordinate axes, if required
	// ---------------------------------
	if (bShowAxesModelBrep){
		DrawMainAxes_ModelBrep( VolumeDDBB.dSizeCoordAxes, bMainAxes); // Switch to main coordinate axes, if required
	}

	// Draws faces, if required
	//-------------------------
	if (bShowFaces3D){
		glRGB(230, 230, 230);  //Color of the faces: Grey

		for (long i=0; i < DB_Aux->GetSizeFaces(); i++) {
			CFace currentFace= DB_Aux->GetFace(i);
			std::vector<long> Triangles= currentFace.GetFaceTriangulation();

			if((long)Triangles.size() > 0){
				//Display triangles of the face
				for(long t=0; t<(long)Triangles.size(); t+=3 ) {
					CVertex V1= DB_Aux->GetVertex(Triangles[t]);
					CVertex V2= DB_Aux->GetVertex(Triangles[t+1]);
					CVertex V3= DB_Aux->GetVertex(Triangles[t+2]);

					glBegin(GL_TRIANGLES);
						glVertex3d( V1.x - VolumeDDBB.dMidPointX,  //Coordinates are translated, to center the drawing
									V1.y - VolumeDDBB.dMidPointY,
									V1.z - VolumeDDBB.dMidPointZ );
						glVertex3d( V2.x - VolumeDDBB.dMidPointX,
									V2.y - VolumeDDBB.dMidPointY,
									V2.z - VolumeDDBB.dMidPointZ );
						glVertex3d( V3.x - VolumeDDBB.dMidPointX,
									V3.y - VolumeDDBB.dMidPointY,
									V3.z - VolumeDDBB.dMidPointZ );
					glEnd();
				}
			}

			else{
				//Display the whole polygonal face
					//------------------------------
					// GL_POLYGON by OpenGL may only draw convex, simple polygons,
					// while "faces" may be both concave or convex polygons,
					// Hence, polygons must be "tessellated" (divided into triangles)
					// to get them displayed by OpenGL
					//------------------------------

				//Gets the vertices of the face in sequential order
				std::vector<long> vertices= DB_Aux->GetAllVerticesInFace(currentFace);
				long NumVertices= (long)vertices.size();

				if (NumVertices < GL_MAX_NUMBER_OF_VERTICES_PER_FACE){
					// Get normal vector
					//CFace currentFace= DB_Aux->GetFace(i);
					CVector Normal= currentFace.GetNormal(false);

					static GLdouble Vertices[GL_MAX_NUMBER_OF_VERTICES_PER_FACE][3];
					GLUtriangulatorObj *pTeselador;

					//Tessellates the polygonal face
					pTeselador = gluNewTess();
					if (pTeselador != 0) {
						gluTessCallback( pTeselador, GLU_BEGIN, (void (CALLBACK *)()) glBegin );
						gluTessCallback( pTeselador, GLU_VERTEX, (void (CALLBACK *)()) glVertex3dv );
						gluTessCallback( pTeselador, GLU_END, glEnd );

						gluBeginPolygon( pTeselador );

						for( long j=0; j<NumVertices; j++ ) {
							CVertex Vj= DB_Aux->GetVertex(vertices[j]);

							double aNormal[3]; aNormal[0] = Normal.x; aNormal[1] = Normal.y; aNormal[2] = Normal.z;
							glNormal3dv(aNormal);  //Face normal applies to each vertex of the face

							Vertices[j][0] = Vj.x - VolumeDDBB.dMidPointX;  //Coordinates are translated to center the drawing
							Vertices[j][1] = Vj.y - VolumeDDBB.dMidPointY;
							Vertices[j][2] = Vj.z - VolumeDDBB.dMidPointZ;

							gluTessVertex( pTeselador, Vertices[j], Vertices[j] );
						};
						gluEndPolygon( pTeselador );

						gluDeleteTess( pTeselador );
					}
				};
			};
		};
	};

	// Draws perimeters of triangulated faces, if required
	//----------------------------------------------------
	if (bShowTriangulatedFaces3D) {
		GLdouble dRango, dRadio;
		//Draw thin or thick perimeter lines
		if (!bThickEdges){
			//Draws edges as thin lines if thick cylinders are not required
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else{
			GLdouble dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
			DB_Aux->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
			dRango= std::max((dXmax-dXmin) , (dYmax-dYmin));
			dRadio= dRango/1000*Camera.Radius;
		}

		glRGB(230, 0, 0); //Color of the perimeter of the triangles: Red

		for (long i=0; i < (long)DB_Aux->GetSizeFaces(); i++) {
			CFace currentFace= DB_Aux->GetFace(i);
			std::vector<long> Triangles= currentFace.GetFaceTriangulation();

			for(long t=0; t<(long)Triangles.size(); t+=3 ) {
				if (!bThickEdges){
					//Draw thin perimeter lines
					CVertex V1= DB_Aux->GetVertex(Triangles[t]);
					CVertex V2= DB_Aux->GetVertex(Triangles[t+1]);
					CVertex V3= DB_Aux->GetVertex(Triangles[t+2]);

					glBegin(GL_TRIANGLES);
						glVertex3d( V1.x - VolumeDDBB.dMidPointX,  //Coordinates are translated, to center the drawing
									V1.y - VolumeDDBB.dMidPointY,
									V1.z - VolumeDDBB.dMidPointZ );
						glVertex3d( V2.x - VolumeDDBB.dMidPointX,
									V2.y - VolumeDDBB.dMidPointY,
									V2.z - VolumeDDBB.dMidPointZ );
						glVertex3d( V3.x - VolumeDDBB.dMidPointX,
									V3.y - VolumeDDBB.dMidPointY,
									V3.z - VolumeDDBB.dMidPointZ );
					glEnd();
				}

				else{
					//Draw thick (cylindrical) perimeter lines
					GLUquadricObj *Cilindro= gluNewQuadric();
						std::vector<CEdge> edgesT;
						CEdge T1(Triangles[t+1], Triangles[t]);
						edgesT.push_back(T1);
						CEdge T2(Triangles[t+2], Triangles[t+1]);
						edgesT.push_back(T2);
						CEdge T3(Triangles[t], Triangles[t+2]);
						edgesT.push_back(T3);

						for(long it=0; it < 3; it++) {
							CVertex V1= DB_Aux->GetVertex(edgesT[it].Head);
							CVertex V2= DB_Aux->GetVertex(edgesT[it].Tail);
							CVertex V;
							V.x= V2.x - V1.x;
							V.y= V2.y - V1.y;
							V.z= V2.z - V1.z;
							GLdouble L=sqrt((V.x*V.x)+(V.y*V.y)+(V.z*V.z));
							if (V.x == 0.0 && V.y == 0.0)
								V.y = 1.0;

							double angDeg = RADIANS_TO_DEGREES(atan2(sqrt((V.x * V.x)+(V.y * V.y)), V.z));

							glPushMatrix();
							glTranslated(V1.x-VolumeDDBB.dMidPointX,  //Coordinates are translated to center the drawing
										 V1.y-VolumeDDBB.dMidPointY,
										 V1.z-VolumeDDBB.dMidPointZ);
							glRotated( angDeg, -V.y, V.x, 0.0);

							gluCylinder( Cilindro, dRadio/2., dRadio/2., L, 2*Camera.Accuracy, 2*Camera.Accuracy/5 );

							glPopMatrix();
						}
					gluDeleteQuadric( Cilindro );
				}
			}
		};

		if (!bThickEdges){
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  //Reset default value (Paint the polygons in front and behind)
		}
	};

	// Draws edges, if required
	//-------------------------
	if (bShowEdges3D){
		if (!bThickEdges){
			//Draws edges as thin lines
			for(long i=0; i < DB_Aux->GetSizeEdges(); i++) {
				CEdge Arista= DB_Aux->GetEdge(i);

				glRGB(Arista.ColorR, Arista.ColorG, Arista.ColorB);

				if (ReferApp.Cu.Hi.TypesOfEdges.size() != 0 &&        // Hidden edges calculated
					ReferApp.Cu.Hi.TypesOfEdges[i] == UNDEFINED) {
					glEnable(GL_LINE_STIPPLE);
					glLineStipple(2, 0xAAAA);  //Dotted line
				}
				else if (ReferApp.Cu.Hi.TypesOfEdges.size() != 0 &&   // Hidden edges calculated
					ReferApp.Cu.Hi.TypesOfEdges[i] == HIDDEN) {
					glEnable(GL_LINE_STIPPLE);
					glLineStipple(1, 0x00FF);  //Dashed line
				}
				else
					glDisable(GL_LINE_STIPPLE);

				CVertex V1= DB_Aux->GetVertex(Arista.Head);
				CVertex V2= DB_Aux->GetVertex(Arista.Tail);
				glBegin(GL_LINES);
					glVertex3d( V1.x - VolumeDDBB.dMidPointX,  //Coordinates are translated to center the drawing
								V1.y - VolumeDDBB.dMidPointY,
								V1.z - VolumeDDBB.dMidPointZ );
					glVertex3d( V2.x - VolumeDDBB.dMidPointX,
								V2.y - VolumeDDBB.dMidPointY,
								V2.z - VolumeDDBB.dMidPointZ );

				glEnd();
			}
		}

		else{
			//Draws edges as thick cylinders
			GLdouble dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
			DB_Aux->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
			GLdouble dRango= std::max((dXmax-dXmin) , (dYmax-dYmin));
			GLdouble dRadio= dRango/1000*Camera.Radius;

			GLUquadricObj *Cilindro= gluNewQuadric();
				for(long i=0; i < DB_Aux->GetSizeEdges(); i++) {
					CEdge Arista= DB_Aux->GetEdge(i);
					CVertex V1= DB_Aux->GetVertex(Arista.Head);
					CVertex V2= DB_Aux->GetVertex(Arista.Tail);
					CVertex V;
					V.x= V2.x - V1.x;
					V.y= V2.y - V1.y;
					V.z= V2.z - V1.z;
					GLdouble L=sqrt((V.x*V.x)+(V.y*V.y)+(V.z*V.z));
					if (V.x == 0.0 && V.y == 0.0)
						V.y = 1.0;

					double angDeg = RADIANS_TO_DEGREES(atan2(sqrt((V.x * V.x)+(V.y * V.y)), V.z));

					glPushMatrix();
					glTranslated(V1.x-VolumeDDBB.dMidPointX,  //Coordinates are translated to center the drawing
								 V1.y-VolumeDDBB.dMidPointY,
								 V1.z-VolumeDDBB.dMidPointZ);
					glRotated( angDeg, -V.y, V.x, 0.0);

					glRGB(0, 0, 0);  //Black color

					gluCylinder( Cilindro, dRadio, dRadio, L, 2*Camera.Accuracy, 2*Camera.Accuracy/5 );

					glPopMatrix();
				}
			gluDeleteQuadric( Cilindro );
		};
	};

	// Draws face normals, if required
	//--------------------------------
	if (bShowFaceNormals3D) {
		//Determine the scale for the length of the normal vectors
		GLdouble dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
		DB_Aux->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
		double scaleNormal= std::max((dXmax-dXmin) , (dYmax-dYmin)) * 0.10; //10% of the size of the model

		glRGB(0, 0, 180);   //Blue;

		for (long i=0; i < DB_Aux->GetSizeFaces(); i++) {
			CFace currentFace= DB_Aux->GetFace(i);
			std::vector<CVector> Normals;
			std::vector<CVertex> Centroids;

			//Select normals to display
			std::vector<long> Triangles= currentFace.GetFaceTriangulation();

			if(bShowTriangulatedFaces3D && Triangles.size() > 0){
				//Select the normal of each triangle
				Normals= currentFace.GetFaceTrianglesNormals();

				for(long t=0; t<(long)Triangles.size(); t+=3 ) {
					CVertex Centroid (0., 0., 0.);
					for(long j=0; j<3; j++) {
						CVertex Vj= DB_Aux->GetVertex(Triangles[t+j]);
						Centroid.x += Vj.x;
						Centroid.y += Vj.y;
						Centroid.z += Vj.z;
					}

					Centroid.x /= 3.;
					Centroid.y /= 3.;
					Centroid.z /= 3.;
					Centroids.push_back(Centroid);
				}
			}

			else{
				//Select the normal of the whole face, if triangles are not calculated and displayed
				Normals.push_back(currentFace.GetNormal(false));
				Centroids.push_back(GetFaceCentroid(DB_Aux, i));
			}

			//Display selected normals
			for(long n=0; n<(long)Normals.size(); n+=1) {
				CVector Normal= Normals[n];

				CVertex Centroid;
				Centroid.x = Centroids[n].x - VolumeDDBB.dMidPointX;  //Coordinates are translated to center the drawing
				Centroid.y = Centroids[n].y - VolumeDDBB.dMidPointY;
				Centroid.z = Centroids[n].z - VolumeDDBB.dMidPointZ;

				//if (!bThickEdges){
				//	glDisable(GL_LINE_STIPPLE);
				//	glBegin(GL_LINES);
				//		glVertex3d( Centroid.x,
				//					Centroid.y,
				//					Centroid.z);
				//		glVertex3d( Centroid.x + scaleNormal*Normal.x,
				//					Centroid.y + scaleNormal*Normal.y,
				//					Centroid.z + scaleNormal*Normal.z);
				//	glEnd();
				//}

				//else{
					CVertex N;
					N.x= scaleNormal*Normal.x;
					N.y= scaleNormal*Normal.y;
					N.z= scaleNormal*Normal.z;
					GLdouble dRadius = scaleNormal / 20.0;
					GLdouble dLengthTip = scaleNormal * 0.5;

					double angDeg = RADIANS_TO_DEGREES(atan2(sqrt((N.x * N.x)+(N.y * N.y)), N.z));

					GLUquadricObj *Axis = gluNewQuadric();
						glPushMatrix();
						glTranslated( Centroid.x, Centroid.y, Centroid.z );
						glRotated( angDeg, -N.y, N.x, 0.0);
						gluCylinder( Axis, dRadius, dRadius, scaleNormal, 2*Camera.Accuracy, 2*Camera.Accuracy/5);  // Axis
						glTranslated( 0, 0, 2*dLengthTip-2*dRadius );
						gluCylinder( Axis, dRadius*2, 0, dLengthTip, 2*Camera.Accuracy, 2*Camera.Accuracy/5);  // Tip
						glPopMatrix();
					gluDeleteQuadric( Axis );
				//}
			};
		};
	};


	// Flush drawing commands
	glFlush();
}
