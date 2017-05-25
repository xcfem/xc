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
                                                                        
// $Revision: 1.8 $
// $Date: 2003/03/04 00:48:18 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/UniaxialMaterial.h,v $
                                                                        
                                                                        
// File: ~/material/UniaxialMaterial.h
//
// Written: fmk 
// Created: 05/98
// Revision: A
//
// Description: This file contains the class definition for 
// UniaxialMaterial. UniaxialMaterial is a base class and 
// thus no objects of it's type can be instantiated. It has pure virtual 
// functions which must be implemented in it's derived classes. 
//
// What: "@(#) UniaxialMaterial.h, revA"

#ifndef UniaxialMaterial_h
#define UniaxialMaterial_h

#define POS_INF_STRAIN        1.0e16
#define NEG_INF_STRAIN       -1.0e16

#include <material/Material.h>
namespace XC {
class ID;
class Vector;
class Matrix;
class Information;
class Response;

class SectionForceDeformation;

//! @ingroup Mat
//
//! @defgroup MatUnx Uniaxial material models.
//
//! @ingroup MatUnx
//
//! @brief Base class for uniaxial materials.
//!
//! A UniaxialMaterial object is responsible for
//! keeping track of stress, strain and the
//! constitution for a particular point in the domain. 
class UniaxialMaterial : public Material
  {
  private:
    double rho; //!< Linear mass density.
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    UniaxialMaterial(int tag, int classTag);
        
    virtual int setInitialStrain(double strain);
    //! @brief Sets the value of the trial strain.
    //!
    //! Sets the value of the trial strain
    //!return 0 if successful, a negative number if not.
    virtual int setTrialStrain(double strain, double strainRate = 0.0)= 0;
    virtual int setTrial(double strain, double &stress, double &tangent, double strainRate = 0.0);

    virtual double getInitialStrain(void) const;
    virtual double getStrain(void) const= 0;
    virtual double getStrainRate(void) const;
    //! @brief Return the current value of stress.
    virtual double getStress(void) const= 0;
    const Vector &getGeneralizedStress(void) const;
    const Vector &getGeneralizedStrain(void) const;

    virtual void setInitialGeneralizedStrain(const Vector &);
    const Vector &getInitialGeneralizedStrain(void) const;

    //! @brief Return the current value of the tangent for the trial strain.
    virtual double getTangent(void) const= 0;
    virtual double getInitialTangent(void) const= 0;
    virtual double getDampTangent(void) const;
    virtual double getSecant(void) const;
    virtual double getFlexibility(void) const;
    virtual double getInitialFlexibility(void) const;
    virtual double getRho(void) const;
    void setRho(const double &);

    //! @brief Virtual constructor.
    virtual UniaxialMaterial *getCopy(void) const=0;
    virtual UniaxialMaterial *getCopy(SectionForceDeformation *s) const;
	
    virtual Response *setResponse(const std::vector<std::string> &argv, Information &matInformation);
    virtual int getResponse(int responseID, Information &matInformation);    

// AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual int    setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int    updateParameter(int parameterID, Information &info);
    virtual int    activateParameter(int parameterID);
    virtual double getStressSensitivity(int gradNumber, bool conditional);
    virtual double getStrainSensitivity(int gradNumber);
    virtual double getInitialTangentSensitivity(int gradNumber);
    virtual double getDampTangentSensitivity(int gradNumber);
    virtual double getRhoSensitivity(int gradNumber);
    virtual int    commitSensitivity(double strainGradient, int gradNumber, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////
  };
UniaxialMaterial *receiveUniaxialMaterialPtr(UniaxialMaterial *,DbTagData &,const CommParameters &,const BrokedPtrCommMetaData &);

} // end of XC namespace


#endif

