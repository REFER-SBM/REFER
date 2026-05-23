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

#ifndef OPENGLVIEWINFLATION_VISITED
#define OPENGLVIEWINFLATION_VISITED

class CWinRf_OpenGLViewInflation:public CObject
{
	protected:
		// CAMERA
		struct TCamera_Inflation {
			int Radius, Accuracy;
			double dIncrementAngleX, dIncrementAngleY;
			POINT PosMousePrev, PosMouseCurrent;
			SIZE IncrementPosMouse;
			SIZE SizeWindow;
			float curquat[4];
		};

		// VOLUME
		struct TVolume_Inflation {
			double dXmin, dYmin, dZmin,
				   dXmax, dYmax, dZmax,
				   dRangeX, dRangeY, dRangeZ,
				   dMidPointX, dMidPointY, dMidPointZ,
				   dZoom,
				   dPlaneXmin, dPlaneYmin, dPlaneXmax, dPlaneYmax, dPlaneZ,
				   dSizeCoordAxes;
		};


	private:
		void DrawMainAxes_Inflation(double Size);

	public:
		TCamera_Inflation Camera;
		TVolume_Inflation VolumeDDBB;

		CWinRf_OpenGLViewInflation();
		void GLSetupRC_Inflation(void *pData);
		void GLSetupBackground(COLORREF m_BackgroundColor);
		void GLResize_ViewInflation(long width, long heigh);
		void GLRenderSceneInflation(CReferDoc* pDoc);
};

#endif // OPENGLVIEWINFLATION_VISITED