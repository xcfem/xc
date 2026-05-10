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

#ifndef steel02_state_variables_h
#define steel02_state_variables_h

namespace XC {

//! @brief Variables to keep track of the material state.
struct steel02_state_variables
  {    
    double eps; //!< strain
    double sig; //!< stress
    double e; //!< stiffness modulus

    steel02_state_variables(void);
    void setup_parameters(const double &, const double &);
    void print(std::ostream &) const;
  };
  
inline steel02_state_variables::steel02_state_variables(void)
  : eps(0.0), sig(0.0), e(0.0)
  {}

inline void steel02_state_variables::setup_parameters(const double &E0, const double &sigini)
  {
    this->e= E0;
    this->sig= 0.0;
    this->eps= 0.0;

    if(sigini!=0.0)
      {
        this->eps= sigini/E0;
        this->sig= sigini;
      }
  }

//! @brief Print stuff.
inline void steel02_state_variables::print(std::ostream &os) const
  { os << this->eps << " " << this->sig << " " << this->e; }
  
inline std::ostream& operator<<(std::ostream &os, const steel02_state_variables &stv)
  {    
    stv.print(os);
    return os;
  }  

} // end of XC namespace

#endif

