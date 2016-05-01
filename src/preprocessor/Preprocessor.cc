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
//Preprocessor.cc

#include "Preprocessor.h"
#include "ProblemaEF.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "preprocessor/set_mgmt/SetEstruct.h"
#include "preprocessor/set_mgmt/Set.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "boost/lexical_cast.hpp"


#include "utility/matrix/ID.h"

//! @brief Constructor por defecto.
XC::Preprocessor::Preprocessor(EntCmd *owr,DataOutputHandler::map_output_handlers *oh)
  : EntCmd(owr), MovableObject(0), dominio(nullptr), materiales(this), transf(this), beamIntegrators(this), 
    nodos(this), elementos(this), loads(this), constraints(this),
    cad(this),sets(this)
  { dominio= new Domain(this,oh); }

//! @brief Constructor de copia (prohibida).
XC::Preprocessor::Preprocessor(const Preprocessor &otro)
  : EntCmd(otro), MovableObject(otro), dominio(nullptr), materiales(this), transf(this), beamIntegrators(this),
    nodos(this), elementos(this), loads(this), constraints(this),
    cad(this),sets(this)
  {
    std::cerr << "No se puede copiar un objeto de clase Preprocessor." << std::endl;
  }

//! @brief Operador asignación (prohibida).
XC::Preprocessor &XC::Preprocessor::operator=(const Preprocessor &otro)
  {
    std::cerr << "No se puede asignar un objeto de clase Preprocessor." << std::endl;
    return *this;
  }

//! @brief Inserta el puntero al nodo en el conjunto «total» y en los conjuntos
//! que estén abiertos.
void XC::Preprocessor::UpdateSets(Node *nuevo_nodo)
  {
    sets.get_set_total()->agregaNodo(nuevo_nodo);
    MapSet::map_sets &abiertos= sets.get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->agregaNodo(nuevo_nodo);
      }
  }

//! @brief Inserta el puntero al elemento en el conjunto «total» y en los conjuntos
//! que estén abiertos.
void XC::Preprocessor::UpdateSets(Element *nuevo_elem)
  {
    sets.get_set_total()->agregaElemento(nuevo_elem);
    MapSet::map_sets &abiertos= sets.get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->agregaElemento(nuevo_elem);
      }
  }

//! @brief  Inserta el puntero a la coacción en el conjunto «total» y en los conjuntos
//! que estén abiertos.
void XC::Preprocessor::UpdateSets(Constraint *nuevo_constraint)
  {
    sets.get_set_total()->GetConstraints().push_back(nuevo_constraint);
    MapSet::map_sets &abiertos= sets.get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->GetConstraints().push_back(nuevo_constraint);
      }
  }

//! @brief Destructor.
XC::Preprocessor::~Preprocessor(void)
  {
    cad.clearAll();
    if(dominio) delete dominio;
    dominio= nullptr;
  }


//! @brief Assigns Stress Reduction Factor for element deactivation.
void XC::Preprocessor::setDeadSRF(const double &d)
  { Element::setDeadSRF(d); }

//! @brief Busca el conjunto o entidad del preprocessador (punto,linea,...) cuyo nombre se pasa como parámetro.
XC::SetEstruct *XC::Preprocessor::busca_set_estruct(const std::string &nmb)
  {
    SetEstruct *retval= sets.busca_set_estruct(nmb);
    if(!retval)
      retval= cad.busca_set_estruct(boost::lexical_cast<size_t>(nmb));
    return retval;
  }

//! @brief Prepara el dominio para resolver un nuevo caso
//! de carga.
void XC::Preprocessor::resetLoadCase(void)
  { 
    getLoadLoader().removeAllFromDomain();
    if(dominio)
      dominio->resetLoadCase();
    else
      std::cerr << "Preprocessor::resetLoadCase; domain not defined (null ptr)."
                << std::endl;
  }


//! @brief Deletes all preprocessor entities.
void XC::Preprocessor::clearAll(void)
  {
    sets.reset();
    cad.clearAll();
    transf.clearAll();
    beamIntegrators.clearAll();
    nodos.clearAll();
    elementos.clearAll();
    if(dominio)
      dominio->clearAll();
    loads.clearAll();
    constraints.clearAll();
    materiales.clearAll();
  }

//! @brief Devuelve un puntero a la base de datos.
XC::FE_Datastore *XC::Preprocessor::getDataBase(void)
  {
    FE_Datastore *retval= nullptr;
    ProblemaEF *prb= dynamic_cast<ProblemaEF *>(Owner());
    if(prb)
      retval= prb->getDataBase();
    return retval;
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::Preprocessor::getDbTagData(void) const
  {
    static DbTagData retval(10);
    return retval;
  }

//! @brief Envía los datos a través del canal que se pasa como parámetro.
int XC::Preprocessor::sendData(CommParameters &cp)
  {
    //res+= cp.sendMovable(materiales,getDbTagData(),CommMetaData(0));
    //res+= cp.sendMovable(transf,getDbTagData(),CommMetaData(1));
    //res+= cp.sendMovable(beamIntegrators,getDbTagData(),CommMetaData(2));
    //res+= cp.sendMovable(nodos,getDbTagData(),CommMetaData(3));
    //res+= cp.sendMovable(elementos,getDbTagData(),CommMetaData(4));
    int res= cp.sendMovable(loads,getDbTagData(),CommMetaData(5));
    //res+= cp.sendMovable(constraints,getDbTagData(),CommMetaData(6));
    //res+= cp.sendMovable(cad,getDbTagData(),CommMetaData(7));
    assert(dominio);
    res+= sendDomain(*dominio,8,getDbTagData(),cp);
    res+= cp.sendMovable(sets,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::Preprocessor::recvData(const CommParameters &cp)
  {
    //res+= cp.receiveMovable(materiales,getDbTagData(),CommMetaData(0));
    //res+= cp.receiveMovable(transf,getDbTagData(),CommMetaData(1));
    //res+= cp.receiveMovable(beamIntegrators,getDbTagData(),CommMetaData(2));
    //res+= cp.receiveMovable(nodos,getDbTagData(),CommMetaData(3));
    //res+= cp.receiveMovable(elementos,getDbTagData(),CommMetaData(4));
    int res= cp.receiveMovable(loads,getDbTagData(),CommMetaData(5));
    //res+= cp.receiveMovable(constraints,getDbTagData(),CommMetaData(6));
    //res+= cp.receiveMovable(cad,getDbTagData(),CommMetaData(7));
    assert(dominio);
    res+= receiveDomain(*dominio,8,getDbTagData(),cp);
    res+= cp.receiveMovable(sets,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
int XC::Preprocessor::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(10);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Recibe el objeto por el canal que se pasa como parámetro.
int XC::Preprocessor::recvSelf(const CommParameters &cp)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

