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
//SetBase.cc

#include "SetBase.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/Lista.h"
#include <boost/any.hpp>
#include "domain/constraints/SP_Constraint.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "utility/matrix/ID.h"
#include "modelador/Modelador.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

//! @brief Constructor.
XC::SetBase::SetBase(const std::string &nmb,Modelador *md)
  : EntMdlrBase(nmb,md) {}

//!  @brief Lee un objeto SetBase desde el archivo de entrada.
//! 
//!  Soporta los comandos:
//! 
//!  - malla: Dispara el mallado de los objetos del conjunto.
bool XC::SetBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(SetBase) Procesando comando: " << cmd << std::endl;

    if(cmd=="malla")
      {
        const std::string str= status.GetString();
	std::string str_dm;
        if(!str.empty())
          str_dm= interpretaString(status.GetString());
        dir_mallado dm= str_to_dir_mallado(str_dm);
        Malla(dm);
        return true;
      }
    else if(cmd == "fix")
      {
        std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
        if(fnc_indices.size()<1)
	  std::cerr << "fix - uso: fix[dofID] " << std::endl;
        const int id_gdl= convert_to_int(fnc_indices[0]); //Identificador del grado de libertad.
	SP_Constraint tmp(0,0,id_gdl, 0.0);
        tmp.LeeCmd(status);
        fix(tmp);
        return true;
      }
    else if(cmd == "fix_gdls")
      {
        const CmdParser &parser= status.Parser();
        if(parser.TieneIndices())
          {
	    const std::deque<boost::any> idGdls= parser.SeparaIndices(this);
            const std::vector<double> valores= convert_to_vector_double(interpretaVectorAny(status.GetString()));
            const size_t sz= idGdls.size();
            const size_t szv= valores.size();
            double valor= 0.0;
            if(sz)
              {
                for(size_t i= 0;i<sz;i++)
                  {
                    if(i<szv)
                      valor= valores[i];
                    else
                      valor= 0.0;
                    SP_Constraint tmp(0,0,convert_to_int(idGdls[i]),valor);
                    fix(tmp);
                  }
	      }
	  }
        return true;
      }
    else if(cmd == "reset_tributarias")
      {
        status.GetBloque();
        resetTributarias();
        return true;
      }
    else if(cmd == "calcula_longs_tributarias")
      {
        const bool geomInicial= interpretaBool(status.GetBloque());
        calculaLongsTributarias(geomInicial);
        return true;
      }
    else if(cmd == "calcula_areas_tributarias")
      {
        const bool geomInicial= interpretaBool(status.GetBloque());
        calculaAreasTributarias(geomInicial);
        return true;
      }
    else if(cmd == "calcula_vols_tributarias")
      {
        const bool geomInicial= interpretaBool(status.GetBloque());
        calculaVolsTributarios(geomInicial);
        return true;
      }
    else
      return EntMdlrBase::procesa_comando(status);
  }

//! @brief Genera la malla a partir de los objetos del conjunto.
void XC::SetBase::Malla(dir_mallado dm)
  {
    std::cerr << "Método Malla() no implementado." << std::endl;
  }

//! @brief Impone desplazamiento nulo en los nodos de este conjunto.
void XC::SetBase::fix(const SP_Constraint &)
  {
    std::cerr << "Método fix() no implementado." << std::endl;
  }

//! @brief Devuelve los tags de los nodos en un vector de enteros.
const XC::ID &XC::SetBase::getIdNodeTags(void) const
  {
    static ID retval;
    const std::set<int> tmp= getNodeTags();
    if(!tmp.empty())
      {
        const size_t sz= tmp.size();
	retval.resize(sz);
        size_t conta= 0;
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++,conta++)
          retval[conta]= *i;
      }
    return retval;
  }

//! @brief Devuelve los tags de los elementos en un vector de enteros.
const XC::ID &XC::SetBase::getIdElementTags(void) const
  {
    static ID retval;
    const std::set<int> tmp= getElementTags();
    if(!tmp.empty())
      {
        const size_t sz= tmp.size();
	retval.resize(sz);
        size_t conta= 0;
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++,conta++)
          retval[conta]= *i;
      }
    return retval;
  }

//! @brief Devuelve verdadero si el punto pertenece al conjunto.
bool XC::SetBase::In(const Pnt *) const
  { return false; }

//! @brief Devuelve verdadero si el «edge» pertenece al conjunto.
bool XC::SetBase::In(const Edge *) const
  { return false; }

//! @brief Devuelve verdadero si la superficie pertenece al conjunto.
bool XC::SetBase::In(const Face *) const
  { return false; }

//! @brief Devuelve verdadero si el cuerpo pertenece al conjunto.
bool XC::SetBase::In(const Body *) const
  { return false; }

//! @brief Devuelve verdadero si la «uniform grid» pertenece al conjunto.
bool XC::SetBase::In(const UniformGrid *) const
  { return false; }

//! @brief Anula las magnitudes tributarias de los nodos conectados
//! a los elementos del conjunto.
void XC::SetBase::resetTributarias(void) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= GetModelador()->GetDominio();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->resetTributarias();
            else
	      std::cerr << "SetBase::resetTributarias; "
                        << " no se encontró el elemento: "
                        << tag_elem << std::endl;
          }
      }
    else
      std::cerr << "No se ha definido el dominio." << std::endl;    
  }
 
//! @brief Calcula las longitudes tributarias correspondientes a cada
//! nodo de los elementos del conjunto.
void XC::SetBase::calculaLongsTributarias(bool geomInicial) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= GetModelador()->GetDominio();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->calculaLongsTributarias(geomInicial);
            else
	      std::cerr << "SetBase::calculaLongsTributarias; "
                        << " no se encontró el elemento: "
                        << tag_elem << std::endl;
          }
      }
    else
      std::cerr << "No se ha definido el dominio." << std::endl;
  }

//! @brief Calcula las áreas tributarias correspondientes a cada
//! nodo de los elementos del conjunto.
void XC::SetBase::calculaAreasTributarias(bool geomInicial) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= GetModelador()->GetDominio();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->calculaAreasTributarias(geomInicial);
            else
	      std::cerr << "SetBase::calculaLongsTributarias; "
                        << " no se encontró el elemento: "
                        << tag_elem << std::endl;
          }
      }
    else
      std::cerr << "No se ha definido el dominio." << std::endl;
  }

//! @brief Calcula los volúmenes tributarios correspondientes a cada
//! nodo de los elementos del conjunto.
void XC::SetBase::calculaVolsTributarios(bool geomInicial) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= GetModelador()->GetDominio();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->calculaVolsTributarios(geomInicial);
            else
	      std::cerr << "SetBase::calculaLongsTributarias; "
                        << " no se encontró el elemento: "
                        << tag_elem << std::endl;
          }
      }
    else
      std::cerr << "No se ha definido el dominio." << std::endl;
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::SetBase::GetProp(const std::string &cod) const
  {
    if(verborrea>4)
      std::clog << "SetBase::GetProp (" << nombre_clase() << "::GetProp) Buscando propiedad: " << cod << std::endl;

    if(cod=="getNumNodos") //Devuelve el número de nodos.
      {
        tmp_gp_szt= NumNodos();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getNumElementos") //Devuelve el número de elementos.
      {
        tmp_gp_szt= NumElementos();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getListTagNodos")
      {
        SetBase *this_no_const= const_cast<SetBase *>(this);
        tmp_gp_lista.set_owner(this_no_const);
        tmp_gp_lista.clearAll();
	const std::set<int> tagsNodos= getNodeTags();
        for(std::set<int>::const_iterator i= tagsNodos.begin();i!= tagsNodos.end();i++)
          tmp_gp_lista.Inserta(*i);
        if(tmp_gp_lista.empty())
          {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
	    std::clog << "SetBase::GetProp getListTagNodos; el conjunto no tiene nodos."
                      << posLectura << std::endl;
          }
        return any_const_ptr(tmp_gp_lista);
      }
    else if(cod=="getListTagElementos")
      {
        SetBase *this_no_const= const_cast<SetBase *>(this);
        tmp_gp_lista.set_owner(this_no_const);
        tmp_gp_lista.clearAll();
	const std::set<int> tagsElementos= getElementTags();
        for(std::set<int>::const_iterator i= tagsElementos.begin();i!= tagsElementos.end();i++)
          tmp_gp_lista.Inserta(*i);
        if(tmp_gp_lista.empty())
          {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
	    std::clog << "SetBase::GetProp getListTagElementos; el conjunto no tiene elementos."
                      << posLectura << std::endl;
          }
        return any_const_ptr(tmp_gp_lista);
      }
    else
      return EntMdlrBase::GetProp(cod);
  }

