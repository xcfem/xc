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
//ElementPtrs.cpp

#include "ElementPtrs.h"
#include "domain/mesh/element/Element.h"
#include "domain/domain/Domain.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_basic/src/matrices/m_int.h"
#include "utility/matrix/ID.h"


//! @brief Constructor.
XC::ElementPtrs::ElementPtrs(void)
  : theElements(0) {}


//! @brief Destructor.
XC::ElementPtrs::~ElementPtrs(void)
  { theElements.clear(); }

//! @brief Lee un objeto ElementPtrs desde archivo
bool XC::ElementPtrs::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ElementPtrs) Procesando comando: " << cmd << std::endl;
    if(cmd == "for_each")
      {
        const std::string nmbBlq= nombre_clase()+":for_each";
        const std::string &bloque= status.GetBloque();
	iterator i= begin();
	for(;i!= end();i++)
          (*i)->EjecutaBloque(status,bloque,nmbBlq);
        return true;
      }
    else if(cmd == "findElementWithTag")
      {
        std::deque<boost::any> fnc_args= status.Parser().SeparaArgs(this);
        if(!fnc_args.empty())
          {
            const int tag_elem= convert_to_int(fnc_args[0]); //Tag del elemento.
            Element *ptr= findPtr(tag_elem);
            if(ptr)
              ptr->LeeCmd(status);
            else
              std::clog << "(ElementPtrs) Procesando comando: '" << cmd
                        << "' no se encontró el elemento de tag: " << tag_elem << std::endl;
          }
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
  }

//! @brief Asigna los punteros a partir de los identificadores de elemento.
void XC::ElementPtrs::setPtrs(Domain *theDomain, const ID &theElementTags)
  {
    size_t sz= theElementTags.Size();
    theElements.clear();
    if(theDomain)
      {
        theElements.resize(sz);
        for(size_t i=0; i<sz; i++)
          {
            theElements[i]= theDomain->getElement(theElementTags(i));
            if(!theElements[i])
              {
                std::cerr << "WARNING - XC::ElementPtrs::setDomain - ele with tag ";
	        std::cerr << theElementTags(i) << " does not exist in the domain\n";
              }
          }
      }
  }

//! @brief Devuelve un iterador al elemento cuyo tag se pasa como parámetro.
XC::ElementPtrs::iterator XC::ElementPtrs::find(const int &tag)
  {
    iterator retval= end();
    for(iterator i= begin();i!=end();i++)
      if((*i)->getTag() == tag)
        retval= i;
    return retval;
  }

//! @brief Devuelve un iterador al elemento cuyo tag se pasa como parámetro.
XC::ElementPtrs::const_iterator XC::ElementPtrs::find(const int &tag) const
  {
    const_iterator retval= end();
    for(const_iterator i= begin();i!=end();i++)
      if((*i)->getTag() == tag)
        retval= i;
    return retval;
  }

XC::Element *XC::ElementPtrs::findPtr(const int &tag)
  {
    XC::Element *retval= nullptr;
    iterator i= find(tag);
    if(i!=end())
      retval= *i;
    return retval;
  }

const XC::Element *XC::ElementPtrs::findPtr(const int &tag) const
  {
    const XC::Element *retval= nullptr;
    const_iterator i= find(tag);
    if(i!=end())
      retval= *i;
    return retval;
  }


//! @brief Elimina el elemento cuyo tag se pasa como parámetro.
size_t XC::ElementPtrs::removeElement(const int &tag) 
  {
    iterator i= find(tag);
    if(i!=end())
      theElements.erase(i,i);
    return size();
  }



XC::ElementPtrs::const_reference XC::ElementPtrs::operator()(const size_t &i) const
  { return theElements[i]; }
XC::ElementPtrs::reference XC::ElementPtrs::operator()(const size_t &i)
  { return theElements[i]; }
XC::ElementPtrs::const_reference XC::ElementPtrs::operator[](const size_t &i) const
  { return theElements[i]; }
XC::ElementPtrs::reference XC::ElementPtrs::operator[](const size_t &i)
  { return theElements[i]; }


//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::ElementPtrs::GetProp(const std::string &cod) const
  {
    if(cod == "num_elem")
      {
        tmp_gp_szt= theElements.size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getTags")
      {
        const size_t sz= theElements.size();
	static m_int retval;
        retval= m_int(1,sz,0);
        for(size_t i=0; i<sz; i++)
          if(theElements[i])
            retval(1,i)= theElements[i]->getTag();
        return any_const_ptr(retval);
      }
    else
      return EntCmd::GetProp(cod);
  }
