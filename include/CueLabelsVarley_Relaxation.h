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

#ifndef _CUE_LABELS_VARLEY_RELAXATION_DEFINED
	#define _CUE_LABELS_VARLEY_RELAXATION_DEFINED

	class CCueLabelsVarley_Relaxation{
		private:
			lBOOLEAN UseJunctionStatisticsForRelaxationLabelling;
			lBOOLEAN UseShapePairStatisticsForRelaxationLabelling;
			lBOOLEAN UseMaxInRelaxationLabelling;

			CCueLabelsVarley_LabellingConstants *LC;
			CCueLabelsVarley_LabellingParameters *LP;

			void InitialiseSingleEdgeProbabilitiesUsingShapePairs (OBJECT *Object, EDGE_INDEX Nedge);
			void NormaliseEProbabilities (EPROBABILITIES *EP);
			void InitialiseEdgeProbabilitiesUsingShapePairs (OBJECT *Object);
			void InitialiseEdgeProbabilities (OBJECT *Object);

		public:
			void InitialiseRelaxationEdgeLabels (OBJECT *Object);
		private:

			void InitialiseRelaxationKJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			void InitialiseRelaxationLJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			void InitialiseRelaxationMJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			void InitialiseRelaxationTJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			void InitialiseRelaxationWJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			void InitialiseRelaxationXJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			void InitialiseRelaxationX5Junction (OBJECT *Object, VERTEX_INDEX Nvert);
			void InitialiseRelaxationX6Junction (OBJECT *Object, VERTEX_INDEX Nvert);
			void InitialiseRelaxationYJunction (OBJECT *Object, VERTEX_INDEX Nvert);

		public:
			void InitialiseRelaxationVertexLabels (OBJECT *Object);
		//private:

			void UpdateRelaxationProbabilitiesFromFeatures (OBJECT *Object);

		//public:
			void RenormaliseVertexProbabilities (OBJECT *Object);
			void RenormaliseEdgeProbabilities (OBJECT *Object);
		private:

			lBOOLEAN IsOptionSpecified[26][26];
			double OptionValue[26][26];

			double GetProgramOptionValue (OBJECT *Object, char *Option);
			lBOOLEAN IsProgramOptionSpecified (OBJECT *Object, char *Option);

			MERIT PLVertexIsK (OBJECT *Object, VERTEX_INDEX Nvert);
			MERIT PLVertexIsX (OBJECT *Object, VERTEX_INDEX Nvert);
			MERIT PTVertexIsK (OBJECT *Object, VERTEX_INDEX Nvert);
			MERIT PWVertexIsK (OBJECT *Object, VERTEX_INDEX Nvert);
			MERIT PWVertexIsX (OBJECT *Object, VERTEX_INDEX Nvert);
			MERIT PYVertexIsK (OBJECT *Object, VERTEX_INDEX Nvert);
			MERIT PYVertexIsX (OBJECT *Object, VERTEX_INDEX Nvert);
			#define PTVertexIsX(Object,Nvert)  (0.0)   // T vertices are never X-type
			MERIT DetermineKJunctionPresenceMerit (OBJECT *Object);
			MERIT DetermineXJunctionPresenceMerit (OBJECT *Object);


			MERIT PEdgeA (OBJECT *Object, VERTEX_INDEX Nvert, int Index);
			MERIT PEdgeB (OBJECT *Object, VERTEX_INDEX Nvert, int Index);
			MERIT PEdgeC (OBJECT *Object, VERTEX_INDEX Nvert, int Index);
			MERIT PEdgeD (OBJECT *Object, VERTEX_INDEX Nvert, int Index);
			void AdvancedRelax (MERIT *Variable, MERIT Slowness, MERIT NewValue);
			void ClearMerits (MERIT *Merits, int Count, int ExceptFor);
			void NormaliseMerits (MERIT *Merits, int Count);
			void CCueLabelsVarley_Relaxation::ClearT (MERIT *TP);

			void UpdateProbabilitiesForLJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			void UpdateProbabilitiesForMJunction (OBJECT *Object, VERTEX_INDEX Nvert);

			FUZZY IsOccludingTJunction[MAX_VERTICES];
			MERIT PIsOccludingTJunction[MAX_VERTICES];
			EDGE_INDEX Ttail[MAX_VERTICES];
			void SetTJunctionOccluding (VERTEX_INDEX Nvert);
			void SetTJunctionNonOccluding (VERTEX_INDEX Nvert);
			void UpdateProbabilitiesForTJunction (OBJECT *Object, VERTEX_INDEX Nvert);

			void UpdateProbabilitiesForKJunction (OBJECT *Object, VERTEX_INDEX Nvert);

			void NormaliseWProbabilities (void *P);
			void ClearW (WPROBABILITIES *WP);
			void LabelVertexAsWbca (OBJECT *Object, VERTEX_INDEX Nvert);
			void LabelVertexAsWcdc (OBJECT *Object, VERTEX_INDEX Nvert);
			void LabelVertexAsWdcd (OBJECT *Object, VERTEX_INDEX Nvert);
			void UpdateProbabilitiesForWJunction (OBJECT *Object, VERTEX_INDEX Nvert);

			void NormaliseXProbabilities (void *P);
			void ClearX (XPROBABILITIES *XP);

			void UpdateProbabilitiesForXJunction (OBJECT *Object, VERTEX_INDEX Nvert);
			void UpdateProbabilitiesForX5Junction (OBJECT *Object, VERTEX_INDEX Nvert);
			void UpdateProbabilitiesForX6Junction (OBJECT *Object, VERTEX_INDEX Nvert);

			void NormaliseYProbabilities (void *P);
			void ClearY (YPROBABILITIES *YP);
			void LabelVertexAsYccc (OBJECT *Object, VERTEX_INDEX Nvert);
			void LabelVertexAsYddd (OBJECT *Object, VERTEX_INDEX Nvert);
			void UpdateProbabilitiesForYJunction (OBJECT *Object, VERTEX_INDEX Nvert);

			void UpdateProbabilitiesForVertex (OBJECT *Object, VERTEX_INDEX Nvert);


			MERIT HKVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HKVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HKVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HKVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);

			MERIT HLVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HLVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HLVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HLVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);

			MERIT HMVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HMVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HMVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HMVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);

			MERIT HTVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HTVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HTVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HTVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);

			MERIT HWVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HWVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HWVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HWVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);

			MERIT HXVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HXVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HXVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HXVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);

			MERIT HYVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HYVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HYVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HYVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);

			MERIT HVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT HVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);


			MERIT PKVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PKVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PKVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PKVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PKVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface);

			MERIT PLVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PLVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PLVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PLVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PLVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface);

			MERIT PMVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PMVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PMVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PMVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PMVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface);

			MERIT PTVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PTVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PTVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PTVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PTVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface, EDGE_INDEX Nedge);

			MERIT PWVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PWVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PWVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PWVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PWVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface);

			MERIT PXVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PXVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PXVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PXVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PXVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface);

			MERIT PX5VertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PX5VertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PX5VertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PX5VertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PX5VertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface);

			MERIT PX6VertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PX6VertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PX6VertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PX6VertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PX6VertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface);

			MERIT PYVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PYVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PYVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PYVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PYVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface);

			MERIT PZVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PZVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PZVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PZVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			#define PZVertexOccludesFace(Object,Nvert,Nface)  (0.0)  // Z-vertices never occlude faces

			MERIT PVertexIsAatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PVertexIsBatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PVertexIsCatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PVertexIsDatEdge (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge);
			MERIT PVertexOccludesFace (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface, EDGE_INDEX Nedge);

			void UpdateProbabilitiesForEdge (OBJECT *Object, EDGE_INDEX Nedge);


			lBOOLEAN UpdateLabelProbabilities (OBJECT *Object);

			lBOOLEAN AllEdgesAndVerticesLabelled (OBJECT *Object);

		public:
			CCueLabelsVarley_Relaxation(CCueLabelsVarley_LabellingConstants *LC,
										CCueLabelsVarley_LabellingParameters *LP);
			~CCueLabelsVarley_Relaxation();

			lBOOLEAN LabelLinesUsingRelaxation (OBJECT *Object,
												int RelaxationIterations,
												lBOOLEAN eUseJunctionStatisticsForRelaxationLabelling,
												lBOOLEAN eUseShapePairStatisticsForRelaxationLabelling,
												lBOOLEAN eUseMaxInRelaxationLabelling);

	};

#endif  //_CUE_LABELS_VARLEY_RELAXATION_DEFINED
