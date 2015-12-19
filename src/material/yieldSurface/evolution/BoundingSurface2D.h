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
// BoundingSurface2D.h
//
//////////////////////////////////////////////////////////////////////

#ifndef BOUNDINGSURFACE2D_H
#define BOUNDINGSURFACE2D_H

#include "material/yieldSurface/evolution/YS_Evolution2D.h"

namespace XC {
//! \ingroup MATYS
//! @brief Superficie de fluencia de tipo ??.
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
    virtual int 	commitState();
    virtual int		revertToLastCommit(void);
    virtual const   Vector &getEquiPlasticStiffness(void)=0;

    virtual void	Print(std::ostream &s, int flag =0);
    virtual YS_Evolution *getCopy(void)=0;

protected:
    virtual void	setTrialPlasticStrains(double ep, const Vector &f, const Vector &g)=0;
    virtual double getIsoPlasticStiffness(int dir)=0;
    virtual double getKinPlasticStiffness(int dir)=0;
    virtual Vector& getEvolDirection(Vector &f_new)=0;
  };
} // end of XC namespace

#endif
