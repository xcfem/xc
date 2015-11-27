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
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/Lista.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/any_const_ptr.h"
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

bool XC::ElementBodyLoad::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ElementBodyLoad) Procesando comando: " << cmd << std::endl;
    if(cmd == "elements")
      {
	std::vector<int> tmp= crea_vector_int(status.GetString());
        const int sz= tmp.size();
	ID tags_elementos(sz);
        for(int i= 0;i<sz;i++)
          tags_elementos(i)= tmp[i];
        setElementTags(tags_elementos);
        return true;
      }
    else if(cmd == "elementSet")
      {
	const std::string nmbSet= interpretaString(status.GetString());
        const Preprocessor *preprocessor= GetPreprocessor();
        if(preprocessor)
          {
            const MapSet &sets= preprocessor->get_sets();
            const SetBase *set= sets.busca_set(nmbSet);
            if(set)
              {
                const size_t sz= set->NumElementos();
                if(sz>0)
                  setElementTags(set->getIdElementTags());
              }
          }
        else
	  std::cerr  << "(ElementBodyLoad) Procesando comando: '" << cmd 
                     << "' necesito un preprocesador." << std::endl;
        return true;
      }
    else if(cmd == "elementList")
      {
        const Lista tmp= interpretaLista(status.GetString());
        const size_t sz= tmp.size();
        if(sz>0)
          { 
	    ID tags_elementos(sz);
            size_t conta= 0;
            for(Lista::const_iterator i= tmp.begin();i!=tmp.end();i++,conta++)
              tags_elementos(conta)= convert_to_int(*i);
            setElementTags(tags_elementos);
          }
        else
	  std::cerr  << "(ElementBodyLoad) Procesando comando: '" << cmd 
                     << "' se pasó una lista vacía." << std::endl;
        return true;
      }
    else if(cmd == "element_pointers")
      {
        theElements.LeeCmd(status);
        return true;
      }
    else
      return ElementalLoad::procesa_comando(status);
  }

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

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::ElementBodyLoad::GetProp(const std::string &cod) const
  {
    if(cod == "elementTags")
      {
        const size_t nv= numElements();
	static m_int retval;
        retval= m_int(nv,1,0);
        if(nv<1) return any_const_ptr(retval);
        for(size_t i=0;i<nv;i++)
          retval[i]= elemTags(i);
        return any_const_ptr(retval);
      }
    else
      return ElementalLoad::GetProp(cod);
  }
