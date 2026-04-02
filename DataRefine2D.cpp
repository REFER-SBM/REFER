
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

#include <vector>

#include "Tools_Geometry.h" // DEGREES_TO_RADIANS

#include "DataInflation.h"     //TListOfRegularities
#include "DataRefine2D.h"

CDataRefine2D::CDataRefine2D()
//Data for 2D refinement
{
	//Initialization of parameters for refinement
	IgnoreShortEdges = true;
	ToleranceIgnoreShortEdges = 0.01; //1% Of the size of the drawing

	bMergeVertices= true;
	MergeCubicCorners= false;
	DistanceMergeCubicCorners= 6; //pixels
	MergeAlignedCubicCorners= true;
	MergeRotationCubicCorners= DEGREES_TO_RADIANS(10);

	MergeCarefulLineTips= true;
	MergeMaxDistVertices= 0.08;   // 8% of the length of the line
	MergeMaxVerticesRotation= DEGREES_TO_RADIANS(10);
	ReificationMerit= 0.5;

	MergeCasualLineTips= true;
	ReMergeCasualInTol= 0.50; //50% of the length of the line
	ReMergeCasualOutTol= 0.25; //25% of the length of the line

	ReMergeVerticesTriplets= true;
	ReMergeMaxDistTriplets= 0.10;   // 10% of the length of the line
	EmergentTripletsMerit= 0.5;     //Maximum merit for triplets is fixed as 50%

	ReMergeVerticesDangling= true;
	ReMergeMaxDistDangling= 0.10;   // 10% of the length of the line
	EmergenceDanglingDeMerit= 0.5;     //Merit of junctions with dangling tips may be reduced up to 50%

	Merge_T_Vertices = true;
	Split_T_Vertices = false;
	ToleranceSplitTVertices = 0.01; //1%

	SplitNon2ManifoldVertices = false;
	SplitNon2ManifoldEdges = false;

	MergeCollinearEdges = false;
	ToleranceMergeCollinearEdges = DEGREES_TO_RADIANS(5);

	bOptimize2D = false;
	Regularities2D.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive= true;
	Regularities2D.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive= false;
	Regularities2D.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive= false;
	Regularities2D.NumActiveRegularities = 1;   // It must match the number of regularities "true" [0, 3]

	ErrorParallelism2D= DEGREES_TO_RADIANS(10);
	ErrorVerticality2D= DEGREES_TO_RADIANS(10);
	ErrorCollinearity2D= DEGREES_TO_RADIANS(10);

	//Initialization of parameters for distortion
	CoeficienteDistorsion=0.005;
};


CDataRefine2D::~CDataRefine2D(){
};
