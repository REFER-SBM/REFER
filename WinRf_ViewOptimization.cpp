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

// ViewOptimization.cpp : implementation file

#include <stdafx.h>

#include <cfloat>
#include <cmath>
#include <cassert>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "ReferDoc.h"
#include "Refer.h"
#include "resource.h"
#include "OptimizationCost.h"

#include "WinRf_MainFrame.h"
#include "WinRf_GDI_Transform.h"

#include "WinRf_ViewOptimization_Graphics.h"
#include "DialogOptimizationConfig.h"
#include "DialogOptimizationIsolines.h"
#include "DialogOptimizationPan.h"

#include "WinRf_ViewOptimization.h"


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewOptimization

IMPLEMENT_DYNCREATE(CWinRf_ViewOptimization, CScrollView)

CWinRf_ViewOptimization::CWinRf_ViewOptimization()
{
	bZoom = false;

	CurveCalculationInProgress = false;
	ResetCurves = true;

	PanDefined = false;

	std::vector<double> Xgrid;
	std::vector<double> Ygrid;
	std::vector<double> Fgrid;

	//Parameters to visualize isolines
	pParamsIsolines= new CParamsIsolines;
}

CWinRf_ViewOptimization::~CWinRf_ViewOptimization()
{
	if (PanDefined){
		PanDefined=false;
		pEncuadre->DestroyWindow();
	}

	if(pParamsIsolines != NULL){
		delete pParamsIsolines;
		pParamsIsolines= NULL;
	}
}


BEGIN_MESSAGE_MAP(CWinRf_ViewOptimization, CScrollView)
	//{{AFX_MSG_MAP(CWinRf_ViewOptimization)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	//ON_WM_RBUTTONDOWN()
	//ON_WM_LBUTTONDBLCLK()
	//ON_WM_RBUTTONDBLCLK()

	//Toobar Optimization
	ON_COMMAND(ID_OPTIM_VIEW_COST, OnViewCost)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_COST, OnUpdateViewCost)

	ON_COMMAND(ID_OPTIM_VIEW_Z, OnViewZ)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_Z, OnUpdateViewZ)

	ON_COMMAND(ID_OPTIM_VIEW_SOME_Z, OnViewSomeZs)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_SOME_Z, OnUpdateViewSomeZs)

	ON_COMMAND(ID_OPTIM_VIEW_ISOLINES, OnViewIsolines)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_ISOLINES, OnUpdateViewIsolines)

	ON_COMMAND(ID_OPTIM_VIEW_PARTIAL_COSTS, OnViewPartialCosts)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_PARTIAL_COSTS, OnUpdateViewPartialCosts)

	ON_COMMAND(ID_OPTIM_VIEW_LEGEND, OnShowLegend)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_LEGEND, OnUpdateShowLegend)

	ON_COMMAND(ID_OPTIM_VIEW_LABELS, OnLabels)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_LABELS, OnUpdateLabels)

	ON_COMMAND(ID_OPTIM_VIEW_CONFIG_EVOL_VARIABLES_OPT, OnViewConfigEvolutionVariablesOpt)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_CONFIG_EVOL_VARIABLES_OPT, OnUpdateViewConfigEvolutionVariablesOpt)

	ON_COMMAND(ID_OPTIM_VIEW_CONFIG_ISOLINES, OnConfigIsolines)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_CONFIG_ISOLINES, OnUpdateConfigIsolines)

	ON_COMMAND(ID_OPTIM_VIEW_ZOOM_IN, OnZoomAmpliar)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_ZOOM_IN, OnUpdateZoomAmpliar)

	ON_COMMAND(ID_OPTIM_VIEW_ZOOM_OUT, OnZoomReducir)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_ZOOM_OUT, OnUpdateZoomReducir)

	ON_COMMAND(ID_OPTIM_VIEW_ZOOM_ALL, OnZoomTodo)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_ZOOM_ALL, OnUpdateZoomTodo)

	ON_COMMAND(ID_OPTIM_VIEW_PAN_CONTROL, OnEncuadre)
	ON_UPDATE_COMMAND_UI(ID_OPTIM_VIEW_PAN_CONTROL, OnUpdateEncuadre)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CWinRf_ViewOptimization::SubSpaceTitles(CDC * pDC, CString &sTitleAxisX, CString &sTitleAxisY)
//Determines the titles of the current map of the sub-space of design
{
	CRect rect;
	GetClientRect(&rect);

	//Title of the map
	//----------------
	CFont Fuente, *pFuenteAnterior;
	Fuente.CreatePointFont( 10*10, "Arial", pDC);
	pFuenteAnterior = (CFont *) pDC->SelectObject(&Fuente);

	if (pDC->IsPrinting() != 0)
		pDC->SetTextColor( RGB( 0, 0, 255) );
	else{
		if (pParamsIsolines->Color==1){//Black and white
			if (ReferApp.m_BackgroundColor == RGB(255,255,255))//If background is white
				pDC->SetTextColor(RGB( 0, 0, 0)); //Black text
			else//If background is other color
				pDC->SetTextColor(RGB(255, 255, 255)); //White text
		}
		else{
			if (ReferApp.m_BackgroundColor==16777215)
				pDC->SetTextColor( RGB(0, 0, 0));
			else
				pDC->SetTextColor( RGB(0, 160, 255) ^ ReferApp.m_BackgroundColor);
		}
	}

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);

	CString toStringX;    toStringX.Format("%d", pParamsIsolines->X);
	CString toStringY;    toStringY.Format("%d", pParamsIsolines->Y);

	if (pParamsIsolines->TG==0){
		CString sTexto= "Cost for different values of the variable " + toStringX;
		pDC->TextOut( rect.right/2, 16, sTexto );
	}
	else{
		CString sTexto= "Isolines in the design sub-space (" + toStringX + ", " + toStringY +")";
		pDC->TextOut( rect.right/2, 16, sTexto );
	}

	//Determine labels for the axes
	sTitleAxisX = "Vertex " + toStringX;

	if (pParamsIsolines->TG==0)
		sTitleAxisY = "Cost (Z" + toStringY + ")";
	else
		sTitleAxisY = "Vertex " + toStringY;
}


void CWinRf_ViewOptimization::ResetZoomPan()
//Si es la primera vez que muestro el cuadro de dialogo
//adapto los parametros por defecto de acuerdo con las coordenadas de la imagen
{
	CReferDoc *pDoc = GetDocument();
	double dXmin, dXmax, dYmin, dYmax, dZmin, dZmax;
	pDoc->DB_Brep_Model->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);

	if (dZmin!=0 || dZmax!=0){
		pParamsIsolines->X0 = (int) (dZmin - 0.1 * (dZmax-dZmin));
		pParamsIsolines->XF = (int) (dZmax + 0.1 * (dZmax-dZmin));
		pParamsIsolines->INCX = (int) (0.15 * (dZmax-dZmin));
		if (pParamsIsolines->INCX == 0)
			pParamsIsolines->INCX= 0.5;
		pParamsIsolines->ZTX0 = pParamsIsolines->X0; 
		pParamsIsolines->ZTXF = pParamsIsolines->XF; 
		pParamsIsolines->ZTINCX = pParamsIsolines->INCX; 

		pParamsIsolines->Y0 = (int) (dZmin - 0.1 * (dZmax-dZmin));
		pParamsIsolines->YF = (int) (dZmax + 0.1 * (dZmax-dZmin));
		pParamsIsolines->INCY = (int) (0.15 * (dZmax-dZmin));
		if (pParamsIsolines->INCY == 0)
			pParamsIsolines->INCY= 0.5;
		pParamsIsolines->ZTY0 = pParamsIsolines->Y0; 
		pParamsIsolines->ZTYF = pParamsIsolines->YF;
		pParamsIsolines->ZTINCY = pParamsIsolines->INCY; 
	}
	else{
		double dRangeX, dRangeY, dRango;
		dRangeX=dXmax-dXmin;
		dRangeY=dYmax-dYmin;
		if (dRangeX>dRangeY)
			dRango = dRangeX;
		else
			dRango = dRangeY;

		pParamsIsolines->X0 = (int) (-dRango/2 - 0.1 * dRango);
		pParamsIsolines->XF = (int) (dRango/2 + 0.1 * dRango);
		pParamsIsolines->INCX = (int) (0.15 * dRango);
		if (pParamsIsolines->INCX == 0)
			pParamsIsolines->INCX = 0.5;
		pParamsIsolines->ZTX0 = pParamsIsolines->X0; 
		pParamsIsolines->ZTXF = pParamsIsolines->XF; 
		pParamsIsolines->ZTINCX = pParamsIsolines->INCX; 

		pParamsIsolines->Y0 = (int) (-dRango/2 - 0.1 * dRango);
		pParamsIsolines->YF = (int) (dRango/2 + 0.1 * dRango);
		pParamsIsolines->INCY = (int) (0.15 * dRango);
		if(pParamsIsolines->INCY == 0)
			pParamsIsolines->INCY = 0.5;
		pParamsIsolines->ZTY0 = pParamsIsolines->Y0; 
		pParamsIsolines->ZTYF = pParamsIsolines->YF; 
		pParamsIsolines->ZTINCY = pParamsIsolines->INCY; 
	}
}


void CWinRf_ViewOptimization::RecalculateGrid()
//Calculates a grid of point uniformly distributed in the frame of the map
//Calculates the cost function for each point in the grid

//The grid is unidimensional in mode TG==0,
//where only the variation of the cost against one single variable is plotted

//The grid is bidimensional in mode TG==1
//This grid is later used to interpolate the isolines that display the optimization map
{
	//Reset parameters
	//----------------
	CReferDoc *pDoc = GetDocument();

	CurveCalculationInProgress = true;

	if(ResetCurves){
		ResetCurves=false;
		ResetZoomPan();
	}

	ReferApp.In.FactorNormalizacion.clear();
	ReferApp.In.EstaCosteNormalizado.clear();
	for (long i=0; i<REGULARITY_LAST+1; i++){
		ReferApp.In.FactorNormalizacion.push_back(0.0);
		ReferApp.In.EstaCosteNormalizado.push_back(false);
	}

	Xgrid.clear();
	Ygrid.clear();
	Fgrid.clear();

	if ((ReferApp.In.Regularities.ListOfRegularities[ REGULARITY_FACE_PERPENDICULARITY ].bActive ||
		ReferApp.In.Regularities.ListOfRegularities[ REGULARITY_FACE_PLANARITY ].bActive ||
		ReferApp.In.Regularities.ListOfRegularities[ REGULARITY_LINE_COLLINEARITY ].bActive)&&
		pDoc->DB_Brep_Model->GetSizeFaces() == 0 &&
		ReferApp.In.iOptAlgorithm != NO_OPTIMIZATION){

		if (AfxMessageBox("The map displayed maybe incorrect, as some active cues need faces detection", MB_YESNO)==IDYES){
		}
	};

	//Configure the design sub-space
	std::vector<double> Solution;
	if (pParamsIsolines->OtherVariables==0){ //Reset all other Z to zero
		for (long i=0; i<pDoc->DB_Brep_Model->GetSizeVertices(); i++)
			Solution.push_back(0.0);
	}

	CVertex Vertice;
	if (pParamsIsolines->OtherVariables==1 || //Maintain current Z'
		pParamsIsolines->OtherVariables==2){
		for (long i=0; i<pDoc->DB_Brep_Model->GetSizeVertices(); i++){
				Vertice= pDoc->DB_Brep_Model->GetVertex(i);
				Solution.push_back(Vertice.z);
		}
	}

	if (pParamsIsolines->OtherVariables==2) //Modify vertex V to Z
		Solution[pParamsIsolines->V]= pParamsIsolines->Z;

	//Determine the points of the grid in X direction
	double X= pParamsIsolines->X0;
	while (X<=pParamsIsolines->XF){
		Xgrid.push_back(X);
		X+=pParamsIsolines->INCX;
	}

	//Calculate values of cost for different values of one single variable
	//--------------------------------------------------------------------
	if (pParamsIsolines->TG==0){
		for (long i=0; i<REGULARITY_LAST+1; i++){
			ReferApp.In.FactorNormalizacion.push_back(0.0);
			ReferApp.In.EstaCosteNormalizado.push_back(false);
		}
		for (long i=0; i<(long)Xgrid.size(); i++){
			Solution[pParamsIsolines->X]= Xgrid[i];

			std::vector<double> voidEvolutionCosts;
			std::vector<double> voidEvolutionZ;
			double Cost= CalculeCost(Solution, 
									 voidEvolutionCosts, voidEvolutionZ, false,
									 pDoc->DB_Brep_Model);

			Fgrid.push_back(Cost);
		}
	}

	//Calculate isolines of cost for two optimization variables map
	//-------------------------------------------------------------
	else{
		//Determine the points of the grid in Y direction
		double Y= pParamsIsolines->Y0;
		while (Y<=pParamsIsolines->YF){
			Ygrid.push_back(Y);
			Y+=pParamsIsolines->INCY;
		}

		//Calculate cost in the points of the grid
		for (long i=0; i<(long)Xgrid.size(); i++){
			Solution[pParamsIsolines->X]= Xgrid[i];
			for (long j=0; j<(long)Ygrid.size(); j++){
				Solution[pParamsIsolines->Y]= Ygrid[j];

				std::vector<double> voidEvolutionCosts;
				std::vector<double> voidEvolutionZ;
				double Cost= CalculeCost(Solution, 
										 voidEvolutionCosts, voidEvolutionZ, false,
										 pDoc->DB_Brep_Model);

				Fgrid.push_back(Cost);
			}
		}
	}

	CurveCalculationInProgress = false;
}


void CWinRf_ViewOptimization::RefineSpline(std::vector<double> &SplineXgrid,
										   std::vector<double> &SplineYgrid,
										   std::vector<double> &XPuntosControl,
										   std::vector<double> &YPuntosControl,
										   int NSSG)
//Refine the spline by interpolating intermediate points
{
	int ifb, icc, icb;
	long lIndice1, lIndice2;
	double valor1, valor2, valor3, valor4;
	double *Beta, **Smat, **Cons;
	double Geom [2][4], Prod[2][4], Pvec[4][1], Spnt[2][1];

	long NPTS = YPuntosControl.size();
	long MPTS = YPuntosControl.size()+2;

	Beta = (double*) calloc( MPTS, sizeof(double) );

	Smat = new double* [MPTS];
	for (lIndice1=0; lIndice1<MPTS; lIndice1++)
		Smat[lIndice1]=new double [3];

	Cons = new double* [2];
	for (lIndice1=0; lIndice1<2; lIndice1++)
		Cons[lIndice1]=new double [MPTS];


	//Contruyo la matriz de Coeficientes Spline
	double Coeficientes[4][4] = {2.0, -3.0, 0.0, 1.0,
								-2.0, 3.0, 0.0, 0.0,
								 99.0, 99.0, 99.0, 0.0,
								 99.0, 99.0, 0.0, 0.0};

	//Genero la matriz de valores Beta suponiendo que el número de parametros
	//es nulo
	for(lIndice1= 0; lIndice1<NPTS-1; lIndice1++){
		valor1= XPuntosControl[lIndice1];
		valor2= XPuntosControl[lIndice1+1];
		valor3= valor2-valor1;
		valor1= YPuntosControl[lIndice1];
		valor2= YPuntosControl[lIndice1+1];
		valor4= valor2-valor1;
		Beta[lIndice1]= 1.0/sqrt(valor3*valor3+valor4*valor4);
	}

	//Genero la matriz Spline
	Smat[0][1]= 2.0*Beta[0];
	Smat[0][2] = Beta[0];

	valor1= XPuntosControl[0];
	valor2= XPuntosControl[1];
	Cons[0][0] = 3.0*Beta[0]*Beta[0]*(valor2-valor1);

	valor1= YPuntosControl[0];
	valor2= YPuntosControl[1];
	Cons[1][0] = 3.0*Beta[0]*Beta[0]*(valor2-valor1);

	for (lIndice1 = 1; lIndice1<NPTS-1; lIndice1++){
		Smat[lIndice1][0] = Beta[lIndice1-1];
		Smat[lIndice1][1] = 2.0*(Beta[lIndice1-1]+Beta[lIndice1]);
		Smat[lIndice1][2] = Beta[lIndice1];

		valor1= XPuntosControl[lIndice1-1];
		valor2= XPuntosControl[lIndice1];
		valor3= XPuntosControl[lIndice1+1];
		Cons[0][lIndice1] = 3.0*Beta[lIndice1-1]*Beta[lIndice1-1]*(valor2-valor1)+
							3.0*Beta[lIndice1]*Beta[lIndice1]*(valor3-valor2);

		valor1= YPuntosControl[lIndice1-1];
		valor2= YPuntosControl[lIndice1];
		valor3= YPuntosControl[lIndice1+1];
		Cons[1][lIndice1] = 3.0*Beta[lIndice1-1]*Beta[lIndice1-1]*(valor2-valor1)+
							3.0*Beta[lIndice1]*Beta[lIndice1]*(valor3-valor2);
	}

	Smat[NPTS-1][0] = Beta[NPTS-2];
	Smat[NPTS-1][1] = 2*Beta[NPTS-2];

	valor1= XPuntosControl[NPTS-1];
	valor2= XPuntosControl[NPTS-2];
	Cons[0][NPTS-1] = 3.0*Beta[NPTS-2]*Beta[NPTS-2]*(valor1-valor2);

	valor1= YPuntosControl[NPTS-1];
	valor2= YPuntosControl[NPTS-2];
	Cons[1][NPTS-1] = 3.0*Beta[NPTS-2]*Beta[NPTS-2]*(valor1-valor2);

	//Resuelvo la matriz Spline
	for (lIndice1=0; lIndice1<NPTS; lIndice1++){
		Smat[lIndice1][2] = Smat[lIndice1][2]/Smat[lIndice1][1];
		Cons[0][lIndice1] = Cons[0][lIndice1]/Smat[lIndice1][1];
		Cons[1][lIndice1] = Cons[1][lIndice1]/Smat[lIndice1][1];
		if (lIndice1 != NPTS-1){
			Smat[lIndice1+1][1] = Smat[lIndice1+1][1]-Smat[lIndice1+1][0]*Smat[lIndice1][2];
			Cons[0][lIndice1+1] = Cons[0][lIndice1+1]-Smat[lIndice1+1][0]*Cons[0][lIndice1];
			Cons[1][lIndice1+1] = Cons[1][lIndice1+1]-Smat[lIndice1+1][0]*Cons[1][lIndice1];
		}
	}

	for (lIndice1=NPTS-2; lIndice1!=0; lIndice1--){
		Cons[0][lIndice1] = Cons[0][lIndice1]-Smat[lIndice1][2]*Cons[0][lIndice1+1];
		Cons[1][lIndice1] = Cons[1][lIndice1]-Smat[lIndice1][2]*Cons[1][lIndice1+1];
	}

	//Obtengo los puntos de la curva spline
	for (lIndice1=0; lIndice1<NPTS-1; lIndice1++){
		valor1= XPuntosControl[lIndice1];
		valor2= YPuntosControl[lIndice1];
		Geom[0][0] = valor1;
		Geom[1][0] = valor2;
		valor1= XPuntosControl[lIndice1+1];
		valor2= YPuntosControl[lIndice1+1];
		Geom[0][1] = valor1;
		Geom[1][1] = valor2;

		Geom[0][2] = Cons[0][lIndice1];
		Geom[1][2] = Cons[1][lIndice1];
		Geom[0][3] = Cons[0][lIndice1+1];
		Geom[1][3] = Cons[1][lIndice1+1];

		Coeficientes[2][0] = 1.0/Beta[lIndice1];
		Coeficientes[2][1] = -2.0*Coeficientes[2][0];
		Coeficientes[2][2] = Coeficientes[2][0];
		Coeficientes[3][0] = Coeficientes[2][0];
		Coeficientes[3][1] = -1*Coeficientes[2][0];

		//Multiplo la matriz Geom por Coeficientes y guardo el resultado en Prod
		for(ifb= 0; ifb < 2; ifb++){
			for(icc= 0; icc < 4; icc++){
				Prod[ifb][icc]= 0.0;
				for(icb= 0; icb <4; icb++)
					Prod[ifb][icc] += Geom[ifb][icb]*Coeficientes[icb][icc];
			}
		}

		for (lIndice2=0; lIndice2<=NSSG; lIndice2++){
			Pvec[3][0] = 1.0;
			Pvec[2][0] = (double)lIndice2/(double)NSSG;
			Pvec[1][0] = Pvec[2][0]*Pvec[2][0];
			Pvec[0][0] = Pvec[1][0]*Pvec[2][0];


			//Multiplo la matriz Prod por Pvec y guardo el resultado en Spnt
			for(ifb= 0; ifb < 2; ifb++){
				for(icc= 0; icc < 1; icc++){
					Spnt[ifb][icc]= 0.0;
					for(icb= 0; icb <4; icb++)
						Spnt[ifb][icc] += Prod[ifb][icb]*Pvec[icb][icc];
				}
			}

			SplineXgrid.push_back(Spnt[0][0]);
			SplineYgrid.push_back(Spnt[1][0]);
		}
	}

	free(Beta);
	for (lIndice1=0; lIndice1<MPTS; lIndice1++)
		delete Smat[lIndice1];
	delete Smat;

	for (lIndice1=0; lIndice1<2; lIndice1++)
		delete Cons[lIndice1];
	delete Cons;
}


void CWinRf_ViewOptimization::DrawSpline(std::vector<double> &XPuntosControl,
										 std::vector<double> &YPuntosControl,
										 COLORREF ColorCurvas,
										 CDC * pDC,
										 cGDI_Transform TransfEjeX,
										 cGDI_Transform TransfEjeY,
										 int InterpolationDegree, double Z, double Zmax)
//Draw the spline after refining it by interpolating intermediate points
{
	double Xi, Xf, Yi, Yf, X, Y, dPosicion;

	std::vector<double> SplineXgrid;
	std::vector<double> SplineYgrid;

	//Refine the spline curve by interpolating intermediate points
	if (InterpolationDegree !=0){
		for(long i=0; i<(long)XPuntosControl.size()-1; i+=2){
			Xi= XPuntosControl[i];
			Xf= XPuntosControl[i+1];

			if (Xi<Xf)
				X=Xi+(Xf-Xi)/2;
			else
				X=Xf+(Xi-Xf)/2;

			Yi= YPuntosControl[i];
			Yf= YPuntosControl[i+1];
			if (Yi<Yf)
				Y=Yi+(Yf-Yi)/2-((Yf-Yi)/2)/10;
			else
				Y=Yf+(Yi-Yf)/2+((Yi-Yf)/2)/10;

			XPuntosControl.insert(XPuntosControl.begin()+i+1, X);
			YPuntosControl.insert(YPuntosControl.begin()+i+1, Y);
		}

		if ((long)XPuntosControl.size()>2)
			RefineSpline(SplineXgrid, SplineYgrid,
						 XPuntosControl, YPuntosControl,
						 InterpolationDegree);
	}
	else{
		SplineXgrid= XPuntosControl;
		SplineYgrid= YPuntosControl;
	}

	//Draw the spline
	CPen PlumaCurvas;
	PlumaCurvas.CreatePen( PS_SOLID, 1, ColorCurvas);
	pDC->SelectObject( &PlumaCurvas);

	Xi= Xgrid[0];
	Xf= Xgrid[1];

	X= SplineXgrid[0];
	dPosicion= (X-Xi)*((double)Xgrid.size()/((double)Xgrid.size()-1)/(Xf-Xi));

	Y= SplineYgrid[0];

	CPoint Punto1 = TransfEjeX.GDI_WorldToViewport( dPosicion, 0 );
	CPoint Punto2 = TransfEjeY.GDI_WorldToViewport( 0, Y );
	CPoint PuntoAnterior(Punto1.x, Punto2.y);

	for (long i=1; i<(long)SplineYgrid.size(); i++){
		X= SplineXgrid[i];
		dPosicion= (X-Xi)*((double)Xgrid.size()/((double)Xgrid.size()-1)/(Xf-Xi));
		Y= SplineYgrid[i];

		Punto1 = TransfEjeX.GDI_WorldToViewport( dPosicion, 0 );
		Punto2 = TransfEjeY.GDI_WorldToViewport( 0, Y );
		CPoint Punto(Punto1.x, Punto2.y);
		pDC->MoveTo(PuntoAnterior.x, PuntoAnterior.y);
		pDC->LineTo(Punto.x, Punto.y);
		PuntoAnterior=Punto;
	}

	PlumaCurvas.DeleteObject();

	if (m_GraphicStyle.bShowLabels){
		if (Zmax-Z > 1){
			CString sTexto;
			sTexto.Format("%4.0f", (float)Z);
			pDC->TextOut( Punto1.x, Punto2.y-6, sTexto);
		}
	}
}


bool CWinRf_ViewOptimization::CutLeft(int indexX, int indexY,
									  double *pXp, double *pYp,
									  std::vector<double> &CopyXCutPoints,
									  std::vector<double> &CopyYCutPoints)
{
	bool Found=false;

	if(indexX <= 0 || indexX>=(long)Xgrid.size())
		return Found;
	if(indexY < 0 || indexY>=(long)Ygrid.size())
		return Found;

	double X= Xgrid[indexX];
	double Y= Ygrid[indexY];
	double Xmin= Xgrid[indexX-1];

	int Index;
	double Xi, Yi;

	for(int i=0; i<(long)CopyXCutPoints.size(); i++){
		Xi= CopyXCutPoints[i];
		Yi= CopyYCutPoints[i];
		if (std::fabs(X-Xi)>0.001 && std::fabs(Y-Yi)<0.001 && (Xmin<=Xi && Xi<=X)){
			if(!Found){
				Found=true;
				*pXp=Xi;
				*pYp=Yi;
				Index=i;
			}
			else{
				if (Xi>*pXp){
					*pXp=Xi;
					*pYp=Yi;
					Index=i;
				}
			}
		}
	}
	if (Found){
		CopyXCutPoints.erase(CopyXCutPoints.begin()+Index);
		CopyYCutPoints.erase(CopyYCutPoints.begin()+Index);
	}

	return Found;
}


bool CWinRf_ViewOptimization::CutRight(int indexX, int indexY,
									   double *pXp, double *pYp,
									   std::vector<double> &CopyXCutPoints,
									   std::vector<double> &CopyYCutPoints)
{
	bool Found=false;

	if(indexX < 0 || indexX>=(long)Xgrid.size()-1)
		return Found;
	if(indexY < 0 || indexY>=(long)Ygrid.size())
		return Found;

	double X= Xgrid[indexX];
	double Y= Ygrid[indexY];
	double Xmax= Xgrid[indexX+1];

	int Index;
	double Xi, Yi; 

	for(int i=0; i<(long)CopyXCutPoints.size(); i++){
		Xi= CopyXCutPoints[i];
		Yi= CopyYCutPoints[i];
		if (std::fabs(X-Xi)>0.001 && std::fabs(Y-Yi)<0.001 && (X<=Xi && Xi<=Xmax)){
			if(!Found){
				Found=true;
				*pXp=Xi;
				*pYp=Yi;
				Index=i;
			}
			else{
				if (Xi<*pXp){
					*pXp=Xi;
					*pYp=Yi;
					Index=i;
				}
			}
		}
	}
	if(Found){
		CopyXCutPoints.erase(CopyXCutPoints.begin()+Index);
		CopyYCutPoints.erase(CopyYCutPoints.begin()+Index);
	}

	return Found;
}


bool CWinRf_ViewOptimization::CutDown(int indexX, int indexY,
									  double *pXp, double *pYp,
									  std::vector<double> &CopyXCutPoints,
									  std::vector<double> &CopyYCutPoints)
{
	bool Found=false;

	if(indexX < 0 || indexX>=(long)Xgrid.size())
		return Found;
	if(indexY <= 0 || indexY>=(long)Ygrid.size())
		return Found;

	double X= Xgrid[indexX];
	double Y= Ygrid[indexY];
	double Ymin= Ygrid[indexY-1];

	int Index;
	double Xi, Yi;

	for(int i=0; i<(long)CopyXCutPoints.size(); i++){
		Xi= CopyXCutPoints[i];
		Yi= CopyYCutPoints[i];
		if (std::fabs(Y-Yi)>0.001 && std::fabs(X-Xi)<0.001 && (Ymin<=Yi && Yi<=Y)){
			if(!Found){
				Found=true;
				*pXp=Xi;
				*pYp=Yi;
				Index=i;
			}
			else{
				if (Yi>*pYp){
					*pXp=Xi;
					*pYp=Yi;
					Index=i;
				}
			}
		}
	}
	if (Found){
		CopyXCutPoints.erase(CopyXCutPoints.begin()+Index);
		CopyYCutPoints.erase(CopyYCutPoints.begin()+Index);
	}

	return Found;
}


bool CWinRf_ViewOptimization::CutUp(int indexX, int indexY,
									double *pXp, double *pYp,
									std::vector<double> &CopyXCutPoints,
									std::vector<double> &CopyYCutPoints)
{
	bool Found=false;

	if(indexX < 0 || indexX>=(long)Xgrid.size())
		return Found;
	if(indexY < 0 || indexY>=(long)Ygrid.size()-1)
		return Found;

	double X= Xgrid[indexX];
	double Y= Ygrid[indexY];
	double Ymax= Ygrid[indexY+1];

	int Index;
	double Xi, Yi;

	for(int i=0; i<(long)CopyXCutPoints.size(); i++){
		Xi= CopyXCutPoints[i];
		Yi= CopyYCutPoints[i];
		if (std::fabs(Y-Yi)>0.001 && std::fabs(X-Xi)<0.001 && (Y<=Yi && Yi<=Ymax)){
			if(!Found){
				Found=true;
				*pXp=Xi;
				*pYp=Yi;
				Index=i;
			}
			else{
				if (Yi<*pYp){
					*pXp=Xi;
					*pYp=Yi;
					Index=i;
				}
			}
		}
	}
	if(Found){
		CopyXCutPoints.erase(CopyXCutPoints.begin()+Index);
		CopyYCutPoints.erase(CopyYCutPoints.begin()+Index);
	}

	return Found;
}


void CWinRf_ViewOptimization::DrawIsolinesAxisY(CDC * pDC,
											  CRect rect,
											  long iBorderLeft, long iBorderRight,
											  long iBorderUp, long iBorderDown,
											  double Xmin, double Ymin, double Xmax, double Ymax,
											  double dRangeX, double dRangeY,
											  COLORREF ColorEjeX, COLORREF ColorEjeY,
											  double dFactorAmpliacionRangos,
											  CString sTitleAxisY,
											  cGDI_Transform &TransfEjeY)
//Draw the axis Y of the map of isolines
{
	TransfEjeY.GDI_SetViewport(iBorderLeft, iBorderUp,
							   rect.right-iBorderRight, rect.bottom-iBorderDown );
	//if (pParamsIsolines->TG==0)
	//	TransfEjeY.GDI_SetWorld(0, (double)Ymin, (double)Fgrid.size(),
	//							(double)Ymax + dRangeY*dFactorAmpliacionRangos );
	//else
		TransfEjeY.GDI_SetWorld(0, (double)Ymin, (double)Ygrid.size(),
								(double)Ymax + dRangeY*dFactorAmpliacionRangos );

	CPen PlumaEjeY;
	if (pParamsIsolines->TG==0)
		PlumaEjeY.CreatePen( PS_SOLID, 1, ColorEjeY );
	else
		PlumaEjeY.CreatePen( PS_SOLID, 1, ColorEjeX );
	pDC->SelectObject( &PlumaEjeY );

	pDC->MoveTo(iBorderLeft, iBorderUp);
	pDC->LineTo(iBorderLeft, rect.bottom - iBorderDown);

	//Add title
	if (pParamsIsolines->TG==0)
		pDC->SetTextColor( ColorEjeY );
	else
		pDC->SetTextColor( ColorEjeX );

	pDC->SetTextAlign(TA_BOTTOM | TA_CENTER);
	pDC->TextOut( iBorderLeft, iBorderUp-2, sTitleAxisY);
	pDC->SetTextAlign(TA_BOTTOM | TA_RIGHT);

	//Dibuja lineas de division y etiquetas de valores en el Eje Y
	//Si el gráfico es de una variable y existen valores a representar en Y
	double Y;
	if((pParamsIsolines->TG==0 && (long)Fgrid.size()!=0) ||
		(pParamsIsolines->TG==1 && (long)Ygrid.size()!=0)){
		//Si todos ellos son iguales
		if(Ymax==Ymin){
			long lNumeroDivisionesEjeY=1;
			//Trazo una pequeña marca
			pDC->MoveTo(iBorderLeft-2, (rect.bottom - iBorderDown)/2);
			pDC->LineTo(iBorderLeft+2, (rect.bottom - iBorderDown)/2);

			//Anoto el valor de dicho coste
			CString sTexto;
			sTexto.Format("%4.2f", (float)Ymin);
			pDC->TextOut( iBorderLeft-3, (rect.bottom - iBorderDown)/2+6, sTexto );
		}
		else{
			//Si existen costos distintos
			long SpacingBetweenYLabels= 40;
			long NumDivisionsY= max((rect.bottom - (iBorderUp + iBorderDown)) / SpacingBetweenYLabels, 1);
			Y = Ymin;
			for (long lIndice=0 ; lIndice <= NumDivisionsY ; lIndice++) {
				CPoint Punto = TransfEjeY.GDI_WorldToViewport(0, Y);
				pDC->MoveTo( Punto.x-2, Punto.y );
				pDC->LineTo( Punto.x+2, Punto.y );
				CString sTexto;
				sTexto.Format("%4.2f", (float)Y);
				pDC->TextOut( Punto.x-3, Punto.y+6, sTexto );
				Y += dRangeY / ((double)NumDivisionsY);
			}
		}
	}

	PlumaEjeY.DeleteObject();
}


void CWinRf_ViewOptimization::DrawIsolinesAxisX(CDC * pDC,
											  CRect rect,
											  long iBorderLeft, long iBorderRight,
											  long iBorderUp, long iBorderDown,
											  double Xmin, double Ymin, double Xmax, double Ymax,
											  double dRangeX, double dRangeY,
											  COLORREF ColorEjeX, COLORREF ColorEjeY,
											  double dFactorAmpliacionRangos,
											  CString sTitleAxisX,
											  cGDI_Transform &TransfEjeX)
//Draw the axis X of the map of isolines
{
	TransfEjeX.GDI_SetViewport(iBorderLeft, iBorderUp,
							   rect.right-iBorderRight, rect.bottom-iBorderDown );
	TransfEjeX.GDI_SetWorld(0,
							(double)Xmin - dRangeX*dFactorAmpliacionRangos,
							(double)Xgrid.size(),
							(double)Xmax + dRangeX*dFactorAmpliacionRangos );

	CPen PlumaEjeX;
	PlumaEjeX.CreatePen( PS_SOLID, 1, ColorEjeX );
	pDC->SelectObject( &PlumaEjeX );
	pDC->MoveTo(iBorderLeft, rect.bottom - iBorderDown);
	pDC->LineTo(rect.right-iBorderRight, rect.bottom - iBorderDown);
	pDC->SetTextColor( ColorEjeX );
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	pDC->TextOut( rect.right/2, rect.bottom - iBorderDown/2, sTitleAxisX );

	// Dibuja lineas de division y etiquetas de valores en el Eje X
	double X;
	if ((long)Xgrid.size()!=0){
		//Si solo existe un valor a representar
		if ((long)Xgrid.size()==1){
			long lNumeroDivisionesEjeX=1;
			X= Xgrid[0];
			pDC->MoveTo( iBorderLeft+(rect.right-iBorderRight-iBorderLeft)/2, rect.bottom-iBorderDown-2 );
			pDC->LineTo( iBorderLeft+(rect.right-iBorderRight-iBorderLeft)/2, rect.bottom-iBorderDown+2 );
			CString sTexto;
			sTexto.Format("%4.2f", (float)X);
			pDC->TextOut( iBorderLeft+(rect.right-iBorderRight-iBorderLeft)/2, rect.bottom-iBorderDown, sTexto );
		}
		else{
			long SpacingBetweenXLabels= 40;
			long NumDivisionsX = max((rect.right - (iBorderLeft + iBorderRight)) / SpacingBetweenXLabels, 1);
			double dPosicion=0;
			X=Xmin;
			for (long lIndice=0 ; lIndice <= NumDivisionsX ; lIndice++) {
				CPoint Punto = TransfEjeX.GDI_WorldToViewport( dPosicion, Ymin );
				Punto.y = rect.bottom-iBorderDown;
				pDC->MoveTo( Punto.x, Punto.y-2 );
				pDC->LineTo( Punto.x, Punto.y+2 );
				CString sTexto;
				sTexto.Format("%4.2f", (float)X);
				pDC->TextOut( Punto.x, Punto.y+3, sTexto );
				X += dRangeX / ((double)NumDivisionsX);
				dPosicion += ((double)Xgrid.size() / (double)NumDivisionsX);
			}
		}
	}
	PlumaEjeX.DeleteObject();
}


void CWinRf_ViewOptimization::DetectIsolines1(CDC * pDC,
											  COLORREF ColorCurvas,
											  cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
											  int InterpolationDegree,
											  double Z, double Zmax,
											  std::vector<double> &CopyXCutPoints,
											  std::vector<double> &CopyYCutPoints)
//DETECT ISOLINES STEP 1
	//Busco regiones cerradas alrededor de cada uno de los
	//vértices de la cuadricula definida comenzando por los extremos
	//que definen el maping de cuadriculas
	//Primero en la linea inferior, luego la superior
{
	CopyXCutPoints= XPuntosCorte;
	CopyYCutPoints= YPuntosCorte;

	//Loop over lower and upper borders
	for(long FilaY=0; FilaY<(long)Ygrid.size(); FilaY+=(long)Ygrid.size()-1){
		//Loop over all the columns of the grid
		for(long FilaX=0; FilaX<(long)Xgrid.size(); FilaX++){
			std::vector<double> XPuntosControl;
			std::vector<double> YPuntosControl;

			bool Found=true;
			//Si no es un punto del limite inferior, busco corte con la parte
			//negativa del eje Y centrado el origen el punto analizado
			if (FilaY!=0){
				double Xp, Yp;
				Found =CutDown(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
				//Si existe punto de corte lo guardo para dibujar la Spline
				//y lo elimino de los puntos de corte
				if (Found){
					XPuntosControl.push_back(Xp);
					YPuntosControl.push_back(Yp);
				}
			}

			//Si no es el primer punto de la fila y ha pasado el test anterior,
			//busco corte con la parte negativa del eje X centrado el origen el
			//punto analizado
			if (FilaX!=0 && Found){
				double Xp, Yp;
				Found =CutLeft(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
				//Si existe punto de corte lo guardo para dibujar la Spline
				//y lo elimino de los puntos de corte
				if (Found){
					XPuntosControl.push_back(Xp);
					YPuntosControl.push_back(Yp);
				}
			}

			//Si se ha pasado el test anterior y no es un punto de la última
			//Fila Y busco corte con la parte positiva del eje Y centrado el
			//origen en el punto analizado
			if (FilaY!=(long)Ygrid.size()-1 && Found){
				double Xp, Yp;
				Found =CutUp(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
				//Si existe punto de corte lo guardo para dibujar la Spline
				//y lo elimino de los puntos de corte
				if (Found){
					XPuntosControl.push_back(Xp);
					YPuntosControl.push_back(Yp);
				}
			}

			//Si no es el último punto, y se ha pasado el test anterior
			//busco corte con la parte positiva del eje X centrado el origen
			//el punto analizado
			if (Found && FilaX!=(long)Xgrid.size()-1){
				double Xp, Yp;
				Found =CutRight(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
				//Si existe punto de corte lo guardo para dibujar la Spline
				//y lo elimino de los puntos de corte
				if (Found){
					XPuntosControl.push_back(Xp);
					YPuntosControl.push_back(Yp);
				}
			}

			//Si se dibuja alguna curva actualizo los puntos de corte que
			//aun quedan pendientes de dibujar
			if (Found){
				XPuntosCorte= CopyXCutPoints;
				YPuntosCorte= CopyYCutPoints;
			}
			//Si no he conseguido dibujar ninguna curva restauro los puntos de corte
			//por si se ha cumplido alguna condición y se han eliminado algunos
			//puntos
			else{
				CopyXCutPoints= XPuntosCorte;
				CopyYCutPoints= YPuntosCorte;
			}

			if (Found && (long)XPuntosControl.size()>1){
				//Dibujaré la curva
				//Antes compruebo si estoy en el margen superior, en cuyo
				//caso deberé modificar el orden de los puntos
				if (FilaX!=(long)Ygrid.size()-1 && FilaY==(long)Ygrid.size()-1){
					double Xp= XPuntosControl[(long)XPuntosControl.size()-1];
					double Yp= YPuntosControl[(long)XPuntosControl.size()-1];
					XPuntosControl.erase(XPuntosControl.begin()+(long)XPuntosControl.size()-1);
					YPuntosControl.erase(YPuntosControl.begin()+(long)YPuntosControl.size()-1);
					XPuntosControl.insert(XPuntosControl.begin()+0, Xp);
					YPuntosControl.insert(YPuntosControl.begin()+0, Yp);
				}

				pDC->SetTextColor( ColorCurvas );
				DrawSpline(XPuntosControl, YPuntosControl,
						   ColorCurvas, pDC, TransfEjeX, TransfEjeY,
						   InterpolationDegree, Z, Zmax);
			}
		}
	}
}


void CWinRf_ViewOptimization::DetectIsolines2(CDC * pDC,
											  COLORREF ColorCurvas,
											  cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
											  int InterpolationDegree,
											  double Z, double Zmax,
											  std::vector<double> &CopyXCutPoints,
											  std::vector<double> &CopyYCutPoints)
//DETECT ISOLINES STEP 2
	//Busco regiones cerradas alrededor de cada uno de los
	//vértices de la cuadricula definida comenzando por los extremos
	//que definen el maping de cuadriculas
	//Primero en la linea izquierda, luego la derewcha
{
	//Loop over left and right borders
	for(long FilaX=0; FilaX<(long)Xgrid.size(); FilaX+=(long)Xgrid.size()-1){
		//Loop over all the rows of the grid
		for(long FilaY=0; FilaY<(long)Ygrid.size(); FilaY++){
			std::vector<double> XPuntosControl;
			std::vector<double> YPuntosControl;

			bool Found=true;
			//Si no es un punto del limite inferior, busco corte con la parte
			//negativa del eje Y centrado el origen el punto analizado
			if (FilaY!=0){
				double Xp, Yp;
				Found =CutDown(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
				//Si existe punto de corte lo guardo para dibujar la Spline
				//y lo elimino de los puntos de corte
				if (Found){
					XPuntosControl.push_back(Xp);
					YPuntosControl.push_back(Yp);
				}
			}

			//Si no es el primer punto de la fila y  ha pasado el test anterior,
			//busco corte con la parte negativa del eje X centrado el origen el
			//punto analizado
			if (FilaX!=0 && Found){
				double Xp, Yp;
				Found =CutLeft(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
				//Si existe punto de corte lo guardo para dibujar la Spline
				//y lo elimino de los puntos de corte
				if (Found){
					XPuntosControl.push_back(Xp);
					YPuntosControl.push_back(Yp);
				}
			}

			//Si se ha pasado el test anterior y no es un punto de la última
			//Fila Y busco corte con la parte positiva del eje Y centrado el
			//origen en el punto analizado
			if (FilaY!=(long)Ygrid.size()-1 && Found){
				double Xp, Yp;
				Found =CutUp(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
				//Si existe punto de corte lo guardo para dibujar la Spline
				//y lo elimino de los puntos de corte
				if (Found){
					XPuntosControl.push_back(Xp);
					YPuntosControl.push_back(Yp);
				}
			}

			//Si no es el último punto, y se ha pasado el test anterior
			//busco corte con la parte positiva del eje X centrado el origen
			//el punto analizado
			if (Found && FilaX!=(long)Xgrid.size()-1){
				double Xp, Yp;
				Found =CutRight(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
				//Si existe punto de corte lo guardo para dibujar la Spline
				//y lo elimino de los puntos de corte
				if (Found){
					XPuntosControl.push_back(Xp);
					YPuntosControl.push_back(Yp);
				}
			}

			//Si se dibuja alguna curva actualizo los puntos de corte que
			//aun quedan pendientes de dibujar
			if (Found){
				XPuntosCorte= CopyXCutPoints;
				YPuntosCorte= CopyYCutPoints;
			}
			//Si no conseguido dibujar ninguna curva restauro los puntos de corte
			//por si se ha cumplido alguna condición y se han eliminado algunos
			//puntos
			else{
				CopyXCutPoints= XPuntosCorte;
				CopyYCutPoints= YPuntosCorte;
			}

			if (Found){
			//Dibujaré la curva
			//Antes compruebo si estoy en el margen izquierdo, en cuyo caso
			//deberé modificar el orden de los puntos
				if (FilaX==0 && FilaY!=0 && FilaY!=(long)Ygrid.size()-1){
					double Xp= XPuntosControl[0];
					double Yp= YPuntosControl[0];
					XPuntosControl.erase(XPuntosControl.begin()+0);
					YPuntosControl.erase(YPuntosControl.begin()+0);
					XPuntosControl.push_back(Xp);
					YPuntosControl.push_back(Yp);
				}
				pDC->SetTextColor( ColorCurvas );
				DrawSpline(XPuntosControl, YPuntosControl,
						   ColorCurvas, pDC, TransfEjeX, TransfEjeY,
						   InterpolationDegree, Z, Zmax);
			}
		}
	}
}


void CWinRf_ViewOptimization::DetectIsolines3(CDC * pDC,
											  COLORREF ColorCurvas,
											  cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
											  int InterpolationDegree,
											  double Z, double Zmax,
											  std::vector<double> &CopyXCutPoints,
											  std::vector<double> &CopyYCutPoints)
//DETECT ISOLINES STEP 3
	//Repito exactamente igual para comprobar el resto de los puntos que no
	//pertenencen a ningún laterales del maping
	//Circuitos cerrados alrededor de cualquier vertice del interior del
	//maping
{
	for(long FilaY=1; FilaY<(long)Ygrid.size()-1; FilaY++){
		for(long FilaX=1; FilaX<(long)Xgrid.size()-1; FilaX++){
			std::vector<double> XPuntosControl;
			std::vector<double> YPuntosControl;

			//Busco corte con la parte negativa del eje Y centrado el origen
			//el punto analizado
			double Xp, Yp;
			bool Found= CutDown(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
			//Si existe punto de corte lo guardo para dibujar la Spline
			//y lo elimino de los puntos de corte
			if (Found){
				XPuntosControl.push_back(Xp);
				YPuntosControl.push_back(Yp);
			}

			//Si ha pasado el test anterior, busco corte con la parte
			//negativa del eje X centrado el origen el punto analizado
			if (Found){
				Found =CutLeft(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
				//Si existe punto de corte lo guardo para dibujar la Spline
				//y lo elimino de los puntos de corte
				if (Found){
					XPuntosControl.push_back(Xp);
					YPuntosControl.push_back(Yp);
				}
			}

			//Si se ha pasado el test anterior busco corte con la parte
			//positiva del eje Y centrado el origen en el punto analizado
			if (Found){
				Found =CutUp(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
				//Si existe punto de corte lo guardo para dibujar la Spline
				//y lo elimino de los puntos de corte
				if (Found){
					XPuntosControl.push_back(Xp);
					YPuntosControl.push_back(Yp);
				}
			}

			//Si se ha pasado el test anterior busco corte con la parte
			//positiva del eje X centrado el origen el punto analizado
			if (Found){
				Found =CutRight(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
				//Si existe punto de corte lo guardo para dibujar la Spline
				//y lo elimino de los puntos de corte
				if (Found){
					XPuntosControl.push_back(Xp);
					YPuntosControl.push_back(Yp);
				}
			}

			//Si se dibuja alguna curva actualizo los puntos de corte que
			//aun quedan pendientes de dibujar
			if (Found){
				XPuntosCorte= CopyXCutPoints;
				YPuntosCorte= CopyYCutPoints;
			}
			//Si no conseguido dibujar ninguna curva restauro los puntos de corte
			//por si se ha cumplido alguna condición y se han eliminado algunos
			//puntos
			else{
				CopyXCutPoints= XPuntosCorte;
				CopyYCutPoints= YPuntosCorte;
			}

			if (Found){
				//Dibujaré la curva
				//Antes debo cerrar el poligono
				double Xi= XPuntosControl[0];
				double Yi= YPuntosControl[0];
				XPuntosControl.push_back(Xi);
				YPuntosControl.push_back(Yi);
				pDC->SetTextColor( ColorCurvas );
				DrawSpline(XPuntosControl, YPuntosControl,
						   ColorCurvas, pDC, TransfEjeX, TransfEjeY,
						   InterpolationDegree, Z, Zmax);
			}
		}
	}
}


void CWinRf_ViewOptimization::DetectIsolines4(CDC * pDC,
											  COLORREF ColorCurvas,
											  cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
											  int InterpolationDegree,
											  double Z, double Zmax,
											  std::vector<double> &CopyXCutPoints,
											  std::vector<double> &CopyYCutPoints)
//DETECT ISOLINES STEP 4
	//Ahora miro como unir el resto de puntos si es que quedan
	//Busco primero aquellas curvas que tengan entrada o salida del maping
	//por el margen superior
{
	if ((long)CopyXCutPoints.size()!=0){
		double Xmin= Xgrid[0];
		double Xmax= Xgrid[(long)Xgrid.size()-1];
		double Ymin= Ygrid[0];
		double Ymax= Ygrid[(long)Ygrid.size()-1];

		//Comienzo a buscar en la esquina superior izquierda y me voy
		//desplazando hacia la derecha
		for(long FilaX1=0; FilaX1<(long)Xgrid.size()-1; FilaX1++){
			long FilaY1=(long)Ygrid.size()-1;
			long FilaX=FilaX1;
			long FilaY=FilaY1;
			std::vector<double> XPuntosControl;
			std::vector<double> YPuntosControl;
			double Xp, Yp;
			bool Found= CutRight(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
			if (!Found && FilaX==0)
				Found=CutDown(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
			if(Found){
				XPuntosControl.push_back(Xp);
				YPuntosControl.push_back(Yp);
				FilaX++; FilaY--;
				do{
					Found=false;
					if (FilaY<(long)Ygrid.size()-1)
						Found=CutUp(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
					if (Found){
						XPuntosControl.push_back(Xp);
						YPuntosControl.push_back(Yp);
						FilaX++;
					}
					else{
						if (FilaX>0)
							Found=CutLeft(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
						if (Found){
							XPuntosControl.push_back(Xp);
							YPuntosControl.push_back(Yp);
							FilaY--;
						}
						else{
							FilaX--;
							if (FilaY<(long)Ygrid.size()-1)
								Found=CutUp(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
							if (Found){
								XPuntosControl.push_back(Xp);
								YPuntosControl.push_back(Yp);
							}
							else{
								FilaX++; FilaY++;
								if (FilaX>0)
									Found=CutLeft(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
								if(Found){
									XPuntosControl.push_back(Xp);
									YPuntosControl.push_back(Yp);
								}
							}
						}
					}
				}while(Found &&
					   std::fabs(Xmin-Xp)>0.001 && std::fabs(Xmax-Xp)>0.001 && 
					   std::fabs(Ymin-Yp)>0.001 && std::fabs(Ymax-Yp)>0.001);
			}

			//Si se dibuja alguna curva actualizo los puntos de corte que
			//aun quedan pendientes de dibujar
			if (Found && (long)XPuntosControl.size()>1){
				XPuntosCorte= CopyXCutPoints;
				YPuntosCorte= CopyYCutPoints;
			}
			//Si no conseguido dibujar ninguna curva restauro los puntos de corte
			//por si se ha cumplido alguna condición y se han eliminado algunos
			//puntos
			else{
				CopyXCutPoints= XPuntosCorte;
				CopyYCutPoints= YPuntosCorte;
			}

			//Si existen puntos dibujo la curva
			if (Found && (long)XPuntosControl.size()>1){
				pDC->SetTextColor( ColorCurvas );
				DrawSpline(XPuntosControl, YPuntosControl,
						   ColorCurvas, pDC, TransfEjeX, TransfEjeY,
						   InterpolationDegree, Z, Zmax);
			}
		}
	}
}


void CWinRf_ViewOptimization::DetectIsolines5(CDC * pDC,
											  COLORREF ColorCurvas,
											  cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
											  int InterpolationDegree,
											  double Z, double Zmax,
											  double Xmin, double Xmax, double Ymin, double Ymax,
											  std::vector<double> &CopyXCutPoints,
											  std::vector<double> &CopyYCutPoints)
//DETECT ISOLINES STEP 5
	//Ahora busco que tengan entrada o salida por el margen inferior
{
	if ((long)CopyXCutPoints.size()!=0){
		//Comienzo a buscar en la esquina inferior izquierda y me voy
		//desplazando hacia la derecha
		for(long FilaX1=0; FilaX1<(long)Xgrid.size()-1; FilaX1++){
			long FilaY1=0;
			long FilaX=FilaX1;
			long FilaY=FilaY1;
			std::vector<double> XPuntosControl;
			std::vector<double> YPuntosControl;
			double Xp, Yp;
			bool Found= CutRight(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
			if (!Found && FilaX==0)
				Found=CutUp(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
			if(Found){
				XPuntosControl.push_back(Xp);
				YPuntosControl.push_back(Yp);
				FilaX++; FilaY++;
				do{
					Found=false;
					if (FilaY>0)//<(long)Ygrid.size()-1) //OJO
						Found=CutDown(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
					if (Found){
						XPuntosControl.push_back(Xp);
						YPuntosControl.push_back(Yp);
						FilaX++;
					}
					else{
						if (FilaX>0)
							Found=CutLeft(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
						if (Found){
							XPuntosControl.push_back(Xp);
							YPuntosControl.push_back(Yp);
							FilaY++;
						}
						else{
							FilaX--;
							if (FilaY>0)//(long)Ygrid.size()-1)//FALLO
								Found=CutDown(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
							if (Found){
								XPuntosControl.push_back(Xp);
								YPuntosControl.push_back(Yp);
							}
							else{
								FilaX++; FilaY--;
								if (FilaX>0)
									Found=CutLeft(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
								if(Found){
									XPuntosControl.push_back(Xp);
									YPuntosControl.push_back(Yp);
								}
							}
						}
					}
				}while(Found &&
					   std::fabs(Xmin-Xp)>0.001 && std::fabs(Xmax-Xp)>0.001 && 
					   std::fabs(Ymin-Yp)>0.001 && std::fabs(Ymax-Yp)>0.001);
			}

			//Si se dibuja alguna curva actualizo los puntos de corte que
			//aun quedan pendientes de dibujar
			if (Found){
				XPuntosCorte= CopyXCutPoints;
				YPuntosCorte= CopyYCutPoints;
			}
			//Si no conseguido dibujar ninguna curva restauro los puntos de corte
			//por si se ha cumplido alguna condición y se han eliminado algunos
			//puntos
			else{
				CopyXCutPoints= XPuntosCorte;
				CopyYCutPoints= YPuntosCorte;
			}

			//Si existen puntos dibujo la curva
			if ((long)XPuntosControl.size()>1){
				pDC->SetTextColor( ColorCurvas );
				DrawSpline(XPuntosControl, YPuntosControl,
						   ColorCurvas, pDC, TransfEjeX, TransfEjeY,
						   InterpolationDegree, Z, Zmax);
			}
		}
	}
}


void CWinRf_ViewOptimization::DetectIsolines6(CDC * pDC,
											  COLORREF ColorCurvas,
											  cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
											  int InterpolationDegree,
											  double Z, double Zmax,
											  double Xmin, double Xmax, double Ymin, double Ymax,
											  std::vector<double> &CopyXCutPoints,
											  std::vector<double> &CopyYCutPoints)
//DETECT ISOLINES STEP 6
	//Ahora busco que tengan entrada o salida por el margen izquierdo
{
	if ((long)CopyXCutPoints.size()!=0){
		//Comienzo a buscar en la semi-esquina inferior izquierda y me voy
		//desplazando hacia arriba
		for(long FilaY1=2; FilaY1<(long)Ygrid.size()-1; FilaY1++){
			long FilaX1=0;
			long FilaX=FilaX1;
			long FilaY=FilaY1;
			std::vector<double> XPuntosControl;
			std::vector<double> YPuntosControl;
			double Xp, Yp;
			bool Found= CutDown(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
			if(Found){
				XPuntosControl.push_back(Xp);
				YPuntosControl.push_back(Yp);
				FilaX++;
				do{
					Found=false;
					if (FilaX>0)
						Found=CutLeft(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
					if (Found){
						XPuntosControl.push_back(Xp);
						YPuntosControl.push_back(Yp);
						FilaY++;
					}
					else{
						if (FilaY>0)
							Found=CutDown(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
						if (Found){
							XPuntosControl.push_back(Xp);
							YPuntosControl.push_back(Yp);
							FilaX++;
//							FilaY--;
						}
						else{
							FilaY--;
							if (FilaY>0)
								Found=CutLeft(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
							if (Found){
								XPuntosControl.push_back(Xp);
								YPuntosControl.push_back(Yp);
							}
							else{
								FilaX--; FilaY++;
								if (FilaX>0)
									Found=CutDown(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
								if(Found){
									XPuntosControl.push_back(Xp);
									YPuntosControl.push_back(Yp);
								}
							}
						}
					}
				}while(Found &&
					   std::fabs(Xmin-Xp)>0.001 && std::fabs(Xmax-Xp)>0.001 && 
					   std::fabs(Ymin-Yp)>0.001 && std::fabs(Ymax-Yp)>0.001);
			}

			//Si se dibuja alguna curva actualizo los puntos de corte que
			//aun quedan pendientes de dibujar
			if (Found){
				XPuntosCorte= CopyXCutPoints;
				YPuntosCorte= CopyYCutPoints;
			}
			//Si no conseguido dibujar ninguna curva restauro los puntos de corte
			//por si se ha cumplido alguna condición y se han eliminado algunos
			//puntos
			else{
				CopyXCutPoints= XPuntosCorte;
				CopyYCutPoints= YPuntosCorte;
			}

			//Si existen puntos dibujo la curva
			if ((long)XPuntosControl.size()>1){
				pDC->SetTextColor( ColorCurvas );
				DrawSpline(XPuntosControl, YPuntosControl,
						   ColorCurvas, pDC, TransfEjeX, TransfEjeY,
						   InterpolationDegree, Z, Zmax);
			}
		}
	}
}


void CWinRf_ViewOptimization::DetectIsolines7(CDC * pDC,
											  COLORREF ColorCurvas,
											  cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
											  int InterpolationDegree,
											  double Z, double Zmax,
											  double Xmin, double Xmax, double Ymin, double Ymax,
											  std::vector<double> &CopyXCutPoints,
											  std::vector<double> &CopyYCutPoints)
//DETECT ISOLINES STEP 7
	//Ahora busco que tengan entrada o salida por el margen derecho
{
	if ((long)CopyXCutPoints.size()!=0){
		//Comienzo a buscar en la semi-esquina inferior derecha y me voy
		//desplazando hacia arriba
		for(long FilaY1=2; FilaY1<(long)Ygrid.size()-1; FilaY1++){
			long FilaX1=(long)Xgrid.size()-1;
			long FilaX=FilaX1;
			long FilaY=FilaY1;
			std::vector<double> XPuntosControl;
			std::vector<double> YPuntosControl;
			double Xp, Yp;
			bool Found= CutDown(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
			if(Found){
				XPuntosControl.push_back(Xp);
				YPuntosControl.push_back(Yp);
				FilaX--;
				do{
					Found=false;
					if (FilaX<(long)Xgrid.size()-1)
						Found=CutRight(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
					if (Found){
						XPuntosControl.push_back(Xp);
						YPuntosControl.push_back(Yp);
						FilaY++;
					}
					else{
						if (FilaY>0)
							Found=CutDown(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
						if (Found){
							XPuntosControl.push_back(Xp);
							YPuntosControl.push_back(Yp);
							FilaX--;
						}
						else{
							FilaY--;
							if (FilaX<(long)Xgrid.size()-1)
								Found=CutRight(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
							if (Found){
								XPuntosControl.push_back(Xp);
								YPuntosControl.push_back(Yp);
							}
							else{
								FilaX++; FilaY--;
								if (FilaX>0)
									Found=CutDown(FilaX, FilaY, &Xp, &Yp, CopyXCutPoints, CopyYCutPoints);
								if(Found){
									XPuntosControl.push_back(Xp);
									YPuntosControl.push_back(Yp);
								}
							}
						}
					}
				}while(Found &&
					   std::fabs(Xmin-Xp)>0.001 && std::fabs(Xmax-Xp)>0.001 &&
					   std::fabs(Ymin-Yp)>0.001 && std::fabs(Ymax-Yp)>0.001);
			}

			//Si se dibuja alguna curva actualizo los puntos de corte que
			//aun quedan pendientes de dibujar
			if (Found){
				XPuntosCorte= CopyXCutPoints;
				YPuntosCorte= CopyYCutPoints;
			}
			//Si no conseguido dibujar ninguna curva restauro los puntos de corte
			//por si se ha cumplido alguna condición y se han eliminado algunos
			//puntos
			else{
				CopyXCutPoints= XPuntosCorte;
				CopyYCutPoints= YPuntosCorte;
			}

			//Si existen puntos dibujo la curva
			if ((long)XPuntosControl.size()>1){
				pDC->SetTextColor( ColorCurvas );
				DrawSpline(XPuntosControl, YPuntosControl,
						   ColorCurvas, pDC, TransfEjeX, TransfEjeY,
						   InterpolationDegree, Z, Zmax);
			}
		}
	}
}


void CWinRf_ViewOptimization::DrawIsoLines(CDC * pDC, CRect rect,
										   int InterpolationDegree,
										   long iBorderLeft, long iBorderRight,
										   long iBorderUp, long iBorderDown,
										   double Xmin, double Xmax,
										   double Ymin, double Ymax,
										   double Zmin, double Zmax,
										   cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY)
//Draw the isolines of the design sub-space
{
	//Configure text
	//-------------
	CFont FuenteRotulos;
	FuenteRotulos.CreatePointFont( 7*10, "Arial", pDC);
	CFont *pFuenteAnterior;
	pFuenteAnterior = (CFont *) pDC->SelectObject(&FuenteRotulos);
	pDC->SetBkMode(OPAQUE);

	//Draw peaks and depths
	//---------------------
	for(long i=0; i<(long)Fgrid.size(); i++){
		if (std::fabs(Fgrid[i] - Zmin)<0.001){
			COLORREF ColorCurvas;
			if (pParamsIsolines->Color == 1){ //En blanco y negro
				if (ReferApp.m_BackgroundColor == RGB(255,255,255)) //Si el fondo es blanco
					ColorCurvas = RGB( 0, 0, 0 );//Lineas negras
				else //Si el fondo es de otro color, lineas blancas
					ColorCurvas = RGB( 255, 255, 255 );
			}
			else
				ColorCurvas = RGB(0, 255, 0)^ ReferApp.m_BackgroundColor;

			CPen PlumaCurvas;
			PlumaCurvas.CreatePen( PS_SOLID, 0, ColorCurvas );
			pDC->SelectObject( &PlumaCurvas );

			CBrush *pPincelActual = new CBrush;
			pPincelActual->CreateSolidBrush(ColorCurvas);
			pDC->SelectObject(pPincelActual);

			div_t Fila_Columna= div((int)i, (int)Ygrid.size());
			double dPosicion=0;
			dPosicion+=Fila_Columna.quot*(double)Xgrid.size()/((double)Xgrid.size()-1);
			CPoint Punto1 = TransfEjeX.GDI_WorldToViewport( dPosicion, 0 );
			CPoint Punto2 = TransfEjeY.GDI_WorldToViewport( 0, Ygrid[Fila_Columna.rem] );
			Ellipse (*pDC, Punto1.x-3, Punto2.y-3, Punto1.x+3, Punto2.y+3);

			if (m_GraphicStyle.bShowLabels){
				CString sTexto, toString;
				pDC->SetTextColor( ColorCurvas );
				toString.Format("%4.0f", (float)Zmin);
				sTexto= "MIN" + toString;
				pDC->TextOut( Punto1.x, Punto2.y-24, sTexto);
			}
			PlumaCurvas.DeleteObject();
			pPincelActual->DeleteObject();
		}
		else if (std::fabs(Zmax - Fgrid[i])<0.001){
			COLORREF ColorCurvas;
			if (pParamsIsolines->Color == 1){ //En blanco y negro
				if (ReferApp.m_BackgroundColor == RGB(255,255,255)) //Si el fondo es blanco
					ColorCurvas = RGB( 0, 0, 0 );//Lineas negras
				else //Si el fondo es de otro color, lineas blancas
					ColorCurvas = RGB( 255, 255, 255 );
			}
			else
				ColorCurvas = RGB(255, 0, 0) ^ ReferApp.m_BackgroundColor;

			CPen PlumaCurvas;
			PlumaCurvas.CreatePen( PS_SOLID, 0, ColorCurvas );
			pDC->SelectObject( &PlumaCurvas );

			CBrush *pPincelActual = new CBrush;
			pPincelActual->CreateSolidBrush(ColorCurvas);
			pDC->SelectObject(pPincelActual);

			div_t Fila_Columna= div((int)i, (int)Ygrid.size());
			double dPosicion=0;
			dPosicion+=Fila_Columna.quot*(double)Xgrid.size()/((double)Xgrid.size()-1);
			CPoint Punto1 = TransfEjeX.GDI_WorldToViewport( dPosicion, 0 );
			CPoint Punto2 = TransfEjeY.GDI_WorldToViewport( 0, Ygrid[Fila_Columna.rem] );
			Ellipse (*pDC, Punto1.x-3, Punto2.y-3, Punto1.x+3, Punto2.y+3);

			if (m_GraphicStyle.bShowLabels){
				CString sTexto, toString;
				pDC->SetTextColor( ColorCurvas );
				toString.Format("%4.0f", (float)Zmax);
				sTexto= "MAX" + toString;
				pDC->TextOut( Punto1.x, Punto2.y-24, sTexto);
			}

			PlumaCurvas.DeleteObject();
			pPincelActual->DeleteObject();
		}
	}

	//Set params to calculate isolines
	//--------------------------------
	double IncrementoEntreCurvas=(Zmax-Zmin)*pParamsIsolines->INCZ;
	double TotalCurvas=(Zmax-Zmin)/IncrementoEntreCurvas;
	double IncrementoRotulos=(rect.right - (iBorderLeft + iBorderRight))/TotalCurvas;
	double PosicionRotulos=iBorderLeft;
	int Curva=-1;
	bool Baja=false;
	long IncrementosColor=2*255/(int)TotalCurvas;
	int R, G, B;
	COLORREF ColorCurvas;

	if (pParamsIsolines->Color == 0){//En color
		R=0; G=255; B=0;
	}
	else{
		if (pParamsIsolines->Color == 1){ //En blanco y negro
			if (ReferApp.m_BackgroundColor == RGB(255,255,255)) //Si el fondo es blanco
				ColorCurvas = RGB( 0, 0, 0 );//Lineas negras
			else //Si el fondo es de otro color, lineas blancas
				ColorCurvas = RGB( 255, 255, 255 );
		}
	}

	//Loop to get isolines
	//--------------------
	for (double Z= Zmin+IncrementoEntreCurvas; Z<Zmax; Z+=IncrementoEntreCurvas){
		Curva++;

		//Asign color according to the z coordinate
		if (pParamsIsolines->Color == 0){ //En color
			if(!Baja){
				R+=IncrementosColor;
				if (R>255){
					R=255;
					Baja=true;
				}
			}
			else
				G-=IncrementosColor;

			ColorCurvas=RGB(R,G,B);
		}

		//CALCULO LOS PUNTOS DE CORTE DEL PLANO Z
		XPuntosCorte.clear();
		YPuntosCorte.clear();

		//Compruebo si la Z del plano se encuentra entre la Z inicial y final de cada
		//recta de la superficie
		for (long FilaX=0; FilaX<(long)Xgrid.size(); FilaX++){
			for(long FilaY=0; FilaY<(long)Ygrid.size()-1; FilaY++){
				double Zi= Fgrid[FilaY+FilaX*(long)Ygrid.size()];
				double Zf= Fgrid[FilaY+1+FilaX*(long)Ygrid.size()];

				if ((Zi<Z && Z<Zf) || (Zi>Z && Z>Zf)){
					double Yi= Ygrid[FilaY];
					double Yf= Ygrid[FilaY+1];
					XPuntosCorte.push_back(Xgrid[FilaX]);
					YPuntosCorte.push_back(((Z-Zi)*(Yf-Yi))/(Zf-Zi)+Yi);
				}
			}
			if(FilaX<(long)Xgrid.size()-1){
				for(long FilaY=0; FilaY<(long)Ygrid.size(); FilaY++){
					double Zi= Fgrid[FilaY+FilaX*(long)Ygrid.size()];
					double Zf= Fgrid[FilaY+(1+FilaX)*(long)Ygrid.size()];

					if ((Zi>Z && Z>Zf) || (Zf>Z && Z>Zi)){
						double Xi= Xgrid[FilaX];
						double Xf= Xgrid[FilaX+1];
						XPuntosCorte.push_back(((Z-Zi)*(Xf-Xi))/(Zf-Zi)+Xi);
						YPuntosCorte.push_back(Ygrid[FilaY]);
					}
				}
			}
		}

		//DETECT ISOLINES
		std::vector<double> CopyXCutPoints;
		std::vector<double> CopyYCutPoints;
		//CopyXCutPoints.clear();
		//CopyYCutPoints.clear();

		//DETECT ISOLINES STEP 1
		DetectIsolines1(pDC, ColorCurvas,
						TransfEjeX, TransfEjeY, InterpolationDegree,
						Z, Zmax,
						CopyXCutPoints, CopyYCutPoints);

		//DETECT ISOLINES STEP 2
		DetectIsolines2(pDC, ColorCurvas,
						TransfEjeX, TransfEjeY, InterpolationDegree,
						Z, Zmax,
						CopyXCutPoints, CopyYCutPoints);

		//DETECT ISOLINES STEP 3
		DetectIsolines3(pDC, ColorCurvas,
						TransfEjeX, TransfEjeY, InterpolationDegree,
						Z, Zmax,
						CopyXCutPoints, CopyYCutPoints);

		//DETECT ISOLINES STEP 4
		DetectIsolines4(pDC, ColorCurvas,
						TransfEjeX, TransfEjeY, InterpolationDegree,
						Z, Zmax,
						CopyXCutPoints, CopyYCutPoints);

		//Ahora miro como unir el resto de puntos si es que quedan
		//Busco primero aquellas curvas que tengan entrada o salida del maping
		//por el margen superior

		//DETECT ISOLINES STEP 5
		DetectIsolines5(pDC, ColorCurvas,
						TransfEjeX, TransfEjeY, InterpolationDegree,
						Z, Zmax, Xmin, Xmax, Ymin, Ymax,
						CopyXCutPoints, CopyYCutPoints);

		//Ahora busco que tengan entrada o salida por el margen inferior

		//DETECT ISOLINES STEP 6
		DetectIsolines6(pDC, ColorCurvas,
						TransfEjeX, TransfEjeY, InterpolationDegree,
						Z, Zmax, Xmin, Xmax, Ymin, Ymax,
						CopyXCutPoints, CopyYCutPoints);

		//DETECT ISOLINES STEP 7
		DetectIsolines7(pDC, ColorCurvas,
						TransfEjeX, TransfEjeY, InterpolationDegree,
						Z, Zmax, Xmin, Xmax, Ymin, Ymax,
						CopyXCutPoints, CopyYCutPoints);

//		if (CopyXCutPoints.size()!=0)
//			AfxMessageBox("Some points remain unmarked");
	}

	pDC->SelectObject(pFuenteAnterior);
	FuenteRotulos.DeleteObject();
}


void CWinRf_ViewOptimization::DrawLegendDesignSubSpace(CDC *pDC, CRect rect,
													   long iBorderRight, long iBorderUp,
													   COLORREF ColorEjeY)
//Draw the legend of the map of isolines
{
	CString sRegularity;
	int iPosLeyendaX, iPosLeyendaY, iActivadas;

	if (m_GraphicStyle.bShowLegend) {
		CFont Fuente;
		Fuente.CreatePointFont( 8*10, "Arial", pDC);
		CFont *pFuenteAnterior = (CFont *) pDC->SelectObject(&Fuente);
		pDC->SetBkMode(TRANSPARENT);

		iPosLeyendaX = rect.right - iBorderRight - 110;
		iPosLeyendaY = iBorderUp+20+10*REGULARITY_LAST;
		iActivadas = 0;
		pDC->SetTextAlign(TA_BASELINE | TA_LEFT);

		for(long lIndice=0; lIndice < REGULARITY_LAST+1; lIndice++){
			if (ReferApp.In.Regularities.ListOfRegularities[lIndice].bActive) {
				pDC->SetTextColor( ColorEjeY );
				iActivadas++;
			}
			else
				pDC->SetTextColor( RGB(200,200,200) );
			switch(lIndice) {
				case REGULARITY_MIN_STD_DEV_ANGLES:
					sRegularity.LoadString( IDSL_REGULARITY_MSDA );
					break;
				case REGULARITY_FACE_PLANARITY:
					sRegularity.LoadString( IDSL_REGULARITY_FACE_PLANARITY );
					break;

				case REGULARITY_LINE_PARALLELISM:
					sRegularity.LoadString( IDSL_REGULARITY_LINE_PARALLELISM );
					break;
				case REGULARITY_LINE_COLLINEARITY:
					sRegularity.LoadString( IDSL_REGULARITY_LINES_COLLINEARITY );
					break;
				case REGULARITY_LINE_VERTICALITY:
					sRegularity.LoadString( IDSL_REGULARITY_LINE_VERTICALITY );
					break;

				case REGULARITY_LINE_ORTHOGONALITY:
					sRegularity.LoadString( IDSL_REGULARITY_LINES_ORTHOGONALITY );
					break;

				case REGULARITY_ISOMETRY:
					sRegularity.LoadString( IDSL_REGULARITY_ISOMETRY );
					break;

				case REGULARITY_CUBIC_CORNERS:
					sRegularity.LoadString( IDSL_REGULARITY_CUBIC_CORNERS );
					break;
				case REGULARITY_FACE_PERPENDICULARITY:
					sRegularity.LoadString( IDSL_REGULARITY_FACES_PERPENDICULARITY );
					break;

				case REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY:
					sRegularity.LoadString( IDSL_REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY );
					break;
				case REGULARITY_SKEWED_FACIAL_SYMMETRY:
					sRegularity.LoadString( IDSL_REGULARITY_OBLIQUE_FACIAL_SYMMETRY );
					break;
			};

			if (pDC->IsPrinting() != 0)
				pDC->SetBkColor( RGB(255, 255, 255) );
			else
				pDC->SetBkColor( ReferApp.m_BackgroundColor );
			pDC->SetBkMode(OPAQUE);
			pDC->TextOut( iPosLeyendaX-40, iPosLeyendaY, sRegularity );
			pDC->SetBkMode(TRANSPARENT);
			iPosLeyendaY -= 10;
		};
		pDC->SetBkMode(TRANSPARENT);
		pDC->SelectObject(pFuenteAnterior);
		Fuente.DeleteObject();
	};

	if (bZoom){
		CPen PlumaZoom;
		COLORREF ColorZoom= RGB(255, 255, 255);

		PlumaZoom.CreatePen( PS_DASH, 1, ColorZoom );
		pDC->SelectObject( &PlumaZoom );

		Polyline( *pDC, Zoom, 5);

		PlumaZoom.DeleteObject();
	}
}


void CWinRf_ViewOptimization::MapSubSpace(CDC * pDC, CString sTitleAxisX, CString sTitleAxisY)
//Draw information relative to the design space

//There are two different modes:
//  - In mode TG==0 the cost is plotted against the value of one of the
//    optimization variables

//  - In mode TG==1 draws the map of isolines of the sub-space of design
//    defined by two of its variables
//    The map is calculated by firstly calculating the values of the cost funcion in all the points of a grid
//    Then the isolines are interpolated
{
	//Parameter initialization
	//------------------------
	CRect rect;
	GetClientRect(&rect);

	COLORREF ColorEjeX, ColorEjeY;
	if (pParamsIsolines->Color==0){//Color
		ColorEjeX = RGB( 200, 200, 200 ) ^ ReferApp.m_BackgroundColor;
		ColorEjeY = RGB( 255, 0, 0 ) ^ ReferApp.m_BackgroundColor;
	}
	else{//Black and white
		if (ReferApp.m_BackgroundColor == RGB(255,255,255)){ //Black over white
			ColorEjeX = RGB( 0, 0, 0 );
			ColorEjeY = RGB( 0, 0, 0 );
		}
		else{//White over black
			ColorEjeX = RGB( 255, 255, 255 );
			ColorEjeY = RGB( 255, 255, 255 );
		}
	}

	//Define the frame to draw the isolines map
	long iBorderRight = 52;
	long iBorderLeft = 60;
	long iBorderUp = 45;
	long iBorderDown = 30;

	double dFactorAmpliacionRangos = 0.1;

	CFont Fuente;
	CFont *pFuenteAnterior;
	Fuente.CreatePointFont( 8*10, "Arial", pDC);
	pFuenteAnterior = (CFont *) pDC->SelectObject(&Fuente);
	pDC->SetBkMode(TRANSPARENT);

	//Get ranges
	//----------
	double dRangeX, dRangeY;
	double Xmin, Xmax, Ymin, Ymax;

	if ((long)Xgrid.size()!=0){
		Xmin= Xgrid[0];
		Xmax= Xgrid[(long)Xgrid.size()-1];
		dRangeX=(Xmax-Xmin);
	}
	else
		dRangeX=0.0;

	if (pParamsIsolines->TG==0){
		if ((long)Fgrid.size()!=0){
			Ymin= Fgrid[0];
			Ymax= Fgrid[0];
			for(long lIndice=0; lIndice<(long)Fgrid.size(); lIndice++){
				Ymin= min(Ymin, Fgrid[lIndice]);
				Ymax= max(Ymax, Fgrid[lIndice]);
			}
			dRangeY=(Ymax-Ymin);
		}
		else
			dRangeY=0;
	}
	else{
		if ((long)Ygrid.size()!=0){
			Ymin= Ygrid[0];
			Ymax= Ygrid[(long)Ygrid.size()-1];
			dRangeY=(Ymax-Ymin);
		}
		else
			dRangeY=0;
	}

	//Draw axis Y
	//-----------
	cGDI_Transform TransfEjeY;
	DrawIsolinesAxisY(pDC,
					  rect,
					  iBorderLeft, iBorderRight,
					  iBorderUp, iBorderDown,
					  Xmin, Ymin, Xmax, Ymax,
					  dRangeX, dRangeY,
					  ColorEjeX, ColorEjeY,
					  dFactorAmpliacionRangos,
					  sTitleAxisY,
					  TransfEjeY);

	//Draw axis X
	//-----------
	cGDI_Transform TransfEjeX;
	DrawIsolinesAxisX(pDC,
					  rect,
					  iBorderLeft, iBorderRight,
					  iBorderUp, iBorderDown,
					  Xmin, Ymin, Xmax, Ymax,
					  dRangeX, dRangeY,
					  ColorEjeX, ColorEjeY,
					  dFactorAmpliacionRangos,
					  sTitleAxisX,
					  TransfEjeX);

	pDC->SelectObject(pFuenteAnterior);
	Fuente.DeleteObject();

	//Draw the plot of cost vs variable
	//---------------------------------
	if (pParamsIsolines->TG==0){
		double dPosition=0;
		CPoint Punto1 = TransfEjeX.GDI_WorldToViewport( dPosition, 0 );
		dPosition+=(double)Xgrid.size()/((double)Xgrid.size()-1);
		CPoint Punto2 = TransfEjeX.GDI_WorldToViewport( dPosition, 0 );
		int NumeroPixels=Punto2.x-Punto1.x;
		int InterpolationDegree = (int)((pParamsIsolines->InterpolationDegree*NumeroPixels) * 0.1);

		if ((long)Fgrid.size()!=0){
			//Si no hay distintos costes
			if (Ymax==Ymin){
				//Dibujo una linea recta de coste
				CPen PlumaCurvas;
				PlumaCurvas.CreatePen( PS_SOLID, 1, ColorEjeY);
				pDC->SelectObject( &PlumaCurvas);

				pDC->MoveTo(iBorderLeft, (rect.bottom - iBorderDown)/2);
				pDC->LineTo(rect.right-iBorderRight, (rect.bottom - iBorderDown)/2);

				PlumaCurvas.DeleteObject();
			}
			//Si existen distintos costes
			else{
				std::vector<double> SplineXgrid;
				std::vector<double> SplineYgrid;
				//Refine the spline if necessary
				if (InterpolationDegree != 0){
					std::vector<double> XPuntosControl;
					std::vector<double> YPuntosControl;
					XPuntosControl= Xgrid;
					YPuntosControl= Fgrid;

					RefineSpline(SplineXgrid, SplineYgrid,
								 XPuntosControl, YPuntosControl,
								 InterpolationDegree);

					TransfEjeX.GDI_SetWorld(0,
											(double)Xmin - dRangeX*dFactorAmpliacionRangos,
											(double)SplineXgrid.size(),
											(double)Xmax + dRangeX*dFactorAmpliacionRangos );
				}
				else{
					SplineXgrid= Xgrid;
					SplineYgrid= Fgrid;
				}

				//Draw the spline
				CPen PlumaCurvas;
				PlumaCurvas.CreatePen( PS_SOLID, 1, ColorEjeY);
				pDC->SelectObject( &PlumaCurvas);

				double Y= SplineYgrid[0];
				dPosition= 0;
				CPoint Punto1 = TransfEjeX.GDI_WorldToViewport( dPosition, 0 );
				CPoint Punto2 = TransfEjeY.GDI_WorldToViewport( 0, Y );
				CPoint PuntoAnterior;
				PuntoAnterior.x=Punto1.x; PuntoAnterior.y=Punto2.y;
	
				for (long lIndice=1; lIndice<(long)SplineYgrid.size(); lIndice++){
					Y= SplineYgrid[lIndice];
					dPosition+=(double)SplineXgrid.size()/((double)SplineXgrid.size()-1);
					Punto1 = TransfEjeX.GDI_WorldToViewport( dPosition, 0 );
					Punto2 = TransfEjeY.GDI_WorldToViewport( 0, Y );
					CPoint Punto;
					Punto.x=Punto1.x; Punto.y=Punto2.y;
					pDC->MoveTo(PuntoAnterior.x, PuntoAnterior.y);
					pDC->LineTo(Punto.x, Punto.y);
					PuntoAnterior=Punto;
				}
				TransfEjeX.GDI_SetWorld(0,
										(double)Xmin - dRangeX*dFactorAmpliacionRangos,
										(double)Xgrid.size(),
										(double)Xmax + dRangeX*dFactorAmpliacionRangos );

				PlumaCurvas.DeleteObject();
			}
		}
	}

	//Draw the map of isolines
	//------------------------
	else{
		double dPosition=0;
		CPoint Punto1 = TransfEjeX.GDI_WorldToViewport( dPosition, 0 );
		dPosition+=(double)Xgrid.size()/((double)Xgrid.size()-1);
		CPoint Punto2 = TransfEjeX.GDI_WorldToViewport( dPosition, 0 );
		int NumeroPixels=Punto2.x-Punto1.x;
		int InterpolationDegree = (int)((pParamsIsolines->InterpolationDegree*NumeroPixels) * 0.1);

		//Two variables map
		if((long)Fgrid.size()!=0){
			//Calculate the range of Z
			double Zmin= Fgrid[0];
			double Zmax= Fgrid[0];
			for(long lIndice=0; lIndice<(long)Fgrid.size(); lIndice++){
				Zmin= min(Zmin, Fgrid[lIndice]);
				Zmax= max(Zmax, Fgrid[lIndice]);
			}

			//Si no existen valores distintos de costo a representar
			//represento un rectangulo de dimensiones máximas
			if (Zmin==Zmax){
				//Si se pide dibujar rotulos anoto el valor del coste en el centro del
				//rectangulo
				if (m_GraphicStyle.bShowLabels){
					CFont FuenteRotulos;
					FuenteRotulos.CreatePointFont( 7*10, "Arial", pDC);
					pFuenteAnterior = (CFont *) pDC->SelectObject(&FuenteRotulos);
					pDC->SetBkMode(TRANSPARENT);

					pDC->SetTextColor( ColorEjeY );
					pDC->SetTextAlign(TA_TOP | TA_CENTER);
					CString sTexto;
					sTexto.Format("%4.2f", (float)Zmin);
					pDC->TextOut(iBorderLeft+(rect.right-iBorderRight-iBorderLeft)/2,
								 iBorderUp+(rect.bottom-iBorderDown-iBorderUp)/2,
								 sTexto);
				
					pDC->SelectObject(pFuenteAnterior);
					FuenteRotulos.DeleteObject();
				}
			}
			else{
				DrawIsoLines(pDC, rect,
							 InterpolationDegree,
							 iBorderLeft, iBorderRight,
							 iBorderUp, iBorderDown,
							 Xmin, Xmax, Ymin, Ymax, Zmin, Zmax,
							 TransfEjeX, TransfEjeY);
			}
		}
	}

	//Draw the legend
	//---------------
	DrawLegendDesignSubSpace(pDC, rect, iBorderRight, iBorderUp, ColorEjeY);
}


void CWinRf_ViewOptimization::DrawSubSpace(CDC* pDC)
//Uses the Optimization window to draw information relative to the design space

//There are two different modes:
//  - In mode TG==0 the cost is plotted against the value of one of the
//    optimization variables

//  - In mode TG==1 the isolines for different values of the cost function
//    are represented to display the map of the sub-space of design
//    defined by two of its optimization variables
{
	//Titles
	CString sTitleAxisX, sTitleAxisY;
	SubSpaceTitles(pDC, sTitleAxisX, sTitleAxisY);

	//Determine a grid into a range of a sub-space of design
	//and recalculate cost function for the points of the grid
	RecalculateGrid();

	//Plot or map the cost function against one or two optimization variables
	//to display a sub-space of the design space
	MapSubSpace(pDC, sTitleAxisX, sTitleAxisY);
}


void CWinRf_ViewOptimization::PlotCost(CDC *pDC, std::vector<double> Data,
									   CDataInflation ParametrosOpt,
									   CString sTitleAxisYMain,
									   CString sTitleAxisYAux,
									   CString sTitleAxisX)
// The data in 'Data' has the following format: 
// it is a consecutive list of sets data 
// (Temperatura, CosteTotal, Coste1, Coste2, ... CosteN)
// Example:   Data=[9,1,8,2,7,3]    TamanyoListaDouble(&Data)=6
//            is interpreted as:
//              Iteration 0 -> Temperature:9, Cost:1
//              Iteration 1 -> Temperature:8, Cost:2
//              Iteration 2 -> Temperature:7, Cost:3
{
	if (!(long)Data.size()) return;

	long iNumPlots = 2 + ParametrosOpt.Regularities.NumActiveRegularities;
		// There will always be at least two graphs: Total Cost and StepLength or Temperature
		// There may be more graphs, which represent the Costs of the regularities,
		// that form the Total Cost

	// Check what is the range of the data to represent
	long lNumeroMuestras = (long)Data.size() / iNumPlots;
	double dTemperaturaMayor = Data[0];
	double dTemperaturaMenor = Data[0];
	double dCosteMayor = Data[1];
	double dCosteMenor = Data[1];

	// Search maximum and minimum Temperature and Cost
	for(long i= iNumPlots ; i <(long)Data.size(); i += iNumPlots) {
		dTemperaturaMayor = max( dTemperaturaMayor, Data[i] );
		dTemperaturaMenor = min( dTemperaturaMenor, Data[i] );
		dCosteMayor = max( dCosteMayor, Data[i+1] );
		dCosteMenor = min( dCosteMenor, Data[i+1] );
	};

	if (ParametrosOpt.bViewPartialCostsEvolution)
		dCosteMenor = 0;

	bool bHayDatosTemperatura;
	if (dTemperaturaMayor == dTemperaturaMenor)
		bHayDatosTemperatura = false;
	else
		bHayDatosTemperatura = true;

	bool bHayDatosCoste;
	if (dCosteMayor == dCosteMenor)
		bHayDatosCoste = false;
	else
		bHayDatosCoste = true;

	// Prepares the Transformation to the Device, 
	// where the Device will be the window where we want to draw.
	// The Real Window will be the range of the data.
	cGDI_Transform TransfTemperatura, TransfCoste;
	CRect rect;
	GetClientRect( &rect );

	double dFactorAmpliacionRangos = 0.1;
	double dRangeYTemperatura = (dTemperaturaMayor - dTemperaturaMenor) * (1 + dFactorAmpliacionRangos*2);
	double dRangeYCoste = (dCosteMayor - dCosteMenor) * (1 + dFactorAmpliacionRangos*1);

	//Adjust these parameters to modify the margins between the function and the window
	int iBorderRight = 52;
	int iBorderLeft = 60;
	int iBorderUp = 45;
	int iBorderDown = 30;

	TransfTemperatura.GDI_SetViewport(iBorderLeft, iBorderUp,
									  rect.right-iBorderRight, rect.bottom-iBorderDown );
	TransfTemperatura.GDI_SetWorld( 0,
								   (double)dTemperaturaMenor - dRangeYTemperatura*dFactorAmpliacionRangos,
								   (double)lNumeroMuestras,
								   (double)dTemperaturaMayor + dRangeYTemperatura*dFactorAmpliacionRangos );

	TransfCoste.GDI_SetViewport(iBorderLeft, iBorderUp,
								rect.right-iBorderRight, rect.bottom-iBorderDown ); 
	TransfCoste.GDI_SetWorld( 0,
							 (double)dCosteMenor,
							 (double)lNumeroMuestras,
							 (double)dCosteMayor + dRangeYCoste*dFactorAmpliacionRangos );

	// The area below the graph is filled with consecutive lines or bars
	bool bDrawBars;
	if ( lNumeroMuestras < (rect.right-iBorderRight-iBorderLeft) )
		bDrawBars = true;    //If there are few points, the graph area is filled with bars
	else
		bDrawBars = false;   //If there are many points, the graph area is filled with lines

	// Prepare the Pens and Fonts to draw the graph
	CPen PlumaTemperatura, PlumaCoste, PlumaIteraciones, PlumaComponentes, *pPlumaAnterior;
	CFont Fuente, *pFuenteAnterior;

	CPoint PuntoTemperaturaAnterior, PuntoCosteAnterior;
	CString sTexto;
	COLORREF ColorTemperatura, ColorCoste, ColorIteraciones, ColorComponente;

	if (pDC->IsPrinting() != 0){
		ColorTemperatura = RGB (0,0,0);
		ColorCoste = RGB(0,0,0);
		ColorIteraciones = RGB(0,0,0);
	}
	else{
		if (ReferApp.m_BackgroundColor==16777215){
			ColorIteraciones = RGB(0,48,180);
			ColorTemperatura = RGB(210,32,0);
			ColorCoste = RGB(37,150,0);
		}
		else{
			ColorIteraciones = RGB( 200, 200, 200 ) ^ ReferApp.m_BackgroundColor;
			ColorTemperatura = RGB( 0, 200, 0 ) ^ ReferApp.m_BackgroundColor;
			ColorCoste = RGB( 255, 0, 0 ) ^ ReferApp.m_BackgroundColor;
		}
	}

	Fuente.CreatePointFont( 8*10, "Arial", pDC);
	pFuenteAnterior = (CFont *) pDC->SelectObject(&Fuente);
	pDC->SetBkMode(TRANSPARENT);

	PlumaTemperatura.CreatePen( PS_SOLID, 1, ColorTemperatura );
	PlumaCoste.CreatePen( PS_SOLID, 1, ColorCoste );
	PlumaIteraciones.CreatePen( PS_SOLID, 1, ColorIteraciones );

	// DRAWING OF DATA CURVES BEGINS
	// (Consecutively draws the stretch of the plot that corresponds to each iteration)
	
	// Take the first iteration and locate the initial data points
	PuntoTemperaturaAnterior = TransfTemperatura.GDI_WorldToViewport( (double)0, Data[0] );
	PuntoCosteAnterior = TransfCoste.GDI_WorldToViewport( (double)0, Data[1] );

	// Take the first iteration and locate the initial data points
	for(long lIndice= iNumPlots; lIndice < (long)Data.size(); lIndice += iNumPlots ) {
		// Draw cost areas of the regularities
		if (ParametrosOpt.bViewPartialCostsEvolution && bHayDatosCoste){
			CPoint Punto = TransfCoste.GDI_WorldToViewport( (double)(lIndice/iNumPlots), 0 );
			pDC->MoveTo( Punto.x, Punto.y );

			double dSumaComponentes = 0;

			for(long i=2; i<iNumPlots; i++ ){
				ColorComponente = RGB(256-i*256/iNumPlots, 50, i*256/iNumPlots);
				PlumaComponentes.CreatePen( PS_SOLID, 1, ColorComponente );
				pPlumaAnterior = pDC->SelectObject( &PlumaComponentes );

				int iPuntoAnteriorY;
				if (bDrawBars)
					iPuntoAnteriorY = Punto.y;
				dSumaComponentes += Data[lIndice+i];
				Punto = TransfCoste.GDI_WorldToViewport( (double)(lIndice/iNumPlots), dSumaComponentes );
				pDC->LineTo( Punto.x, Punto.y );
				if (bDrawBars)
					pDC->FillSolidRect( CRect(Punto.x, Punto.y, PuntoCosteAnterior.x, iPuntoAnteriorY), ColorComponente );

				pDC->SelectObject(pPlumaAnterior);
				PlumaComponentes.DeleteObject();
			};
		};

		// Draw cost line
		if (bHayDatosCoste) {
			CPoint Punto = TransfCoste.GDI_WorldToViewport( (double)(lIndice/iNumPlots), Data[lIndice+1] );
			pDC->SelectObject( &PlumaCoste );
			pDC->MoveTo( PuntoCosteAnterior.x, PuntoCosteAnterior.y );
			pDC->LineTo( Punto.x, Punto.y );
			PuntoCosteAnterior = Punto;
		};

		// Draw temperature line
		if (!bHayDatosTemperatura){
			CPoint Punto;
			pDC->SelectObject( &PlumaTemperatura );
			Punto.x = rect.left+iBorderLeft;
			Punto.y = iBorderUp+8;
			pDC->MoveTo( Punto.x, Punto.y );
			Punto.x = rect.right - iBorderRight;
			Punto.y = iBorderUp+8;
			pDC->LineTo( Punto.x, Punto.y );
		}
		else{
			CPoint Punto;
			Punto = TransfTemperatura.GDI_WorldToViewport( (double)(lIndice/iNumPlots), Data[lIndice] );
			pDC->SelectObject( &PlumaTemperatura );
			pDC->MoveTo( PuntoTemperaturaAnterior.x, PuntoTemperaturaAnterior.y );
			pDC->LineTo( Punto.x, Punto.y );
			PuntoTemperaturaAnterior = Punto;
		}
	}

	pPlumaAnterior = pDC->SelectObject( &PlumaCoste );

	// DRAWS THE LEGEND WITH THE FINAL COST OF EACH REGULARITY
	CString sRegularity;
	if (ParametrosOpt.bViewPartialCostsEvolution && m_GraphicStyle.bShowLegend) {
		int iPosLeyendaX = rect.right - iBorderRight - (170);    // Increasing the adjust parameter 170 moves te legend to the left
		int iPosLeyendaY = iBorderUp+20+(15)*REGULARITY_LAST;    // Increasing the adjust parameter 15 moves te legend down
		int iActivadas = 0;
		// We take a central data tuple, to draw a line from the legend
		long lIndice = ((long)Data.size() / iNumPlots) / 2;
		double dSumaCostes = 0;
		pDC->SetTextAlign(TA_BASELINE | TA_LEFT);

		int ContadorComponentes = 0;
		for(long i=0; i < REGULARITY_LAST+1; i++){
			if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive) {

				ColorComponente = RGB(256-(ContadorComponentes+2)*256/iNumPlots, 50, (ContadorComponentes+2)*256/iNumPlots);

				// Draw a line from the legend to a corresponding data
				if (bHayDatosCoste) {
					dSumaCostes += Data[(lIndice*iNumPlots)+2+iActivadas];
					CPoint Punto = TransfCoste.GDI_WorldToViewport( (double)lIndice, dSumaCostes );
					PlumaComponentes.CreatePen( PS_SOLID, 1, ColorComponente );
					pPlumaAnterior = pDC->SelectObject( &PlumaComponentes );
					pDC->MoveTo( Punto.x, Punto.y );
					pDC->LineTo( iPosLeyendaX+1-45, iPosLeyendaY-2 );
					pDC->SelectObject(pPlumaAnterior);
					PlumaComponentes.DeleteObject();
				};
				pDC->SetTextColor( ColorComponente );
				iActivadas++;
			}
			else
				pDC->SetTextColor( RGB(50,50,50) );

			switch(i) {
				case REGULARITY_MIN_STD_DEV_ANGLES:
					sRegularity.LoadString( IDSL_REGULARITY_MSDA );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-iNumPlots+2+ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%f", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else
						sRegularity = sRegularity + "  0.0" ;
					break;

				case REGULARITY_FACE_PLANARITY:
					sRegularity.LoadString( IDSL_REGULARITY_FACE_PLANARITY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-iNumPlots+2+ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%f", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else
						sRegularity = sRegularity + "  0.0" ;
					break;


				case REGULARITY_LINE_PARALLELISM:
					sRegularity.LoadString( IDSL_REGULARITY_LINE_PARALLELISM );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-iNumPlots+2+ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%f", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else
						sRegularity = sRegularity + "  0.0" ;
					break;

				case REGULARITY_LINE_COLLINEARITY:
					sRegularity.LoadString( IDSL_REGULARITY_LINES_COLLINEARITY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-iNumPlots+2+ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%f", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else
						sRegularity = sRegularity + "  0.0" ;
					break;

				case REGULARITY_LINE_VERTICALITY:
					sRegularity.LoadString( IDSL_REGULARITY_LINE_VERTICALITY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-iNumPlots+2+ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%f", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else
						sRegularity = sRegularity + "  0.0" ;
					break;


				case REGULARITY_LINE_ORTHOGONALITY:
					sRegularity.LoadString( IDSL_REGULARITY_LINES_ORTHOGONALITY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-iNumPlots+2+ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%f", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else
						sRegularity = sRegularity + "  0.0" ;
					break;


				case REGULARITY_ISOMETRY:
					sRegularity.LoadString( IDSL_REGULARITY_ISOMETRY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-iNumPlots+2+ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%f", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else
						sRegularity = sRegularity + "  0.0" ;
					break;


				case REGULARITY_CUBIC_CORNERS:
					sRegularity.LoadString( IDSL_REGULARITY_CUBIC_CORNERS );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-iNumPlots+2+ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%f", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else
						sRegularity = sRegularity + "  0.0" ;
					break;

				case REGULARITY_FACE_PERPENDICULARITY:
					sRegularity.LoadString( IDSL_REGULARITY_FACES_PERPENDICULARITY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-iNumPlots+2+ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%f", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else
						sRegularity = sRegularity + "  0.0" ;
					break;


				case REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY:
					sRegularity.LoadString( IDSL_REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-iNumPlots+2+ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%f", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else
						sRegularity = sRegularity + "  0.0" ;
					break;

				case REGULARITY_SKEWED_FACIAL_SYMMETRY:
					sRegularity.LoadString( IDSL_REGULARITY_OBLIQUE_FACIAL_SYMMETRY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-iNumPlots+2+ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%f", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else sRegularity = sRegularity + "  0.0" ;
					break;
			};

			if (pDC->IsPrinting() != 0)
				pDC->SetBkColor( RGB(255, 255, 255) );
			else
				pDC->SetBkColor( ReferApp.m_BackgroundColor );
			pDC->SetBkMode(OPAQUE);
			pDC->TextOut( iPosLeyendaX-40, iPosLeyendaY, sRegularity );
			pDC->SetBkMode(TRANSPARENT);
			iPosLeyendaY -= 15;  // Increasing this adjust parameter 15 increases the interspacing of lines of the legend
		};
		pDC->SetBkMode(TRANSPARENT);
	};

	// DRAWS THE AXES
	long SpacingBetweenYLabels= 40;
	long NumDivisionsY= max((rect.bottom - (iBorderUp + iBorderDown)) / SpacingBetweenYLabels, 1);

	// Draw the primary Y Axis: Cost (the one on the left)
	pDC->MoveTo(iBorderLeft, iBorderUp);
	pDC->LineTo(iBorderLeft, rect.bottom - iBorderDown);
	pDC->SetTextColor( ColorCoste );
	pDC->SetTextAlign(TA_BOTTOM | TA_CENTER);
	pDC->TextOut( iBorderLeft, iBorderUp, sTitleAxisYMain );
	pDC->SetTextAlign(TA_BOTTOM | TA_RIGHT);

	// Draw division lines and value labels on the primary Y Axis
	if (bHayDatosCoste) {
		double dCoste = dCosteMenor;
		for (long lIndice=0 ; lIndice <= NumDivisionsY ; lIndice++) {
			CPoint Punto = TransfCoste.GDI_WorldToViewport( 0.0, dCoste );
			pDC->MoveTo( Punto.x-2, Punto.y );
			pDC->LineTo( Punto.x+2, Punto.y );
			ASSERT( !_isnan(dCoste) );
			sTexto.Format("%4.3f", (float)dCoste);
			pDC->TextOut( Punto.x-3, Punto.y+6, sTexto );
			dCoste += (dCosteMayor - dCosteMenor)/ NumDivisionsY;
		};
	};

	// Draw the secondary Y Axis: Variables (HC) or Temperature (SA) (the one on the right)
	pDC->SelectObject( &PlumaTemperatura );
	pDC->MoveTo(rect.right - iBorderRight, iBorderUp);
	pDC->LineTo(rect.right - iBorderRight, rect.bottom - iBorderDown);
	pDC->SetTextColor( ColorTemperatura );
	pDC->SetTextAlign(TA_BOTTOM | TA_CENTER);
	pDC->TextOut( rect.right - iBorderRight, iBorderUp, sTitleAxisYAux );
	pDC->SetTextAlign(TA_BOTTOM | TA_LEFT);

	// Draw the secondary Y Axis: Variables (HC) or Temperature (SA) (the one on the right)
	
	// If only one temperature value has to be represented
	if (!bHayDatosTemperatura){
		CPoint Punto;
		Punto.x = rect.right - iBorderRight;
		Punto.y = iBorderUp;
		pDC->MoveTo( Punto.x-2, Punto.y+8 );
		pDC->LineTo( Punto.x+2, Punto.y+8 );
		sTexto.Format("%4.3f", (float)Data[0]);
		pDC->TextOut( Punto.x+3, Punto.y+6+8, sTexto );
	}
	else{
		if (ReferApp.In.iOptAlgorithm == HILL_CLIMBING){
			std::vector<double> StepLengths;
			for(long lIndex=0; lIndex<(long)Data.size(); lIndex += iNumPlots){
				StepLengths.push_back(Data[lIndex]);
			}
			double CoordenadaY = -1;
			for (long lIndice=(long)StepLengths.size()-1; lIndice>0; lIndice--) {
				double step= StepLengths[lIndice];
				CPoint Punto = TransfTemperatura.GDI_WorldToViewport( (double)lNumeroMuestras, step );
				if (CoordenadaY == -1){
					CoordenadaY=Punto.y;
					pDC->MoveTo( Punto.x-2, Punto.y );
					pDC->LineTo( Punto.x+2, Punto.y );
					sTexto.Format("%4.3f", (float)step);
					pDC->TextOut( Punto.x+3, Punto.y+6, sTexto );
				}
				else{
					if (CoordenadaY-Punto.y>10){
						CoordenadaY=Punto.y;
						pDC->MoveTo( Punto.x-2, Punto.y );
						pDC->LineTo( Punto.x+2, Punto.y );
						sTexto.Format("%4.3f", (float)step);
						pDC->TextOut( Punto.x+3, Punto.y+6, sTexto );
					}
				}
			}
		}
		else{
			double dTemperatura = dTemperaturaMenor;
			for (long lIndice=0 ; lIndice <= NumDivisionsY ; lIndice++) {
				CPoint Punto = TransfTemperatura.GDI_WorldToViewport( (double)lNumeroMuestras, dTemperatura );
				pDC->MoveTo( Punto.x-2, Punto.y );
				pDC->LineTo( Punto.x+2, Punto.y );
				sTexto.Format("%4.3f", (float)dTemperatura);
				pDC->TextOut( Punto.x+3, Punto.y+6, sTexto );
				dTemperatura += (dTemperaturaMayor - dTemperaturaMenor)/ NumDivisionsY;
			};
		}
	}

	// Draw the X Axis: Iterations
	pDC->SelectObject( &PlumaIteraciones );
	pDC->MoveTo(iBorderLeft, rect.bottom - iBorderDown);
	pDC->LineTo(rect.right-iBorderRight, rect.bottom - iBorderDown);
	pDC->SetTextColor( ColorIteraciones );
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	pDC->TextOut( rect.right/2, rect.bottom - iBorderDown/2, sTitleAxisX );

	// Draw division lines and value labels on the X Axis
	long SpacingBetweenXLabels= 40;
	long NumDivisionsX = max(min(lNumeroMuestras, ((rect.right - (iBorderLeft + iBorderRight)) / SpacingBetweenXLabels)), 1);
	double dIteraciones = 0;
	double dPosicion=0;
	for (long lIndice=0 ; lIndice <= NumDivisionsX ; lIndice++) {
		CPoint Punto = TransfTemperatura.GDI_WorldToViewport( dPosicion, dTemperaturaMayor );
		Punto.y = rect.bottom-iBorderDown;
		pDC->MoveTo( Punto.x, Punto.y-2 );
		pDC->LineTo( Punto.x, Punto.y+2 );
		sTexto.Format("%i", (int)dIteraciones);
		pDC->TextOut( Punto.x, Punto.y+3, sTexto );
		dPosicion += ((double)lNumeroMuestras / (double)NumDivisionsX)+ 0.0000001;
		dIteraciones += ((double)lNumeroMuestras / (double)NumDivisionsX);
//						*ReferApp.In.NumVariables+ 0.0000001;
	};

	// Pens and Fonts are destroyed to free resources
	pDC->SelectObject( pPlumaAnterior );
	PlumaTemperatura.DeleteObject();
	PlumaCoste.DeleteObject();
	PlumaIteraciones.DeleteObject();
	pDC->SelectObject(pFuenteAnterior);
	Fuente.DeleteObject();
}


void CWinRf_ViewOptimization::PlotCostsMulti(CDC *pDC, std::vector<double> Data,
											 CDataInflation ParametrosOpt,
											 CString sTitleAxisYMain,
											 CString sTitleAxisYAux,
											 CString sTitleAxisX)
// Dibuja el grafico de los costes de un SIMULATED ANNEALING MULTICRITERIO
// Aqui no dibujaremos la TEMPERATURA, porque no hay una sino varias.
//
// Los datos de 'Data' tienen el siguiente formato: son una lista consecutiva de conjuntos
// de datos (Temperatura, CosteTotal, Coste1, Coste2, ... CosteN)
// Ej:   pData=[9,1,8,2,7,3]    TamanyoListaDouble(&Data)=6
//       se interpreta como:
//                           Iteracion 0  ->  Temperatura:9 , Coste:1
//                           Iteracion 1  ->  Temperatura:8 , Coste:2
//                           Iteracion 2  ->  Temperatura:7 , Coste:3
{
	if (!(long)Data.size()) return;

	long iNumPlots = 2+ ParametrosOpt.Regularities.NumActiveRegularities;
		// There will always be at least two graphs: Total Cost and StepLength or Temperature
		// There may be more graphs, which represent the Costs of the regularities,
		// that form the Total Cost

	long lNumeroMuestras = (long)Data.size() / iNumPlots;

	//Prepare pens and fonts
	COLORREF ColorIteraciones;
	if (pDC->IsPrinting() != 0)
		ColorIteraciones = RGB( 0, 0, 0);
	else
		ColorIteraciones = RGB( 200, 200, 200 ) ^ ReferApp.m_BackgroundColor;
	CPen PlumaSubCoste, PlumaIteraciones, *pPlumaAnterior;
	CFont Fuente, *pFuenteAnterior;
	PlumaIteraciones.CreatePen( PS_SOLID, 1, ColorIteraciones );
	Fuente.CreatePointFont( 8*10, "Arial", pDC);
	pFuenteAnterior = (CFont *) pDC->SelectObject(&Fuente);
	pDC->SetBkMode(TRANSPARENT);

	//Determine the frame
	double dFactorAmpliacionRangos = 0.1;
//	long iBorderRight = 52;
//	long iBorderLeft = 60;
	long iBorderRight = 30;
	long iBorderLeft = 30;
	long iBorderUp = 45;
	long iBorderDown = 30;

	CRect rect;
	GetClientRect( &rect );

	cGDI_Transform TransfSubCoste;
	TransfSubCoste.GDI_SetViewport( iBorderLeft, iBorderUp,
									rect.right-iBorderRight, rect.bottom-iBorderDown ); 

	//DRAW THE AXES
	pPlumaAnterior = pDC->SelectObject( &PlumaSubCoste );

	//long SpacingBetweenYLabels= 40;
	//long lNumeroDivisiones= max((rect.bottom - (iBorderUp + iBorderDown)) / SpacingBetweenYLabels, 1);

	// Dibuja el Eje Y primario : Coste (el de la izquierda)
	pDC->SelectObject( &PlumaIteraciones );
	pDC->MoveTo(iBorderLeft, iBorderUp);
	pDC->LineTo(iBorderLeft, rect.bottom - iBorderDown);
	pDC->SetTextColor( ColorIteraciones );
	pDC->SetTextAlign(TA_BOTTOM | TA_CENTER);
	pDC->TextOut( iBorderLeft, iBorderUp, sTitleAxisYMain );
	pDC->SetTextAlign(TA_BOTTOM | TA_RIGHT);

	// Dibuja el Eje X : Iteraciones
	pDC->SelectObject( &PlumaIteraciones );
	pDC->MoveTo(iBorderLeft, rect.bottom - iBorderDown);
	pDC->LineTo(rect.right-iBorderRight, rect.bottom - iBorderDown);
	pDC->SetTextColor( ColorIteraciones );
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	pDC->TextOut( rect.right/2, rect.bottom - iBorderDown/2, sTitleAxisX );

	// Dibuja lineas de division y etiquetas de valores en el Eje X
	long SpacingBetweenXLabels= 40;
	long lNumeroDivisionesX = max(min(lNumeroMuestras, ((rect.right - (iBorderLeft + iBorderRight)) / SpacingBetweenXLabels)), 1);
	
	// Esta transformacion es ficticia; es un truco para obtener el rango X adecuado.
	// Luego se actualizara adecuadamente.
	TransfSubCoste.GDI_SetWorld( 0.0, 0.0, (double)lNumeroMuestras, 1.0 );

	double dPosicion = 0;
	double dIteraciones = 0;
	CString sTexto;
	for (long i=0 ; i <= lNumeroDivisionesX ; i++) {
		CPoint Punto= TransfSubCoste.GDI_WorldToViewport( dPosicion, 0 );
		Punto.y = rect.bottom-iBorderDown;
		pDC->MoveTo( Punto.x, Punto.y-2 );
		pDC->LineTo( Punto.x, Punto.y+2 );
		sTexto.Format("%i", (int)dIteraciones);
		pDC->TextOut( Punto.x, Punto.y+3, sTexto );
		dPosicion += ((double)lNumeroMuestras / (double)lNumeroDivisionesX) + 0.0000001;
		dIteraciones += ((double)lNumeroMuestras / (double)lNumeroDivisionesX) *
						ReferApp.In.NumVariables+ 0.0000001;
	};

	// START DRAWING EACH OF THE SUBCOSTS
	long iNumeroSubCostes;
	if(ParametrosOpt.bViewPartialCostsEvolution)
		iNumeroSubCostes = iNumPlots - 2;
	else
		iNumeroSubCostes = 0;

	for(long iSubCoste=0 ; iSubCoste < iNumeroSubCostes ; iSubCoste++ ) {

		// Check what is the range of each subcost to represent.
		double dDato= Data[iSubCoste+2];
		ASSERT( (dDato<1000000) && (dDato>-1000000));
		double dSubCosteMayor = dDato;
		double dSubCosteMenor = dDato;

		// Find each major and minor SubCost
		for(long i= iNumPlots+2 ; i <(long)Data.size(); i += iNumPlots) {
			dSubCosteMayor = max( dSubCosteMayor, Data[i+iSubCoste] );
			dSubCosteMenor = min( dSubCosteMenor, Data[i+iSubCoste] );
		};

		// Reset the sub-cost flag
		bool bHayDatosSubCoste;
		if ( dSubCosteMayor == dSubCosteMenor )
			bHayDatosSubCoste = false;
		else
			bHayDatosSubCoste = true;

		// prepares the Transformation to the Device, 
		// where the Device will be the window where we want to draw. 
		// The Real Window will be the range of the data.
		double dRangeYCoste = (dSubCosteMayor - dSubCosteMenor) * (1.0 + dFactorAmpliacionRangos);
		TransfSubCoste.GDI_SetWorld( 0.0, dSubCosteMenor, (double)lNumeroMuestras,
											 (double)dSubCosteMayor + dRangeYCoste*dFactorAmpliacionRangos );

		// Prepare the pen to draw
		COLORREF ColorSubCoste = RGB(256-(iSubCoste+2)*256/iNumPlots, 50, (iSubCoste+2)*256/iNumPlots);
		PlumaSubCoste.CreatePen( PS_SOLID, 1, ColorSubCoste );
		pPlumaAnterior = pDC->SelectObject( &PlumaSubCoste );

		// DRAWING OF DATA CURVES BEGINS
		if (ParametrosOpt.bViewPartialCostsEvolution && bHayDatosSubCoste) {

			// Take out the first row of data and place the starting point of the data
			dDato= Data[iSubCoste+2];
			CPoint Punto = TransfSubCoste.GDI_WorldToViewport( 0.0, dDato );
			pDC->MoveTo( Punto.x, Punto.y );

			// Takes the current SubCost of the rest of the tuples and represents them in the graph
			for(long lIndice = (iSubCoste+2+iNumPlots);
				lIndice < (long)Data.size(); lIndice += iNumPlots ) {

				dDato= Data[lIndice];
				Punto = TransfSubCoste.GDI_WorldToViewport( (double)((double)lIndice/(double)iNumPlots), dDato );
				pDC->LineTo( Punto.x, Punto.y );

				// Draw maximum and minimum value labels
				if (dDato == dSubCosteMayor){
					sTexto.Format("%f", dDato);
					pDC->SetTextColor( ColorSubCoste );
					pDC->SetTextAlign(TA_BOTTOM | TA_CENTER);
					pDC->SetBkMode(OPAQUE);
					pDC->TextOut( Punto.x, Punto.y, sTexto );
					pDC->SetBkMode(TRANSPARENT);
				};
				if (dDato == dSubCosteMenor){
					sTexto.Format("%f", dDato);
					pDC->SetTextColor( ColorSubCoste );
					pDC->SetTextAlign(TA_TOP | TA_CENTER);
					pDC->SetBkMode(OPAQUE);
					pDC->TextOut( Punto.x, Punto.y, sTexto );
					pDC->SetBkMode(TRANSPARENT);
				};
			};
		};

		pDC->SelectObject( pPlumaAnterior );
		PlumaSubCoste.DeleteObject();
	};

	pPlumaAnterior = pDC->SelectObject( &PlumaSubCoste );

	// DRAW THE LEGEND IF APPROPRIATE
	CString sRegularity;
	int iPosLeyendaX, iPosLeyendaY, iActivadas;
	if (ParametrosOpt.bViewPartialCostsEvolution && m_GraphicStyle.bShowLegend) {
		iPosLeyendaX = rect.right - iBorderRight - 80-100;
		iPosLeyendaY = iBorderUp+20+10*REGULARITY_LAST;
		iActivadas = 0;
		// We take a central data tuple, to draw a line from the legend

		pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
		int ContadorComponentes = 0;
		for(long i=0; i < REGULARITY_LAST+1; i++){
			if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive) {
				pDC->SetTextColor(RGB(256-(iActivadas+2)*256/iNumPlots, 50, (iActivadas+2)*256/iNumPlots));
				iActivadas++;
			}
			else
				pDC->SetTextColor( RGB(50,50,50) );
			switch(i) {
				case REGULARITY_MIN_STD_DEV_ANGLES:
					sRegularity.LoadString( IDSL_REGULARITY_MSDA );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%d", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else sRegularity = sRegularity + "  0.0" ;
					break;

				case REGULARITY_FACE_PLANARITY:
					sRegularity.LoadString( IDSL_REGULARITY_FACE_PLANARITY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%d", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else sRegularity = sRegularity + "  0.0" ;
					break;


				case REGULARITY_LINE_PARALLELISM:
					sRegularity.LoadString( IDSL_REGULARITY_LINE_PARALLELISM );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%d", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else sRegularity = sRegularity + "  0.0" ;
					break;

				case REGULARITY_LINE_COLLINEARITY:
					sRegularity.LoadString( IDSL_REGULARITY_LINES_COLLINEARITY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%d", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else sRegularity = sRegularity + "  0.0" ;
					break;

				case REGULARITY_LINE_VERTICALITY:
					sRegularity.LoadString( IDSL_REGULARITY_LINE_VERTICALITY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%d", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else sRegularity = sRegularity + "  0.0" ;
					break;


				case REGULARITY_LINE_ORTHOGONALITY:
					sRegularity.LoadString( IDSL_REGULARITY_LINES_ORTHOGONALITY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%d", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else sRegularity = sRegularity + "  0.0" ;
					break;


				case REGULARITY_ISOMETRY:
					sRegularity.LoadString( IDSL_REGULARITY_ISOMETRY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%d", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else sRegularity = sRegularity + "  0.0" ;
					break;


				case REGULARITY_CUBIC_CORNERS:
					sRegularity.LoadString( IDSL_REGULARITY_CUBIC_CORNERS );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%d", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else sRegularity = sRegularity + "  0.0" ;
					break;

				case REGULARITY_FACE_PERPENDICULARITY:
					sRegularity.LoadString( IDSL_REGULARITY_FACES_PERPENDICULARITY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%d", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else sRegularity = sRegularity + "  0.0" ;
					break;


				case REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY:
					sRegularity.LoadString( IDSL_REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%d", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else sRegularity = sRegularity + "  0.0" ;
					break;

				case REGULARITY_SKEWED_FACIAL_SYMMETRY:
					sRegularity.LoadString( IDSL_REGULARITY_OBLIQUE_FACIAL_SYMMETRY );
					if (ParametrosOpt.Regularities.ListOfRegularities[i].bActive){
						double Coste= Data[(long)Data.size()-ContadorComponentes];
						ContadorComponentes++;
						CString toString;    toString.Format("%d", Coste);
						sRegularity =  sRegularity + "   " + toString;
					}
					else sRegularity = sRegularity + "  0.0" ;
					break;
			};
			if (pDC->IsPrinting() != 0)
				pDC->SetBkColor( RGB(255, 255, 255) );
			else
				pDC->SetBkColor( ReferApp.m_BackgroundColor );
			pDC->SetBkMode(OPAQUE);
			pDC->TextOut( iPosLeyendaX, iPosLeyendaY, sRegularity );
			pDC->SetBkMode(TRANSPARENT);
			iPosLeyendaY -= 10;
		};
		pDC->SetBkMode(TRANSPARENT);
	};

	// Destroy pens and Fonts now to free up resources.
	pDC->SelectObject( pPlumaAnterior );
	PlumaIteraciones.DeleteObject();
	pDC->SelectObject(pFuenteAnterior);
	Fuente.DeleteObject();
}


void CWinRf_ViewOptimization::PlotVariablesZ(CDC *pDC, std::vector<double> Data,
											 CDataInflation ParametrosOpt,
											 CString sTitleAxisYMain,
											 CString sTitleAxisX)
//Plot the evolutions of the variables Z

//Depending on the mode, plots all the variables or just a subset
{
	if ( (long)Data.size() == 0 ) return;

	//Range of variables to plot
	long iInitialVariable, iFinalVariable;
	if ( m_GraphicStyle.iTypeOfGraphic == DIAGRAM_ALL_Z_VARIABLES ) {
		iInitialVariable = 0;
		iFinalVariable = ReferApp.In.NumVariables - 1;
	}
	else {  // DIAGRAM_SOME_Z_VARIABLES
		iInitialVariable = m_GraphicStyle.iInitialVariable;
		iFinalVariable = m_GraphicStyle.iFinalVariable;
		if (iFinalVariable > ReferApp.In.NumVariables-1)
			iFinalVariable = ReferApp.In.NumVariables - 1;
	};

	long iDataPerIteration = ReferApp.In.NumVariables;

	//Range of the functions to plot
	long lNumeroMuestras = (long)Data.size() / iDataPerIteration;

	double dDatoMayor, dDatoMenor;
	dDatoMayor = dDatoMenor = Data[0];
	for (long i = iInitialVariable; i <= iFinalVariable ; i++ ){
		for(long lIndice=0 ; lIndice < lNumeroMuestras ; lIndice++ ) {
			double dDato= Data[lIndice * iDataPerIteration + i];
			dDatoMayor = max( dDatoMayor, dDato );
			dDatoMenor = min( dDatoMenor, dDato );
		}
	}

	//Calculate transformation
	cGDI_Transform Transf;
	//cGDI_Transform TransfRegularity;
	CRect rect;
	GetClientRect( &rect );

	double dFactorAmpliacionRangos = 0.1;
	double dRangeY = (dDatoMayor - dDatoMenor) * (1 + dFactorAmpliacionRangos*2);
	if(dRangeY == 0) dRangeY= 100; //Just in case only one "flat" curve is selected

	int iBorderLeft = 60;
	int iBorderRight = 52;
	int iBorderUp = 45;
	int iBorderDown = 30;

	Transf.GDI_SetViewport(iBorderLeft, iBorderUp,
						   rect.right-iBorderRight, rect.bottom-iBorderDown );
	Transf.GDI_SetWorld(0,
					   (double)dDatoMenor - dRangeY*dFactorAmpliacionRangos,
					   (double)lNumeroMuestras,
					   (double)dDatoMayor + dRangeY*dFactorAmpliacionRangos );

	//TransfRegularity.GDI_SetViewport(iBorderLeft, iBorderUp,
	//								  rect.right-iBorderRight, rect.bottom-iBorderDown );

	// Prepara las Plumas y las Fuentes para dibujar el grafico
	CPen PlumaDato, PlumaIteraciones, PlumaComponentes, *pPlumaAnterior;
	CFont Fuente, *pFuenteAnterior;
	CString sTexto;
	COLORREF ColorIteraciones, ColorZ;
//	ColorDato = RGB( 0, 200, 0 ) ^ ReferApp.m_BackgroundColor;
//	ColorDato = RGB( 255, 255, 255 ) ^ ReferApp.m_BackgroundColor;
	if (pDC->IsPrinting() != 0)
		ColorIteraciones = RGB( 0, 0, 0);
	else
		ColorIteraciones = RGB( 200, 200, 200 ) ^ ReferApp.m_BackgroundColor;

	Fuente.CreatePointFont( 8*10, "Arial", pDC);
	pFuenteAnterior = (CFont *) pDC->SelectObject(&Fuente);
	pDC->SetBkMode(TRANSPARENT);

	PlumaIteraciones.CreatePen( PS_SOLID, 1, ColorIteraciones );

	//Plot the curves
	//---------------
	pDC->SetTextAlign(TA_BOTTOM | TA_LEFT);
	for (long ii = iInitialVariable; ii <= iFinalVariable; ii++){
		CPoint PuntoAnterior = Transf.GDI_WorldToViewport( (double)0, Data[ii] );
		ColorZ = 100 + ii*20 + (ii*256*10);
		PlumaDato.CreatePen( PS_SOLID, 1, ColorZ );
		pPlumaAnterior = pDC->SelectObject( &PlumaDato );

		CPoint Punto;
		for(long j=0 ; j < lNumeroMuestras ; j++ ) {
			Punto = Transf.GDI_WorldToViewport((double)j, Data[j * iDataPerIteration + ii]);
			pDC->MoveTo( PuntoAnterior.x, PuntoAnterior.y );
			pDC->LineTo( Punto.x, Punto.y );
			PuntoAnterior = Punto;
		};
		sTexto.Format( "Z%i", ii);
		pDC->SetTextColor( ColorZ );
		pDC->TextOut( Punto.x+3, Punto.y+6, sTexto );

		pDC->SelectObject( pPlumaAnterior );
		PlumaDato.DeleteObject();
	};

	//Plot the axes
	//-------------
	pPlumaAnterior = pDC->SelectObject( &PlumaIteraciones );

	long SpacingBetweenYLabels= 40;
	long NumDivisionsY= max((rect.bottom - (iBorderUp + iBorderDown)) / SpacingBetweenYLabels, 1);

	// Draw the primary Y Axis : values of the Variables (the one on the left)
	pDC->MoveTo(iBorderLeft, iBorderUp);
	pDC->LineTo(iBorderLeft, rect.bottom - iBorderDown);
	pDC->SetTextColor( ColorIteraciones );
	pDC->SetTextAlign(TA_BOTTOM | TA_CENTER);
	pDC->TextOut( iBorderLeft, iBorderUp, sTitleAxisYMain );
	pDC->SetTextAlign(TA_BOTTOM | TA_RIGHT);

	// Draw division lines and value labels on the primary Y Axis
	double dDato = dDatoMenor;
	for (long i=0 ; i <= NumDivisionsY ; i++) {
		CPoint Punto = Transf.GDI_WorldToViewport( (double)0, dDato );
		pDC->MoveTo( Punto.x-2, Punto.y );
		pDC->LineTo( Punto.x+2, Punto.y );
		sTexto.Format("%4.3f", (float)dDato);
		pDC->TextOut( Punto.x-3, Punto.y+6, sTexto );
		dDato += (dDatoMayor - dDatoMenor)/ NumDivisionsY;
	};

	// Draw the X Axis: Iterations
	pDC->MoveTo(iBorderLeft, rect.bottom - iBorderDown);
	pDC->LineTo(rect.right-iBorderRight, rect.bottom - iBorderDown);
	pDC->SetTextColor( ColorIteraciones );
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	pDC->TextOut( rect.right/2, rect.bottom - iBorderDown/2, sTitleAxisX );

	// Draw division lines and value labels on the X Axis
	long SpacingBetweenXLabels= 40;
	long NumDivisionsX = max(min(lNumeroMuestras, ((rect.right - (iBorderLeft + iBorderRight)) / SpacingBetweenXLabels)), 1);
	double dPosicion = 0;
	double dIteraciones = 0;
	for (long i=0 ; i<=NumDivisionsX ; i++) {
		CPoint Punto = Transf.GDI_WorldToViewport( dPosicion, dDatoMayor );
		Punto.y = rect.bottom-iBorderDown;
		pDC->MoveTo( Punto.x, Punto.y-2 );
		pDC->LineTo( Punto.x, Punto.y+2 );
		sTexto.Format("%i", (int)dIteraciones);
		pDC->TextOut( Punto.x, Punto.y+3, sTexto );
		dPosicion += ((double)lNumeroMuestras / (double)NumDivisionsX) + 0.0000001;
		dIteraciones += ((double)lNumeroMuestras / (double)NumDivisionsX);
						//ReferApp.In.NumVariables + 0.0000001;
	};

	//Pens and Fonts are destroyed to free resources
	pDC->SelectObject( pPlumaAnterior );
	PlumaIteraciones.DeleteObject();
	pDC->SelectObject(pFuenteAnterior);
	Fuente.DeleteObject();
}


void CWinRf_ViewOptimization::PlotDiagrams(CDC* pDC)
//Uses the Optimization window to plot different diagrams of the evolution of the optimization process

//Available diagrams are:
//  - Evolutions of costs and temperatures
//  - Evolutions of optimization variables
{
	CString sTexto, sAux;
	CString sTitleAxisX, sTitleAxisYMain, sTitleAxisYAux;

	CRect rect;
	GetClientRect(&rect);

	//Title
	//-----
	CFont Fuente, *pFuenteAnterior;
	Fuente.CreatePointFont( 10*10, "Arial", pDC);
	pFuenteAnterior = (CFont *) pDC->SelectObject(&Fuente);

	if (pDC->IsPrinting() != 0)
		pDC->SetTextColor( RGB( 0, 0, 255) );
	else{
		if (ReferApp.m_BackgroundColor==16777215)     //RGB(255, 255, 255)
			pDC->SetTextColor( RGB(0, 0, 0));
		else
			pDC->SetTextColor( RGB(0, 160, 255) ^ ReferApp.m_BackgroundColor);
	}

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);

	if (ReferApp.In.iOptAlgorithm == SIMULATED_ANNEALING_MULTI)
			sTexto= "Cost evolution for every regularity";
	else{
		switch( m_GraphicStyle.iTypeOfGraphic ) {
			case DIAGRAM_TEMPERATURE_COST:
				if (ReferApp.In.iOptAlgorithm == HILL_CLIMBING)
					sTexto= "Cost Evolution";
				else
					sTexto= "Cost and Temperature evolution";
				break;
			case DIAGRAM_ALL_Z_VARIABLES:
				sTexto= "Evolution of the Z variables";
				break;
			case DIAGRAM_SOME_Z_VARIABLES:
				sTexto= "Evolution of the Z variables";
				sAux.Format( " : %i - %i", m_GraphicStyle.iInitialVariable,
										   m_GraphicStyle.iFinalVariable );
				sTexto += sAux;
				break;
		}
	}

	pDC->TextOut( rect.right/2, 13, sTexto );

	sTexto= "in the ";
	sTitleAxisX= "Iterations";
	switch(ReferApp.In.iOptAlgorithm) {
		case HILL_CLIMBING:
			sTexto += "Hill-Climbing";
			sTitleAxisYAux= "Step length";
			break;
		case SIMULATED_ANNEALING:
			sTexto += "Simulated Annealing";
			sTitleAxisYAux= "Temperature";
			break;
		case SIMULATED_ANNEALING_MULTI:
			sTexto += "Simulated Annealing Multicriteria";
			sTitleAxisYAux= "Temperature";
			break;
		default:
			sTexto += "of Optimization";
			sTitleAxisYAux = "";
	};
	sTexto+= " Algorithm";

	pDC->TextOut( rect.right/2, 29, sTexto );

	pDC->SelectObject(pFuenteAnterior);
	Fuente.DeleteObject();

	Fuente.CreatePointFont( 7*10, "Arial", pDC);
	pFuenteAnterior = (CFont *) pDC->SelectObject(&Fuente);
	if (pDC->IsPrinting() != 0)
		pDC->SetTextColor( RGB( 0,0,0) );
	else{
		if (ReferApp.m_BackgroundColor==16777215)     //RGB(255, 255, 255)
			pDC->SetTextColor( RGB(0, 0, 0));
		else
			pDC->SetTextColor( RGB( 200, 200, 200) ^ ReferApp.m_BackgroundColor);
	}
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
	sTexto = "Time: " + ReferApp.m_sTimeCalculating;
	pDC->TextOut( 2, rect.bottom-2, sTexto );

	pDC->SelectObject(pFuenteAnterior);
	Fuente.DeleteObject();

	//Diagram
	//-------
	if (m_GraphicStyle.iTypeOfGraphic == DIAGRAM_TEMPERATURE_COST)
		if (ReferApp.In.iOptAlgorithm == SIMULATED_ANNEALING_MULTI){
			sTitleAxisYMain= "SubCosts";
			PlotCostsMulti(pDC, ReferApp.In.EvolutionCosts, ReferApp.In,
						   sTitleAxisYMain, sTitleAxisYAux, sTitleAxisX);
		}
		else {
			sTitleAxisYMain= "Cost";
			PlotCost(pDC, ReferApp.In.EvolutionCosts, ReferApp.In,
					 sTitleAxisYMain, sTitleAxisYAux, sTitleAxisX);
		}
	else {
		sTitleAxisYMain= "Value";
		PlotVariablesZ(pDC, ReferApp.In.EvolutionZ, ReferApp.In,
					   sTitleAxisYMain, sTitleAxisX);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewOptimization drawing

void CWinRf_ViewOptimization::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO: calculate the total size of this view
	CSize sizeTotal;
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


void CWinRf_ViewOptimization::OnDraw(CDC* pDC)
{
	if (m_GraphicStyle.iTypeOfGraphic==MAP_ISOLINES){
		DrawSubSpace(pDC);
	}
	else{
		PlotDiagrams(pDC);
	}
}



/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewOptimization diagnostics

#ifdef _DEBUG
//void CWinRf_ViewOptimization::AssertValid() const
//{
//	CScrollView::AssertValid();
//}
//
//void CWinRf_ViewOptimization::Dump(CDumpContext& dc) const
//{
//	CScrollView::Dump(dc);
//}

CReferDoc* CWinRf_ViewOptimization::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CReferDoc)));
	return (CReferDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewOptimization message handlers

BOOL CWinRf_ViewOptimization::OnEraseBkgnd(CDC* pDC) 
// Clears the windows client area with background color
{
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect( rect, ReferApp.m_BackgroundColor );

	// Return TRUE indicates that no further erasing is required
	return true;
}


void CWinRf_ViewOptimization::OnDestroy() 
{
	CScrollView::OnDestroy();

	GetDocument()->m_pFrameOptimization = NULL;

	// Control bar objects will be cleaned up when the parent HWND and object are destroyed
	//CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
	//pFrame->ShowControlBar( &pFrame->m_wndToolBar_Optimizacion_Diagrams, false, false );
}


void CWinRf_ViewOptimization::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CSize TamanyoDibujo;
	CRect rect;

	GetClientRect(&rect);
	if (pDC->IsPrinting() != 0){

		// Tamaño para conseguir "llenar" el papel en la impresion

		//TamanyoDibujo.cx = TamanyoDibujo.cy = 50;
		TamanyoDibujo.cx = TamanyoDibujo.cy = 200;
		SetScrollSizes(MM_TWIPS, TamanyoDibujo);
	}
	else {
		TamanyoDibujo.cx = TamanyoDibujo.cy = min( rect.bottom, rect.right );
		SetScrollSizes(MM_TEXT, TamanyoDibujo);
	};

//	CScrollView::OnPrepareDC(pDC, pInfo);
}


void CWinRf_ViewOptimization::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollView::OnSetFocus(pOldWnd);
}


void CWinRf_ViewOptimization::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//107 Código de teclado de "+"
	//109 Código de teclado de "-"
	//37 Código de teclado de "Flecha izquierda"
	//39 Código de teclado de "Flecha derecha"
	//38 Código de teclado de "Flecha superior"
	//40 Código de teclado de "Flecha inferior"

	if (!CurveCalculationInProgress){
		double DesplazamientoX=(pParamsIsolines->XF-pParamsIsolines->X0)/3;
		double DesplazamientoY=(pParamsIsolines->YF-pParamsIsolines->Y0)/3;

		if (pParamsIsolines->TG==0){
			//SOne variable diagram
			switch (nChar){
			case 39:
				//Flecha derecha
				pParamsIsolines->X0+=DesplazamientoX;
				pParamsIsolines->XF+=DesplazamientoX;
				Invalidate();
				break;
			case 37:
				//Flecha izquierda
				pParamsIsolines->X0-=DesplazamientoX;
				pParamsIsolines->XF-=DesplazamientoX;
				Invalidate();
				break;
			case 109:
				//Signo -
				OnZoomReducir();
				break;
			case 107:
				//Signo +
				OnZoomAmpliar();
				break;
			}
		}
		else{
			//Two variables map
			switch (nChar){
			case 39:
				//Felcha derecha
				pParamsIsolines->X0+=DesplazamientoX;
				pParamsIsolines->XF+=DesplazamientoX;
				Invalidate();
				break;
			case 37:
				//Flecha izquierda
				pParamsIsolines->X0-=DesplazamientoX;
				pParamsIsolines->XF-=DesplazamientoX;
				Invalidate();
				break;
			case 38:
				//Flecha superior
				pParamsIsolines->Y0+=DesplazamientoY;
				pParamsIsolines->YF+=DesplazamientoY;
				Invalidate();
				break;
			case 40:
				//Flecha inferior
				pParamsIsolines->Y0-=DesplazamientoY;
				pParamsIsolines->YF-=DesplazamientoY;
				Invalidate();
				break;
			case 109:
				//Signo -
				OnZoomReducir();
				break;
			case 107:
				//Signo +
				OnZoomAmpliar();
				break;
			}
		}
	}

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CWinRf_ViewOptimization::OnMouseMove(UINT nFlags, CPoint point) 
{
	CPoint Punto1, Punto2;
	if (bZoom){
		Punto1=Zoom[0];
		Punto1.x=point.x;
		Zoom[1]=Punto1;
		Zoom[2]=point;
		Punto1=Zoom[0];
		Punto1.y=point.y;
		Zoom[3]=Punto1;
		Zoom[4]=Zoom[0];
		Invalidate();
	}
	
	CScrollView::OnMouseMove(nFlags, point);
}

//------------------toolbar Optimization--------------------------------------------

void CWinRf_ViewOptimization::OnViewCost() 
{
	m_GraphicStyle.iTypeOfGraphic = DIAGRAM_TEMPERATURE_COST;
	
	if (PanDefined){
		pEncuadre->DestroyWindow();
		PanDefined=false;
	}

	Invalidate();
}

void CWinRf_ViewOptimization::OnUpdateViewCost(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_GraphicStyle.iTypeOfGraphic == DIAGRAM_TEMPERATURE_COST);
//	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
//	pCmdUI->Enable(pFrame->MDIGetActive() == m_pFrameOptimization);
}


void CWinRf_ViewOptimization::OnViewPartialCosts() 
{
	ReferApp.In.bViewPartialCostsEvolution =
		!ReferApp.In.bViewPartialCostsEvolution;

	Invalidate();
}


void CWinRf_ViewOptimization::OnUpdateViewPartialCosts(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(ReferApp.In.bViewPartialCostsEvolution);
//	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
//	pCmdUI->Enable(pFrame->MDIGetActive() == m_pFrameOptimization);
}

void CWinRf_ViewOptimization::OnViewZ() 
{
	m_GraphicStyle.iTypeOfGraphic = DIAGRAM_ALL_Z_VARIABLES;
	Invalidate();
	if (PanDefined){
		pEncuadre->DestroyWindow();
		PanDefined=false;
	}
}


void CWinRf_ViewOptimization::OnUpdateViewZ(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_GraphicStyle.iTypeOfGraphic == DIAGRAM_ALL_Z_VARIABLES);
//	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
//	pCmdUI->Enable(pFrame->MDIGetActive() == m_pFrameOptimization);
}


void CWinRf_ViewOptimization::OnViewSomeZs() 
{
	m_GraphicStyle.iTypeOfGraphic = DIAGRAM_SOME_Z_VARIABLES;
	if (PanDefined){
		pEncuadre->DestroyWindow();
		PanDefined=false;
	}

	Invalidate();
}


void CWinRf_ViewOptimization::OnUpdateViewSomeZs(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_GraphicStyle.iTypeOfGraphic == DIAGRAM_SOME_Z_VARIABLES);
//	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
//	pCmdUI->Enable(pFrame->MDIGetActive() == m_pFrameOptimization);
}


void CWinRf_ViewOptimization::OnViewIsolines() 
{
	m_GraphicStyle.iTypeOfGraphic = MAP_ISOLINES;
	Invalidate();
}

void CWinRf_ViewOptimization::OnUpdateViewIsolines(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_GraphicStyle.iTypeOfGraphic==MAP_ISOLINES);
//	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
//	pCmdUI->Enable(pFrame->MDIGetActive() == m_pFrameOptimization);
}


void CWinRf_ViewOptimization::OnShowLegend() 
{
	m_GraphicStyle.bShowLegend = !m_GraphicStyle.bShowLegend;
	Invalidate();
}


void CWinRf_ViewOptimization::OnUpdateShowLegend(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_GraphicStyle.bShowLegend);
//	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
//	pCmdUI->Enable(pFrame->MDIGetActive() == m_pFrameOptimization);
}


void CWinRf_ViewOptimization::OnLabels() 
{
	m_GraphicStyle.bShowLabels = !m_GraphicStyle.bShowLabels;
	Invalidate();
}


void CWinRf_ViewOptimization::OnUpdateLabels(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_GraphicStyle.bShowLabels);
//	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
//	pCmdUI->Enable(pFrame->MDIGetActive() == m_pFrameOptimization &
//				   m_GraphicStyle.iTypeOfGraphic==MAP_ISOLINES &
//				   pParamsIsolines->TG == 1);
	pCmdUI->Enable(m_GraphicStyle.iTypeOfGraphic==MAP_ISOLINES &&
				   pParamsIsolines->TG == 1);
}


void CWinRf_ViewOptimization::OnViewConfigEvolutionVariablesOpt() 
{
	CDialogOptimizationConfig Dlg;
	Dlg.SetParams(m_GraphicStyle);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(m_GraphicStyle);
		Invalidate();
	};
	if (m_GraphicStyle.iTypeOfGraphic == MAP_ISOLINES){
		if (!PanDefined){
			pEncuadre = new CDialogOptimizationPan();
			pEncuadre->SetParams(pParamsIsolines);
			PanDefined=true;
		}
		else{
			pEncuadre->DestroyWindow();
			PanDefined=false;
		}
	}
}


void CWinRf_ViewOptimization::OnUpdateViewConfigEvolutionVariablesOpt(CCmdUI* pCmdUI) 
{
//	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
//	pCmdUI->Enable(pFrame->MDIGetActive() == m_pFrameOptimization);
}


void CWinRf_ViewOptimization::OnConfigIsolines() 
//Displays the dialog of isolines, and updates the parameters

//Uses data from the general DataBase DB_Brep_Model
{
	CReferDoc *pDoc = GetDocument();

	long NumeroVariables= pDoc->DB_Brep_Model->GetSizeVertices();

	bool Recalcula=false;
	bool Fin=false;

	//Calculates initial params if they have not yet been calculated
	if(ResetCurves){
		ResetCurves=false;

		ResetZoomPan();
	}

	//Open dialog
	CDialogOptimizationIsolines Dlg;

	CParamsIsolines LocalParams;
	LocalParams= *pParamsIsolines;
	Dlg.SetParams(&LocalParams);

	do{
		if (Dlg.DoModal() == IDOK){

			Dlg.GetParams(&LocalParams);
			if (LocalParams.X>=NumeroVariables)
				AfxMessageBox("Variable X does not exist");
			else{
				if (LocalParams.Y>=NumeroVariables)
					AfxMessageBox("Variable Y does not exist");
				else{
					if (LocalParams.X==LocalParams.Y &&
						LocalParams.TG==1)
						AfxMessageBox("Variables X and Y are the same");
					else{
						if (LocalParams.TG==1){
							if(LocalParams.OtherVariables==2 &&
							(LocalParams.V==LocalParams.X ||
							LocalParams.V==LocalParams.Y))
								AfxMessageBox("The vertex analyzed matches with the variable");
							else{
								Recalcula=true;
								*pParamsIsolines=LocalParams;
							}
						}
						else{
							if(LocalParams.OtherVariables==2 &&
							   LocalParams.V==LocalParams.X)
								AfxMessageBox("The vertex analyzed matches with the variable");
							else{
								Recalcula=true;
								*pParamsIsolines= LocalParams;
							}
						}
					}
				}
			}
		}
		else
			Fin=true;

	}while(!Recalcula && !Fin);


	//Force redraw
	Invalidate();
}


void CWinRf_ViewOptimization::OnUpdateConfigIsolines(CCmdUI* pCmdUI) 
{
//	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
//	pCmdUI->Enable(pFrame->MDIGetActive() == m_pFrameOptimization);
}


void CWinRf_ViewOptimization::OnZoomAmpliar() 
{
	double AmpliacionX;
	double AmpliacionY;
	double X0, XF, Y0, YF;

	if (!CurveCalculationInProgress){

		AmpliacionX = (pParamsIsolines->XF - pParamsIsolines->X0)/4;

		if (pParamsIsolines->TG==0){

			//One variable diagram
			X0=pParamsIsolines->X0+AmpliacionX;
			XF=pParamsIsolines->XF-AmpliacionX;
			if (X0<XF){
				pParamsIsolines->X0+=AmpliacionX;
				pParamsIsolines->XF-=AmpliacionX;
				Invalidate();
			}
		}
		else{
			AmpliacionY = (pParamsIsolines->YF - pParamsIsolines->Y0)/4;

			//Two variables map
			X0=pParamsIsolines->X0+AmpliacionX;
			XF=pParamsIsolines->XF-AmpliacionX;
			Y0=pParamsIsolines->Y0+AmpliacionY;
			YF=pParamsIsolines->YF-AmpliacionY;

			if (X0<XF && Y0<YF){
				pParamsIsolines->X0+=AmpliacionX;
				pParamsIsolines->XF-=AmpliacionX;
				pParamsIsolines->Y0+=AmpliacionY;
				pParamsIsolines->YF-=AmpliacionY;
				Invalidate();
			}
		}
	}
}


void CWinRf_ViewOptimization::OnUpdateZoomAmpliar(CCmdUI* pCmdUI) 
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *)ReferApp.m_pMainWnd;

	pCmdUI->Enable(pFrame->MDIGetActive() == GetDocument()->m_pFrameOptimization &&
					m_GraphicStyle.iTypeOfGraphic == MAP_ISOLINES);
}


void CWinRf_ViewOptimization::OnZoomReducir() 
{
	double AmpliacionX;
	double AmpliacionY;

	if (!CurveCalculationInProgress){

		AmpliacionX = (pParamsIsolines->XF-pParamsIsolines->X0)/2;

		if (pParamsIsolines->TG==0){
			//One variable diagram
			pParamsIsolines->X0-=AmpliacionX;
			pParamsIsolines->XF+=AmpliacionX;
			Invalidate();
		}
		else{
			AmpliacionY = (pParamsIsolines->YF-pParamsIsolines->Y0)/2;

			//Two variables map
			pParamsIsolines->X0-=AmpliacionX;
			pParamsIsolines->XF+=AmpliacionX;
			pParamsIsolines->Y0-=AmpliacionY;
			pParamsIsolines->YF+=AmpliacionY;
			Invalidate();
		}
	}
}


void CWinRf_ViewOptimization::OnUpdateZoomReducir(CCmdUI* pCmdUI) 
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *)ReferApp.m_pMainWnd;

	pCmdUI->Enable(pFrame->MDIGetActive() == GetDocument()->m_pFrameOptimization &&
				   m_GraphicStyle.iTypeOfGraphic == MAP_ISOLINES);
}


void CWinRf_ViewOptimization::OnZoomTodo() 
{
	if (!ResetCurves && !CurveCalculationInProgress){
		pParamsIsolines->X0 = pParamsIsolines->ZTX0; 
		pParamsIsolines->XF = pParamsIsolines->ZTXF; 
		pParamsIsolines->INCX = pParamsIsolines->ZTINCX; 
		pParamsIsolines->Y0 = pParamsIsolines->ZTY0; 
		pParamsIsolines->YF = pParamsIsolines->ZTYF;
		pParamsIsolines->INCY = pParamsIsolines->ZTINCY; 

		Invalidate();
	}
}


void CWinRf_ViewOptimization::OnUpdateZoomTodo(CCmdUI* pCmdUI) 
{
	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *)ReferApp.m_pMainWnd;

	pCmdUI->Enable(pFrame->MDIGetActive() == GetDocument()->m_pFrameOptimization &&
				   m_GraphicStyle.iTypeOfGraphic == MAP_ISOLINES &&
				   !ResetCurves);
}


void CWinRf_ViewOptimization::OnEncuadre() 
{
	if (!PanDefined){
		pEncuadre = new CDialogOptimizationPan();
		pEncuadre->SetParams(pParamsIsolines);
		PanDefined=true;
	}
	else{
		PanDefined=false;
		pEncuadre->DestroyWindow();
	}
}


void CWinRf_ViewOptimization::OnUpdateEncuadre(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(PanDefined);
//	CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
//	pCmdUI->Enable(pFrame->MDIGetActive() == m_pFrameOptimization &
//				   m_GraphicStyle.iTypeOfGraphic==MAP_ISOLINES);
	pCmdUI->Enable(m_GraphicStyle.iTypeOfGraphic==MAP_ISOLINES);
}
