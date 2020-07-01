// -*-c++-*-
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
**                                                                    **
** Additions and changes by:                                          **
**   Boris Jeremic (@ucdavis.edu)                                     **
**                                                                    **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.16 $
// $Date: 2004/07/20 22:39:02 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/NDMaterial.h,v $


#ifndef NDMaterial_h
#define NDMaterial_h

// File: ~/material/NDMaterial.h
//
// Written: MHS
// Created: Feb 2000
// Revision: A
//
// Description: This file contains the class definition for NDMaterial.
// NDMaterial is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.
//
// What: "@(#) NDMaterial.h, revA"

#include <material/Material.h>
#include "utility/matrix/nDarray/stresst.h"

namespace XC {
class Matrix;
class ID;
class Vector;
class Information;
class Response;
class straintensor;
class stresstensor;
class Tensor;

//! @ingroup Mat
//!
//! @defgroup NDMat n-dimensional materials.
//
//! @ingroup NDMat
//
//! @brief Base class for 2D and 3D materials.
//!
//! NDMaterial class provides the interface that all
//! NDMaterial writers must provide when introducing new
//! NDMaterial subclasses. An NDMaterial object 
//! is responsible for keeping track of stress, strain and the
//! constitution for a particular integration point in the domain. 
class NDMaterial: public Material
  {
  private:
    static Matrix errMatrix;
    static Vector errVector;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    NDMaterial();
    NDMaterial(int tag, int classTag);

    // methods to set state and retrieve state using Matrix and Vector classes
    virtual double getRho(void) const;
    virtual void setRho(const double &);
// BJ added 19June2002
    virtual double getE(void);
    virtual double getnu(void);
    virtual double getpsi(void);


    virtual int setTrialStrain(const Vector &v);
    virtual int setTrialStrain(const Vector &v, const Vector &r);
    virtual int setTrialStrainIncr(const Vector &v);
    virtual int setTrialStrainIncr(const Vector &v, const Vector &r);
    virtual const Matrix &getTangent(void) const;
    inline virtual const Matrix &getInitialTangent(void) const
      {return this->getTangent();};

    virtual const Vector &getStress(void) const;
    virtual const Vector &getStrain(void) const;
    //! @brief Return the generalized stress.
    inline const Vector &getGeneralizedStress(void) const
      { return getStress(); }
    //! @brief Return the generalized strain.
    inline const Vector &getGeneralizedStrain(void) const
      { return getStrain(); }

    virtual int setTrialStrain(const Tensor &);
    virtual int setTrialStrain(const Tensor &, const Tensor &);
    virtual int setTrialStrainIncr(const Tensor &);
    virtual int setTrialStrainIncr(const Tensor &, const Tensor &);
    const Tensor &getTangentTensor(void) const;
    virtual const stresstensor &getStressTensor(void) const;
    virtual const straintensor &getStrainTensor(void) const;
    virtual const straintensor &getPlasticStrainTensor(void) const; //Added Joey Aug. 13, 2001

    virtual void setInitialGeneralizedStrain(const Vector &);
    const Vector &getInitialGeneralizedStrain(void) const;

    virtual const Vector &getCommittedStress(void);
    virtual const Vector &getCommittedStrain(void);

    //! @brief Virtual constructor.
    virtual NDMaterial *getCopy(void) const= 0;
    virtual NDMaterial *getCopy(const std::string &) const= 0;

    virtual const std::string &getType(void) const = 0;
    virtual int getOrder(void) const = 0;

    virtual Response *setResponse(const std::vector<std::string> &argv, Information &matInformation);
    virtual int getResponse (int responseID, Information &matInformation);

// AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);
    virtual const Vector & getStressSensitivity(int gradIndex, bool conditional);
    virtual const Vector & getStrainSensitivity(int gradIndex);
    virtual const Matrix & getTangentSensitivity(int gradIndex);
    virtual const Matrix & getDampTangentSensitivity(int gradIndex);
    virtual double getRhoSensitivity(int gradIndex);
    virtual int commitSensitivity(Vector & strainGradient, int gradIndex, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////

  };

NDMaterial *receiveNDMaterialPtr(NDMaterial *,DbTagData &,const Communicator &,const BrokedPtrCommMetaData &);

} // end of XC namespace


#endif
