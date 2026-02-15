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
#ifndef ConcreteHistoryVars_h
#define ConcreteHistoryVars_h

#include <iostream>
#include <cmath>

namespace XC {

//! @ingroup MatUnx
//
//! @brief Concrete history variables.
struct ConcreteHistoryVars
  {
    double ecmin;  //!<  hstP(1)
    double dept;   //!<  hstP(2)
    double eps;  //!< strain
    double sig;  //!< stress
    double e;    //!< stiffness modulus
    
    inline ConcreteHistoryVars(void)
      : ecmin(0.0), dept(0.0), eps(0.0), sig(0.0), e(0.0) {}
    inline void setup_parameters(const double &initialTangent)
      {
	e= initialTangent;
        eps= 0.0;
        sig= 0.0;
      }
    inline double getStrain(void) const
      { return eps; }
    inline double getStress(void) const
      { return sig; }
    inline double getTangent(void) const
      { return e; }

    //! @brief Return the wecant stiffness for determination of creep strain.
    //! @param Ec: concrete stiffness.
    //! @param eps_m: concrete mechanical strain.
    double getSecantStiffness(const double &Ec, const double &eps_m) const
      {
	double retval= Ec;
	if(std::abs(eps_m/this->sig)<=Ec)
	  { retval = std::abs(this->sig/eps_m); } //ADDED 7/22

	if(std::isnan(retval))
	  { retval = Ec; }
        return retval;
      }
    void cutStress(const double &sigmin,const double &sigmax,const double &er)
      {
        if(sig <= sigmin)
          {
            sig= sigmin;
            e= er;
          }
        else if(sig >= sigmax)
	  {
            sig= sigmax;
            e= 0.5 * er;
          }
      }
    void Print(std::ostream &os) const
      {
        os << "Concrete02:(strain, stress, tangent) " << eps
	  << ", " << sig << ", " << e << std::endl;
      }
  };
 
//! @brief Printing stuff.
inline std::ostream &operator<<(std::ostream &os,const ConcreteHistoryVars &hv)
   {
     hv.Print(os);
     return os;
   }

struct CreepConcreteHistoryVars: public ConcreteHistoryVars
  {
    double ecmax; //!< added by AMK
    
    inline CreepConcreteHistoryVars(void)
      : ConcreteHistoryVars(), ecmax(0.0) {}
    inline void revertToStart(const double &initialTangent)
    {
      this->ecmin= 0.0;
      this->ecmax= 0.0;
      this->dept= 0.0;
      this->setup_parameters(initialTangent);
    }
  };
 
} // end of XC namespace

#endif
