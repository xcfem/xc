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
//ElasticPlateBase.h

#ifndef ElasticPlateBase_h
#define ElasticPlateBase_h

#include "PlateBase.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

namespace XC {

//! @ingroup MATPLAC
//
//! @brief Base class for elastic plate materials.
class ElasticPlateBase: public PlateBase
  {
  protected:
    double E;  //!< elastic modulus
    double nu; //!< poisson ratio

    static const double five6; // =5/6 = shear correction factor

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public : 

    ElasticPlateBase(int tag,int classTag);
    ElasticPlateBase(int classTag);
    ElasticPlateBase(int tag,int classTag, double E, double nu, double h);

    int commitState(void); 
    int revertToLastCommit(void);
    int revertToStart(void);

    inline double getE(void) const
      { return E; }
    void setE(const double &d)
      { E= d; }
    inline double getnu(void) const
      { return nu; }
    void setnu(const double &d)
      { nu= d; }

    inline double membraneModulus(void) const
      { return (E/(1.0-nu*nu)*h); }
    inline double shearModulus(void) const
      { return (0.5*E/(1.0+nu)*h); }
    inline double bendingModulus(void) const
      { return (E*(h*h*h)/12.0/( 1.0 - nu*nu )); }


  }; //end of ElasticPlateBase declarations

} // end of XC namespace

#endif
