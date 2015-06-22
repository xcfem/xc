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
//SetFilaI.h

#ifndef SETFILAI_H
#define SETFILAI_H

#include "SetFila.h"
#include "modelador/cad/matrices/TritrizPtrNod.h"
#include "modelador/cad/matrices/TritrizPtrElem.h"

class RangoIndice;

namespace XC {

class EntMdlr;

//!  \ingroup Set
//! 
//!  \brief Conjunto de objetos.
//! 
//!  Un objeto SetFilaI contiene un conjunto de 0 o más:
//!  - Nodos.
//!  - Elementos finitos.
//!  que corresponden a una fila_i de un objeto EntMdlr.
class SetFilaI: public SetFila<TritrizPtrNod::var_ref_fila_i,TritrizPtrElem::var_ref_fila_i>
  {
  public:
    typedef TritrizPtrNod::var_ref_fila_i tfilanod;
    typedef TritrizPtrElem::var_ref_fila_i tfilaelem;
    SetFilaI(EntMdlr &e,const size_t &f=1,const size_t &c=1,const std::string &nmb="",Modelador *mdlr= NULL);
    SetFilaI(EntMdlr &e,const RangoIndice &rango_capas,const size_t &f,const size_t &c,const std::string &nmb="",Modelador *mdlr= NULL);
  };
} //fin namespace XC
#endif
