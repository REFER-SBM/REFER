//+-----------------------------------------------------------------------------+
//| Copyleft (C) 2016 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

#include <stdafx.h>

#include <cassert>
#include <cfloat>     // isnan
#include <cmath>

#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "DB_Model_Maths.h"

#include "OptimizationCost.h"

const double PrecisionAna = 0.001;  // Used in Regularidades3D

///////////////////////////GEOMETRY///////////////////////////
class vert2d
	{ public: double x,y; };
class vert3d
	{ public: double x,y,z; };
class vector2d
	{ public: vert2d v1,v2; };
class vector3d
	{ public: vert3d v1,v2; };


class pila_vec3d
	{ public: vector3d *dato; long tam; pila_vec3d *sig; };


void apilar(pila_vec3d **pila,vector3d *trio,long tam) {
	pila_vec3d *p=new pila_vec3d;  // delete missing
	p->dato=trio;
	p->tam=tam;
	p->sig=*pila;
	*pila=p;
}


pila_vec3d *desapilar(pila_vec3d **pila) {
	pila_vec3d *p=*pila;
	*pila=(*pila)->sig;
	return p;
}


inline double modulo3d(CVertex v1,CVertex v2) {
	return sqrt(pow(v1.x-v2.x, 2)+pow(v1.y-v2.y, 2)+pow(v1.z-v2.z,2));
}


inline double modulo2d(CVertex v1,CVertex v2) {
	return sqrt(pow(v1.x-v2.x, 2)+pow(v1.y-v2.y, 2));
}


inline double prod_esc_2d(vector2d v1,vector2d v2,char tipo)
{
	//v2 is the head and v1 is the base of the vector
	vert2d vector1,vector2;
	vector1.x= v1.v2.x-v1.v1.x;
	vector1.y= v1.v2.y-v1.v1.y;
	vector2.x= v2.v2.x-v2.v1.x;
	vector2.y= v2.v2.y-v2.v1.y;

	if (tipo){
		double modulo1= sqrt(vector1.x*vector1.x+vector1.y*vector1.y);
		if (modulo1!=0)
			{ vector1.x/=modulo1; vector1.y/=modulo1; }
		else
			{vector1.x=0; vector1.y=0; }

		double modulo2= sqrt(vector2.x*vector2.x+vector2.y*vector2.y);
		if (modulo2!=0)
			{ vector2.x/=modulo2; vector2.y/=modulo2; }
		else
			{vector2.x=0; vector2.y=0; }
	}

	return vector1.x*vector2.x+vector1.y*vector2.y;
}


//producto escalar 2D que admite entradas 3D
inline double prod_esc_2d(vector3d v1,vector3d v2,char tipo)
{
	double modulo1,modulo2;
	vert3d vector1,vector2;
	vector1.x=v1.v2.x-v1.v1.x;
	vector1.y=v1.v2.y-v1.v1.y;
	vector2.x=v2.v2.x-v2.v1.x;
	vector2.y=v2.v2.y-v2.v1.y;
	if (tipo) { modulo1=sqrt(pow(vector1.x,2)+pow(vector1.y,2));
		modulo2=sqrt(pow(vector2.x,2)+pow(vector2.y,2));
		if (modulo1!=0) { vector1.x/=modulo1; vector1.y/=modulo1; }
		else {vector1.x=0; vector1.y=0;}
		if (modulo2!=0) { vector2.x/=modulo2; vector2.y/=modulo2; }
		else {vector2.x=0; vector2.y=0;}
	}
	return vector1.x*vector2.x+vector1.y*vector2.y;
}


//producto escalar 2D que admite entradas 3D
inline double prod_esc_2d(vert3d v1,vert3d v2,char tipo)
{
	double modulo1,modulo2;
	if (tipo) { modulo1=sqrt(pow(v1.x,2)+pow(v1.y,2));
		modulo2=sqrt(pow(v2.x,2)+pow(v2.y,2));
		if (modulo1!=0) { v1.x/=modulo1; v1.y/=modulo1; }
		else {v1.x=0; v1.y=0;}
		if (modulo2!=0) { v2.x/=modulo2; v2.y/=modulo2; }
		else {v2.x=0; v2.y=0;}
	}
	return v1.x*v2.x+v1.y*v2.y;
}


inline double prod_esc_3d(vector3d v1,vector3d v2,char tipo)
{
	double modulo1,modulo2;
	vert3d vector1,vector2;

	vector1.x=v1.v2.x-v1.v1.x;
	vector1.y=v1.v2.y-v1.v1.y;
	vector1.z=v1.v2.z-v1.v1.z;
	vector2.x=v2.v2.x-v2.v1.x;
	vector2.y=v2.v2.y-v2.v1.y;
	vector2.z=v2.v2.z-v2.v1.z;
	if (tipo) { modulo1=sqrt(pow(vector1.x,2)+pow(vector1.y,2)+pow(vector1.z,2));
		modulo2=sqrt(pow(vector2.x,2)+pow(vector2.y,2)+pow(vector2.z,2));
		if (modulo1!=0) { vector1.x/=modulo1; vector1.y/=modulo1; vector1.z/=modulo1;}
		else {vector1.x=0; vector1.y=0;}
		if (modulo2!=0) {vector2.x/=modulo2; vector2.y/=modulo2; vector2.z/=modulo2;}
		else {vector2.x=0; vector2.y=0;}
	}
	return vector1.x*vector2.x+vector1.y*vector2.y+vector1.z*vector2.z;
}


inline double prod_esc_3d(vert3d vector1,vert3d vector2,char tipo){
	
	double modulo1,modulo2;
	if (tipo){
		modulo1=sqrt(pow(vector1.x,2)+pow(vector1.y,2)+pow(vector1.z,2));
		modulo2=sqrt(pow(vector2.x,2)+pow(vector2.y,2)+pow(vector2.z,2));
		if (modulo1!=0){
			vector1.x/=modulo1;
			vector1.y/=modulo1;
			vector1.z/=modulo1;
		}
		else{
			vector1.x=0;
			vector1.y=0;
		}
		if (modulo2!=0){
			vector2.x/=modulo2;
			vector2.y/=modulo2;
			vector2.z/=modulo2;
		}
		else{
			vector2.x=0;
			vector2.y=0;
		}
	}
	return vector1.x*vector2.x+vector1.y*vector2.y+vector1.z*vector2.z;
}


vert3d *prod_vec_3d(vector3d v1,vector3d v2)
{
	vert3d l1,l2,*p=new vert3d;  // delete missing

	l1.x=v1.v2.x-v1.v1.x;
	l1.y=v1.v2.y-v1.v1.y;
	l1.z=v1.v2.z-v1.v1.z;
	l2.x=v2.v2.x-v2.v1.x;
	l2.y=v2.v2.y-v2.v1.y;
	l2.z=v2.v2.z-v2.v1.z;

	p->x=l1.y*l2.z-l1.z*l2.y;
	p->y=l1.z*l2.x-l1.x*l2.z;
	p->z=l1.x*l2.y-l1.y*l2.x;

	return p;
}


vert3d *prod_vec_3d(vert3d v1,vert3d v2)
{
	vert3d *p=new vert3d;  // delete missing

	p->x=v1.y*v2.z-v1.z*v2.y;
	p->y=v1.z*v2.x-v1.x*v2.z;
	p->z=v1.x*v2.y-v1.y*v2.x;

	return p;
}


inline double det( double e11,double e12,double e13,
					double e21,double e22,double e23,
					double e31,double e32,double e33) {

	return ( (e11*e22*e33)+(e12*e23*e31)+(e13*e21*e32)
			-(e13*e22*e31)-(e12*e21*e33)-(e11*e23*e32));
}


///////////////////////////END GEOMETRY///////////////////////////



inline double mu(double a,double b,double x)
{
	double dValor= 1.1 * exp( - pow((x-a)/b, 2) ) - 0.1;
	return max( 0, dValor );
}


double CalculaUab( double a, double b, double x )
// Calcule continuos compliance factor

//It is a "dumped" factor that equals 1 for null divergence (x=a)
//and descends to zero like a normal distribution curve 
//for x different to a (with sigma=b)

//In other words:
//	- x is the value to be checked, 
//	- a is the nominal value, and 
//	- b is a reasonable max deviation

//Based on:
//	H Lipson, M Shpitalni
//	Optimization-based reconstruction of a 3D object
//	from a single freehand line drawing
//	Computer-Aided Design, 28(8) 651-663, 1996
{
	double dValor= 1.1 * exp( - pow((x-a)/b, 2) ) - 0.1;
	assert(!_isnan(dValor));

	return max(0, dValor);
}


void GetWeights_LineParallelism(CDB_Model *DDBB,
								std::vector<std::vector<double>> &Weights_LineParallelism)
//Explore all possible pairings between model edges
//and calculate a weight for each pairing.

//Those pairs with non-zero weight will be used to increase the cost of parallelism
//in the event that they are not parallel in the tentative model.
{
	// Match each edge with all the others
	for(long i=0; i<DDBB->GetSizeEdges(); i++ ){
		Weights_LineParallelism[i][i]= 0; 
	
		CEdge Arista1= DDBB->GetEdge(i);

		CVertex HeadVertex1= DDBB->GetVertex(Arista1.Head);
		CVertex TailVertex1= DDBB->GetVertex(Arista1.Tail);

		CVertex VectorUnitario2D1= GetUnitVector2D(HeadVertex1, TailVertex1);

		//Given the edge Arista1, check its parallelism with all the others
		for(long j=i+1; j<DDBB->GetSizeEdges(); j++ ) {
			CEdge Arista2= DDBB->GetEdge(j);

			CVertex HeadVertex2= DDBB->GetVertex(Arista2.Head);
			CVertex TailVertex2= DDBB->GetVertex(Arista2.Tail);

			CVertex VectorUnitario2D2= GetUnitVector2D(HeadVertex2, TailVertex2);

			double dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D2 );
			
			//Keep dProductoEscalar in the range [-1, 1]
			assert( !_isnan(dProductoEscalar) );
			if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1)
				dProductoEscalar=1;
			if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy)
				dProductoEscalar=-1;
			double dAnguloRadianes = acos(dProductoEscalar);
			assert(!_isnan(dAnguloRadianes));

			while ((std::fabs(dAnguloRadianes))>NUMBER_PI)
				dAnguloRadianes-=NUMBER_PI;
			if (std::fabs(dAnguloRadianes)>(NUMBER_PI/2))
				dAnguloRadianes = std::fabs(dAnguloRadianes-NUMBER_PI);

			//Calculate weight as a damped function
			//which is worth 1 when they are parallel (angle 0)
			//reduce a little until reaching the threshold
			//and reduces to zero for values?slightly larger than the threshold
			double dPesoW12 = CalculaUab( 0, ReferApp.In.InflationAngleThreshold, dAnguloRadianes );

			Weights_LineParallelism[i][j]= dPesoW12; 
			Weights_LineParallelism[j][i]= dPesoW12;
		};
	}
}


void GetWeights_LineCollinearity(CDB_Model *DDBB,
								 std::vector<std::vector<double>> &Weights_LineCollinearity)
{
	int es;
	bool CompartenCara=false, bien=false;
	double angulo1, angulo2, angulo3, angulo4, angulo5, angulo6, w, dProductoEscalar;

	//Antes de nada me voy a asegurar que la DDBB tiene las Z's a cero
	DDBB->SetAllVertex_Z_ToZero();

	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		Weights_LineCollinearity [i][i] =0;
		CEdge Arista= DDBB->GetEdge(i);
		CVertex v1= DDBB->GetVertex(Arista.Head);
		CVertex v2= DDBB->GetVertex(Arista.Tail);

		if (v1.x > v2.x){
			CVertex vaux;
			vaux= v1;
			v1= v2;
			v2= vaux;
		}
		for (long j=i+1; j<DDBB->GetSizeEdges(); j++) {
			CEdge Arista= DDBB->GetEdge(j);
			CVertex v3= DDBB->GetVertex(Arista.Head);
			CVertex v4= DDBB->GetVertex(Arista.Tail);
			if (v3.x > v4.x){
				CVertex vaux;
				vaux= v3;
				v3= v4;
				v4= vaux;
			}
			if (v1.x < v3.x) es=1;
			if (v1.x > v3.x) es=2;
			if (v1.x == v3.x) es=0;
			bien=false;
			switch (es){
			case 1:
				if (v2.x <=v4.x)(bien=true);
				break;
			case 2:
				if (v4.x<=v1.x) bien=true;
				break;
			case 0:
				//este caso es si las lineas son verticales
				if (v1.y<v2.y){
					if (v3.y<v4.y){
						if (v2.y<=v3.y)
							bien=true;
					}
					else{
						if (v2.y<=v4.y)
							bien=true;
					}
				}
				else{
					if (v3.y<v4.y){
						if (v1.y<=v3.y)
							bien=true;
					}
					else{
						if (v1.y<=v4.y)
							bien=true;
					}
				}
			}
			if (bien){
				CVertex VectorUnitario2D1= GetUnitVector2D(v1, v2);
				CVertex VectorUnitario2D2= GetUnitVector2D(v3, v4);

				dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D2 );

				if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
				if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
				assert( !_isnan(dProductoEscalar) );

				angulo1=acos(dProductoEscalar);
				assert(!_isnan(angulo1));

				if (std::fabs(angulo1)>(NUMBER_PI/2)) angulo1 = std::fabs(angulo1-NUMBER_PI);

				if ((std::fabs(v1.x-v4.x)<Accuracy) && (std::fabs(v1.y-v4.y)<Accuracy) &&
					(std::fabs(v1.z-v4.z)<Accuracy)) (angulo3=0);
				else{
					CVertex VectorUnitario2D3= GetUnitVector2D(v1, v4);
					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D3 );

					if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
					if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
					assert( !_isnan(dProductoEscalar) );

					angulo3=acos(dProductoEscalar);
					assert( !_isnan(angulo3) );
					if (std::fabs(angulo3)>(NUMBER_PI/2)) angulo3 = std::fabs(angulo3-NUMBER_PI);
				}
				if ((std::fabs(v1.x-v3.x)<Accuracy) && (std::fabs(v1.y-v3.y)<Accuracy) &&
					(std::fabs(v1.z-v3.z)<Accuracy)) (angulo4=0);
				else{
					CVertex VectorUnitario2D4= GetUnitVector2D(v1, v3);
					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D4 );

					if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
					if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
					assert( !_isnan(dProductoEscalar) );

					angulo4=acos(dProductoEscalar);
					assert( !_isnan(angulo4) );
					if (std::fabs(angulo4)>(NUMBER_PI/2)) angulo4 = std::fabs(angulo4-NUMBER_PI);
				}
				if ((std::fabs(v2.x-v4.x)<Accuracy) && (std::fabs(v2.y-v4.y)<Accuracy) &&
					(std::fabs(v2.z-v4.z)<Accuracy)) (angulo5=0);
				else{
					CVertex VectorUnitario2D5= GetUnitVector2D(v2, v4);
					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D5 );

					if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
					if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
					assert(!_isnan(dProductoEscalar));

					angulo5=acos(dProductoEscalar);
					assert( !_isnan(angulo5) );
					if (std::fabs(angulo5)>(NUMBER_PI/2)) angulo5 = std::fabs(angulo5-NUMBER_PI);
				}
				if ((std::fabs(v2.x-v3.x)<Accuracy) && (std::fabs(v2.y-v3.y)<Accuracy) &&
					(std::fabs(v2.z-v3.z)<Accuracy)) (angulo6=0);
				else{
					CVertex VectorUnitario2D6= GetUnitVector2D(v2, v3);
					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D6 );

					if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
					if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
					assert( !_isnan(dProductoEscalar) );

					angulo6=acos(dProductoEscalar);
					assert( !_isnan(angulo6) );
					if (std::fabs(angulo6)>(NUMBER_PI/2)) angulo6 = std::fabs(angulo6-NUMBER_PI);
				}
				angulo2= max(angulo3, angulo4);
				angulo2= max(angulo2, angulo5);
				angulo2= max(angulo2, angulo6);

				assert( !_isnan(angulo2) );
				w = CalculaUab( 0, ReferApp.In.InflationAngleThreshold, (angulo2+angulo1));
			}
			else w=0;

			if(w >Accuracy){
				CompartenCara=false;
				for(long k=0; k<DDBB->GetSizeFaces(); k++){
					if((DDBB->IsEdgeInFace(k,i)) && (DDBB->IsEdgeInFace(k,j))){
						CompartenCara=true;
						break;
					}
				}
				if (CompartenCara==true){
					Weights_LineCollinearity[i][j]= w; 
					Weights_LineCollinearity[j][i]= w;
				}
				else{
					Weights_LineCollinearity[j][i]= 0.0;
					Weights_LineCollinearity[i][j]= 0.0;
				}
			}
			else{
				Weights_LineCollinearity[j][i]= 0.0;
				Weights_LineCollinearity[i][j]= 0.0;
			}
		}
	}
}


void GetWeights_LineOrthogonality(CDB_Model *DDBB,
								  std::vector<std::vector<double>> &WeightsLO)
//Assign weights to pairs of lines with the following criterion:
//	- 1 if both lines run nearly parallel to any of the three main directions 
//	  (which must have been calculated in advance)
//	- 0 Otherwise

//Te weights are stored in a matrix of numVertices x numVertices:
//	- If lines i and j are both parallel to any of the main axes, weight(i,j)= 1
//	- Otherwise weight(i,j)= 0
{
	//Weights are reset to zeros
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		for (long j=0; j<DDBB->GetSizeEdges(); j++){
			WeightsLO[i][j]=0;
			WeightsLO[j][i]=0;
		}
	}

	if(ReferApp.Cu.MA.MainAngles.size() == 0){
		//Main Angles must have been calculated in advance
		return;
	}

	//Visit all the edges of the model
	for (long i=0; i<DDBB->GetSizeEdges(); i++){

		//Calculate the angle of the edge
		CEdge Arista= DDBB->GetEdge(i);
		double Angulo1= DDBB->GetAngle_2PI(i, Arista.Head);
		double Angulo11= 2.*NUMBER_PI - Angulo1;

		//Check if the angle is one of the main directions
		bool found= false;
		long jj=0;
		while (jj<(long)ReferApp.Cu.MA.MainAngles.size() && !found){
			if (std::fabs(Angulo1 - ReferApp.Cu.MA.MainAngles[jj]) < ReferApp.In.InflationAngleThreshold ||
				std::fabs(Angulo11 - ReferApp.Cu.MA.MainAngles[jj]) < ReferApp.In.InflationAngleThreshold)
				found= true;
			jj++;
		}

		if (!found){
			//If it is not a main address, put a weight of 0 
			//in the elements of the matrix where the i edge is
			//for (long j=0; j<DDBB->GetSizeEdges(); j++){
			//	WeightsLO[i][j]=0;
			//	WeightsLO[j][i]=0;
			//}
		}
		else{
			//If it is a main address 
			//check if the other edges are main directions
			for (long j=0; j<DDBB->GetSizeEdges(); j++){
				//Calculate the angle of the edge
				CEdge AristaJ= DDBB->GetEdge(j);
				double Angulo2= DDBB->GetAngle_2PI(j, AristaJ.Head);
				double Angulo22= 2.*NUMBER_PI - Angulo2;

				//Check if the angle is one of the principal directions
				found= false;
				long k=0;
				while (k<(long)ReferApp.Cu.MA.MainAngles.size() && !found){
					if (std::fabs(Angulo1-ReferApp.Cu.MA.MainAngles[k]) < ReferApp.In.InflationAngleThreshold ||
						std::fabs(Angulo22 - ReferApp.Cu.MA.MainAngles[k]) < ReferApp.In.InflationAngleThreshold)
						found= true;
					k++;
				}

				if (!found){
					//If it is not the main address 
					//put a weight of 0 on the elements of the matrix where the i edge is
					//WeightsLO[i][j]=0;
					//WeightsLO[j][i]=0;
				}
				else{
					//If it is main address 
					//put a weight of one to the pair of edges
					WeightsLO[i][j]=1;
					WeightsLO[j][i]=1;
				}
			}
		}
	}
}


double GetCost_LineParallelism(std::vector<double> &Solucion, CDB_Model *DDBB)
// Gets the cost for Line Parallelism

//Lines that are parallel or nearly parallel in the drawing 
//(which are assigned a non-zero parallelism weight) 
//must be parallel in the model, so the less parallel they are in the model, 
//the more the parallelism cost increases.
{
	// Get weights for line parallelism
	std::vector<std::vector<double>> Weights_LineParallelism(DDBB->GetSizeEdges(),
											   std::vector<double>(DDBB->GetSizeEdges()));
	GetWeights_LineParallelism(DDBB, Weights_LineParallelism);

	double dSumatorioParalelismo = 0;

	// Comprueba el paralelismo de cada arista con todas las demas
	for(long i=0; i<DDBB->GetSizeEdges(); i++ ){
		CEdge Arista1= DDBB->GetEdge(i);

		CVertex HeadVertex1= DDBB->GetVertex(Arista1.Head);
		HeadVertex1.z = Solucion[Arista1.Head];
		CVertex TailVertex1= DDBB->GetVertex(Arista1.Tail);
		TailVertex1.z = Solucion[Arista1.Tail];

		CVertex VectorUnitario3D1= GetUnitVector3D(HeadVertex1, TailVertex1);

		//Dada la arista 'Arista1', comprueba su paralelismo con todas las demas.
		for(long j=i+1; j<DDBB->GetSizeEdges(); j++ ) {
			if(Weights_LineParallelism[i][j] >0.){
				CEdge Arista2= DDBB->GetEdge(j);

				CVertex HeadVertex2= DDBB->GetVertex(Arista2.Head);
				HeadVertex2.z = Solucion[Arista2.Head];
				CVertex TailVertex2= DDBB->GetVertex(Arista2.Tail);
				TailVertex2.z = Solucion[Arista2.Tail];

				CVertex VectorUnitario3D2= GetUnitVector3D(HeadVertex2, TailVertex2);

				double dProductoEscalar = DOT_PRODUCT_3D( VectorUnitario3D1, VectorUnitario3D2 );
				assert( !_isnan(dProductoEscalar) );
				if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) 
					dProductoEscalar=1;
				if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) 
					dProductoEscalar=-1;

				double dAnguloRadianes = acos( dProductoEscalar );
				if (std::fabs(dAnguloRadianes)>(NUMBER_PI/2))
					dAnguloRadianes = std::fabs(dAnguloRadianes-NUMBER_PI);

				dSumatorioParalelismo += std::fabs(( Weights_LineParallelism[i][j] *
													(dAnguloRadianes*dAnguloRadianes)));
				assert( !_isnan(dSumatorioParalelismo) );
			}
		};
	}
	assert( !_isnan(dSumatorioParalelismo) );
	return dSumatorioParalelismo;
}


double GetCost_LineCollinearity(std::vector<double> &Solucion, CDB_Model *DDBB)
{
	//Gets the weights for line collinearity
	std::vector<std::vector<double>> Weights_LineCollinearity(DDBB->GetSizeEdges(),
															  std::vector<double>(DDBB->GetSizeEdges()));
	GetWeights_LineCollinearity(DDBB, Weights_LineCollinearity);

	bool CompartenCara=false;
	double angulo1, angulo2, angulo3, angulo4, angulo5, angulo6, dProductoEscalar;
	double dDato1, dDato2;

	double dSumatorioColinealidad= 0;

	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CEdge Arista1= DDBB->GetEdge(i);
		CVertex v1= DDBB->GetVertex(Arista1.Head);
		CVertex v2= DDBB->GetVertex(Arista1.Tail);
		dDato1= Solucion[Arista1.Head];
		dDato2= Solucion[Arista1.Tail];
		v1.z= dDato1;
		v2.z= dDato2;

		//v1 and v2 are the vertices of the first edge to compare
		if (v1.x > v2.x){
			CVertex vaux1;
			vaux1= v1;
			v1= v2;
			v2= vaux1;
		}

		for (long j=i+1; j<DDBB->GetSizeEdges(); j++) {
			assert( !_isnan(Weights_LineCollinearity[i][j]) );
			
			if(Weights_LineCollinearity[i][j]> PrecisionAna){

				CEdge Arista2= DDBB->GetEdge(j);

				//v3 and v4 are the vertices of the second edge to compare
				CVertex v3= DDBB->GetVertex(Arista2.Head);
				CVertex v4= DDBB->GetVertex(Arista2.Tail);
				dDato1= Solucion[Arista2.Head];
				dDato2= Solucion[Arista2.Tail];
				v3.z= dDato1;
				v4.z= dDato2;

				if (v3.x > v4.x){
					CVertex vaux2;
					vaux2= v3;
					v3= v4;
					v4=vaux2;
				}

				int es;
				if (v1.x < v3.x) es=1;
				if (v1.x >v3.x) es=2;
				if (v1.x == v3.x) es=0;
				bool bien=false;
				switch (es){
				case 1:
					if (v2.x <=v4.x) bien=true;
					break;
				case 2:
					if (v4.x<=v1.x) bien=true;
					break;
				case 0:
					//este caso es si las lineas son verticales
					if (v1.y<v2.y){
						if (v3.y<v4.y){
							if (v2.y<=v3.y)
								bien=true;
						}
						else{
							if (v2.y<=v4.y)
								bien=true;
						}
					}
					else{
						if (v3.y<v4.y){
							if (v1.y<=v3.y)
								bien=true;
						}
						else{
							if (v1.y<=v4.y)
								bien=true;
						}
					}
				}

				if (bien){
					CVertex VectorUnitario3D1 = GetUnitVector3D(v1,v2);
					CVertex VectorUnitario3D2 = GetUnitVector3D(v3,v4);
			
					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario3D1, VectorUnitario3D2 );
					assert( !_isnan(dProductoEscalar) );

					if (dProductoEscalar < 1+PrecisionAna && dProductoEscalar > 1) dProductoEscalar=1;
					if (dProductoEscalar < -1 && dProductoEscalar > -1-PrecisionAna) dProductoEscalar=-1;
					assert( !_isnan(dProductoEscalar) );

					angulo1=acos(dProductoEscalar);
					assert( !_isnan(angulo1) );	
					if (std::fabs(angulo1)>(NUMBER_PI/2)) angulo1 = std::fabs(angulo1-NUMBER_PI);

					if ((std::fabs(v1.x-v4.x)<PrecisionAna) && (std::fabs(v1.y-v4.y)<PrecisionAna) &&
						(std::fabs(v1.z-v4.z)<PrecisionAna))	(angulo3=0);
					else{
						CVertex VectorUnitario3D3= GetUnitVector3D(v1,v4);
						dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario3D1, VectorUnitario3D3 );
						if (dProductoEscalar < 1+PrecisionAna && dProductoEscalar > 1) dProductoEscalar=1;
						if (dProductoEscalar < -1 && dProductoEscalar > -1-PrecisionAna) dProductoEscalar=-1;
						assert( !_isnan(dProductoEscalar) );
	
						angulo3=acos(dProductoEscalar);
						assert( !_isnan(angulo3) );	
						if (std::fabs(angulo3)>(NUMBER_PI/2)) angulo3 = std::fabs(angulo3-NUMBER_PI);
					}
					if ((std::fabs(v1.x-v3.x)<PrecisionAna) && (std::fabs(v1.y-v3.y)<PrecisionAna) &&
						(std::fabs(v1.z-v3.z)<PrecisionAna))	(angulo4=0);
					else{
						CVertex VectorUnitario3D4= GetUnitVector3D(v1,v3);
						dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario3D1, VectorUnitario3D4 );

						if (dProductoEscalar < 1+PrecisionAna && dProductoEscalar > 1) dProductoEscalar=1;
						if (dProductoEscalar < -1 && dProductoEscalar > -1-PrecisionAna) dProductoEscalar=-1;
						assert( !_isnan(dProductoEscalar) );

						angulo4=acos(dProductoEscalar);
						assert( !_isnan(angulo4) );	
						if (std::fabs(angulo4)>(NUMBER_PI/2)) angulo4 = std::fabs(angulo4-NUMBER_PI);
					}
					if ((std::fabs(v2.x-v4.x)<PrecisionAna) && (std::fabs(v2.y-v4.y)<PrecisionAna) &&
						(std::fabs(v2.z-v4.z)<PrecisionAna))	(angulo5=0);
					else{
						CVertex VectorUnitario3D5= GetUnitVector3D(v2,v4);
						dProductoEscalar = DOT_PRODUCT_2D(VectorUnitario3D1, VectorUnitario3D5);

						if (dProductoEscalar < 1+PrecisionAna && dProductoEscalar > 1) dProductoEscalar=1;
						if (dProductoEscalar < -1 && dProductoEscalar > -1-PrecisionAna) dProductoEscalar=-1;
						assert( !_isnan(dProductoEscalar) );

						angulo5=acos(dProductoEscalar);
						assert( !_isnan(angulo5) );	
						if (std::fabs(angulo5)>(NUMBER_PI/2)) angulo5 = std::fabs(angulo5-NUMBER_PI);
					}
					if ((std::fabs(v2.x-v3.x)<PrecisionAna) &&
						(std::fabs(v2.y-v3.y)<PrecisionAna) &&
						(std::fabs(v2.z-v3.z)<PrecisionAna))
						(angulo6=0);
					else{
						CVertex VectorUnitario3D6= GetUnitVector3D(v2,v3);
						dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario3D1, VectorUnitario3D6 );

						if (dProductoEscalar < 1+PrecisionAna && dProductoEscalar > 1) dProductoEscalar=1;
						if (dProductoEscalar < -1 && dProductoEscalar > -1-PrecisionAna) dProductoEscalar=-1;
						assert( !_isnan(dProductoEscalar) );

						angulo6=acos(dProductoEscalar);
						assert( !_isnan(angulo6) );	
						if (std::fabs(angulo6)>(NUMBER_PI/2)) angulo6 = std::fabs(angulo6-NUMBER_PI);
					}
					angulo2= max(angulo3, angulo4);
					angulo2= max(angulo2, angulo5);
					angulo2= max(angulo2, angulo6);

					assert( !_isnan(angulo2) );
				}
				
				dSumatorioColinealidad += std::fabs(( Weights_LineCollinearity[i][j] * (angulo1+angulo2)*(angulo1+angulo2)));
				assert( !_isnan(dSumatorioColinealidad) );
			}
		}
	}
	return dSumatorioColinealidad;
}


double GetCost_LineOrthogonality(std::vector<double> &Solucion, CDB_Model *DDBB)
//Calculates a cost that is zero if the pairs of lines of the model are orthogonal to each other, 
//and increases the more pairs of lines that are non-orthogonal.

//Only pairs of lines that share a common vertex
//and run nearly parallel to any of the three main directions are considered

// Note that main directions must have been calculated in advance
{
	//Get weights related to line orthogonality
	//(Weight is 1 for lines parallel to main axes, and 0 otherwise)
	std::vector<std::vector<double>> WeightsLO(DDBB->GetSizeEdges(),
											   std::vector<double>(DDBB->GetSizeEdges()));
	GetWeights_LineOrthogonality(DDBB, WeightsLO);

	double v1x, v1y, v1z, v2x, v2y, v2z, v3x, v3y, v3z, v4x, v4y, v4z; 
	double CostLineOrthogonality = 0;

	//Visit all the vertices
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		std::vector<long> EdgesInVertexI= DDBB->GetAllEdgesInVertex(i);

		//Calculate angles for pairs of lines that share the current vertex
		for (long j=0; j<(long)EdgesInVertexI.size(); j++){
			for (long k=j+1; k<(long)EdgesInVertexI.size(); k++){
				//Only pairs with non-zero weight are candidate to produce cost
				if (WeightsLO[EdgesInVertexI[j]][EdgesInVertexI[k]] != 0){
					CEdge Arista= DDBB->GetEdge(EdgesInVertexI[j]);

					CVertex HeadVertex, TailVertex;
					if (Arista.Head==i){
						HeadVertex= DDBB->GetVertex(Arista.Head);
						TailVertex= DDBB->GetVertex(Arista.Tail);
						v1z= Solucion[Arista.Head];
						v2z= Solucion[Arista.Tail];
					}
					else{
						HeadVertex= DDBB->GetVertex(Arista.Tail);
						TailVertex= DDBB->GetVertex(Arista.Head);
						v1z= Solucion[Arista.Tail];
						v2z= Solucion[Arista.Head];
					}
					v1x=HeadVertex.x;
					v1y=HeadVertex.y;
					v2x=TailVertex.x;
					v2y=TailVertex.y;

					Arista= DDBB->GetEdge(EdgesInVertexI[k]);

					if (Arista.Head==i){
						HeadVertex= DDBB->GetVertex(Arista.Head);
						TailVertex= DDBB->GetVertex(Arista.Tail);
						v3z= Solucion[Arista.Head];
						v4z= Solucion[Arista.Tail];
					}
					else{
						HeadVertex= DDBB->GetVertex(Arista.Tail);
						TailVertex= DDBB->GetVertex(Arista.Head);
						v3z= Solucion[Arista.Tail];
						v4z= Solucion[Arista.Head];
					}
					v3x=HeadVertex.x;
					v3y=HeadVertex.y; 
					v4x=TailVertex.x; 
					v4y=TailVertex.y;

					if (!NearlyCollinear(v1x, v1y, v2x, v2y, v3x, v3y, v4x, v4y,
										 DEGREES_TO_RADIANS(5), 0.10)){
						//Only lines that are not nearly parallel to each other
						double Modulo1= MODULE3D(v1x, v1y, v1z, v2x, v2y, v2z);
						double Modulo2= MODULE3D(v3x, v3y, v3z, v4x, v4y, v4z);
						double aux= ((v2x-v1x)/Modulo1)*((v4x-v3x)/Modulo2) +
									((v2y-v1y)/Modulo1)*((v4y-v3y)/Modulo2) +
									((v2z-v1z)/Modulo1)*((v4z-v3z)/Modulo2);
						aux= asin(aux);
						assert(std::fabs(aux)<=NUMBER_PI);
						//CostLineOrthogonality += WeightsLO[EdgesInVertexI[j]][EdgesInVertexI[k]]*aux*aux;
						CostLineOrthogonality += aux*aux;
					}
				}
			}
		}
	}
	return CostLineOrthogonality;
}


double GetCost_LineVerticallity(std::vector<double> &Solucion, CDB_Model *DDBB)
{
	double Coseno2D, Coseno3D, AnguloVertical2D, AnguloVertical3D, dPesoW12;

	double Verticalidad = 0;

	for (long i=0; i<DDBB->GetSizeEdges(); i++) {
		// determinar el paralelismo de una arista con respecto al eje Y
		// en 2d es cuestion de dividir la coordenada Y de la arista 2d entre
		// su módulo, si este cociente es 1 quiere decir que es paralela y
		// por tanto el acos es máximo, pi/2. Lo mismo ocurre con la
		// reconstrucción 3d.

		CEdge Arista= DDBB->GetEdge(i);
		CVertex HeadVertex= DDBB->GetVertex(Arista.Head);
		CVertex TailVertex= DDBB->GetVertex(Arista.Tail);
		HeadVertex.z= Solucion[Arista.Head];
		TailVertex.z= Solucion[Arista.Tail];

		//Tomo valor absoluto para asegurar angulos positivos en el calculo del
		//acoseno
		Coseno2D=std::fabs(HeadVertex.y - TailVertex.y)/DDBB->GetLength2D(i);
		if (Coseno2D < 1+Accuracy && Coseno2D > 1) Coseno2D=1;
		if (Coseno2D < -1 && Coseno2D > -1-Accuracy) Coseno2D=-1;
		AnguloVertical2D = acos(Coseno2D);

		Coseno3D =std::fabs(HeadVertex.y - TailVertex.y)/DDBB->GetLength3D(i);
		if (Coseno3D < 1+Accuracy && Coseno3D > 1) Coseno3D=1;
		if (Coseno3D < -1 && Coseno3D > -1-Accuracy) Coseno3D=-1;
		AnguloVertical3D = acos(Coseno3D);

		dPesoW12 = CalculaUab(0, /*7 degrees*/ 0.1221730476396, AnguloVertical2D);
		Verticalidad+=dPesoW12*AnguloVertical3D*AnguloVertical3D;
	}
	return Verticalidad;
}


double GetCost_Isometry(std::vector<double> &Solucion, CDB_Model *DDBB)
{
	if (DDBB->GetSizeEdges()<2)
		return 0;

	double Longitud2D;
	std::vector<double> ri;
	for (long i=0; i<DDBB->GetSizeEdges(); i++) {
		CEdge Arista= DDBB->GetEdge(i);
		CVertex HeadVertex= DDBB->GetVertex(Arista.Head);
		CVertex TailVertex= DDBB->GetVertex(Arista.Tail);
		HeadVertex.z= Solucion[Arista.Head];
		TailVertex.z= Solucion[Arista.Tail];

		if ((Longitud2D = DDBB->GetLength2D(i))!=0)
			ri.push_back(DDBB->GetLength3D(i)/Longitud2D);
		else
			ri.push_back(0);
	}

	double media=0;
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		media+= ri[i];
	}
	media/= DDBB->GetSizeEdges();

	double Varianza=0;
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		Varianza+=(ri[i]-media)*(ri[i]-media);
	}
	Varianza=Varianza*DDBB->GetSizeEdges()/(DDBB->GetSizeEdges()-1);

	return Varianza;
}


//ANA++ Esta función la creo aqui a mi manera porque la de Regulari no hay quien la entienda
//double GetCost_LineOrthogonality(TListaDouble *pSolucion,TDBEntidades *pDB)

// Devuelve el coste de la 'pSolucion' teniendo en cuenta la Base de Datos 'pDB' y la
//	siguiente regularidad:
//	ORTOGONALIDAD DE LINEAS
//{
//	long i, j, k, IndiceArista;
//	CVertex Union1,
//		HeadVertex1, TailVertex1,
//	    HeadVertex2, TailVertex2,
//		VectorUnitario3D1, VectorUnitario3D2;
//	CEdge Arista, Arista1, Arista2;
//	CEdgesCollection ListaAristasEnUnion;
//	double dDato, dAnguloRadianes, dAngulo,
// 		   dSumatorioOrtogonalidad, dProductoEscalar, dTotalOrtogonalidad;
//
//	dTotalOrtogonalidad = 0;
//	dProductoEscalar = 0;
////AQUI TENGO QUE EXCLUIR DE LA COMPARACION TODAS AQUELLAS QUE SEAN COLINEALES.
//	// Comprueba el paralelismo de cada arista con todas las demas
//
//	for (i=0; i<TamanyoListaVertices(&pDB->ListaVertices); i++){
//		ObtenListaVertices(&pDB->ListaVertices, i, &Union1);
//
//		dSumatorioOrtogonalidad =0;
//		NuevaListaAristas(&ListaAristasEnUnion);
//
//		for(j=0; j<TamanyoListaLong(&Union1.ListaAristas); j++){
//			ObtenListaLong(&Union1.ListaAristas, j, &IndiceArista); 
//			ObtenListaAristas(&pDB->ListaAristas, IndiceArista, &Arista);
//			AnyadeListaAristas(&ListaAristasEnUnion, &Arista);
//		}
//
//		for(j=0; j<TamanyoListaLong(&Union1.ListaAristas); j++){
//			ObtenListaAristas(&ListaAristasEnUnion, j, &Arista1);
//			
//			NuevoVertice(&HeadVertex1);
//			ObtenListaVertices(&pDB->ListaVertices, Arista1.Head, &HeadVertex1);
//			ObtenListaDouble( pSolucion, Arista1.Head, &dDato );
//			HeadVertex1.z = dDato;
//
//			NuevoVertice(&TailVertex1);
//			ObtenListaVertices(&pDB->ListaVertices, Arista1.Tail, &TailVertex1);
//			ObtenListaDouble( pSolucion, Arista1.Tail, &dDato );
//			TailVertex1.z = dDato;
//
//			for(k=j+1; k<TamanyoListaLong(&Union1.ListaAristas); k++){
//				ObtenListaAristas(&ListaAristasEnUnion, k, &Arista2);
//				
//				NuevoVertice(&HeadVertex2);
//				ObtenListaVertices(&pDB->ListaVertices, Arista2.Head, &HeadVertex2);
//				ObtenListaDouble( pSolucion, Arista2.Head, &dDato );
//				HeadVertex2.z = dDato;
//				
//				NuevoVertice(&TailVertex2);
//				ObtenListaVertices(&pDB->ListaVertices, Arista2.Tail, &TailVertex2);
//				ObtenListaDouble( pSolucion, Arista2.Head, &dDato );
//				HeadVertex2.z = dDato;
//
//				if (Colineal(HeadVertex1.x, HeadVertex1.y, TailVertex1.x, TailVertex1.y, 
//					HeadVertex2.x, HeadVertex2.y, TailVertex2.x, TailVertex2.y, 99.0))
//					break;
//				else{
//					CVertex VectorUnitario3D1 = GetUnitVector3D( &HeadVertex1, &TailVertex1 );
//					CVertex VectorUnitario3D2 = GetUnitVector3D( &HeadVertex2, &TailVertex2 );
//					
//					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario3D1, VectorUnitario3D2 );                                                           
//					assert( !_isnan(dProductoEscalar) );
//
//					DestruyeVertice(&VectorUnitario3D1);
//					DestruyeVertice(&VectorUnitario3D2);
//
//					//El angulo que se calcula aqui, es el angulo complementario (fi-90) del que forman los dos 
//					//vectores unitarios
//					dAnguloRadianes = (asin(dProductoEscalar));
//					assert(dAnguloRadianes>=-pi/2 && dAnguloRadianes<= pi/2);
//
//					dSumatorioOrtogonalidad += (std::fabs(dAnguloRadianes*dAnguloRadianes));
//					assert( !_isnan(dSumatorioOrtogonalidad) );
//				}
//				DestruyeVertice( &HeadVertex2 );
//				DestruyeVertice( &TailVertex2 );
//			};
//			DestruyeVertice( &HeadVertex1 );
//			DestruyeVertice( &TailVertex1 );
//		}
//		dTotalOrtogonalidad +=dSumatorioOrtogonalidad;
//	}
//	assert( !_isnan(dTotalOrtogonalidad) );
//	return dTotalOrtogonalidad;
//}


double GetCost_FacePlanarity(std::vector<double> &Solucion, CDB_Model *DDBB)
{
	long IndiceArista, IndiceVertice, Aux;
	double e11=0,e12=0,e13=0,
			e21=0,e22=0,e23=0,
			e31=0,e32=0,e33=0,
			ind1=0, ind2=0, ind3=0,
			Z;
	double a, b, c, d, denominador;
	double Coste, aux;
	CEdge Arista;
	std::vector<long> VerticesEnCara;

	Coste=0;

	//Para cada cara saco las aristas que la forman
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		CFace CaraI= DDBB->GetFace(i);

		//Obtengo todos los vértices de la cara
		VerticesEnCara.clear();
		for (long j=0; j<CaraI.GetSizeEdgesInFace(); j++){
			IndiceArista= CaraI.GetEdgeInFace(j);
			Arista= DDBB->GetEdge(IndiceArista);
			if (!IsValueInVector(Arista.Head, VerticesEnCara, Aux))
				VerticesEnCara.push_back(Arista.Head);
			if (!IsValueInVector(Arista.Tail, VerticesEnCara, Aux))
				VerticesEnCara.push_back(Arista.Tail);
		}
		//Calculo los coeficientes de la matriz
		e11=0; e12=0; e13=0;
		e21=0; e22=0; e23=0;
		e31=0; e32=0; e33=0;
		ind1=0; ind2=0; ind3=0;

		for (long j=0; j<(long)VerticesEnCara.size(); j++){
			IndiceVertice= VerticesEnCara[j];
			CVertex Vertice= DDBB->GetVertex(IndiceVertice);
			Z= Solucion[IndiceVertice];
			e11+=Vertice.x * Vertice.x;
			e12+=Vertice.x * Vertice.y;
			e13+=Vertice.x * Z;
			e21+=Vertice.x * Vertice.y;
			e22+=Vertice.y * Vertice.y;
			e23+=Vertice.y * Z;
			e31+=Vertice.x * Z;
			e32+=Vertice.y * Z;
			e33+=Z * Z;
			ind1+=Vertice.x;
			ind2+=Vertice.y;
			ind3+=Z;
		}

		//Calculo los coeficientes de la ecuación del plano
		denominador=det(e11,e12,e13,e21,e22,e23,e31,e32,e33);
		if (std::fabs(denominador)>0.001){
			a=det(ind1,e12,e13,ind2,e22,e23,ind3,e32,e33)/denominador;
			b=det(e11,ind1,e13,e12,ind2,e23,e31,ind3,e33)/denominador;
			c=det(e11,e12,ind1,e21,e22,ind2,e31,e32,ind3)/denominador;

			//Normalizo los coeficientes
			aux = sqrt(a*a + b*b + c*c);
			a=a/aux;
			b=b/aux;
			c=c/aux;
			d=-1/aux;

			//Calculo la distancia de cada vértice al plano
			for (long j=0; j<(long)VerticesEnCara.size(); j++){
				IndiceVertice= VerticesEnCara[j];
				CVertex Vertice= DDBB->GetVertex(IndiceVertice);
				Z= Solucion[IndiceVertice];
				Coste+=std::fabs(a*Vertice.x+b*Vertice.y+c*Z+d);
			}
		}
	}

	return Coste;


//	long TamVer=pGrafo->ListaVertices.lTamanyo,
//         TamAri=pGrafo->ListaAristas.lTamanyo,
//         TamCar=pGrafo->CFaces.lTamanyo,tari,i,j,n,v1,v2;
//    char *presente;
//    CVertex *Ver=pGrafo->ListaVertices.aLista,V1,V2;
//    CEdge *Ari=pGrafo->ListaAristas.aLista;
//    CFace *Cara=pGrafo->CFaces.aLista;
//    long *arista;
//    
//	//los eij almacenan cada valor de los coeficientes del det, y i almacena
//    //terminos independientes
//    double aux1=0,aux2=0,aux3=0,e11=0,e12=0,e13=0,e21=0,e22=0,e23=0,
//	       e31=0,e32=0,e33=0,i1=0,i2=0,i3=0,a,b,c,d,norm,denominador,sum=0;
//    //para cada cara se ejecuta la regularidad y se suman los distintos resultados 
//    for (i=0;i<TamCar;i++) {
//		 double medx=0,medy=0,medz=0;
//         e11=0; e12=0; e13=0; e21=0; e22=0; e23=0;
//	     e31=0; e32=0; e33=0; i1=0; i2=0; i3=0;
//        
//		 //marca los vertices ya usados
//         presente=(char*)malloc(TamVer*sizeof(char));
//         for (n=0;n<TamVer;n++) presente[n]=0;
//         tari=Cara[i].ListaAristas.lTamanyo;
//         //para cada arista de la cara:
//         for (j=0;j<tari;j++){
//			  arista=Cara[i].ListaAristas.aLista;
//              v1=Ari[arista[j]].Head;
//              v2=Ari[arista[j]].Tail;
//              V1=Ver[v1];
//              V2=Ver[v2];
//              //evita que se repitan vertices de una cara
//              if (!presente[v1]) {
//                 presente[v1]=1;
//				 aux1=Ver[v1].y*pSolucion->aLista[v1];
//                 aux2=Ver[v1].x*Ver[v1].y;
//                 aux3=Ver[v1].x*pSolucion->aLista[v1];
//                 //se calculan los sumatorios de los elementos del determinante
//                 e11=e11+cuad(Ver[v1].x);  e12=aux2+e12;  e13=aux3+e13;
//                 e21=aux2+e21;  e22=e22+cuad(Ver[v1].y);  e23=aux1+e23;
//                 e31=aux3+e31;  e32=aux1+e32;  e33=e33+cuad(pSolucion->aLista[v1]);
//                 i1=Ver[v1].x+i1;  i2=Ver[v1].y+i2;  i3=pSolucion->aLista[v1]+i3;
//				 medx=medx+V1.x;
//				 medy=medy+V1.y;	
//				 medz=medz+V1.z;	
//			  }
//
//              if (!presente[v2]) {
//				  presente[v2]=1;
//
//                  aux1=Ver[v2].y*pSolucion->aLista[v2];
//                  aux2=Ver[v2].x*Ver[v2].y;
//                  aux3=Ver[v2].x*pSolucion->aLista[v2];
//
//                  e11=e11+cuad(Ver[v2].x);  e12=aux2+e12;  e13=aux3+e13;
//                  e21=aux2+e21;  e22=e22+cuad(Ver[v2].y);  e23=aux1+e23;
//                  e31=aux3+e31;  e32=aux1+e32;  e33=e33+cuad(pSolucion->aLista[v2]);
//                  i1=Ver[v2].x+i1;  i2=Ver[v2].y+i2;  i3=pSolucion->aLista[v2]+i3;
//				  medx=medx+V2.x;
//				  medy=medy+V2.y;	
//				  medz=medz+V2.z;	
//			  }
//		 }
//
//         //se calculan los coeficientes de la ecuacion del plano, si
//         //el determinante es igual a cero no se calculara el factor
//         //correspondiente a esa cara
//		 denominador=det(e11,e12,e13,e21,e22,e23,e31,e32,e33);
//         if (denominador!=0){
//			 a=det(i1,e12,e13,i2,e22,e23,i3,e32,e33)/denominador;
//             b=det(e11,i1,e13,e12,i2,e23,e31,i3,e33)/denominador;
//             c=det(e11,e12,i1,e21,e22,i2,e31,e32,i3)/denominador;
//			 medx=medx/tari;
//			 medy=medy/tari;
//			 medz=medz/tari;
//             //se normalizan los coeficientes
//             norm=sqrt(cuad(a)+cuad(b)+cuad(c));
//             if (norm==0) return -1;
//             //d=-(a*medx+b*medy+c*medz);
//			 a=a/norm; b=b/norm; c=c/norm; d=-1/norm;
//
//			 tari=Cara[i].ListaAristas.lTamanyo;
//             for (n=0;n<TamVer;n++) presente[n]=0;
//             //se calculan las distancias de cada vertice con repecto al plano
//             for (j=0;j<tari;j++){
//				 arista=Cara[i].ListaAristas.aLista;
//                 v1=Ari[arista[j]].Head;
//                 v2=Ari[arista[j]].Tail;
//                 V1=Ver[v1];
//                 V2=Ver[v2];
//                 if (!presente[v1]){
//					 sum=sum+posit(a*V1.x+b*V1.y+c*V1.z+d);
//                     presente[Ari[j].Head]=1;
//				 }
//
//				 if (!presente[v2]){
//					 sum=sum+posit(a*V2.x+b*V2.y+c*V2.z+d);
//					 presente[Ari[j].Tail]=1;
//				 }
//			 }
//		 }
//		 free(presente);
//	}
//
//	return sum;

}

//-------------------- ortogonalidad de esquinas -----------------------------


double beta_corner_ort(vector3d a1,vector3d a2,vector3d a3)
{
	vector3d v1,v2,v3;
	double valor,max,min;
	max= (double)-0xfffffffffffffff;
	char i,j,k;

	for (i=0;i<=1;i++){
		for (j=0;j<=1;j++){
			for (k=0;k<=1;k++){
				if (i==0){
					v1.v1=a1.v1; v1.v2=a1.v2;
				}
				else{
					v1.v1=a1.v2; v1.v2=a1.v1;
				}
				if (j==0){
					v2.v1=a2.v1; v2.v2=a2.v2;
				}
				else{
					v2.v1=a2.v2; v2.v2=a2.v1;
				}
				if (k==0){
					v3.v1=a3.v1; v3.v2=a3.v2;
				}
				else{
					v3.v1=a3.v2; v3.v2=a3.v1;
				}
				// calcula el producto escalar de los vectores hechos unitarios
				min=prod_esc_2d(v1,v2,1);
				valor=prod_esc_2d(v3,v2,1);
				if (min>valor) min=valor;
				valor=prod_esc_2d(v1,v3,1);
				if (min>valor) min=valor;
				if (max<min) max=min;
			}
		}
	}
	return max;
}


double GetCost_CubicCorners(std::vector<double> &Solucion, CDB_Model *DDBB)
{
	pila_vec3d *pila=NULL;
	vector3d vec,*trio;

	// para cada vertice que tenga tres aristas incidentes se hace:
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		if (DDBB->GetSizeEdgesInVertex(i)==3) {
			trio=(vector3d*)malloc(3*sizeof(vector3d));
			/* para cada arista incidente:  */
			for (long j=0;j<3;j++) {
				//si la cabeza de la arista coincide con el vertice se invierte,
				//de modo que en vec la cabeza sea el vertice contrario al vertice de
				//union, es decir todos los vectores apuntan hacia fuera de la union

				long IndiceArista= DDBB->GetEdgeInVertex(i, j);
				CEdge Arista= DDBB->GetEdge(IndiceArista);

				if (Arista.Tail==i) {
					CVertex Vertice2= DDBB->GetVertex(Arista.Head);
					vec.v2.x=Vertice2.x;
					vec.v2.y=Vertice2.y;
					vec.v2.z=Solucion[Arista.Head];


					Vertice2.Clear();
					Vertice2= DDBB->GetVertex(Arista.Tail);
					vec.v1.x=Vertice2.x;
					vec.v1.y=Vertice2.y;
					vec.v1.z=Solucion[Arista.Tail];
				}
				else {
					CVertex Vertice2= DDBB->GetVertex(Arista.Head);
					vec.v1.x=Vertice2.x;
					vec.v1.y=Vertice2.y;
					vec.v1.z=Solucion[Arista.Head];

					Vertice2.Clear();
					Vertice2= DDBB->GetVertex(Arista.Tail);
					vec.v2.x=Vertice2.x;
					vec.v2.y=Vertice2.y;
					vec.v2.z=Solucion[Arista.Tail];
				}
				/* en este array se almacenan los tres vectores */
				trio[j]=vec;
			}
			/*  luego se apilan los vectores de cada union */
			apilar(&pila,trio,3);
			free(trio);
		}
	}

	double total=0;
	/* para cada esquina se hace:*/

	pila_vec3d *dev;
	while (pila!=NULL) {
		//dev=(vector3d*)malloc(3*sizeof(vector3d));

		dev=desapilar(&pila);
		/* se calcula el sumatorio */

//ANA++/////////////TODO ESTO DEBE ESTAR MAL
		double aux1=asin(prod_esc_3d(dev->dato[0],dev->dato[1],1));
		double aux2=asin(prod_esc_3d(dev->dato[0],dev->dato[2],1));
		double aux3=asin(prod_esc_3d(dev->dato[2],dev->dato[1],1));

	//	while ((std::fabs(aux1))>NUMBER_PI) aux1-=NUMBER_PI;
	//		if (std::fabs(aux1)>(NUMBER_PI/2)) aux1= std::fabs(aux1-NUMBER_PI);

	//	while ((std::fabs(aux2))>NUMBER_PI) aux2-=NUMBER_PI;
	//		if (std::fabs(aux2)>(NUMBER_PI/2)) aux2= std::fabs(aux2-NUMBER_PI);

	//		while ((std::fabs(aux3))>NUMBER_PI) aux3-=NUMBER_PI;
	//			if (std::fabs(aux3)>(NUMBER_PI/2)) aux3= std::fabs(aux3-NUMBER_PI);

		double parcial=aux1*aux1+aux2*aux2+aux3*aux3;

		/* si beta es menor o igual que cero se deja el sumatorio igual, sino se le
		calcula mu */
		if (beta_corner_ort(dev->dato[0],dev->dato[1],dev->dato[2])>0)
			parcial=mu(0,0.1,parcial);

		//free(dev->dato);
		//free(dev);

		total=total+parcial;
	}
	return total;
}

//-------------------------- fin de ortogonalidad de esquinas ----------------


//-------------------- perpendicularidad de caras ----------------------------
bool Comparten_Arista(CDB_Model *DDBB, long i, long j)
//determina si dos caras comparten alguna arista
{
	bool comparten= false;

	CFace CaraJ= DDBB->GetFace(j);
	for (long jj=0; jj<CaraJ.GetSizeEdgesInFace(); jj++){
		long IndiceArista= CaraJ.GetEdgeInFace(jj);
		if (DDBB->IsEdgeInFace(i, IndiceArista)){
			comparten= true;
			break;
		}
	}
	return comparten;
}


double GetCost_FaceOrthogonality(std::vector<double> &Solucion, CDB_Model *DDBB)
{
	long IndiceArista, IndiceVertice, Aux;
	double e11, e12, e13,
			e21, e22, e23,
			e31, e32, e33,
			ind1, ind2, ind3=0;
	double a, b, c, d, aux;
	double Coste, Z,denominador;

	std::vector<long> VVerticesEnCara;

	Coste=0;
	//compruebo si cada par de caras comparte aristas
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		CFace CaraI= DDBB->GetFace(i);


		for (long j=i+1; j<DDBB->GetSizeFaces(); j++){
			CFace CaraJ= DDBB->GetFace(j);


			if (Comparten_Arista(DDBB, i, j)){
				//Si las caras i y j comparten una arista, calculo los planos de mejor
				//ajuste de dichas caras
				
				//Saco las aristas que forman la cara i
				//Obtengo todos los vértices de la cara
				VVerticesEnCara.clear();
				for (long k=0; k<CaraI.GetSizeEdgesInFace(); k++){
					IndiceArista= CaraI.GetEdgeInFace(k);
					CEdge Arista= DDBB->GetEdge(IndiceArista);
					if (!IsValueInVector(Arista.Head, VVerticesEnCara, Aux))
						VVerticesEnCara.push_back(Arista.Head);
					if (!IsValueInVector(Arista.Tail, VVerticesEnCara, Aux))
						VVerticesEnCara.push_back(Arista.Tail);
				}

				//Obtengo el plano de mejor ajuste de la cara i

				//Calculo los coeficientes de la matriz
				e11=0; e12=0; e13=0;
				e21=0; e22=0; e23=0;
				e31=0; e32=0; e33=0;
				ind1=0; ind2=0; ind3=0;

				for (long k=0; k<(long)VVerticesEnCara.size(); k++){
					CVertex Vertice= DDBB->GetVertex(VVerticesEnCara[k]);
					Z= Solucion[IndiceVertice];
					e11+=Vertice.x * Vertice.x;
					e12+=Vertice.x * Vertice.y;
					e13+=Vertice.x * Z;
					e21+=Vertice.x * Vertice.y;
					e22+=Vertice.y * Vertice.y;
					e23+=Vertice.y * Z;
					e31+=Vertice.x * Z;
					e32+=Vertice.y * Z;
					e33+=Z * Z;
					ind1+=Vertice.x;
					ind2+=Vertice.y;
					ind3+=Z;
				}

				//Calculo los coeficientes de la ecuación del plano
				denominador=det(e11,e12,e13,e21,e22,e23,e31,e32,e33);
				if (denominador!=0){
					a=det(ind1,e12,e13,ind2,e22,e23,ind3,e32,e33)/denominador;
					b=det(e11,ind1,e13,e12,ind2,e23,e31,ind3,e33)/denominador;
					c=det(e11,e12,ind1,e21,e22,ind2,e31,e32,ind3)/denominador;
				}
	
				 //Normalizo los coeficientes
				aux = sqrt(a*a + b*b + c*c);
				a=a/aux;
				b=b/aux;
				c=c/aux;
				d=-1/aux;

				//Determino tres puntos del plano i
				CVertex Punto1, Punto2, Punto3;
				Punto1.x=1; Punto1.y=0; Punto1.z=-1*(a*Punto1.x+b*Punto1.y+d)/c;
				Punto2.x=0; Punto2.y=1; Punto2.z=-1*(a*Punto2.x+b*Punto2.y+d)/c;
				Punto3.x=1; Punto3.y=1; Punto3.z=-1*(a*Punto3.x+b*Punto3.y+d)/c;
				//Determino el vector normal al plano
				CVertex v1, v2;
				v1.x = Punto2.x-Punto1.x;
				v1.y = Punto2.y-Punto1.y;
				v1.z = Punto2.z-Punto1.z;
				v2.x = Punto3.x-Punto1.x;
				v2.y = Punto3.y-Punto1.y;
				v2.z = Punto3.z-Punto1.z;

				CVertex Vector1;
				Vector1.x=v1.y*v2.z-v2.y*v1.z;
				Vector1.y=v1.z*v2.x-v1.x*v2.z;
				Vector1.z=v1.x*v2.y-v2.x*v1.y;

				//Repito los mismo para la otra cara
				//Saco las aristas que forman la cara j
				//Obtengo todos los vértices de la cara
				VVerticesEnCara.clear();
				for (long k=0; k<CaraJ.GetSizeEdgesInFace(); k++){
					IndiceArista= CaraJ.GetEdgeInFace(k);
					CEdge Arista= DDBB->GetEdge(IndiceArista);
					if (!IsValueInVector(Arista.Head, VVerticesEnCara, Aux))
						VVerticesEnCara.push_back(Arista.Head);
					if (!IsValueInVector(Arista.Tail, VVerticesEnCara, Aux))
						VVerticesEnCara.push_back(Arista.Tail);
				}

				//Obtengo el plano de mejor ajuste de la cara j

				//Calculo los coeficientes de la matriz
				e11=0; e12=0; e13=0;
				e21=0; e22=0; e23=0;
				e31=0; e32=0; e33=0;
				ind1=0; ind2=0; ind3=0;

				for (long k=0; k<(long)VVerticesEnCara.size(); k++){
					IndiceVertice= VVerticesEnCara[k];
					CVertex Vertice= DDBB->GetVertex(IndiceVertice);
					Z= Solucion[IndiceVertice];
					e11+=Vertice.x * Vertice.x;
					e12+=Vertice.x * Vertice.y;
					e13+=Vertice.x * Z;
					e21+=Vertice.x * Vertice.y;
					e22+=Vertice.y * Vertice.y;
					e23+=Vertice.y * Z;
					e31+=Vertice.x * Z;
					e32+=Vertice.y * Z;
					e33+=Z * Z;
					ind1+=Vertice.x;
					ind2+=Vertice.y;
					ind3+=Z;
				}

				//Calculo los coeficientes de la ecuación del plano
				denominador=det(e11,e12,e13,e21,e22,e23,e31,e32,e33);
				if (denominador!=0){
					a=det(ind1,e12,e13,ind2,e22,e23,ind3,e32,e33)/denominador;
					b=det(e11,ind1,e13,e12,ind2,e23,e31,ind3,e33)/denominador;
					c=det(e11,e12,ind1,e21,e22,ind2,e31,e32,ind3)/denominador;
				}
	
				 //Normalizo los coeficientes
				aux = sqrt(a*a + b*b + c*c);
				a=a/aux;
				b=b/aux;
				c=c/aux;
				d=-1/aux;

				//Determino tres puntos del plano i
				//CVertex Punto1, Punto2, Punto3;
				Punto1.x=1; Punto1.y=0; Punto1.z=-1*(a*Punto1.x+b*Punto1.y+d)/c;
				Punto2.x=0; Punto2.y=1; Punto2.z=-1*(a*Punto2.x+b*Punto2.y+d)/c;
				Punto3.x=1; Punto3.y=1; Punto3.z=-1*(a*Punto3.x+b*Punto3.y+d)/c;
				//Determino el vector normal al plano
				//CVertex v1, v2;
				v1.x = Punto2.x-Punto1.x;
				v1.y = Punto2.y-Punto1.y;
				v1.z = Punto2.z-Punto1.z;
				v2.x = Punto3.x-Punto1.x;
				v2.y = Punto3.y-Punto1.y;
				v2.z = Punto3.z-Punto1.z;

				CVertex Vector2;
				Vector2.x=v1.y*v2.z-v2.y*v1.z;
				Vector2.y=v1.z*v2.x-v1.x*v2.z;
				Vector2.z=v1.x*v2.y-v2.x*v1.y;

				//Incremento el valor del coste debido a este nuevo par de caras
				//Primero normalizo los vectores
				aux = sqrt(Vector1.x*Vector1.x + Vector1.y*Vector1.y + Vector1.z*Vector1.z);
				Vector1.x = Vector1.x/aux; Vector1.y = Vector1.y/aux; Vector1.z = Vector1.z/aux;
				aux = sqrt(Vector2.x*Vector2.x + Vector2.y*Vector2.y + Vector2.z*Vector2.z);
				Vector2.x = Vector2.x/aux; Vector2.y = Vector2.y/aux; Vector2.z = Vector2.z/aux;

				//Ahora calculo el incremento de coste
				aux = Vector1.x*Vector2.x + Vector1.y*Vector2.y + Vector1.z*Vector2.z;
				if (aux>1) aux=1;
				if (aux<-1) aux = -1;
				aux = asin(aux);
				Coste+=aux *aux;
			}
		}
	}

	return Coste;

//	vec3d v1,v2;
//    vert3d *n1,*n2;
//    long *LA1,*LA2;
//    double total,num;
//    long i,j,TamCar=pGrafo->CFaces.lTamanyo;
//    CVertex *Ver=pGrafo->ListaVertices.aLista;
//    CEdge *Ari=pGrafo->ListaAristas.aLista;
//    TListaLong *A;
//
//    for (i=0;i<TamCar;i++){
//		A=&(pGrafo->CFaces.aLista[i].ListaAristas);
//        QuickSort(A->aLista,0,(A->lTamanyo)-1);
//	}
//
//	total=0;
//    //compruebo si cada par de caras comparte aristas
//    for (i=0;i<TamCar;i++){
//		for (j=i+1;j<TamCar;j++){
//			if (Comparten_Arista(DDBB,i,j)){
//				LA1=pGrafo->CFaces.aLista[i].ListaAristas.aLista;
//                LA2=pGrafo->CFaces.aLista[j].ListaAristas.aLista;
//
//                v1.v1.x=Ver[Ari[LA1[0]].Head].x;
//                v1.v1.y=Ver[Ari[LA1[0]].Head].y;
//                v1.v1.z=pSolucion->aLista[Ari[LA1[0]].Head];
//                v1.v2.x=Ver[Ari[LA1[0]].Tail].x;
//                v1.v2.y=Ver[Ari[LA1[0]].Tail].y;
//                v1.v2.z=pSolucion->aLista[Ari[LA1[0]].Tail];
//
//                v2.v1.x=Ver[Ari[LA1[1]].Head].x;
//                v2.v1.y=Ver[Ari[LA1[1]].Head].y;
//                v2.v1.z=pSolucion->aLista[Ari[LA1[1]].Head];
//                v2.v2.x=Ver[Ari[LA1[1]].Tail].x;
//                v2.v2.y=Ver[Ari[LA1[1]].Tail].y;
//                v2.v2.z=pSolucion->aLista[Ari[LA1[1]].Tail];
//                
//				//calculo el vector normal al plano
//                n1=prod_vec_3d(v1,v2);
//
//                v1.v1.x=Ver[Ari[LA2[0]].Head].x;
//                v1.v1.y=Ver[Ari[LA2[0]].Head].y;
//                v1.v1.z=pSolucion->aLista[Ari[LA2[0]].Head];
//                v1.v2.x=Ver[Ari[LA2[0]].Tail].x;
//                v1.v2.y=Ver[Ari[LA2[0]].Tail].y;
//                v1.v2.z=pSolucion->aLista[Ari[LA2[0]].Tail];
//
//                v2.v1.x=Ver[Ari[LA2[1]].Head].x;
//                v2.v1.y=Ver[Ari[LA2[1]].Head].y;
//                v2.v1.z=pSolucion->aLista[Ari[LA2[1]].Head];
//                v2.v2.x=Ver[Ari[LA2[1]].Tail].x;
//                v2.v2.y=Ver[Ari[LA2[1]].Tail].y;
//                v2.v2.z=pSolucion->aLista[Ari[LA2[1]].Tail]; 
//
//                n2=prod_vec_3d(v1,v2);
//                
//				//se calcula el producto escalar de los vectores normalizados
//                num=prod_esc_3d(*n1,*n2,1);
//
//                free(n1); free(n2);
//                assert(!((num>1)||(num<-1)));
//                num=asin(num);
//                if (num<0)
//					num=num+NUMBER_PI;
//				total=total+cuad(num);
//			}
//		}
//	}
//
//	return total;
}


//-----------------------ortogonalidad facial oblicua----------------------------------------
double CalculaCosteOrtogonalidadFacialOblicua(std::vector<double> &Solucion, CDB_Model *DDBB)
{
	long IndiceVerticeInicio,
		 IndiceVertice1, IndiceVertice2, IndiceVertice3, ContadorArista;
	double Coste, pEscalar, pVectorial, Beta, SumaBetas, MediaBetas, Numerador, Sigma, w,
		   SumaArcoSenos, aux, Modulo1, Modulo2;
	bool fin;

	Coste=0;
	long k=0;

	//Para cada cara del grafo
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		CFace CaraI= DDBB->GetFace(i);


		//Memorizo el vertice cabeza y cola de la primera arista
		long IndiceArista= CaraI.GetEdgeInFace(k);
		CEdge Arista= DDBB->GetEdge(IndiceArista);
		IndiceVerticeInicio=Arista.Head;
		IndiceVertice1=Arista.Head;
		IndiceVertice2=Arista.Tail;
		ContadorArista=1;
		SumaBetas=0;
		SumaArcoSenos=0;
		std::vector<double> ValoresBeta;
		fin = false;

		while (!fin){
			//Busco una arista convergente con la primera y memorizo el vértice no común
			long j;
			for (j=0; j<CaraI.GetSizeEdgesInFace(); j++){
				if (j!=k){
					IndiceArista= CaraI.GetEdgeInFace(j);
					CEdge Arista= DDBB->GetEdge(IndiceArista);
					if (Arista.Head == IndiceVertice2){
						IndiceVertice3=Arista.Tail;
						break;
					}
					if (Arista.Tail == IndiceVertice2){
						IndiceVertice3=Arista.Head;
						break;
					}
				}
			}

			//Realizo los calculos con los tres vértices de que dispongo
			CVertex Vertice1= DDBB->GetVertex(IndiceVertice1);
			CVertex Vertice2= DDBB->GetVertex(IndiceVertice2);
			CVertex Vertice3= DDBB->GetVertex(IndiceVertice3);
			Vertice1.z= Solucion[IndiceVertice1];
			Vertice2.z= Solucion[IndiceVertice2];
			Vertice3.z= Solucion[IndiceVertice3];

			Modulo1=Modulo2=Beta=0;
			Modulo1= MODULE2D (Vertice1.x, Vertice1.y, Vertice2.x, Vertice2.y);
			Modulo2= MODULE2D (Vertice2.x, Vertice2.y, Vertice3.x, Vertice3.y);

			pEscalar= ((Vertice2.x-Vertice1.x)/Modulo1)*((Vertice2.x-Vertice3.x)/Modulo2) +
					  ((Vertice2.y-Vertice1.y)/Modulo1)*((Vertice2.y-Vertice3.y)/Modulo2);

			pVectorial = (Vertice1.x-Vertice2.x)*(Vertice3.y-Vertice2.y)
				- (Vertice1.y-Vertice2.y)*(Vertice3.x-Vertice2.x);

			Beta = pow((double)-1, ContadorArista) * pEscalar * pVectorial;
			SumaBetas+=Beta;
			ValoresBeta.push_back(Beta);

///Ahora voy a analizar los vectores 3D
	
			Modulo1= MODULE3D(Vertice1.x, Vertice1.y, Vertice1.z, Vertice2.x, Vertice2.y, Vertice2.z);
			Modulo2= MODULE3D(Vertice3.x, Vertice3.y, Vertice3.z, Vertice2.x, Vertice2.y, Vertice2.z);		
			aux= ((Vertice2.x-Vertice1.x)/Modulo1)*((Vertice2.x-Vertice3.x)/Modulo2) +
				 ((Vertice2.y-Vertice1.y)/Modulo1)*((Vertice2.y-Vertice3.y)/Modulo2) +
				 ((Vertice2.z-Vertice1.z)/Modulo1)*((Vertice2.z-Vertice3.z)/Modulo2);

			aux= asin(aux);
			assert(std::fabs(aux)<=NUMBER_PI);

			SumaArcoSenos+=(aux*aux);

			//Repito el proceso mientras no se cierre la cara
			IndiceVertice1 = IndiceVertice2;
			IndiceVertice2 = IndiceVertice3;
			k=j;
//ANA++ Aqui se podria ahorrar mas tiempo de compuacion pasando mas valores y no se necesitarian volver a 
			//calcular. Pero lo hare mas adelante.

			if (IndiceVertice1==IndiceVerticeInicio)
				fin = true;
			else
				ContadorArista++;
		}

		MediaBetas=SumaBetas/ContadorArista;
		Numerador=0;
		for (long j=0; j<(long)ValoresBeta.size(); j++){
			Beta= ValoresBeta[j];
			Numerador+=(Beta-MediaBetas)*(Beta-MediaBetas);
		}
		Sigma = sqrt(Numerador/(ContadorArista-1));
		w = mu(0,0.2,Sigma);
		Coste+=w*SumaArcoSenos;
	}
	return Coste;
}


double GetCost_SkewSymmetry(std::vector<double> &Solucion, CDB_Model *DDBB)
//ANA++
//aqui implemento la regularidad de simetria facial oblicua, tal y como la escribio
//en su tesis Lipson
{
	double Total= 0;

	//Loop over the faces
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		CVerticesCollection VerticesPlusMid;

		//Add vertices
		std::vector<long> NumVerticesPlusMidPoints;
		std::vector<long> vertices= DDBB->GetAllVerticesInFace(DDBB->GetFace(i));
		for (long j=0; j<(long)vertices.size(); j++){
			CVertex Vertice= DDBB->GetVertex(vertices[j]);
			Vertice.z= Solucion[vertices[j]];
			VerticesPlusMid.AddVertex(Vertice);
			NumVerticesPlusMidPoints.push_back(vertices[j]);
		}

		//Add midpoints
		AddMidPoints(DDBB, NumVerticesPlusMidPoints, VerticesPlusMid);

		double W= 0;
		int k=0;
		CVertex VerticeEje, VerticeEje2;
		GetSymmetryCoefs(&VerticesPlusMid, VerticeEje, VerticeEje2, k, W);

		CFace CaraI= DDBB->GetFace(i);

		double Coste=0;
		long NumVertices= VerticesPlusMid.GetSize();

		//Get opposite pairs around k-th vertex/midpoint
		for (long j=1; j<(NumVertices/2); j++){
			double ProductoEscalar=0;
			double ModuloEje= 0;
			double ModuloTransversal=0;

			long next= (k+j)% NumVertices;
			long prev= (k-j)% NumVertices; if(prev < 0) prev += NumVertices;
			CVertex VerticeDespues= VerticesPlusMid.GetVertex(next);
			CVertex VerticeAntes= VerticesPlusMid.GetVertex(prev);

			ModuloEje= MODULE3D(VerticeEje.x, VerticeEje.y, VerticeEje.z, VerticeEje2.x, VerticeEje2.y, VerticeEje2.z);
			ModuloTransversal= MODULE3D(VerticeDespues.x, VerticeDespues.y,VerticeDespues.z, VerticeAntes.x, VerticeAntes.y, VerticeAntes.z);
			assert (ModuloEje !=0 || ModuloTransversal !=0);

			ProductoEscalar= ((VerticeEje2.x-VerticeEje.x)/ModuloEje)*((VerticeAntes.x-VerticeDespues.x)/ModuloTransversal) +
							 ((VerticeEje2.y-VerticeEje.y)/ModuloEje)*((VerticeAntes.y-VerticeDespues.y)/ModuloTransversal) +
							 ((VerticeEje2.z-VerticeEje.z)/ModuloEje)*((VerticeAntes.z-VerticeDespues.z)/ModuloTransversal);

			ProductoEscalar= (asin(ProductoEscalar)) * (asin(ProductoEscalar));
			Coste += ProductoEscalar;
		}
		assert(!_isnan(Coste));

		Coste = Coste * W;

		//Este debe ser el coste de esta función para esta cara, porque se calcula para cada cara.
		//Mi duda es, cómo se computa globalmente
		Total += Coste;
	}
	assert(!_isnan(Total));
	return Total;
}


double SymLipson(int j, CVerticesCollection *pListaVertices2)
//para calcular la parte de Simetria de la Simetria Facial Oblicua, para ello necesita como entradas
//el j que es el índice del posible eje evaluado según el orden que tienen la listaVertices2 que es
//la lista de los vértices de la cara evaluada y sus puntos medios (todos ordenados)
{

	int N, i, l,m, ll;
	double DistanciaAntes, DistanciaDespues, Varianza;

	i=l=m=ll=0;
	double Media= 0;
	double Sym= 0;
	N= pListaVertices2->GetSize();
	//De esta forma N es el doble del número de vértices en la cara

	CVertex VerticeEje= pListaVertices2->GetVertex(j);
	ll=(j+N/2) % N;
	CVertex VerticeEje2= pListaVertices2->GetVertex(ll);

	std::vector<double> ListaSym;

	for (i=1; i<N/2; i++){
		l= (j+i) % N;
		m=(j-i) % N; if(m<0) m += N;
		CVertex VerticeDespues= pListaVertices2->GetVertex(l);
		CVertex VerticeAntes= pListaVertices2->GetVertex(m);
		//La funcion DistanciaPuntoRecta solo tiene en cuenta las coordenadas x e y (no las z) 
		DistanciaDespues= GetDistanceVertexToLine(VerticeDespues, VerticeEje, VerticeEje2);
		DistanciaAntes= GetDistanceVertexToLine(VerticeAntes, VerticeEje, VerticeEje2);
		
		assert(!_isnan(DistanciaAntes));
		if (DistanciaAntes ==0) 
			Sym =-1;
		else
			Sym=(DistanciaDespues/DistanciaAntes)-1;
		
		Media +=Sym;
		ListaSym.push_back(Sym);
	}
	//Ahora falta por calcular la varianza (la desviación tipica)

	Media  /= ((long)ListaSym.size());
	Varianza =0;
	
	for (i=0;i<(long)ListaSym.size();i++){
		Sym= ListaSym[i];
		Varianza+=(Sym-Media)*(Sym-Media);
	}
	Varianza/=(N-1);

	return sqrt(Varianza);
}


double SkewLipson(int j, CVerticesCollection *pListaVertices2)
//Igual que la anterior pero para valorar la parte Skew de Lipson
{
double ModuloEje, ModuloTransversal, ProductoEscalar, ProductoVectorial;

	double Media=0;
	double Skew=0;

	long N= pListaVertices2->GetSize();
	//De esta forma N es el doble del número de vértices en la cara

	CVertex VerticeEje= pListaVertices2->GetVertex(j);
	CVertex VerticeEje2= pListaVertices2->GetVertex((j+N/2) % N);

	std::vector<double> ListaSkew;

	for (long i=1; i<N/2; i++){

		ModuloEje=ModuloTransversal=0;
		ProductoEscalar=ProductoVectorial=0;

		long l=(j+i) % N;
		long m=(j-i) % N; if(m<0) m += N;
		CVertex VerticeDespues= pListaVertices2->GetVertex(l);
		CVertex VerticeAntes= pListaVertices2->GetVertex(m);

		ModuloEje=MODULE2D(VerticeEje.x, VerticeEje.y, VerticeEje2.x, VerticeEje2.y);
		ModuloTransversal=MODULE2D(VerticeDespues.x, VerticeDespues.y, VerticeAntes.x, VerticeAntes.y);

		ProductoEscalar= ((VerticeEje2.x-VerticeEje.x)/ModuloEje)*((VerticeAntes.x-VerticeDespues.x)/ModuloTransversal) +
						 ((VerticeEje2.y-VerticeEje.y)/ModuloEje)*((VerticeAntes.y-VerticeDespues.y)/ModuloTransversal);

		ProductoVectorial = (((VerticeEje.x -VerticeEje2.x)*(VerticeDespues.y-VerticeAntes.y))-
			((VerticeEje.y -VerticeEje2.y)*(VerticeDespues.x-VerticeAntes.x)))/(ModuloEje*ModuloTransversal);

		Skew= ProductoEscalar * ProductoVectorial;
		
		Media +=Skew;
		ListaSkew.push_back(Skew);
	}

	Media =Media /N;
	double Varianza=0;

	for (long i=0; i<(long)ListaSkew.size(); i++){
		Skew= ListaSkew[i];
		Varianza+=(Skew-Media)*(Skew-Media);
	}
	Varianza/=(N-1);

	return sqrt(Varianza);
}


void GetSymmetryCoefs(CVerticesCollection *pListaVertices2,
					  CVertex &VerticeEje,
					  CVertex &VerticeEje2,
					  int &k, double &W)
//Funcion que calcula el coeficiente W para cada cara. Como salida debe dar las coordenadas del
//eje de la cara, y como entrada en vez de la cara entra la lista de vertices de la cara y sus puntos
//medios.
//Ademas de calcular el coeficiente me devuelve la linea del eje
//Y k es el vertice por el que pasa el eje: el orden segun la lista doble.
{

	double dDato= 0;

	std::vector<double> ListaMu;
	//j es el indice del posible eje evaluado, pero está referido al conjunto ListaVertices2
	for (long i=0; i<(pListaVertices2->GetSize()/2); i++){
		dDato=(SymLipson(i, pListaVertices2)+SkewLipson(i, pListaVertices2));

			assert(!_isnan(dDato));

			double Mu = mu(0,0.2,dDato);
			ListaMu.push_back(Mu);
		}

		W= ListaMu[0];
		k=0;
		for (long j=0; j<(long)ListaMu.size(); j++){
			if (ListaMu[j]>W) {
				k=j;
				W=ListaMu[j];}
		}
//k es el orden del vértice que es el posible eje. Pero es el orden dentro de la listaVertices2
			//que es lista de la cara y doble. 

//AQUI SI W ES =0 DEBERIAMOS PUENTEAR LO QUE VIENE
		assert(!_isnan(W));

		//W es el valor de ponderación que Lipson da en su tesis para cada Cara
		//Ahora falta calcular el coste
		VerticeEje.Clear();
		VerticeEje2.Clear();

		VerticeEje= pListaVertices2->GetVertex(k);
		VerticeEje2= pListaVertices2->GetVertex(k+(pListaVertices2->GetSize()/2));
}
