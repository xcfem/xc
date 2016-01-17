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
//DefAgotPivotes.cc

#include "DefAgotPivotes.h"
#include "xc_utils/src/base/any_const_ptr.h"

XC::DefAgotPivotes::DefAgotPivotes(const double &eA,const double &eB,const double &eC)
 : eps_agot_A(eA), eps_agot_B(eB), eps_agot_C(eC) {}

const double &XC::DefAgotPivotes::getDefAgotPivoteA(void) const
  { return eps_agot_A; }

const double &XC::DefAgotPivotes::getDefAgotPivoteB(void) const
  { return eps_agot_B; }

const double &XC::DefAgotPivotes::getDefAgotPivoteC(void) const
  { return eps_agot_C; }

void XC::DefAgotPivotes::setDefAgotPivoteA(const double &v)
  { eps_agot_A= v; }
void XC::DefAgotPivotes::setDefAgotPivoteB(const double &v)
  { eps_agot_B= v; }
void XC::DefAgotPivotes::setDefAgotPivoteC(const double &v)
  { eps_agot_C= v; }

const double XC::DefAgotPivotes::getIncEpsAB(const double &ndiv) const
  { return (eps_agot_A-eps_agot_B)/ndiv; }
