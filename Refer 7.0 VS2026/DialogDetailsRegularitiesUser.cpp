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

// DialogDetailsRegularitiesUser.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDetailsRegularitiesUser.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRegularitiesUser dialog


CDialogDetailsRegularitiesUser::CDialogDetailsRegularitiesUser(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsRegularitiesUser::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsRegularitiesUser)
	//}}AFX_DATA_INIT
}


void CDialogDetailsRegularitiesUser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsRegularitiesUser)
	DDX_Check(pDX, IDC_CHECK_DA, m_DA);
	DDX_Check(pDX, IDC_CHECK_FP, m_FP);
	DDX_Check(pDX, IDC_CHECK_PL, m_PL);
	DDX_Check(pDX, IDC_CHECK_CL, m_CL);
	DDX_Check(pDX, IDC_CHECK_VL, m_VL);
	DDX_Check(pDX, IDC_CHECK_OL, m_OL);
	DDX_Check(pDX, IDC_CHECK_I, m_I);
	DDX_Check(pDX, IDC_CHECK_CC, m_CC);
	DDX_Check(pDX, IDC_CHECK_PEC, m_PEC);
	DDX_Check(pDX, IDC_CHECK_OFO, m_OFO);
	DDX_Check(pDX, IDC_CHECK_SFO, m_SFO);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsRegularitiesUser, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsRegularitiesUser)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRegularitiesUser message handlers

void CDialogDetailsRegularitiesUser::SetParams(CDataInflation In)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_DA =  In.RegularitiesUser.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive;
	m_FP =  In.RegularitiesUser.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive;

	m_PL =  In.RegularitiesUser.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive;
	m_CL =  In.RegularitiesUser.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive;
	m_VL =  In.RegularitiesUser.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive;

	m_OL =  In.RegularitiesUser.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive;

	m_I =   In.RegularitiesUser.ListOfRegularities[REGULARITY_ISOMETRY].bActive;

	m_CC =  In.RegularitiesUser.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive;
	m_PEC = In.RegularitiesUser.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive;

	m_OFO = In.RegularitiesUser.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive;
	m_SFO = In.RegularitiesUser.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive;
}

void CDialogDetailsRegularitiesUser::GetParams(CDataInflation &In)
// Updates the general parameters that have been modified in the dialog
{
	In.RegularitiesUser.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive = m_DA !=0;
	In.RegularitiesUser.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive = m_FP !=0;

	In.RegularitiesUser.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive = m_PL !=0;
	In.RegularitiesUser.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive = m_CL !=0;
	In.RegularitiesUser.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive = m_VL !=0;

	In.RegularitiesUser.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive = m_OL !=0;

	In.RegularitiesUser.ListOfRegularities[REGULARITY_ISOMETRY].bActive = m_I !=0;

	In.RegularitiesUser.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive = m_CC !=0;
	In.RegularitiesUser.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive = m_PEC !=0;

	In.RegularitiesUser.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive = m_OFO !=0;
	In.RegularitiesUser.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive = m_SFO !=0;
	long iActivadas=0; 
	for(long i=0; i<REGULARITY_LAST+1 ; i++ ){
		if ( In.RegularitiesUser.ListOfRegularities[i].bActive)
			iActivadas++;
	}
	In.RegularitiesUser.NumActiveRegularities = iActivadas;

}

void CDialogDetailsRegularitiesUser::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataInflation initial;
	SetParams(initial);
	UpdateData( false );
}
