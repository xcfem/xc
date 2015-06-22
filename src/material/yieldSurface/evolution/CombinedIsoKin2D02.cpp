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
/***************************************************************************
                          CombinedIsoKin2D02.cpp  -  description
                             -------------------
    begin                : Fri Jul 12 2002
    email                : rkaul@ce-blume215-pent-2.stanford.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#include "material/yieldSurface/evolution/CombinedIsoKin2D02.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::CombinedIsoKin2D02::CombinedIsoKin2D02(int tag,  double min_iso_factor,
				double iso_ratio, double kin_ratio,
				YieldSurface_BC  &lim_surface,
				PlasticHardeningMaterial &kinX,
				PlasticHardeningMaterial &kinY,
				PlasticHardeningMaterial &isoXPos,
				PlasticHardeningMaterial &isoXNeg,
				PlasticHardeningMaterial &isoYPos,
				PlasticHardeningMaterial &isoYNeg,
				bool isDeformable,
				int  algo, double resfact, double appfact, double dir)
:BkStressLimSurface2D(tag, -1, min_iso_factor,
				iso_ratio, kin_ratio,
				lim_surface, kinX, kinY,
				isoXPos, isoXNeg, isoYPos, isoYNeg, algo, resfact, appfact, dir)
{
	deformable = isDeformable;
}

XC::YS_Evolution * XC::CombinedIsoKin2D02::getCopy()
  { return new CombinedIsoKin2D02(*this); }


void XC::CombinedIsoKin2D02::Print(std::ostream & s, int flag)
{
	s << "CombinedIsoKin2D02 \n";
	s << "iso_Ratio = " << isotropicRatio << "\n";
	s << "isotropicFactor_hist = " << isotropicFactor_hist;
	s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
	s << "\n";

}
