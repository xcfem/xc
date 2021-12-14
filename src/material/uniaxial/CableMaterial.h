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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/25 23:33:37 $: 2001/07/16 08:23:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/CableMaterial.h,v $
                                                                        
                                                                        
#ifndef CableMaterial_h
#define CableMaterial_h

// Written: Charles Chadwell
// Created: 07/01
//
// Description: This file contains the class definition for 
// CableMaterial. CableMaterial provides the abstraction
// of an elastic uniaxial material,
//
// The input parameters are the Prestress, E, Effective Self Weight (gravity component of 
// Weight per volume transverse to the cable), and Length of the cable.
//
// The cable Force Displacement is converted to Stress Strain material for use 
// with the truss element.  The stress strain ranges from slack (large strain at zero 
// stress) to taught (linear with modulus E).  The material has no history and is not
// path dependent.
//
//
// What: "@(#) CableMaterial.h, revA"


#include <material/uniaxial/ElasticBaseMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief CableMaterial provides the abstraction of an elastic
//! uniaxial material, the input parameters are the Prestress, E,
//! Effective Self Weight (gravity component of 
//! Weight per volume transverse to the cable), and Length of the cable.
//!
//! The cable Force Displacement is converted to Stress Strain material
//! for use with the truss element. The stress strain ranges from
//! slack (large strain at zero stress) to taught (linear with modulus E).
//! The material has no history and is not path dependent.
class CableMaterial: public ElasticBaseMaterial
  {
  private:
    double Ps; //!< Material prestress.
    double Mue;
    double L; //!< cable length.
    double trialStress;      //!< current trial stress
    double trialTangent;     //!< current trial tangent
    
    double evalStress(double stress);
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    CableMaterial(int tag, double Prestress, double E, double unitWeightEff, double L_Element);
    CableMaterial(int tag= 0);    

    void setLength(const double &);
    inline double getLength(void) const
      { return L; }
    int setTrialStrain(double strain, double strainRate = 0.0); 
    int setTrial(double strain, double &stress, double &tangent, double strainRate = 0.0);

    inline double getStrain(void) const
      {return trialStrain;};
    double getStress(void) const;
    double getTangent(void) const;
    inline double getInitialTangent(void) const
      {return 1.0e-8;}; 

    //! @brief Set value of cable prestress.
    inline void setPrestress(const double &d)
      { Ps= d; }
    //! @brief Return value of cable prestress.
    double getPrestress(void) const
      { return Ps; }

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;

    
    //int setParameter(const std::vector<std::string> &argv, Parameter &param);
    //int updateParameter(int parameterID, Information &info);
  };
} // end of XC namespace


#endif

