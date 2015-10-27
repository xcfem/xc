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
//SetFilaJ.cc

#include "SetFilaJ.h"
#include "preprocessor/cad/entidades/EntMdlr.h"
#include <boost/any.hpp>
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

XC::SetFilaJ::SetFilaJ(EntMdlr &e,const size_t &f,const size_t &c,const std::string &nmb,XC::Preprocessor *preprocessor)
  : SetFila<tfilanod,tfilaelem>(e.GetTtzNodos().GetVarRefFilaJ(f,c),e.GetTtzElementos().GetVarRefFilaJ(f,c),nmb,preprocessor) {}

XC::SetFilaJ::SetFilaJ(EntMdlr &e,const size_t &capa,const RangoIndice &rango_filas,const size_t &c,const std::string &nmb,Preprocessor *preprocessor)
  : SetFila<tfilanod,tfilaelem>(e.GetTtzNodos().GetVarRefFilaJ(capa,rango_filas,c),e.GetTtzElementos().GetVarRefFilaJ(capa,rango_filas,c),nmb,preprocessor) {}
