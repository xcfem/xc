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
//Kinematic2D01.cpp: implementation of the YS_HardeningModel class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/evolution/Kinematic2D01.h"
#include <math.h>

#define evolDebug 0
#define KINEMATIC2D01_CLASSTAG -1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::Kinematic2D01::Kinematic2D01(int tag, double min_iso_factor,
                        PlasticHardeningMaterial &kpx,
						PlasticHardeningMaterial &kpy, double dir)
:PlasticHardening2D(tag, KINEMATIC2D01_CLASSTAG, min_iso_factor,0, 1,
                    kpx, kpx, kpy, kpy, dir){}

XC::YS_Evolution *XC::Kinematic2D01::getCopy(void)
  { return new Kinematic2D01(*this); }

void XC::Kinematic2D01::Print(std::ostream &s, int flag)
  {
	s << "Kinematic2D01 \n";
	s << "iso_Ratio = " << isotropicRatio << "\n";
	s << "isotropicFactor_hist = " << isotropicFactor_hist;
	s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
	s << "\n";
  }
	
