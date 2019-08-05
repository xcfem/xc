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
                                                                        
// $Revision: 1.10 $
// $Date: 2003/02/14 23:01:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/elastic_isotropic/ElasticIsotropicMaterial.h,v $
                                                                        
                                                                        
#ifndef ElasticIsotropicMaterial_h
#define ElasticIsotropicMaterial_h

// File: ~/material/ElasticIsotropicMaterial.h
//
// Written: MHS
// Created: Feb 2000
// Revision: A
//
// Description: This file contains the class definition for ElasticIsotropicMaterialModel.
// ElasticIsotropicMaterialModel is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes. 
//
// What: "@(#) ElasticIsotropicMaterial.h, revA"

#include <material/nD/NDMaterial.h>
#include <utility/matrix/Vector.h>

namespace XC {
//! @ingroup NDMat
//!
//! @defgroup EINDMat Elastic isotropic n-dimensional material.
//
//! @ingroup EINDMat
//
//! @brief Base class for elastic isotropic materials.
//!
//! ElasticIsotropicMaterial is an abstract class.  It provides the
//! interface to which all elastic isotropic material implementations
//! must conform.  It also serves as a prototype for all elastic isotropic
//! material implementations, as described by the Prototype pattern in
//! {\em Design Patterns} by Gamma et al.
//! (see https://en.wikipedia.org/wiki/Design_Patterns).
class ElasticIsotropicMaterial: public NDMaterial
  {
  protected:
    double E; //!< Elastic modulus
    double v; //!< Poisson ratio
    double rho ; //!< mass per unit 3D volume
    Vector epsilon; //!< Strain vector

    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    // Only called by subclasses to pass their tags to NDMaterialModel
    ElasticIsotropicMaterial(int tag, int classTag, int eps_size, double E, double nu, double rho = 0.0);

    // Called by clients
    ElasticIsotropicMaterial(int tag, int eps_size, double E, double nu, double rho = 0.0);
    ElasticIsotropicMaterial(int tag, int eps_size);

    // For parallel processing
    ElasticIsotropicMaterial(void);

    inline virtual double getRho(void) const
      { return rho; }
    inline virtual void setRho(const double &r)
      { rho= r; }
// BJ added 19June2002
    double getE(void);
    inline void setE(const double &e)
      { E= e; }
    double getnu(void);
    inline void setnu(const double &nu)
      { v= nu; }

    virtual int setTrialStrain(const Vector &v);
    virtual int setTrialStrain(const Vector &v, const Vector &r);
    virtual int setTrialStrainIncr(const Vector &v);
    virtual int setTrialStrainIncr(const Vector &v, const Vector &r);
    virtual const Matrix &getTangent(void) const;
    virtual const Matrix &getInitialTangent(void) const;
    virtual const Vector &getStress(void) const;
    virtual const Vector &getStrain(void) const;

    int setTrialStrain(const Tensor &v);
    int setTrialStrain(const Tensor &v, const Tensor &r);
    int setTrialStrainIncr(const Tensor &v);
    int setTrialStrainIncr(const Tensor &v, const Tensor &r);
    const Tensor &getTangentTensor(void) const;
    const stresstensor &getStressTensor(void) const;
    const straintensor &getStrainTensor(void) const;
    const straintensor &getPlasticStrainTensor(void) const;
    
    
    virtual int commitState(void);
    virtual int revertToLastCommit(void);
    virtual int revertToStart(void);
    
    // Create a copy of material parameters AND state variables
    // Called by GenericSectionXD
    virtual NDMaterial *getCopy(void) const;

    // Create a copy of just the material parameters
    // Called by the continuum elements
    virtual NDMaterial *getCopy(const std::string &) const;

    // Return a string indicating the type of material model
    virtual const std::string &getType(void) const;

    virtual int getOrder(void) const;
    
    virtual int sendSelf(CommParameters &);  
    virtual int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0);

    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);


  };
} // end of XC namespace


#endif
