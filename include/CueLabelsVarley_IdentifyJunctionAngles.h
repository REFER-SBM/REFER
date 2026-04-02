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

#ifndef _CUE_LABELS_VARLEY_IDENTIFYJUNCTIONANGLES_DEFINED
	#define _CUE_LABELS_VARLEY_IDENTIFYJUNCTIONANGLES_DEFINED

	class CCueLabelsVarley_IdentifyJunctionAngles{
		private:
			CCueLabelsVarley_LabellingParameters *LP;

			int CountOfEdgesMeetingVertex (OBJECT *Object, VERTEX_INDEX Nvert);
			lBOOLEAN EdgesShareVertex (OBJECT *Object, EDGE_INDEX E0, EDGE_INDEX E1);

			lREAL DistanceFromPoint2ToPoint2 (POINT2Dv *FromPoint, POINT2Dv *ToPoint);

			lBOOLEAN ThereIsAUseableIntercept (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, EDGE_INDEX G, POINT2Dv *Pcross, VERTEX_INDEX *CrossV);
			lBOOLEAN ThereIsAUseableJunction (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, EDGE_INDEX G, POINT2Dv *Pcross, VERTEX_INDEX *CrossV);
			lBOOLEAN Intercept (IMPLICIT_LINE2 *FirstLine, IMPLICIT_LINE2 *SecondLine, POINT2Dv *P);
			lBOOLEAN TwoLinesCrossAtPoint (OBJECT *Object, EDGE_INDEX Line0, EDGE_INDEX Line1, POINT2Dv *P);
			double DistanceFromJunctionToPoint2 (OBJECT *Object, VERTEX_INDEX From, POINT2Dv *To);
			lBOOLEAN OneEdgeInterceptsTheOther (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F);
			void CreateCrossVertexFromIntercept (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, VERTEX_INDEX CrossV);
			void CreateTripleCrossVertexFromIntercept (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, EDGE_INDEX G, VERTEX_INDEX CrossV);
			void CreateCrossVertexFromJunction (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, VERTEX_INDEX CrossV);
			void CreateTripleCrossVertexFromJunction (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, EDGE_INDEX G, VERTEX_INDEX CrossV);
			void CreateCrossVertexFromNew (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, POINT2Dv *Pcross);
			void CreateXJunctionAtPointOfIntersection (OBJECT *Object, EDGE_INDEX Ei, EDGE_INDEX Ej, EDGE_INDEX Ek, POINT2Dv *Pij, POINT2Dv *Pik, POINT2Dv *Pjk);

			lBOOLEAN Turn2CrossingLinesIntoXJunction (OBJECT *Object);
			lBOOLEAN Turn3CrossingLinesIntoXJunction (OBJECT *Object);

			DEGREES DirectionFromPoint2ToPoint2 (POINT2Dv *FromPoint, POINT2Dv *ToPoint);
			DEGREES OppositeDirection (DEGREES FromDirection);

			void ConvertT3JunctionToT4 (OBJECT *Object, VERTEX_INDEX Nvert);

			void SetLineDirection (OBJECT *Object, EDGE_INDEX Nedge, lBOOLEAN IsKnown, DEGREES Direction);
			DEGREES GetLineDirection (OBJECT *Object, EDGE_INDEX Nedge);
			DEGREES CalculateLineDirection (OBJECT *Object, LABELLING *Labelling, EDGE_INDEX Nedge);
			void CalculateAllLineDirections (OBJECT *Object);
			DEGREES DirectionAlongLineFromJunction (OBJECT *Object, EDGE_INDEX Nedge, VERTEX_INDEX Nvert);


			lBOOLEAN AllInOneHemisphere (OBJECT *Object, VERTEX_INDEX Nvert);
			void RearrangeEdgesAtTJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			void RearrangeEdgesAtLJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			lBOOLEAN RearrangeEdgesAtKJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			lBOOLEAN RearrangeEdgesAtXJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			lBOOLEAN RearrangeEdgesAtKorXJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			void RearrangeEdgesAtWJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			void RearrangeEdgesAtYJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			void RearrangeEdgesAt4Junction (OBJECT *Object, VERTEX_INDEX Nvert);
			void RearrangeEdgesAtMultiJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			lBOOLEAN RearrangeEdgesAtKMultiJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			lBOOLEAN IdentifyAnglesAtNormalJunction (OBJECT *Object, VERTEX_INDEX n);
			lBOOLEAN IdentifyAnglesAtInterceptJunction (OBJECT *Object, VERTEX_INDEX n);
			lBOOLEAN IdentifyAnglesAtJunction (OBJECT *Object, VERTEX_INDEX n);

		public:
			CCueLabelsVarley_IdentifyJunctionAngles(CCueLabelsVarley_LabellingParameters *LP);
			~CCueLabelsVarley_IdentifyJunctionAngles();

			lBOOLEAN IdentifyJunctionAngles (OBJECT *Object);
	};

#endif // _CUE_LABELS_VARLEY_IDENTIFYJUNCTIONANGLES_DEFINED
