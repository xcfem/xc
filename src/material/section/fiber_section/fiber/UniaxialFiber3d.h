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
                                                                        
// $Revision: 1.7 $
// $Date: 2003/02/25 23:33:36 $
// $Source: material/section/fiber_section/fiber/UniaxialFiber3d.h,v $
                                                                        
                                                                        
// File: ~/fiber/UniaxialFiber3d.h
//
// Written: Remo Magalhaes de Souza
// Created: 10/98
// Revision: 
//
// Description: This file contains the class definition for 
// UniaxialFiber3d.h. 
//
// What: "@(#) UniaxialFiber3d.h, revA"

#ifndef UniaxialFiber3d_h
#define UniaxialFiber3d_h

#include "UniaxialFiber.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
class UniaxialMaterial;
class Response;

//! @ingroup MATSCCFibers
//
//! @brief Uniaxial fiber on a three-dimensional space.
//!
//! UniaxialFiber3d provides the abstraction of a
//! uniaxial fiber that forms a fiber section for 3d frame elements (the 
//! fiber position inside the section is defined by two coordinates)
//! The UniaxialFiber3d is subjected to a stress state with 
//! only one nonzero axial stress and corresponding axial strain.
class UniaxialFiber3d: public UniaxialFiber
  {
  private:
    double as[2]; //!< position of the fiber (Y has its sign changed).
    static Matrix ks; //! static class wide matrix object for returns
    static Vector fs; //! static class wide vector object for returns

    void set_position(const Vector &position);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    UniaxialFiber3d(int classTag= FIBER_TAG_Uniaxial3d);    
    UniaxialFiber3d(int tag,const UniaxialMaterial &theMat,const double &Area, const Vector &position,int classTag= FIBER_TAG_Uniaxial3d);
    UniaxialFiber3d(int tag,const MaterialHandler &,const std::string &nmbMat,const double &, const Vector &position,int classTag= FIBER_TAG_Uniaxial3d);

    int setTrialFiberStrain(const Vector &vs);
    Vector &getFiberStressResultants(void);
    Matrix &getFiberTangentStiffContr(void); 

    Fiber *getCopy(void) const;
    int getOrder(void) const;
    const ResponseId &getType(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;
    
    Response *setResponse(const std::vector<std::string> &argv, Information &info);
    int getResponse(int responseID, Information &info);

    void getFiberLocation(double &y, double &z) const;
    inline virtual double getLocY(void) const
      { return -as[0]; } //XXX sign of y 
    inline virtual double getLocZ(void) const
      { return as[1]; }
  };
} // end of XC namespace


#endif

