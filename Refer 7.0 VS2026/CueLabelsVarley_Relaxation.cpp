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

#include <stdafx.h>
#include <cstdio>

#include <cassert>
#include <cmath>

//-------used to import/export data from/to the external database-----------
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"
//#include "resource.h"
//#include "ReferDoc.h"
//--------------------------------------------------------------------------

#include "DataLabels.h"

#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Maths.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_Object.h"
#include "CueLabelsVarley_LabellingConstants.h"
#include "CueLabelsVarley_LabellingParameters.h"

#include "CueLabelsVarley_LabelJunctions.h"

#include "CueLabelsVarley_Relaxation.h"


CCueLabelsVarley_Relaxation::CCueLabelsVarley_Relaxation(CCueLabelsVarley_LabellingConstants *eLC,
														 CCueLabelsVarley_LabellingParameters *eLP)
{
	//Labelling constants (tuning constants)
	LC= eLC;

	//Labelling parameters
	LP= eLP;
}


CCueLabelsVarley_Relaxation::~CCueLabelsVarley_Relaxation()
{
}


void CCueLabelsVarley_Relaxation::InitialiseSingleEdgeProbabilitiesUsingShapePairs (OBJECT *Object, EDGE_INDEX Nedge)
{
   VERTEX_INDEX Vs = Object->Labelling.EdgeStartVertex[Nedge];
   VERTEX_INDEX Vt = Object->Labelling.EdgeEndVertex[Nedge];
   VERTEX_LABEL Ss = Object->Labelling.VertexLabel[Vs].Shape;
   VERTEX_LABEL St = Object->Labelling.VertexLabel[Vt].Shape;

   if ((Ss == JunctionIsK) && (St == JunctionIsK))
   {
      int Iks = VertexEdgeIndex(Object,Vs,Nedge);
      int Ikt = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Iks*10+Ikt)
      {
         case 23:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SK ";
            sMensajeAux.Format("%d", Iks);
            sMensaje+=sMensajeAux;
            sMensaje+="TK ";
            sMensajeAux.Format("%d", Ikt);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsK) && (St == JunctionIsM))
   {
      Object->Eprobabilities[Nedge].Amerit =  0.25;
      Object->Eprobabilities[Nedge].Bmerit =  0.25;
      Object->Eprobabilities[Nedge].Cmerit =  3.5;
      Object->Eprobabilities[Nedge].Dmerit =  2.75;
   }
   else
   if ((Ss == JunctionIsK) && (St == JunctionIsT4))
   {
      int Ik = VertexEdgeIndex(Object,Vs,Nedge);
      int It = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Ik*10+It)
      {
         case  1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    3.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 10:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    7.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 22:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 32:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    4.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SK ";
            sMensajeAux.Format("%d", Ik);
            sMensaje+=sMensajeAux;
            sMensaje+="TT ";
            sMensajeAux.Format("%d", It);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsK) && (St == JunctionIsW))
   {
      int Ik = VertexEdgeIndex(Object,Vs,Nedge);
      int Iw = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Ik*10+Iw)
      {
         case 20:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    1.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 21:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    3.5;
            Object->Eprobabilities[Nedge].Dmerit =    7.5;
         break;

         case 22:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 31:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    6.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SK ";
            sMensajeAux.Format("%d", Ik);
            sMensaje+=sMensajeAux;
            sMensaje+="TW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsK) && (St == JunctionIsX))
   {
      int Ik = VertexEdgeIndex(Object,Vs,Nedge);

      switch (Ik)
      {
         case 2:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    1.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SK ";
            sMensajeAux.Format("%d", Ik);
            sMensaje+=sMensajeAux;
            sMensaje+="TX ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsK) && (St == JunctionIsY))
   {
      int Ik = VertexEdgeIndex(Object,Vs,Nedge);

      switch (Ik)
      {
         case 2:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    9.5;
            Object->Eprobabilities[Nedge].Dmerit =   19.5;
         break;

         case 3:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   15.5;
            Object->Eprobabilities[Nedge].Dmerit =   20.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SK ";
            sMensajeAux.Format("%d", Ik);
            sMensaje+=sMensajeAux;
            sMensaje+="TY ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsL) && (St == JunctionIsK))
   {
      int Il = VertexEdgeIndex(Object,Vs,Nedge);
      int Ik = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Il*10+Ik)
      {
         case  2:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case  3:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    1.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 13:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SL ";
            sMensajeAux.Format("%d", Il);
            sMensaje+=sMensajeAux;
            sMensaje+="TK ";
            sMensajeAux.Format("%d", Ik);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsL) && (St == JunctionIsL))
   {
      int Ils = VertexEdgeIndex(Object,Vs,Nedge);
      int Ilt = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Ils*10+Ilt)
      {
         case  0:
            Object->Eprobabilities[Nedge].Amerit =    2.25;
            Object->Eprobabilities[Nedge].Bmerit =    2.25;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case  1:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =   54.5;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 10:
            Object->Eprobabilities[Nedge].Amerit =   54.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 11:
            Object->Eprobabilities[Nedge].Amerit =    1.75;
            Object->Eprobabilities[Nedge].Bmerit =    1.75;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SL ";
            sMensajeAux.Format("%d", Ils);
            sMensaje+=sMensajeAux;
            sMensaje+="TL ";
            sMensajeAux.Format("%d", Ilt);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsL) && (St == JunctionIsM))
   {
      int Il = VertexEdgeIndex(Object,Vs,Nedge);
      int Im = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Il*10+Im)
      {
         case  0:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    2.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 10:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    2.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 13:
            Object->Eprobabilities[Nedge].Amerit =    2.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SL ";
            sMensajeAux.Format("%d", Il);
            sMensaje+=sMensajeAux;
            sMensaje+="TM ";
            sMensajeAux.Format("%d", Im);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsL) && (St == JunctionIsT4))
   {
      int Il = VertexEdgeIndex(Object,Vs,Nedge);
      int It = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Il*10+It)
      {
         case  0:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =  188.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case  1:
            Object->Eprobabilities[Nedge].Amerit =   61.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case  2:
            Object->Eprobabilities[Nedge].Amerit =    1.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 10:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =  112.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 11:
            Object->Eprobabilities[Nedge].Amerit =  114.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 12:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    3.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SL ";
            sMensajeAux.Format("%d", Il);
            sMensaje+=sMensajeAux;
            sMensaje+="TT ";
            sMensajeAux.Format("%d", It);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsL) && (St == JunctionIsW))
   {
      int Il = VertexEdgeIndex(Object,Vs,Nedge);
      int Iw = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Il*10+Iw)
      {
         case  0:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =   32.5;
            Object->Eprobabilities[Nedge].Cmerit =   11.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case  1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   86.5;
            Object->Eprobabilities[Nedge].Dmerit =    7.5;
         break;

         case  2:
            Object->Eprobabilities[Nedge].Amerit =   36.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =   83.5;
            Object->Eprobabilities[Nedge].Dmerit =   23.5;
         break;

         case 10:
            Object->Eprobabilities[Nedge].Amerit =   26.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =  117.5;
            Object->Eprobabilities[Nedge].Dmerit =   21.5;
         break;

         case 11:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   38.5;
            Object->Eprobabilities[Nedge].Dmerit =    4.5;
         break;

         case 12:
            Object->Eprobabilities[Nedge].Amerit =   44.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    9.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SL ";
            sMensajeAux.Format("%d", Il);
            sMensaje+=sMensajeAux;
            sMensaje+="TW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsL) && (St == JunctionIsX))
   {
      int Il = VertexEdgeIndex(Object,Vs,Nedge);

      switch (Il)
      {
         case  0:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case  1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    1.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         default:
            printf("ERROR SL%d TX\n", Il);
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SL ";
            sMensajeAux.Format("%d", Il);
            sMensaje+=sMensajeAux;
            sMensaje+="TX ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsL) && (St == JunctionIsY))
   {
      int Il = VertexEdgeIndex(Object,Vs,Nedge);

      switch (Il)
      {
         case 0:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    4.5;
            Object->Eprobabilities[Nedge].Cmerit =    3.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 1:
            Object->Eprobabilities[Nedge].Amerit =    3.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SL ";
            sMensajeAux.Format("%d", Il);
            sMensaje+=sMensajeAux;
            sMensaje+="TY ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsM) && (St == JunctionIsL))
   {
      int Im = VertexEdgeIndex(Object,Vs,Nedge);
      int Il = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Im*10+Il)
      {
         case 01:
            Object->Eprobabilities[Nedge].Amerit =    2.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 30:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    1.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SM ";
            sMensajeAux.Format("%d", Im);
            sMensaje+=sMensajeAux;
            sMensaje+="TL ";
            sMensajeAux.Format("%d", Il);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsM) && (St == JunctionIsM))
   {
      int Ims = VertexEdgeIndex(Object,Vs,Nedge);
      int Imt = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Ims*10+Imt)
      {
         case  1:
         case 10:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         case  2:
         case 20:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 11:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         case 12:
         case 21:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   20.5;
            Object->Eprobabilities[Nedge].Dmerit =    6.5;
         break;

         case 13:
         case 31:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    1.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 22:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 23:
         case 32:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    5.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SM ";
            sMensajeAux.Format("%d", Ims);
            sMensaje+=sMensajeAux;
            sMensaje+="TM ";
            sMensajeAux.Format("%d", Imt);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsM) && (St == JunctionIsT4))
   {
      int Im = VertexEdgeIndex(Object,Vs,Nedge);
      int It = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Im*10+It)
      {
         case  1:
            Object->Eprobabilities[Nedge].Amerit =    2.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 30:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    3.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SM ";
            sMensajeAux.Format("%d", Im);
            sMensaje+=sMensajeAux;
            sMensaje+="TT ";
            sMensajeAux.Format("%d", It);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsM) && (St == JunctionIsW))
   {
      int Im = VertexEdgeIndex(Object,Vs,Nedge);
      int Iw = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Im*10+Iw)
      {
         case 01:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         case 02:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 11:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    9.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 21:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    7.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 31:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SM ";
            sMensajeAux.Format("%d", Im);
            sMensaje+=sMensajeAux;
            sMensaje+="TW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsM) && (St == JunctionIsX))
   {
      int Im = VertexEdgeIndex(Object,Vs,Nedge);

      switch (Im)
      {
         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   19.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         case 2:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   22.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SM ";
            sMensajeAux.Format("%d", Im);
            sMensaje+=sMensajeAux;
            sMensaje+="TX ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsM) && (St == JunctionIs5way))
   {
      int Im = VertexEdgeIndex(Object,Vs,Nedge);

      switch (Im)
      {
         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   11.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 2:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   11.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SM ";
            sMensajeAux.Format("%d", Im);
            sMensaje+=sMensajeAux;
            sMensaje+="TX5 ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsM) && (St == JunctionIsY))
   {
      int Im = VertexEdgeIndex(Object,Vs,Nedge);

      switch (Im)
      {
         case 0:
            Object->Eprobabilities[Nedge].Amerit =    1.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =   11.5;
            Object->Eprobabilities[Nedge].Dmerit =    6.5;
         break;

         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   33.5;
            Object->Eprobabilities[Nedge].Dmerit =   10.5;
         break;

         case 2:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   28.5;
            Object->Eprobabilities[Nedge].Dmerit =   11.5;
         break;

         case 3:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    3.5;
            Object->Eprobabilities[Nedge].Cmerit =   10.5;
            Object->Eprobabilities[Nedge].Dmerit =    3.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SM ";
            sMensajeAux.Format("%d", Im);
            sMensaje+=sMensajeAux;
            sMensaje+="TY ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsT4) && (St == JunctionIsK))
   {
      int It = VertexEdgeIndex(Object,Vs,Nedge);
      int Ik = VertexEdgeIndex(Object,Vt,Nedge);

      switch (It*10+Ik)
      {
         case  1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    7.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 10:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    3.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: ST ";
            sMensajeAux.Format("%d", It);
            sMensaje+=sMensajeAux;
            sMensaje+="TK ";
            sMensajeAux.Format("%d", Ik);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsT4) && (St == JunctionIsT4))
   {
      int Its = VertexEdgeIndex(Object,Vs,Nedge);
      int Itt = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Its*10+Itt)
      {
         case  0:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    0.25;
            Object->Eprobabilities[Nedge].Dmerit =    0.25;
         break;

         case  1:
            Object->Eprobabilities[Nedge].Amerit =   61.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    3.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case  2:
            Object->Eprobabilities[Nedge].Amerit =   21.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 10:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =   61.5;
            Object->Eprobabilities[Nedge].Cmerit =    3.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 11:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    0.25;
            Object->Eprobabilities[Nedge].Dmerit =    0.25;
         break;

         case 20:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =   21.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 21:
            Object->Eprobabilities[Nedge].Amerit =    9.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 22:
            Object->Eprobabilities[Nedge].Amerit =    3.0;
            Object->Eprobabilities[Nedge].Bmerit =    3.0;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: ST ";
            sMensajeAux.Format("%d", Its);
            sMensaje+=sMensajeAux;
            sMensaje+="TT ";
            sMensajeAux.Format("%d", Itt);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsT4) && (St == JunctionIsW))
   {
      int It = VertexEdgeIndex(Object,Vs,Nedge);
      int Iw = VertexEdgeIndex(Object,Vt,Nedge);

      switch (It*10+Iw)
      {
         case 20:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =   12.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         case 21:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   14.5;
            Object->Eprobabilities[Nedge].Dmerit =   61.5;
         break;

         case 22:
            Object->Eprobabilities[Nedge].Amerit =   17.5;
            Object->Eprobabilities[Nedge].Bmerit =    1.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: ST ";
            sMensajeAux.Format("%d", It);
            sMensaje+=sMensajeAux;
            sMensaje+="TW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsT4) && (St == JunctionIsY))
   {
      int It = VertexEdgeIndex(Object,Vs,Nedge);

      switch (It)
      {
         case 2:
            Object->Eprobabilities[Nedge].Amerit =   13.5;
            Object->Eprobabilities[Nedge].Bmerit =   13.5;
            Object->Eprobabilities[Nedge].Cmerit =   21.5;
            Object->Eprobabilities[Nedge].Dmerit =   16.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: ST ";
            sMensajeAux.Format("%d", It);
            sMensaje+=sMensajeAux;
            sMensaje+="TY ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsW) && (St == JunctionIsK))
   {
      int Iw = VertexEdgeIndex(Object,Vs,Nedge);
      int Ik = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Iw*10+Ik)
      {
         case 10:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    8.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         case 11:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    6.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         case 12:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    3.5;
            Object->Eprobabilities[Nedge].Dmerit =    7.5;
         break;

         case 13:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    6.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            sMensaje+="TK ";
            sMensajeAux.Format("%d", Ik);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsW) && (St == JunctionIsL))
   {
      int Iw = VertexEdgeIndex(Object,Vs,Nedge);
      int Il = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Iw*10+Il)
      {
         case  0:
            Object->Eprobabilities[Nedge].Amerit =   32.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =   11.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case  1:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =   26.5;
            Object->Eprobabilities[Nedge].Cmerit =  117.5;
            Object->Eprobabilities[Nedge].Dmerit =   21.5;
         break;

         case 10:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   86.5;
            Object->Eprobabilities[Nedge].Dmerit =    7.5;
         break;

         case 11:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   38.5;
            Object->Eprobabilities[Nedge].Dmerit =    4.5;
         break;

         case 20:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =   36.5;
            Object->Eprobabilities[Nedge].Cmerit =   83.5;
            Object->Eprobabilities[Nedge].Dmerit =   23.5;
         break;

         case 21:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =   44.5;
            Object->Eprobabilities[Nedge].Cmerit =    9.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            sMensaje+="TL ";
            sMensajeAux.Format("%d", Il);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsW) && (St == JunctionIsM))
   {
      int Iw = VertexEdgeIndex(Object,Vs,Nedge);
      int Im = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Iw*10+Im)
      {
         case  1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    3.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 10:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         case 11:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    9.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 12:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    7.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 13:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         case 20:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 22:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    4.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            sMensaje+="TM ";
            sMensajeAux.Format("%d", Im);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsW) && (St == JunctionIsT4))
   {
      int Iw = VertexEdgeIndex(Object,Vs,Nedge);
      int It = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Iw*10+It)
      {
         case  0:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    4.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case  1:
            Object->Eprobabilities[Nedge].Amerit =   95.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case  2:
            Object->Eprobabilities[Nedge].Amerit =   12.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         case 10:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    9.5;
            Object->Eprobabilities[Nedge].Cmerit =    9.5;
            Object->Eprobabilities[Nedge].Dmerit =   10.5;
         break;

         case 11:
            Object->Eprobabilities[Nedge].Amerit =    3.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    9.5;
            Object->Eprobabilities[Nedge].Dmerit =   10.5;
         break;

         case 12:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   14.5;
            Object->Eprobabilities[Nedge].Dmerit =   61.5;
         break;

         case 20:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =  151.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 21:
            Object->Eprobabilities[Nedge].Amerit =    1.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 22:
            Object->Eprobabilities[Nedge].Amerit =    1.5;
            Object->Eprobabilities[Nedge].Bmerit =   17.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            sMensaje+="TT ";
            sMensajeAux.Format("%d", It);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsW) && (St == JunctionIsW))
   {
      int Iws = VertexEdgeIndex(Object,Vs,Nedge);
      int Iwt = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Iws*10+Iwt)
      {
         case  0:
            Object->Eprobabilities[Nedge].Amerit =    1.5;
            Object->Eprobabilities[Nedge].Bmerit =    1.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case  1:
            Object->Eprobabilities[Nedge].Amerit =    3.5;
            Object->Eprobabilities[Nedge].Bmerit =    3.5;
            Object->Eprobabilities[Nedge].Cmerit =   25.5;
            Object->Eprobabilities[Nedge].Dmerit =    8.5;
         break;

         case  2:
            Object->Eprobabilities[Nedge].Amerit =   11.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    8.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 10:
            Object->Eprobabilities[Nedge].Amerit =    3.5;
            Object->Eprobabilities[Nedge].Bmerit =    3.5;
            Object->Eprobabilities[Nedge].Cmerit =   25.5;
            Object->Eprobabilities[Nedge].Dmerit =    8.5;
         break;

         case 11:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   25.5;
            Object->Eprobabilities[Nedge].Dmerit =    4.5;
         break;

         case 12:
            Object->Eprobabilities[Nedge].Amerit =    4.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =   27.5;
            Object->Eprobabilities[Nedge].Dmerit =    8.5;
         break;

         case 20:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =   11.5;
            Object->Eprobabilities[Nedge].Cmerit =    8.5;
            Object->Eprobabilities[Nedge].Dmerit =    1.5;
         break;

         case 21:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    4.5;
            Object->Eprobabilities[Nedge].Cmerit =   27.5;
            Object->Eprobabilities[Nedge].Dmerit =    8.5;
         break;

         case 22:
            Object->Eprobabilities[Nedge].Amerit =    1.25;
            Object->Eprobabilities[Nedge].Bmerit =    1.25;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SW ";
            sMensajeAux.Format("%d", Iws);
            sMensaje+=sMensajeAux;
            sMensaje+="TW ";
            sMensajeAux.Format("%d", Iwt);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsW) && (St == JunctionIsX))
   {
      int Iw = VertexEdgeIndex(Object,Vs,Nedge);

      switch (Iw)
      {
         case 0:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    7.5;
            Object->Eprobabilities[Nedge].Dmerit =    4.5;
         break;

         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =  172.5;
            Object->Eprobabilities[Nedge].Dmerit =   14.5;
         break;

         case 2:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    1.5;
            Object->Eprobabilities[Nedge].Cmerit =   10.5;
            Object->Eprobabilities[Nedge].Dmerit =    3.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            sMensaje+="TX ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsW) && (St == JunctionIs5way))
   {
      int Iw = VertexEdgeIndex(Object,Vs,Nedge);

      switch (Iw)
      {
         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   18.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            sMensaje+="TX5 ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsW) && (St == JunctionIs6way))
   {
      int Iw = VertexEdgeIndex(Object,Vs,Nedge);

      switch (Iw)
      {
         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   12.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            sMensaje+="TX6 ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsW) && (St == JunctionIsY))
   {
      int Iw = VertexEdgeIndex(Object,Vs,Nedge);

      switch (Iw)
      {
         case 0:
            Object->Eprobabilities[Nedge].Amerit =   13.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =  264.5;
            Object->Eprobabilities[Nedge].Dmerit =   18.5;
         break;

         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit = 1780.5;
            Object->Eprobabilities[Nedge].Dmerit =  239.5;
         break;

         case 2:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    9.5;
            Object->Eprobabilities[Nedge].Cmerit =  244.5;
            Object->Eprobabilities[Nedge].Dmerit =   26.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            sMensaje+="TY ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsX) && (St == JunctionIsM))
   {
      int Im = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Im)
      {
         case 0:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    0.25;
            Object->Eprobabilities[Nedge].Dmerit =    0.25;
         break;

         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   19.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         case 2:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   22.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SX ";
            sMensajeAux.Format("%d", Im);
            sMensaje+=sMensajeAux;
            sMensaje+="TM ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsX) && (St == JunctionIsT4))
   {
      int It = VertexEdgeIndex(Object,Vt,Nedge);

      switch (It)
      {
         case 0:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    2.5;
            Object->Eprobabilities[Nedge].Cmerit =    0.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    0.25;
            Object->Eprobabilities[Nedge].Dmerit =    0.25;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SX ";
            sMensajeAux.Format("%d", It);
            sMensaje+=sMensajeAux;
            sMensaje+="TT ";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsX) && (St == JunctionIsW))
   {
      int Iw = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Iw)
      {
         case 0:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    7.5;
            Object->Eprobabilities[Nedge].Dmerit =    4.5;
         break;

         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =  172.5;
            Object->Eprobabilities[Nedge].Dmerit =   14.5;
         break;

         case 2:
            Object->Eprobabilities[Nedge].Amerit =    1.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =   10.5;
            Object->Eprobabilities[Nedge].Dmerit =    3.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SX TW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsX) && (St == JunctionIsX))
   {
      Object->Eprobabilities[Nedge].Amerit =   0.25;
      Object->Eprobabilities[Nedge].Bmerit =   0.25;
      Object->Eprobabilities[Nedge].Cmerit =  70.5;
      Object->Eprobabilities[Nedge].Dmerit =   6.5;
   }
   else
   if ((Ss == JunctionIsX) && (St == JunctionIs5way))
   {
      Object->Eprobabilities[Nedge].Amerit =   0.25;
      Object->Eprobabilities[Nedge].Bmerit =   0.25;
      Object->Eprobabilities[Nedge].Cmerit =   3.5;
      Object->Eprobabilities[Nedge].Dmerit =   1.5;
   }
   else
   if ((Ss == JunctionIsX) && (St == JunctionIsY))
   {
      Object->Eprobabilities[Nedge].Amerit =   0.25;
      Object->Eprobabilities[Nedge].Bmerit =   0.25;
      Object->Eprobabilities[Nedge].Cmerit =  25.5;
      Object->Eprobabilities[Nedge].Dmerit =  20.5;
   }
   else
   if ((Ss == JunctionIs5way) && (St == JunctionIsM))
   {
      int Im = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Im)
      {
         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   11.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 2:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   11.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SX5 TM ";
            sMensajeAux.Format("%d", Im);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIs5way) && (St == JunctionIsW))
   {
      int Iw = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Iw)
      {
         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   18.5;
            Object->Eprobabilities[Nedge].Dmerit =    2.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SX5 TW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIs5way) && (St == JunctionIsX))
   {
      Object->Eprobabilities[Nedge].Amerit =   0.25;
      Object->Eprobabilities[Nedge].Bmerit =   0.25;
      Object->Eprobabilities[Nedge].Cmerit =   3.5;
      Object->Eprobabilities[Nedge].Dmerit =   1.5;
   }
   else
   if ((Ss == JunctionIs5way) && (St == JunctionIs5way))
   {
      Object->Eprobabilities[Nedge].Amerit =   0.25;
      Object->Eprobabilities[Nedge].Bmerit =   0.25;
      Object->Eprobabilities[Nedge].Cmerit =  71.5;
      Object->Eprobabilities[Nedge].Dmerit =   0.5;
   }
   else
   if ((Ss == JunctionIs5way) && (St == JunctionIsY))
   {
      Object->Eprobabilities[Nedge].Amerit =   0.25;
      Object->Eprobabilities[Nedge].Bmerit =   0.25;
      Object->Eprobabilities[Nedge].Cmerit =   3.5;
      Object->Eprobabilities[Nedge].Dmerit =   0.5;
   }
   else
   if ((Ss == JunctionIs6way) && (St == JunctionIsX))
   {
      Object->Eprobabilities[Nedge].Amerit =   0.25;
      Object->Eprobabilities[Nedge].Bmerit =   0.25;
      Object->Eprobabilities[Nedge].Cmerit =   3.5;
      Object->Eprobabilities[Nedge].Dmerit =   3.5;
   }
   else
   if ((Ss == JunctionIs6way) && (St == JunctionIsY))
   {
      Object->Eprobabilities[Nedge].Amerit =   0.25;
      Object->Eprobabilities[Nedge].Bmerit =   0.25;
      Object->Eprobabilities[Nedge].Cmerit =   3.5;
      Object->Eprobabilities[Nedge].Dmerit =   3.5;
   }
   else
   if ((Ss == JunctionIsY) && (St == JunctionIsK))
   {
      int Ik = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Ik)
      {
         case 0:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   12.5;
            Object->Eprobabilities[Nedge].Dmerit =    6.5;
         break;

         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   14.5;
            Object->Eprobabilities[Nedge].Dmerit =    5.5;
         break;

         case 2:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =    9.5;
            Object->Eprobabilities[Nedge].Dmerit =   19.5;
         break;

         case 3:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   15.5;
            Object->Eprobabilities[Nedge].Dmerit =   20.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SY TK ";
            sMensajeAux.Format("%d", Ik);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsY) && (St == JunctionIsL))
   {
      int Il = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Il)
      {
         case 0:
            Object->Eprobabilities[Nedge].Amerit =    4.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    3.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    3.5;
            Object->Eprobabilities[Nedge].Cmerit =    2.5;
            Object->Eprobabilities[Nedge].Dmerit =    0.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SY TL ";
            sMensajeAux.Format("%d", Il);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsY) && (St == JunctionIsM))
   {
      int Im = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Im)
      {
         case 0:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =    1.5;
            Object->Eprobabilities[Nedge].Cmerit =   11.5;
            Object->Eprobabilities[Nedge].Dmerit =    6.5;
         break;

         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   33.5;
            Object->Eprobabilities[Nedge].Dmerit =   10.5;
         break;

         case 2:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit =   28.5;
            Object->Eprobabilities[Nedge].Dmerit =   11.5;
         break;

         case 3:
            Object->Eprobabilities[Nedge].Amerit =    3.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =   10.5;
            Object->Eprobabilities[Nedge].Dmerit =    3.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SY TM ";
            sMensajeAux.Format("%d", Im);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsY) && (St == JunctionIsT4))
   {
      int It = VertexEdgeIndex(Object,Vt,Nedge);

      switch (It)
      {
         case 0:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =   14.5;
            Object->Eprobabilities[Nedge].Cmerit =    5.5;
            Object->Eprobabilities[Nedge].Dmerit =    4.5;
         break;

         case 1:
            Object->Eprobabilities[Nedge].Amerit =    2.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =    5.5;
            Object->Eprobabilities[Nedge].Dmerit =   11.5;
         break;

         case 2:
            Object->Eprobabilities[Nedge].Amerit =   13.5;
            Object->Eprobabilities[Nedge].Bmerit =   13.5;
            Object->Eprobabilities[Nedge].Cmerit =   21.5;
            Object->Eprobabilities[Nedge].Dmerit =   16.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SY TT ";
            sMensajeAux.Format("%d", It);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsY) && (St == JunctionIsW))
   {
      int Iw = VertexEdgeIndex(Object,Vt,Nedge);

      switch (Iw)
      {
         case 0:
            Object->Eprobabilities[Nedge].Amerit =    0.5;
            Object->Eprobabilities[Nedge].Bmerit =   13.5;
            Object->Eprobabilities[Nedge].Cmerit =  264.5;
            Object->Eprobabilities[Nedge].Dmerit =   18.5;
         break;

         case 1:
            Object->Eprobabilities[Nedge].Amerit =    0.25;
            Object->Eprobabilities[Nedge].Bmerit =    0.25;
            Object->Eprobabilities[Nedge].Cmerit = 1780.5;
            Object->Eprobabilities[Nedge].Dmerit =  239.5;
         break;

         case 2:
            Object->Eprobabilities[Nedge].Amerit =    9.5;
            Object->Eprobabilities[Nedge].Bmerit =    0.5;
            Object->Eprobabilities[Nedge].Cmerit =  244.5;
            Object->Eprobabilities[Nedge].Dmerit =   26.5;
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: SY TW ";
            sMensajeAux.Format("%d", Iw);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
      }
   }
   else
   if ((Ss == JunctionIsY) && (St == JunctionIsX))
   {
      Object->Eprobabilities[Nedge].Amerit =   0.25;
      Object->Eprobabilities[Nedge].Bmerit =   0.25;
      Object->Eprobabilities[Nedge].Cmerit =  25.5;
      Object->Eprobabilities[Nedge].Dmerit =  20.5;
   }
   else
   if ((Ss == JunctionIsY) && (St == JunctionIs5way))
   {
      Object->Eprobabilities[Nedge].Amerit =   0.25;
      Object->Eprobabilities[Nedge].Bmerit =   0.25;
      Object->Eprobabilities[Nedge].Cmerit =   3.5;
      Object->Eprobabilities[Nedge].Dmerit =   0.5;
   }
   else
   if ((Ss == JunctionIsY) && (St == JunctionIs6way))
   {
      Object->Eprobabilities[Nedge].Amerit =   0.25;
      Object->Eprobabilities[Nedge].Bmerit =   0.25;
      Object->Eprobabilities[Nedge].Cmerit =   3.5;
      Object->Eprobabilities[Nedge].Dmerit =   3.5;
   }
   else
   if ((Ss == JunctionIsY) && (St == JunctionIsY))
   {
      Object->Eprobabilities[Nedge].Amerit =   2.75;
      Object->Eprobabilities[Nedge].Bmerit =   2.75;
      Object->Eprobabilities[Nedge].Cmerit = 283.5;
      Object->Eprobabilities[Nedge].Dmerit =   9.5;
   }
   else
   {
      CString sMensaje, sMensajeAux;
      sMensaje= "Labelling error: S ";
      sMensajeAux.Format("%c", Ss);
      sMensaje+=sMensajeAux;
      sMensaje+="TT ";
      sMensajeAux.Format("%c", St);
      sMensaje+=sMensajeAux;
      AfxMessageBox(sMensaje);
      //exit(BADNEWS);
   }

   Object->Eprobabilities[Nedge].Amerit = LC->tcEa;
   Object->Eprobabilities[Nedge].Bmerit = LC->tcEa;
   Object->Eprobabilities[Nedge].Cmerit = LC->tcEc;
   Object->Eprobabilities[Nedge].Dmerit = LC->tcEd;
}


void CCueLabelsVarley_Relaxation::NormaliseEProbabilities (EPROBABILITIES *EP)
{
   double D = EP->Amerit + EP->Bmerit + EP->Cmerit + EP->Dmerit;
   if (D > 0.0)
   {
      EP->Amerit /= D;
      EP->Bmerit /= D;
      EP->Cmerit /= D;
      EP->Dmerit /= D;
   }
}


void CCueLabelsVarley_Relaxation::InitialiseEdgeProbabilitiesUsingShapePairs (OBJECT *Object)
{
   for (EDGE_INDEX n=0; n<Object->Labelling.EdgeCount; ++n)
   {
      switch (Object->Labelling.EdgeLabel[n])
      {
         case LineIsArrow:
            LabelEdgeAsArrow(Object,n);
         break;

         case LineIsBackArrow:
            LabelEdgeAsBackArrow(Object,n);
         break;

         case LineIsUnknown:
            InitialiseSingleEdgeProbabilitiesUsingShapePairs(Object,n);
            NormaliseEProbabilities(&Object->Eprobabilities[n]);
         break;

         case LineIsConvex|LineIsConcave:
            InitialiseSingleEdgeProbabilitiesUsingShapePairs(Object,n);
            Object->Eprobabilities[n].Amerit = 0.0;
            Object->Eprobabilities[n].Bmerit = 0.0;
            NormaliseEProbabilities(&Object->Eprobabilities[n]);
         break;
      }
   }
}


void CCueLabelsVarley_Relaxation::InitialiseEdgeProbabilities (OBJECT *Object)
{
   for (EDGE_INDEX n=0; n<Object->Labelling.EdgeCount; ++n)
   {
      Object->Eprobabilities[n].Amerit = (Object->Labelling.EdgeLabel[n] & LineIsArrow)     ? LC->tcEa : 0.0;
      Object->Eprobabilities[n].Bmerit = (Object->Labelling.EdgeLabel[n] & LineIsBackArrow) ? LC->tcEa : 0.0;
      Object->Eprobabilities[n].Cmerit = (Object->Labelling.EdgeLabel[n] & LineIsConvex)    ? LC->tcEc : 0.0;
      Object->Eprobabilities[n].Dmerit = (Object->Labelling.EdgeLabel[n] & LineIsConcave)   ? LC->tcEd : 0.0;
   }
}


void CCueLabelsVarley_Relaxation::InitialiseRelaxationEdgeLabels (OBJECT *Object)
{
   if (UseShapePairStatisticsForRelaxationLabelling)
      InitialiseEdgeProbabilitiesUsingShapePairs(Object);
   else
      InitialiseEdgeProbabilities(Object);
}


void CCueLabelsVarley_Relaxation::InitialiseRelaxationKJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   TYPEMARK KT = Object->Labelling.VertexLabel[Nvert].Vexity;
   MERIT *KP = Object->Vprobabilities[Nvert];

   KP[Kabcdmerit] = (KT & Kabcd) ? LC->tcKabcd : 0.0;
   KP[Kabdcmerit] = (KT & Kabdc) ? LC->tcKabcd : 0.0;
   KP[Kcccdmerit] = (KT & Kcccd) ? LC->tcKcccd : 0.0;
   KP[Kccdcmerit] = (KT & Kccdc) ? LC->tcKcccd : 0.0;
   KP[Kcdcdmerit] = (KT & Kcdcd) ? LC->tcKcdcd : 0.0;
   KP[Kdcdcmerit] = (KT & Kdcdc) ? LC->tcKcdcd : 0.0;
   KP[Kddcdmerit] = (KT & Kddcd) ? LC->tcKddcd : 0.0;
   KP[Kdddcmerit] = (KT & Kdddc) ? LC->tcKddcd : 0.0;
}


void CCueLabelsVarley_Relaxation::InitialiseRelaxationLJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   TYPEMARK LT = Object->Labelling.VertexLabel[Nvert].Vexity;
   MERIT *LP = Object->Vprobabilities[Nvert];

   LP[Lbamerit] = (LT & Lba) ? LC->tcLba : 0.0;
   LP[Labmerit] = (LT & Lab) ? LC->tcLab : 0.0;
   LP[Lacmerit] = (LT & Lac) ? LC->tcLac : 0.0;
   LP[Lcbmerit] = (LT & Lcb) ? LC->tcLac : 0.0;
   LP[Lbdmerit] = (LT & Lbd) ? LC->tcLbd : 0.0;
   LP[Ldamerit] = (LT & Lda) ? LC->tcLbd : 0.0;
   LP[Laamerit] = (LT & Laa) ? LC->tcLaa : 0.0;
   LP[Lbbmerit] = (LT & Lbb) ? LC->tcLaa : 0.0;
}


void CCueLabelsVarley_Relaxation::InitialiseRelaxationMJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   TYPEMARK MT = Object->Labelling.VertexLabel[Nvert].Vexity;
   MERIT *MP = Object->Vprobabilities[Nvert];

   MP[Mbccamerit] = (MT & Mbcca) ? LC->tcMbcca : 0.0;
   MP[Mbcdamerit] = (MT & Mbcda) ? LC->tcMbcda : 0.0;
   MP[Mbdcamerit] = (MT & Mbdca) ? LC->tcMbcda : 0.0;
   MP[Mccdcmerit] = (MT & Mccdc) ? LC->tcMccdc : 0.0;
   MP[Mcdccmerit] = (MT & Mcdcc) ? LC->tcMccdc : 0.0;
   MP[Mcdcdmerit] = (MT & Mcdcd) ? LC->tcMcdcd : 0.0;
   MP[Mcddcmerit] = (MT & Mcddc) ? LC->tcMcddc : 0.0;
   MP[Mdccdmerit] = (MT & Mdccd) ? LC->tcMdccd : 0.0;
   MP[Mdcdcmerit] = (MT & Mdcdc) ? LC->tcMcdcd : 0.0;
   MP[Mdcddmerit] = (MT & Mdcdd) ? LC->tcMdcdd : 0.0;
   MP[Mddcdmerit] = (MT & Mddcd) ? LC->tcMdcdd : 0.0;
}


void CCueLabelsVarley_Relaxation::InitialiseRelaxationTJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   TYPEMARK TT = Object->Labelling.VertexLabel[Nvert].Vexity;
   MERIT *TP = Object->Vprobabilities[Nvert];

   TP[Tbaamerit] = (TT & Tbaa) ? LC->tcTbaa : 0.0;
   TP[Tbabmerit] = (TT & Tbab) ? LC->tcTbaa : 0.0;
   TP[Tbacmerit] = (TT & Tbac) ? LC->tcTbac : 0.0;
   TP[Tbadmerit] = (TT & Tbad) ? LC->tcTbad : 0.0;
   TP[Tbdamerit] = (TT & Tbda) ? LC->tcTbda : 0.0;
   TP[Tbdcmerit] = (TT & Tbdc) ? LC->tcTbdc : 0.0;
   TP[Tdabmerit] = (TT & Tdab) ? LC->tcTbda : 0.0;
   TP[Tdacmerit] = (TT & Tdac) ? LC->tcTbdc : 0.0;
   TP[Tabamerit] = (TT & Taba) ? LC->tcTaba : 0.0;
   TP[Tabbmerit] = (TT & Tabb) ? LC->tcTaba : 0.0;
   TP[Tbcamerit] = (TT & Tbca) ? LC->tcTbca : 0.0;
   TP[Tbccmerit] = (TT & Tbcc) ? LC->tcTbcc : 0.0;
   TP[Tcabmerit] = (TT & Tcab) ? LC->tcTbca : 0.0;
   TP[Tcacmerit] = (TT & Tcac) ? LC->tcTbcc : 0.0;
   TP[Tccamerit] = (TT & Tcca) ? LC->tcTcca : 0.0;
   TP[Tccbmerit] = (TT & Tccb) ? LC->tcTcca : 0.0;
   TP[Tcdamerit] = (TT & Tcda) ? LC->tcTcda : 0.0;
   TP[Tdcbmerit] = (TT & Tdcb) ? LC->tcTcda : 0.0;
   TP[Tddamerit] = (TT & Tdda) ? LC->tcTdda : 0.0;
   TP[Tddbmerit] = (TT & Tddb) ? LC->tcTdda : 0.0;
}


void CCueLabelsVarley_Relaxation::InitialiseRelaxationWJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   TYPEMARK WT = Object->Labelling.VertexLabel[Nvert].Vexity;
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);

   WP->Wbcamerit = (WT & Wbca) ? LC->tcWbca : 0.0;
   WP->Wcdcmerit = (WT & Wcdc) ? LC->tcWcdc : 0.0;
   WP->Wdcdmerit = (WT & Wdcd) ? LC->tcWdcd : 0.0;
   WP->Wabcmerit = (WT & Wabc) ? LC->tcWabc : 0.0;
   WP->Wabdmerit = (WT & Wabd) ? LC->tcWabd : 0.0;
   WP->Waccmerit = (WT & Wacc) ? LC->tcWacc : 0.0;
   WP->Wacdmerit = (WT & Wacd) ? LC->tcWacd : 0.0;
   WP->Wadcmerit = (WT & Wadc) ? LC->tcWadc : 0.0;
   WP->Wbaamerit = (WT & Wbaa) ? LC->tcWbaa : 0.0;
   WP->Wbbamerit = (WT & Wbba) ? LC->tcWbaa : 0.0;
   WP->Wbcdmerit = (WT & Wbcd) ? LC->tcWbcd : 0.0;
   WP->Wbdamerit = (WT & Wbda) ? LC->tcWbda : 0.0;
   WP->Wbdcmerit = (WT & Wbdc) ? LC->tcWbdc : 0.0;
   WP->Wbddmerit = (WT & Wbdd) ? LC->tcWbdd : 0.0;
   WP->Wcabmerit = (WT & Wcab) ? LC->tcWabc : 0.0;
   WP->Wcacmerit = (WT & Wcac) ? LC->tcWcac : 0.0;
   WP->Wcbcmerit = (WT & Wcbc) ? LC->tcWcac : 0.0;
   WP->Wcbdmerit = (WT & Wcbd) ? LC->tcWcbd : 0.0;
   WP->Wccbmerit = (WT & Wccb) ? LC->tcWacc : 0.0;
   WP->Wcdamerit = (WT & Wcda) ? LC->tcWbdc : 0.0;
   WP->Wcdbmerit = (WT & Wcdb) ? LC->tcWadc : 0.0;
   WP->Wdabmerit = (WT & Wdab) ? LC->tcWabd : 0.0;
   WP->Wdacmerit = (WT & Wdac) ? LC->tcWcbd : 0.0;
   WP->Wdadmerit = (WT & Wdad) ? LC->tcWdad : 0.0;
   WP->Wdbdmerit = (WT & Wdbd) ? LC->tcWdad : 0.0;
   WP->Wdcamerit = (WT & Wdca) ? LC->tcWbcd : 0.0;
   WP->Wdcbmerit = (WT & Wdcb) ? LC->tcWacd : 0.0;
   WP->Wddamerit = (WT & Wdda) ? LC->tcWbdd : 0.0;
}


void CCueLabelsVarley_Relaxation::InitialiseRelaxationXJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   TYPEMARK XT = Object->Labelling.VertexLabel[Nvert].Vexity;
   XPROBABILITIES *XP = (XPROBABILITIES *)(Object->Vprobabilities[Nvert]);

   XP->Xabcdmerit = (XT & Xabcd) ? LC->tcXabcd : 0.0;
   XP->Xbcdamerit = (XT & Xbcda) ? LC->tcXabcd : 0.0;
   XP->Xcdabmerit = (XT & Xcdab) ? LC->tcXabcd : 0.0;
   XP->Xdabcmerit = (XT & Xdabc) ? LC->tcXabcd : 0.0;
   XP->Xabdcmerit = (XT & Xabdc) ? LC->tcXabcd : 0.0;
   XP->Xbdcamerit = (XT & Xbdca) ? LC->tcXabcd : 0.0;
   XP->Xdcabmerit = (XT & Xdcab) ? LC->tcXabcd : 0.0;
   XP->Xcabdmerit = (XT & Xcabd) ? LC->tcXabcd : 0.0;
   XP->Xabddmerit = (XT & Xabdd) ? LC->tcXabdd : 0.0;
   XP->Xbddamerit = (XT & Xbdda) ? LC->tcXabdd : 0.0;
   XP->Xddabmerit = (XT & Xddab) ? LC->tcXabdd : 0.0;
   XP->Xdabdmerit = (XT & Xdabd) ? LC->tcXabdd : 0.0;
   XP->Xccccmerit = (XT & Xcccc) ? LC->tcXcccc : 0.0;
   XP->Xcccdmerit = (XT & Xcccd) ? LC->tcXcccd : 0.0;
   XP->Xccdcmerit = (XT & Xccdc) ? LC->tcXcccd : 0.0;
   XP->Xcdccmerit = (XT & Xcdcc) ? LC->tcXcccd : 0.0;
   XP->Xdcccmerit = (XT & Xdccc) ? LC->tcXcccd : 0.0;
   XP->Xcdcdmerit = (XT & Xcdcd) ? LC->tcXcdcd : 0.0;
   XP->Xdcdcmerit = (XT & Xdcdc) ? LC->tcXcdcd : 0.0;
   XP->Xcdddmerit = (XT & Xcddd) ? LC->tcXcddd : 0.0;
   XP->Xdcddmerit = (XT & Xdcdd) ? LC->tcXcddd : 0.0;
   XP->Xddcdmerit = (XT & Xddcd) ? LC->tcXcddd : 0.0;
   XP->Xdddcmerit = (XT & Xdddc) ? LC->tcXcddd : 0.0;
   XP->Xddddmerit = (XT & Xdddd) ? LC->tcXdddd : 0.0;
}


void CCueLabelsVarley_Relaxation::InitialiseRelaxationX5Junction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   TYPEMARK XT = Object->Labelling.VertexLabel[Nvert].Vexity;
   MERIT *XP = Object->Vprobabilities[Nvert];

   XP[Xcccccmerit] = (XT & Xccccc) ? LC->tcXccccc : 0.0;
   XP[Xdddddmerit] = (XT & Xddddd) ? LC->tcXddddd : 0.0;
}


void CCueLabelsVarley_Relaxation::InitialiseRelaxationX6Junction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   TYPEMARK XT = Object->Labelling.VertexLabel[Nvert].Vexity;
   MERIT *XP = Object->Vprobabilities[Nvert];

   XP[Xccccccmerit] = (XT & Xcccccc) ? LC->tcXcccccc : 0.0;
   XP[Xcdcdcdmerit] = (XT & Xcdcdcd) ? LC->tcXcdcdcd : 0.0;
   XP[Xdcdcdcmerit] = (XT & Xdcdcdc) ? LC->tcXcdcdcd : 0.0;
   XP[Xddddddmerit] = (XT & Xdddddd) ? LC->tcXdddddd : 0.0;
}


void CCueLabelsVarley_Relaxation::InitialiseRelaxationYJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   TYPEMARK YT = Object->Labelling.VertexLabel[Nvert].Vexity;
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);

   YP->Ycccmerit = (YT & Yccc) ? LC->tcYccc : 0.0;
   YP->Ydddmerit = (YT & Yddd) ? LC->tcYddd : 0.0;
   YP->Yabdmerit = (YT & Yabd) ? LC->tcYabd : 0.0;
   YP->Ybdamerit = (YT & Ybda) ? LC->tcYabd : 0.0;
   YP->Ydabmerit = (YT & Ydab) ? LC->tcYabd : 0.0;
   YP->Yaabmerit = (YT & Yaab) ? LC->tcYaab : 0.0;
   YP->Yabamerit = (YT & Yaba) ? LC->tcYaab : 0.0;
   YP->Ybaamerit = (YT & Ybaa) ? LC->tcYaab : 0.0;
   YP->Yabbmerit = (YT & Yabb) ? LC->tcYaab : 0.0;
   YP->Ybbamerit = (YT & Ybba) ? LC->tcYaab : 0.0;
   YP->Ybabmerit = (YT & Ybab) ? LC->tcYaab : 0.0;
   YP->Yabcmerit = (YT & Yabc) ? LC->tcYabc : 0.0;
   YP->Ybcamerit = (YT & Ybca) ? LC->tcYabc : 0.0;
   YP->Ycabmerit = (YT & Ycab) ? LC->tcYabc : 0.0;
   YP->Yaccmerit = (YT & Yacc) ? LC->tcYacc : 0.0;
   YP->Yccamerit = (YT & Ycca) ? LC->tcYacc : 0.0;
   YP->Ycacmerit = (YT & Ycac) ? LC->tcYacc : 0.0;
   YP->Yacdmerit = (YT & Yacd) ? LC->tcYacd : 0.0;
   YP->Ycdamerit = (YT & Ycda) ? LC->tcYacd : 0.0;
   YP->Ydacmerit = (YT & Ydac) ? LC->tcYacd : 0.0;
   YP->Yaddmerit = (YT & Yadd) ? LC->tcYadd : 0.0;
   YP->Yddamerit = (YT & Ydda) ? LC->tcYadd : 0.0;
   YP->Ydadmerit = (YT & Ydad) ? LC->tcYadd : 0.0;
   YP->Ybccmerit = (YT & Ybcc) ? LC->tcYacc : 0.0;
   YP->Yccbmerit = (YT & Yccb) ? LC->tcYacc : 0.0;
   YP->Ycbcmerit = (YT & Ycbc) ? LC->tcYacc : 0.0;
   YP->Ybdcmerit = (YT & Ybdc) ? LC->tcYacd : 0.0;
   YP->Ydcbmerit = (YT & Ydcb) ? LC->tcYacd : 0.0;
   YP->Ycbdmerit = (YT & Ycbd) ? LC->tcYacd : 0.0;
   YP->Ybddmerit = (YT & Ybdd) ? LC->tcYadd : 0.0;
   YP->Yddbmerit = (YT & Yddb) ? LC->tcYadd : 0.0;
   YP->Ydbdmerit = (YT & Ydbd) ? LC->tcYadd : 0.0;
}


void CCueLabelsVarley_Relaxation::InitialiseRelaxationVertexLabels (OBJECT *Object)
{
   for (int n=0; n<Object->Labelling.VertexCount; ++n)
   {
      Object->Labelling.VertexLabellingIsFinalised[n] = cFALSE;

      switch(Object->Labelling.VertexLabel[n].Shape)
      {
         case JunctionIsK:
            InitialiseRelaxationKJunction(Object,n);
         break;

         case JunctionIsL:
            InitialiseRelaxationLJunction(Object,n);
         break;

         case JunctionIsM:
            InitialiseRelaxationMJunction(Object,n);
         break;

         case JunctionIsT:
         case JunctionIsT4:
            InitialiseRelaxationTJunction(Object,n);
         break;

         case JunctionIsW:
            InitialiseRelaxationWJunction(Object,n);
         break;

         case JunctionIsX:
            InitialiseRelaxationXJunction(Object,n);
         break;

         case JunctionIs5way:
            InitialiseRelaxationX5Junction(Object,n);
         break;

         case JunctionIsZ6:
         case JunctionIs6way:
            InitialiseRelaxationX6Junction(Object,n);
         break;

         case JunctionIsY:
            InitialiseRelaxationYJunction(Object,n);
         break;

         case JunctionIsZ:
            // nothing to do for Z-junctions
         break;
      }
   }
}


void CCueLabelsVarley_Relaxation::UpdateRelaxationProbabilitiesFromFeatures (OBJECT *Object)
{
   for (int i=0;  i<Object->CofacialCount;  ++i)
   {
      COFACIAL_CONFIGURATION *C = &Object->CofacialConfigurations[i];
      //printf("Updating outer vertex %d with cofacial configuration merit %f\n",C->OuterVertex,C->Merit);

      if (VertexLabelsIdentical(C->ExpectedOuterType,&LP->LabelLba))
      {
         Object->Vprobabilities[C->OuterVertex][Lbamerit] += C->Merit*LC->tcSq;
      }
      else
      if (C->ExpectedOuterType->Shape == JunctionIsW)
      {
         if (C->ExpectedOuterType->Vexity == Wcdc)
         {
            WPROBABILITIES *WP = (WPROBABILITIES *)(Object->Vprobabilities[C->OuterVertex]);
	        WP->Wcdcmerit += C->Merit*LC->tcSq;
         }
         else
         if (C->ExpectedOuterType->Vexity == Wbca)
         {
            WPROBABILITIES *WP = (WPROBABILITIES *)(Object->Vprobabilities[C->OuterVertex]);
	        WP->Wbcamerit += C->Merit*LC->tcSq;
         }
      }
      else
      if (C->ExpectedOuterType->Shape == JunctionIsY)
      {
         if (C->ExpectedOuterType->Vexity == Yccc)
         {
            YPROBABILITIES *YP = (YPROBABILITIES *)(Object->Vprobabilities[C->OuterVertex]);
	        YP->Ycccmerit += C->Merit*LC->tcSq;
         }
         else
         if (C->ExpectedOuterType->Vexity == Yabd)
         {
            YPROBABILITIES *YP = (YPROBABILITIES *)(Object->Vprobabilities[C->OuterVertex]);
	        YP->Yabdmerit += C->Merit*LC->tcSq/3.0;
	        YP->Ybdamerit += C->Merit*LC->tcSq/3.0;
	        YP->Ydabmerit += C->Merit*LC->tcSq/3.0;
         }
      }

      //printf("Updating outer vertex %d with cofacial configuration merit %f\n",C->InnerVertex,C->Merit);

      if (C->ExpectedInnerType->Shape == JunctionIsL)
      {
         //printf(" ... inner L\n");
         if (C->ExpectedInnerType->Vexity == Lab)
         {
            //printf(" ... inner Lab\n");
            Object->Vprobabilities[C->InnerVertex][Labmerit] += C->Merit*LC->tcSq;
         }
         else
         if (C->ExpectedInnerType->Vexity == Lac)
         {
            //printf(" ... inner Lac\n");
            Object->Vprobabilities[C->InnerVertex][Lacmerit] += C->Merit*LC->tcSq;
         }
         else
         if (C->ExpectedInnerType->Vexity == Lbd)
         {
            //printf(" ... inner Lbd\n");
            Object->Vprobabilities[C->InnerVertex][Lbdmerit] += C->Merit*LC->tcSq;
         }
         else
         if (C->ExpectedInnerType->Vexity == Lcb)
         {
            //printf(" ... inner Lcb\n");
            Object->Vprobabilities[C->InnerVertex][Lcbmerit] += C->Merit*LC->tcSq;
         }
         else
         if (C->ExpectedInnerType->Vexity == Lda)
         {
            //printf(" ... inner Lda\n");
            Object->Vprobabilities[C->InnerVertex][Ldamerit] += C->Merit*LC->tcSq;
         }
      }
      else
      if (C->ExpectedInnerType->Shape == JunctionIsW)
      {
         //printf(" ... inner W\n");
         if (C->ExpectedInnerType->Vexity == Wcdc)
         {
            WPROBABILITIES *WP = (WPROBABILITIES *)(Object->Vprobabilities[C->InnerVertex]);
	        WP->Wcdcmerit += C->Merit*LC->tcSq;
         }
         else
         if (C->ExpectedInnerType->Vexity == Wdcd)
         {
            WPROBABILITIES *WP = (WPROBABILITIES *)(Object->Vprobabilities[C->InnerVertex]);
	        WP->Wdcdmerit += C->Merit*LC->tcSq;
         }
      }
   }

   for (int i=0;  i<Object->FeatureCount;  ++i)
   {
      FEATURE *F = &Object->Features[i];

      switch (F->FeatureType)
      {
         case FEATURE_WY:
            //AfxMessageBox("Labelling error: Lrel Feat WY ");
            //exit(BADNEWS);
         break;

         case FEATURE_UNDERSLOT:
            {
  		       MERIT *TP = Object->Vprobabilities[F->T];
  		       WPROBABILITIES *WP = (WPROBABILITIES *)(Object->Vprobabilities[F->W]);
  		       Object->Vprobabilities[F->L][Lbamerit] += F->Merit*LC->tcSr;
               Object->Vprobabilities[F->B][Labmerit] += F->Merit*LC->tcSr;

               TP[Tbaamerit] += F->Merit*LC->tcSr;
               TP[Tbabmerit] += F->Merit*LC->tcSr;
  		       WP->Wbcamerit += F->Merit*LC->tcSr;
            }
         break;

         case FEATURE_VALLEY:
            {
  		       MERIT *TP = Object->Vprobabilities[F->T];
               WPROBABILITIES *WP = (WPROBABILITIES *)(Object->Vprobabilities[F->W]);
  		       Object->Vprobabilities[F->L][Lbamerit] += F->Merit*LC->tcSr;
               TP[Tbaamerit] += F->Merit*LC->tcSr;
               TP[Tbabmerit] += F->Merit*LC->tcSr;
               WP->Wbcamerit += F->Merit*LC->tcSr;
            }
         break;

         default:
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: Lrel Feat ";
            sMensajeAux.Format("%x", F->FeatureType);
            sMensaje+=sMensajeAux;
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
         break;
      }
   }
}


void CCueLabelsVarley_Relaxation::RenormaliseVertexProbabilities (OBJECT *Object)
{
   for (int n=0; n<Object->Labelling.VertexCount; ++n)
   {
      switch(Object->Labelling.VertexLabel[n].Shape)
      {
         case JunctionIsK:    NormaliseMerits(Object->Vprobabilities[n],KMERITS);  break;
         case JunctionIsL:    NormaliseMerits(Object->Vprobabilities[n],LMERITS);  break;
         case JunctionIsM:    NormaliseMerits(Object->Vprobabilities[n],MMERITS);  break;
         case JunctionIsT:
         case JunctionIsT4:   NormaliseMerits(Object->Vprobabilities[n],TMERITS);  break;
         case JunctionIsW:    NormaliseWProbabilities(Object->Vprobabilities[n]);  break;
         case JunctionIsX:    NormaliseXProbabilities(Object->Vprobabilities[n]);  break;
         case JunctionIs5way: NormaliseMerits(Object->Vprobabilities[n],X5MERITS);  break;
         case JunctionIsZ6:
         case JunctionIs6way: NormaliseMerits(Object->Vprobabilities[n],X6MERITS);  break;
         case JunctionIsY:    NormaliseYProbabilities(Object->Vprobabilities[n]);  break;
         case JunctionIsZ:    break;
      }
   }
}


void CCueLabelsVarley_Relaxation::RenormaliseEdgeProbabilities (OBJECT *Object)
{
   for (EDGE_INDEX n=0; n<Object->Labelling.EdgeCount; ++n)  NormaliseEProbabilities(&Object->Eprobabilities[n]);
}


double CCueLabelsVarley_Relaxation::GetProgramOptionValue (OBJECT *Object, char *Option)
{
   int a = (Option[0] >= 'a') ? (Option[0]-'a') : (Option[0] >= 'A') ? (Option[0]-'A') : Option[0];
   int b = (Option[1] >= 'a') ? (Option[1]-'a') : (Option[1] >= 'A') ? (Option[1]-'A') : Option[1];
   return IsOptionSpecified[a][b] ? OptionValue[a][b] : 0.0;
}


lBOOLEAN CCueLabelsVarley_Relaxation::IsProgramOptionSpecified (OBJECT *Object, char *Option)
{
   int a = (Option[0] >= 'a') ? (Option[0]-'a') : (Option[0] >= 'A') ? (Option[0]-'A') : Option[0];
   int b = (Option[1] >= 'a') ? (Option[1]-'a') : (Option[1] >= 'A') ? (Option[1]-'A') : Option[1];
   return IsOptionSpecified[a][b];
}



MERIT CCueLabelsVarley_Relaxation::PLVertexIsK (OBJECT *Object, VERTEX_INDEX Nvert)
{
   MERIT *LP = Object->Vprobabilities[Nvert];
   return max(LP[Laamerit],LP[Lbbmerit]);
}


MERIT CCueLabelsVarley_Relaxation::PLVertexIsX (OBJECT *Object, VERTEX_INDEX Nvert)
{
   MERIT *LP = Object->Vprobabilities[Nvert];
   return max(LP[Laamerit],LP[Lbbmerit]);
}


MERIT CCueLabelsVarley_Relaxation::PTVertexIsK (OBJECT *Object, VERTEX_INDEX Nvert)
{
   MERIT *TP = Object->Vprobabilities[Nvert];
   return dmax4(dmax3(TP[Tabamerit],TP[Tabbmerit],TP[Tbcamerit]),
                dmax3(TP[Tbccmerit],TP[Tcabmerit],TP[Tcacmerit]),
                dmax3(TP[Tccamerit],TP[Tccbmerit],TP[Tcdamerit]),
                dmax3(TP[Tdcbmerit],TP[Tddamerit],TP[Tddbmerit]));
}


MERIT CCueLabelsVarley_Relaxation::PWVertexIsK (OBJECT *Object, VERTEX_INDEX Nvert)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   return dmax4(WP->Wabdmerit,WP->Wacdmerit,WP->Wdabmerit,WP->Wdcbmerit);
}


MERIT CCueLabelsVarley_Relaxation::PWVertexIsX (OBJECT *Object, VERTEX_INDEX Nvert)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   return dmax5(dmax5(WP->Wabcmerit,WP->Wabdmerit,WP->Waccmerit,WP->Wacdmerit,WP->Wadcmerit),
                dmax5(WP->Wbaamerit,WP->Wbbamerit,WP->Wbcdmerit,WP->Wbdamerit,WP->Wbdcmerit),
                dmax5(WP->Wbddmerit,WP->Wcabmerit,WP->Wcacmerit,WP->Wcbcmerit,WP->Wcbdmerit),
                dmax5(WP->Wccbmerit,WP->Wcdamerit,WP->Wcdbmerit,WP->Wdabmerit,WP->Wdacmerit),
                dmax5(WP->Wdadmerit,WP->Wdbdmerit,WP->Wdcamerit,WP->Wdcbmerit,WP->Wddamerit));
}


MERIT CCueLabelsVarley_Relaxation::PYVertexIsK (OBJECT *Object, VERTEX_INDEX Nvert)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   return dmax4(dmax3(YP->Yacdmerit,YP->Ycdamerit,YP->Ydacmerit),
                dmax3(YP->Yaddmerit,YP->Yddamerit,YP->Ydadmerit),
                dmax3(YP->Ybdcmerit,YP->Ydcbmerit,YP->Ycbdmerit),
                dmax3(YP->Ybddmerit,YP->Yddbmerit,YP->Ydbdmerit));
}

MERIT CCueLabelsVarley_Relaxation::PYVertexIsX (OBJECT *Object, VERTEX_INDEX Nvert)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   return dmax9(dmax3(YP->Yaabmerit,YP->Yabamerit,YP->Ybaamerit),
                dmax3(YP->Yabbmerit,YP->Ybbamerit,YP->Ybabmerit),
                dmax3(YP->Yabcmerit,YP->Ybcamerit,YP->Ycabmerit),
                dmax3(YP->Yaccmerit,YP->Yccamerit,YP->Ycacmerit),
                dmax3(YP->Yacdmerit,YP->Ycdamerit,YP->Ydacmerit),
                dmax3(YP->Yaddmerit,YP->Yddamerit,YP->Ydadmerit),
                dmax3(YP->Ybccmerit,YP->Yccbmerit,YP->Ycbcmerit),
                dmax3(YP->Ybdcmerit,YP->Ydcbmerit,YP->Ycbdmerit),
                dmax3(YP->Ybddmerit,YP->Yddbmerit,YP->Ydbdmerit));
}


MERIT CCueLabelsVarley_Relaxation::DetermineKJunctionPresenceMerit (OBJECT *Object)
{
   MERIT m = 0.0;

   for (VERTEX_INDEX v = 0; v < Object->Labelling.VertexCount; ++v)
   {
      MERIT u;

      switch (Object->Labelling.VertexLabel[v].Shape)
      {
         case JunctionIsK:    return 1.0;
         case JunctionIsL:    u = PLVertexIsK(Object,v);  break;
         case JunctionIsT4:   u = PTVertexIsK(Object,v);  break;
         case JunctionIsW:    u = PWVertexIsK(Object,v);  break;
         case JunctionIsY:    u = PYVertexIsK(Object,v);  break;

         case JunctionIsM:
         case JunctionIsX:
         case JunctionIs5way:
         case JunctionIsZ6:
         case JunctionIs6way:
         case JunctionIsZ:    u = 0.0;                       break;
      }

      if (u > m)  m = u;
   }

   return m;
}


MERIT CCueLabelsVarley_Relaxation::DetermineXJunctionPresenceMerit (OBJECT *Object)
{
   MERIT m = 0.0;

   for (VERTEX_INDEX v = 0; v < Object->Labelling.VertexCount; ++v)
   {
      MERIT u;
      //printf("Determine X Junction Presence Merit, v=%d, shape=%c\n", v, Object->Labelling.VertexLabel[v].Shape);

      switch (Object->Labelling.VertexLabel[v].Shape)
      {
         case JunctionIsK:    u = 0.0;                    break;
         case JunctionIsL:    u = PLVertexIsX(Object,v);  break;
         case JunctionIsT4:   u = PTVertexIsX(Object,v);  break;
         case JunctionIsW:    u = PWVertexIsX(Object,v);  break;
         case JunctionIsZ6:   u = 0.0;                    break;
         case JunctionIsY:    u = PYVertexIsX(Object,v);  break;
         case JunctionIsZ:    u = 0.0;                    break;

         case JunctionIsM:
         case JunctionIsX:
         case JunctionIs5way:
         case JunctionIs6way: return 1.0;
      }

      if (u > m)  m = u;
   }

   return m;
}


MERIT CCueLabelsVarley_Relaxation::PEdgeA (OBJECT *Object, VERTEX_INDEX Nvert, int Index)
{
   EDGE_INDEX Nedge = Object->Labelling.VENumber[Nvert][Index];
   return (Object->Labelling.EdgeStartVertex[Nedge] == Nvert) ? Object->Eprobabilities[Nedge].Bmerit : Object->Eprobabilities[Nedge].Amerit;
}


MERIT CCueLabelsVarley_Relaxation::PEdgeB (OBJECT *Object, VERTEX_INDEX Nvert, int Index)
{
   EDGE_INDEX Nedge = Object->Labelling.VENumber[Nvert][Index];
   return (Object->Labelling.EdgeStartVertex[Nedge] == Nvert) ? Object->Eprobabilities[Nedge].Amerit : Object->Eprobabilities[Nedge].Bmerit;
}


MERIT CCueLabelsVarley_Relaxation::PEdgeC (OBJECT *Object, VERTEX_INDEX Nvert, int Index)
{
   return Object->Eprobabilities[Object->Labelling.VENumber[Nvert][Index]].Cmerit;
}


MERIT CCueLabelsVarley_Relaxation::PEdgeD (OBJECT *Object, VERTEX_INDEX Nvert, int Index)
{
   return Object->Eprobabilities[Object->Labelling.VENumber[Nvert][Index]].Dmerit;
}


void CCueLabelsVarley_Relaxation::AdvancedRelax (MERIT *Variable, MERIT Slowness, MERIT NewValue)
// If the new value says that this is impossible, set it to zero
// Otherwise, relax slowly towards the new value
// N.B. this is to ensure that variables don't relax to contradictory values
{
   if (NewValue <= 0.0)
   {
      (*Variable) = 0.0;
   }
   else
   {
      (*Variable) = (*Variable)*Slowness + NewValue*(1.0-Slowness);
   }
}


void CCueLabelsVarley_Relaxation::ClearMerits (MERIT *Merits, int Count, int ExceptFor)
{
   int n;
   for (n=0; n<Count; ++n)  Merits[n] = 0.0;
   if (ExceptFor >= 0)  Merits[ExceptFor] = 1.0;
}


void CCueLabelsVarley_Relaxation::NormaliseMerits (MERIT *Merits, int Count)
{
   int n;
   MERIT D = 0.0;
   for (n=0; n<Count; ++n)  D += Merits[n];
   if (D > 0.0)  for (n=0; n<Count; ++n)  Merits[n] /= D;
}

void CCueLabelsVarley_Relaxation::ClearT (MERIT *TP)
{
   TP[Tbaamerit] = 0.0;
   TP[Tbabmerit] = 0.0;
   TP[Tbacmerit] = 0.0;
   TP[Tbadmerit] = 0.0;

   TP[Tbdamerit] = 0.0;
   TP[Tbdcmerit] = 0.0;
   TP[Tdabmerit] = 0.0;
   TP[Tdacmerit] = 0.0;

   TP[Tabamerit] = 0.0;
   TP[Tabbmerit] = 0.0;
   TP[Tbcamerit] = 0.0;
   TP[Tbccmerit] = 0.0;
   TP[Tcabmerit] = 0.0;
   TP[Tcacmerit] = 0.0;

   TP[Tccamerit] = 0.0;
   TP[Tccbmerit] = 0.0;
   TP[Tcdamerit] = 0.0;
   TP[Tdcbmerit] = 0.0;
   TP[Tddamerit] = 0.0;
   TP[Tddbmerit] = 0.0;
}


void CCueLabelsVarley_Relaxation::UpdateProbabilitiesForMJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   MERIT *MP = Object->Vprobabilities[Nvert];
   MERIT PB0 = PEdgeB(Object,Nvert,0);
   MERIT PC0 = PEdgeC(Object,Nvert,0);
   MERIT PD0 = PEdgeD(Object,Nvert,0);
   MERIT PC1 = PEdgeC(Object,Nvert,1);
   MERIT PD1 = PEdgeD(Object,Nvert,1);
   MERIT PC2 = PEdgeC(Object,Nvert,2);
   MERIT PD2 = PEdgeD(Object,Nvert,2);
   MERIT PA3 = PEdgeA(Object,Nvert,3);
   MERIT PC3 = PEdgeC(Object,Nvert,3);
   MERIT PD3 = PEdgeD(Object,Nvert,3);
   AdvancedRelax(&MP[Mbccamerit],LC->tcSp,PB0 * PC1 * PC2 * PA3);
   AdvancedRelax(&MP[Mbcdamerit],LC->tcSp,PB0 * PC1 * PD2 * PA3);
   AdvancedRelax(&MP[Mbdcamerit],LC->tcSp,PB0 * PD1 * PC2 * PA3);
   AdvancedRelax(&MP[Mccdcmerit],LC->tcSp,PC0 * PC1 * PD2 * PC3);
   AdvancedRelax(&MP[Mcdccmerit],LC->tcSp,PC0 * PD1 * PC2 * PC3);
   AdvancedRelax(&MP[Mcdcdmerit],LC->tcSp,PC0 * PD1 * PC2 * PD3);
   AdvancedRelax(&MP[Mcddcmerit],LC->tcSp,PC0 * PD1 * PD2 * PC3);
   AdvancedRelax(&MP[Mdccdmerit],LC->tcSp,PD0 * PC1 * PC2 * PD3);
   AdvancedRelax(&MP[Mdcdcmerit],LC->tcSp,PD0 * PC1 * PD2 * PC3);
   AdvancedRelax(&MP[Mdcddmerit],LC->tcSp,PD0 * PC1 * PD2 * PD3);
   AdvancedRelax(&MP[Mddcdmerit],LC->tcSp,PD0 * PD1 * PC2 * PD3);
   NormaliseMerits(MP,MMERITS);

   if (MP[Mbccamerit] > 0.9999)
   {
	  FinaliseMLabel(&Object->Labelling,Nvert,Mbcca,LineIsBackArrow,LineIsConvex,LineIsConvex,LineIsArrow);
      ClearMerits(MP,MMERITS,Mbccamerit);
   }
   else
   if (MP[Mbcdamerit] > 0.9999)
   {
	  FinaliseMLabel(&Object->Labelling,Nvert,Mbcda,LineIsBackArrow,LineIsConvex,LineIsConcave,LineIsArrow);
      ClearMerits(MP,MMERITS,Mbcdamerit);
   }
   else
   if (MP[Mbdcamerit] > 0.9999)
   {
	  FinaliseMLabel(&Object->Labelling,Nvert,Mbdca,LineIsBackArrow,LineIsConcave,LineIsConvex,LineIsArrow);
      ClearMerits(MP,MMERITS,Mbdcamerit);
   }
   else
   if (MP[Mccdcmerit] > 0.9999)
   {
	  FinaliseMLabel(&Object->Labelling,Nvert,Mccdc,LineIsConvex,LineIsConvex,LineIsConcave,LineIsConvex);
      ClearMerits(MP,MMERITS,Mccdcmerit);
   }
   else
   if (MP[Mcdccmerit] > 0.9999)
   {
	  FinaliseMLabel(&Object->Labelling,Nvert,Mcdcc,LineIsConvex,LineIsConcave,LineIsConvex,LineIsConvex);
      ClearMerits(MP,MMERITS,Mcdccmerit);
   }
   else
   if (MP[Mcdcdmerit] > 0.9999)
   {
	  FinaliseMLabel(&Object->Labelling,Nvert,Mcdcd,LineIsConvex,LineIsConcave,LineIsConvex,LineIsConcave);
      ClearMerits(MP,MMERITS,Mcdcdmerit);
   }
   else
   if (MP[Mcddcmerit] > 0.9999)
   {
	  FinaliseMLabel(&Object->Labelling,Nvert,Mcddc,LineIsConvex,LineIsConcave,LineIsConcave,LineIsConvex);
      ClearMerits(MP,MMERITS,Mcddcmerit);
   }
   else
   if (MP[Mdccdmerit] > 0.9999)
   {
	  FinaliseMLabel(&Object->Labelling,Nvert,Mdccd,LineIsConcave,LineIsConvex,LineIsConvex,LineIsConcave);
      ClearMerits(MP,MMERITS,Mdccdmerit);
   }
   else
   if (MP[Mdcdcmerit] > 0.9999)
   {
	  FinaliseMLabel(&Object->Labelling,Nvert,Mdcdc,LineIsConcave,LineIsConvex,LineIsConcave,LineIsConvex);
      ClearMerits(MP,MMERITS,Mdcdcmerit);
   }
   else
   if (MP[Mdcddmerit] > 0.9999)
   {
	  FinaliseMLabel(&Object->Labelling,Nvert,Mdcdd,LineIsConcave,LineIsConvex,LineIsConcave,LineIsConcave);
      ClearMerits(MP,MMERITS,Mdcddmerit);
   }
   else
   if (MP[Mddcdmerit] > 0.9999)
   {
	  FinaliseMLabel(&Object->Labelling,Nvert,Mddcd,LineIsConcave,LineIsConcave,LineIsConvex,LineIsConcave);
      ClearMerits(MP,MMERITS,Mddcdmerit);
   }
   else
   {
      //printf("M%d bcca%f bcda%f bdca%f",
      //         Nvert,
      //         MP->Mbccamerit, MP->Mbcdamerit, MP->Mbdcamerit);
      //printf(" ccdc%f cdcc%f cdcd%f cddc%f dccd%f dcdc%f dcdd%f ddcd%f\n",
      //        MP->Mccdcmerit, MP->Mcdccmerit, MP->Mcdcdmerit, MP->Mcddcmerit,
      //        MP->Mdccdmerit, MP->Mdcdcmerit, MP->Mdcddmerit, MP->Mddcdmerit);
   }
}


void CCueLabelsVarley_Relaxation::SetTJunctionOccluding (VERTEX_INDEX Nvert)
{
   PIsOccludingTJunction[Nvert] = 1.0;
   IsOccludingTJunction[Nvert]  = cYES;
}


void CCueLabelsVarley_Relaxation::SetTJunctionNonOccluding (VERTEX_INDEX Nvert)
{
   PIsOccludingTJunction[Nvert] = 0.0;
   IsOccludingTJunction[Nvert]  = cNO;
   Ttail[Nvert] = NO_EDGE;
}


void CCueLabelsVarley_Relaxation::UpdateProbabilitiesForTJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   MERIT *TP = Object->Vprobabilities[Nvert];
   MERIT PA0 = PEdgeA(Object,Nvert,0);
   MERIT PB0 = PEdgeB(Object,Nvert,0);
   MERIT PC0 = PEdgeC(Object,Nvert,0);
   MERIT PD0 = PEdgeD(Object,Nvert,0);
   MERIT PA1 = PEdgeA(Object,Nvert,1);
   MERIT PB1 = PEdgeB(Object,Nvert,1);
   MERIT PC1 = PEdgeC(Object,Nvert,1);
   MERIT PD1 = PEdgeD(Object,Nvert,1);
   MERIT PA2 = PEdgeA(Object,Nvert,2);
   MERIT PB2 = PEdgeB(Object,Nvert,2);
   MERIT PC2 = PEdgeC(Object,Nvert,2);
   AdvancedRelax(&TP[Tbaamerit],LC->tcSp,PB0 * PA1 * PA2);
   AdvancedRelax(&TP[Tbabmerit],LC->tcSp,PB0 * PA1 * PB2);
   AdvancedRelax(&TP[Tbacmerit],LC->tcSp,PB0 * PA1 * PC2);
   AdvancedRelax(&TP[Tbadmerit],LC->tcSp,PB0 * PA1 * PEdgeD(Object,Nvert,2));
   AdvancedRelax(&TP[Tbdamerit],LC->tcSp,PB0 * PD1 * PA2);
   AdvancedRelax(&TP[Tbdcmerit],LC->tcSp,PB0 * PD1 * PC2);
   AdvancedRelax(&TP[Tdabmerit],LC->tcSp,PD0 * PA1 * PB2);
   AdvancedRelax(&TP[Tdacmerit],LC->tcSp,PD0 * PA1 * PC2);
   AdvancedRelax(&TP[Tabamerit],LC->tcSp,PA0 * PB1 * PA2);
   AdvancedRelax(&TP[Tabbmerit],LC->tcSp,PA0 * PB1 * PB2);
   AdvancedRelax(&TP[Tbcamerit],LC->tcSp,PB0 * PC1 * PA2);
   AdvancedRelax(&TP[Tbccmerit],LC->tcSp,PB0 * PC1 * PC2);
   AdvancedRelax(&TP[Tcabmerit],LC->tcSp,PC0 * PA1 * PB2);
   AdvancedRelax(&TP[Tcacmerit],LC->tcSp,PC0 * PA1 * PC2);
   AdvancedRelax(&TP[Tccamerit],LC->tcSp,PC0 * PC1 * PA2);
   AdvancedRelax(&TP[Tccbmerit],LC->tcSp,PC0 * PC1 * PB2);
   AdvancedRelax(&TP[Tcdamerit],LC->tcSp,PC0 * PD1 * PA2);
   AdvancedRelax(&TP[Tdcbmerit],LC->tcSp,PD0 * PC1 * PB2);
   AdvancedRelax(&TP[Tddamerit],LC->tcSp,PD0 * PD1 * PA2);
   AdvancedRelax(&TP[Tddbmerit],LC->tcSp,PD0 * PD1 * PB2);

   MERIT Konly = LP->KJunctionPresenceMerit + (1.0-LP->KJunctionPresenceMerit)*(1.0-LC->tcSm);

   TP[Tabamerit] *= Konly;
   TP[Tabbmerit] *= Konly;
   TP[Tbcamerit] *= Konly;
   TP[Tbccmerit] *= Konly;
   TP[Tcabmerit] *= Konly;
   TP[Tcacmerit] *= Konly;
   TP[Tccamerit] *= Konly;
   TP[Tccbmerit] *= Konly;
   TP[Tcdamerit] *= Konly;
   TP[Tdcbmerit] *= Konly;
   TP[Tddamerit] *= Konly;
   TP[Tddbmerit] *= Konly;

   NormaliseMerits(TP,TMERITS);

   if (TP[Tbaamerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tbaa,LineIsBackArrow,LineIsArrow,LineIsArrow);
      ClearT(TP);
      TP[Tbaamerit] = 1.0;
      SetTJunctionOccluding(Nvert);
   }
   else
   if (TP[Tbabmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tbab,LineIsBackArrow,LineIsArrow,LineIsBackArrow);
      ClearT(TP);
      TP[Tbabmerit] = 1.0;
      SetTJunctionOccluding(Nvert);
   }
   else
   if (TP[Tbacmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tbac,LineIsBackArrow,LineIsArrow,LineIsConvex);
      ClearT(TP);
      TP[Tbacmerit] = 1.0;
      SetTJunctionOccluding(Nvert);
   }
   else
   if (TP[Tbadmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tbad,LineIsBackArrow,LineIsArrow,LineIsConcave);
      ClearT(TP);
      TP[Tbadmerit] = 1.0;
      SetTJunctionOccluding(Nvert);
   }
   else
   if (TP[Tbdamerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tbda,LineIsBackArrow,LineIsConcave,LineIsArrow);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tbdamerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tbdcmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tbdc,LineIsBackArrow,LineIsConcave,LineIsConvex);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tbdcmerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tdabmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tdab,LineIsConcave,LineIsArrow,LineIsBackArrow);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tdabmerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tdacmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tdac,LineIsConcave,LineIsArrow,LineIsConvex);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tdacmerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tabamerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Taba,LineIsArrow,LineIsBackArrow,LineIsArrow);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tabamerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tabbmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tabb,LineIsArrow,LineIsBackArrow,LineIsBackArrow);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tabbmerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tbcamerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tbca,LineIsBackArrow,LineIsConvex,LineIsArrow);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tbcamerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tbccmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tbcc,LineIsBackArrow,LineIsConvex,LineIsConvex);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tbccmerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tcabmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tcab,LineIsConvex,LineIsArrow,LineIsBackArrow);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tcabmerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tcacmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tcac,LineIsConvex,LineIsArrow,LineIsConvex);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tcacmerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tccamerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tcca,LineIsConvex,LineIsConvex,LineIsArrow);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tccamerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tccbmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tccb,LineIsConvex,LineIsConvex,LineIsBackArrow);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tccbmerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tcdamerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tcda,LineIsConvex,LineIsConcave,LineIsArrow);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tcdamerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tdcbmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tdcb,LineIsConcave,LineIsConvex,LineIsBackArrow);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
	  ClearT(TP);
	  TP[Tdcbmerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tddamerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tdda,LineIsConcave,LineIsConcave,LineIsArrow);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tddamerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   else
   if (TP[Tddbmerit] > 0.9999)
   {
	  FinaliseTLabel(&Object->Labelling,Nvert,Tddb,LineIsConcave,LineIsConcave,LineIsBackArrow);
	  Object->Vertices[Nvert].IsHidden = cFALSE;
      ClearT(TP);
      TP[Tddbmerit] = 1.0;
      SetTJunctionNonOccluding(Nvert);
   }
   // else PrintTProbabilities(TP);
}


void CCueLabelsVarley_Relaxation::UpdateProbabilitiesForLJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   MERIT PA0 = PEdgeA(Object,Nvert,0);
   MERIT PB0 = PEdgeB(Object,Nvert,0);
   MERIT PC0 = PEdgeC(Object,Nvert,0);
   MERIT PD0 = PEdgeD(Object,Nvert,0);
   MERIT PA1 = PEdgeA(Object,Nvert,1);
   MERIT PB1 = PEdgeB(Object,Nvert,1);
   MERIT PC1 = PEdgeC(Object,Nvert,1);
   MERIT PD1 = PEdgeD(Object,Nvert,1);

   //MERIT *LP = Object->Vprobabilities[Nvert];
   AdvancedRelax(&Object->Vprobabilities[Nvert][Lbamerit],LC->tcSp,PB0 * PA1);
   AdvancedRelax(&Object->Vprobabilities[Nvert][Lacmerit],LC->tcSp,PA0 * PC1);
   AdvancedRelax(&Object->Vprobabilities[Nvert][Labmerit],LC->tcSp,PA0 * PB1);
   AdvancedRelax(&Object->Vprobabilities[Nvert][Lcbmerit],LC->tcSp,PC0 * PB1);
   AdvancedRelax(&Object->Vprobabilities[Nvert][Lbdmerit],LC->tcSp,PB0 * PD1);
   AdvancedRelax(&Object->Vprobabilities[Nvert][Ldamerit],LC->tcSp,PD0 * PA1);
   AdvancedRelax(&Object->Vprobabilities[Nvert][Laamerit],LC->tcSp,PA0 * PA1);
   AdvancedRelax(&Object->Vprobabilities[Nvert][Lbbmerit],LC->tcSp,PB0 * PB1);

   MERIT XorK  = dmax2(LP->KJunctionPresenceMerit,LP->XJunctionPresenceMerit) + (1.0-dmax2(LP->KJunctionPresenceMerit,LP->XJunctionPresenceMerit))*(1.0-LC->tcSm);

   Object->Vprobabilities[Nvert][Laamerit] *= XorK;
   Object->Vprobabilities[Nvert][Lbbmerit] *= XorK;

   NormaliseMerits(Object->Vprobabilities[Nvert],LMERITS);

   if (Object->Vprobabilities[Nvert][Lbamerit] > 0.9999)
   {
	  FinaliseLLabel(&Object->Labelling,Nvert,Lba,LineIsBackArrow,LineIsArrow);
      ClearMerits(Object->Vprobabilities[Nvert],LMERITS,Lbamerit);
   }
   else
   if (Object->Vprobabilities[Nvert][Lacmerit] > 0.9999)
   {
	  FinaliseLLabel(&Object->Labelling,Nvert,Lac,LineIsArrow,LineIsConvex);
      ClearMerits(Object->Vprobabilities[Nvert],LMERITS,Lacmerit);
   }
   else
   if (Object->Vprobabilities[Nvert][Labmerit] > 0.9999)
   {
	  FinaliseLLabel(&Object->Labelling,Nvert,Lab,LineIsArrow,LineIsBackArrow);
      ClearMerits(Object->Vprobabilities[Nvert],LMERITS,Labmerit);
   }
   else
   if (Object->Vprobabilities[Nvert][Lcbmerit] > 0.9999)
   {
	  FinaliseLLabel(&Object->Labelling,Nvert,Lcb,LineIsConvex,LineIsBackArrow);
      ClearMerits(Object->Vprobabilities[Nvert],LMERITS,Lcbmerit);
   }
   else
   if (Object->Vprobabilities[Nvert][Lbdmerit] > 0.9999)
   {
	  FinaliseLLabel(&Object->Labelling,Nvert,Lbd,LineIsBackArrow,LineIsConcave);
      ClearMerits(Object->Vprobabilities[Nvert],LMERITS,Lbdmerit);
   }
   else
   if (Object->Vprobabilities[Nvert][Ldamerit] > 0.9999)
   {
	  FinaliseLLabel(&Object->Labelling,Nvert,Lda,LineIsConcave,LineIsArrow);
      ClearMerits(Object->Vprobabilities[Nvert],LMERITS,Ldamerit);
   }
   else
   if (Object->Vprobabilities[Nvert][Laamerit] > 0.9999)
   {
	  FinaliseLLabel(&Object->Labelling,Nvert,Laa,LineIsArrow,LineIsArrow);
      ClearMerits(Object->Vprobabilities[Nvert],LMERITS,Laamerit);
   }
   else
   if (Object->Vprobabilities[Nvert][Lbbmerit] > 0.9999)
   {
	  FinaliseLLabel(&Object->Labelling,Nvert,Lbb,LineIsBackArrow,LineIsBackArrow);
      ClearMerits(Object->Vprobabilities[Nvert],LMERITS,Lbbmerit);
   }
   else
   {
      //PrintLProbabilities(Object,Nvert);
   }
}


void CCueLabelsVarley_Relaxation::UpdateProbabilitiesForKJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   MERIT *KP = Object->Vprobabilities[Nvert];
   MERIT PA0 = PEdgeA(Object,Nvert,0);
   MERIT PC0 = PEdgeC(Object,Nvert,0);
   MERIT PD0 = PEdgeD(Object,Nvert,0);
   MERIT PB1 = PEdgeB(Object,Nvert,1);
   MERIT PC1 = PEdgeC(Object,Nvert,1);
   MERIT PD1 = PEdgeD(Object,Nvert,1);
   MERIT PC2 = PEdgeC(Object,Nvert,2);
   MERIT PD2 = PEdgeD(Object,Nvert,2);
   MERIT PC3 = PEdgeC(Object,Nvert,3);
   MERIT PD3 = PEdgeD(Object,Nvert,3);
   AdvancedRelax(&KP[Kabcdmerit],LC->tcSp,PA0 * PB1 * PC2 * PD3);
   AdvancedRelax(&KP[Kabdcmerit],LC->tcSp,PA0 * PB1 * PD2 * PC3);
   AdvancedRelax(&KP[Kcccdmerit],LC->tcSp,PC0 * PC1 * PC2 * PD3);
   AdvancedRelax(&KP[Kccdcmerit],LC->tcSp,PC0 * PC1 * PD2 * PC3);
   AdvancedRelax(&KP[Kcdcdmerit],LC->tcSp,PC0 * PD1 * PC2 * PD3);
   AdvancedRelax(&KP[Kdcdcmerit],LC->tcSp,PD0 * PC1 * PD2 * PC3);
   AdvancedRelax(&KP[Kddcdmerit],LC->tcSp,PD0 * PD1 * PC2 * PD3);
   AdvancedRelax(&KP[Kdddcmerit],LC->tcSp,PD0 * PD1 * PD2 * PC3);

   NormaliseMerits(KP,KMERITS);

   if (KP[Kabcdmerit] > 0.9999)
   {
      FinaliseKLabel(&Object->Labelling,Nvert,Kabcd,LineIsArrow,LineIsBackArrow,LineIsConvex,LineIsConcave);
      ClearMerits(KP,KMERITS,Kabcdmerit);
   }
   else
   if (KP[Kabdcmerit] > 0.9999)
   {
      FinaliseKLabel(&Object->Labelling,Nvert,Kabdc,LineIsArrow,LineIsBackArrow,LineIsConcave,LineIsConvex);
      ClearMerits(KP,KMERITS,Kabdcmerit);
   }
   else
   if (KP[Kcccdmerit] > 0.9999)
   {
      FinaliseKLabel(&Object->Labelling,Nvert,Kcccd,LineIsConvex,LineIsConvex,LineIsConvex,LineIsConcave);
      ClearMerits(KP,KMERITS,Kcccdmerit);
   }
   else
   if (KP[Kccdcmerit] > 0.9999)
   {
      FinaliseKLabel(&Object->Labelling,Nvert,Kccdc,LineIsConvex,LineIsConvex,LineIsConcave,LineIsConvex);
      ClearMerits(KP,KMERITS,Kccdcmerit);
   }
   else
   if (KP[Kcdcdmerit] > 0.9999)
   {
      FinaliseKLabel(&Object->Labelling,Nvert,Kcdcd,LineIsConvex,LineIsConcave,LineIsConvex,LineIsConcave);
      ClearMerits(KP,KMERITS,Kcdcdmerit);
   }
   else
   if (KP[Kdcdcmerit] > 0.9999)
   {
      FinaliseKLabel(&Object->Labelling,Nvert,Kdcdc,LineIsConcave,LineIsConvex,LineIsConcave,LineIsConvex);
      ClearMerits(KP,KMERITS,Kdcdcmerit);
   }
   else
   if (KP[Kddcdmerit] > 0.9999)
   {
      FinaliseKLabel(&Object->Labelling,Nvert,Kddcd,LineIsConcave,LineIsConcave,LineIsConvex,LineIsConcave);
      ClearMerits(KP,KMERITS,Kddcdmerit);
   }
   else
   if (KP[Kdddcmerit] > 0.9999)
   {
      FinaliseKLabel(&Object->Labelling,Nvert,Kdddc,LineIsConcave,LineIsConcave,LineIsConcave,LineIsConvex);
      ClearMerits(KP,KMERITS,Kdddcmerit);
   }
   else
   {
   //   printf("K%d abcd%f abdc%f cccd%f ccdc%f cdcd%f dcdc%f ddcd%f dddc%f\n",
   //           Nvert,
   //           KP->Kabcdmerit, KP->Kabdcmerit,
   //           KP->Kcccdmerit, KP->Kccdcmerit, KP->Kcdcdmerit, KP->Kdcdcmerit, KP->Kddcdmerit, KP->Kdddcmerit);
   }
}




void CCueLabelsVarley_Relaxation::NormaliseWProbabilities (void *P)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)P;
   double D = WP->Wbcamerit + WP->Wcdcmerit + WP->Wdcdmerit
            + WP->Wabcmerit + WP->Wabdmerit + WP->Waccmerit + WP->Wacdmerit + WP->Wadcmerit
            + WP->Wbaamerit + WP->Wbbamerit + WP->Wbcdmerit + WP->Wbdamerit + WP->Wbdcmerit + WP->Wbddmerit
            + WP->Wcabmerit + WP->Wcacmerit + WP->Wcbcmerit + WP->Wcbdmerit + WP->Wccbmerit + WP->Wcdamerit + WP->Wcdbmerit
            + WP->Wdabmerit + WP->Wdacmerit + WP->Wdadmerit + WP->Wdbdmerit + WP->Wdcamerit + WP->Wdcbmerit + WP->Wddamerit;

   if (D <= 0.0)  return;

   WP->Wbcamerit /= D;
   WP->Wcdcmerit /= D;
   WP->Wdcdmerit /= D;

   WP->Wabcmerit /= D;
   WP->Wabdmerit /= D;
   WP->Waccmerit /= D;
   WP->Wacdmerit /= D;
   WP->Wadcmerit /= D;
   WP->Wbaamerit /= D;
   WP->Wbbamerit /= D;
   WP->Wbcdmerit /= D;
   WP->Wbdamerit /= D;
   WP->Wbdcmerit /= D;
   WP->Wbddmerit /= D;
   WP->Wcabmerit /= D;
   WP->Wcacmerit /= D;
   WP->Wcbcmerit /= D;
   WP->Wcbdmerit /= D;
   WP->Wccbmerit /= D;
   WP->Wcdamerit /= D;
   WP->Wcdbmerit /= D;
   WP->Wdabmerit /= D;
   WP->Wdacmerit /= D;
   WP->Wdadmerit /= D;
   WP->Wdbdmerit /= D;
   WP->Wdcamerit /= D;
   WP->Wdcbmerit /= D;
   WP->Wddamerit /= D;
}


void CCueLabelsVarley_Relaxation::ClearW (WPROBABILITIES *WP)
{
   WP->Wbcamerit = 0.0;
   WP->Wcdcmerit = 0.0;
   WP->Wdcdmerit = 0.0;

   WP->Wabcmerit = 0.0;
   WP->Wabdmerit = 0.0;
   WP->Waccmerit = 0.0;
   WP->Wacdmerit = 0.0;
   WP->Wadcmerit = 0.0;
   WP->Wbaamerit = 0.0;
   WP->Wbbamerit = 0.0;
   WP->Wbcdmerit = 0.0;
   WP->Wbdamerit = 0.0;
   WP->Wbdcmerit = 0.0;
   WP->Wbddmerit = 0.0;
   WP->Wcabmerit = 0.0;
   WP->Wcacmerit = 0.0;
   WP->Wcbcmerit = 0.0;
   WP->Wcbdmerit = 0.0;
   WP->Wccbmerit = 0.0;
   WP->Wcdamerit = 0.0;
   WP->Wcdbmerit = 0.0;
   WP->Wdabmerit = 0.0;
   WP->Wdacmerit = 0.0;
   WP->Wdadmerit = 0.0;
   WP->Wdbdmerit = 0.0;
   WP->Wdcamerit = 0.0;
   WP->Wdcbmerit = 0.0;
   WP->Wddamerit = 0.0;
}


void CCueLabelsVarley_Relaxation::LabelVertexAsWbca (OBJECT *Object, VERTEX_INDEX Nvert)
{
   if (!Object->Labelling.VertexLabellingIsFinalised[Nvert])
   {
      WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
      FinaliseWLabel(&Object->Labelling,Nvert,Wbca,LineIsBackArrow,LineIsConvex,LineIsArrow);
      ClearW(WP);
      WP->Wbcamerit = 1.0;
   }
}


void CCueLabelsVarley_Relaxation::LabelVertexAsWcdc (OBJECT *Object, VERTEX_INDEX Nvert)
{
   if (!Object->Labelling.VertexLabellingIsFinalised[Nvert])
   {
      WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
      FinaliseWLabel(&Object->Labelling,Nvert,Wcdc,LineIsConvex,LineIsConcave,LineIsConvex);
      ClearW(WP);
      WP->Wcdcmerit = 1.0;
   }
}


void CCueLabelsVarley_Relaxation::LabelVertexAsWdcd (OBJECT *Object, VERTEX_INDEX Nvert)
{
   if (!Object->Labelling.VertexLabellingIsFinalised[Nvert])
   {
      WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
      FinaliseWLabel(&Object->Labelling,Nvert,Wdcd,LineIsConcave,LineIsConvex,LineIsConcave);
      ClearW(WP);
      WP->Wdcdmerit = 1.0;
   }
}


void CCueLabelsVarley_Relaxation::UpdateProbabilitiesForWJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   MERIT PA0 = PEdgeA(Object,Nvert,0);
   MERIT PB0 = PEdgeB(Object,Nvert,0);
   MERIT PC0 = PEdgeC(Object,Nvert,0);
   MERIT PD0 = PEdgeD(Object,Nvert,0);
   MERIT PA1 = PEdgeA(Object,Nvert,1);
   MERIT PB1 = PEdgeB(Object,Nvert,1);
   MERIT PC1 = PEdgeC(Object,Nvert,1);
   MERIT PD1 = PEdgeD(Object,Nvert,1);
   MERIT PA2 = PEdgeA(Object,Nvert,2);
   MERIT PB2 = PEdgeB(Object,Nvert,2);
   MERIT PC2 = PEdgeC(Object,Nvert,2);
   MERIT PD2 = PEdgeD(Object,Nvert,2);
   AdvancedRelax(&WP->Wbcamerit,LC->tcSp,PB0 * PC1 * PA2);
   AdvancedRelax(&WP->Wcdcmerit,LC->tcSp,PC0 * PD1 * PC2);
   AdvancedRelax(&WP->Wdcdmerit,LC->tcSp,PD0 * PC1 * PD2);
   AdvancedRelax(&WP->Wabcmerit,LC->tcSp,PA0 * PB1 * PC2);
   AdvancedRelax(&WP->Wabdmerit,LC->tcSp,PA0 * PB1 * PD2);
   AdvancedRelax(&WP->Waccmerit,LC->tcSp,PA0 * PC1 * PC2);
   AdvancedRelax(&WP->Wacdmerit,LC->tcSp,PA0 * PC1 * PD2);
   AdvancedRelax(&WP->Wadcmerit,LC->tcSp,PA0 * PD1 * PC2);
   AdvancedRelax(&WP->Wbaamerit,LC->tcSp,PB0 * PA1 * PA2);
   AdvancedRelax(&WP->Wbbamerit,LC->tcSp,PB0 * PB1 * PA2);
   AdvancedRelax(&WP->Wbcdmerit,LC->tcSp,PB0 * PC1 * PD2);
   AdvancedRelax(&WP->Wbdamerit,LC->tcSp,PB0 * PD1 * PA2);
   AdvancedRelax(&WP->Wbdcmerit,LC->tcSp,PB0 * PD1 * PC2);
   AdvancedRelax(&WP->Wbddmerit,LC->tcSp,PB0 * PD1 * PD2);
   AdvancedRelax(&WP->Wcabmerit,LC->tcSp,PC0 * PA1 * PB2);
   AdvancedRelax(&WP->Wcacmerit,LC->tcSp,PC0 * PA1 * PC2);
   AdvancedRelax(&WP->Wcbcmerit,LC->tcSp,PC0 * PB1 * PC2);
   AdvancedRelax(&WP->Wcbdmerit,LC->tcSp,PC0 * PB1 * PD2);
   AdvancedRelax(&WP->Wccbmerit,LC->tcSp,PC0 * PC1 * PB2);
   AdvancedRelax(&WP->Wcdamerit,LC->tcSp,PC0 * PD1 * PA2);
   AdvancedRelax(&WP->Wcdbmerit,LC->tcSp,PC0 * PD1 * PB2);
   AdvancedRelax(&WP->Wdabmerit,LC->tcSp,PD0 * PA1 * PB2);
   AdvancedRelax(&WP->Wdacmerit,LC->tcSp,PD0 * PA1 * PC2);
   AdvancedRelax(&WP->Wdadmerit,LC->tcSp,PD0 * PA1 * PD2);
   AdvancedRelax(&WP->Wdbdmerit,LC->tcSp,PD0 * PB1 * PD2);
   AdvancedRelax(&WP->Wdcamerit,LC->tcSp,PD0 * PC1 * PA2);
   AdvancedRelax(&WP->Wdcbmerit,LC->tcSp,PD0 * PC1 * PB2);
   AdvancedRelax(&WP->Wddamerit,LC->tcSp,PD0 * PD1 * PA2);

   MERIT Xonly = LP->XJunctionPresenceMerit + (1.0-LP->XJunctionPresenceMerit)*(1.0-LC->tcSm);
   MERIT XorK  = dmax2(LP->KJunctionPresenceMerit,LP->XJunctionPresenceMerit) + (1.0-dmax2(LP->KJunctionPresenceMerit,LP->XJunctionPresenceMerit))*(1.0-LC->tcSm);

   WP->Wabcmerit *= Xonly;
   WP->Wabdmerit *= XorK;
   WP->Waccmerit *= Xonly;
   WP->Wacdmerit *= XorK;
   WP->Wadcmerit *= Xonly;
   WP->Wbaamerit *= Xonly;
   WP->Wbbamerit *= Xonly;
   WP->Wbcdmerit *= Xonly;
   WP->Wbdamerit *= Xonly;
   WP->Wbdcmerit *= Xonly;
   WP->Wbddmerit *= Xonly;
   WP->Wcabmerit *= Xonly;
   WP->Wcacmerit *= Xonly;
   WP->Wcbcmerit *= Xonly;
   WP->Wcbdmerit *= Xonly;
   WP->Wccbmerit *= Xonly;
   WP->Wcdamerit *= Xonly;
   WP->Wcdbmerit *= Xonly;
   WP->Wdabmerit *= XorK;
   WP->Wdacmerit *= Xonly;
   WP->Wdadmerit *= Xonly;
   WP->Wdbdmerit *= Xonly;
   WP->Wdcamerit *= Xonly;
   WP->Wdcbmerit *= XorK;
   WP->Wddamerit *= Xonly;

   NormaliseWProbabilities(WP);

   if (WP->Wbcamerit > 0.9999)  LabelVertexAsWbca(Object,Nvert);
   else
   if (WP->Wcdcmerit > 0.9999)  LabelVertexAsWcdc(Object,Nvert);
   else
   if (WP->Wdcdmerit > 0.9999)  LabelVertexAsWdcd(Object,Nvert);
   else
   if (WP->Wabcmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wabc,LineIsArrow,LineIsBackArrow,LineIsConvex);
      ClearW(WP);
      WP->Wabcmerit = 1.0;
   }
   else
   if (WP->Wabdmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wabd,LineIsArrow,LineIsBackArrow,LineIsConcave);
      ClearW(WP);
      WP->Wabdmerit = 1.0;
   }
   else
   if (WP->Waccmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wacc,LineIsArrow,LineIsConvex,LineIsConvex);
      ClearW(WP);
      WP->Waccmerit = 1.0;
   }
   else
   if (WP->Wacdmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wacd,LineIsArrow,LineIsConvex,LineIsConcave);
      ClearW(WP);
      WP->Wacdmerit = 1.0;
   }
   else
   if (WP->Wadcmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wadc,LineIsArrow,LineIsConcave,LineIsConvex);
      ClearW(WP);
      WP->Wadcmerit = 1.0;
   }
   else
   if (WP->Wbaamerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wbaa,LineIsBackArrow,LineIsArrow,LineIsArrow);
      ClearW(WP);
      WP->Wbaamerit = 1.0;
   }
   else
   if (WP->Wbbamerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wbba,LineIsBackArrow,LineIsBackArrow,LineIsArrow);
      ClearW(WP);
      WP->Wbbamerit = 1.0;
   }
   else
   if (WP->Wbcdmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wbcd,LineIsBackArrow,LineIsConvex,LineIsConcave);
      ClearW(WP);
      WP->Wbcdmerit = 1.0;
   }
   else
   if (WP->Wbdamerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wbda,LineIsBackArrow,LineIsConcave,LineIsArrow);
      ClearW(WP);
      WP->Wbdamerit = 1.0;
   }
   else
   if (WP->Wbdcmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wbdc,LineIsBackArrow,LineIsConcave,LineIsConvex);
      ClearW(WP);
      WP->Wbdcmerit = 1.0;
   }
   else
   if (WP->Wbddmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wbdd,LineIsBackArrow,LineIsConcave,LineIsConcave);
      ClearW(WP);
      WP->Wbddmerit = 1.0;
   }
   else
   if (WP->Wcabmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wcab,LineIsConvex,LineIsArrow,LineIsBackArrow);
      ClearW(WP);
      WP->Wcabmerit = 1.0;
   }
   else
   if (WP->Wcacmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wcac,LineIsConvex,LineIsArrow,LineIsConvex);
      ClearW(WP);
      WP->Wcacmerit = 1.0;
   }
   else
   if (WP->Wcbcmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wcbc,LineIsConvex,LineIsBackArrow,LineIsConvex);
      ClearW(WP);
      WP->Wcbcmerit = 1.0;
   }
   else
   if (WP->Wcbdmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wcbd,LineIsConvex,LineIsBackArrow,LineIsConcave);
      ClearW(WP);
      WP->Wcbdmerit = 1.0;
   }
   else
   if (WP->Wccbmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wccb,LineIsConvex,LineIsConvex,LineIsBackArrow);
      ClearW(WP);
      WP->Wccbmerit = 1.0;
   }
   else
   if (WP->Wcdamerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wcda,LineIsConvex,LineIsConcave,LineIsArrow);
      ClearW(WP);
      WP->Wcdamerit = 1.0;
   }
   else
   if (WP->Wcdbmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wcdb,LineIsConvex,LineIsConcave,LineIsBackArrow);
      ClearW(WP);
      WP->Wcdbmerit = 1.0;
   }
   else
   if (WP->Wdabmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wdab,LineIsConcave,LineIsArrow,LineIsBackArrow);
      ClearW(WP);
      WP->Wdabmerit = 1.0;
   }
   else
   if (WP->Wdacmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wdac,LineIsConcave,LineIsArrow,LineIsConvex);
      ClearW(WP);
      WP->Wdacmerit = 1.0;
   }
   else
   if (WP->Wdadmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wdad,LineIsConcave,LineIsArrow,LineIsConcave);
      ClearW(WP);
      WP->Wdadmerit = 1.0;
   }
   else
   if (WP->Wdbdmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wdbd,LineIsConcave,LineIsBackArrow,LineIsConcave);
      ClearW(WP);
      WP->Wdbdmerit = 1.0;
   }
   else
   if (WP->Wdcamerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wdca,LineIsConcave,LineIsConvex,LineIsArrow);
      ClearW(WP);
      WP->Wdcamerit = 1.0;
   }
   else
   if (WP->Wdcbmerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wdcb,LineIsConcave,LineIsConvex,LineIsBackArrow);
      ClearW(WP);
      WP->Wdcbmerit = 1.0;
   }
   else
   if (WP->Wddamerit > 0.9999)
   {
	  FinaliseWLabel(&Object->Labelling,Nvert,Wdda,LineIsConcave,LineIsConcave,LineIsArrow);
      ClearW(WP);
      WP->Wddamerit = 1.0;
   }
   else
   {
      //PrintWProbabilities(RelaxationDebugFile,Labelling,Nvert);
   }
}








void CCueLabelsVarley_Relaxation::NormaliseXProbabilities (void *P)
{
   XPROBABILITIES *XP = (XPROBABILITIES *)P;
   double D = XP->Xabcdmerit + XP->Xbcdamerit + XP->Xcdabmerit + XP->Xdabcmerit
            + XP->Xabdcmerit + XP->Xbdcamerit + XP->Xdcabmerit + XP->Xcabdmerit
            + XP->Xabddmerit + XP->Xbddamerit + XP->Xddabmerit + XP->Xdabdmerit
            + XP->Xccccmerit
            + XP->Xcccdmerit + XP->Xccdcmerit + XP->Xcdccmerit + XP->Xdcccmerit
            + XP->Xcdcdmerit + XP->Xdcdcmerit
            + XP->Xcdddmerit + XP->Xdcddmerit + XP->Xddcdmerit + XP->Xdddcmerit
            + XP->Xddddmerit;

   if (D <= 0.0)  return;

   XP->Xabcdmerit /= D;
   XP->Xbcdamerit /= D;
   XP->Xcdabmerit /= D;
   XP->Xdabcmerit /= D;
   XP->Xabdcmerit /= D;
   XP->Xbdcamerit /= D;
   XP->Xdcabmerit /= D;
   XP->Xcabdmerit /= D;
   XP->Xabddmerit /= D;
   XP->Xbddamerit /= D;
   XP->Xddabmerit /= D;
   XP->Xdabdmerit /= D;
   XP->Xccccmerit /= D;
   XP->Xcccdmerit /= D;
   XP->Xccdcmerit /= D;
   XP->Xcdccmerit /= D;
   XP->Xdcccmerit /= D;
   XP->Xcdcdmerit /= D;
   XP->Xdcdcmerit /= D;
   XP->Xcdddmerit /= D;
   XP->Xdcddmerit /= D;
   XP->Xddcdmerit /= D;
   XP->Xdddcmerit /= D;
   XP->Xddddmerit /= D;
}


void CCueLabelsVarley_Relaxation::ClearX (XPROBABILITIES *XP)
{
   XP->Xabcdmerit = 0.0;
   XP->Xbcdamerit = 0.0;
   XP->Xcdabmerit = 0.0;
   XP->Xdabcmerit = 0.0;
   XP->Xabdcmerit = 0.0;
   XP->Xbdcamerit = 0.0;
   XP->Xdcabmerit = 0.0;
   XP->Xcabdmerit = 0.0;
   XP->Xabddmerit = 0.0;
   XP->Xbddamerit = 0.0;
   XP->Xddabmerit = 0.0;
   XP->Xdabdmerit = 0.0;
   XP->Xccccmerit = 0.0;
   XP->Xcccdmerit = 0.0;
   XP->Xccdcmerit = 0.0;
   XP->Xcdccmerit = 0.0;
   XP->Xdcccmerit = 0.0;
   XP->Xcdcdmerit = 0.0;
   XP->Xdcdcmerit = 0.0;
   XP->Xcdddmerit = 0.0;
   XP->Xdcddmerit = 0.0;
   XP->Xddcdmerit = 0.0;
   XP->Xdddcmerit = 0.0;
   XP->Xddddmerit = 0.0;
}


void CCueLabelsVarley_Relaxation::UpdateProbabilitiesForXJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   XPROBABILITIES *XP = (XPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   EDGE_INDEX Np,Nq,Nr,Ns;

   MERIT PA0 = PEdgeA(Object,Nvert,0);
   MERIT PB0 = PEdgeB(Object,Nvert,0);
   MERIT PC0 = PEdgeC(Object,Nvert,0);
   MERIT PD0 = PEdgeD(Object,Nvert,0);
   MERIT PA1 = PEdgeA(Object,Nvert,1);
   MERIT PB1 = PEdgeB(Object,Nvert,1);
   MERIT PC1 = PEdgeC(Object,Nvert,1);
   MERIT PD1 = PEdgeD(Object,Nvert,1);
   MERIT PA2 = PEdgeA(Object,Nvert,2);
   MERIT PB2 = PEdgeB(Object,Nvert,2);
   MERIT PC2 = PEdgeC(Object,Nvert,2);
   MERIT PD2 = PEdgeD(Object,Nvert,2);
   MERIT PA3 = PEdgeA(Object,Nvert,3);
   MERIT PB3 = PEdgeB(Object,Nvert,3);
   MERIT PC3 = PEdgeC(Object,Nvert,3);
   MERIT PD3 = PEdgeD(Object,Nvert,3);

   CheckVertexNumber(&Object->Labelling,Nvert,"UpdateProbabilitiesForXJunction");

   AdvancedRelax(&XP->Xabcdmerit,LC->tcSp,PA0 * PB1 * PC2 * PD3);
   AdvancedRelax(&XP->Xbcdamerit,LC->tcSp,PB0 * PC1 * PD2 * PA3);
   AdvancedRelax(&XP->Xcdabmerit,LC->tcSp,PC0 * PD1 * PA2 * PB3);
   AdvancedRelax(&XP->Xdabcmerit,LC->tcSp,PD0 * PA1 * PB2 * PC3);
   AdvancedRelax(&XP->Xabdcmerit,LC->tcSp,PA0 * PB1 * PD2 * PC3);
   AdvancedRelax(&XP->Xbdcamerit,LC->tcSp,PB0 * PD1 * PC2 * PA3);
   AdvancedRelax(&XP->Xdcabmerit,LC->tcSp,PD0 * PC1 * PA2 * PB3);
   AdvancedRelax(&XP->Xcabdmerit,LC->tcSp,PC0 * PA1 * PB2 * PD3);
   AdvancedRelax(&XP->Xabddmerit,LC->tcSp,PA0 * PB1 * PD2 * PD3);
   AdvancedRelax(&XP->Xbddamerit,LC->tcSp,PB0 * PD1 * PD2 * PA3);
   AdvancedRelax(&XP->Xddabmerit,LC->tcSp,PD0 * PD1 * PA2 * PB3);
   AdvancedRelax(&XP->Xdabdmerit,LC->tcSp,PD0 * PA1 * PB2 * PD3);
   AdvancedRelax(&XP->Xccccmerit,LC->tcSp,PC0 * PC1 * PC2 * PC3);
   AdvancedRelax(&XP->Xcccdmerit,LC->tcSp,PC0 * PC1 * PC2 * PD3);
   AdvancedRelax(&XP->Xccdcmerit,LC->tcSp,PC0 * PC1 * PD2 * PC3);
   AdvancedRelax(&XP->Xcdccmerit,LC->tcSp,PC0 * PD1 * PC2 * PC3);
   AdvancedRelax(&XP->Xdcccmerit,LC->tcSp,PD0 * PC1 * PC2 * PC3);
   AdvancedRelax(&XP->Xcdcdmerit,LC->tcSp,PC0 * PD1 * PC2 * PD3);
   AdvancedRelax(&XP->Xdcdcmerit,LC->tcSp,PD0 * PC1 * PD2 * PC3);
   AdvancedRelax(&XP->Xcdddmerit,LC->tcSp,PC0 * PD1 * PD2 * PD3);
   AdvancedRelax(&XP->Xdcddmerit,LC->tcSp,PD0 * PC1 * PD2 * PD3);
   AdvancedRelax(&XP->Xddcdmerit,LC->tcSp,PD0 * PD1 * PC2 * PD3);
   AdvancedRelax(&XP->Xdddcmerit,LC->tcSp,PD0 * PD1 * PD2 * PC3);
   AdvancedRelax(&XP->Xddddmerit,LC->tcSp,PD0 * PD1 * PD2 * PD3);
   NormaliseXProbabilities(XP);

   if (XP->Xabcdmerit > 0.9999)
   {
	  FinaliseXLabel(&Object->Labelling,Nvert,Xabcd,LineIsArrow,LineIsBackArrow,LineIsConvex,LineIsConcave);
      ClearX(XP);
      XP->Xabcdmerit = 1.0;
   }
   else
   if (XP->Xbcdamerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Ns;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
      Object->Labelling.VENumber[Nvert][3] = Nr;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xabcd,LineIsArrow,LineIsBackArrow,LineIsConvex,LineIsConcave);
      ClearX(XP);
      XP->Xabcdmerit = 1.0;
   }
   else
   if (XP->Xcdabmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Ns;
      Object->Labelling.VENumber[Nvert][2] = Np;
      Object->Labelling.VENumber[Nvert][3] = Nq;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xabcd,LineIsArrow,LineIsBackArrow,LineIsConvex,LineIsConcave);
      ClearX(XP);
      XP->Xabcdmerit = 1.0;
   }
   else
   if (XP->Xdabcmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Ns;
      Object->Labelling.VENumber[Nvert][3] = Np;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xabcd,LineIsArrow,LineIsBackArrow,LineIsConvex,LineIsConcave);
      ClearX(XP);
      XP->Xabcdmerit = 1.0;
   }
   else
   if (XP->Xabdcmerit > 0.9999)
   {
	  FinaliseXLabel(&Object->Labelling,Nvert,Xabdc,LineIsArrow,LineIsBackArrow,LineIsConcave,LineIsConvex);
      ClearX(XP);
      XP->Xabdcmerit = 1.0;
   }
   else
   if (XP->Xbdcamerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Ns;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
      Object->Labelling.VENumber[Nvert][3] = Nr;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xabdc,LineIsArrow,LineIsBackArrow,LineIsConcave,LineIsConvex);
      ClearX(XP);
      XP->Xabdcmerit = 1.0;
   }
   else
   if (XP->Xdcabmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Ns;
      Object->Labelling.VENumber[Nvert][2] = Np;
      Object->Labelling.VENumber[Nvert][3] = Nq;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xabdc,LineIsArrow,LineIsBackArrow,LineIsConcave,LineIsConvex);
      ClearX(XP);
      XP->Xabdcmerit = 1.0;
   }
   else
   if (XP->Xcabdmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Ns;
      Object->Labelling.VENumber[Nvert][3] = Np;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xabdc,LineIsArrow,LineIsBackArrow,LineIsConcave,LineIsConvex);
      ClearX(XP);
      XP->Xabdcmerit = 1.0;
   }
   else
   if (XP->Xabddmerit > 0.9999)
   {
	  FinaliseXLabel(&Object->Labelling,Nvert,Xabdd,LineIsArrow,LineIsBackArrow,LineIsConcave,LineIsConcave);
      ClearX(XP);
      XP->Xabddmerit = 1.0;
   }
   else
   if (XP->Xbddamerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Ns;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
      Object->Labelling.VENumber[Nvert][3] = Nr;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xabdd,LineIsArrow,LineIsBackArrow,LineIsConcave,LineIsConcave);
      ClearX(XP);
      XP->Xabddmerit = 1.0;
   }
   else
   if (XP->Xddabmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Ns;
      Object->Labelling.VENumber[Nvert][2] = Np;
      Object->Labelling.VENumber[Nvert][3] = Nq;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xabdd,LineIsArrow,LineIsBackArrow,LineIsConcave,LineIsConcave);
      ClearX(XP);
      XP->Xabddmerit = 1.0;
   }
   else
   if (XP->Xdabdmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Ns;
      Object->Labelling.VENumber[Nvert][3] = Np;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xabdd,LineIsArrow,LineIsBackArrow,LineIsConcave,LineIsConcave);
      ClearX(XP);
      XP->Xabddmerit = 1.0;
   }
   else
   if (XP->Xccccmerit > 0.9999)
   {
	  FinaliseXLabel(&Object->Labelling,Nvert,Xcccc,LineIsConvex,LineIsConvex,LineIsConvex,LineIsConvex);
      ClearX(XP);
      XP->Xccccmerit = 1.0;
   }
   else
   if (XP->Xcccdmerit > 0.9999)
   {
	  FinaliseXLabel(&Object->Labelling,Nvert,Xcccd,LineIsConvex,LineIsConvex,LineIsConvex,LineIsConcave);
      ClearX(XP);
      XP->Xcccdmerit = 1.0;
   }
   else
   if (XP->Xccdcmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Ns;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
      Object->Labelling.VENumber[Nvert][3] = Nr;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xcccd,LineIsConvex,LineIsConvex,LineIsConvex,LineIsConcave);
      ClearX(XP);
      XP->Xcccdmerit = 1.0;
   }
   else
   if (XP->Xcdccmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Ns;
      Object->Labelling.VENumber[Nvert][2] = Np;
      Object->Labelling.VENumber[Nvert][3] = Nq;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xcccd,LineIsConvex,LineIsConvex,LineIsConvex,LineIsConcave);
      ClearX(XP);
      XP->Xcccdmerit = 1.0;
   }
   else
   if (XP->Xdcccmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Ns;
      Object->Labelling.VENumber[Nvert][3] = Np;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xcccd,LineIsConvex,LineIsConvex,LineIsConvex,LineIsConcave);
      ClearX(XP);
      XP->Xcccdmerit = 1.0;
   }
   else
   if (XP->Xcdcdmerit > 0.9999)
   {
	  FinaliseXLabel(&Object->Labelling,Nvert,Xcdcd,LineIsConvex,LineIsConcave,LineIsConvex,LineIsConcave);
      ClearX(XP);
      XP->Xcdcdmerit = 1.0;
   }
   else
   if (XP->Xdcdcmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Ns;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
      Object->Labelling.VENumber[Nvert][3] = Nr;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xcdcd,LineIsConvex,LineIsConcave,LineIsConvex,LineIsConcave);
      ClearX(XP);
      XP->Xcdcdmerit = 1.0;
   }
   else
   if (XP->Xcdddmerit > 0.9999)
   {
	  FinaliseXLabel(&Object->Labelling,Nvert,Xcddd,LineIsConvex,LineIsConcave,LineIsConcave,LineIsConcave);
      ClearX(XP);
      XP->Xcdddmerit = 1.0;
   }
   else
   if (XP->Xdcddmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Ns;
      Object->Labelling.VENumber[Nvert][3] = Np;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xcddd,LineIsConvex,LineIsConcave,LineIsConcave,LineIsConcave);
      ClearX(XP);
      XP->Xcdddmerit = 1.0;
   }
   else
   if (XP->Xddcdmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Ns;
      Object->Labelling.VENumber[Nvert][2] = Np;
      Object->Labelling.VENumber[Nvert][3] = Nq;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xcddd,LineIsConvex,LineIsConcave,LineIsConcave,LineIsConcave);
      ClearX(XP);
      XP->Xcdddmerit = 1.0;
   }
   else
   if (XP->Xdddcmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Ns = Object->Labelling.VENumber[Nvert][3];
      Object->Labelling.VENumber[Nvert][0] = Ns;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
      Object->Labelling.VENumber[Nvert][3] = Nr;
	  FinaliseXLabel(&Object->Labelling,Nvert,Xcddd,LineIsConvex,LineIsConcave,LineIsConcave,LineIsConcave);
      ClearX(XP);
      XP->Xcdddmerit = 1.0;
   }
   else
   if (XP->Xddddmerit > 0.9999)
   {
	  FinaliseXLabel(&Object->Labelling,Nvert,Xdddd,LineIsConcave,LineIsConcave,LineIsConcave,LineIsConcave);
      ClearX(XP);
      XP->Xddddmerit = 1.0;
   }
   else
   {
      //printf("X%d cbda%f abcd%f bcda%f cdab%f dabc%f abdc%f bdca%f dcab%f cabd%f abdd%f bdda%f ddab%f dabd%f\n",
      //        Nvert,
      //        XP->Xabcdmerit, XP->Xbcdamerit, XP->Xcdabmerit, XP->Xdabcmerit,
      //        XP->Xabdcmerit, XP->Xbdcamerit, XP->Xdcabmerit, XP->Xcabdmerit,
      //        XP->Xabddmerit, XP->Xbddamerit, XP->Xddabmerit, XP->Xdabdmerit);
      //printf(" cccc%f cccd%f ccdc%f cdcc%f dccc%f cdcd%f dcdc%f cddd%f dcdd%f ddcd%f dddc%f dddd%f\n",
      //        XP->Xccccmerit,
      //        XP->Xcccdmerit, XP->Xccdcmerit, XP->Xcdccmerit, XP->Xdcccmerit,
      //        XP->Xcdcdmerit, XP->Xdcdcmerit,
      //        XP->Xcdddmerit, XP->Xdcddmerit, XP->Xddcdmerit, XP->Xdddcmerit,
      //        XP->Xddddmerit);
   }
}


void CCueLabelsVarley_Relaxation::UpdateProbabilitiesForX5Junction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   static const TYPEMARK Label[X5MERITS] = {Xccccc,         Xddddd};
   static const TYPEMARK Final[X5MERITS] = {VFinalIs5ccccc, VFinalIs5ddddd};

   MERIT *XP = Object->Vprobabilities[Nvert];
   int n;

   AdvancedRelax(&XP[Xcccccmerit],LC->tcSp,PEdgeC(Object,Nvert,0) * PEdgeC(Object,Nvert,1) * PEdgeC(Object,Nvert,2) * PEdgeC(Object,Nvert,3) * PEdgeC(Object,Nvert,4));
   AdvancedRelax(&XP[Xdddddmerit],LC->tcSp,PEdgeD(Object,Nvert,0) * PEdgeD(Object,Nvert,1) * PEdgeD(Object,Nvert,2) * PEdgeD(Object,Nvert,3) * PEdgeD(Object,Nvert,4));
   NormaliseMerits(XP,X5MERITS);

   for (n=0; n<X5MERITS; ++n)  if (XP[n] > 0.9999)
   {
      Object->Labelling.VertexLabel[Nvert].Vexity = Label[n];
	  Object->Labelling.VertexUnderlyingType[Nvert] = Object->Labelling.FinalVertexTypes & Final[n];
	  Object->Labelling.VertexLabellingIsFinalised[Nvert] = cTRUE;
      ClearMerits(XP,X5MERITS,n);
      return;
   }
   //printf("X5.%d ccccc%f ddddd%f\n", Nvert, XP->Xcccccmerit, XP->Xdddddmerit);
}


void CCueLabelsVarley_Relaxation::UpdateProbabilitiesForX6Junction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   MERIT *XP = Object->Vprobabilities[Nvert];

   MERIT PC0 = PEdgeC(Object,Nvert,0);
   MERIT PD0 = PEdgeD(Object,Nvert,0);
   MERIT PC1 = PEdgeC(Object,Nvert,1);
   MERIT PD1 = PEdgeD(Object,Nvert,1);
   MERIT PC2 = PEdgeC(Object,Nvert,2);
   MERIT PD2 = PEdgeD(Object,Nvert,2);
   MERIT PC3 = PEdgeC(Object,Nvert,3);
   MERIT PD3 = PEdgeD(Object,Nvert,3);
   MERIT PC4 = PEdgeC(Object,Nvert,4);
   MERIT PD4 = PEdgeD(Object,Nvert,4);
   MERIT PC5 = PEdgeC(Object,Nvert,5);
   MERIT PD5 = PEdgeD(Object,Nvert,5);
   AdvancedRelax(&XP[Xccccccmerit],LC->tcSp,PC0 * PC1 * PC2 * PC3 * PC4 * PC5);
   AdvancedRelax(&XP[Xcdcdcdmerit],LC->tcSp,PC0 * PD1 * PC2 * PD3 * PC4 * PD5);
   AdvancedRelax(&XP[Xdcdcdcmerit],LC->tcSp,PD0 * PC1 * PD2 * PC3 * PD4 * PC5);
   AdvancedRelax(&XP[Xddddddmerit],LC->tcSp,PD0 * PD1 * PD2 * PD3 * PD4 * PD5);
   NormaliseMerits(XP,X6MERITS);

   if (XP[Xccccccmerit] > 0.9999)
   {
      Object->Labelling.VertexLabel[Nvert].Vexity = Xcccccc;
	  Object->Labelling.VertexUnderlyingType[Nvert] = Object->Labelling.FinalVertexTypes & VFinalIs6cccccc;
	  Object->Labelling.VertexLabellingIsFinalised[Nvert] = cTRUE;
      ClearMerits(XP,X6MERITS,Xccccccmerit);
   }
   else
   if (XP[Xcdcdcdmerit] > 0.9999)
   {
      Object->Labelling.VertexLabel[Nvert].Vexity = Xcdcdcd;
	  Object->Labelling.VertexUnderlyingType[Nvert] = Object->Labelling.FinalVertexTypes & VFinalIs6cdcdcd;
	  Object->Labelling.VertexLabellingIsFinalised[Nvert] = cTRUE;
      ClearMerits(XP,X6MERITS,Xcdcdcdmerit);
   }
   else
   if (XP[Xdcdcdcmerit] > 0.9999)
   {
      EDGE_INDEX Np = Object->Labelling.VENumber[Nvert][0];
      EDGE_INDEX Nq = Object->Labelling.VENumber[Nvert][1];
      EDGE_INDEX Nr = Object->Labelling.VENumber[Nvert][2];
      EDGE_INDEX Ns = Object->Labelling.VENumber[Nvert][3];
      EDGE_INDEX Nt = Object->Labelling.VENumber[Nvert][4];
      EDGE_INDEX Nu = Object->Labelling.VENumber[Nvert][5];
      Object->Labelling.VENumber[Nvert][0] = Nu;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
      Object->Labelling.VENumber[Nvert][3] = Nr;
      Object->Labelling.VENumber[Nvert][4] = Ns;
      Object->Labelling.VENumber[Nvert][5] = Nt;
      Object->Labelling.VertexLabel[Nvert].Vexity = Xcdcdcd;
	  Object->Labelling.VertexUnderlyingType[Nvert] = Object->Labelling.FinalVertexTypes & VFinalIs6cdcdcd;
	  Object->Labelling.VertexLabellingIsFinalised[Nvert] = cTRUE;
      ClearMerits(XP,X6MERITS,Xcdcdcdmerit);
   }
   else
   if (XP[Xddddddmerit] > 0.9999)
   {
      Object->Labelling.VertexLabel[Nvert].Vexity = Xdddddd;
	  Object->Labelling.VertexUnderlyingType[Nvert] = Object->Labelling.FinalVertexTypes & VFinalIs6dddddd;
	  Object->Labelling.VertexLabellingIsFinalised[Nvert] = cTRUE;
      ClearMerits(XP,X6MERITS,Xddddddmerit);
   }
   else
   {
      //printf("X6 %f %f %f %f\n", XP->Xccccccmerit, XP->Xcdcdcdmerit, XP->Xdcdcdcmerit, XP->Xddddddmerit);
   }
}


void CCueLabelsVarley_Relaxation::NormaliseYProbabilities (void *P)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)P;
   double D = YP->Ycccmerit + YP->Ydddmerit
            + YP->Yabdmerit + YP->Ybdamerit + YP->Ydabmerit
            + YP->Yaabmerit + YP->Yabamerit + YP->Ybaamerit
            + YP->Yabbmerit + YP->Ybbamerit + YP->Ybabmerit
            + YP->Yabcmerit + YP->Ybcamerit + YP->Ycabmerit
            + YP->Yaccmerit + YP->Yccamerit + YP->Ycacmerit
            + YP->Yacdmerit + YP->Ycdamerit + YP->Ydacmerit
            + YP->Yaddmerit + YP->Yddamerit + YP->Ydadmerit
            + YP->Ybccmerit + YP->Yccbmerit + YP->Ycbcmerit
            + YP->Ybdcmerit + YP->Ydcbmerit + YP->Ycbdmerit
            + YP->Ybddmerit + YP->Yddbmerit + YP->Ydbdmerit;

   if (D <= 0.0)  return;

   YP->Ycccmerit /= D;
   YP->Ydddmerit /= D;
   YP->Yabdmerit /= D;
   YP->Ybdamerit /= D;
   YP->Ydabmerit /= D;
   YP->Yaabmerit /= D;
   YP->Yabamerit /= D;
   YP->Ybaamerit /= D;
   YP->Yabbmerit /= D;
   YP->Ybbamerit /= D;
   YP->Ybabmerit /= D;
   YP->Yabcmerit /= D;
   YP->Ybcamerit /= D;
   YP->Ycabmerit /= D;
   YP->Yaccmerit /= D;
   YP->Yccamerit /= D;
   YP->Ycacmerit /= D;
   YP->Yacdmerit /= D;
   YP->Ycdamerit /= D;
   YP->Ydacmerit /= D;
   YP->Yaddmerit /= D;
   YP->Yddamerit /= D;
   YP->Ydadmerit /= D;
   YP->Ybccmerit /= D;
   YP->Yccbmerit /= D;
   YP->Ycbcmerit /= D;
   YP->Ybdcmerit /= D;
   YP->Ydcbmerit /= D;
   YP->Ycbdmerit /= D;
   YP->Ybddmerit /= D;
   YP->Yddbmerit /= D;
   YP->Ydbdmerit /= D;
}


void CCueLabelsVarley_Relaxation::ClearY (YPROBABILITIES *YP)
{
   YP->Ycccmerit = 0.0;
   YP->Ydddmerit = 0.0;
   YP->Yabdmerit = 0.0;
   YP->Ybdamerit = 0.0;
   YP->Ydabmerit = 0.0;

   YP->Yaabmerit = 0.0;
   YP->Yabamerit = 0.0;
   YP->Ybaamerit = 0.0;
   YP->Yabbmerit = 0.0;
   YP->Ybbamerit = 0.0;
   YP->Ybabmerit = 0.0;
   YP->Yabcmerit = 0.0;
   YP->Ybcamerit = 0.0;
   YP->Ycabmerit = 0.0;
   YP->Yaccmerit = 0.0;
   YP->Ycacmerit = 0.0;
   YP->Yccamerit = 0.0;
   YP->Yacdmerit = 0.0;
   YP->Ydacmerit = 0.0;
   YP->Ycdamerit = 0.0;
   YP->Yaddmerit = 0.0;
   YP->Ydadmerit = 0.0;
   YP->Yddamerit = 0.0;
   YP->Ybccmerit = 0.0;
   YP->Ycbcmerit = 0.0;
   YP->Yccbmerit = 0.0;
   YP->Ybdcmerit = 0.0;
   YP->Ydcbmerit = 0.0;
   YP->Ycbdmerit = 0.0;
   YP->Ybddmerit = 0.0;
   YP->Yddbmerit = 0.0;
   YP->Ydbdmerit = 0.0;
}


void CCueLabelsVarley_Relaxation::LabelVertexAsYccc (OBJECT *Object, VERTEX_INDEX Nvert)
{
   if (!Object->Labelling.VertexLabellingIsFinalised[Nvert])
   {
      YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
      FinaliseYLabel(&Object->Labelling,Nvert,Yccc,LineIsConvex,LineIsConvex,LineIsConvex);
      ClearY(YP);
      YP->Ycccmerit = 1.0;
   }
}


void CCueLabelsVarley_Relaxation::LabelVertexAsYddd (OBJECT *Object, VERTEX_INDEX Nvert)
{
   if (!Object->Labelling.VertexLabellingIsFinalised[Nvert])
   {
      YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
      FinaliseYLabel(&Object->Labelling,Nvert,Yddd,LineIsConcave,LineIsConcave,LineIsConcave);
      ClearY(YP);
      YP->Ydddmerit = 1.0;
   }
}


void CCueLabelsVarley_Relaxation::UpdateProbabilitiesForYJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   EDGE_INDEX Np,Nq,Nr;

   MERIT PA0 = PEdgeA(Object,Nvert,0);
   MERIT PB0 = PEdgeB(Object,Nvert,0);
   MERIT PC0 = PEdgeC(Object,Nvert,0);
   MERIT PD0 = PEdgeD(Object,Nvert,0);
   MERIT PA1 = PEdgeA(Object,Nvert,1);
   MERIT PB1 = PEdgeB(Object,Nvert,1);
   MERIT PC1 = PEdgeC(Object,Nvert,1);
   MERIT PD1 = PEdgeD(Object,Nvert,1);
   MERIT PA2 = PEdgeA(Object,Nvert,2);
   MERIT PB2 = PEdgeB(Object,Nvert,2);
   MERIT PC2 = PEdgeC(Object,Nvert,2);
   MERIT PD2 = PEdgeD(Object,Nvert,2);

   CheckVertexNumber(&Object->Labelling,Nvert,"UpdateProbabilitiesForYJunction");

   AdvancedRelax(&YP->Ycccmerit,LC->tcSp,PC0 * PC1 * PC2);
   AdvancedRelax(&YP->Ydddmerit,LC->tcSp,PD0 * PD1 * PD2);
   AdvancedRelax(&YP->Yabdmerit,LC->tcSp,PA0 * PB1 * PD2);
   AdvancedRelax(&YP->Ybdamerit,LC->tcSp,PB0 * PD1 * PA2);
   AdvancedRelax(&YP->Ydabmerit,LC->tcSp,PD0 * PA1 * PB2);
   AdvancedRelax(&YP->Yaabmerit,LC->tcSp,PA0 * PA1 * PB2);
   AdvancedRelax(&YP->Yabamerit,LC->tcSp,PA0 * PB1 * PA2);
   AdvancedRelax(&YP->Ybaamerit,LC->tcSp,PB0 * PA1 * PA2);
   AdvancedRelax(&YP->Yabbmerit,LC->tcSp,PA0 * PB1 * PB2);
   AdvancedRelax(&YP->Ybbamerit,LC->tcSp,PB0 * PB1 * PA2);
   AdvancedRelax(&YP->Ybabmerit,LC->tcSp,PB0 * PA1 * PB2);
   AdvancedRelax(&YP->Yabcmerit,LC->tcSp,PA0 * PB1 * PC2);
   AdvancedRelax(&YP->Ybcamerit,LC->tcSp,PB0 * PC1 * PA2);
   AdvancedRelax(&YP->Ycabmerit,LC->tcSp,PC0 * PA1 * PB2);
   AdvancedRelax(&YP->Yaccmerit,LC->tcSp,PA0 * PC1 * PC2);
   AdvancedRelax(&YP->Yccamerit,LC->tcSp,PC0 * PC1 * PA2);
   AdvancedRelax(&YP->Ycacmerit,LC->tcSp,PC0 * PA1 * PC2);
   AdvancedRelax(&YP->Yacdmerit,LC->tcSp,PA0 * PC1 * PD2);
   AdvancedRelax(&YP->Ycdamerit,LC->tcSp,PC0 * PD1 * PA2);
   AdvancedRelax(&YP->Ydacmerit,LC->tcSp,PD0 * PA1 * PC2);
   AdvancedRelax(&YP->Yaddmerit,LC->tcSp,PA0 * PD1 * PD2);
   AdvancedRelax(&YP->Yddamerit,LC->tcSp,PD0 * PD1 * PA2);
   AdvancedRelax(&YP->Ydadmerit,LC->tcSp,PD0 * PA1 * PD2);
   AdvancedRelax(&YP->Ybccmerit,LC->tcSp,PB0 * PC1 * PC2);
   AdvancedRelax(&YP->Yccbmerit,LC->tcSp,PC0 * PC1 * PB2);
   AdvancedRelax(&YP->Ycbcmerit,LC->tcSp,PC0 * PB1 * PC2);
   AdvancedRelax(&YP->Ybdcmerit,LC->tcSp,PB0 * PD1 * PC2);
   AdvancedRelax(&YP->Ydcbmerit,LC->tcSp,PD0 * PC1 * PB2);
   AdvancedRelax(&YP->Ycbdmerit,LC->tcSp,PC0 * PB1 * PD2);
   AdvancedRelax(&YP->Ybddmerit,LC->tcSp,PB0 * PD1 * PD2);
   AdvancedRelax(&YP->Yddbmerit,LC->tcSp,PD0 * PD1 * PB2);
   AdvancedRelax(&YP->Ydbdmerit,LC->tcSp,PD0 * PB1 * PD2);

   MERIT Xonly = LP->XJunctionPresenceMerit + (1.0-LP->XJunctionPresenceMerit)*(1.0-LC->tcSm);
   MERIT XorK  = dmax2(LP->KJunctionPresenceMerit,LP->XJunctionPresenceMerit) + (1.0-dmax2(LP->KJunctionPresenceMerit,LP->XJunctionPresenceMerit))*(1.0-LC->tcSm);

   YP->Yaabmerit *= Xonly;
   YP->Yabamerit *= Xonly;
   YP->Ybaamerit *= Xonly;
   YP->Yabbmerit *= Xonly;
   YP->Ybbamerit *= Xonly;
   YP->Ybabmerit *= Xonly;
   YP->Yabcmerit *= Xonly;
   YP->Ybcamerit *= Xonly;
   YP->Ycabmerit *= Xonly;
   YP->Yaccmerit *= Xonly;
   YP->Yccamerit *= Xonly;
   YP->Ycacmerit *= Xonly;
   YP->Yacdmerit *= XorK;
   YP->Ycdamerit *= XorK;
   YP->Ydacmerit *= XorK;
   YP->Yaddmerit *= XorK;
   YP->Yddamerit *= XorK;
   YP->Ydadmerit *= XorK;
   YP->Ybccmerit *= Xonly;
   YP->Yccbmerit *= Xonly;
   YP->Ycbcmerit *= Xonly;
   YP->Ybdcmerit *= XorK;
   YP->Ydcbmerit *= XorK;
   YP->Ycbdmerit *= XorK;
   YP->Ybddmerit *= XorK;
   YP->Yddbmerit *= XorK;
   YP->Ydbdmerit *= XorK;

   NormaliseYProbabilities(YP);

   if (YP->Ycccmerit > 0.9999)  LabelVertexAsYccc(Object,Nvert);
   else
   if (YP->Ydddmerit > 0.9999)  LabelVertexAsYddd(Object,Nvert);
   else
   if (YP->Yabdmerit > 0.9999)
   {
      FinaliseYLabel(&Object->Labelling,Nvert,Yabd,LineIsArrow,LineIsBackArrow,LineIsConcave);
      ClearY(YP);
      YP->Yabdmerit = 1.0;
   }
   else
   if (YP->Ybdamerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
      FinaliseYLabel(&Object->Labelling,Nvert,Yabd,LineIsArrow,LineIsBackArrow,LineIsConcave);
      ClearY(YP);
      YP->Yabdmerit = 1.0;
   }
   else
   if (YP->Ydabmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Np;
      FinaliseYLabel(&Object->Labelling,Nvert,Yabd,LineIsArrow,LineIsBackArrow,LineIsConcave);
      ClearY(YP);
      YP->Yabdmerit = 1.0;
   }
   else
   if (YP->Yaabmerit > 0.9999)
   {
	  FinaliseYLabel(&Object->Labelling,Nvert,Yaab,LineIsArrow,LineIsArrow,LineIsBackArrow);
      ClearY(YP);
      YP->Yaabmerit = 1.0;
   }
   else
   if (YP->Yabamerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
	  FinaliseYLabel(&Object->Labelling,Nvert,Yaab,LineIsArrow,LineIsArrow,LineIsBackArrow);
      ClearY(YP);
      YP->Yaabmerit = 1.0;
   }
   else
   if (YP->Ybaamerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Np;
	  FinaliseYLabel(&Object->Labelling,Nvert,Yaab,LineIsArrow,LineIsArrow,LineIsBackArrow);
      ClearY(YP);
      YP->Yaabmerit = 1.0;
   }
   else
   if (YP->Yabbmerit > 0.9999)
   {
	  FinaliseYLabel(&Object->Labelling,Nvert,Yabb,LineIsArrow,LineIsBackArrow,LineIsBackArrow);
      ClearY(YP);
      YP->Yabbmerit = 1.0;
   }
   else
   if (YP->Ybbamerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
	  FinaliseYLabel(&Object->Labelling,Nvert,Yabb,LineIsArrow,LineIsBackArrow,LineIsBackArrow);
      ClearY(YP);
      YP->Yabbmerit = 1.0;
   }
   else
   if (YP->Ybabmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Np;
	  FinaliseYLabel(&Object->Labelling,Nvert,Yabb,LineIsArrow,LineIsBackArrow,LineIsBackArrow);
      ClearY(YP);
      YP->Yabbmerit = 1.0;
   }
   else
   if (YP->Yabcmerit > 0.9999)
   {
      FinaliseYLabel(&Object->Labelling,Nvert,Yabc,LineIsArrow,LineIsBackArrow,LineIsConvex);
      ClearY(YP);
      YP->Yabcmerit = 1.0;
   }
   else
   if (YP->Ybcamerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
      FinaliseYLabel(&Object->Labelling,Nvert,Yabc,LineIsArrow,LineIsBackArrow,LineIsConvex);
      ClearY(YP);
      YP->Yabcmerit = 1.0;
   }
   else
   if (YP->Ycabmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Np;
      FinaliseYLabel(&Object->Labelling,Nvert,Yabc,LineIsArrow,LineIsBackArrow,LineIsConvex);
      ClearY(YP);
      YP->Yabcmerit = 1.0;
   }
   else
   if (YP->Yaccmerit > 0.9999)
   {
	  FinaliseYLabel(&Object->Labelling,Nvert,Yacc,LineIsArrow,LineIsConvex,LineIsConvex);
      ClearY(YP);
      YP->Yaccmerit = 1.0;
   }
   else
   if (YP->Yccamerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
	  FinaliseYLabel(&Object->Labelling,Nvert,Yacc,LineIsArrow,LineIsConvex,LineIsConvex);
      ClearY(YP);
      YP->Yaccmerit = 1.0;
   }
   else
   if (YP->Ycacmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Np;
	  FinaliseYLabel(&Object->Labelling,Nvert,Yacc,LineIsArrow,LineIsConvex,LineIsConvex);
      ClearY(YP);
      YP->Yaccmerit = 1.0;
   }
   else
   if (YP->Yacdmerit > 0.9999)
   {
	  FinaliseYLabel(&Object->Labelling,Nvert,Yacd,LineIsArrow,LineIsConvex,LineIsConcave);
      ClearY(YP);
      YP->Yacdmerit = 1.0;
   }
   else
   if (YP->Ycdamerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
	  FinaliseYLabel(&Object->Labelling,Nvert,Yacd,LineIsArrow,LineIsConvex,LineIsConcave);
      ClearY(YP);
      YP->Yacdmerit = 1.0;
   }
   else
   if (YP->Ydacmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Np;
	  FinaliseYLabel(&Object->Labelling,Nvert,Yacd,LineIsArrow,LineIsConvex,LineIsConcave);
      ClearY(YP);
      YP->Yacdmerit = 1.0;
   }
   else
   if (YP->Yaddmerit > 0.9999)
   {
	  FinaliseYLabel(&Object->Labelling,Nvert,Yadd,LineIsArrow,LineIsConcave,LineIsConcave);
      ClearY(YP);
      YP->Yaddmerit = 1.0;
   }
   else
   if (YP->Yddamerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
	  FinaliseYLabel(&Object->Labelling,Nvert,Yadd,LineIsArrow,LineIsConcave,LineIsConcave);
      ClearY(YP);
      YP->Yaddmerit = 1.0;
   }
   else
   if (YP->Ydadmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Np;
	  FinaliseYLabel(&Object->Labelling,Nvert,Yadd,LineIsArrow,LineIsConcave,LineIsConcave);
      ClearY(YP);
      YP->Yaddmerit = 1.0;
   }
   else
   if (YP->Ybccmerit > 0.9999)
   {
	  FinaliseYLabel(&Object->Labelling,Nvert,Ybcc,LineIsBackArrow,LineIsConvex,LineIsConvex);
      ClearY(YP);
      YP->Ybccmerit = 1.0;
   }
   else
   if (YP->Yccbmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
	  FinaliseYLabel(&Object->Labelling,Nvert,Ybcc,LineIsBackArrow,LineIsConvex,LineIsConvex);
      ClearY(YP);
      YP->Ybccmerit = 1.0;
   }
   else
   if (YP->Ycbcmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Np;
	  FinaliseYLabel(&Object->Labelling,Nvert,Ybcc,LineIsBackArrow,LineIsConvex,LineIsConvex);
      ClearY(YP);
      YP->Ybccmerit = 1.0;
   }
   else
   if (YP->Ybdcmerit > 0.9999)
   {
      FinaliseYLabel(&Object->Labelling,Nvert,Ybdc,LineIsBackArrow,LineIsConcave,LineIsConvex);
      ClearY(YP);
      YP->Ybdcmerit = 1.0;
   }
   else
   if (YP->Ydcbmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
      FinaliseYLabel(&Object->Labelling,Nvert,Ybdc,LineIsBackArrow,LineIsConcave,LineIsConvex);
      ClearY(YP);
      YP->Ybdcmerit = 1.0;
   }
   else
   if (YP->Ycbdmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Np;
      FinaliseYLabel(&Object->Labelling,Nvert,Ybdc,LineIsBackArrow,LineIsConcave,LineIsConvex);
      ClearY(YP);
      YP->Ybdcmerit = 1.0;
   }
   else
   if (YP->Ybddmerit > 0.9999)
   {
      FinaliseYLabel(&Object->Labelling,Nvert,Ybdd,LineIsBackArrow,LineIsConcave,LineIsConcave);
      ClearY(YP);
      YP->Ybddmerit = 1.0;
   }
   else
   if (YP->Yddbmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nr;
      Object->Labelling.VENumber[Nvert][1] = Np;
      Object->Labelling.VENumber[Nvert][2] = Nq;
      FinaliseYLabel(&Object->Labelling,Nvert,Ybdd,LineIsBackArrow,LineIsConcave,LineIsConcave);
      ClearY(YP);
      YP->Ybddmerit = 1.0;
   }
   else
   if (YP->Ydbdmerit > 0.9999)
   {
      Np = Object->Labelling.VENumber[Nvert][0];
      Nq = Object->Labelling.VENumber[Nvert][1];
      Nr = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Nr;
      Object->Labelling.VENumber[Nvert][2] = Np;
      FinaliseYLabel(&Object->Labelling,Nvert,Ybdd,LineIsBackArrow,LineIsConcave,LineIsConcave);
      ClearY(YP);
      YP->Ybddmerit = 1.0;
   }
   else
   {
      //PrintYProbabilities(Labelling,Nvert);
   }
}


void CCueLabelsVarley_Relaxation::UpdateProbabilitiesForVertex (OBJECT *Object, VERTEX_INDEX Nvert)
{
   switch (Object->Labelling.VertexLabel[Nvert].Shape)
   {
      case JunctionIsK:
         UpdateProbabilitiesForKJunction(Object,Nvert);
      break;

      case JunctionIsL:
         UpdateProbabilitiesForLJunction(Object,Nvert);
      break;

      case JunctionIsM:
         UpdateProbabilitiesForMJunction(Object,Nvert);
      break;

      case JunctionIsT:
      case JunctionIsT4:
         UpdateProbabilitiesForTJunction(Object,Nvert);
      break;

      case JunctionIsW:
         UpdateProbabilitiesForWJunction(Object,Nvert);
      break;

      case JunctionIsX:
         UpdateProbabilitiesForXJunction(Object,Nvert);
      break;

      case JunctionIs5way:
         UpdateProbabilitiesForX5Junction(Object,Nvert);
      break;

      case JunctionIsZ6:
      case JunctionIs6way:
         UpdateProbabilitiesForX6Junction(Object,Nvert);
      break;

      case JunctionIsY:
         UpdateProbabilitiesForYJunction(Object,Nvert);
      break;

      case JunctionIsZ:
         // nothing to do
      break;

      case JunctionIs7way:
      case JunctionIs8way:
      case JunctionIsKplus:
         // these aren't in the junction catalogues, so don't do anything
      break;

      default:
         CString sMensaje, sMensajeAux;
         sMensaje= "Labelling error: UPDATE V ";
         //sMensajeAux.Format("%s", VertexString(Object,&Object->Labelling,Nvert));
         //sMensaje+=sMensajeAux;
         AfxMessageBox(sMensaje);
         //exit(BADNEWS);
   }
}


MERIT CCueLabelsVarley_Relaxation::HKVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      MERIT *KP = Object->Vprobabilities[Nvert];
      return dmax2(KP[Kabcdmerit],KP[Kabdcmerit]);
   }

   // other edges cannot be A
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HKVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      MERIT *KP = Object->Vprobabilities[Nvert];
      return dmax2(KP[Kabcdmerit],KP[Kabdcmerit]);
   }

   // other edges cannot be B
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HKVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *KP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax3(KP[Kcccdmerit],KP[Kccdcmerit],KP[Kcdcdmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax3(KP[Kcccdmerit],KP[Kccdcmerit],KP[Kdcdcmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax4(KP[Kabcdmerit],KP[Kcccdmerit],KP[Kcdcdmerit],KP[Kddcdmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return dmax4(KP[Kabdcmerit],KP[Kccdcmerit],KP[Kdcdcmerit],KP[Kdddcmerit]);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HKVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *KP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax3(KP[Kdcdcmerit],KP[Kddcdmerit],KP[Kdddcmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax3(KP[Kcdcdmerit],KP[Kddcdmerit],KP[Kdddcmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax4(KP[Kabdcmerit],KP[Kccdcmerit],KP[Kdcdcmerit],KP[Kdddcmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return dmax4(KP[Kabcdmerit],KP[Kcccdmerit],KP[Kcdcdmerit],KP[Kddcdmerit]);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HLVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *LP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax3(LP[Labmerit],LP[Lacmerit],LP[Laamerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax3(LP[Lbamerit],LP[Ldamerit],LP[Laamerit]);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HLVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *LP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax3(LP[Lbamerit],LP[Lbdmerit],LP[Lbbmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax3(LP[Labmerit],LP[Lcbmerit],LP[Lbbmerit]);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HLVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   if (Nedge == Object->Labelling.VENumber[Nvert][0])  return Object->Vprobabilities[Nvert][Lcbmerit];
   if (Nedge == Object->Labelling.VENumber[Nvert][1])  return Object->Vprobabilities[Nvert][Lacmerit];
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HLVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   if (Nedge == Object->Labelling.VENumber[Nvert][0])  return Object->Vprobabilities[Nvert][Ldamerit];
   if (Nedge == Object->Labelling.VENumber[Nvert][1])  return Object->Vprobabilities[Nvert][Lbdmerit];
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HMVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      MERIT *MP = Object->Vprobabilities[Nvert];
      return dmax3(MP[Mbccamerit],MP[Mbcdamerit],MP[Mbdcamerit]);
   }

   // other edges cannot be A
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HMVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      MERIT *MP = Object->Vprobabilities[Nvert];
      return dmax3(MP[Mbccamerit],MP[Mbcdamerit],MP[Mbdcamerit]);
   }

   // other edges cannot be B
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HMVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *MP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax4(MP[Mccdcmerit],MP[Mcdccmerit],MP[Mcdcdmerit],MP[Mcddcmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax6(MP[Mbccamerit],MP[Mbcdamerit],MP[Mccdcmerit],MP[Mdccdmerit],MP[Mdcdcmerit],MP[Mdcddmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax6(MP[Mbccamerit],MP[Mbdcamerit],MP[Mcdccmerit],MP[Mcdcdmerit],MP[Mdccdmerit],MP[Mddcdmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return dmax4(MP[Mccdcmerit],MP[Mcdccmerit],MP[Mcddcmerit],MP[Mdcdcmerit]);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HMVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *MP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax4(MP[Mdccdmerit],MP[Mdcdcmerit],MP[Mdcddmerit],MP[Mddcdmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax5(MP[Mbdcamerit],MP[Mcdccmerit],MP[Mcdcdmerit],MP[Mcddcmerit],MP[Mddcdmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax5(MP[Mbcdamerit],MP[Mccdcmerit],MP[Mcddcmerit],MP[Mdcdcmerit],MP[Mdcddmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return dmax4(MP[Mcdcdmerit],MP[Mdccdmerit],MP[Mdcddmerit],MP[Mddcdmerit]);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HTVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *TP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax2(TP[Tabamerit],TP[Tabbmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax8(TP[Tbaamerit],TP[Tbabmerit],TP[Tbacmerit],TP[Tbadmerit],TP[Tcabmerit],TP[Tcacmerit],TP[Tdabmerit],TP[Tdacmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax7(TP[Tbaamerit],TP[Tbdamerit],TP[Tabamerit],TP[Tbcamerit],TP[Tccamerit],TP[Tcdamerit],TP[Tddamerit]);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HTVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *TP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax8(TP[Tbaamerit],TP[Tbabmerit],TP[Tbacmerit],TP[Tbadmerit],TP[Tbdamerit],TP[Tbdcmerit],TP[Tbcamerit],TP[Tbccmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax2(TP[Tabamerit],TP[Tabbmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax7(TP[Tbabmerit],TP[Tdabmerit],TP[Tabbmerit],TP[Tcabmerit],TP[Tccbmerit],TP[Tdcbmerit],TP[Tddbmerit]);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HTVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *TP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax5(TP[Tcabmerit],TP[Tcacmerit],TP[Tccamerit],TP[Tccbmerit],TP[Tcdamerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax5(TP[Tbcamerit],TP[Tbccmerit],TP[Tccamerit],TP[Tccbmerit],TP[Tdcbmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax5(TP[Tbacmerit],TP[Tbdcmerit],TP[Tbccmerit],TP[Tcacmerit],TP[Tdacmerit]);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HTVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *TP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax5(TP[Tdabmerit],TP[Tdacmerit],TP[Tdcbmerit],TP[Tddamerit],TP[Tddbmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax5(TP[Tbdamerit],TP[Tbdcmerit],TP[Tcdamerit],TP[Tddamerit],TP[Tddbmerit]);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return TP[Tbadmerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HWVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax5(WP->Wabcmerit,WP->Wabdmerit,WP->Waccmerit,WP->Wacdmerit,WP->Wadcmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax6(WP->Wbaamerit,WP->Wcabmerit,WP->Wcacmerit,WP->Wdabmerit,WP->Wdacmerit,WP->Wdadmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax7(WP->Wbcamerit,WP->Wbaamerit,WP->Wbbamerit,WP->Wbdamerit,WP->Wcdamerit,WP->Wdcamerit,WP->Wddamerit);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HWVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax7(WP->Wbcamerit,WP->Wbaamerit,WP->Wbbamerit,WP->Wbcdmerit,WP->Wbdamerit,WP->Wbdcmerit,WP->Wbddmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax6(WP->Wabcmerit,WP->Wabdmerit,WP->Wbbamerit,WP->Wcbcmerit,WP->Wcbdmerit,WP->Wdbdmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax5(WP->Wcabmerit,WP->Wccbmerit,WP->Wcdbmerit,WP->Wdabmerit,WP->Wdcbmerit);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HWVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax8(WP->Wcdcmerit,WP->Wcabmerit,WP->Wcacmerit,WP->Wcbcmerit,WP->Wcbdmerit,WP->Wccbmerit,WP->Wcdamerit,WP->Wcdbmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax8(WP->Wbcamerit,WP->Wdcdmerit,WP->Waccmerit,WP->Wacdmerit,WP->Wbcdmerit,WP->Wccbmerit,WP->Wdcamerit,WP->Wdcbmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax8(WP->Wcdcmerit,WP->Wabcmerit,WP->Waccmerit,WP->Wadcmerit,WP->Wbdcmerit,WP->Wcacmerit,WP->Wcbcmerit,WP->Wdacmerit);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HWVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax8(WP->Wdcdmerit,WP->Wdabmerit,WP->Wdacmerit,WP->Wdadmerit,WP->Wdbdmerit,WP->Wdcamerit,WP->Wdcbmerit,WP->Wddamerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax8(WP->Wcdcmerit,WP->Wadcmerit,WP->Wbdamerit,WP->Wbdcmerit,WP->Wbddmerit,WP->Wcdamerit,WP->Wcdbmerit,WP->Wddamerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax8(WP->Wdcdmerit,WP->Wabdmerit,WP->Wacdmerit,WP->Wbcdmerit,WP->Wbddmerit,WP->Wcbdmerit,WP->Wdadmerit,WP->Wdbdmerit);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HXVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   XPROBABILITIES *XP = (XPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax3(XP->Xabcdmerit,XP->Xabdcmerit,XP->Xabddmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax3(XP->Xdabcmerit,XP->Xcabdmerit,XP->Xdabdmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax3(XP->Xcdabmerit,XP->Xdcabmerit,XP->Xddabmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return dmax3(XP->Xbcdamerit,XP->Xbdcamerit,XP->Xbddamerit);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HXVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   XPROBABILITIES *XP = (XPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax3(XP->Xbcdamerit,XP->Xbdcamerit,XP->Xbddamerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax3(XP->Xabcdmerit,XP->Xabdcmerit,XP->Xabddmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax3(XP->Xdabcmerit,XP->Xcabdmerit,XP->Xdabdmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return dmax3(XP->Xcdabmerit,XP->Xdcabmerit,XP->Xddabmerit);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HXVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   XPROBABILITIES *XP = (XPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax8(XP->Xcdabmerit,XP->Xcabdmerit,XP->Xccccmerit,XP->Xcccdmerit,XP->Xccdcmerit,XP->Xcdccmerit,XP->Xcdcdmerit,XP->Xcdddmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax8(XP->Xbcdamerit,XP->Xdcabmerit,XP->Xccccmerit,XP->Xcccdmerit,XP->Xccdcmerit,XP->Xdcccmerit,XP->Xdcdcmerit,XP->Xdcddmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax8(XP->Xabcdmerit,XP->Xbdcamerit,XP->Xccccmerit,XP->Xcccdmerit,XP->Xcdccmerit,XP->Xdcccmerit,XP->Xcdcdmerit,XP->Xddcdmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return dmax8(XP->Xdabcmerit,XP->Xabdcmerit,XP->Xccccmerit,XP->Xccdcmerit,XP->Xcdccmerit,XP->Xdcccmerit,XP->Xdcdcmerit,XP->Xdddcmerit);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HXVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   XPROBABILITIES *XP = (XPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax10(XP->Xdabcmerit,XP->Xdcabmerit,XP->Xddabmerit,XP->Xdabdmerit,XP->Xcccdmerit,XP->Xdcdcmerit,XP->Xdcddmerit,XP->Xddcdmerit,XP->Xdddcmerit,XP->Xddddmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax10(XP->Xcdabmerit,XP->Xbdcamerit,XP->Xbddamerit,XP->Xddabmerit,XP->Xccdcmerit,XP->Xcdcdmerit,XP->Xcdddmerit,XP->Xddcdmerit,XP->Xdddcmerit,XP->Xddddmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax10(XP->Xbcdamerit,XP->Xabdcmerit,XP->Xabddmerit,XP->Xbddamerit,XP->Xcdccmerit,XP->Xdcdcmerit,XP->Xcdddmerit,XP->Xdcddmerit,XP->Xdddcmerit,XP->Xddddmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return dmax10(XP->Xabcdmerit,XP->Xcabdmerit,XP->Xabddmerit,XP->Xdabdmerit,XP->Xdcccmerit,XP->Xdcdcmerit,XP->Xcdddmerit,XP->Xdcddmerit,XP->Xddcdmerit,XP->Xddddmerit);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HYVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax8(YP->Yabdmerit,YP->Yaabmerit,YP->Yabamerit,YP->Yabbmerit,YP->Yabcmerit,YP->Yaccmerit,YP->Yacdmerit,YP->Yaddmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax8(YP->Ydabmerit,YP->Yaabmerit,YP->Ybaamerit,YP->Ybabmerit,YP->Ycabmerit,YP->Ycacmerit,YP->Ydacmerit,YP->Ydadmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax8(YP->Ybdamerit,YP->Yabamerit,YP->Ybaamerit,YP->Ybbamerit,YP->Ybcamerit,YP->Yccamerit,YP->Ycdamerit,YP->Yddamerit);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HYVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax8(YP->Ybdamerit,YP->Ybaamerit,YP->Ybbamerit,YP->Ybabmerit,YP->Ybcamerit,YP->Ybccmerit,YP->Ybdcmerit,YP->Ybddmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax8(YP->Yabdmerit,YP->Yabamerit,YP->Yabbmerit,YP->Ybbamerit,YP->Yabcmerit,YP->Ycbcmerit,YP->Ycbdmerit,YP->Ydbdmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax8(YP->Ydabmerit,YP->Yaabmerit,YP->Yabbmerit,YP->Ybabmerit,YP->Ycabmerit,YP->Yccbmerit,YP->Ydcbmerit,YP->Yddbmerit);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HYVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax8(YP->Ycccmerit,YP->Ycabmerit,YP->Ycacmerit,YP->Yccamerit,YP->Ycdamerit,YP->Ycbcmerit,YP->Yccbmerit,YP->Ycbdmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax8(YP->Ycccmerit,YP->Ybcamerit,YP->Yaccmerit,YP->Yccamerit,YP->Yacdmerit,YP->Ybccmerit,YP->Yccbmerit,YP->Ydcbmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax8(YP->Ycccmerit,YP->Yabcmerit,YP->Yaccmerit,YP->Ycacmerit,YP->Ydacmerit,YP->Ybccmerit,YP->Ycbcmerit,YP->Ybdcmerit);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HYVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return dmax8(YP->Ydddmerit,YP->Ydabmerit,YP->Ydacmerit,YP->Yddamerit,YP->Ydadmerit,YP->Ydcbmerit,YP->Ydbdmerit,YP->Yddbmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return dmax8(YP->Ydddmerit,YP->Ybdamerit,YP->Ycdamerit,YP->Yaddmerit,YP->Yddamerit,YP->Ybdcmerit,YP->Ybddmerit,YP->Yddbmerit);
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return dmax8(YP->Ydddmerit,YP->Yabdmerit,YP->Yacdmerit,YP->Yaddmerit,YP->Ydadmerit,YP->Ycbdmerit,YP->Ybddmerit,YP->Ydbdmerit);
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   switch (Object->Labelling.VertexLabel[Nvert].Shape)
   {
      case JunctionIsK:   return HKVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsL:   return HLVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsM:   return HMVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsT4:  return HTVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsW:   return HWVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsX:   return HXVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIs5way:return PX5VertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsZ6:
      case JunctionIs6way:return PX6VertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsY:   return HYVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsZ:   return PZVertexIsAatEdge(Object,Nvert,Nedge);
   }
   //CString sMensaje, sMensajeAux;
   //sMensaje= "Labelling error: Don't know how to work out Hvertex-a-at-edge, shape ";
   //sMensajeAux.Format("%c", Object->Labelling.VertexLabel[Nvert].Shape);
   //sMensaje+=sMensajeAux;
   //AfxMessageBox(sMensaje);
   //exit(999);

   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   switch (Object->Labelling.VertexLabel[Nvert].Shape)
   {
      case JunctionIsK:   return HKVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsL:   return HLVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsM:   return HMVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsT4:  return HTVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsW:   return HWVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsX:   return HXVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIs5way:return PX5VertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsZ6:
      case JunctionIs6way:return PX6VertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsY:   return HYVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsZ:   return PZVertexIsBatEdge(Object,Nvert,Nedge);
   }
   //CString sMensaje, sMensajeAux;
   //sMensaje= "Labelling error: Don't know how to work out Hvertex-b-at-edge, shape ";
   //sMensajeAux.Format("%c", Object->Labelling.VertexLabel[Nvert].Shape);
   //sMensaje+=sMensajeAux;
   //AfxMessageBox(sMensaje);
   //exit(999);
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   switch (Object->Labelling.VertexLabel[Nvert].Shape)
   {
      case JunctionIsK:   return HKVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsL:   return HLVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsM:   return HMVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsT4:  return HTVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsW:   return HWVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsX:   return HXVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIs5way:return PX5VertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsZ6:
      case JunctionIs6way:return PX6VertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsY:   return HYVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsZ:   return PZVertexIsCatEdge(Object,Nvert,Nedge);
   }
   //CString sMensaje, sMensajeAux;
   //sMensaje= "Labelling error: Don't know how to work out Hvertex-c-at-edge, shape ";
   //sMensajeAux.Format("%c", Object->Labelling.VertexLabel[Nvert].Shape);
   //sMensaje+=sMensajeAux;
   //AfxMessageBox(sMensaje);
   //exit(999);

   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::HVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   switch (Object->Labelling.VertexLabel[Nvert].Shape)
   {
      case JunctionIsK:   return HKVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsL:   return HLVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsM:   return HMVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsT4:  return HTVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsW:   return HWVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsX:   return HXVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIs5way:return PX5VertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsZ6:
      case JunctionIs6way:return PX6VertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsY:   return HYVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsZ:   return PZVertexIsDatEdge(Object,Nvert,Nedge);
   }
   //CString sMensaje, sMensajeAux;
   //sMensaje= "Labelling error: Don't know how to work out Hvertex-d-at-edge, shape ";
   //sMensajeAux.Format("%c", Object->Labelling.VertexLabel[Nvert].Shape);
   //sMensaje+=sMensajeAux;
   //AfxMessageBox(sMensaje);
   //exit(999);

   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PKVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      MERIT *KP = Object->Vprobabilities[Nvert];
      return KP[Kabcdmerit] + KP[Kabdcmerit];
   }

   // Edges 1, 2, 3 cannot be A
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PKVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      MERIT *KP = Object->Vprobabilities[Nvert];
      return KP[Kabcdmerit] + KP[Kabdcmerit];
   }

   // Edges 0, 2, 3 cannot be B
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PKVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *KP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return KP[Kcccdmerit] + KP[Kccdcmerit] + KP[Kcdcdmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return KP[Kcccdmerit] + KP[Kccdcmerit] + KP[Kdcdcmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return KP[Kabcdmerit] + KP[Kcccdmerit] + KP[Kcdcdmerit] + KP[Kddcdmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return KP[Kabdcmerit] + KP[Kccdcmerit] + KP[Kdcdcmerit] + KP[Kdddcmerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PKVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *KP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return KP[Kdcdcmerit] + KP[Kddcdmerit] + KP[Kdddcmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return KP[Kcdcdmerit] + KP[Kddcdmerit] + KP[Kdddcmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return KP[Kabdcmerit] + KP[Kccdcmerit] + KP[Kdcdcmerit] + KP[Kdddcmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return KP[Kabcdmerit] + KP[Kcccdmerit] + KP[Kcdcdmerit] + KP[Kddcdmerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PKVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface)
{
   if ((Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][1]])
    || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][1]]))
   {
      if ((Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][0]])
       || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][0]]))
      {
         MERIT *KP = Object->Vprobabilities[Nvert];
         return KP[Kabcdmerit] + KP[Kabdcmerit];
      }
   }

   // edges 2 and 3 are never occluding at a K-junction
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PLVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *LP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return LP[Labmerit] + LP[Lacmerit] + LP[Laamerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return LP[Lbamerit] + LP[Ldamerit] + LP[Laamerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PLVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *LP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return LP[Lbamerit] + LP[Lbdmerit] + LP[Lbbmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return LP[Labmerit] + LP[Lcbmerit] + LP[Lbbmerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PLVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   if (Nedge == Object->Labelling.VENumber[Nvert][0])  return Object->Vprobabilities[Nvert][Lcbmerit];
   if (Nedge == Object->Labelling.VENumber[Nvert][1])  return Object->Vprobabilities[Nvert][Lacmerit];
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PLVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   if (Nedge == Object->Labelling.VENumber[Nvert][0])  return Object->Vprobabilities[Nvert][Ldamerit];
   if (Nedge == Object->Labelling.VENumber[Nvert][1])  return Object->Vprobabilities[Nvert][Lbdmerit];
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PLVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface)
{
   if ((Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][0]])
    || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][0]]))
   {
      if ((Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][1]])
       || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][1]]))
      {
         MERIT *LP = Object->Vprobabilities[Nvert];
         //TBD: L-junctions are particularly tricky, since with the information
         //     available in the labelling record it isn't possible to determine
         //     whether the face is the inner or the outer face
         //     so make a wild guess and hope for the best
         return dmax2(LP[Lbamerit],LP[Labmerit]);
      }
   }

   //AfxMessageBox("Labelling error: Don't know how to work out whether L-vertex occludes face!");

   return 0.382;  // guess
   //exit(999);
   //return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PMVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      MERIT *MP = Object->Vprobabilities[Nvert];
      return MP[Mbccamerit] + MP[Mbcdamerit] + MP[Mbdcamerit];
   }

   // other edges cannot be A
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PMVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      MERIT *MP = Object->Vprobabilities[Nvert];
      return MP[Mbccamerit] + MP[Mbcdamerit] + MP[Mbdcamerit];
   }

   // other edges cannot be B
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PMVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *MP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return MP[Mccdcmerit] + MP[Mcdccmerit] + MP[Mcdcdmerit] + MP[Mcddcmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return MP[Mbccamerit] + MP[Mbcdamerit] + MP[Mccdcmerit] + MP[Mdccdmerit] + MP[Mdcdcmerit] + MP[Mdcddmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return MP[Mbccamerit] + MP[Mbdcamerit] + MP[Mcdccmerit] + MP[Mcdcdmerit] + MP[Mdccdmerit] + MP[Mddcdmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return MP[Mccdcmerit] + MP[Mcdccmerit] + MP[Mcddcmerit] + MP[Mdcdcmerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PMVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *MP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return MP[Mdccdmerit] + MP[Mdcdcmerit] + MP[Mdcddmerit] + MP[Mddcdmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return MP[Mbdcamerit] + MP[Mcdccmerit] + MP[Mcdcdmerit] + MP[Mcddcmerit] + MP[Mddcdmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return MP[Mbcdamerit] + MP[Mccdcmerit] + MP[Mcddcmerit] + MP[Mdcdcmerit] + MP[Mdcddmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return MP[Mcdcdmerit] + MP[Mdccdmerit] + MP[Mdcddmerit] + MP[Mddcdmerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PMVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface)
{
   if ((Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][3]])
    || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][3]]))
   {
      if ((Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][0]])
       || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][0]]))
      {
         MERIT *MP = Object->Vprobabilities[Nvert];
         return MP[Mbccamerit] + MP[Mbcdamerit] + MP[Mbdcamerit];
      }
   }

   // central edges in an M-vertex are never occluding
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PTVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *TP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return TP[Tabamerit] + TP[Tabbmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return TP[Tbaamerit] + TP[Tbabmerit] + TP[Tbacmerit] + TP[Tbadmerit] + TP[Tcabmerit] + TP[Tcacmerit] + TP[Tdabmerit] + TP[Tdacmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return TP[Tbaamerit] + TP[Tbdamerit] + TP[Tabamerit] + TP[Tbcamerit] + TP[Tccamerit] + TP[Tcdamerit] + TP[Tddamerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PTVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *TP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return TP[Tbaamerit] + TP[Tbabmerit] + TP[Tbacmerit] + TP[Tbadmerit] + TP[Tbdamerit] + TP[Tbdcmerit] + TP[Tbcamerit] + TP[Tbccmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return TP[Tabamerit] + TP[Tabbmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return TP[Tbabmerit] + TP[Tdabmerit] + TP[Tabbmerit] + TP[Tcabmerit] + TP[Tccbmerit] + TP[Tdcbmerit] + TP[Tddbmerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PTVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *TP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return TP[Tcabmerit] + TP[Tcacmerit] + TP[Tccamerit] + TP[Tccbmerit] + TP[Tcdamerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return TP[Tbcamerit] + TP[Tbccmerit] + TP[Tccamerit] + TP[Tccbmerit] + TP[Tdcbmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return TP[Tbacmerit] + TP[Tbdcmerit] + TP[Tbccmerit] + TP[Tcacmerit] + TP[Tdacmerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PTVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *TP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return TP[Tdabmerit] + TP[Tdacmerit] + TP[Tdcbmerit] + TP[Tddamerit] + TP[Tddbmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return TP[Tbdamerit] + TP[Tbdcmerit] + TP[Tcdamerit] + TP[Tddamerit] + TP[Tddbmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return TP[Tbadmerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PTVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface, EDGE_INDEX Nedge)
{
   MERIT *TP = Object->Vprobabilities[Nvert];
   char FR2;
   //PrintTProbabilities(TP);
   //if (DebugUFG)
   //{
   //   fprintf(DebugUFG,"EF0: %d\n",Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][0]]);
   //   fprintf(DebugUFG,"ER0: %d\n",Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][0]]);
   //   fprintf(DebugUFG,"EF1: %d\n",Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][1]]);
   //   fprintf(DebugUFG,"ER1: %d\n",Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][1]]);
   //   fprintf(DebugUFG,"EF2: %d\n",Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][2]]);
   //   fprintf(DebugUFG,"ER2: %d\n",Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][2]]);
   //}

   char FR0 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][0]])
           || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][0]]);

   char FR1 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][1]])
           || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][1]]);

   if (FR0 && FR1)  return TP[Tabamerit] + TP[Tabbmerit];

   FR2 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][2]])
      || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][2]]);

   if (FR1 && FR2)
   {
      if (Nedge == Object->Labelling.VENumber[Nvert][2])
      {
         // does the back end occlude the face?
         return TP[Tbabmerit] + TP[Tcabmerit] + TP[Tdabmerit];
      }
      else
      {
         // does the front end occlude the face?
         return TP[Tbaamerit] + TP[Tbabmerit] + TP[Tbacmerit] + TP[Tbadmerit] + TP[Tcabmerit] + TP[Tdabmerit];
      }
   }

   if (FR2 && FR0)
   {
      if (Nedge == Object->Labelling.VENumber[Nvert][2])
      {
         // does the back end occlude the face?
         return TP[Tbaamerit] + TP[Tbcamerit] + TP[Tbdamerit];
      }
      else
      {
         // does the front end occlude the face?
         return TP[Tbaamerit] + TP[Tbabmerit] + TP[Tbacmerit] + TP[Tbadmerit] + TP[Tbcamerit] + TP[Tbdamerit];
      }
   }

   AfxMessageBox("Labelling error: Don't know how to work out whether T-vertex occludes face!");
   //exit(999);
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PWVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return WP->Wabcmerit + WP->Wabdmerit + WP->Waccmerit + WP->Wacdmerit + WP->Wadcmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return WP->Wbaamerit + WP->Wcabmerit + WP->Wcacmerit + WP->Wdabmerit + WP->Wdacmerit + WP->Wdadmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return WP->Wbcamerit + WP->Wbaamerit + WP->Wbbamerit + WP->Wbdamerit + WP->Wcdamerit + WP->Wdcamerit + WP->Wddamerit;
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PWVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return WP->Wbcamerit + WP->Wbaamerit + WP->Wbbamerit + WP->Wbcdmerit + WP->Wbdamerit + WP->Wbdcmerit + WP->Wbddmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return WP->Wabcmerit + WP->Wabdmerit + WP->Wbbamerit + WP->Wcbcmerit + WP->Wcbdmerit + WP->Wdbdmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return WP->Wcabmerit + WP->Wccbmerit + WP->Wcdbmerit + WP->Wdabmerit + WP->Wdcbmerit;
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PWVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   MERIT P = 0.0;

   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      P = WP->Wcdcmerit + WP->Wcabmerit + WP->Wcacmerit + WP->Wcbcmerit + WP->Wcbdmerit + WP->Wccbmerit + WP->Wcdamerit + WP->Wcdbmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      P = WP->Wbcamerit + WP->Wdcdmerit + WP->Waccmerit + WP->Wacdmerit + WP->Wbcdmerit + WP->Wccbmerit + WP->Wdcamerit + WP->Wdcbmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      P = WP->Wcdcmerit + WP->Wabcmerit + WP->Waccmerit + WP->Wadcmerit + WP->Wbdcmerit + WP->Wcacmerit + WP->Wcbcmerit + WP->Wdacmerit;
   }

   //printf("PW Vertex %d is C at edge %d is %f\n", Nvert, Nedge, P);
   return P;
}


MERIT CCueLabelsVarley_Relaxation::PWVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   MERIT P = 0.0;

   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      P = WP->Wdcdmerit + WP->Wdabmerit + WP->Wdacmerit + WP->Wdadmerit + WP->Wdbdmerit + WP->Wdcamerit + WP->Wdcbmerit + WP->Wddamerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      P = WP->Wcdcmerit + WP->Wadcmerit + WP->Wbdamerit + WP->Wbdcmerit + WP->Wbddmerit + WP->Wcdamerit + WP->Wcdbmerit + WP->Wddamerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      P = WP->Wdcdmerit + WP->Wabdmerit + WP->Wacdmerit + WP->Wbcdmerit + WP->Wbddmerit + WP->Wcbdmerit + WP->Wdadmerit + WP->Wdbdmerit;
   }

   //printf("PW Vertex %d is D at edge %d is %f\n", Nvert, Nedge, P);
   return P;
}


MERIT CCueLabelsVarley_Relaxation::PWVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface)
{
   WPROBABILITIES *WP = (WPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   char FR2;

   char FR0 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][0]])
           || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][0]]);

   char FR1 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][1]])
           || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][1]]);

   if (FR0 && FR1)  return WP->Wabcmerit + WP->Wabdmerit;

   FR2 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][2]])
      || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][2]]);

   if (FR1 && FR2)  return WP->Wcabmerit + WP->Wdabmerit;

   if (FR2 && FR0)  return WP->Wbaamerit + WP->Wbbamerit + WP->Wbcamerit + WP->Wbdamerit;

   //AfxMessageBox("Labelling error: Don't know how to work out whether W-vertex occludes face!");
   return 0.382;  // wild guess

   //exit(999);
   //return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PXVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   XPROBABILITIES *XP = (XPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return XP->Xabcdmerit + XP->Xabdcmerit + XP->Xabddmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return XP->Xdabcmerit + XP->Xcabdmerit + XP->Xdabdmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return XP->Xcdabmerit + XP->Xdcabmerit + XP->Xddabmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return XP->Xbcdamerit + XP->Xbdcamerit + XP->Xbddamerit;
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PXVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   XPROBABILITIES *XP = (XPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return XP->Xbcdamerit + XP->Xbdcamerit + XP->Xbddamerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return XP->Xabcdmerit + XP->Xabdcmerit + XP->Xabddmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return XP->Xdabcmerit + XP->Xcabdmerit + XP->Xdabdmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return XP->Xcdabmerit + XP->Xdcabmerit + XP->Xddabmerit;
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PXVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   XPROBABILITIES *XP = (XPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return XP->Xcdabmerit + XP->Xcabdmerit + XP->Xccccmerit + XP->Xcccdmerit + XP->Xccdcmerit + XP->Xcdccmerit + XP->Xcdcdmerit + XP->Xcdddmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return XP->Xbcdamerit + XP->Xdcabmerit + XP->Xccccmerit + XP->Xcccdmerit + XP->Xccdcmerit + XP->Xdcccmerit + XP->Xdcdcmerit + XP->Xdcddmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return XP->Xabcdmerit + XP->Xbdcamerit + XP->Xccccmerit + XP->Xcccdmerit + XP->Xcdccmerit + XP->Xdcccmerit + XP->Xcdcdmerit + XP->Xddcdmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return XP->Xdabcmerit + XP->Xabdcmerit + XP->Xccccmerit + XP->Xccdcmerit + XP->Xcdccmerit + XP->Xdcccmerit + XP->Xdcdcmerit + XP->Xdddcmerit;
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PXVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   XPROBABILITIES *XP = (XPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return XP->Xdabcmerit + XP->Xdcabmerit + XP->Xddabmerit + XP->Xdabdmerit + XP->Xcccdmerit + XP->Xdcdcmerit + XP->Xdcddmerit + XP->Xddcdmerit + XP->Xdddcmerit + XP->Xddddmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return XP->Xcdabmerit + XP->Xbdcamerit + XP->Xbddamerit + XP->Xddabmerit + XP->Xccdcmerit + XP->Xcdcdmerit + XP->Xcdddmerit + XP->Xddcdmerit + XP->Xdddcmerit + XP->Xddddmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return XP->Xbcdamerit + XP->Xabdcmerit + XP->Xabddmerit + XP->Xbddamerit + XP->Xcdccmerit + XP->Xdcdcmerit + XP->Xcdddmerit + XP->Xdcddmerit + XP->Xdddcmerit + XP->Xddddmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return XP->Xabcdmerit + XP->Xcabdmerit + XP->Xabddmerit + XP->Xdabdmerit + XP->Xdcccmerit + XP->Xdcdcmerit + XP->Xcdddmerit + XP->Xdcddmerit + XP->Xddcdmerit + XP->Xddddmerit;
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PXVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface)
{
   XPROBABILITIES *XP = (XPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   char FR2,FR3;

   char FR0 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][0]])
           || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][0]]);

   char FR1 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][1]])
           || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][1]]);

   if (FR0 && FR1)  return XP->Xabcdmerit + XP->Xabdcmerit + XP->Xabddmerit;

   FR2 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][2]])
      || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][2]]);

   if (FR1 && FR2)  return XP->Xdabcmerit + XP->Xcabdmerit + XP->Xdabdmerit;

   FR3 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][3]])
      || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][3]]);

   if (FR2 && FR3)  return XP->Xcdabmerit + XP->Xdcabmerit + XP->Xddabmerit;

   if (FR3 && FR0)  return XP->Xbcdamerit + XP->Xbdcamerit + XP->Xbddamerit;

   //CString sMensaje, sMensajeAux;
   //sMensaje= "Labelling error: Don't know how to work out whether X-vertex ";
   //sMensajeAux.Format("%d", Nvert);
   //sMensaje+=sMensajeAux;
   //sMensaje+="occludes face ";
   //sMensajeAux.Format("%d", Nface);
   //sMensaje+=sMensajeAux;
   //AfxMessageBox(sMensaje);
   return 0.382;  // guess

   //printf("(%d)Faces %d,%d,%d,%d\n",
   //       Object->Labelling.VECount[Nvert],
   //       Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][0]],
   //       Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][1]],
   //       Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][2]],
   //       Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][3]]);
   //exit(999);
   //return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PX5VertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PX5VertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PX5VertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   return Object->Vprobabilities[Nvert][Xcccccmerit];
}


MERIT CCueLabelsVarley_Relaxation::PX5VertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   return Object->Vprobabilities[Nvert][Xdddddmerit];
}



MERIT CCueLabelsVarley_Relaxation::PX5VertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface)
{
   // X5 vertices never occlude anything except the boundary
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PX6VertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PX6VertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PX6VertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *XP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return XP[Xccccccmerit] + XP[Xcdcdcdmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return XP[Xccccccmerit] + XP[Xdcdcdcmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return XP[Xccccccmerit] + XP[Xcdcdcdmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return XP[Xccccccmerit] + XP[Xdcdcdcmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][4])
   {
      return XP[Xccccccmerit] + XP[Xcdcdcdmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][5])
   {
      return XP[Xccccccmerit] + XP[Xdcdcdcmerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PX6VertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   MERIT *XP = Object->Vprobabilities[Nvert];
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return XP[Xdcdcdcmerit] + XP[Xddddddmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return XP[Xcdcdcdmerit] + XP[Xddddddmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return XP[Xdcdcdcmerit] + XP[Xddddddmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][3])
   {
      return XP[Xcdcdcdmerit] + XP[Xddddddmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][4])
   {
      return XP[Xdcdcdcmerit] + XP[Xddddddmerit];
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][5])
   {
      return XP[Xcdcdcdmerit] + XP[Xddddddmerit];
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PX6VertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface)
{
   //6-way vertices never occlude anything
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PYVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return YP->Yabdmerit + YP->Yaabmerit + YP->Yabamerit + YP->Yabbmerit + YP->Yabcmerit + YP->Yaccmerit + YP->Yacdmerit + YP->Yaddmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return YP->Ydabmerit + YP->Yaabmerit + YP->Ybaamerit + YP->Ybabmerit + YP->Ycabmerit + YP->Ycacmerit + YP->Ydacmerit + YP->Ydadmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return YP->Ybdamerit + YP->Yabamerit + YP->Ybaamerit + YP->Ybbamerit + YP->Ybcamerit + YP->Yccamerit + YP->Ycdamerit + YP->Yddamerit;
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PYVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      return YP->Ybdamerit + YP->Ybaamerit + YP->Ybbamerit + YP->Ybabmerit + YP->Ybcamerit + YP->Ybccmerit + YP->Ybdcmerit + YP->Ybddmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      return YP->Yabdmerit + YP->Yabamerit + YP->Yabbmerit + YP->Ybbamerit + YP->Yabcmerit + YP->Ycbcmerit + YP->Ycbdmerit + YP->Ydbdmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      return YP->Ydabmerit + YP->Yaabmerit + YP->Yabbmerit + YP->Ybabmerit + YP->Ycabmerit + YP->Yccbmerit + YP->Ydcbmerit + YP->Yddbmerit;
   }
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PYVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   MERIT P = 0.0;

   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      P = YP->Ycccmerit + YP->Ycabmerit + YP->Ycacmerit + YP->Yccamerit + YP->Ycdamerit + YP->Ycbcmerit + YP->Yccbmerit + YP->Ycbdmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      P = YP->Ycccmerit + YP->Ybcamerit + YP->Yaccmerit + YP->Yccamerit + YP->Yacdmerit + YP->Ybccmerit + YP->Yccbmerit + YP->Ydcbmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      P = YP->Ycccmerit + YP->Yabcmerit + YP->Yaccmerit + YP->Ycacmerit + YP->Ydacmerit + YP->Ybccmerit + YP->Ycbcmerit + YP->Ybdcmerit;
   }

   //printf("PY Vertex %d is C at edge %d is %f\n", Nvert, Nedge, P);
   return P;
}


MERIT CCueLabelsVarley_Relaxation::PYVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   MERIT P = 0.0;

   if (Nedge == Object->Labelling.VENumber[Nvert][0])
   {
      P = YP->Ydddmerit + YP->Ydabmerit + YP->Ydacmerit + YP->Yddamerit + YP->Ydadmerit + YP->Ydcbmerit + YP->Ydbdmerit + YP->Yddbmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][1])
   {
      P = YP->Ydddmerit + YP->Ybdamerit + YP->Ycdamerit + YP->Yaddmerit + YP->Yddamerit + YP->Ybdcmerit + YP->Ybddmerit + YP->Yddbmerit;
   }
   if (Nedge == Object->Labelling.VENumber[Nvert][2])
   {
      P = YP->Ydddmerit + YP->Yabdmerit + YP->Yacdmerit + YP->Yaddmerit + YP->Ydadmerit + YP->Ycbdmerit + YP->Ybddmerit + YP->Ydbdmerit;
   }

   //printf("PY Vertex %d is D at edge %d is %f\n", Nvert, Nedge, P);
   return P;
}


MERIT CCueLabelsVarley_Relaxation::PYVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface)
{
   YPROBABILITIES *YP = (YPROBABILITIES *)(&Object->Vprobabilities[Nvert]);
   char FR2;

   char FR0 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][0]])
           || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][0]]);

   char FR1 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][1]])
           || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][1]]);

   if (FR0 && FR1)  return YP->Yabamerit + YP->Yabbmerit + YP->Yabcmerit + YP->Yabdmerit;

   FR2 = (Nface == Object->Labelling.EdgeForwardFace[Object->Labelling.VENumber[Nvert][2]])
      || (Nface == Object->Labelling.EdgeReverseFace[Object->Labelling.VENumber[Nvert][2]]);

   if (FR1 && FR2)  return YP->Yaabmerit + YP->Ybabmerit + YP->Ycabmerit + YP->Ydabmerit;

   if (FR2 && FR0)  return YP->Ybaamerit + YP->Ybbamerit + YP->Ybcamerit + YP->Ybdamerit;

   //CString sMensaje, sMensajeAux;
   //sMensaje= "Labelling error: Don't know how to work out whether y-vertex ";
   //sMensajeAux.Format("%d", Nvert);
   //sMensaje+=sMensajeAux;
   //sMensaje+="occludes face ";
   //sMensajeAux.Format("%d", Nface);
   //sMensaje+=sMensajeAux;
   //AfxMessageBox(sMensaje);

   return 0.382;  // guess
   //exit(999);
   //return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PZVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   return (Nedge == Object->Labelling.VENumber[Nvert][3]) ? 1.0 : 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PZVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   return (Nedge == Object->Labelling.VENumber[Nvert][1]) ? 1.0 : 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PZVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   return (Nedge == Object->Labelling.VENumber[Nvert][0]) ? 1.0 : 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PZVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   return (Nedge == Object->Labelling.VENumber[Nvert][2]) ? 1.0 : 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   switch (Object->Labelling.VertexLabel[Nvert].Shape)
   {
      case JunctionIsK:    return PKVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsL:    return PLVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsM:    return PMVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsT4:   return PTVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsW:    return PWVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsX:    return PXVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIs5way: return PX5VertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsZ6:
      case JunctionIs6way: return PX6VertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIs7way: return 0.0;
      case JunctionIs8way: return 0.0;
      case JunctionIsY:    return PYVertexIsAatEdge(Object,Nvert,Nedge);
      case JunctionIsZ:    return PZVertexIsAatEdge(Object,Nvert,Nedge);

        // Not in the catalogue, so make no inferences for these
      case JunctionIsKplus:  return 1.0;
   }
   //CString sMensaje, sMensajeAux;
   //sMensaje= "Labelling error: Don't know how to work out Pvertex-a-at-edge, shape ";
   //sMensajeAux.Format("%c", Object->Labelling.VertexLabel[Nvert].Shape);
   //sMensaje+=sMensajeAux;
   //AfxMessageBox(sMensaje);
   //exit(999);
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   switch (Object->Labelling.VertexLabel[Nvert].Shape)
   {
      case JunctionIsK:    return PKVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsL:    return PLVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsM:    return PMVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsT4:   return PTVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsW:    return PWVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsX:    return PXVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIs5way: return PX5VertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsZ6:
      case JunctionIs6way: return PX6VertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIs7way: return 0.0;
      case JunctionIs8way: return 0.0;
      case JunctionIsY:    return PYVertexIsBatEdge(Object,Nvert,Nedge);
      case JunctionIsZ:    return PZVertexIsBatEdge(Object,Nvert,Nedge);

        // Not in the catalogue, so make no inferences for these
      case JunctionIsKplus:  return 1.0;
   }
   //CString sMensaje, sMensajeAux;
   //sMensaje= "Labelling error: Don't know how to work out Pvertex-b-at-edge, shape ";
   //sMensajeAux.Format("%c", Object->Labelling.VertexLabel[Nvert].Shape);
   //sMensaje+=sMensajeAux;
   //AfxMessageBox(sMensaje);
   //exit(999);
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   switch (Object->Labelling.VertexLabel[Nvert].Shape)
   {
      case JunctionIsK:    return PKVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsL:    return PLVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsM:    return PMVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsT4:   return PTVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsW:    return PWVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsX:    return PXVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIs5way: return PX5VertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsZ6:
      case JunctionIs6way: return PX6VertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIs7way: return 0.618; // temporary fiddle
      case JunctionIs8way: return 0.618; // temporary fiddle
      case JunctionIsY:    return PYVertexIsCatEdge(Object,Nvert,Nedge);
      case JunctionIsZ:    return PZVertexIsCatEdge(Object,Nvert,Nedge);

        // Not in the catalogue, so make no inferences for these
      case JunctionIsKplus:  return 1.0;
   }
   //CString sMensaje, sMensajeAux;
   //sMensaje= "Labelling error: Don't know how to work out Pvertex-c-at-edge, shape ";
   //sMensajeAux.Format("%c", Object->Labelling.VertexLabel[Nvert].Shape);
   //sMensaje+=sMensajeAux;
   //AfxMessageBox(sMensaje);
   //exit(999);
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   switch (Object->Labelling.VertexLabel[Nvert].Shape)
   {
      case JunctionIsK:    return PKVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsL:    return PLVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsM:    return PMVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsT4:   return PTVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsW:    return PWVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsX:    return PXVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIs5way: return PX5VertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsZ6:
      case JunctionIs6way: return PX6VertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIs7way: return 0.382; // temporary fiddle
      case JunctionIs8way: return 0.382; // temporary fiddle
      case JunctionIsY:    return PYVertexIsDatEdge(Object,Nvert,Nedge);
      case JunctionIsZ:    return PZVertexIsDatEdge(Object,Nvert,Nedge);

        // Not in the catalogue, so make no inferences for these
      case JunctionIsKplus:  return 1.0;
   }
   //CString sMensaje, sMensajeAux;
   //sMensaje= "Labelling error: Don't know how to work out Pvertex-d-at-edge, shape ";
   //sMensajeAux.Format("%c", Object->Labelling.VertexLabel[Nvert].Shape);
   //sMensaje+=sMensajeAux;
   //AfxMessageBox(sMensaje);
   //exit(999);
   return 0.0;
}


MERIT CCueLabelsVarley_Relaxation::PVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface, EDGE_INDEX Nedge)
{
   extern FILE *DebugUFG;
   MERIT M = 1.0;

   // if it's the background, it must be occluded
   if (Nface >= 0)
   {
      switch (Object->Labelling.VertexLabel[Nvert].Shape)
      {
         case JunctionIsK:
            M = PKVertexOccludesFace(Object,Nvert,Nface);
         break;

         case JunctionIsL:
            M = PLVertexOccludesFace(Object,Nvert,Nface);
         break;

         case JunctionIsM:
            M = PMVertexOccludesFace(Object,Nvert,Nface);
         break;

         case JunctionIsT4:
            M = PTVertexOccludesFace(Object,Nvert,Nface,Nedge);
         break;

         case JunctionIsW:
            M = PWVertexOccludesFace(Object,Nvert,Nface);
         break;

         case JunctionIsX:
            M = PXVertexOccludesFace(Object,Nvert,Nface);
         break;

         case JunctionIs5way:
            M = PX5VertexOccludesFace(Object,Nvert,Nface);
         break;

         case JunctionIsZ6:
         case JunctionIs6way:
            M = PX6VertexOccludesFace(Object,Nvert,Nface);
         break;

         case JunctionIsY:
            M = PYVertexOccludesFace(Object,Nvert,Nface);
         break;

         case JunctionIsZ:
            M = PZVertexOccludesFace(Object,Nvert,Nface);
         break;
      }
   }
   //if (DebugUFG)  fprintf(DebugUFG,"P Vertex %d Occludes Face %d is %f\n",Nvert,Nface,M);
   return M;
}


void CCueLabelsVarley_Relaxation::UpdateProbabilitiesForEdge (OBJECT *Object, EDGE_INDEX Nedge)
{
   EPROBABILITIES *EP = &Object->Eprobabilities[Nedge];
   extern FILE *DebugUFG;

   VERTEX_INDEX SV = Object->Labelling.EdgeStartVertex[Nedge];
   VERTEX_INDEX EV = Object->Labelling.EdgeEndVertex[Nedge];
   FACE_INDEX FF = Object->Labelling.EdgeForwardFace[Nedge];
   FACE_INDEX RF = Object->Labelling.EdgeReverseFace[Nedge];

   if (UseMaxInRelaxationLabelling)
   {
      AdvancedRelax(&EP->Amerit,LC->tcSo,HVertexIsBatEdge(Object,SV,Nedge) * HVertexIsAatEdge(Object,EV,Nedge) * Pmul(PVertexOccludesFace(Object,SV,RF,Nedge),PVertexOccludesFace(Object,EV,RF,Nedge)));
      AdvancedRelax(&EP->Bmerit,LC->tcSo,HVertexIsAatEdge(Object,SV,Nedge) * HVertexIsBatEdge(Object,EV,Nedge) * Pmul(PVertexOccludesFace(Object,SV,FF,Nedge),PVertexOccludesFace(Object,EV,FF,Nedge)));
      AdvancedRelax(&EP->Cmerit,LC->tcSo,HVertexIsCatEdge(Object,SV,Nedge) * PVertexIsCatEdge(Object,EV,Nedge));
      AdvancedRelax(&EP->Dmerit,LC->tcSo,HVertexIsDatEdge(Object,SV,Nedge) * PVertexIsDatEdge(Object,EV,Nedge));
   }
   else
   {
      AdvancedRelax(&EP->Amerit,LC->tcSo,PVertexIsBatEdge(Object,SV,Nedge) * PVertexIsAatEdge(Object,EV,Nedge) * Pmul(PVertexOccludesFace(Object,SV,RF,Nedge),PVertexOccludesFace(Object,EV,RF,Nedge)));
      AdvancedRelax(&EP->Bmerit,LC->tcSo,PVertexIsAatEdge(Object,SV,Nedge) * PVertexIsBatEdge(Object,EV,Nedge) * Pmul(PVertexOccludesFace(Object,SV,FF,Nedge),PVertexOccludesFace(Object,EV,FF,Nedge)));
      AdvancedRelax(&EP->Cmerit,LC->tcSo,PVertexIsCatEdge(Object,SV,Nedge) * PVertexIsCatEdge(Object,EV,Nedge));
      AdvancedRelax(&EP->Dmerit,LC->tcSo,PVertexIsDatEdge(Object,SV,Nedge) * PVertexIsDatEdge(Object,EV,Nedge));
   }
   //if (DebugUFG)  PrintEdgeProbabilities(DebugUFG,Object);
   NormaliseEProbabilities(&Object->Eprobabilities[Nedge]);

   //if (DebugUFG)  PrintEdgeProbabilities(DebugUFG,Object);

   if (EP->Cmerit > 0.9999)
   {
      //printf("Label Edge %d as Convex\n", Nedge);

      if (!Object->Labelling.EdgeLabelIsFinalised[Nedge])
      {
	     Object->Labelling.EdgeLabel[Nedge] = LineIsConvex;
	     Object->Eprobabilities[Nedge].Amerit = 0.0;
	     Object->Eprobabilities[Nedge].Bmerit = 0.0;
	     Object->Eprobabilities[Nedge].Cmerit = 1.0;
	     Object->Eprobabilities[Nedge].Dmerit = 0.0;
	     Object->Labelling.EdgeLabelIsFinalised[Nedge] = cTRUE;
	     if (UseBinaryFrontalGeometry(false, cFALSE))
	     {
	        SetFaceAndEdgeCoplanar(Object,Object->Labelling.EdgeForwardFace[Nedge],Nedge);
	        SetFaceAndEdgeCoplanar(Object,Object->Labelling.EdgeReverseFace[Nedge],Nedge);
	     }
	  }
   }
   else
   if (EP->Dmerit > 0.9999)
   {
      //printf("Label Edge %d as Concave\n", Nedge);

      if (!Object->Labelling.EdgeLabelIsFinalised[Nedge])
      {
         Object->Labelling.EdgeLabel[Nedge] = LineIsConcave;
         Object->Eprobabilities[Nedge].Amerit = 0.0;
         Object->Eprobabilities[Nedge].Bmerit = 0.0;
         Object->Eprobabilities[Nedge].Cmerit = 0.0;
         Object->Eprobabilities[Nedge].Dmerit = 1.0;
         Object->Labelling.EdgeLabelIsFinalised[Nedge] = cTRUE;
         if (UseBinaryFrontalGeometry(false, cFALSE))
         {
            SetFaceAndEdgeCoplanar(Object,Object->Labelling.EdgeForwardFace[Nedge],Nedge);
            SetFaceAndEdgeCoplanar(Object,Object->Labelling.EdgeReverseFace[Nedge],Nedge);
         }
      }
   }
   else
   if (EP->Amerit > 0.9999)  LabelEdgeAsArrow(Object,Nedge);
   else
   if (EP->Bmerit > 0.9999)  LabelEdgeAsBackArrow(Object,Nedge);
}


lBOOLEAN CCueLabelsVarley_Relaxation::UpdateLabelProbabilities (OBJECT *Object)
{
   lBOOLEAN Update = cFALSE;

   //printf("=== Relaxation Iteration %d ===\n",Iterations);
   //fprintf(RelaxationDebugFile,"=== Relaxation Iteration %d ===\n",Iterations);

   //printf("=== Setting 4h K Presence Merit ===\n");
	LP->KJunctionPresenceMerit = LP->KJunctionsKnownToBePresent ? 1.0 : DetermineKJunctionPresenceMerit(Object);
   //printf("=== Setting 4h X Presence Merit ===\n");
	LP->XJunctionPresenceMerit = LP->XJunctionsKnownToBePresent ? 1.0 : DetermineXJunctionPresenceMerit(Object);

   // Update Probabilities For Vertices
   //printf("=== Update Probabilities For Vertices ===\n");
	for (VERTEX_INDEX n=0; n<Object->Labelling.VertexCount; ++n)  if (!Object->Labelling.VertexLabellingIsFinalised[n])
	{
	   Update = cTRUE;
	   UpdateProbabilitiesForVertex(Object,n);
	}

   //PrintVertexProbabilities(Labelling);

   // Update Probabilities For Edges
   //printf("=== Update Probabilities For Edges ===\n");
   for (EDGE_INDEX e=0; e<Object->Labelling.EdgeCount; ++e)  if (!Object->Labelling.EdgeLabelIsFinalised[e])
   {
      Update = cTRUE;
      UpdateProbabilitiesForEdge(Object,e);
   }

   return Update;
}


lBOOLEAN CCueLabelsVarley_Relaxation::AllEdgesAndVerticesLabelled (OBJECT *Object)
{
   int n;
   for (n=0; n<Object->Labelling.VertexCount; ++n)  if (!Object->Labelling.VertexLabellingIsFinalised[n])
   {
      //printf("Vertex %d has not been labelled unambiguously\n",n);
      return cFALSE;
   }
   for (n=0; n<Object->Labelling.EdgeCount; ++n)  if (!Object->Labelling.EdgeLabelIsFinalised[n])  return cFALSE;
   return cTRUE;
}


lBOOLEAN CCueLabelsVarley_Relaxation::LabelLinesUsingRelaxation (OBJECT *Object,
																 int RelaxationIterations,
																 lBOOLEAN eUseJunctionStatisticsForRelaxationLabelling,
																 lBOOLEAN eUseShapePairStatisticsForRelaxationLabelling,
																 lBOOLEAN eUseMaxInRelaxationLabelling)
{
	//Create local copies of external variables
	UseJunctionStatisticsForRelaxationLabelling= eUseJunctionStatisticsForRelaxationLabelling;
	UseShapePairStatisticsForRelaxationLabelling= eUseShapePairStatisticsForRelaxationLabelling;
	UseMaxInRelaxationLabelling= eUseMaxInRelaxationLabelling;


   int Iterations = (int)GetProgramOptionValue(Object,OPTION_USE_RELAXATION_LABELLING);
   if (Iterations) RelaxationIterations = Iterations;  /* otherwise use default value */
   UseJunctionStatisticsForRelaxationLabelling  = IsProgramOptionSpecified(Object,OPTION_USE_JUNCTION_STATISTICS);
   UseShapePairStatisticsForRelaxationLabelling = IsProgramOptionSpecified(Object,OPTION_USE_SHAPEPAIR_STATISTICS);
   UseMaxInRelaxationLabelling                  = IsProgramOptionSpecified(Object,OPTION_USE_MAX_IN_RELAXATION);

   //RelaxationDebugFile = fopen("C:\\Ribald\\Relaxation.txt","w");

   //PrintVertexProbabilities(Labelling);
   //PrintEdgeProbabilities(RelaxationDebugFile,Labelling);

   //printf("=== Label Lines using Relaxation ===\n");
   //fprintf(RelaxationDebugFile,"=== Label Lines using Relaxation ===\n");
   //PrintVerticesAndEdges(Object,Labelling);

   // fiddle to stop system hanging
   if (LC->tcSp > 0.95)  LC->tcSp = 0.95;

   for (Iterations=0; Iterations<RelaxationIterations; ++Iterations)
   {
      lBOOLEAN Update = UpdateLabelProbabilities(Object);
      if (!Update)  break;
   }

   if (AllEdgesAndVerticesLabelled(Object))
   {
      //printf("=== All Edges and Vertices labelled using Relaxation after %d iterations ===\n", Iterations+1);
      //fprintf(RelaxationDebugFile,"=== All Edges and Vertices labelled using Relaxation after %d iterations ===\n", Iterations+1);
      //fPrintVerticesAndEdges(RelaxationDebugFile,Object,Labelling);
      return cTRUE;
   }

   //printf("=== relaxation labelling incomplete, using propagation ===\n");
   //fprintf(RelaxationDebugFile,"=== relaxation labelling incomplete, using propagation ===\n");
   //fPrintVerticesAndEdges(RelaxationDebugFile,Object,Labelling);
   return cFALSE;
}