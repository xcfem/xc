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
//SeccionInerte.h

#ifndef SeccionInerte_h 
#define SeccionInerte_h 

#include "xc_utils/src/nucleo/EntCmd.h"

class Pos2d;
class Recta2d;
class EjesPrincInercia2d;
class Vector2d;

namespace XC {
class CrossSectionProperties3d;
class CrossSectionProperties2d;
class Vector;
class Matrix;

//! @ingroup MATSCCRepres
//
//! @brief Seccion capaz de devolver parámetros mecánicos:
//! area, momentos de inercia, ...
class SeccionInerte: public EntCmd
  {
  public:
    SeccionInerte(EntCmd *owr= NULL);

    virtual double getAreaGrossSection(void) const= 0;
    virtual Vector getCdgGrossSection(void) const= 0;
    virtual double getIyGrossSection(void) const= 0;
    virtual double getIzGrossSection(void) const= 0;
    virtual double getPyzGrossSection(void) const= 0;

    double getIxGrossSection(void) const;
    double getTheta_pGrossSection(void) const;
    Vector getDirEjeI_aGrossSection(void) const;
    Vector getDirEjeI_bGrossSection(void) const;
    double getI1GrossSection(void) const;
    double getI2GrossSection(void) const;
    EjesPrincInercia2d getEjesInerciaGrossSection(void) const;
    Vector2d getVDirEje1GrossSection(void) const;
    Vector2d getVDirStrongAxisGrossSection(void) const;
    Vector2d getVDirEje2GrossSection(void) const;
    Vector2d getVDirWeakAxisGrossSection(void) const;
    double getIGrossSection(const unsigned short int &,const unsigned short int &) const;
    double getIGrossSection(const unsigned short int &,const unsigned short int &,const Pos2d &) const;
    double getIGrossSection(const Pos2d &,const Vector &) const;
    double getIGrossSection(const Recta2d &) const;
    double getIOGrossSection(const Pos2d &) const;
    Matrix getIGrossSection(void) const;
    Matrix getIGrossSection(const Pos2d &) const;

    virtual double getAreaHomogenizedSection(const double &) const= 0;
    virtual Vector getCdgHomogenizedSection(const double &) const= 0;
    virtual double getIyHomogenizedSection(const double &) const= 0;
    virtual double getIzHomogenizedSection(const double &) const= 0;
    virtual double getPyzHomogenizedSection(const double &) const= 0;

    double getIxHomogenizedSection(const double &) const;
    double getTheta_pHomogenizedSection(void) const;
    Vector getDirEjeI_aHomogenizedSection(void) const;
    Vector getDirEjeI_bHomogenizedSection(void) const;
    double getI1HomogenizedSection(const double &) const;
    double getI2HomogenizedSection(const double &) const;
    EjesPrincInercia2d getEjesInerciaHomogenizedSection(const double &) const;
    Vector2d getVDirEje1HomogenizedSection(const double &) const;
    Vector2d getVDirStrongAxisHomogenizedSection(const double &) const;
    Vector2d getVDirEje2HomogenizedSection(const double &) const;
    Vector2d getVDirWeakAxisHomogenizedSection(const double &) const;
    double getIHomogenizedSection(const double &,const unsigned short int &,const unsigned short int &) const;
    double getIHomogenizedSection(const double &,const unsigned short int &,const unsigned short int &,const Pos2d &) const;
    double getIHomogenizedSection(const double &,const Pos2d &,const Vector &) const;
    double getIHomogenizedSection(const double &,const Recta2d &) const;
    double getIOHomogenizedSection(const double &,const Pos2d &) const;
    Matrix getIHomogenizedSection(const double &) const;
    Matrix getIHomogenizedSection(const double &,const Pos2d &) const;

    CrossSectionProperties3d getCrossSectionProperties3d(const CrossSectionProperties3d &) const;
    CrossSectionProperties2d getCrossSectionProperties2d(const CrossSectionProperties2d &) const;

  };

} // end of XC namespace


#endif

