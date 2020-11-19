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
//PlateBase.h

#ifndef PlateBase_h
#define PlateBase_h

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include "material/section/SectionForceDeformation.h"


namespace XC {

//! @ingroup MATSCC
//!  @defgroup MATPLAC Materials for bidimensional elements (plates, membranes, shells,...).
//
//! @ingroup MATPLAC
//
//! @brief Base class for bidimensional membrane/plate/shell materials.
class PlateBase: public SectionForceDeformation
  {
  protected:
    double h; //!< plate thickness
    
    int sendData(Communicator &);
    int recvData(const Communicator &);
  private:
    double rhoH ; //!< mass per unit 2D area
  public: 
    PlateBase(int tag,int classTag);
    PlateBase(int classTag);
    PlateBase(int tag,int classTag, double h, double rho);

    double getStrain(const double &y,const double &z) const;

    inline double getH(void) const
      { return h; }
    inline void setH(const double &d)
      { h= d; }
    virtual double getRho(void) const;
    virtual void setRho(const double &);
    virtual double getArealDensity(void) const;
    virtual void setArealDensity(const double &);
    XC::Matrix getValues(const std::string &, bool silent) const;
  };
} // end of XC namespace

#endif
