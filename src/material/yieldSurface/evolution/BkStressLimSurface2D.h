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
// BkStressLimSurface2D.h
//
//////////////////////////////////////////////////////////////////////

#ifndef BKSTRESSLIMSURFACE2D_H
#define BKSTRESSLIMSURFACE2D_H

#include "material/yieldSurface/evolution/YS_Evolution2D.h"
namespace XC {
class YieldSurface_BC;
class PlasticHardeningMaterial;
  
//! @ingroup YSEvolution
//!
//! @brief  Back-strees isotropic model evolution law.
class BkStressLimSurface2D: public YS_Evolution2D
  {
  protected:
	virtual void  setTrialPlasticStrains(double ep, const Vector &f, const Vector &g);
	virtual double getIsoPlasticStiffness(int dir);
	virtual double getKinPlasticStiffness(int dir);
	virtual Vector& getEvolDirection(Vector &f_new);
	
	PlasticHardeningMaterial  *kinMatX, *kinMatY;
	PlasticHardeningMaterial  *isoMatXPos, *isoMatXNeg;
	PlasticHardeningMaterial  *isoMatYPos, *isoMatYNeg;
	
	YieldSurface_BC *limSurface;
	bool defPosX, defPosY;
	bool resHardening, resApproach;
	int resAlgo;
	double resFactor, appFactor;
	double direction, direction_orig;
  public:
    BkStressLimSurface2D(int tag, int classTag, double min_iso_factor, 
				double iso_ratio, double kin_ratio,
				YieldSurface_BC  &lim_surface,
				PlasticHardeningMaterial &kinX,
				PlasticHardeningMaterial &kinY, 
				PlasticHardeningMaterial &isoXPos,
				PlasticHardeningMaterial &isoXNeg,
				PlasticHardeningMaterial &isoYPos,
				PlasticHardeningMaterial &isoYNeg,
				int restype, double res_Fact, double app_Fact, double dir
				);

    virtual ~BkStressLimSurface2D();
    virtual int commitState();
    virtual int	revertToLastCommit(void);

    virtual void Print(std::ostream &s, int flag =0) const;
    void setResidual(double res=1.0);
    virtual YS_Evolution *getCopy(void)=0;
    virtual const Vector &getEquiPlasticStiffness(void);
    double getTrialPlasticStrains(int dof) const;
    double getCommitPlasticStrains(int dof);
  };
} // end of XC namespace

#endif
