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
//InteractionDiagramData.cc

#include "InteractionDiagramData.h"


XC::InteractionDiagramData::InteractionDiagramData(void)
  : umbral(10), inc_eps(0.0), inc_t(M_PI/4), agot_pivotes(),
    nmb_set_hormigon("hormigon"), tag_hormigon(0),
    nmb_set_armadura("armadura"), tag_armadura(0)
  {
    inc_eps= agot_pivotes.getIncEpsAB(); //Incremento de tensiones.
    if(inc_eps<=1e-6)
      std::cerr << "El incremento para deformaciones es muy pequeño o negativo: " << inc_eps << std::endl; 
  }

XC::InteractionDiagramData::InteractionDiagramData(const double &u,const double &inc_e,const double &inc_theta,const DefAgotPivotes &agot)
  : umbral(u), inc_eps(inc_e), inc_t(inc_theta), agot_pivotes(agot),
    nmb_set_hormigon("hormigon"), tag_hormigon(0),
    nmb_set_armadura("armadura"), tag_armadura(0) {}
