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

#ifndef _CUE_LABELS_VARLEY_DEFINED
	#define _CUE_LABELS_VARLEY_DEFINED

	//Finding labels in natural line-drawings
	//---------------------------------------
	class CCueLabelsVarley{
		private:
			//Manage constants and parameters
			//-------------------------------
			CCueLabelsVarley_LabellingConstants *LC;
			CCueLabelsVarley_LabellingParameters *LP;

			//Input object
			//------------
			OBJECT *Object;

			//Calculate derived data
			void DetermineLimits (OBJECT *Object);
			void CreateEdgeJoiningTable (OBJECT *Object);

			void Input_Varley(CDB_Model *DDBB);

			//Labelling
			//---------
			lBOOLEAN UnifiedFrontalGeometry;

			void SelectApproach(short int &LabellingProcedure,
								char &WhichLabellingCatalogue,
								int &RelaxationIterations,
								bool &UseJunctionStatisticsForRelaxationLabelling,
								bool &UseShapePairStatisticsForRelaxationLabelling,
								bool &UseMaxInRelaxationLabelling);

			void InitialiseLabellingBlackboard (OBJECT *Object,
												short int LabellingProcedure,
												int RelaxationIterations,
												bool UseJunctionStatisticsForRelaxationLabelling,
												bool UseShapePairStatisticsForRelaxationLabelling,
												bool UseMaxInRelaxationLabelling);

			void CreateLabellingSubset (LABELLING *Output, LABELLING *Input, char LabellingType);
			void AddLabellingToProbabilities (OBJECT *Object, LABELLING *Labelling);
			void AddCombinatorialResultsToRelaxation (OBJECT *Object);

			bool LabelSketch (OBJECT *Object,
							  short int LabellingProcedure,
							  int RelaxationIterations,
							  bool UseJunctionStatisticsForRelaxationLabelling,
							  bool UseShapePairStatisticsForRelaxationLabelling,
							  bool UseMaxInRelaxationLabelling);

			// Reverse some of the funnies which were needed to label non-trihedral sketches
			//------------------------------------------------------------------------------
			void ConvertT4JunctionToOccluding (OBJECT *Object, VERTEX_INDEX Nvert);
			void ReturnT4JunctionsToT3 (OBJECT *Object);

			#define Superset(xxBigValue,xxSmallValue) (((xxSmallValue) & (~(xxBigValue))) == 0)
			bool TLabellingsAreConflicting (int VertexCount, LABELLING *A, LABELLING *B);
			void RemoveConflictingTLabellings (OBJECT *Object);

			////bool NoVerticesRemainOnFace (FACE *Face);
			void IdentifyVerticesOccludingRegion (OBJECT *Object, FACE_INDEX Nface);
			void IdentifyEdgesOccludingRegion (OBJECT *Object, FACE_INDEX Nface);
			void IdentifyOcclusionInRegions (OBJECT *Object);

			//Output labelling
			//----------------
			void Output_Varley(CDB_Model *DDBB);

		public:
			CCueLabelsVarley();
			~CCueLabelsVarley();

			bool Get_Labels_Varley(CDB_Model *DDBB);
	};

#endif // _CUE_LABELS_VARLEY_DEFINED
