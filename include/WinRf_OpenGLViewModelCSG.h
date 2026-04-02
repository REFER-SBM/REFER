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

#ifndef OPENGLVIEWCSG_VISITED
#define OPENGLVIEWCSG_VISITED

class CWinRf_OpenGLViewModelCSG:public CObject
{
	protected:
		// CAMERA
		struct TCamera_CSG {
			int Radius, Accuracy;
			double dIncrementAngleX, dIncrementAngleY;
			POINT PosMousePrev, PosMouseCurrent;
			SIZE IncrementPosMouse;
			SIZE SizeWindow;
			float curquat[4];
		};

		// VOLUME
		struct TVolume_CSG {
			double dXmin, dYmin, dZmin,
				   dXmax, dYmax, dZmax,
				   dRangeX, dRangeY, dRangeZ,
				   dMidPointX, dMidPointY, dMidPointZ,
				   dZoom,
				   dPlaneXmin, dPlaneYmin, dPlaneXmax, dPlaneYmax, dPlaneZ,
				   dSizeCoordAxes;
		};


	private:
		void DrawMainAxes_ModelCSG(double Size);

	public:
		TCamera_CSG Camera_CSG;
		TVolume_CSG VolumeDDBB_CSG;

		CWinRf_OpenGLViewModelCSG();
		void GLSetupRC_ModelCSG(void *pData);
		void GLSetupBackground(COLORREF m_BackgroundColor);
		void GLResize_ModelCSG(long width, long heigh);
		void GLRenderSceneCSG(CDB_Model *DB_Aux);
};

#endif // OPENGLVIEWCSG_VISITED