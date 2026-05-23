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

//Get labels from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C.
	//Automatic Creation of Boundary-Representation Models 
	//from Single Line Drawings, PhD Thesis, University of Wales. 2003

#include <vector>
#include "DataLabels.h"

#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_Object.h"

#include "CueLabelsVarley_LabelLines.h"


void LabelLineAsArrow (LABELLING *Labelling, EDGE_INDEX Nedge)
{
	if (!Labelling->EdgeLabelIsFinalised[Nedge])
	{
		Labelling->EdgeLabel[Nedge] = LineIsArrow;
		Labelling->EdgeLabelIsFinalised[Nedge] = cTRUE;
	}
}


void LabelLineAsBackArrow (LABELLING *Labelling, EDGE_INDEX Nedge)
{

	if (!Labelling->EdgeLabelIsFinalised[Nedge])
	{
		// Reverse Vertices For Edge
		VERTEX_INDEX Vtemp = Labelling->EdgeEndVertex[Nedge];
		Labelling->EdgeEndVertex[Nedge] = Labelling->EdgeStartVertex[Nedge];
		Labelling->EdgeStartVertex[Nedge] = Vtemp;

		// Reverse Faces For Edge
		FACE_INDEX Ftemp = Labelling->EdgeForwardFace[Nedge];
		Labelling->EdgeForwardFace[Nedge] = Labelling->EdgeReverseFace[Nedge];
		Labelling->EdgeReverseFace[Nedge] = Ftemp;

		LabelLineAsArrow(Labelling,Nedge);
	}
}


void LabelLineAsRelativeArrow (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Nvert)
{
   if (Nvert == Labelling->EdgeStartVertex[Nedge])
   {
      LabelLineAsBackArrow(Labelling,Nedge);
   }
   else
   {
      LabelLineAsArrow(Labelling,Nedge);
   }
}


void LabelLineAsRelativeBackArrow (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Nvert)
{
   if (Nvert == Labelling->EdgeStartVertex[Nedge])
   {
      LabelLineAsArrow(Labelling,Nedge);
   }
   else
   {
      LabelLineAsBackArrow(Labelling,Nedge);
   }
}




void LabelLineAsConvex (LABELLING *Labelling, EDGE_INDEX Nedge)
{
   //printf("Label Edge %d as Convex\n", Nedge);

   if (!Labelling->EdgeLabelIsFinalised[Nedge])
   {
      Labelling->EdgeLabel[Nedge] = LineIsConvex;
      Labelling->EdgeLabelIsFinalised[Nedge] = cTRUE;
   }
}


void LabelLineAsConcave (LABELLING *Labelling, EDGE_INDEX Nedge)
{
   //printf("Label Edge %d as Concave\n", Nedge);

   if (!Labelling->EdgeLabelIsFinalised[Nedge])
   {
      Labelling->EdgeLabel[Nedge] = LineIsConcave;
      Labelling->EdgeLabelIsFinalised[Nedge] = cTRUE;
   }
}
