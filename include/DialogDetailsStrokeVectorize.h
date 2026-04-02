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

#if !defined(DIALOGDETAILSSTROKEVECTORIZE_INCLUDED)
	#define DIALOGDETAILSSTROKEVECTORIZE_INCLUDED

	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
	// DialogDetailsStrokeVectorize.h : header file
	//

	/////////////////////////////////////////////////////////////////////////////
	// CDialogDetailsStrokeVectorize dialog

	class CDialogDetailsStrokeVectorize : public CDialog
	{
		// Construction
		public:
			void SetParams(CDataVectorize Ve);
			void GetParams(CDataVectorize &Ve);
			CDialogDetailsStrokeVectorize(CWnd* pParent = NULL);   // standard constructor

		// Dialog Data
			//{{AFX_DATA(CDialogDetailsStrokeVectorize)
			enum { IDD = IDD_DIALOG_Details_StrokeVectorize };
			BOOL m_VectorizeLines;
			double m_LineTolMin;
			double m_LineTolMax;
			double m_LineSmoothingPenalty;
			double m_MinMeritForLine;
			int m_FitLineApproach;

			BOOL m_RemoveTails;
			int m_MaxTrimPointsInTail;
			double m_MinAngleInTail;

			BOOL m_VectorizeArcs;
			double m_ArcTolMin;
			double m_ArcTolMax;
			double m_ArcSmoothingPenalty;
			double m_MinMeritForArc;
			int m_FitEllipseApproach;
			BOOL m_CheckQuality5P;
			BOOL m_CheckQualityDIR;
			BOOL m_ResampleStrokesDIR;
			int m_NumResampledPoints;
			BOOL m_ResampleStrokesGEF;
			int m_NumResampledPointsGEF;
			BOOL m_DisplayColors;
			//}}AFX_DATA


		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CDialogDetailsStrokeVectorize)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(CDialogDetailsStrokeVectorize)
				// NOTE: the ClassWizard will add member functions here
			afx_msg void OnBUTTONRestoreDefault();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(DIALOGDETAILSSTROKEVECTORIZE_INCLUDED)
