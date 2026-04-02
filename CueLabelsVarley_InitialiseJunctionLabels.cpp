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

//Get faces from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C.
	//Automatic Creation of Boundary-Representation Models 
	//from Single Line Drawings, PhD Thesis, University of Wales. 2003
#include <stdafx.h>
#include <cstdio>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "resource.h"
#include "ReferDoc.h"

#include "DataLabels.h"

#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_Object.h"

#include "CueLabelsVarley_LabellingConstants.h"
#include "CueLabelsVarley_LabellingParameters.h"

#include "CueLabelsVarley_InitialiseJunctionLabels.h"




CCueLabelsVarley_InitialiseJunctionLabels::CCueLabelsVarley_InitialiseJunctionLabels()
{
}


CCueLabelsVarley_InitialiseJunctionLabels::~CCueLabelsVarley_InitialiseJunctionLabels()
{
}


void CCueLabelsVarley_InitialiseJunctionLabels::SetLJunctionToType (LABELLING *Labelling, VERTEX_INDEX Nvert, char LabellingType)
{
   switch(LabellingType)
   {
      case LABELLING_TRIHEDRAL:
      case LABELLING_NTAA:
      case LABELLING_TRIHEDRAL_LWY:
         Labelling->VertexLabel[Nvert].Vexity = Ltrihedral;
      break;

      case LABELLING_TETRAHEDRAL:
      case LABELLING_PENTAHEDRAL:
      case LABELLING_HEXAHEDRAL:
         Labelling->VertexLabel[Nvert].Vexity = Lall;
      break;
   }
}


void CCueLabelsVarley_InitialiseJunctionLabels::SetT4JunctionToType (LABELLING *Labelling, VERTEX_INDEX Nvert, char LabellingType)
{
   switch(LabellingType)
   {
      case LABELLING_TRIHEDRAL:
         Labelling->VertexLabel[Nvert].Vexity = Ttrihedral;
         {
            EDGE_INDEX Neb = Labelling->VENumber[Nvert][0];
            EDGE_INDEX Nea = Labelling->VENumber[Nvert][1];
            Labelling->EdgeLabel[Neb] &= LineIsB(Labelling,Neb,Nvert);
            Labelling->EdgeLabel[Nea] &= LineIsA(Labelling,Nea,Nvert);
         }
      break;

      case LABELLING_NTAA:
         Labelling->VertexLabel[Nvert].Vexity = Ttrihedral|TNTAA;
      break;

      case LABELLING_TETRAHEDRAL:
      case LABELLING_PENTAHEDRAL:
      case LABELLING_HEXAHEDRAL:
      case LABELLING_TRIHEDRAL_LWY:
         Labelling->VertexLabel[Nvert].Vexity = Tall;
      break;
   }
}


void CCueLabelsVarley_InitialiseJunctionLabels::SetWJunctionToType (LABELLING *Labelling, VERTEX_INDEX Nvert, char LabellingType)
{
   switch(LabellingType)
   {
      case LABELLING_TRIHEDRAL:
      case LABELLING_NTAA:
      case LABELLING_TRIHEDRAL_LWY:
         Labelling->VertexLabel[Nvert].Vexity = Wtrihedral;
      break;

      case LABELLING_TETRAHEDRAL:
      case LABELLING_PENTAHEDRAL:
      case LABELLING_HEXAHEDRAL:
         Labelling->VertexLabel[Nvert].Vexity = Wall;
      break;
   }
}


void CCueLabelsVarley_InitialiseJunctionLabels::SetXJunctionToType (LABELLING *Labelling, VERTEX_INDEX Nvert, char LabellingType)
{
   switch(LabellingType)
   {
      case LABELLING_TETRAHEDRAL:
      case LABELLING_PENTAHEDRAL:
      case LABELLING_HEXAHEDRAL:
      case LABELLING_TRIHEDRAL_LWY:
         Labelling->VertexLabel[Nvert].Vexity = Xnontrihedral;
      break;
   }
}


void CCueLabelsVarley_InitialiseJunctionLabels::SetYJunctionToType (LABELLING *Labelling, VERTEX_INDEX Nvert, char LabellingType)
{
   switch(LabellingType)
   {
      case LABELLING_TRIHEDRAL:
      case LABELLING_NTAA:
      case LABELLING_TRIHEDRAL_LWY:
         Labelling->VertexLabel[Nvert].Vexity = Ytrihedral;
      break;

      case LABELLING_TETRAHEDRAL:
      case LABELLING_PENTAHEDRAL:
      case LABELLING_HEXAHEDRAL:
         Labelling->VertexLabel[Nvert].Vexity = Yall;
      break;
   }
}


void CCueLabelsVarley_InitialiseJunctionLabels::Set6JunctionToType (LABELLING *Labelling, VERTEX_INDEX Nvert, char LabellingType)
{
   switch(LabellingType)
   {
      case LABELLING_NTAA:
         Labelling->VertexLabel[Nvert].Vexity = Xcdcdcd|Xdcdcdc;
      break;

      case LABELLING_HEXAHEDRAL:
      case LABELLING_TRIHEDRAL_LWY:
         Labelling->VertexLabel[Nvert].Vexity = X6all;
      break;
   }
}


void CCueLabelsVarley_InitialiseJunctionLabels::SetAllJunctionsToType (LABELLING *Labelling, char LabellingType)
{
	for (VERTEX_INDEX n=0; n<Labelling->VertexCount; ++n)  if (!Labelling->VertexLabel[n].Vexity)
	{
		switch(Labelling->VertexLabel[n].Shape)
		{
			case JunctionIsL:
				SetLJunctionToType(Labelling,n,LabellingType);
			break;

			case JunctionIsW:
				SetWJunctionToType(Labelling,n,LabellingType);
			break;

			case JunctionIsY:
				SetYJunctionToType(Labelling,n,LabellingType);
			break;

			case JunctionIsT4:
				SetT4JunctionToType(Labelling,n,LabellingType);
			break;

			case JunctionIsX:
				SetXJunctionToType(Labelling,n,LabellingType);
			break;

			case JunctionIsK:
				Labelling->VertexLabel[n].Vexity = Kall;
			break;

			case JunctionIsM:
				Labelling->VertexLabel[n].Vexity = Mall;
			break;

			case JunctionIs5way:
				Labelling->VertexLabel[n].Vexity = X5all;
			break;

			case JunctionIsZ6:
			case JunctionIs6way:
				Set6JunctionToType(Labelling,n,LabellingType);
			break;
		}
	}
}


void CCueLabelsVarley_InitialiseJunctionLabels::InitialiseJunctionLabels (OBJECT *Object,
																		  char LabellingType)
//using a particular catalogue
//	Set all line labels to unknown
//	For each junction
//	*	If it is an X-junction and trihedral labelling is selected, 
//		select non-trihedral normalon labelling instead
//	*	If it is a K- or M-junction and trihedral labelling is selected, 
//		select tetrahedral labelling instead
//	*	If it is a K- or M-junction and non-trihedral normalon labelling is selected,
//		select hexahedral labelling instead
//	*	If it is a 5-way junction and lower-order labelling is selected,
//		select pentahedral labelling instead
//	*	If it is a 6-way junction and trihedral labelling is selected,
//		select non-trihedral normalon labelling instead
//	*	If it is a 6-way junction and tetrahedral or pentahedral labelling is selected,
//		select hexahedral labelling instead
//		*	(And then set junction labels accordingly)
//		*	For each junction for which the label has not already been set, allow all labels
//			consistent with the junction geometry and the type of labelling selected
//		*	Set the vertex maximum hedrality and set of acceptable final vertex types
//			according to the catalogue actually being used
{
	// Set all line labels to unknown
	for (int n=0; n<Object->Labelling.EdgeCount; ++n)
		Object->Labelling.EdgeLabel[n] = LineIsUnknown;

	// Firstly, check whether the labelling type we've got is good enough
	for (int n=0; n<Object->Labelling.VertexCount; ++n)
	{
		switch(Object->Labelling.VertexLabel[n].Shape)
		{
		case JunctionIsL:
		case JunctionIsW:
		case JunctionIsY:
		case JunctionIsT4:
			/* Anything can label trihedral junctions */
		break;

		case JunctionIsT:
			{
			CString sMensaje, sMensajeAux;
			sMensaje= "Labelling error: *** INCONSISTENCY ERROR T in LABELSKETCH! ";
			//sMensajeAux.Format("%s", VertexString(Object,&Object->Labelling,n));
			//sMensaje+=sMensajeAux;
			AfxMessageBox(sMensaje);
			//exit(BADNEWS);
			}
		break;

		case JunctionIsZ:
			if (LabellingType == LABELLING_TRIHEDRAL)  LabellingType = LABELLING_NTAA;
		break;

		case JunctionIsX:
		case JunctionIsK:
		case JunctionIsM:
			if (LabellingType == LABELLING_TRIHEDRAL)  LabellingType = LABELLING_TETRAHEDRAL;
			else
			if (LabellingType == LABELLING_NTAA)       LabellingType = LABELLING_HEXAHEDRAL;
		break;

		case JunctionIs5way:
			switch(LabellingType)
			{
				case LABELLING_TRIHEDRAL:
				case LABELLING_NTAA:
				case LABELLING_TETRAHEDRAL:
					LabellingType = LABELLING_PENTAHEDRAL;
				break;
			}
		break;

		case JunctionIsZ6:
		case JunctionIs6way:
			switch(LabellingType)
			{
				case LABELLING_TRIHEDRAL:
					LabellingType = LABELLING_NTAA;
				break;

				case LABELLING_TETRAHEDRAL:
				case LABELLING_PENTAHEDRAL:
					LabellingType = LABELLING_HEXAHEDRAL;
				break;
			}
			break;
		}
	}

	// And then set junction labels accordingly
	SetAllJunctionsToType(&Object->Labelling,LabellingType);

	switch (LabellingType)
	{
		case LABELLING_TRIHEDRAL:
			Object->MaxHedralVertex  = 3;
			Object->Labelling.FinalVertexTypes = VFinalIsTrihedral;
		break;

		case LABELLING_NTAA:
			Object->MaxHedralVertex  = 6;
			Object->Labelling.FinalVertexTypes = VFinalIsNTAA;
		break;

		case LABELLING_TETRAHEDRAL:
			Object->MaxHedralVertex  = 4;
			Object->Labelling.FinalVertexTypes = VFinalIs4hedral;
		break;

		case LABELLING_PENTAHEDRAL:
			Object->MaxHedralVertex  = 5;
			Object->Labelling.FinalVertexTypes = VFinalIs5hedral;
		break;

		case LABELLING_HEXAHEDRAL:
			Object->MaxHedralVertex  = 6;
			Object->Labelling.FinalVertexTypes = VFinalIs6hedral;
		break;

		case LABELLING_TRIHEDRAL_LWY:
			Object->MaxHedralVertex  = 6;
			Object->Labelling.FinalVertexTypes = VFinalIs6hedral;
		break;
	}
}
