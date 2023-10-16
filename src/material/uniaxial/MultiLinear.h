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
                                                                        
#ifndef MultiLinear_h
#define MultiLinear_h

// Written: fmk 
// Created: 05/12
// Revision: A
//
// Description: This file contains the class definition for 
// MultiLinear. MultiLinear provides the abstraction
// of an elastic perfectly plastic uniaxial material, 
//
// What: "@(#) MultiLinear.h, revA"

#include "material/uniaxial/UniaxialMaterial.h"
#include "utility/matrix/Matrix.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Multilinear uniaxial material.
//!
//! This class provides the abstraction of an elastic
//! uniaxial material, i.e. the stress-strain relationship is given by the
//! linear equation \f$\sigma = E * \epsilon\f$.
class MultiLinear : public UniaxialMaterial
  {
  protected:
    Vector e0; // Initial backbone strains
    Vector s0; // Initial backbone stresses

    Matrix data;
    int numSlope; //!< number of intervals.
    int tSlope;

    double tStrain;     // current t strain
    double tStress;     // current t stress
    double tTangent;    // current t tangent
    double cStrain;     // last ced strain
    double cStress;     // last ced stress
    double cTangent;    // last cted  tangent
    
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);
    void setup_data(void);
    void setup(const Vector &, const Vector &);
  public:
    MultiLinear(int tag= 0);
    MultiLinear(int tag, const Vector &s, const Vector &e);
    UniaxialMaterial *getCopy(void) const;
    virtual ~MultiLinear();

    void setupPyList(const boost::python::list &points);
    int getNumPoints(void) const;
    
    int setTrialStrain(double strain, double strainRate = 0.0);
    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;

    double getInitialTangent(void) const
      {return data(0,4);}

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    UniaxialMaterial *getCopy(void);

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag =0) const;
    
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
  };
} // end of XC namespace

#endif
