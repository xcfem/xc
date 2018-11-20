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
                                                                        
// $Revision: 1.13 $
// $Date: 2003/03/17 19:19:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/truss/Spring.h,v $
                                                                        
                                                                        
#ifndef Spring_h
#define Spring_h

// File: ~/element/truss/Spring.h
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for Spring. A Spring object
// provides the abstraction of the small deformation bar element. Each truss
// object is associated with a material object. This Truss element will work
// in 1d, 2d or 3d problems.
//
// What: "@(#) Truss.h, revA"

#include "ProtoTruss.h"
#include <utility/matrix/Matrix.h>

namespace XC {
class Channel;
class UniaxialMaterial;

class Spring : public ProtoTruss
  {
  private:
    // private attributes - a copy for each object of the class
    UniaxialMaterial *theMaterial;  //!< pointer to a material
    double cosX[3]; //!< @brief direction cosines

    double computeCurrentStrain(void) const;
    double computeCurrentStrainRate(void) const;

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    void inicializa(void);
  protected:

  public:
    Spring(int tag, int dimension, int Nd1, int Nd2, UniaxialMaterial &theMaterial, double A);
    Spring(int tag,int dimension,const Material *ptr_mat);
    Spring(void);
    Spring(const Spring &);
    Spring &operator=(const Spring &);
    Element *getCopy(void) const;
    ~Spring(void);

    // public methods to obtain inforrmation about dof & connectivity    
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
    const Matrix &getKi(void);
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getDamp(void) const;    
    const Matrix &getMass(void) const;    

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    double getAxialForce(void) const;
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);    


    Response *setResponse(const std::vector<std::string> &argv, Information &eleInfo);
    int getResponse(int responseID, Information &eleInformation);
  };
} // end of XC namespace

#endif




