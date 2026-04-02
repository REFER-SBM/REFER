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

#if !defined(DIALOGDETAILSVANISHINGPOINTS_INCLUDED)
	#define DIALOGDETAILSVANISHINGPOINTS_INCLUDED

	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
	// DialogDetailsVanishingPoints.h : header file
	//

	/////////////////////////////////////////////////////////////////////////////
	// CDialogDetailsVanishingPoints dialog

	class CDialogDetailsVanishingPoints : public CDialog
	{
		// Construction
		public:
			void GetParams(CDataVanishingPoints &VP);
			void SetParams(CDataVanishingPoints VP);
			CDialogDetailsVanishingPoints(CWnd* pParent = NULL);   // standard constructor

		// Dialog Data
			//{{AFX_DATA(CDialogDetailsVanishingPoints)
			enum { IDD = IDD_DIALOG_Details_VanishingPoints };
			double m_ClusteringAngle;
			double m_InRing;
			double m_OutRing;
			double m_ToleranceCoincident;
			double m_ToleranceParallel;
			double m_ToleranceDispersion;
			//}}AFX_DATA


		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CDialogDetailsVanishingPoints)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(CDialogDetailsVanishingPoints)
				// NOTE: the ClassWizard will add member functions here
			afx_msg void OnRestoreDefault();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(DIALOGDETAILSVANISHINGPOINTS_INCLUDED)
