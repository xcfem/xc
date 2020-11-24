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
// HingeForceDeformation.h: interface for the HingeForceDeformation class.
//
//////////////////////////////////////////////////////////////////////

#ifndef YIELDSURFACE_BC_H
#define YIELDSURFACE_BC_H


#include "utility/tagged/TaggedObject.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {
class Information;
class Response;
class YS_Evolution;
class ID;

//! @defgroup YS_BC Interface between yield surfaces and beam-column elements.
//! @ingroup MATYS

//! @brief YieldSurface_BC is an abstract class and provides the interface
//! between the specific yield surface description and the beam-column
//! element. It implements methods to transform vectors and matrices between
//! the element system and local yield-surface coordinates.
//! @ingroup YS_BC
class YieldSurface_BC: public TaggedObject, public MovableObject
  {
  private:
    int checkT(void);

  public:
    YS_Evolution *hModel;

  protected:
    ID *T;
    ID *S;
    double capX_orig, capY_orig, capZ_orig;
    double capX, capY, capZ;
    double capXdim, capYdim, capZdim;
    int dimension;
    bool isLoading;

  public:
    int ele_Tag, ele_Location;
    const static int dFReturn, RadialReturn, ConstantXReturn, ConstantYReturn;
    const static int NoFP, SurfOnly, StateLoading;
  public:
    YieldSurface_BC(int tag, int classtag, YS_Evolution &model,double capx);
    YieldSurface_BC(int tag, int classtag, YS_Evolution &model,double capx, double capy);
    YieldSurface_BC(int tag, int classtag, YS_Evolution &model,double capx, double capy, double capz);
    virtual ~YieldSurface_BC();

    virtual void Print(std::ostream &s, int flag =0) const;
    void    setEleInfo(int eleTag, int loc);
    // keep transformation virtual
    virtual void    setTransformation(int xDof, int xFact);
     virtual void    setTransformation(int xDof, int yDof, int xFact, int yFact);
     virtual void    setTransformation(int xDof, int yDof, int zDof, int xFact, int yFact, int zFact);

    // in element system
    virtual void getCommitGradient(Matrix &G) = 0;
    virtual void getTrialGradient(Matrix &G, Vector &force) = 0;
    virtual double getTrialDrift(Vector &force) = 0;

//    virtual void  setExtent()=0;
    virtual const Vector &getExtent(void)=0;
    virtual int update(int flag = 0);

    // in ys system
    double getCap(int dir);
    virtual Vector &translationTo(Vector &f_new, Vector &f_dir)=0;
    virtual int getState(int stateInfo)=0;

    virtual double getDrift(double x1);
    virtual double getDrift(double x1, double y1);
    virtual double getDrift(double x1, double y1, double z1);

    // needed by evlution model
    virtual double interpolate(double x1, double x2);
    virtual double interpolate(double x1, double y1, double x2, double y2);
    virtual double interpolate(double x1, double y1, double z1, double x2, double y2, double z2);

    virtual int getTrialForceLocation(Vector &force)=0;
    virtual int getCommitForceLocation()=0;

    virtual void addPlasticStiffness(Matrix &K)=0;

    virtual double setToSurface(Vector &force, int algoType, int flag=0)=0;
    virtual int modifySurface(double magPlasticDefo, Vector &Fsurface, Matrix &G, int flag=0)=0;

    virtual int commitState(Vector &force);
    virtual int revertToLastCommit(void)=0;

    virtual YieldSurface_BC *getCopy(void) = 0;

protected:
    void toLocalSystem  (Vector &eleVector, double &x, bool nonDimensionalize, bool signMult=true);
    void toLocalSystem  (Vector &eleVector, double &x, double &y, bool nonDimensionalize, bool signMult=true);
    void toLocalSystem  (Vector &eleVector, double &x, double &y, double &z, bool nonDimensionalize, bool signMult=true);

    // matrix do not multiply G!
    void toLocalSystem  (Matrix &eleMatrix, double &x, bool nonDimensionalize, bool signMult=true);
    void toLocalSystem  (Matrix &eleMatrix, double &x, double &y, bool nonDimensionalize, bool signMult=true);
    void toLocalSystem  (Matrix &eleMatrix, double &x, double &y, double &z, bool nonDimensionalize, bool signMult=true);

    void toElementSystem(Vector &eleVector, double &x, bool dimensionalize, bool signMult=true);
    void toElementSystem(Vector &eleVector, double &x, double &y, bool dimensionalize, bool signMult=true);
    void toElementSystem(Vector &eleVector, double &x, double &y, double &z, bool dimensionalize, bool signMult=true);

    void toElementSystem(Matrix &eleMatrix, double &x, bool dimensionalize, bool signMult=true);
    void toElementSystem(Matrix &eleMatrix, double &x, double &y, bool dimensionalize, bool signMult=true);
    void toElementSystem(Matrix &eleMatrix, double &x, double &y, double &z, bool dimensionalize, bool signMult=true);
};
} // end of XC namespace

/*
enum Fstate { orig, trans };

class ysfp
{
public:
    ysfp(double x, enum Fstate);
    ysfp(double x, double y, enum Fstate);
    ysfp(double x, double y, double z, enum Fstate);

    double getOrig(int dof);
    double getTrans(int dof);
    //setState..
private:
    bool orig, trans;

    Vector *F0;
    Vector *Ft;
};
*/

#endif
