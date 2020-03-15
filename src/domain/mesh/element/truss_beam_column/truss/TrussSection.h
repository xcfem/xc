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
                                                                        
// $Revision: 1.9 $
// $Date: 2003/03/17 19:33:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/truss/TrussSection.h,v $
                                                                        
                                                                        
#ifndef TrussSection_h
#define TrussSection_h

// File: ~/element/truss/TrussSection.h
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for Truss. A Truss 
// object provides the abstraction of the small deformation bar element. 
// Each truss object is associated with a section object. This Truss element 
// will work in 1d, 2d or 3d problems.
//
// What: "@(#) Truss.h, revA"

#include "TrussBase.h"
#include <utility/matrix/Matrix.h>

namespace XC {
class Channel;
class SectionForceDeformation;

//! @ingroup TrussElem
//
//! @brief Truss element with SectionForceDeformation material.
class TrussSection : public TrussBase
  {
  private:
    SectionForceDeformation  *theSection;
    double computeCurrentStrain(void) const;    
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    TrussSection(int tag, int dimension, int Nd1, int Nd2, SectionForceDeformation &theSection);
    TrussSection(int tag,int dimension,const Material *ptr_mat);    
    TrussSection(void);    
    Element *getCopy(void) const;
    ~TrussSection(void);

    // public methods to obtain information about dof & connectivity    
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);        
    int update(void);

    const Material *getMaterial(void) const;
    Material *getMaterial(void);
    virtual double getRho(void) const;

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;    
    
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
    
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter (int parameterID, Information &info);
  };
} // end of XC namespace

#endif
