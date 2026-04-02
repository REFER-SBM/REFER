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

#ifndef _CUE_LABELS_VARLEY_LINETOJUNCTIONS_DEFINED
	#define _CUE_LABELS_VARLEY_LINETOJUNCTIONS_DEFINED

	class CCueLabelsVarley_LineToJunctions{
		private:
			void PropagateConstraintsFromConvexToJunction (LABELLING *Labelling, EDGE_INDEX EdgeNumber, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromConvex (LABELLING *Labelling, EDGE_INDEX Nedge);
			void PropagateConstraintsFromNonConvexToJunction (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromNonConvex (LABELLING *Labelling, EDGE_INDEX Nedge);

			void PropagateConstraintsFromConcaveToJunction (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromConcave (LABELLING *Labelling, EDGE_INDEX Nedge);
			void PropagateConstraintsFromNonConcaveToJunction (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromNonConcave (LABELLING *Labelling, EDGE_INDEX Nedge);

			void PropagateConstraintsFromVisibleToJunction (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Nvert);
			void PropagateConstraintsFromVisible (LABELLING *Labelling, EDGE_INDEX Nedge);

		public:
			CCueLabelsVarley_LineToJunctions ();
			~CCueLabelsVarley_LineToJunctions();
			void PropagateConstraintsFromArrow (LABELLING *Labelling, EDGE_INDEX Nedge);

			void PropagateLineConstraintsToJunctions (LABELLING *Labelling);
	};

#endif // _CUE_LABELS_VARLEY_LINETOJUNCTIONS_DEFINED
