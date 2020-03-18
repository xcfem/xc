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
// $Date: 2003/02/25 23:33:36 $
// $Source: material/section/fiber_section/fiber/UniaxialFiber2d.h,v $
                                                                        
                                                                        
// File: ~/fiber/UniaxialFiber2d.h
//
// Written: Remo Magalhaes de Souza
// Created: 10/98
// Revision: 
//
// Description: This file contains the class definition for 
// UniaxialFiber2d.h.
//
// What: "@(#) UniaxialFiber2d.h, revA"

#ifndef UniaxialFiber2d_h
#define UniaxialFiber2d_h

#include "UniaxialFiber.h"

namespace XC {
class UniaxialMaterial;

//! @ingroup MATSCCFibers
//
//! @brief Uniaxial fiber on a bi-dimensional space.
//!
//! UniaxialFiber2d provides the abstraction of a
//! uniaxial fiber whose position  is defined with only one coordinate.
//! The UniaxialFiber2d is subjected to a stress state with 
//! only one nonzero axial stress and corresponding axial strain.
class UniaxialFiber2d: public UniaxialFiber
  {
  private:
    double y; //!< Position of the fiber
              //(its sign is changed -see comments along the file-). 

    static Matrix ks; //!< static class wide matrix object for returns
    static Vector fs; //!< static class wide vector object for returns
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    UniaxialFiber2d(void);   
    UniaxialFiber2d(int tag, UniaxialMaterial &, const double &,const double &);
    UniaxialFiber2d(int tag,const MaterialHandler &,const std::string &nmbMat,const double &, const double &);
    
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
    virtual double getLocY(void) const
      { return -y; } //Sign of Y coordinate is changed.
  };
} // end of XC namespace


#endif






