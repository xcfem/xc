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
//Kinematic2D02
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/evolution/Kinematic2D02.h"
#include <math.h>
//#include "material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h"
 
#define evolDebug 0
#define KINEMATIC2D02_CLASSTAG -1

 XC::NullPlasticMaterial XC::Kinematic2D02::nullMat(-1);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::Kinematic2D02::Kinematic2D02(int tag, double min_iso_factor, 
									YieldSurface_BC &lim_surface,
									PlasticHardeningMaterial &kpx,
									PlasticHardeningMaterial &kpy,
									int algo, double resfact, double appfact, double dir)
:BkStressLimSurface2D(tag, KINEMATIC2D02_CLASSTAG, min_iso_factor,0.0, 1.0,
                        lim_surface, kpx, kpy,
                        nullMat, nullMat, nullMat, nullMat, algo, resfact, appfact, dir){}

XC::YS_Evolution *XC::Kinematic2D02::getCopy(void)
  { return new Kinematic2D02(*this); }


void XC::Kinematic2D02::Print(std::ostream &s, int flag)
  {
    s << "Kinematic2D02 \n";
	s << "iso_Ratio = " << isotropicRatio << "\n";
	s << "isotropicFactor_hist = " << isotropicFactor_hist;
	s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
	s << "\n";

}
	
