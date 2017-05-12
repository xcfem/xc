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
//PrismaticBarCrossSection.h
                                                                        
                                                                        
#ifndef PrismaticBarCrossSection_h
#define PrismaticBarCrossSection_h

class PrincipalAxesOfInertia2D;

#include "material/section/SectionForceDeformation.h"

class Semiplano2d;
class Recta2d;
class Pos2d;
class Vector2d;

namespace XC {

class DeformationPlane;

//! \ingroup MATSCC
//
//! @brief Base class for beam-column cross sections.
//! Constitutive equations of the section.
class PrismaticBarCrossSection: public SectionForceDeformation
  {
  public:
    PrismaticBarCrossSection(int tag,int classTag,MaterialLoader *mat_ldr= nullptr);
    PrismaticBarCrossSection(const PrismaticBarCrossSection &otro);
    PrismaticBarCrossSection &operator=(const PrismaticBarCrossSection &otro);

    int setTrialDeformationPlane(const DeformationPlane &);
    int setInitialDeformationPlane(const DeformationPlane &);
    virtual const Vector &getGeneralizedStrainVector(const DeformationPlane &) const;
    DeformationPlane getDeformationPlane(void) const;
    DeformationPlane getInitialDeformationPlane(void) const;
    virtual double getStrain(const double &y,const double &z= 0) const;

    double getN(void) const;
    double getMy(void) const;
    double getMz(void) const;

    virtual double getCdgY(void) const;
    virtual double getCdgZ(void) const;
    Pos2d getCdg(void) const;
    Recta2d getFibraNeutra(void) const;
    Recta2d getEjeEsfuerzos(void) const;
    Pos2d getPointOnTensionedHalfPlane(void) const;
    Pos2d getPointOnCompressedHalfPlane(void) const;
    Semiplano2d getTensionedHalfPlane(void) const;
    Semiplano2d getTensionedHalfPlane(const Recta2d &) const;
    Semiplano2d getCompressedHalfPlane(void) const;
    Semiplano2d getCompressedHalfPlane(const Recta2d &) const;

    const double &EA(void) const;
    const double &EIz(void) const;
    const double &EIy(void) const;
    const double &EIyz(void) const;
    double getTheta(void) const;
    double getEI1(void) const;
    double getEI2(void) const;
    PrincipalAxesOfInertia2D getEjesInercia(void) const;
    Vector2d getVDirEje1(void) const;
    Vector2d getVDirStrongAxis(void) const;
    Vector2d getVDirEje2(void) const;
    Vector2d getVDirWeakAxis(void) const;

    bool hayAxil(const double &tol=1e-4) const;
    bool hayCortante(const double &tol=1e-4) const;
    bool hayMomento(const double &tol=1e-4) const;
    bool hayTorsor(const double &tol=1e-4) const;

  };

} // end of XC namespace


#endif
