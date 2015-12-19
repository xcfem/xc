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
// MultiLinear.h: interface for the MultiLinear class.
//
//////////////////////////////////////////////////////////////////////


#ifndef ExponReducing_h
#define ExponReducing_h

#include "material/yieldSurface/plasticHardeningMaterial/PlasticHardeningMaterial.h"

namespace XC {
//! @ingroup Mat
//
//! @brief XXX??
class ExponReducing : public PlasticHardeningMaterial
  {
  private:
    double Kp0;
    double alpha;
    double resFactor;
  public:
    ExponReducing(int tag);
    ExponReducing(int tag, double kp0, double alfa);
    ExponReducing(int tag, double kp0, double alfa, double res_fact);

    double getTrialPlasticStiffness();
    PlasticHardeningMaterial *getCopy(void);
    void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace

#endif
