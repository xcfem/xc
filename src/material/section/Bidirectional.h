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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/25 23:33:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/Bidirectional.h,v $
                                                                        
                                                                        
#ifndef Bidirectional_h
#define Bidirectional_h

// File: ~/material/Bidirectional.h
//
// Written: MHS
// Created: Feb 2000
// Revision: A
//
// Description: 
//
// What: "@(#) Bidirectional.h, revA"

#include <material/section/SectionForceDeformation.h>

namespace XC {
//! @ingroup MATSCC
//
//! @brief bi-directional section, which is a stress-resultant
//! plasticity model of two coupled forces.
//!
//! The yield surface is circular and there is combined isotropic
//! and kinematic hardening. The implementation is a generalization
//! of the uniaxial return map algorithm for rate independent
//! plasticity (page 45, Simo and Hughes, 1998) with the same input
//! parameters as the Hardening Material uniaxial material model.
//! The bidirectional section is a suitable base isolator model and
//! should be used in conjunction with a ZeroLengthSection element to
//! this end. It can also be used in a nonlinear beam element to
//! define stress resultant plasticity at an integration point.
//! The optional code1 and code2 values correspond to the beam cross-section
//! analogy with respect to the local axes of the calling element (P, Vy,
//! and Vz = force along section local x, y, and z axes, respectively; T,
//! My, and Mz = moment about section local x, y, and z axes, respectively).
class Bidirectional: public SectionForceDeformation
  {
  private:
    double E; //!< Elastic modulus.
    double sigY; //! Yield
    double Hiso; //! isotropic hardening modulus.
    double Hkin; //!< kinematic hardening modulus.

    double e_n1Trial[2]; //!< Trial strain.
    double e_n1Inic[2]; //!< Initial strain.
    double eP_n[2];
    mutable double eP_n1[2];

    double q_n[2];
    mutable double q_n1[2];

    double alpha_n;
    mutable double alpha_n1;
	
    static Vector s;
    static Matrix ks;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    Bidirectional(int tag, double E, double sigY, double Hiso, double Hkin);
    Bidirectional(int tag= 0);

    int setInitialSectionDeformation(const Vector &v);
    int setTrialSectionDeformation(const Vector &v);
    const Matrix &getSectionTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Vector &getStressResultant(void) const;
    void zeroInitialSectionDeformation(void);
    const Vector &getInitialSectionDeformation(void) const;
    const Vector &getSectionDeformation(void) const;
    virtual double getStrain(const double &,const double &) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    
    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder(void) const;
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag = 0) const;
  };
} // end of XC namespace


#endif
