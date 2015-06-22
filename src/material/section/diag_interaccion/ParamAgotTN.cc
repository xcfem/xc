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
//ParamAgotTN.cc

#include "ParamAgotTN.h"
#include "DefAgotPivotes.h"
#include "xc_basic/src/util/matem.h"


//Factor de cumplimiento para la deformación máxima.
//válido si es mayor que uno.
double XC::ParamAgotTN::fc_tracc(void) const
  { return agot_pivotes.getDefAgotPivoteA()/eps_c_max; }

//Factor de cumplimiento para la deformación mínima.
//válido si es mayor que uno.
double XC::ParamAgotTN::fc_comp(void) const
  { return agot_pivotes.getDefAgotPivoteB()/eps_c_min; }

//Factor de cumplimiento para la deformación en el pivote C.
//válido si es mayor que uno.
double XC::ParamAgotTN::fc_pC(void) const
  { 
    if(eps_c_pC < 0)
      return agot_pivotes.getDefAgotPivoteC()/eps_c_pC;
    else
      return 1.0+eps_c_pC;
  }

//Factor de cumplimiento mínimo.
double XC::ParamAgotTN::fc_min(void) const
  { return std::min(fabs(fc_tracc()),std::min(fabs(fc_comp()),fabs(fc_pC()))); }

XC::ParamAgotTN::ParamAgotTN(const DefAgotPivotes &ap,const double &emx,const double &emn,const double &ec)
  : agot_pivotes(ap), eps_c_max(emx), eps_c_min(emn), eps_c_pC(ec) {}

bool XC::ParamAgotTN::Cumple(void) const
  { return (fc_min()>=1.0); }

bool XC::ParamAgotTN::Agotada(void) const
  { return !Cumple(); }
