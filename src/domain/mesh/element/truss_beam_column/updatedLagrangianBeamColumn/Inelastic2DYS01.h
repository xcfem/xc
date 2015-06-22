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
#ifndef Inelastic2DYS01_H
#define Inelastic2DYS01_H

#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/InelasticYS2DGNL.h"

namespace XC {
//! \ingroup ElemBarra
//
//! @brief ??.
class Inelastic2DYS01: public InelasticYS2DGNL
  {
  private:
    double A;
    double Iz;
    double E;
    double damageFactorEnd1;
    double damageFactorEnd2;
    double fpeak;
  public:
    Inelastic2DYS01(int tag, double A, double E, double Iz,
                    int Nd1, int Nd2,
                    YieldSurface_BC *ysEnd1,  YieldSurface_BC *ysEnd2,
                    int rf_algo=-1, bool islinear=false, double rho=0.0);
    Element *getCopy(void) const;
  protected:
    virtual void getLocalStiff(Matrix &K) const;
  };
} // fin namespace XC

#endif

