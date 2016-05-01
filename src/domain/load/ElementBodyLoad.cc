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

#include "domain/load/ElementBodyLoad.h"
#include <domain/mesh/element/Element.h>
#include <domain/domain/Domain.h>



#include "xc_basic/src/matrices/m_int.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableID.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/MapSet.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "domain/mesh/node/Node.h"

XC::ElementBodyLoad::ElementBodyLoad(int tag, int classTag, const XC::ID &theEleTags)
  :ElementalLoad(tag, classTag,theEleTags), theElements() {}

XC::ElementBodyLoad::ElementBodyLoad(int tag, int classTag)
  :ElementalLoad(tag, classTag), theElements() {}

// provided for the FEM_Object broker; the tag and elementTag need
// to be supplied in recvSelf();
XC::ElementBodyLoad::ElementBodyLoad(int classTag)
  :ElementalLoad(0, classTag), theElements() {}

void XC::ElementBodyLoad::setDomain(Domain *theDomain)
  {
    ElementalLoad::setDomain(theDomain);
    theElements.setPtrs(theDomain,elemTags);
  }

void XC::ElementBodyLoad::applyLoad(double loadFactor) 
  {
    const int sz= theElements.size();
    if(sz!=numElements())
      std::cerr << "ElementBodyLoad::applyLoad; el número de punteros no coincide con el de identificadores." << std::endl;
    for(int i=0; i<sz; i++)
      if(theElements[i])
        theElements[i]->addLoad(this, loadFactor);
  }

//! @brief Elimina el elemento cuyo tag se pasa como parámetro
//! de la lista de elementos afectados por la carga. Si falla
//! devuelve -1.
int XC::ElementBodyLoad::removeElement(int tag) 
  {
    int loc = elemTags.getLocation(tag);
    if(loc >= 0)
      {
        elemTags(loc) = -1;
        theElements.removeElement(tag);
      }
    return numElements();
  }

//! @brief Imprime el objeto.
void XC::ElementBodyLoad::Print(std::ostream &s, int flag) const
  {
    s << "  Elementos sobre los que actúa: " << getElementTags();
  }

//! @brief Envía los datos a través del canal que se pasa como parámetro.
int XC::ElementBodyLoad::sendData(CommParameters &cp)
  {
    int res= ElementalLoad::sendData(cp);
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::ElementBodyLoad::recvData(const CommParameters &cp)
  {
    int res= ElementalLoad::recvData(cp);
    return res;
  }
