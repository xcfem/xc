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
// YieldSurfaceBC_2D.h: interface for the HingeForceDeformation class.
//
//////////////////////////////////////////////////////////////////////

#ifndef YIELDSURFACE_BC2D_H
#define YIELDSURFACE_BC2D_H

#include "material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h"
#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! \ingroup MATYS
//! @brief BC 2D yield surface.
class YieldSurface_BC2D: public YieldSurface_BC
  {
  protected:
//        UniaxialMaterial  *kpMatX, *kpMatY;
    double xPos, xNeg, yPos, yNeg;          // Extent along X and Y
    double a1, b1, a2, b2, a3, b3, a4, b4;  // y = ax +b -> inner quad
    double offset, increment;
//    double isotropicRatio;          // Ratio of the plastic deformation that
                                    // is isotropic
/////////////////////////////////////////////////////////////////////////////
//  state variables
/////////////////////////////////////////////////////////////////////////////
//    double translateX, translateY;  // Kinematic displacement
//    double isotropicFactor;         // Magnification factor
//    double sumPlasticDeformX, sumPlasticDeformY;

//    double translateX_hist, translateY_hist;
    int status_hist;
    int state;
//    double isotropicFactor_hist;
//    double sumPlasticDeformX_hist, sumPlasticDeformY_hist;
    double fx_hist, fy_hist, gx_hist, gy_hist;
    double fx_trial, fy_trial, gx_trial, gy_trial;

    static Vector v6;
    static double  error;
    static Vector v2;
    static Vector g2;
    static Vector v4;
    static Vector T2;
    static Vector F2;
  public:
    YieldSurface_BC2D(int tag, int classTag, double xmax, double ymax,YS_Evolution &);



//    virtual Response *setResponse(const std::vector<std::string> &argv, Information &info);
//    virtual int getResponse(int responseID, Information &info);
    virtual void Print(std::ostream &s, int flag =0)=0;
    virtual int sendSelf(CommParameters &){return -1;}
    virtual int recvSelf(const CommParameters &){return -1;}

    virtual void setTransformation(int xDof, int yDof, int xFact, int yFact);

    virtual void getCommitGradient(Matrix &G);
    virtual void getTrialGradient(Matrix &G, Vector &force);
    //virtual const   Vector &getGradient(void);
    //virtual const   Vector &getTrialGradient(void);

    virtual int update(int flag = 0);
    virtual int getState(int stateInfo);
    virtual double  getTrialDrift(Vector &force);
    virtual int getTrialForceLocation(Vector &force);
    virtual int getCommitForceLocation();
    //virtual const   Vector &getForce(void);
    //virtual const   Vector &getTrialForce(void);

    //virtual int setTrialForce(Vector &force);

    // double  getIsotropicFactor(void){ return hModel->getIsotropicFactor();}

    virtual void addPlasticStiffness(Matrix &K);
//    virtual void checkState(Vector &trialforce, bool &plastify, bool &shootsthrough);

    virtual double        setToSurface(Vector &force, int algoType, int colorFlag = 0);
    virtual int modifySurface(double magPlasticDefo, Vector &Fsurface, Matrix &G, int flag=0);
    //virtual int trialModifySurface(double magPlasticDefo);
    //virtual double        getElasticForce(Vector &force, Vector &elasticForce);

    virtual int commitState(Vector &force);
    virtual int revertToLastCommit(void);

    virtual YieldSurface_BC *getCopy(void) = 0;
//protected:
    virtual Vector &translationTo(Vector &f_new, Vector &f_dir);
    virtual double getDrift(double x, double y);
//  For the following 2 methods, x, y already non-dimensionalized
    virtual void getGradient(double &gx, double &gy, double x, double y)=0;
    virtual double        getSurfaceDrift(double x, double y)=0;
    virtual void setExtent()=0;
    virtual const   Vector &getExtent(void);

    virtual int forceLocation(double drift);
    virtual double interpolate(double xi, double yi, double xj, double yj);
    virtual void customizeInterpolate(double &xi, double &yi, double &xj, double &yj);

    double        interpolateClose(double xi, double yi, double xj, double yj);
//  Dimensionalizing taken care at Element System <--> Local System level
//                    void toDeformedCoord(double &x, double &y);
//                    void toOriginalCoord(double &x, double &y);
  };
} // end of XC namespace

#endif
