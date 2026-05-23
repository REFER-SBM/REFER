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

#include <cmath>
#include <vector>
#include <algorithm>  //min, max

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "ReferDoc.h"

#include "WinRf_TrackBall.h"
#include "WinRf_OpenGLViewInflation.h"

// Easier way to specify color for Windows bigots
#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)


CWinRf_OpenGLViewInflation::CWinRf_OpenGLViewInflation()
{
}


void CWinRf_OpenGLViewInflation::GLSetupRC_Inflation(void *pData)
// Do any initialization of the rendering context here, such as
// setting background colors, setting up lighting, or performing
// preliminary calculations.
{
	glShadeModel(GL_FLAT);    // Modelo de sombreado plano
//	glShadeModel(GL_SMOOTH);  // Modelo de sombreado suave
	glEnable(GL_DEPTH_TEST);  // Test de profundidad (Z-buffer)
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL); // Pintar los poligonos por delante y por detras

	// ACTIVACION DE LA ILUMINACION
	// Coordenadas y valores de las luces
	GLfloat LuzAmbiente[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat LuzDifusa[] =   { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat PosicionLuz[] = { 1.0f, 1.0f, 1.0f, 0.0f };   // La posicion esta en el infinito

//	glEnable(GL_LIGHTING);
	// Define e inicializa la luz 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, LuzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LuzDifusa);
	glLightfv(GL_LIGHT0, GL_POSITION, PosicionLuz);
	glEnable(GL_LIGHT0);

	// Activa el seguimiento de color
	glEnable(GL_COLOR_MATERIAL);

	// Selecciona las propiedades de material para el seguimiento de los valores glColor
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	//Ajusta la calidad de la representacion
	//Activar el anti-aliasing (anti-escalonado)
	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	glEnable(GL_LINE_SMOOTH);                // Suavizado de lineas
	//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Maxima calidad en el suavizado de lineas

	//Desactivar el anti-aliasing (anti-escalonado)
		glDisable(GL_BLEND);
		glDisable(GL_LINE_SMOOTH);     // Desactiva el suavizado de lineas
}


void CWinRf_OpenGLViewInflation::GLSetupBackground(COLORREF m_BackgroundColor)
// Adjust the clear color
{
	GLclampf Rojo, Verde, Azul;
	Rojo  = ((GLclampf) GetRValue(m_BackgroundColor)) / (GLclampf)255.0;
	Verde = ((GLclampf) GetGValue(m_BackgroundColor)) / (GLclampf)255.0;
	Azul  = ((GLclampf) GetBValue(m_BackgroundColor)) / (GLclampf)255.0;
	glClearColor( Rojo, Verde, Azul, 1.0f );
}


void CWinRf_OpenGLViewInflation::GLResize_ViewInflation(long width, long heigh)
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
											VolumeDDBB.dRangeZ)) / 2.;

	double dRangeDeep = 1.1 * dRangeFrame;  //10% margin
	dRangeFrame *= (1.1* VolumeDDBB.dZoom);

	if (w <= h) 
		glOrtho(-dRangeFrame, dRangeFrame, -dRangeFrame*h/w, dRangeFrame*h/w, -dRangeDeep, dRangeDeep);
	else
		glOrtho(-dRangeFrame*w/h, dRangeFrame*w/h, -dRangeFrame, dRangeFrame, -dRangeDeep, dRangeDeep);
}


void CWinRf_OpenGLViewInflation::DrawMainAxes_Inflation(double Size)
// Draws the coordinate axes
{
	GLdouble dTam= Size;
	GLdouble dRadius = dTam / 20.0;
	GLdouble dLengthTip = dTam * 0.5;

	glBegin(GL_QUADS);
		// Plane XY; Z=0
		//glRGB(180, 180, 180);   //Gray
		//glVertex3d( 0.0, 0.0, 0.0 );
		//glVertex3d( dTam, 0.0, 0.0 );
		//glVertex3d( dTam, dTam, 0.0 );
		//glVertex3d( 0.0, dTam, 0.0 );    //Already drawn in big size
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
		gluCylinder( Axis, dRadius, dRadius, dTam*0.65, 10, 2 );  // Axis X
		glTranslated( 0, 0, dTam-dLengthTip-dRadius*2 );
		gluCylinder( Axis, dRadius*2, 0, dLengthTip, 10, 2 );  // Tip axis X
		glPopMatrix();

		glPushMatrix();
		glRotated( -90, 1.0f, 0.0f, 0.0f );
		gluCylinder( Axis, dRadius, dRadius, dTam*0.65, 10, 2 );  // Axis Y
		glTranslated( 0, 0, dTam-dLengthTip-dRadius*2 );
		gluCylinder( Axis, dRadius*2, 0, dLengthTip, 10, 2 );  // Tip axis Y
		glPopMatrix();

		glPopMatrix();
	gluDeleteQuadric( Axis );
}


void CWinRf_OpenGLViewInflation::GLRenderSceneInflation(CReferDoc* pDoc)
// Renders the 3D inflation process
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Update trackball and camera
	float matRot[4][4];
	Ctrackball T;
	T.CalcRotMatrix( Camera.PosMousePrev, Camera.PosMouseCurrent,
					 Camera.SizeWindow, Camera.curquat, matRot );
	glMultMatrixf( &(matRot[0][0]) );
	Camera.PosMousePrev = Camera.PosMouseCurrent;

	//Draw the XY plane
	glRGB(180, 180, 180);
	glBegin(GL_QUADS);
		glVertex3d( VolumeDDBB.dPlaneXmax, VolumeDDBB.dPlaneYmax, VolumeDDBB.dPlaneZ );
		glVertex3d( VolumeDDBB.dPlaneXmin, VolumeDDBB.dPlaneYmax, VolumeDDBB.dPlaneZ );
		glVertex3d( VolumeDDBB.dPlaneXmin, VolumeDDBB.dPlaneYmin, VolumeDDBB.dPlaneZ );
		glVertex3d( VolumeDDBB.dPlaneXmax, VolumeDDBB.dPlaneYmin, VolumeDDBB.dPlaneZ );
	glEnd();

	//Draw the axes
	glPushMatrix();
		glTranslated( VolumeDDBB.dPlaneXmin, VolumeDDBB.dPlaneYmin, VolumeDDBB.dPlaneZ );
		DrawMainAxes_Inflation( VolumeDDBB.dSizeCoordAxes );
	glPopMatrix();

	// Draws edges
	for (long i=0; i < pDoc->DB_Brep_Inflation->GetSizeEdges(); i++ ) {
		CEdge Arista= pDoc->DB_Brep_Inflation->GetEdge(i);

		CVertex Vertice1= pDoc->DB_Brep_Inflation->GetVertex(Arista.Head);
		CVertex Vertice2= pDoc->DB_Brep_Inflation->GetVertex(Arista.Tail);

		glRGB(Arista.ColorR, Arista.ColorG, Arista.ColorB);

		if (ReferApp.Cu.Hi.TypesOfEdges.size()!= 0 &&     // Hidden edges calculated
			ReferApp.Cu.Hi.TypesOfEdges[i] == UNDEFINED) {
			glEnable(GL_LINE_STIPPLE);
			//glLineStipple(2, 43690);
			glLineStipple(2, 34952);
		}
		else
			glDisable(GL_LINE_STIPPLE);

		glBegin(GL_LINES);
			glVertex3d(Vertice1.x - VolumeDDBB.dMidPointX,
					   Vertice1.y - VolumeDDBB.dMidPointY,
					   Vertice1.z - VolumeDDBB.dMidPointZ );
			glVertex3d(Vertice2.x - VolumeDDBB.dMidPointX,
					   Vertice2.y - VolumeDDBB.dMidPointY,
					   Vertice2.z - VolumeDDBB.dMidPointZ );
		glEnd();
	};

	// Flush drawing commands
	glFlush();
}

