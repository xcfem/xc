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
** ****************************************************************** */
                                                                        

// $Revision: 1.3 $
// $Date: 2002-12-05 22:49:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/J2PlateFibre.h,v $

// Written: MHS
// Created: Aug 2001
//
// Description: Elastic isotropic model where stress components 22, 33, and 23
// are condensed out.

#ifndef J2PlateFibre_h
#define J2PlateFibre_h

#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "material/nD/NDMaterial.h"

namespace XC{

class J2PlateFibre: public NDMaterial
  {
  private:
    static constexpr int order= 5;
    static constexpr double one3= 1.0/3.0;
    static constexpr double two3= 2.0/3.0;
    static const double root23;
    static Vector sigma; // Stress vector ... class-wide for returns
    static Matrix D; // Elastic constants
    
    double E; //!< Elastic modulus.
    double nu; //!< Poisson's ration.
    double sigmaY; //!< Yield stress.
    double Hiso; //!< Isotropic hardening parameter.
    double Hkin; //!< Kinematic hardening parameter.

    int parameterID;
    Matrix SHVs;

    Vector Tepsilon; //!< Trial strains
    Vector Tepsilon0; //!< Initial strains.

    double alphan;
    mutable double alphan1;

    mutable double dg_n1;

    double epsPn[order];
    mutable double epsPn1[order];
    
    void init(void);
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    J2PlateFibre(int tag= 0);
    J2PlateFibre(int tag, double E, double G, double sigY, double Hi, double Hk);
    ~J2PlateFibre(void);
    NDMaterial *getCopy(void) const;
    NDMaterial *getCopy(const std::string &type) const;

    double getE(void) const;
    void setE(const double &);
    double getnu(void) const;
    void setnu(const double &);
    double getYieldStress(void) const;
    void setYieldStress(const double &);
    double getHiso(void) const;
    void setHiso(const double &);
    double getHkin(void) const;
    void setHkin(const double &);
    
    int setInitialStrain(const Vector &v);
    const Vector &getInitialStrain(void) const;
    int setTrialStrain(const Vector &v);
    int setTrialStrain(const Vector &v, const Vector &r);
    int setTrialStrainIncr(const Vector &v);
    int setTrialStrainIncr(const Vector &v, const Vector &r);
    const Matrix &getTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Vector &getStress(void) const;
    const Vector &getStrain(void) const;
    double getVonMisesStress(void) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    const std::string &getType(void) const;
    int getOrder(void) const;

    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);

    void Print(std::ostream &, int flag = 0);

    int setParameter(const std::vector<std::string> &, Parameter &);
    int updateParameter(int parameterID, Information &);
    int activateParameter(int paramID);

    const Vector& getStressSensitivity(int gradIndex, bool conditional);
    int commitSensitivity(const Vector &depsdh, int gradIndex, int numGrads);
  };

} //end of XC namespace

#endif
