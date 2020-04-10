// -*-c++-*-
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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/response/Response.h,v $
                                                                        
// Written: MHS 
// Created: Oct 2000
//
// Description: This file contains the Response class interface

#ifndef Response_h
#define Response_h

#include "domain/mesh/element/utils/Information.h"
#include <fstream>

namespace XC {
class ID;
class Vector;
class Matrix;
class BJtensor;


//! @ingroup Recorder
//!
//! @defgroup ResponseGrp Domain objects response.
//
//! @ingroup ResponseGrp
//
//! @brief Base class response objects.
//!
//! Response objects are used to inquiry responses (displacements,
//! internal forces, stresses, strains,...) from the objects
//! of the domain.
class Response
  {
  protected:
    Information myInfo;
  public:
    Response(void);
    Response(int val);
    Response(double val);
    Response(const ID &val);
    Response(const Vector &val);
    Response(const Matrix &val);
    Response(const BJtensor &val);
    inline virtual ~Response(void) {}
  
    virtual int getResponse(void) = 0;
    virtual Information &getInformation(void);

    virtual void Print(std::ofstream &s, int flag = 0) const;
  };
} // end of XC namespace


#endif
