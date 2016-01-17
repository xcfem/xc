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
//BidimLoad.cpp

#include "BidimLoad.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/matrix/ID.h"

XC::BidimLoad::BidimLoad(int tag,int classTag,const ID &theElementTags)
  :ElementBodyLoad(tag, classTag, theElementTags) {}

XC::BidimLoad::BidimLoad(int tag,int classTag)
  :ElementBodyLoad(tag, classTag) {}

//! @brief Imprime el objeto.
void XC::BidimLoad::Print(std::ostream &s, int flag) const
  {
    s << "BidimLoad - Reference load" << std::endl;
    ElementBodyLoad::Print(s,flag);
  }

std::string XC::BidimLoad::Categoria(void) const
  { return "bidim_nodef"; }
