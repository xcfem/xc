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
// $Source: /usr/local/cvs/OpenSees/SRC/element/truss/Truss.h,v $
                                                                        
                                                                        
#ifndef Truss_h
#define Truss_h

// File: ~/element/truss/Truss.h
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for Truss. 
//
// What: "@(#) Truss.h, revA"

#include "TrussBase.h"

namespace XC {
class Channel;
class UniaxialMaterial;

//! @ingroup TrussElem
//
//! @brief Truss finite element.
//!
//! This elements can be defined in one, two or three-dimesional
//! problems. Each element is associated with two Node objects
//! and a UniaxialMaterial object.
class Truss : public TrussBase
  {
  private:
    double computeCurrentStrain(void) const;
    double computeCurrentStrainRate(void) const;
    
    // private attributes - a copy for each object of the class
    UniaxialMaterial *theMaterial;  //!< pointer to a material
    double A; //!< Truss cross-section area

// AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
    Vector *theLoadSens;
// AddingSensitivity:END ///////////////////////////////////////////

    void initialize(void);
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    void free_material(void);
    void free_load_sens(void);
    void free_mem(void);
    void set_material(const UniaxialMaterial &);
    void set_load_sens(const Vector &);

  public:
    Truss(int tag, int dimension, int Nd1, int Nd2, UniaxialMaterial &theMaterial, double A);
    Truss(int tag,int dimension,const Material *ptr_mat);
    Truss(void);
    Truss(const Truss &);
    Truss &operator=(const Truss &);
    Element *getCopy(void) const;
    ~Truss(void);

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
    inline const double &getArea(void) const
      { return A; }
    inline void setArea(const double &a)
      { A= a; }
    inline double getLinearRho(void) const
      { return getArea()*getRho(); }

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getKi(void);
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getDamp(void) const;    
    const Matrix &getMass(void) const; 

    void zeroLoad(void);	
    virtual void createInertiaLoad(const Vector &);
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

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int		   addInertiaLoadSensitivityToUnbalance(const Vector &accel, bool tag);
    int            setParameter(const std::vector<std::string> &argv, Parameter &param);
    int            updateParameter(int parameterID, Information &info);
    int            activateParameter(int parameterID);
    const Vector & getResistingForceSensitivity(int gradNumber);
    const Matrix & getKiSensitivity(int gradNumber);
    const Matrix & getMassSensitivity(int gradNumber);
    int            commitSensitivity(int gradNumber, int numGrads);
    // AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace

#endif




