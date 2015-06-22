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
//BoundingSurface2D
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/evolution/BoundingSurface2D.h"
#include <math.h>
#include <material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h>

#define evolDebug 0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::BoundingSurface2D::BoundingSurface2D(int tag, int classTag, double min_iso_factor,
						double iso_ratio, double kin_ratio,
                        PlasticHardeningMaterial &kpx,
						PlasticHardeningMaterial &kpy,
						YieldSurface_BC &bound_surface)
:YS_Evolution2D(tag, classTag, min_iso_factor, iso_ratio, kin_ratio)
{
	kpMatX = kpx.getCopy();
	kpMatY = kpy.getCopy();
	boundSurface = bound_surface.getCopy();
}

XC::BoundingSurface2D::~BoundingSurface2D()
{
  if (kpMatX != 0)
    delete kpMatX;

  if (kpMatY != 0)
    delete kpMatY;

  if (boundSurface != 0)
    delete boundSurface;
}

int XC::BoundingSurface2D::commitState()
{
	this->XC::YS_Evolution2D::commitState();

    int res  = kpMatX->commitState();
	    res += kpMatY->commitState();

	return res;
}

int XC::BoundingSurface2D::revertToLastCommit(void)
{
	this->XC::YS_Evolution2D::revertToLastCommit();

	kpMatX->revertToLastCommit();
	kpMatY->revertToLastCommit();

	return 0;
}

const XC::Vector &XC::BoundingSurface2D::getEquiPlasticStiffness(void)
{
// find the conjugate point
// find "dR"
// find Kp
	  {
		v2(0) = 0;
		v2(1) = 0;
	  }
	return v2;
}

void    XC::BoundingSurface2D::setTrialPlasticStrains(double ep, const XC::Vector &f, const XC::Vector &g)
{
// set for isotropic materials	
	
}

double  XC::BoundingSurface2D::getIsoPlasticStiffness(int dir)
{
	return 0;
}

double  XC::BoundingSurface2D::getKinPlasticStiffness(int dir)
{
	return 0;	
}

XC::Vector& XC::BoundingSurface2D::getEvolDirection(XC::Vector &f_new)
{
// find the conjugate point
// return that as dir	
	return v2;
}


void XC::BoundingSurface2D::Print(std::ostream &s, int flag)
{
	s << "BoundingSurface2D \n";
	s << "iso_Ratio = " << isotropicRatio << "\n";
	s << "isotropicFactor_hist = " << isotropicFactor_hist;
	s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
	s << "\n";

}
	
