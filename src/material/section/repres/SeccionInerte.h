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

    virtual double getAreaSeccBruta(void) const= 0;
    virtual Vector getCdgSeccBruta(void) const= 0;
    virtual double getIySeccBruta(void) const= 0;
    virtual double getIzSeccBruta(void) const= 0;
    virtual double getPyzSeccBruta(void) const= 0;

    double getIxSeccBruta(void) const;
    double getTheta_pSeccBruta(void) const;
    Vector getDirEjeI_aSeccBruta(void) const;
    Vector getDirEjeI_bSeccBruta(void) const;
    double getI1SeccBruta(void) const;
    double getI2SeccBruta(void) const;
    EjesPrincInercia2d getEjesInerciaSeccBruta(void) const;
    Vector2d getVDirEje1SeccBruta(void) const;
    Vector2d getVDirEjeFuerteSeccBruta(void) const;
    Vector2d getVDirEje2SeccBruta(void) const;
    Vector2d getVDirEjeDebilSeccBruta(void) const;
    double getISeccBruta(const unsigned short int &,const unsigned short int &) const;
    double getISeccBruta(const unsigned short int &,const unsigned short int &,const Pos2d &) const;
    double getISeccBruta(const Pos2d &,const Vector &) const;
    double getISeccBruta(const Recta2d &) const;
    double getIOSeccBruta(const Pos2d &) const;
    Matrix getISeccBruta(void) const;
    Matrix getISeccBruta(const Pos2d &) const;

    virtual double getAreaSeccHomogeneizada(const double &) const= 0;
    virtual Vector getCdgSeccHomogeneizada(const double &) const= 0;
    virtual double getIySeccHomogeneizada(const double &) const= 0;
    virtual double getIzSeccHomogeneizada(const double &) const= 0;
    virtual double getPyzSeccHomogeneizada(const double &) const= 0;

    double getIxSeccHomogeneizada(const double &) const;
    double getTheta_pSeccHomogeneizada(void) const;
    Vector getDirEjeI_aSeccHomogeneizada(void) const;
    Vector getDirEjeI_bSeccHomogeneizada(void) const;
    double getI1SeccHomogeneizada(const double &) const;
    double getI2SeccHomogeneizada(const double &) const;
    EjesPrincInercia2d getEjesInerciaSeccHomogeneizada(const double &) const;
    Vector2d getVDirEje1SeccHomogeneizada(const double &) const;
    Vector2d getVDirEjeFuerteSeccHomogeneizada(const double &) const;
    Vector2d getVDirEje2SeccHomogeneizada(const double &) const;
    Vector2d getVDirEjeDebilSeccHomogeneizada(const double &) const;
    double getISeccHomogeneizada(const double &,const unsigned short int &,const unsigned short int &) const;
    double getISeccHomogeneizada(const double &,const unsigned short int &,const unsigned short int &,const Pos2d &) const;
    double getISeccHomogeneizada(const double &,const Pos2d &,const Vector &) const;
    double getISeccHomogeneizada(const double &,const Recta2d &) const;
    double getIOSeccHomogeneizada(const double &,const Pos2d &) const;
    Matrix getISeccHomogeneizada(const double &) const;
    Matrix getISeccHomogeneizada(const double &,const Pos2d &) const;

    CrossSectionProperties3d getCrossSectionProperties3d(const CrossSectionProperties3d &) const;
    CrossSectionProperties2d getCrossSectionProperties2d(const CrossSectionProperties2d &) const;

  };

} // end of XC namespace


#endif

