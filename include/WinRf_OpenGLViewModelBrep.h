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

#ifndef OPENGLVIEWMODELBREP_VISITED
#define OPENGLVIEWMODELBREP_VISITED

class CWinRf_OpenGLViewModelBrep:public CObject
{
	protected:
		// CAMERA
		struct TCamera_Brep {
			int Radius, Accuracy;
			double dIncrementAngleX, dIncrementAngleY;
			POINT PosMousePrev, PosMouseCurrent;
			SIZE IncrementPosMouse;
			SIZE SizeWindow;
			float curquat[4];
		};

		// VOLUME
		struct TVolume_Brep {
			double dXmin, dYmin, dZmin,
				   dXmax, dYmax, dZmax,
				   dRangeX, dRangeY, dRangeZ,
				   dMidPointX, dMidPointY, dMidPointZ,
				   dZoom,
				   dPlaneXmin, dPlaneYmin, dPlaneXmax, dPlaneYmax, dPlaneZ,
				   dSizeCoordAxes;
		};

	private:
		void DrawMainAxes_ModelBrep(double Size, bool bMainAxes);

	public:
		TCamera_Brep Camera;
		TVolume_Brep VolumeDDBB;

		CWinRf_OpenGLViewModelBrep();
		void GLSetupRC_ModelBrep(void *pData);
		void GLSetupBackground(COLORREF m_BackgroundColor);
		void GLResize_ModelBrep(long width, long heigh);
		void GLRenderScene(CDB_Model *DB_Aux,
						   bool bShowAxesModelBrep,
						   bool bMainAxes,
						   bool bShowEdges3D,
						   bool bThickEdges,
						   bool bShowFaces3D,
						   bool bShowTriangulatedFaces3D,
						   bool bShowFaceNormals3D);
};

#endif // OPENGLVIEWMODELBREP_VISITED