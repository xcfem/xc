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
//Constraint.cc

#include "domain/constraints/Constraint.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/matrix/ID.h"
#include "med.h"
#include "vtkCellType.h"

//! @brief Constructor por defecto.
XC::Constraint::Constraint(int clasTag)
  :ContinuaReprComponent(0,clasTag), constrNodeTag(0) {}


// constructor for a subclass to use
//! @brief Constructor.
//! @param tag: Identificador de la coacción.
//! @param node: Identificador del nodo cuyos grados de libertad se coaccionan.
XC::Constraint::Constraint(int tag, int node, int clasTag)
  :ContinuaReprComponent(tag, clasTag), constrNodeTag(node) {}

//! @brief Destructor.
XC::Constraint::~Constraint(void)
  {
    Domain *tmp= getDomain();
    if(tmp)
      {
        Node *n= tmp->getNode(constrNodeTag);
        if(n) n->desconecta(this);
      }
  }

//! @brief Asigna el dominio a la componente. 
void XC::Constraint::setDomain(Domain *model)
  {
    Domain *tmp= getDomain();
    if(tmp!=model)
      {
        if(tmp)
          {
            Node *n= tmp->getNode(constrNodeTag);
            if(n) n->desconecta(this);
          }
        DomainComponent::setDomain(model);
        if(model)
          {
            Node *n= model->getNode(constrNodeTag);
            if(n) n->conecta(this);
          }
      }
  }

//! @brief Asigna el identificador del nodo al que se impone la coacción.
void XC::Constraint::setNodeTag(const int &nt)
  { constrNodeTag= nt; }


//! @brief Devuelve el identificador del nodo al que se impone la coacción.
int XC::Constraint::getNodeTag(void) const
  {
    // return id of constrained node
    return constrNodeTag;
  }

const XC::Node *XC::Constraint::getNode(void) const
  {
    const Node *retval= nullptr;
    Domain *tmp= getDomain();
    if(tmp)
      retval= tmp->getNode(constrNodeTag);
    return retval;
  }

int XC::Constraint::getNodeIdx(void) const
  {
    int retval= -1;
    const Node *n= getNode();
    if(n)
      retval= n->getIdx();
    return retval;
  }

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::Constraint::sendData(CommParameters &cp)
  {
    int res= ContinuaReprComponent::sendData(cp);
    res+= cp.sendInt(constrNodeTag,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Recibe los miembros a través del canal que se pasa como parámetro.
int XC::Constraint::recvData(const CommParameters &cp)
  {
    int res= ContinuaReprComponent::recvData(cp);
    res+= cp.receiveInt(constrNodeTag,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Imprime la coacción.
void XC::Constraint::Print(std::ostream &s, int flag) 
  {
    s << "Constraint: " << this->getTag();
    s << "\t XC::Node: " << constrNodeTag << std::endl;
  }

//! @brief Interfaz con VTK.
int XC::Constraint::getVtkCellType(void) const
  {
    std::cerr << "Constraint::getVtkCellType; no implementada." << std::endl;
    return VTK_EMPTY_CELL;
  }

//! @brief Interfaz con el formato MED de Salome.
int XC::Constraint::getMEDCellType(void) const
  { return MED_POINT1; }






