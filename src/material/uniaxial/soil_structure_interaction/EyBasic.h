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
                                                                        
#ifndef EyBasic_h
#define EyBasic_h

// File: ~/material/EyBasic.h
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for 
// EyBasic. EyBasic provides the abstraction
// of an elastic perfectly plastic uniaxial material, 
//
// What: "@(#) EyBasic.h, revA"

#include <material/uniaxial/ElasticPPMaterialBase.h>

namespace XC {
//
//! @brief Lateral reaction of soil.
//!
//! This class provides the abstraction of an elastic
//! perfectly plastic uniaxial material, addapted for its
//! use to model the lateral reaction of soil.
class EyBasic: public ElasticPPMaterialBase
  {
  private:
    double split_stress;
    //! @brief Computes yield function value.
    inline double yield_function(const double &sigtrial) const
      {
        if(sigtrial>=split_stress)
          return (sigtrial - fyp);
        else
          return (-sigtrial + fyn);
      }
  public:
    EyBasic(int tag= 0);    
    EyBasic(int tag, double E, double eyp, double eyn, double ezero);
    UniaxialMaterial *getCopy(void) const;    

    int setTrialStrain(double strain, double strainRate = 0.0);

    int commitState(void);
    
    void set_fyp(const double &);
    void set_fyn(const double &);
    
    inline double getLowerYieldStress(void)
      { return get_fyn(); }
    void setLowerYieldStress(const double &d)
      { set_fyn(d); }
    double getUpperYieldStress(void)
      { return get_fyp(); }
    void setUpperYieldStress(const double &d)
      { set_fyp(d); }
    
    void setParameters(const double &, const double &, const double &);
  };
} // end of XC namespace


#endif



