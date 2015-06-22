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
//ShellMecLoad.cpp

#include "ShellMecLoad.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "utility/matrix/ID.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/element/plane/shell/ShellMITC4Base.h"
#include "domain/mesh/element/plane/shell/ShellNL.h"
#include "domain/mesh/element/coordTransformation/CrdTransf.h"
#include "domain/mesh/node/Node.h"

XC::ShellMecLoad::ShellMecLoad(int tag,int classTag,const double &wt,const double &wa1,const double &wa2,const ID &theElementTags)
  :BidimMecLoad(tag, classTag, wt, wa1, wa2, theElementTags) {}

XC::ShellMecLoad::ShellMecLoad(int tag,int classTag)
  :BidimMecLoad(tag, classTag) {}

//! @brief Lee un objeto ShellMecLoad desde archivo
bool XC::ShellMecLoad::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ShellMecLoad) Procesando comando: " << cmd << std::endl;

    return BidimMecLoad::procesa_comando(status);
  }

//! @brief Añade la carga al vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param area Area del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param p0 Vector de cargas del elemento.
void XC::ShellMecLoad::addReactionsInBasicSystem(const double &,const double &,FVectorShell &)
  {
    std::cerr << "addReactionsInBasicSystem no definida." << std::endl;
  }

//! @brief ??
//! @param area Area del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param q0 
void XC::ShellMecLoad::addFixedEndForcesInBasicSystem(const double &,const double &loadFactor,FVectorShell &)
  {
    std::cerr << "addFixedEndForcesInBasicSystem no definida." << std::endl;
  }

//! @brief Devuelve la dimension del vector fuerza.
size_t XC::ShellMecLoad::getDimVectorFuerza(void) const
  { return 3; }

//! @brief Devuelve la dimension del vector momento.
size_t XC::ShellMecLoad::getDimVectorMomento(void) const
  { return 3; }

//! @brief Devuelve las componentes de los vectores fuerza.
const XC::Matrix &XC::ShellMecLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= Trans;
        retval(i,1)= Axial1;
        retval(i,2)= Axial2;
      }
    return retval;
  }

//! @brief Devuelve las componentes de los vectores momento.
const XC::Matrix &XC::ShellMecLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= 0.0;
        retval(i,1)= 0.0;
        retval(i,2)= 0.0;
      }
    return retval;
  }

//! @brief Devuelve los vectores expresados en coordenadas globales.
const XC::Matrix &XC::ShellMecLoad::getGlobalVectors(const Matrix &localVectors) const
  {
    static Matrix retval;
    retval= localVectors;
    const Domain *ptrDom= getDomain();
    if(ptrDom)
      {
        const size_t sz= localVectors.noRows();
        for(size_t i=0; i<sz; i++)
          {
            const size_t elemTag= getElementTags()(i);
            const Element *ptrElem= ptrDom->getElement(elemTag);
            if(const ShellMITC4Base *ptrShell= dynamic_cast<const ShellMITC4Base *>(ptrElem))
              {
                const ShellCrdTransf3dBase *ptrTransf= ptrShell->getCoordTransf();
                if(ptrTransf)
                  retval= ptrTransf->getVectorGlobalCoordFromLocal(localVectors);
                else
		  std::cerr << "ShellMecLoad::getGlobalVectors; el elemento: "
                        << elemTag << " no tiene transformación de coordenadas." << std::endl;
              }
            else if(const ShellNL *ptrShell= dynamic_cast<const ShellNL *>(ptrElem))
              {
                const ShellCrdTransf3dBase *ptrTransf= ptrShell->getCoordTransf();
                if(ptrTransf)
                  retval= ptrTransf->getVectorGlobalCoordFromLocal(localVectors);
                else
		  std::cerr << "ShellMecLoad::getGlobalVectors; el elemento: "
                        << elemTag << " no tiene transformación de coordenadas." << std::endl;
              }
            else
	      std::cerr << "ShellMecLoad::getGlobalVectors; el elemento: "
                        << elemTag << " no es unidimensional." << std::endl;
          }
      }
    else
      std::cerr << "ShellMecLoad::getGlobalVectors; no existe apuntador al dominio." << std::endl;
    return retval;
  }

//! @brief Devuelve la fuerza expresada en coordenadas globales.
const XC::Matrix &XC::ShellMecLoad::getGlobalForces(void) const
  { return getGlobalVectors(getLocalForces()); }

//! @brief Devuelve el momento expresado en coordenadas globales.
const XC::Matrix &XC::ShellMecLoad::getGlobalMoments(void) const
  { return getGlobalVectors(getLocalMoments()); }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::ShellMecLoad::GetProp(const std::string &cod) const
  {
    if(cod == "getLocalForces")
      {
        static m_double retval;
        retval= matrix_to_m_double(getLocalForces());
        return any_const_ptr(retval);
      }
    else if(cod == "getLocalMoments")
      {
        static m_double retval;
        retval= matrix_to_m_double(getLocalMoments());
        return any_const_ptr(retval);
      }
    else if(cod == "getGlobalForces")
      {
        static m_double retval;
        retval= matrix_to_m_double(getGlobalForces());
        return any_const_ptr(retval);
      }
    else
      return BidimMecLoad::GetProp(cod);
  }
