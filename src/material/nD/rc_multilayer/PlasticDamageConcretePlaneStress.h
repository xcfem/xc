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
//===============================================================================
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
                                                                        
// $Revision: 1.6 $
// $Date: 2006-08-04 18:18:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/ElasticIsotropicPlaneStress2D.h,v $

#ifndef PlasticDamageConcretePlaneStress_h
#define PlasticDamageConcretePlaneStress_h

// Written: Thanh Do
// Created: 07/16
//
// Description: 
//
// What: "@(#) ElasticIsotropicThreeDimesnional.h, revA"

#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "material/nD/NDMaterial.h"

namespace XC {
  
// Type Definitions
typedef struct {
  double E;
  double nu;
  double ft;
  double fc;
  double Ap;
  double An;
  double Bn;
  double beta;
} structMatData;

typedef struct {
  double sig[3];
  double eps_p[4];
  double rn;
  double rp;
  double dp;
  double dn;
} structCurrent;

typedef struct {
  double eps_p[4];
  double rn;
  double rp;
  double dp;
  double dn;
} structCommitted;

//typedef struct {
//  double sig[3];
//  double km[9];
//  struct2_T Pres;
//  double eps[3];
//  struct3_T Past;
//  double Deps[3];
//} struct1_T;

class PlasticDamageConcretePlaneStress : public NDMaterial
  {
  private:
    // parameters
    double E; //!< elastic modulus
    double nu; //!< Poisson ratio 
    double ft; //!< tensile yield strength
    double fc; //!< compressive yield strength
    double beta; //!< plastic deformation rate
    double Ap; //!< damage parameter
    double An; //!< damage parameter
    double Bn; //!< damage parameter

    double sig[3];
    double eps[3];
    double Deps[3];

    Matrix Ce;
    Matrix Ce0;
    Matrix CeCommitted;

    //  double Ce[9];  // current tangent
    //  double Ce0[9]; // initial tangent
    //  double CeCommitted[9]; // committed tangent

    // current state variables
    //double sig[3];
    double eps_p[4];
    double rn;
    double rp;
    double dp;
    double dn;

    // committed state variables
    double Committed_sig[4];
    double Committed_eps[3];
    double Committed_eps_p[4];
    double Committed_rn;
    double Committed_rp;
    double Committed_dp;
    double Committed_dn;

    Vector stress;
    Vector strain;
    Vector Cstress;
    Vector Cstrain;
  protected:
    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public:
    PlasticDamageConcretePlaneStress(int tag, 
				     double E, 
				     double nu, 
				     double ft,
				     double fc, 
				     double beta = 0.6, 
				     double Ap = 0.5, 
				     double An = 2.0, 
				     double Bn = 0.75);
    PlasticDamageConcretePlaneStress(int tag= 0);

    int setTrialStrain(const Vector &v);
    int setTrialStrain(const Vector &v, const Vector &r);
    int setTrialStrainIncr(const Vector &v);
    int setTrialStrainIncr(const Vector &v, const Vector &r);
    const Matrix &getTangent(void) const;
    const Matrix &getInitialTangent(void) const;

    const Vector &getStress(void) const;
    const Vector &getStrain(void) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    NDMaterial *getCopy(const std::string &) const;
    NDMaterial *getCopy(void) const;
    const std::string &getType(void) const;
    int getOrder(void) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag = 0) const;  
  };
} // end of XC namespace

#endif

