/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIArrow.cxx
+-----------------------------------------------------------------------------+
| Description: 
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@ist.utl.pt
|
| Date: May 98, May 99
+----------------------------------------------------------------------------
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

#include "CIArrow.h"
#include "CIUnknown.h"
#include <cstring>  // pcompany2009

CIArrow::CIArrow (bool rotated) : CIShape(rotated)
{
    _features = new CIFeatures (&CIEvaluate::Ns, 2, 2, BIG, BIG);
}

CIArrow::CIArrow (CIScribble* sc, CIPoint a, CIPoint b, double dom, bool dash, bool bold)
{
    _sc=sc;
    _points[0] = a; 
    _points[1] = b; 
    _dashed = dash; 
    _bold = bold;
    _open = false;
    _dom = dom;
    _features = NULL;
    _dashFeature = NULL;
}

/*----------------------------------------------------------------------------+
| Description: Computes the points of the recognized arrow
+----------------------------------------------------------------------------*/
void CIArrow::setUp(CIScribble* sc)
{
    _sc = sc;
}

/*----------------------------------------------------------------------------+
| Description: Makes a clone of the current arrow.
+----------------------------------------------------------------------------*/
CIGesture* CIArrow::clone()
{
    return new CIArrow(_sc, _points[0], _points[1], _dom, _dashed, _bold);
}


double CIArrow::evalGlobalFeatures(CIScribble* sc)
{
    _sc = sc;
    _dom = _features->evaluate(sc);
    _dashed = false;
    _bold = false;
    _open = false;
    return _dom;
}

/*----------------------------------------------------------------------------+
| Description: Evaluate the scribble based on local features. An arrow is
|              something with a Move or Triangle at the end.
| Input: a scribble and the list of possible gestures
| Output: dom
+----------------------------------------------------------------------------*/
double CIArrow::evalLocalFeatures(CIScribble *sc, CIList<CIGesture *>* _shapesList)
{
    double val;
    int i;
    int nshapes = _shapesList->getNumItems();
    CIGesture* lastShp;
    CIUnknown* firstShp;


    lastShp = NULL;
    val = 0;
    if (sc->getNumStrokes() > 1) {
        CIScribble *scribb = sc->clone();  // create a new scribble like the original
	    CIScribble *scLast = new CIScribble();
	    scLast->addStroke(scribb->popStroke());  // Last stroke of the scribble

	    for (i=0; i<nshapes; i++) {
            (*_shapesList)[i]->pushAttribs();
            if (val = (*_shapesList)[i]->evalGlobalFeatures(scLast)) {
 			    lastShp = (*_shapesList)[i]; 
                break;
            }
	    }

        if (lastShp != NULL) {
            firstShp = new CIUnknown();
            firstShp->setUp(scribb);
            char *name2 = lastShp->getName();

            if (!strcmp(name2, "Move") || !strcmp(name2, "Triangle")) {
                // verificar se o ultimo ponto do rabo da seta está perto da ponta
                CIList<CIPoint> *points;
                CIPoint pts[3], pc, sp, ep;
    
                points = scLast->largestTriangle()->getPoints();
                pts[0] = (*points)[0];
                pts[1] = (*points)[1];
                pts[2] = (*points)[2];
                pc.x = (pts[0].x + pts[1].x + pts[2].x) / 3;
                pc.y = (pts[0].y + pts[1].y + pts[2].y) / 3;

                // distances from the starting point and ending point 
                // to the center point
                double dxysp, dxyep, r;
                sp = scribb->startingPoint();
                ep = scribb->endingPoint();
                dxysp = fabs ((double)(sp.x - pc.x)) + fabs ((double)(sp.y - pc.y));  //pcompany2009
                dxyep = fabs ((double)(ep.x - pc.x)) + fabs ((double)(ep.y - pc.y));  //pcompany2009
                r = dxyep / dxysp;

                if (firstShp->isBold() || r <= 0.6 || r >= 1.7) {
                    setUp(scribb); 
		    _points[0] = sc->startingPoint();
		    _points[1] = pc;
                    _dashed = firstShp->isDashed();
                    _bold = firstShp->isBold();
                }
                else
                    val = 0;
            } 
            else
                val = 0;
            delete firstShp;
        }
	    for (i=0; i<nshapes; i++)
            (*_shapesList)[i]->popAttribs();
    }
    return val;
}
