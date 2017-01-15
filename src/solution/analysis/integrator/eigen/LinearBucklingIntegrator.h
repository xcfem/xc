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
//LinearBucklingIntegrator.h
// Description: This file contains the class definition of LinearBucklingIntegrator.
// LinearBucklingIntegrator is an algorithmic class for setting up the finite element 
// equations for a eigen problem analysis. 
//
// This class is inheritanted from the base class of Integrator which was
// created by fmk (Frank).


#ifndef LinearBucklingIntegrator_h
#define LinearBucklingIntegrator_h

#include <solution/analysis/integrator/EigenIntegrator.h>

namespace XC {

//! @ingroup EigenIntegrator
//
//! @brief LinearBucklingIntegrator is an algorithmic class for setting up the finite element 
//! equations for a linear buckling analysis. 
class LinearBucklingIntegrator : public EigenIntegrator
  {
  protected:

    friend class SoluMethod;
    LinearBucklingIntegrator(SoluMethod *);
    Integrator *getCopy(void) const;
  public:
     
    // methods to form the K(t) and K(t+Dt) matrices.
    virtual int formKt(void);
    virtual int formKtplusDt(void);
    void Print(std::ostream &,int);
  };
inline Integrator *LinearBucklingIntegrator::getCopy(void) const
  { return new LinearBucklingIntegrator(*this); }
} // end of XC namespace

#endif




