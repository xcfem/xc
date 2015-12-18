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

//! \ingroup MATYS
//!
//! @brief Evolución de una superficie de fluencia.
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
    virtual void Print(std::ostream &s, int flag =0);
//	friend std::ostream &operator<<(std::ostream &s, const YS_Evolution &hModel);

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
      
    virtual double getTrialPlasticStrains(int dof)=0;
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
