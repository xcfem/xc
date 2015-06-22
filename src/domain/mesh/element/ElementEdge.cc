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

#include "ElementEdge.h"
#include "Element.h"
#include "domain/load/ElementEdgeLoad.h"
#include <iostream>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::ElementEdge::ElementEdge(Element *eptr,const int &i)
  :elem(eptr),iedge(i) {}

//! @brief Lee comandos para un objeto ElementEdge
bool XC::ElementEdge::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ElementEdge) Procesando comando: " << cmd << std::endl;

    if(cmd=="vector3d_load_global")
      {
        const Vector v= Vector(convert_to_vector_double(interpretaVectorAny(status.GetBloque())));
        elem->defEdgeLoadGlobal(iedge,v);        
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
  }

const XC::Element *XC::ElementEdge::getElementPtr(void) const
  { return elem; }

const int &XC::ElementEdge::getEdgeIndex(void) const
  { return iedge; }

int XC::ElementEdge::addLoad(ElementEdgeLoad *theLoad, double loadFactor)
  {
    elem->addLoad(theLoad,loadFactor);
    return -1;
  }
