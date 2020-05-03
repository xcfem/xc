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
//SectionMassProperties.h

#ifndef SectionMassProperties_h 
#define SectionMassProperties_h 

#include "xc_utils/src/kernel/CommandEntity.h"

class Pos2d;
class Line2d;
class PrincipalAxesOfInertia2D;
class Vector2d;

namespace XC {
class CrossSectionProperties3d;
class CrossSectionProperties2d;
class CrossSectionProperties1d;
class Vector;
class Matrix;

//! @ingroup MATSCCRepres
//
//! @brief Cross-section representation able to
//! return mechanical propertis a area, moments of inertia,...
class SectionMassProperties: public CommandEntity
  {
  public:
    SectionMassProperties(CommandEntity *owr= nullptr);

    virtual double getAreaGrossSection(void) const= 0;
    virtual Vector getCenterOfMassGrossSection(void) const= 0;
    virtual double getIyGrossSection(void) const= 0;
    virtual double getIzGrossSection(void) const= 0;
    virtual double getPyzGrossSection(void) const= 0;

    double getIxGrossSection(void) const;
    double getTheta_pGrossSection(void) const;
    Vector getIAxisDir_aGrossSection(void) const;
    Vector getIAxisDir_bGrossSection(void) const;
    double getI1GrossSection(void) const;
    double getI2GrossSection(void) const;
    PrincipalAxesOfInertia2D getInertiaAxesGrossSection(void) const;
    Vector2d getAxis1VDirGrossSection(void) const;
    Vector2d getVDirStrongAxisGrossSection(void) const;
    Vector2d getAxis2VDirGrossSection(void) const;
    Vector2d getVDirWeakAxisGrossSection(void) const;
    double getIGrossSection(const unsigned short int &,const unsigned short int &) const;
    double getIGrossSection(const unsigned short int &,const unsigned short int &,const Pos2d &) const;
    double getIGrossSection(const Pos2d &,const Vector &) const;
    double getIGrossSection(const Line2d &) const;
    double getIOGrossSection(const Pos2d &) const;
    Matrix getIGrossSection(void) const;
    Matrix getIGrossSection(const Pos2d &) const;

    virtual double getAreaHomogenizedSection(const double &) const= 0;
    virtual Vector getCenterOfMassHomogenizedSection(const double &) const= 0;
    virtual double getIyHomogenizedSection(const double &) const= 0;
    virtual double getIzHomogenizedSection(const double &) const= 0;
    virtual double getPyzHomogenizedSection(const double &) const= 0;

    double getIxHomogenizedSection(const double &) const;
    double getTheta_pHomogenizedSection(void) const;
    Vector getIAxisDir_aHomogenizedSection(void) const;
    Vector getIAxisDir_bHomogenizedSection(void) const;
    double getI1HomogenizedSection(const double &) const;
    double getI2HomogenizedSection(const double &) const;
    PrincipalAxesOfInertia2D getInertiaAxesHomogenizedSection(const double &) const;
    Vector2d getAxis1VDirHomogenizedSection(const double &) const;
    Vector2d getVDirStrongAxisHomogenizedSection(const double &) const;
    Vector2d getAxis2VDirHomogenizedSection(const double &) const;
    Vector2d getVDirWeakAxisHomogenizedSection(const double &) const;
    double getIHomogenizedSection(const double &,const unsigned short int &,const unsigned short int &) const;
    double getIHomogenizedSection(const double &,const unsigned short int &,const unsigned short int &,const Pos2d &) const;
    double getIHomogenizedSection(const double &,const Pos2d &,const Vector &) const;
    double getIHomogenizedSection(const double &,const Line2d &) const;
    double getIOHomogenizedSection(const double &,const Pos2d &) const;
    Matrix getIHomogenizedSection(const double &) const;
    Matrix getIHomogenizedSection(const double &,const Pos2d &) const;

    CrossSectionProperties3d getCrossSectionProperties3d(const CrossSectionProperties3d &) const;
    CrossSectionProperties2d getCrossSectionProperties2d(const CrossSectionProperties2d &) const;
    CrossSectionProperties1d getCrossSectionProperties1d(const CrossSectionProperties1d &) const;

  };

} // end of XC namespace


#endif

