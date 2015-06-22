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
//SetFilaI.cc

#include "SetFilaI.h"
#include "modelador/cad/entidades/EntMdlr.h"
#include <boost/any.hpp>
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

XC::SetFilaI::SetFilaI(EntMdlr &e,const size_t &f,const size_t &c,const std::string &nmb,XC::Modelador *mdlr)
  : SetFila<tfilanod,tfilaelem>(e.GetTtzNodos().GetVarRefFilaI(f,c),e.GetTtzElementos().GetVarRefFilaI(f,c),nmb,mdlr) {}

XC::SetFilaI::SetFilaI(EntMdlr &e,const RangoIndice &rango_capas,const size_t &f,const size_t &c,const std::string &nmb,Modelador *mdlr)
  : SetFila<tfilanod,tfilaelem>(e.GetTtzNodos().GetVarRefFilaI(rango_capas,f,c),e.GetTtzElementos().GetVarRefFilaI(rango_capas,f,c),nmb,mdlr) {}
