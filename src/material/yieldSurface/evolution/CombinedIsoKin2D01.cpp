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
//CombinedIsoKin2D01.cpp: implementation of the YS_HardeningModel class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/evolution/CombinedIsoKin2D01.h"
#include <math.h>

#define evolDebug 0
#define COMBINEDISOKIN2D01_CLASSTAG -1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::CombinedIsoKin2D01::CombinedIsoKin2D01(int tag,
				  double iso_ratio, double kin_ratio,
                  double shr_iso_ratio, double shr_kin_ratio,
				  double min_iso_factor,
                  PlasticHardeningMaterial &kpx_pos,
                  PlasticHardeningMaterial &kpx_neg,
				  PlasticHardeningMaterial &kpy_pos,
				  PlasticHardeningMaterial &kpy_neg,				  
				  bool isDeformable, double dir)
:PlasticHardening2D(tag, COMBINEDISOKIN2D01_CLASSTAG, min_iso_factor,
                    iso_ratio, kin_ratio, kpx_pos, kpx_neg, kpy_pos, kpy_neg, dir)
{
	deformable = isDeformable;
	isotropicRatio_shrink = shr_iso_ratio;
	kinematicRatio_shrink = shr_kin_ratio;
}

XC::YS_Evolution *XC::CombinedIsoKin2D01::getCopy(void)
  { return new CombinedIsoKin2D01(*this); }

void XC::CombinedIsoKin2D01::Print(std::ostream &s, int flag)
{
	s << "CombinedIsoKin2D01 \n";
	s << "iso_Ratio = " << isotropicRatio << "\n";
	s << "isotropicFactor_hist = " << isotropicFactor_hist;
	s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
	s << "\n";

}
	
