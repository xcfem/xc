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
//ElementEdgeMecLoad.cpp

#include"ElementEdgeMecLoad.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

#include "domain/mesh/element/Element1D.h"
#include "domain/mesh/element/coordTransformation/CrdTransf.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/actor/actor/MovableVector.h"

XC::ElementEdgeMecLoad::ElementEdgeMecLoad(int tag,int classTag,const double &wt,const double &wa,const ID &theElementTags,const ID &edgeIdx)
  :ElementEdgeLoad(tag, classTag, theElementTags,edgeIdx), Trans(wt), Axial(wa) {}

XC::ElementEdgeMecLoad::ElementEdgeMecLoad(int tag,int classTag)
  :ElementEdgeLoad(tag, classTag), Trans(0.0), Axial(0.0) {}

//! @brief Devuelve el vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param nod_index Índices (locales) de los nodos que forman el borde del elemento.
//! @param pond Ponderación de la carga en cada uno de los nodos, la suma de los coeficientes aquí especificados debe resultar la unidad.
//! @param L Longitud del borde del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param p0 Vector en el que el elemento guarda las cargas sobre sus nodos.
void XC::ElementEdgeMecLoad::addReactionsInBasicSystem(const ID &,const Vector &,const double &,const double &,FVector &)
  {
    std::cerr << "addReactionsInBasicSystem no definida." << std::endl;
  }

//! @brief Devuelve el vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param nod_index Índices (locales) de los nodos que forman el borde del elemento.
//! @param pond Ponderación de la carga en cada uno de los nodos, la suma de los coeficientes aquí especificados debe resultar la unidad.
//! @param L Longitud del borde del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param q0 ??
void XC::ElementEdgeMecLoad::addFixedEndForcesInBasicSystem(const ID &,const Vector &,const double &L,const double &loadFactor,FVector &)
  {
    std::cerr << "addFixedEndForcesInBasicSystem no definida." << std::endl;
  }

//! @brief Devuelve la dimension del vector fuerza.
size_t XC::ElementEdgeMecLoad::getDimVectorFuerza(void) const
  { return 2; }

//! @brief Devuelve la dimension del vector momento.
size_t XC::ElementEdgeMecLoad::getDimVectorMomento(void) const
  { return 1; }

//! @brief Devuelve las componentes de los vectores fuerza.
const XC::Matrix &XC::ElementEdgeMecLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,2);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= Trans;
        retval(i,1)= Axial;
      }
    return retval;
  }

//! @brief Devuelve las componentes de los vectores momento.
const XC::Matrix &XC::ElementEdgeMecLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,1);
    for(size_t i=0; i<sz; i++)
      retval(i,0)= 0.0;
    return retval;
  }

const XC::Matrix &XC::ElementEdgeMecLoad::getGlobalVectors(const Matrix &localVectors) const
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
            const Element1D *ptrElem= dynamic_cast<const Element1D *>(ptrDom->getElement(elemTag));
            if(ptrElem)
              {
                const CrdTransf *ptrTransf= ptrElem->getCoordTransf();
                if(ptrTransf)
                  retval= ptrTransf->getVectorGlobalCoordFromLocal(localVectors);
                else
		  std::cerr << "ElementEdgeMecLoad::getGlobalVectors; el elemento: "
                        << elemTag << " no tiene transformación de coordenadas." << std::endl;
              }
            else
	      std::cerr << "ElementEdgeMecLoad::getGlobalVectors; el elemento: "
                        << elemTag << " no es unidimensional." << std::endl;
          }
      }
    else
      std::cerr << "ElementEdgeMecLoad::getGlobalVectors; no existe apuntador al dominio." << std::endl;
    return retval;
  }

//! @brief Devuelve la fuerza expresada en coordenadas globales.
const XC::Matrix &XC::ElementEdgeMecLoad::getGlobalForces(void) const
  { return getGlobalVectors(getLocalForces()); }

//! @brief Devuelve el momento expresado en coordenadas globales.
const XC::Matrix &XC::ElementEdgeMecLoad::getGlobalMoments(void) const
  { return getGlobalVectors(getLocalMoments()); }

void XC::ElementEdgeMecLoad::Print(std::ostream &s, int flag) const
  {
    s << "ElementEdgeMecLoad - Reference load" << std::endl;
    s << "  Transverse: " << Trans << std::endl;
    s << "  Axial:      " << Axial << std::endl;
    ElementEdgeLoad::Print(s,flag);
  }

//! @brief Envía los datos a través del canal que se pasa como parámetro.
int XC::ElementEdgeMecLoad::sendData(CommParameters &cp)
  {
    int res= ElementEdgeLoad::sendData(cp);
    res+= cp.sendDoubles(Trans,Axial,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::ElementEdgeMecLoad::recvData(const CommParameters &cp)
  {
    int res= ElementEdgeLoad::recvData(cp);
    res+= cp.receiveDoubles(Trans,Axial,getDbTagData(),CommMetaData(5));
    return res;
  }
