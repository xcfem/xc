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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:01:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/DrainMaterial.h,v $
                                                                      
// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// DrainMaterial. DrainMaterial wraps a Drain spring element subroutine
// and converts it to the UniaxialMaterial interface for use in
// zero length elements, beam sections, or anywhere else
// UniaxialMaterials may be used.
//
// For more information see the Drain-2DX user guide:
//    Allahabadi, R.; Powell, G. H.
//    UCB/EERC-88/06, Berkeley: Earthquake Engineering Research Center,
//    University of California, Mar. 1988, 1 vol.

#ifndef DrainMaterial_h
#define DrainMaterial_h

#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! @ingroup Drain2DX
//!
//! @defgroup Drain2DX Drain-2DX materials wrappers.
//
//! @ingroup Drain2DX
// 
//! @brief Base class for wrappers for <a href="https://nisee.berkeley.edu/elibrary/getpkg?id=DRAIN2DX">DRAIN-2DX</a> FORTRAN routines.
//!
//! DRAIN-2DX, a computer program for the analysis of inelastic 2-dimensional
//! structures under static and dynamic loading, with particular emphasis
//! on seismic response
class DrainMaterial: public UniaxialMaterial
  {
  private:
    double epsilon; //!< Trial strain
    double epsilonDot; //!< Trial strain rate
    double sigma; //!< Trial stress
    double tangent; //!< Trial tangent
  protected:
    std::vector<double> matParams; //!< Material parameters array
    std::vector<double> hstv; //!< History array: first half is committed, second half is trial

    int numData; //!< Number of material parameters
    int numHstv; //!< Number of history variables

    double epsilonP; //!< Committed strain
    double sigmaP; //!< Committed stress
    double tangentP; //!< Committed tangent

    double beto; //!< Stiffness proportional damping factor
    double initialTangent; //!< initial tangent

    // Invokes the Drain subroutine
    virtual int invokeSubroutine(void);
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    DrainMaterial(int tag, int classTag, int numHV, int numData, double beto = 0.0);

    virtual int setTrialStrain(double strain, double strainRate = 0.0);
    virtual int setTrial(double strain, double &stress, double &tangent, double strainRate = 0.0); 
    virtual double getStrain(void) const;
    virtual double getStrainRate(void) const;
    virtual double getStress(void) const;
    virtual double getTangent(void) const;
    virtual double getDampTangent(void) const;
    virtual double getInitialTangent(void) const;

    virtual int commitState(void);
    virtual int revertToLastCommit(void);    
    virtual int revertToStart(void);        

    // WARNING -- if you wish to override any method in this base class, you must
    // also override the getCopy method to return a pointer to the derived class!!!
    virtual UniaxialMaterial *getCopy(void) const;

    virtual int sendSelf(Communicator &);  
    virtual int recvSelf(const Communicator &);
    
    virtual void Print(std::ostream &s, int flag = 0) const;
  };
} // end of XC namespace


#endif

