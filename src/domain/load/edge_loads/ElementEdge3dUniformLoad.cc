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

#include "ElementEdge3dUniformLoad.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "utility/matrix/ID.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "boost/lexical_cast.hpp"

#include "domain/mesh/element/fvectors/FVector.h"
#include "domain/mesh/element/Element1D.h"

XC::Vector XC::ElementEdge3dUniformLoad::data(4);

XC::ElementEdge3dUniformLoad::ElementEdge3dUniformLoad(int tag,const double &wY, const double &wZ, const double &wX,const XC::ID &theElementTags,const XC::ID &edges_idx)
  :ElementEdgeUniformLoad(tag, LOAD_TAG_ElementEdge3dUniformLoad,wY,wX, theElementTags,edges_idx), wz(wZ) {}

XC::ElementEdge3dUniformLoad::ElementEdge3dUniformLoad(int tag, const Vector &Fxyz,const ID &theElementTags,const XC::ID &edges_idx)
  :ElementEdgeUniformLoad(tag, LOAD_TAG_ElementEdge3dUniformLoad,Fxyz[1],Fxyz[0], theElementTags,edges_idx), wz(Fxyz[2]) {}

XC::ElementEdge3dUniformLoad::ElementEdge3dUniformLoad(int tag)
  :ElementEdgeUniformLoad(tag, LOAD_TAG_ElementEdge3dUniformLoad), wz(0.0) {}

XC::ElementEdge3dUniformLoad::ElementEdge3dUniformLoad(void)
  :ElementEdgeUniformLoad(0,LOAD_TAG_ElementEdge3dUniformLoad), wz(0.0) {}

int XC::ElementEdge3dUniformLoad::getType(void)
  { return LOAD_TAG_ElementEdge3dUniformLoad; }

//! @brief Devuelve el vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param nod_index Índices (locales) de los nodos que forman el borde del elemento.
//! @param pond Ponderación de la carga en cada uno de los nodos, la suma de los coeficientes aquí especificados debe resultar la unidad.
//! @param L Longitud del borde del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param p0 Vector en el que el elemento guarda las cargas sobre sus nodos.
void XC::ElementEdge3dUniformLoad::addReactionsInBasicSystem(const ID &nod_index,const Vector &pond,const double &L,const double &loadFactor,FVector &p0)
  {
    const double w1= Wx()*loadFactor*L;
    const double w2= Wy()*loadFactor*L;
    const double w3= Wz()*loadFactor*L;
    const int nnodos= nod_index.Size();
    assert(nnodos==pond.Size());
    for(int i=0;i<nnodos;i++)
      {
        const double f= pond(i);
        p0.addForce(nod_index(i),w1*f,w2*f,w3*f);
      }
  }

//! @brief Devuelve el vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param nod_index Índices (locales) de los nodos que forman el borde del elemento.
//! @param pond Ponderación de la carga en cada uno de los nodos, la suma de los coeficientes aquí especificados debe resultar la unidad.
//! @param L Longitud del borde del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param q0 ??
void XC::ElementEdge3dUniformLoad::addFixedEndForcesInBasicSystem(const ID &nod_index,const Vector &,const double &L,const double &loadFactor,FVector &q0)
  {
    std::cerr << "ElementEdge3dUniformLoad::addFixedEndForcesInBasicSystem no implementada." << std::endl;
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::ElementEdge3dUniformLoad::getDbTagData(void) const
  {
    static DbTagData retval(7);
    return retval;
  }

//! @brief Envía los datos a través del canal que se pasa como parámetro.
int XC::ElementEdge3dUniformLoad::sendData(CommParameters &cp)
  {
    int res= ElementEdgeUniformLoad::sendData(cp);
    res+= cp.sendDouble(wz,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::ElementEdge3dUniformLoad::recvData(const CommParameters &cp)
  {
    int res= ElementEdgeUniformLoad::recvData(cp);
    res+= cp.receiveDouble(wz,getDbTagData(),CommMetaData(6));
    return res;
  }

int XC::ElementEdge3dUniformLoad::sendSelf(CommParameters &cp)
  {
    inicComm(7);
    int result= sendData(cp);
    const int dbTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << "ElementEdge3dUniformLoad::sendSelf() - failed to send extra data\n";
    return result;
  }

int XC::ElementEdge3dUniformLoad::recvSelf(const CommParameters &cp)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ElementEdge3dUniformLoad::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }

void  XC::ElementEdge3dUniformLoad::Print(std::ostream &s, int flag)
  {
    s << "ElementEdge2dUniformLoad - Reference load" << std::endl;
    s << "  Transverse (y): " << Trans << std::endl;
    s << "  Transverse (z): " << wz << std::endl;
    s << "  Axial (x):      " << Axial << std::endl;
    s << "  Elements acted on: " << getElementTags();
  }

//! @brief Devuelve la dimension del vector fuerza.
size_t XC::ElementEdge3dUniformLoad::getDimVectorFuerza(void) const
  { return 3; }

//! @brief Devuelve la dimension del vector momento.
size_t XC::ElementEdge3dUniformLoad::getDimVectorMomento(void) const
  { return 3; }

//! @brief Devuelve las componentes de los vectores fuerza.
const XC::Matrix &XC::ElementEdge3dUniformLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= Wx();
        retval(i,1)= Wy();
        retval(i,2)= Wz();
      }
    return retval;
  }

//! @brief Devuelve las componentes de los vectores momento.
const XC::Matrix &XC::ElementEdge3dUniformLoad::getLocalMoments(void) const
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
