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
//FE_EleConstIter.cpp

#include "solution/analysis/model/FE_EleConstIter.h"

#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <utility/tagged/storage/TaggedObjectIter.h>
#include <utility/tagged/storage/TaggedObjectStorage.h>


XC::FE_EleConstIter::FE_EleConstIter(TaggedObjectStorage *theStorage)
  :myIter(&(theStorage->getComponents())) {}

void XC::FE_EleConstIter::reset(void)
  { myIter->reset(); }    


const XC::FE_Element *XC::FE_EleConstIter::operator()(void)
  {
    const FE_Element *retval= nullptr;
    const TaggedObject *theComponent= (*myIter)();
    if(theComponent)
      retval= dynamic_cast<const FE_Element *>(theComponent);
    return retval;
  }

    
    
