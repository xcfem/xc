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
// Kinematic2D02.h
//
//////////////////////////////////////////////////////////////////////

#ifndef KINEMATIC2D02_H
#define KINEMATIC2D02_H

#include "material/yieldSurface/evolution/BkStressLimSurface2D.h"
#include "material/yieldSurface/plasticHardeningMaterial/NullPlasticMaterial.h"

//class YieldSurface_BC;
namespace XC {
//! \ingroup MATYS
//!
//! @brief ??
class Kinematic2D02 : public BkStressLimSurface2D
  {
  public:
    Kinematic2D02(int tag, double min_iso_factor, 
				YieldSurface_BC &lim_surface,
				PlasticHardeningMaterial &kpx,
				PlasticHardeningMaterial &kpy,
				int algo, double resfact, double appfact, double dir);

    virtual void Print(std::ostream &s, int flag =0);
    virtual YS_Evolution *getCopy(void);

    static NullPlasticMaterial nullMat;
  };
} // end of XC namespace

#endif
