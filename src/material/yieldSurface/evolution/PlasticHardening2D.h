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
// PlasticHardening2D.h
//
//////////////////////////////////////////////////////////////////////

#if !defined PLASTICHARDENING2D_H
#define PLASTICHARDENING2D_H

#include "material/yieldSurface/evolution/YS_Evolution2D.h"

namespace XC {
//! \ingroup MATYS
//!
//! @brief ??
class PlasticHardening2D : public YS_Evolution2D
  {
  protected:
// double minIsoFactor;
    PlasticHardeningMaterial  *kpMatXPos, *kpMatYPos;
    PlasticHardeningMaterial  *kpMatXNeg, *kpMatYNeg;
    bool defPosX, defPosY;
    double direction;
// bool pinchX;  
public:
    PlasticHardening2D(int tag, int classTag, double min_iso_factor, 
       double iso_ratio, double kin_ratio,
        PlasticHardeningMaterial &kpx_pos,
        PlasticHardeningMaterial &kpx_neg,
        PlasticHardeningMaterial &kpy_pos,
        PlasticHardeningMaterial &kpy_neg, double dir);

    virtual ~PlasticHardening2D();
    
    virtual int  commitState();
    virtual int  revertToLastCommit(void);

    virtual void Print(std::ostream &s, int flag =0);
    virtual YS_Evolution *getCopy(void)=0;
    virtual const Vector &getEquiPlasticStiffness(void);
    double getTrialPlasticStrains(int dof);
    double getCommitPlasticStrains(int dof);
    
protected:
    virtual void setTrialPlasticStrains(double ep, const Vector &f, const Vector &g);
    virtual double getIsoPlasticStiffness(int dir);
    virtual double getKinPlasticStiffness(int dir);
    virtual Vector& getEvolDirection(Vector &f_new);
  };
} // end of XC namespace

#endif
