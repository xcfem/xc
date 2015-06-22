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
// CombinedIsoKin2D01.h
//
//////////////////////////////////////////////////////////////////////

#if !defined USERDEFINED2D_H
#define USERDEFINED2D_H

#include "PlasticHardening2D.h"

namespace XC {
//! \ingroup MATYS
//! @brief ??.
class CombinedIsoKin2D01: public PlasticHardening2D
  {
  public:
    CombinedIsoKin2D01(int tag, double iso_ratio, double kin_ratio, double shr_iso_ratio, double shr_kin_ratio,double min_iso_factor,PlasticHardeningMaterial &kpx_pos, PlasticHardeningMaterial &kpx_neg, PlasticHardeningMaterial &kpy_pos, PlasticHardeningMaterial &kpy_neg, bool isDeformable, double dir);
    virtual void Print(std::ostream &s, int flag =0);
    virtual YS_Evolution *getCopy(void);
  };
} // fin namespace XC

#endif
