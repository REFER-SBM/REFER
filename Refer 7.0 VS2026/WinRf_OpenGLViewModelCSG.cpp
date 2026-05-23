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
#include "WinRf_OpenGLViewModelCSG.h"

#define GL_MAX_NUMBER_OF_VERTICES_PER_FACE 1000

// Easier way to specify color for Windows bigots
#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)


CWinRf_OpenGLViewModelCSG::CWinRf_OpenGLViewModelCSG()
{
}


void CWinRf_OpenGLViewModelCSG::GLSetupRC_ModelCSG(void *pData)
// Do any initialization of the rendering context here, such as
// setting background colors, setting up lighting, or performing
// preliminary calculations.
{
	glShadeModel(GL_FLAT);    // Modelo de sombreado plano
//	glShadeModel(GL_SMOOTH);  // Modelo de sombreado suave
	glEnable(GL_DEPTH_TEST);  // Test de profundidad (Z-buffer)
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL); // Paint the polygons in front and behind

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


void CWinRf_OpenGLViewModelCSG::GLSetupBackground(COLORREF m_BackgroundColor)
// Adjust the clear color
{
	GLclampf Rojo, Verde, Azul;
	Rojo  = ((GLclampf) GetRValue(m_BackgroundColor)) / (GLclampf)255.0;
	Verde = ((GLclampf) GetGValue(m_BackgroundColor)) / (GLclampf)255.0;
	Azul  = ((GLclampf) GetBValue(m_BackgroundColor)) / (GLclampf)255.0;
	glClearColor( Rojo, Verde, Azul, 1.0f );
}


void CWinRf_OpenGLViewModelCSG::GLResize_ModelCSG(long width, long heigh)
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
	GLdouble dRangeFrame= std::max(VolumeDDBB_CSG.dRangeX,
							  std::max(VolumeDDBB_CSG.dRangeY, 
									   VolumeDDBB_CSG.dRangeZ));

	double dRangeDeep = 1.1 * dRangeFrame;   //10% margin

	dRangeFrame *= VolumeDDBB_CSG.dZoom;

	if (w <= h) 
		glOrtho(-dRangeFrame, dRangeFrame, -dRangeFrame*h/w, dRangeFrame*h/w, -dRangeDeep, dRangeDeep);
	else
		glOrtho(-dRangeFrame*w/h, dRangeFrame*w/h, -dRangeFrame, dRangeFrame, -dRangeDeep, dRangeDeep);
}


void CWinRf_OpenGLViewModelCSG::DrawMainAxes_ModelCSG(double Size)
// Draws the coordinate axes
{
	GLdouble dTam= Size;
	GLdouble dRadius = dTam / 20.0;
	GLdouble dLengthTip = dTam * 0.5;

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
};


void CWinRf_OpenGLViewModelCSG::GLRenderSceneCSG(CDB_Model *DB_Aux)
// Renders the 3D CSG
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Update trackball and camera
	float matRot[4][4];
	Ctrackball T;
	T.CalcRotMatrix( Camera_CSG.PosMousePrev, Camera_CSG.PosMouseCurrent,
					 Camera_CSG.SizeWindow, Camera_CSG.curquat, matRot );
	glMultMatrixf( &(matRot[0][0]) );
	Camera_CSG.PosMousePrev = Camera_CSG.PosMouseCurrent;

	// Draw coordinate axes, if required
	// ---------------------------------
	if (false)
		DrawMainAxes_ModelCSG( VolumeDDBB_CSG.dSizeCoordAxes);

	// Draws faces, if required
	//-------------------------
	if (true){
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
						glVertex3d( V1.x - VolumeDDBB_CSG.dMidPointX,  //Coordinates are translated, to center the drawing
									V1.y - VolumeDDBB_CSG.dMidPointY,
									V1.z - VolumeDDBB_CSG.dMidPointZ );
						glVertex3d( V2.x - VolumeDDBB_CSG.dMidPointX,
									V2.y - VolumeDDBB_CSG.dMidPointY,
									V2.z - VolumeDDBB_CSG.dMidPointZ );
						glVertex3d( V3.x - VolumeDDBB_CSG.dMidPointX,
									V3.y - VolumeDDBB_CSG.dMidPointY,
									V3.z - VolumeDDBB_CSG.dMidPointZ );
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

							Vertices[j][0] = Vj.x - VolumeDDBB_CSG.dMidPointX;  //Coordinates are translated to center the drawing
							Vertices[j][1] = Vj.y - VolumeDDBB_CSG.dMidPointY;
							Vertices[j][2] = Vj.z - VolumeDDBB_CSG.dMidPointZ;

							gluTessVertex( pTeselador, Vertices[j], Vertices[j] );
						};
						gluEndPolygon( pTeselador );

						gluDeleteTess( pTeselador );
					}
				};
			};
		};
	};

	// Draws edges, if required
	//-------------------------
	if (true){
		//Draws edges as thick cylinders
		GLdouble dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
		DB_Aux->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
		GLdouble dRango= std::max((dXmax-dXmin) , (dYmax-dYmin));
		GLdouble dRadio= dRango/1000*Camera_CSG.Radius;

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
				glTranslated(V1.x-VolumeDDBB_CSG.dMidPointX,  //Coordinates are translated to center the drawing
							 V1.y-VolumeDDBB_CSG.dMidPointY,
							 V1.z-VolumeDDBB_CSG.dMidPointZ);
				glRotated( angDeg, -V.y, V.x, 0.0);

				glRGB(0, 0, 0);  //Black color

				gluCylinder( Cilindro, dRadio, dRadio, L, 2*Camera_CSG.Accuracy, 2*Camera_CSG.Accuracy/5 );

				glPopMatrix();
			}
		gluDeleteQuadric( Cilindro );
	};

	// Flush drawing commands
	glFlush();
}

