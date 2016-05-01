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

//
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Purpose: This file contains the implementation of class XC::MRMP_Constraint.

#include <domain/constraints/MRMP_Constraint.h>

#include <cstdlib>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <boost/any.hpp>

#include "xc_basic/src/matrices/m_int.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/NodePtrsWithIDs.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "med.h"
#include "vtkCellType.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"


// constructor for FEM_ObjectBroker			// Arash
XC::MRMP_Constraint::MRMP_Constraint(int tag,int clasTag)		
 : MP_ConstraintBase(tag,0,clasTag) {}

// constructor for FEM_ObjectBroker			// LCPT
XC::MRMP_Constraint::MRMP_Constraint(int tag)		
 : MP_ConstraintBase(tag, 0,CNSTRNT_TAG_MRMP_Constraint) {}

// constructor for Subclass
XC::MRMP_Constraint::MRMP_Constraint(int tag, const ID &retainedNodes, int nodeConstr,const ID &constrainedDOF)
  : MP_ConstraintBase(tag, nodeConstr, constrainedDOF, CNSTRNT_TAG_MRMP_Constraint), 
    retainedNodeTags(retainedNodes) {}

// constructor for Subclass
XC::MRMP_Constraint::MRMP_Constraint(int tag, const ID &retainedNodes, int nodeConstr,const ID &constrainedDOF, int clasTag)
  : MP_ConstraintBase(tag, nodeConstr, constrainedDOF, clasTag),
   retainedNodeTags(retainedNodes) { }


// general constructor for XC::ModelBuilder
XC::MRMP_Constraint::MRMP_Constraint(int tag, const ID &retainedNodes, int nodeConstr, const Matrix &constr,const ID &constrainedDOF)
  : MP_ConstraintBase(tag, nodeConstr, constr, constrainedDOF, CNSTRNT_TAG_MRMP_Constraint), retainedNodeTags(retainedNodes) {}

XC::MRMP_Constraint::MRMP_Constraint(int tag,const Element &elem, const Node &node, const ID &constrainedDOF)
  : MP_ConstraintBase(tag, node.getTag(), constrainedDOF, CNSTRNT_TAG_MRMP_Constraint)
  {
    retainedNodeTags= elem.getNodePtrs().getExternalNodes();
    const size_t numNodos= retainedNodeTags.Size();
    const size_t numGdls= constrainedDOF.Size();
    constraintMatrix= Matrix(numGdls,numNodos*numGdls);
    Pos3d pos= node.getPosInicial3d(); //XXX Time dependence?
    const Vector interpolationFactors= elem.getInterpolationFactors(pos);
    for(size_t j= 0;j<numNodos;j++)
      {
        const int offset= j*numGdls;
        for(size_t i= 0;i<numGdls;i++)
          constraintMatrix(i,offset+i)= interpolationFactors(j);
      }
  }

int XC::MRMP_Constraint::getNumDofGroups(void) const
  { return retainedNodeTags.Size()+1; }//retained(n)+constrained(1)

//! @brief Number of affected DOFs.
int XC::MRMP_Constraint::getNumDofs(void) const
  { return getNumDofGroups()*getConstrainedDOFs().Size();  }

//! @brief Number of affected DOFs for Lagrange handler
//! numConstrainedDOFs+numRetainedDOFs+numLangrangeDOFs.
//! numLagrangeDOFs= numConstrainedDOFs.
int XC::MRMP_Constraint::getNumLagrangeDofs(void) const
  { return getNumDofs()+getConstrainedDOFs().Size();  }

//! @brief Number of retained DOFs.
int XC::MRMP_Constraint::getNumRetainedDofs(void) const
  { return retainedNodeTags.Size()*getConstrainedDOFs().Size();  }

//! @brief Number of constrained DOFs.
int XC::MRMP_Constraint::getNumConstrainedDofs(void) const
  { return getConstrainedDOFs().Size();  }


//! @brief Devuelve verdadero si afecta al nodo cuyo tag se pasa como parámetro.
bool XC::MRMP_Constraint::afectaANodo(int tagNodo) const
  {
    bool retval= false;
    if (tagNodo== getNodeConstrained())
      retval= true;
    else
      {
        const ID &rNodes= getRetainedNodeTags();
        const size_t sz= rNodes.Size();
        for(size_t i= 0;i<sz;i++)
          if(tagNodo==rNodes(i))
            {
 	      retval= true;
              break;
            }
      }
    return retval;
  }

int XC::MRMP_Constraint::applyConstraint(double timeStamp)
  {
    // does nothing MRMP_Constraint objects are time invariant
    return 0;
  }

//! @brief Add reactions to nodes.
//! See "Calculation within MSC/Nastran of the forces transmitted by
//! multipoint constraints (MPC) and the forces generated in support
//! constraints" by E, de la Fuente and J. San Millán (INTA), Spain.
int XC::MRMP_Constraint::addResistingForceToNodalReaction(bool inclInertia)
  {
    const size_t numConstrainedDOFs= constrDOF.Size(); //ID of constrained DOFs at constrained node
    Domain *dom= getDomain();
    Node *cN= dom->getNode(getNodeConstrained());
    const int numNodalDOF = cN->getNumberDOF();

    //Add reaction to constrained node.
    //R_m = F_m= K_mm u_m - P_m (see page 3).
    const Vector cNReaction= cN->getReaction().getComponents(constrDOF);
    Vector Rm(numConstrainedDOFs);
    for(size_t i= 0;i<numConstrainedDOFs;i++)
      Rm(i)= -cNReaction(i);

    Vector tmp(numNodalDOF);
    tmp.putComponents(Rm,constrDOF);
    cN->addReactionForce(tmp,1.0);

    //Add reaction to retained nodes.
    const size_t numRetainedNodes= retainedNodeTags.Size();
    Vector Rn(numConstrainedDOFs*numRetainedNodes);
    //R_n = -G^T R_m (see page 4).
    Rn.addMatrixTransposeVector(1.0,constraintMatrix,Rm,-1.0);
    Vector RnNod(numConstrainedDOFs);
    for(size_t j= 0;j<numRetainedNodes;j++)
      {
        const int offset= j*numConstrainedDOFs;
        RnNod.Extract(Rn,offset,1.0);
        tmp.Zero();
        tmp.putComponents(RnNod,constrDOF);
        Node *rN= dom->getNode(retainedNodeTags(j));
        rN->addReactionForce(tmp,1.0);
      }
    if(inclInertia)
      std::cerr << "ERROR; MRMP_Constraint::addResistingForceToNodalReaction"
                << " inertia effects not yet implemented." << std::endl;
    return 0;
  }


//! @brief Envía los datos a través del canal que se pasa como parámetro.
int XC::MRMP_Constraint::sendData(CommParameters &cp)
  {
    int res= MP_ConstraintBase::sendData(cp);
    res+= cp.sendID(retainedNodeTags,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::MRMP_Constraint::recvData(const CommParameters &cp)
  {
    int res= MP_ConstraintBase::recvData(cp);
    res+= cp.receiveID(retainedNodeTags,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::MRMP_Constraint::sendSelf(CommParameters &cp)
  {
    inicComm(10);
    int result= sendData(cp);
    const int dataTag= getDbTag();
    result = cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "WARNING MRMP_Constraint::sendSelf - error sending ID data\n";
    return result;
  }


//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::MRMP_Constraint::recvSelf(const CommParameters &cp)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "MRMP_Constraint::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }


void XC::MRMP_Constraint::Print(std::ostream &s, int flag)
  {     
    s << "MRMP_Constraint: " << this->getTag() << "\n";
    s << "\tNode Constrained: " << getNodeConstrained();
    s << " retained nodes: " << getRetainedNodeTags() ;
    s << " constrained dof: " << constrDOF;    
    s << " constraint matrix: " << constraintMatrix << "\n";
  }


//! @brief Interfaz con VTK.
int XC::MRMP_Constraint::getVtkCellType(void) const
  {
    std::cerr << "MRMP_Constraint::getVtkCellType; no implementada." << std::endl;
    return VTK_EMPTY_CELL;
  }

//! @brief Interfaz con el formato MED de Salome.
int XC::MRMP_Constraint::getMEDCellType(void) const
  { 
    std::cerr << "MRMP_Constraint::getMEDCellType; no implementada." << std::endl;
    return MED_SEG2; 
  }

