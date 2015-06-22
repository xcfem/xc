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
//SingleDomTIter.h

#ifndef SingleDomTIter_h
#define SingleDomTIter_h
#include <utility/tagged/storage/TaggedObjectIter.h>
#include <utility/tagged/storage/TaggedObjectStorage.h>

namespace XC {

template <class TIter>
class SingleDomTIter : public TIter
  {
  private:
    TaggedObjectIter &myIter;
  public:
    SingleDomTIter(TaggedObjectStorage *theStorage);

    inline TaggedObject *getIterPtr(void)
      { return myIter(); }
    virtual void reset(void);
  };

template <class TIter>
SingleDomTIter<TIter>::SingleDomTIter(TaggedObjectStorage *theStorage)
  :myIter(theStorage->getComponents())
  {}

template <class TIter>
void SingleDomTIter<TIter>::reset(void)
  { myIter.reset(); }

template <class T,class TIter>
T *getIterPtr(TIter &titer)
  {
    // check if we still have Nodes in the model
    // if not return 0, indicating we are done
    TaggedObject *theComponent= titer.getIterPtr();
    if(!theComponent)
      return NULL;
    else
      return dynamic_cast<T *>(theComponent);
  }

} // fin namespace XC

#endif

