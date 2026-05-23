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

#if !defined(DIALOGDETAILSSGROUPNONSOLIDSTROKES_INCLUDED)
	#define DIALOGDETAILSSGROUPNONSOLIDSTROKES_INCLUDED

	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
	// DialogDetailsGroupNonSolidStrokes.h : header file
	//

	/////////////////////////////////////////////////////////////////////////////
	// CDialogDetailsGroupNonSolidStrokes dialog

	class CDialogDetailsGroupNonSolidStrokes : public CDialog
	{
		// Construction
		public:
			int m_iSliderDashBalanceStepAngleP;

			void SetParams(CDataVectorize Ve);
			void GetParams(CDataVectorize &Ve);
			CDialogDetailsGroupNonSolidStrokes(CWnd* pParent = NULL);   // standard constructor

		// Dialog Data
			//{{AFX_DATA(CDialogDetailsGroupNonSolidStrokes)
			enum { IDD = IDD_DIALOG_Details_GroupNonSolidStrokes };
			double m_IsolatedTipThreshold;
			double m_DashSizeMax;
			double m_DashSizeShortRange;
			double m_DashSizeLongRange;
			double m_DashGapShortRange;
			double m_DashGapLongRange;
			double m_MaxDashAngle;
			double m_MaxDashOffset;
			double m_IncreaseDashTolerances;
			double m_DashBalanceStepAngle;
			CSliderCtrl m_sliderDashBalanceStepAngle;

			double m_MaxNonStraightDashes;
			double m_DashLineTolMin;
			double m_DashLineTolMax;
			double m_DashLineSmoothPenalty;
			double m_DashMinMeritLine;

			//}}AFX_DATA

		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CDialogDetailsGroupNonSolidStrokes)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(CDialogDetailsGroupNonSolidStrokes)
				// NOTE: the ClassWizard will add member functions here
			virtual BOOL OnInitDialog();

			afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
			afx_msg void OnEnChangeEdit();

			afx_msg void OnBUTTONRestoreDefault();
			virtual void OnOK();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(DIALOGDETAILSSGROUPNONSOLIDSTROKES_INCLUDED)
