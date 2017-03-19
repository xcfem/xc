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
//DeformationPlane

#ifndef DEFORMATION_PLANE_H
#define DEFORMATION_PLANE_H

#include <iostream>
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "utility/actor/actor/MovableObject.h"

class Pos2d;
class Pos3d;
class Recta2d;
class Semiplano2d;

namespace XC {

class Vector;
class ResponseId;

//! \ingroup MATSCC
//
//! @defgroup MATSCCDiagInt Iteraction diagram for a cross-section.
//
//! @ingroup MATSCCDiagInt
//
//! @brief Deformation plane for a cross-section.
class DeformationPlane: public Plano3d, public MovableObject
  {
  protected:
    bool check_positions(const Pos2d &,const Pos2d &, const Pos2d &);
    bool check_positions(const Pos3d &,const Pos3d &, const Pos3d &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    friend class SectionForceDeformation;
    friend class DqFibras;
    friend class FiberSectionBase; //Erase this line.
    const Vector &getDeformation(void) const;
  public:
    DeformationPlane( const Pos3d &p1,const Pos3d &p2, const Pos3d &p3);
    explicit DeformationPlane(const Plano3d &);
    DeformationPlane( const Pos2d &yz1, const double &e_1, //Strains at three fiber sections.
                      const Pos2d &yz2, const double &e_2,
                      const Pos2d &yz3, const double &e_3);
    explicit DeformationPlane(const double &eps= 0.0);
    DeformationPlane(const Vector &e);

    double Strain(const Pos2d &p) const;
    const Vector &getDeformation(const size_t &order,const ResponseId &code) const;

    void ConstantStrain(const double &);

    Recta2d getFibraNeutra(void) const;
    Pos2d getPuntoSemiplanoTracciones(void) const;
    Pos2d getPuntoSemiplanoCompresiones(void) const;
    Semiplano2d getSemiplanoTracciones(void) const;
    Semiplano2d getSemiplanoTracciones(const Recta2d &) const;
    Semiplano2d getSemiplanoCompresiones(void) const;
    Semiplano2d getSemiplanoCompresiones(const Recta2d &) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

} // end of XC namespace

#endif
