//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

//! \ingroup Mat
//!
//!
//! \defgroup MATYS Superficies de fluencia.
//!
//! \ingroup MATYS
//! @brief Superficie de fluencia.
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

    virtual void Print(std::ostream &s, int flag =0);
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
} // fin namespace XC

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
