/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CICross.cxx
+-----------------------------------------------------------------------------+
| Description: 
| 
| Notes:       
|
| $Log$
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@ist.utl.pt
|
| Date: Oct 99
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

#include "CICross.h"
#include "CIVector.h"
#include "CIFunction.h"
#include <cstring>  // pcompany2009

CICross::CICross ()
{
    _features = new CIFeatures (&CIEvaluate::Alq_Ach, 0.85, 0.95, 1, 1
                                ,&CIEvaluate::Plq_Pch, 0.98, 0.99, 1, 1
                                ,&CIEvaluate::Ns, 2, 2, 2, 2
                                );
}

/*----------------------------------------------------------------------------+
| Description: This method analyse the scribble taking into account local 
|              features of a cross. A cross is built up of two lines that
|              intersect each other. It starts by checking if the two strokes
|              are lines and after if they intersect.
| Input: A scribble and list of all possible gestures.
| Output: degree of membership
+----------------------------------------------------------------------------*/
double CICross::evalLocalFeatures(CIScribble *sc, CIList<CIGesture *>* _shapesList)
{
    double val1, val2;
    int i;
    int nshapes = _shapesList->getNumItems();
    CIGesture* gest = NULL;
    char *name;

    val1 = val2 = 0;
    CIScribble *scribb = sc->clone();  // create a new scribble like the original
	CIScribble *scLast = new CIScribble();
	scLast->addStroke(scribb->popStroke());  // Last stroke of the scribble

	for (i=0; i<nshapes; i++) {     // recognize second stroke
        (*_shapesList)[i]->pushAttribs();
        if (val1 = (*_shapesList)[i]->evalGlobalFeatures(scLast)) {
 			gest = (*_shapesList)[i]; 
            break;
        }
	}
	for (i=0; i<nshapes; i++)
        (*_shapesList)[i]->popAttribs();

    if (gest != NULL) {
        name = gest->getName();
        if (!strcmp(name, "Line")) {
            for (i=0; i<nshapes; i++) {     // recognize first stroke
                (*_shapesList)[i]->pushAttribs();
                if (val2 = (*_shapesList)[i]->evalGlobalFeatures(scribb)) {
 			        gest = (*_shapesList)[i]; 
                    break;
                }
	        }
	        for (i=0; i<nshapes; i++)
                (*_shapesList)[i]->popAttribs();

            if (gest != NULL) {
                name = gest->getName();
                if (!strcmp(name, "Line"))
                    val2 = intersect (sc);
                else
                    val2 = 0;
            }
        } 
        else
            val1 = 0;
    }
    delete scLast;
    delete scribb;

    if (val2 < val1)
        val1 = val2;
    return val1;
}


/*----------------------------------------------------------------------------+
| Description: checks if the two lines of the scribble intersects
| Input: a scribble with two strokes
| Output: true if they intersect
+----------------------------------------------------------------------------*/
bool CICross::intersect(CIScribble *sc)
{
    CIList<CIStroke *> *strks;
    CIStroke *strk1, *strk2;
    int np1, np2;
    CIList<CIPoint *> *pts1, *pts2;

    strks = sc->getStrokes();
    strk1 = (*strks)[0];
    strk2 = (*strks)[1];
    np1 = strk1->getNumPoints();
    np2 = strk2->getNumPoints();
    pts1 = strk1->getPoints();
    pts2 = strk2->getPoints();

    CIVector *v1 = new CIVector ((*pts1)[0]->x, (*pts1)[0]->y, (*pts1)[np1-1]->x, (*pts1)[np1-1]->y);
    CIVector *v2 = new CIVector ((*pts2)[0]->x, (*pts2)[0]->y, (*pts2)[np2-1]->x, (*pts2)[np2-1]->y);

    bool left1, left2;
    left1 = CIFunction::left(v1->startp, v1->endp, v2->startp) ^ CIFunction::left(v1->startp, v1->endp, v2->endp);
    left2 = CIFunction::left(v2->startp, v2->endp, v1->startp) ^ CIFunction::left(v2->startp, v2->endp, v1->endp);

    return left1 && left2;
}

