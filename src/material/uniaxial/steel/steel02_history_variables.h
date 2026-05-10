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

#ifndef steel02_history_variables_h
#define steel02_history_variables_h

namespace XC {

//! @brief Variables to keep track of the material history.
struct steel02_history_variables
  {
    double epsmin; //!< max eps in compression
    double epsmax; //!< max eps in tension
    double epspl; //!< plastic excursion
    double epss0; //!< eps at asymptotes intersection
    double sigs0; //!< sig at asymptotes intersection
    double epssr; //!< eps at last inversion point
    double sigsr; //!< sig at last inversion point
    int    kon; //!< index for loading/unloading
    
    steel02_history_variables(void);
    void setup_parameters(const double &, const double &);
    void partial_revert_to_last_commit(const steel02_history_variables &);
  };
  
inline steel02_history_variables::steel02_history_variables(void)
  : epsmin(0.0), epsmax(0.0), epspl(0.0), epss0(0.0),
    sigs0(0.0), epssr(0.0), sigsr(0.0), kon(0)
  {}

inline void steel02_history_variables::setup_parameters(const double &E0, const double &fy)
  {
    this->epsmax= fy/E0;
    epsmin= -this->epsmax;
    this->epspl= 0.0;
    this->epss0= 0.0;
    this->sigs0= 0.0;
    this->epssr= 0.0;
    this->sigsr= 0.0;
  }

inline void steel02_history_variables::partial_revert_to_last_commit(const steel02_history_variables &hstvP)
  {
    this->epsmax= hstvP.epsmax; // XXX = std::max(this->epsmax, hstvP.epsmax) ??
    this->epsmin= hstvP.epsmin; // XXX = std::min(this->epsmin, hstvP.epsmin) ??
    this->epspl= hstvP.epspl;
    this->epss0= hstvP.epss0;
    this->sigs0= hstvP.sigs0;
    this->epssr= hstvP.epssr;  
    this->sigsr= hstvP.sigsr;
    this->kon= hstvP.kon;
  }

} // end of XC namespace

#endif

