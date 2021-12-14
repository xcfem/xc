// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
                                                                        
// $Revision: 1.10 $
// $Date: 2003/10/07 21:18:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/SolidMech4N.h,v $
                                                                        
// Written: MHS
// Created: Feb 2000
// Revised: Dec 2000 for efficiency
//
// Description: This file contains the class definition for SolidMech4N.

#ifndef SolidMech4N_h
#define SolidMech4N_h

#include "domain/mesh/element/plane/QuadBase4N.h"
#include "domain/mesh/element/utils/physical_properties/SolidMech2D.h"
#include "domain/mesh/element/utils/body_forces/BodyForces2D.h"
#include "domain/mesh/element/ElemWithMaterial.h"

namespace XC {
class NDMaterial;
class Material;
class Response;
class GaussPoint;

//! @ingroup PlaneElements
//
//! @brief Four node quadrilateral element for two-dimensional problems.
class SolidMech4N: public QuadBase4N<SolidMech2D>
  {
  protected:
    mutable Matrix Ki;
    
    int sendData(Communicator &);
    int recvData(const Communicator &);
    bool check_material_type(const std::string &type) const;
  public:
    SolidMech4N(int tag, int classTag, const SolidMech2D &);
    SolidMech4N(int tag, int classTag, int nd1, int nd2, int nd3, int nd4, const SolidMech2D &);

    void checkElem(void);
  };
} // end of XC namespace

#endif

