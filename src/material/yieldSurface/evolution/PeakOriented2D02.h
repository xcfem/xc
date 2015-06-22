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
                          PeakOriented2D02.h  -  description
                             -------------------
    begin                : Fri Jul 12 2002
    email                : rkaul@ce-blume215-pent-2.stanford.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef PEAKORIENTED2D02_H
#define PEAKORIENTED2D02_H

#include <material/yieldSurface/evolution/BkStressLimSurface2D.h>
#include <material/yieldSurface/plasticHardeningMaterial/NullPlasticMaterial.h>

/** Grows/ shrinks one side while keeping the diametrically opposite end 
the same.  Kp_isoXPos = Kp_isoXNeg = (should be) Kp_kinX
Since kinematic is function of back-stress and
isotropic is a function of plastic-strains - calibration
will be difficult.

Therefore, this is a pseudo PeakOriented model, for true
PeakOriented, use PeakOriented01

isotropic:kinematic ratio = 0.5:0.5
  *@author rkaul
  */

namespace XC {
//! \ingroup MATYS
//
//! @brief Grows/shrinks one side while keeping the diametrically opposite end
//! the same.  Kp_isoXPos = Kp_isoXNeg = (should be) Kp_kinX
//! Since kinematic is function of back-stress and
//! isotropic is a function of plastic-strains - calibration
//! will be difficult.

//! Therefore, this is a pseudo PeakOriented model, for true
//! PeakOriented, use PeakOriented01

//! isotropic:kinematic ratio = 0.5:0.5
class PeakOriented2D02 : public BkStressLimSurface2D
  {
  private:
    static NullPlasticMaterial nullMat;
  public: 
    PeakOriented2D02(int tag, double min_iso_factor,
                     YieldSurface_BC &lim_surface,
		PlasticHardeningMaterial &kinX,
		PlasticHardeningMaterial &kinY,
		PlasticHardeningMaterial &isoX,
					PlasticHardeningMaterial &isoY,
					int algo
					);
	YS_Evolution * getCopy();
	void Print(std::ostream & s, int flag);
  };
} // fin namespace XC

#endif
