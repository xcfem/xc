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

// $Revision: 1.2 $
// $Date: 2009/11/03 23:12:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/SimpleBearingBase.h,v $

#ifndef SimpleBearingBase_h
#define SimpleBearingBase_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class definition for SimpleBearingBase.
// SimpleBearingBase is a friction slider element defined by two nodes. This
// simplified version uses small angle approximations and accounts for the
// rotation of the sliding surface by shifting the shear force.

#include "FrictionElementBase.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

namespace XC {
class FrictionModel;
class UniaxialMaterial;
class Response;

//! @ingroup FrictionElementGrp
//
//! @brief Bas class for flat slider bearing elements.
class SimpleBearingBase: public FrictionElementBase
  {
  protected:
    // parameters
    double k0; //!< initial stiffness of hysteretic component.
    double shearDistI; //!< shear distance from node I as fraction of length.
    int addRayleigh; //!< flag to add Rayleigh damping.
    bool onP0; //!< flag to indicate if the element is on P0.
    double kFactUplift; //!< stiffness factor when uplift is encountered.

    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // constructors
    SimpleBearingBase(int tag, int classTag, int Nd1, int Nd2, const size_t &dim,
		      const FrictionModels &, double kInit,
		      const std::vector<UniaxialMaterial *> &,
		      const Vector &y, const Vector &x,
		      const double &sdI, const int &addRay,
		      const double &mass,
		      const int &maxIter,
		      const double &tol,
		      const double &kFactUplift);
    SimpleBearingBase(int classTag, const size_t &dim);
    
    void setInitialStiffnessOfHystereticComponent(const double &);
    const double &getInitialStiffnessOfHystereticComponent(void) const;
    void setShearDistanceFromNodeIAsFractionOfLength(const double &);
    const double &getShearDistanceFromNodeIAsFractionOfLength(void) const;
    void setAddRayleighDampingFlag(const int &);
    int getAddRayleighDampingFlag(void) const;
    void setElementIsOnP0Flag(const bool &);
    bool getElementIsOnP0Flag(void) const;
    void setStiffnessFactorWhenUplift(const double &);
    const double &getStiffnessFactorWhenUplift(void) const;
   
    
    // public methods for element output
    void Print(std::ostream &s, int flag = 0) const;
  };
} // end of XC namespace

#endif
