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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/FeapMaterial.h,v $
                                                                      
// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// FeapMaterial. FeapMaterial wraps a Feap material subroutine.

#ifndef FeapMaterial_h
#define FeapMaterial_h

#include <material/nD/NDMaterial.h>

namespace XC {
//! @ingroup NDMat
//!
//! @defgroup FeapNDMat Wrappers for FEAP material subroutine.
//
//! @ingroup FeapNDMat
//
//! @brief Base class for 2D and 3D FEAP materials.
class FeapMaterial : public NDMaterial
  {
  public:
    FeapMaterial(int tag, int classTag, int numHV, int numData,double rho = 0.0);
    FeapMaterial(int classTag);
  virtual ~FeapMaterial();
  
  virtual int setTrialStrain(const Vector &strain);
  virtual const Vector &getStrain(void) const;
  virtual const Vector &getStress(void) const;
  virtual const Matrix &getTangent(void) const;
  
  inline virtual double getRho(void) const
    { return rho; }
  inline virtual void setRho(const double &r)
    { rho= r; }
  
  virtual int commitState(void);
  virtual int revertToLastCommit(void);    
  virtual int revertToStart(void);        
  
  virtual NDMaterial *getCopy(void) const;
  virtual NDMaterial *getCopy(const std::string &) const;
  virtual const std::string &getType(void) const;
  virtual int getOrder(void) const;
  
  virtual int sendSelf(CommParameters &);  
  virtual int recvSelf(const CommParameters &);    
  
  virtual void Print(std::ostream &s, int flag = 0);
  
 protected:
  // Invokes the Feap subroutine
  virtual int invokeSubroutine(int isw);
  virtual int fillDArray(void);
  
  double *ud;	// Material parameters array
  double *hstv;	// History array: first half is committed, second half is trial
  
  static double d[];    // Feap material parameter array
  
  double rho;           // Material mass density
  
 private:
  int numHV;		// Number of history variables
  int numData;		// Number of material parameters
  
  double eps[6];        // Strain vector
  static double sig[6]; // Stress vector
  static double dd[36]; // Tangent matrix
  
  static Vector strain3;
  static Vector strain4;
  static Vector strain6;
  
  static Vector sigma3;
  static Vector sigma4;
  static Vector sigma6;
  
  static Matrix tangent3;
  static Matrix tangent4;
  static Matrix tangent6;
  
  enum Formulation{Unknown, ThreeDimensional, PlaneStrain, AxiSymmetric};
  int myFormulation;
  void setType(Formulation form);
};
} // end of XC namespace

#endif
