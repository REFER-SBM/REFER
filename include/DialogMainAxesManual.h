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

#if !defined DIALOGMAINAXESMANUAL_DEFINED
#define DIALOGMAINAXESMANUAL_DEFINED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

enum TMouseMode {InactiveMode, ZoomMode, PanMode};

// DialogMainAxesManual.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogMainAxesManual dialog

class CDialogMainAxesManual : public CDialog
{
	//Construction
	private:
		void GDI_DrawSketch2D();

		//Parameters and functions to navigate the virtual paper
		double CameraZoom;
		double CameraPanX, CameraPanY;
		double ViewportWide, ViewportHigh;
		double WindowLength, WindowHigh;

		long DeltaX, DeltaY;

		//CPoint position;
		void ResetViewport();
		void Get2DModelBoundingBox(double &wXmin, double &wYmin, double &wXmax, double &wYmax);
		void UpdateWindow();
		void ResetWindow();
		cGDI_Transform Transf;

		CPoint lastPoint;
		TMouseMode MouseMode;

		//Parameters and functions to select edges
		long Selection[3];
		long Selected_axis;
		long SelectEdge(CPoint point);
		bool LiangBarsky (double LimitLeft, double LimitRight, double LimitBottom, double LimitTop,
						  double x0src, double y0src, double x1src, double y1src);

public:
		CDialogMainAxesManual(CWnd* pParent = NULL);   // standard constructor
		~CDialogMainAxesManual();   // standard destructor

	// Dialog Data
		enum {IDD = IDD_DIALOG_MainAxesManual};

	// Overrides
		protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//Implementation
	private:
		// Attributes
		CDB_Model *DDBB;  //Local pointer to the working sketch

	//Operations
	public:
		void SetParams();
		void GetParams();

	protected:
		virtual BOOL OnInitDialog();
		//Window
		afx_msg void OnClearSelection();
		afx_msg void OnPaint();

		//Mouse
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);

		DECLARE_MESSAGE_MAP()
};

#endif // !defined DIALOGMAINAXESMANUAL_DEFINED
