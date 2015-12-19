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
// YieldSurfaceBC_2D.h: interface for the HingeForceDeformation class.
//
//////////////////////////////////////////////////////////////////////

#ifndef YIELDSURFACE_BC2D_H
#define YIELDSURFACE_BC2D_H

#include "material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h"
#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! \ingroup MATYS
//! @brief Superficie de fluencia 2D.
class YieldSurface_BC2D : public YieldSurface_BC
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
