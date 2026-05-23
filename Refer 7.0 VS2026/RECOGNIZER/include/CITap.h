/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CITap.h
+-----------------------------------------------------------------------------+
| Description: This class represents a Tap gesture.
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@ist.utl.pt
|
| Date: April 98, May 99
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
#ifndef _CITAP_H
#define _CITAP_H

#include "CIScribble.h"
#include "CICommand.h"

class CITap : public CICommand {

public: 
    CITap();
    CITap (CIScribble* sc, double dom, CIPoint point) { _sc = sc; _dom = dom; _point = point; _features = NULL; }

	void setUp(CIScribble *sc);
    CIGesture* clone() { return new CITap(_sc, _dom, _point); }
    char* getName() { return "Tap"; }

public:
	CIPoint _point;
};

#endif
