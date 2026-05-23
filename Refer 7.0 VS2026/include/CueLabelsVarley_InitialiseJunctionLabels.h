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



#ifndef _CUE_LABELS_VARLEY_INITIALISEJUNCTIONLABELS_DEFINED
	#define _CUE_LABELS_VARLEY_INITIALISEJUNCTIONLABELS_DEFINED

	class CCueLabelsVarley_InitialiseJunctionLabels{
		private:
			void SetLJunctionToType (LABELLING *Labelling, VERTEX_INDEX Nvert, char LabellingType);
			void SetT4JunctionToType (LABELLING *Labelling, VERTEX_INDEX Nvert, char LabellingType);
			void SetWJunctionToType (LABELLING *Labelling, VERTEX_INDEX Nvert, char LabellingType);
			void SetXJunctionToType (LABELLING *Labelling, VERTEX_INDEX Nvert, char LabellingType);
			void SetYJunctionToType (LABELLING *Labelling, VERTEX_INDEX Nvert, char LabellingType);
			void Set6JunctionToType (LABELLING *Labelling, VERTEX_INDEX Nvert, char LabellingType);

		public:
			CCueLabelsVarley_InitialiseJunctionLabels();
			~CCueLabelsVarley_InitialiseJunctionLabels();

			void SetAllJunctionsToType (LABELLING *Labelling, char LabellingType);
			void InitialiseJunctionLabels (OBJECT *Object, char LabellingType);
	};

#endif  //_CUE_LABELS_VARLEY_INITIALISEJUNCTIONLABELS_DEFINED
