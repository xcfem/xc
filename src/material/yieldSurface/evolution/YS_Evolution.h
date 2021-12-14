// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
// YS_Evolution.h
//
//////////////////////////////////////////////////////////////////////

#ifndef YS_EVOLUTION_H
#define YS_EVOLUTION_H

#include "utility/tagged/TaggedObject.h"
#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"

namespace XC {
class Information;
class Response;
class YieldSurface_BC;

// YSEvolution group.
//! @defgroup YSEvolution Yield surface evolution laws.
//! @ingroup MATYS

//! @brief Yield surface evolution law.
//! @ingroup YSEvolution
class YS_Evolution: public TaggedObject, public MovableObject
  {
  public:
    bool  freezeEvolution;
  private:
    void checkDimension(int dir);
    void toDeformedCoord(Vector &coord);
    void toOriginalCoord(Vector &coord);
  protected:
    //double  isotropicFactor_hist, isotropicFactor; // Effective magnification
    bool   deformable;
    Vector isotropicFactor_hist, isotropicFactor;
    Vector translate_hist, translate;
    Vector translate_init;
    double isotropicRatio_orig,  isotropicRatio, isotropicRatio_shrink;
    double kinematicRatio_orig,  kinematicRatio, kinematicRatio_shrink;
    int    dimension;
    static Vector crd1, crd2, crd3;
  public:
    YS_Evolution(int tag, int classTag, double iso_ratio, double kin_ratio,
    			 int _dimension, double shr_iso=0.5, double shr_kin=0.5);
//  Methods inherited
    virtual void Print(std::ostream &s, int flag =0) const;

    virtual int	update(int flag=0);
    virtual int commitState(void);
    virtual int	revertToLastCommit(void);
    virtual YS_Evolution *getCopy(void) = 0;

    virtual Response *setResponse(const std::vector<std::string> &argv, Information &info)=0;
    virtual int	getResponse(int responseID, Information &info)=0;

// Public methods called by the yield surface
    virtual int	evolveSurface(YieldSurface_BC *ys, double magPlasticDefo, Vector &G, Vector &F_Surface, int flag=0)=0;

    // needed by ys->add Kp
    virtual const Vector &getEquiPlasticStiffness(void)=0;
      
    virtual double getTrialPlasticStrains(int dof) const=0;
    virtual double getCommitPlasticStrains(int dof)=0;
    const Vector &getGeneralizedStress(void) const;
    const Vector &getGeneralizedStrain(void) const;

    virtual void setResidual(double res=1.0);
    void setInitTranslation(Vector &initTranslate);
    const Vector &getInitTranslation(void);

    double getCommitTranslation(int dof);
    double getTrialTranslation(int dof);
    double getTrialIsotropicFactor(int dof);
    double getCommitIsotropicFactor(int dof);
    
    void setDeformable(bool defo);

    void toDeformedCoord(double &x);
    void toDeformedCoord(double &x, double &y);
    void toDeformedCoord(double &x, double &y, double &z);

    void toOriginalCoord(double &x);
    void toOriginalCoord(double &x, double &y);
    void toOriginalCoord(double &x, double &y, double &z);
  };
} // end of XC namespace

#endif
