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



#ifndef _CUE_LABELS_VARLEY_LABELLINESCOMBINATORIALLY_DEFINED
	#define _CUE_LABELS_VARLEY_LABELLINESCOMBINATORIALLY_DEFINED

	class CCueLabelsVarley_Combinatorially{
		private:

			lBOOLEAN CountLabellings;
			lBOOLEAN GenerateLabellingAutoUpdate;
			lBOOLEAN AbandonOnLabellingFailure;

			CCueLabelsVarley_LabellingConstants *LC;
			CCueLabelsVarley_LabellingParameters *LP;

			int FavouredLabelling;

			void InitialiseLabellingBuffer ();
			void AddLabellingHypothesis (VERTEX_INDEX Vertex,  TYPEMARK Vexity,  MERIT Merit);
			//#define SORTROUTINE __cdecl
			//int SORTROUTINE LabellingOrder (const void *Labelling1, const void *Labelling2);
			void GenerateLabellingHypothesisTable (OBJECT *Object);

			lBOOLEAN AmbiguousType (TYPEMARK Mark);
			lBOOLEAN SketchLabellingIsAmbiguous (LABELLING *Labelling);
			MERIT SimplicityLabellingMerit (LABELLING *Labelling, int VertexCount);
			int SubgraphsWithOccludingBoundaries (OBJECT *Object);
			lBOOLEAN VertexLabelsIdentical (JUNCTION_TYPEMARK *A, JUNCTION_TYPEMARK *B);
			MERIT CofacialConfigurationLabellingMerit (LABELLING *Labelling, COFACIAL_CONFIGURATION *C);
			double EdgeLabellingMerit (LABELLING *Labelling, int EdgeCount);
			void GetLabellingMerit (OBJECT *Object, LABELLING *Labelling, int VertexCount, int EdgeCount);

			lBOOLEAN InterpretAmbiguousJunctionType (int VertexCount, LABELLING *One, LABELLING *Two);
			lBOOLEAN GenerateAlternativeInterpretations (LABELLING *One, LABELLING *Two);
			void InterpretAmbiguousEdgeType (TYPEMARK T, TYPEMARK *T1, TYPEMARK *T2);

			void StoreLabelling (LABELLING *Labelling);

			int ResolveAmbiguousLabellingsAndOutput (OBJECT *Object, LABELLING *Labelling, int Level, int MaxConsidered);

			//void GetProgramParameter (OBJECT *Object, char *Name);

			lBOOLEAN RetrieveFavouredLabelling (OBJECT *Object);


			lBOOLEAN SketchLabellingIsValid (LABELLING *Labelling);

		public:
			CCueLabelsVarley_Combinatorially(CCueLabelsVarley_LabellingConstants *eLC,
											 CCueLabelsVarley_LabellingParameters *eLP);
			~CCueLabelsVarley_Combinatorially();

			lBOOLEAN LabelLinesUsingKanatani (OBJECT *Object, LABELLING *Labelling);

			lBOOLEAN LabelLinesCombinatorially (OBJECT *Object);
	};

#endif  //_CUE_LABELS_VARLEY_LABELLINESCOMBINATORIALLY_DEFINED
