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

#ifndef ElementEdge_h
#define ElementEdge_h

#include <cstdlib>
#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {

class Element;
class ElementEdgeLoad;

//! \ingroup Elem
//
//! @brief Borde (o arista) de un elemento.
 class ElementEdge: public EntCmd
  {
    Element *elem; //! Elemento al que pertenece el borde.
    int iedge; //! índice del borde.
  protected:

  public:
    ElementEdge(Element *eptr= NULL,const int &i=-1);
    const Element *getElementPtr(void) const;
    const int &getEdgeIndex(void) const;

    virtual int addLoad(ElementEdgeLoad *theLoad, double loadFactor);
  };
} // end of XC namespace

#endif

