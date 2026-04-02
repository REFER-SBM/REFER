/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIRecognizer.cxx
+-----------------------------------------------------------------------------+
| Description: Implements the class CIRecognizer
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@ist.utl.pt
|
| Date: Oct 98, April 99
+-----------------------------------------------------------------------------+
|
| Copyright (C) 1998, 1999, 2000 Manuel João da Fonseca
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of the GNU General Public License
| as published by the Free Software Foundation; either version 2
| of the License, or any later version.
| 
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
| 
| You should have received a copy of the GNU General Public License
| along with this program; if not, write to the Free Software
| Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
| 
+----------------------------------------------------------------------------*/

#include "CIRecognizer.h"
#include <cstring>   // pcompany2009

CIRecognizer::CIRecognizer (bool rotated, double alfaCut)
{
    _alfaCut = alfaCut;

    _shapesList = new CIList<CIGesture *> ();
    _unknownShape = new CIUnknown();
	_tap = new CITap();

// Gestures (Shapes and Commands) identified by the recognizer. 
// The next lines create a list with all gestures identified by the recognizer
// If you want to add a new gesture to the recognizer, just add it to the list.

// Commands ----------
    _shapesList->insertTail(new CIDelete());
    _shapesList->insertTail(new CIWavyLine());
    _shapesList->insertTail(new CICopy());
    _shapesList->insertTail(new CIMove());
    //_shapesList->insertTail(new CITap());
    _shapesList->insertTail(new CICross());

// Shapes ------------
    _shapesList->insertTail(new CILine(rotated));
    _shapesList->insertTail(new CITriangle(rotated));
    _shapesList->insertTail(new CIRectangle(rotated));
    _shapesList->insertTail(new CICircle(rotated));
    _shapesList->insertTail(new CIEllipse(rotated));
    _shapesList->insertTail(new CIDiamond(rotated));
    _shapesList->insertTail(new CIArrow(rotated));
}

/*----------------------------------------------------------------------------+
| Description: Identifies shapes based on a scribble. It starts by looking
|              for global geometric features and then for local features
| Input: sc - A scribble
| Output: A list of plausible shapes ordered by degree of certainty.
|
| Notes: If the application wants to manipulate the gestures returned as new 
|        entities, it must clone them, because the gestures return by the
|        recognizer are always the same. (The gestures returned are the ones
|        created in the recognizer constructor)
+----------------------------------------------------------------------------*/
CIList<CIGesture *> *CIRecognizer::recognize(CIScribble *sc)
{
    double val, val2;
    int i;
    int nshapes = _shapesList->getNumItems();
    CIList<CIGesture *>* _shapes2Return;
    _shapes2Return = new CIList<CIGesture *> ();

    for (i=0; i<nshapes; i++) {     // set doms of all gestures to zero
        (*_shapesList)[i]->resetDom();
    }

	if (sc->getLen() < 10) {
		_tap->setUp(sc);
		_shapes2Return->insertInOrder(_tap, 1 - 0);
	} 
	else {

/*
    if (sc->getNumPoints() == 1) { // This piece of code is used to 
        CIStroke *strk;            // avoid scribbles of just one point
        CIList<CIPoint *> *pts;

        strk = (*sc->getStrokes())[0];
        pts = strk->getPoints();
        strk->addPoint((*pts)[0]->x,(*pts)[0]->y);
    }
*/

    for (i=0; i<nshapes; i++) {
        char* nam = (*_shapesList)[i]->getName(); // Para apagar
        val = (*_shapesList)[i]->evalGlobalFeatures(sc);
        if (val > _alfaCut) {
            val2 = (*_shapesList)[i]->evalLocalFeatures(sc, _shapesList);
            if (val2 < val)
                val = val2;
            if (val > _alfaCut) {
                nam = (*_shapesList)[i]->getName(); // Para apagar
                _shapes2Return->insertInOrder((*_shapesList)[i], 1 - val); 
                // (1-val) is used because the method insertInOrder creates an
                // ascendant list, and we want a descendant one.
            }
        }
    }
	}

    if (_shapes2Return->getNumItems() == 0) {
		    _unknownShape->setUp(sc);
		    _shapes2Return->insertInOrder(_unknownShape, 1 - 0);
    }

    return _shapes2Return;
}

