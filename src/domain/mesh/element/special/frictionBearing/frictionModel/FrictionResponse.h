//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.1 $
// $Date: 2009/04/17 23:02:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/frictionModel/FrictionResponse.h,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the FrictionResponse class interface
//
// What: "@(#) FrictionResponse.h, revA"

#ifndef FrictionResponse_h
#define FrictionResponse_h

#include "utility/recorder/response/Response.h"


namespace XC {
class FrictionModel;
class ID;
class Vector;
class Matrix;
class BJtensor;

class FrictionResponse: public Response
  {
  private:
    FrictionModel *theFriction;
    int responseID;
  public:
    FrictionResponse(FrictionModel *frn, int id);
    FrictionResponse(FrictionModel *frn, int id, int val);
    FrictionResponse(FrictionModel *frn, int id, double val);
    FrictionResponse(FrictionModel *frn, int id, const ID &val);
    FrictionResponse(FrictionModel *frn, int id, const Vector &val);
    FrictionResponse(FrictionModel *frn, int id, const Matrix &val);
    FrictionResponse(FrictionModel *frn, int id, const BJtensor &val);
    
    int getResponse(void);  
  };
} // end of XC namespace

#endif
