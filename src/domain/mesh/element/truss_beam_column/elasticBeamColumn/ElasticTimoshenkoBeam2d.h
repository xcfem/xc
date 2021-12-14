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

// $Revision$
// $Date$
// $URL$

#ifndef ElasticTimoshenkoBeam2d_h
#define ElasticTimoshenkoBeam2d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 03/13
// Revision: A
//
// Purpose: This file contains the class definition for ElasticTimoshenkoBeam2d.
// ElasticTimoshenkoBeam2d is a frame member that takes shearing deformations
// and rotational inertia effects into account.

#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam2dBase.h"
#include "domain/mesh/element/utils/fvectors/FVectorBeamColumn2d.h"

namespace XC {
  class Matrix;
  class Vector;
//! @brief 2D elastic Timoshenko beam element.
//!
//! ElasticTimoshenkoBeam2d is a frame member that takes shearing deformations
//! and rotational inertia effects into account.
//! @ingroup BeamColumnElemGrp
class ElasticTimoshenkoBeam2d: public ElasticBeam2dBase
  {
  private:
    // private methods
    void setUp();
    
    // parameters
    int cMass;   //!< consistent mass flag
    int nlGeo;   //!< nonlinear geometry flag
    double phi;  //!< ratio of bending to shear stiffness
    double L;    //!< element length
    
    // state variables
    mutable Vector ul; //!< displacements in local system
    mutable Vector ql; //!< forces in local system
    Vector ql0;    //!< fixed end forces due to loads in local system
    Matrix kl;     //!< stiffness matrix in local system
    Matrix klgeo;  //!< geometric stiffness matrix in local system
    
    // constant variables
    Matrix Tgl;  //!< transformation matrix from global to local system
    Matrix Ki;   //!< initial stiffness matrix in global system
    Matrix M;    //!< mass matrix in global system
    
    static Matrix theMatrix;  //!< a class wide Matrix
    static Vector theVector;  //!< a class wide Vector
    Vector theLoad;
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
  public:
    // constructor
    ElasticTimoshenkoBeam2d(int tag= 0);
    ElasticTimoshenkoBeam2d(int tag,const Material *m,const CrdTransf *trf);
    ElasticTimoshenkoBeam2d(int tag, double A, double E, double G, double Iz,
			    double Avy,
			    int Nd1, int Nd2, CrdTransf2d &theTransf,
			    double rho = 0.0, int cMass = 0);
    Element *getCopy(void) const;
    
    const Vector &computeCurrentStrain(void) const;
    
    void setDomain(Domain *theDomain);
    
    int commitState(void);
    
    // public methods to obtain stiffness, mass, damping and residual information
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;
    
    void zeroLoad();
    int addLoad(ElementalLoad *, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;
    
    //! @brief Internal shear force in the middle of the element.
    inline double getV(void) const
      { return (getV1()+getV2())/2.0; }
    //! @brief Internal shear force at the back end.   
    inline double getV1(void) const
      { return -ql(1); }
    //! @brief Internal shear force at the front end.   
    inline double getV2(void) const
      { return ql(4); }
    //! @brief Internal axial force at the back end.   
    inline double getN1(void) const
      { return -ql(0); }
    //! @brief Internal axial force at the front end.   
    inline double getN2(void) const
      { return ql(3); }
    //! @brief Internal axial force at the middle of the element.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getN(void) const //Average axial force.
      {
        return (getN1()+getN2())/2.0;
      }
    //! @brief Internal bending moment at the back end.   
    inline double getM1(void) const
      { return -ql(2); }
    //! @brief Internal bending moment at the front end.   
    inline double getM2(void) const
      { return ql(5); }
    // public methods for element output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag = 0) const;
    
    // public methods for element recorder
    Response *setResponse(const std::vector<std::string> &argv, Parameter &param);
    int getResponse(int responseID, Information &info);
    
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    
  };
} // end of XC namespace

#endif
