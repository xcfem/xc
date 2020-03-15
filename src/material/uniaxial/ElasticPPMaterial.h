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
// $Date: 2003/02/25 23:33:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ElasticPPMaterial.h,v $
                                                                        
#ifndef ElasticPPMaterial_h
#define ElasticPPMaterial_h

// File: ~/material/ElasticPPMaterial.h
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for 
// ElasticPPMaterial. ElasticPPMaterial provides the abstraction
// of an elastic perfectly plastic uniaxial material, 
//
// What: "@(#) ElasticPPMaterial.h, revA"

#include <material/uniaxial/EPPBaseMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Elastic perfectly plastic material.
//!
//! This class provides the abstraction of an elastic
//! perfectly plastic uniaxial material, i.e. the stress-strain
//! relationship is given by the linear equation \f$\sigma = E * \epsilon\f$
//! while the material is elastic and \f$| \sigma = E * \epsilon_p |\f$  while
//! the material is undergoing plastic deformation.
class ElasticPPMaterial : public EPPBaseMaterial
  {
  private:
    double fyp, fyn; //!< positive and negative yield stress

    //! @brief Computes yield function value.
    inline double yield_function(const double &sigtrial) const
      {
        if(sigtrial>=0.0)
          return (sigtrial - fyp);
        else
          return (-sigtrial + fyn);
      }
  protected:
    inline double def_total(void) 
      { return EPPBaseMaterial::def_total()-commitStrain; }
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    ElasticPPMaterial(int tag, double E, double eyp);    
    ElasticPPMaterial(int tag, double E, double eyp, double eyn, double ezero);    
    ElasticPPMaterial(int tag);    
    ElasticPPMaterial(void);

    void set_fyp(const double &);
    void set_eyp(const double &);
    void set_fyn(const double &);
    void set_eyn(const double &);
    double get_fyp(void) const;
    double get_eyp(void) const;
    double get_fyn(void) const;
    double get_eyn(void) const;

    int setTrialStrain(double strain, double strainRate = 0.0); 

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);    

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace


#endif



