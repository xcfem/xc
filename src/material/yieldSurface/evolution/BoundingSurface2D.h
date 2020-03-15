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
// BoundingSurface2D.h
//
//////////////////////////////////////////////////////////////////////

#ifndef BOUNDINGSURFACE2D_H
#define BOUNDINGSURFACE2D_H

#include "material/yieldSurface/evolution/YS_Evolution2D.h"

namespace XC {
//! @ingroup MATYS
//! @brief Bounding 2D yield surface.
class BoundingSurface2D : public YS_Evolution2D
  {
  protected:
//	double minIsoFactor;
    PlasticHardeningMaterial  *kpMatX, *kpMatY;
    YieldSurface_BC   *boundSurface;
  public:
    BoundingSurface2D(int tag, int classTag, double min_iso_factor, 
    			double iso_ratio, double kin_ratio,
    			PlasticHardeningMaterial &kpx,
    			PlasticHardeningMaterial &kpy,
    			YieldSurface_BC &bound_surface);

    virtual ~BoundingSurface2D();
    virtual int commitState();
    virtual int	revertToLastCommit(void);
    virtual const Vector &getEquiPlasticStiffness(void)=0;

    virtual void Print(std::ostream &s, int flag =0) const;
    virtual YS_Evolution *getCopy(void)=0;

protected:
    virtual void setTrialPlasticStrains(double ep, const Vector &f, const Vector &g)=0;
    virtual double getIsoPlasticStiffness(int dir)=0;
    virtual double getKinPlasticStiffness(int dir)=0;
    virtual Vector& getEvolDirection(Vector &f_new)=0;
  };
} // end of XC namespace

#endif
