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

#ifndef _CUE_LABELS_VARLEY_LABELLINES_DEFINED
	#define _CUE_LABELS_VARLEY_LABELLINES_DEFINED

		void LabelLineAsArrow (LABELLING *Labelling, EDGE_INDEX Nedge);
		void LabelLineAsBackArrow (LABELLING *Labelling, EDGE_INDEX Nedge);
		void LabelLineAsRelativeArrow (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Nvert);
		void LabelLineAsRelativeBackArrow (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Nvert);

		void LabelLineAsConvex (LABELLING *Labelling, EDGE_INDEX Nedge);

		void LabelLineAsConcave (LABELLING *Labelling, EDGE_INDEX Nedge);

#endif // _CUE_LABELS_VARLEY_LABELLINES_DEFINED
