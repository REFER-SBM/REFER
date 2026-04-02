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

#ifndef _CUE_LABELS_VARLEY_JUNCTIONTOLINES_DEFINED
	#define _CUE_LABELS_VARLEY_JUNCTIONTOLINES_DEFINED

	class CCueLabelsVarley_JunctionToLines{
		private:
			CCueLabelsVarley_LabellingConstants *LC;

			void PropagateConstraintsFromWJunction (LABELLING *Labelling, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromLJunction (LABELLING *Labelling, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromYJunction (LABELLING *Labelling, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromTJunction (LABELLING *Labelling, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromT4Junction (OBJECT *Object, LABELLING *Labelling, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromZ4Junction (LABELLING *Labelling, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromXJunction (LABELLING *Labelling, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromMJunction (LABELLING *Labelling, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromKJunction (LABELLING *Labelling, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromXbcccaJunction (LABELLING *Labelling, VERTEX_INDEX Nvert);
			void PropagateConstraintsFrom5Junction (LABELLING *Labelling, VERTEX_INDEX Nvert);
			void PropagateConstraintsFrom6Junction (LABELLING *Labelling, VERTEX_INDEX Nvert);

		public:
			CCueLabelsVarley_JunctionToLines (CCueLabelsVarley_LabellingConstants *eLC);
			~CCueLabelsVarley_JunctionToLines();

			void PropagateJunctionConstraintsToLines (OBJECT *Object, LABELLING *Labelling);
	};

#endif // _CUE_LABELS_VARLEY_JUNCTIONTOLINES_DEFINED
