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
//FiberSets.h

#ifndef FiberSets_h
#define FiberSets_h

#include "DqFibras.h"

class Poligono2d;

namespace XC {

//! \ingroup MATSCCFibras
//
//! @brief Clase base para las secciones discretizadas en fibras.
class FiberSets: public std::map<std::string,DqFibras>, public EntCmd
  {
  public:
    FiberSets(void); 

    DqFibras &crea_set_fibras(const std::string &nombre);
    iterator get_set_fibras(const std::string &nmb_set);
    iterator sel_mat_tag(DqFibras &fibras, const std::string &nmb_set,const int &matTag);
    iterator resel_mat_tag(const std::string &nmb_set,const std::string &nmb_set_org,const int &matTag);
  };
} // end of XC namespace

#endif
