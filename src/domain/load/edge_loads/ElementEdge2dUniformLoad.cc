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

#include "ElementEdge2dUniformLoad.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "boost/any.hpp"
#include "xc_utils/src/base/CmdStatus.h"
#include "utility/matrix/ID.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/element/fvectors/FVector.h"

XC::Vector XC::ElementEdge2dUniformLoad::data(2);

XC::ElementEdge2dUniformLoad::ElementEdge2dUniformLoad(int tag,const double &wt,const double &wa,const ID &theElementTags,const ID &edge_idx)
  :ElementEdgeUniformLoad(tag, LOAD_TAG_ElementEdge2dUniformLoad,wt,wa,theElementTags,edge_idx) {}

XC::ElementEdge2dUniformLoad::ElementEdge2dUniformLoad(int tag)
  :ElementEdgeUniformLoad(tag, LOAD_TAG_ElementEdge2dUniformLoad) {}

XC::ElementEdge2dUniformLoad::ElementEdge2dUniformLoad(void)
  :ElementEdgeUniformLoad(0,LOAD_TAG_ElementEdge2dUniformLoad) {}

int XC::ElementEdge2dUniformLoad::getType(void)
  { return LOAD_TAG_ElementEdge2dUniformLoad; }

//! @brief Devuelve la dimension del vector fuerza.
size_t XC::ElementEdge2dUniformLoad::getDimVectorFuerza(void) const
  { return 2; }

//! @brief Devuelve la dimension del vector momento.
size_t XC::ElementEdge2dUniformLoad::getDimVectorMomento(void) const
  { return 1; }

//! @brief Devuelve las componentes de los vectores fuerza.
const XC::Matrix &XC::ElementEdge2dUniformLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,2);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= W1();
        retval(i,1)= W2();
      }
    return retval;
  }

//! @brief Devuelve las componentes de los vectores momento.
const XC::Matrix &XC::ElementEdge2dUniformLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,1);
    for(size_t i=0; i<sz; i++)
      retval(i,0)= 0.0;
    return retval;
  }

//! @brief Devuelve el vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param nod_index Índices (locales) de los nodos que forman el borde del elemento.
//! @param pond Ponderación de la carga en cada uno de los nodos, la suma de los coeficientes aquí especificados debe resultar la unidad.
//! @param L Longitud del borde del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param p0 Vector en el que el elemento guarda las cargas sobre sus nodos.
void XC::ElementEdge2dUniformLoad::addReactionsInBasicSystem(const ID &nod_index,const Vector &pond,const double &L,const double &loadFactor,FVector &p0)
  {
    const double w1= W1()*loadFactor*L;
    const double w2= W2()*loadFactor*L;
    const int nnodos= nod_index.Size();
    assert(nnodos==pond.Size());
    for(int i=0;i<nnodos;i++)
      {
        const double f= pond(i);
        p0.addForce(nod_index(i),w1*f,w2*f,0.0);
      }
  }

//! @brief Devuelve el vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param nod_index Índices (locales) de los nodos que forman el borde del elemento.
//! @param pond Ponderación de la carga en cada uno de los nodos, la suma de los coeficientes aquí especificados debe resultar la unidad.
//! @param L Longitud del borde del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param q0 ??
void XC::ElementEdge2dUniformLoad::addFixedEndForcesInBasicSystem(const ID &nod_index,const Vector &,const double &L,const double &loadFactor,FVector &q0)
  {
    std::cerr << "ElementEdge2dUniformLoad::addFixedEndForcesInBasicSystem no implementada." << std::endl;
    return;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::ElementEdge2dUniformLoad::sendSelf(CommParameters &cp)
  {
    inicComm(6);
    int result= sendData(cp);
    const int dbTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << "ElementEdge2dUniformLoad::sendSelf() - failed to send extra data\n";
    return result;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::ElementEdge2dUniformLoad::recvSelf(const CommParameters &cp)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ElementEdge2dUniformLoad::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }
