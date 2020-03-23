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
//EsfBeamColumn3d.h


#ifndef EsfBeamColumn3d_h
#define EsfBeamColumn3d_h

#include "utility/matrix/Vector.h"

namespace XC {

class DbTagData;

class FEM_ObjectBroker;
class CommParameters;

//! @ingroup OneDimensionalElem
//
//! @brief 3D beam column internal forces.
class EsfBeamColumn3d: public Vector
  {
  public:
    EsfBeamColumn3d(void);
    explicit EsfBeamColumn3d(const Vector &);
    EsfBeamColumn3d(const EsfBeamColumn3d &);
    EsfBeamColumn3d &operator=(const EsfBeamColumn3d &);
    //! @brief Returns the axil.
    inline const double &N(void) const
      { return (*this)[0]; }
    //! @brief Returns the axil.
    inline double &N(void)
      { return (*this)[0]; }
    //! @brief Return the axial force in the back end.
    inline double AN1(void) const
      { return -N(); }
    //! @brief Return the axial force in the front end.
    inline double AN2(void) const
      { return N(); }
    //! @brief Returns the moment about z in the back end.
    inline const double &Mz1(void) const
      { return (*this)[1]; }
    //! @brief Returns the moment about z in the back end.
    inline double &Mz1(void)
      { return (*this)[1]; }
    //! @brief Returns the moment about z in the front end.
    inline const double &Mz2(void) const
      { return (*this)[2]; }
    //! @brief Returns the moment about z in the front end.
    inline double &Mz2(void)
      { return (*this)[2]; }
    //! @brief Returns shear along y axis.
    inline double Vy(const double &L) const
      { return (Mz1()+Mz2())/L; }
    //! @brief Returns the moment about y in the back end.
    inline const double &My1(void) const
      { return (*this)[3]; }
    //! @brief Returns the moment about y in the back end.
    inline double &My1(void)
      { return (*this)[3]; }
    //! @brief Returns the moment about y in the front end.
    inline const double &My2(void) const
      { return (*this)[4]; }
    //! @brief Returns the moment about y in the front end.
    inline double &My2(void)
      { return (*this)[4]; }
    //! @brief Returns the shear along z.
    inline double Vz(const double &L) const
      { return -((My1()+My2())/L); }
    //! @brief Returns the torsor.
    inline const double &T(void) const
      { return (*this)[5]; }
    //! @brief Returns the torsor.
    inline double &T(void)
      { return (*this)[5]; }
    //! @brief Returns the torsor in the back end.
    inline double T1(void) const
      { return -T(); }
    //! @brief Returns the torsor in the front end.
    inline double T2(void) const
      { return T(); }
  };
int sendEsfBeamColumn3d(const EsfBeamColumn3d &, int posDbTag,DbTagData &dt, CommParameters &cp);
int receiveEsfBeamColumn3d(EsfBeamColumn3d &v, int posDbTag,DbTagData &dt,const CommParameters &cp);

} // end of XC namespace

#endif
