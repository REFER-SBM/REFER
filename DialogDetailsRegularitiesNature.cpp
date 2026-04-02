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

// DialogDetailsRegularitiesNature.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDetailsRegularitiesNature.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRegularitiesNature dialog


CDialogDetailsRegularitiesNature::CDialogDetailsRegularitiesNature(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsRegularitiesNature::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsRegularitiesNature)
	//}}AFX_DATA_INIT
}


void CDialogDetailsRegularitiesNature::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsRegularitiesNature)
	DDX_Check(pDX, IDC_CHECK_NDA, m_NDA);
	DDX_Check(pDX, IDC_CHECK_NFP, m_NFP);
	DDX_Check(pDX, IDC_CHECK_NPL, m_NPL);
	DDX_Check(pDX, IDC_CHECK_NVL, m_NVL);
	DDX_Check(pDX, IDC_CHECK_NI, m_NI);
	DDX_Check(pDX, IDC_CHECK_NCC, m_NCC);
	DDX_Check(pDX, IDC_CHECK_NOFO, m_NOFO);
	DDX_Check(pDX, IDC_CHECK_NSFO, m_NSFO);
	DDX_Check(pDX, IDC_CHECK_NOL, m_NOL);
	DDX_Check(pDX, IDC_CHECK_NPEC, m_NPEC);
	DDX_Check(pDX, IDC_CHECK_NCL, m_NCL);

	DDX_Check(pDX, IDC_CHECK_CDA, m_CDA);
	DDX_Check(pDX, IDC_CHECK_CFP, m_CFP);
	DDX_Check(pDX, IDC_CHECK_CPL, m_CPL);
	DDX_Check(pDX, IDC_CHECK_CVL, m_CVL);
	DDX_Check(pDX, IDC_CHECK_CI, m_CI);
	DDX_Check(pDX, IDC_CHECK_CCC, m_CCC);
	DDX_Check(pDX, IDC_CHECK_COFO, m_COFO);
	DDX_Check(pDX, IDC_CHECK_CSFO, m_CSFO);
	DDX_Check(pDX, IDC_CHECK_COL, m_COL);
	DDX_Check(pDX, IDC_CHECK_CPEC, m_CPEC);
	DDX_Check(pDX, IDC_CHECK_CCL, m_CCL);

	DDX_Check(pDX, IDC_CHECK_PRDA, m_PRDA);
	DDX_Check(pDX, IDC_CHECK_PRFP, m_PRFP);
	DDX_Check(pDX, IDC_CHECK_PRVL, m_PRVL);
	DDX_Check(pDX, IDC_CHECK_PRPL, m_PRPL);
	DDX_Check(pDX, IDC_CHECK_PRI, m_PRI);
	DDX_Check(pDX, IDC_CHECK_PRCC, m_PRCC);
	DDX_Check(pDX, IDC_CHECK_PROFO, m_PROFO);
	DDX_Check(pDX, IDC_CHECK_PRSFO, m_PRSFO);
	DDX_Check(pDX, IDC_CHECK_PROL, m_PROL);
	DDX_Check(pDX, IDC_CHECK_PRPEC, m_PRPEC);
	DDX_Check(pDX, IDC_CHECK_PRCL, m_PRCL);

	DDX_Check(pDX, IDC_CHECK_PDA, m_PDA);
	DDX_Check(pDX, IDC_CHECK_PFP, m_PFP);
	DDX_Check(pDX, IDC_CHECK_PPL, m_PPL);
	DDX_Check(pDX, IDC_CHECK_PVL, m_PVL);
	DDX_Check(pDX, IDC_CHECK_PI, m_PI);
	DDX_Check(pDX, IDC_CHECK_PCC, m_PCC);
	DDX_Check(pDX, IDC_CHECK_POFO, m_POFO);
	DDX_Check(pDX, IDC_CHECK_PSFO, m_PSFO);
	DDX_Check(pDX, IDC_CHECK_POL, m_POL);
	DDX_Check(pDX, IDC_CHECK_PPEC, m_PPEC);
	DDX_Check(pDX, IDC_CHECK_PCL, m_PCL);

	DDX_Check(pDX, IDC_CHECK_IDDA, m_IDDA);
	DDX_Check(pDX, IDC_CHECK_IDFP, m_IDFP);
	DDX_Check(pDX, IDC_CHECK_IDPL, m_IDPL);
	DDX_Check(pDX, IDC_CHECK_IDVL, m_IDVL);
	DDX_Check(pDX, IDC_CHECK_IDI, m_IDI);
	DDX_Check(pDX, IDC_CHECK_IDCC, m_IDCC);
	DDX_Check(pDX, IDC_CHECK_IDOFO, m_IDOFO);
	DDX_Check(pDX, IDC_CHECK_IDSFO, m_IDSFO);
	DDX_Check(pDX, IDC_CHECK_IDOL, m_IDOL);
	DDX_Check(pDX, IDC_CHECK_IDPEC, m_IDPEC);
	DDX_Check(pDX, IDC_CHECK_IDCL, m_IDCL);

	DDX_Check(pDX, IDC_CHECK_INDDA, m_INDDA);
	DDX_Check(pDX, IDC_CHECK_INDFP, m_INDFP);
	DDX_Check(pDX, IDC_CHECK_INDPL, m_INDPL);
	DDX_Check(pDX, IDC_CHECK_INDVL, m_INDVL);
	DDX_Check(pDX, IDC_CHECK_INDI, m_INDI);
	DDX_Check(pDX, IDC_CHECK_INDCC, m_INDCC);
	DDX_Check(pDX, IDC_CHECK_INDOFO, m_INDOFO);
	DDX_Check(pDX, IDC_CHECK_INDSFO, m_INDSFO);
	DDX_Check(pDX, IDC_CHECK_INDOL, m_INDOL);
	DDX_Check(pDX, IDC_CHECK_INDPEC, m_INDPEC);
	DDX_Check(pDX, IDC_CHECK_INDCL, m_INDCL);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsRegularitiesNature, CDialog)
	//{{AFX_MSG_MAP(DialogDetailsRegularitiesNature)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRegularitiesNature message handlers


void CDialogDetailsRegularitiesNature::SetParams(CDataInflation In)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_NDA =  In.RegularitiesNormalon.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive;
	m_NFP =  In.RegularitiesNormalon.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive;

	m_NPL =  In.RegularitiesNormalon.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive;
	m_NCL =  In.RegularitiesNormalon.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive;
	m_NVL =  In.RegularitiesNormalon.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive;

	m_NOL =  In.RegularitiesNormalon.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive;

	m_NI =   In.RegularitiesNormalon.ListOfRegularities[REGULARITY_ISOMETRY].bActive;

	m_NCC =  In.RegularitiesNormalon.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive;
	m_NPEC = In.RegularitiesNormalon.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive;

	m_NOFO = In.RegularitiesNormalon.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive;
	m_NSFO = In.RegularitiesNormalon.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive;


	m_CDA =  In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive;
	m_CFP =  In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive;

	m_CPL =  In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive;
	m_CCL =  In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive;
	m_CVL =  In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive;

	m_COL =  In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive;

	m_CI =   In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_ISOMETRY].bActive;

	m_CCC =  In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive;
	m_CPEC = In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive;

	m_COFO = In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive;
	m_CSFO = In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive;


	m_PRDA =  In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive;
	m_PRFP =  In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive;

	m_PRPL =  In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive;
	m_PRCL =  In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive;
	m_PRVL =  In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive;

	m_PROL =  In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive;

	m_PRI =   In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_ISOMETRY].bActive;

	m_PRCC =  In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive;
	m_PRPEC = In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive;

	m_PROFO = In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive;
	m_PRSFO = In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive;


	m_PDA = In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive;
	m_PFP = In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive;

	m_PPL = In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive;
	m_PCL = In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive;
	m_PVL = In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive;

	m_POL = In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive;

	m_PI =  In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_ISOMETRY].bActive;

	m_PCC = In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive;
	m_PPEC =In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive;

	m_POFO =In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive;
	m_PSFO =In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive;


	m_IDDA = In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive;
	m_IDFP = In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive;

	m_IDPL = In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive;
	m_IDCL = In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive;
	m_IDVL = In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive;

	m_IDOL = In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive;

	m_IDI =  In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_ISOMETRY].bActive;

	m_IDCC = In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive;
	m_IDPEC =In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive;

	m_IDOFO =In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive;
	m_IDSFO =In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive;


	m_INDDA = In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive;
	m_INDFP = In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive;

	m_INDPL = In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive;
	m_INDCL = In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive;
	m_INDVL = In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive;

	m_INDOL = In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive;

	m_INDI =  In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_ISOMETRY].bActive;

	m_INDCC = In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive;
	m_INDPEC =In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive;

	m_INDOFO =In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive;
	m_INDSFO =In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive;

}

void CDialogDetailsRegularitiesNature::GetParams(CDataInflation &In)
// Updates the general parameters that have been modified in the dialog
{
	int iActivadas;

	In.RegularitiesNormalon.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive = m_NDA !=0;
	In.RegularitiesNormalon.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive = m_NFP !=0; 

	In.RegularitiesNormalon.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive = m_NPL !=0; 
	In.RegularitiesNormalon.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive = m_NCL !=0;
	In.RegularitiesNormalon.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive = m_NVL !=0;

	In.RegularitiesNormalon.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive = m_NOL !=0; 

	In.RegularitiesNormalon.ListOfRegularities[REGULARITY_ISOMETRY].bActive = m_NI !=0;

	In.RegularitiesNormalon.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive = m_NCC !=0; 
	In.RegularitiesNormalon.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive = m_NPEC !=0; 

	In.RegularitiesNormalon.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive = m_NOFO !=0; 
	In.RegularitiesNormalon.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive = m_NSFO !=0; 
	iActivadas= 0;
	for(long i=0 ; i<REGULARITY_LAST+1 ; i++ ){
		if ( In.RegularitiesNormalon.ListOfRegularities[i].bActive)
			iActivadas++;
	}
	In.RegularitiesNormalon.NumActiveRegularities = iActivadas;

	In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive =	m_CDA !=0;
	In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive = m_CFP !=0; 

	In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive = m_CPL !=0; 
	In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive = m_CCL !=0; 
	In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive = m_CVL !=0;

	In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive = m_COL !=0; 

	In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_ISOMETRY].bActive = m_CI !=0;

	In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive = m_CCC !=0; 
	In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive = m_CPEC !=0; 

	In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive = m_COFO !=0; 
	In.RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive = m_CSFO !=0; 
	iActivadas= 0;
	for(long i=0 ; i<REGULARITY_LAST+1 ; i++ ){
		if ( In.RegularitiesQuasiNormalon.ListOfRegularities[i].bActive)
			iActivadas++;
	}
	In.RegularitiesQuasiNormalon.NumActiveRegularities = iActivadas;

	In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive = m_PRDA !=0;
	In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive = m_PRFP !=0; 

	In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive = m_PRPL !=0; 
	In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive = m_PRCL !=0;
	In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive = m_PRVL !=0;

	In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive = m_PROL !=0; 

	In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_ISOMETRY].bActive = m_PRI !=0;

	In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive = m_PRCC !=0; 
	In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive = m_PRPEC !=0; 

	In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive = m_PROFO !=0; 
	In.RegularitiesPrismatic.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive = m_PRSFO !=0; 
	iActivadas= 0;
	for(long i=0 ; i<REGULARITY_LAST+1 ; i++ ){
		if ( In.RegularitiesPrismatic.ListOfRegularities[i].bActive)
			iActivadas++;
	}
	In.RegularitiesPrismatic.NumActiveRegularities = iActivadas;

	In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive = m_PDA !=0;
	In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive = m_PFP !=0; 

	In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive = m_PPL !=0; 
	In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive = m_PCL !=0;
	In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive = m_PVL !=0;

	In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive = m_POL !=0; 

	In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_ISOMETRY].bActive = m_PI !=0;

	In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive = m_PCC !=0; 
	In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive = m_PPEC !=0; 

	In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive = m_POFO !=0; 
	In.RegularitiesPyramidal.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive = m_PSFO !=0; 
	iActivadas= 0;
	for(long i=0 ; i<REGULARITY_LAST+1 ; i++ ){
		if ( In.RegularitiesPyramidal.ListOfRegularities[i].bActive)
			iActivadas++;
	}
	In.RegularitiesPyramidal.NumActiveRegularities = iActivadas;

	In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive = m_IDDA !=0;
	In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive = m_IDFP !=0; 

	In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive = m_IDPL !=0; 
	In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive = m_IDCL !=0; 
	In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive = m_IDVL !=0;

	In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive = m_IDPEC !=0; 

	In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_ISOMETRY].bActive = m_IDI !=0;

	In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive = m_IDCC !=0; 
	In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive = m_IDPEC !=0; 

	In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive = m_IDOFO !=0; 
	In.RegularitiesIndefined_D.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive = m_IDSFO !=0; 
	iActivadas= 0;
	for(long i=0 ; i<REGULARITY_LAST+1 ; i++ ){
		if ( In.RegularitiesIndefined_D.ListOfRegularities[i].bActive)
			iActivadas++;
	}
	In.RegularitiesIndefined_D.NumActiveRegularities = iActivadas;

	In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive = m_INDDA !=0;
	In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive = m_INDFP !=0; 

	In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive = m_INDPL !=0; 
	In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive = m_INDCL !=0; 
	In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive = m_INDVL !=0;

	In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive = m_INDOL !=0; 

	In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_ISOMETRY].bActive = m_INDI !=0;

	In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive = m_INDCC !=0;
	In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_FACE_PERPENDICULARITY].bActive = m_INDPEC !=0; 

	In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY].bActive = m_INDOFO !=0; 
	In.RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive = m_INDSFO !=0; 
	iActivadas= 0;
	for(long i=0 ; i<REGULARITY_LAST+1 ; i++ ){
		if ( In.RegularitiesIndefined_ND.ListOfRegularities[i].bActive)
			iActivadas++;
	}
	In.RegularitiesIndefined_ND.NumActiveRegularities = iActivadas;
}


void CDialogDetailsRegularitiesNature::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	SetParams(ReferApp.In);
	UpdateData( false );
}
