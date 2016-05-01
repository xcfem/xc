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
//ConstraintLoader.cc

#include "ConstraintLoader.h"
#include "domain/domain/Domain.h"
#include "domain/constraints/SP_Constraint.h"
#include "domain/constraints/MP_Constraint.h"
#include "domain/constraints/MRMP_Constraint.h"
#include "domain/constraints/EqualDOF.h"
#include "domain/constraints/RigidBeam.h"
#include "domain/constraints/RigidRod.h"
#include "domain/constraints/RigidDiaphragm.h"
#include "domain/constraints/GlueNodeToElement.h"


#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"


//! @brief Constructor por defecto.
XC::ConstraintLoader::ConstraintLoader(Preprocessor *owr)
  : Loader(owr), tag_sp_constraint(0), tag_mp_constraint(0) {}

//! @grief Agrega al modelo una condición de contorno monopunto.
XC::SP_Constraint *XC::ConstraintLoader::addSP_Constraint(const int &tag_nod,const SP_Constraint &semilla)
  {
    SP_Constraint *sp= semilla.getCopy(tag_sp_constraint);
    tag_sp_constraint++;
    if(sp)
      {
        sp->setNodeTag(tag_nod);
        getDomain()->addSP_Constraint(sp);
        preprocessor->UpdateSets(sp);
      }
    else
      std::cerr << "ConstraintLoader::addSP_Constraint; se produjo un error al crear la coacción." << std::endl;
    return sp;    
  }

//! @grief Agrega al modelo una condición de contorno monopunto.
XC::SP_Constraint *XC::ConstraintLoader::addSP_Constraint(const int &tag_nod,const int &id_gdl,const double &valor)
  {
    SP_Constraint *sp= new SP_Constraint(tag_sp_constraint,tag_nod,id_gdl, valor);
    tag_sp_constraint++;
    if(sp)
      {
        getDomain()->addSP_Constraint(sp);
        preprocessor->UpdateSets(sp);
      }
    else
      std::cerr << "ConstraintLoader::addSP_Constraint; se produjo un error al crear la coacción." << std::endl;
    return sp;    
  }

//! @grief Define una condición de contorno monopunto.
XC::SP_Constraint *XC::ConstraintLoader::newSPConstraint(const int &tag_nod,const int &id_gdl,const double &valor)
  { return addSP_Constraint(tag_nod,id_gdl,valor); }

//! @grief Agrega al modelo una condición de contorno multipunto.
XC::MP_Constraint *XC::ConstraintLoader::newMPConstraint(const int &masterNode, const int &slaveNode, const ID &constrainedDOF, const ID &retainedDOF)
  {
    MP_Constraint *mp= new MP_Constraint(tag_mp_constraint,masterNode,slaveNode,constrainedDOF,retainedDOF);
    tag_mp_constraint++;
    if(mp)
      {
        getDomain()->addMP_Constraint(mp);
        preprocessor->UpdateSets(mp);
      }
    else
      std::cerr << "ConstraintLoader::newMPConstraint; se produjo un error al crear la coacción." << std::endl;
    return mp;
  }

XC::MP_Constraint *XC::ConstraintLoader::newEqualDOF(const int &masterNode, const int &slaveNode, const ID &dofs)
  {
    EqualDOF *mp= new EqualDOF(tag_mp_constraint,masterNode,slaveNode,dofs);
    tag_mp_constraint++;
    if(mp)
      {
        mp->setup(getDomain());
        getDomain()->addMP_Constraint(mp);
        preprocessor->UpdateSets(mp);
      }
    else
      std::cerr << "ConstraintLoader::addMP_Constraint; se produjo un error al crear la coacción." << std::endl;
    return mp;
  }

XC::MP_Constraint *XC::ConstraintLoader::newRigidBeam(const int &masterNode, const int &slaveNode)
  {
    RigidBeam *mp= new RigidBeam(tag_mp_constraint,masterNode,slaveNode);
    tag_mp_constraint++;
    if(mp)
      {
        mp->setup(getDomain());
        getDomain()->addMP_Constraint(mp);
        preprocessor->UpdateSets(mp);
      }
    else
      std::cerr << "ConstraintLoader::newRigidBeam; se produjo un error al crear la coacción." << std::endl;
    return mp;
  }

XC::MP_Constraint *XC::ConstraintLoader::newRigidRod(const int &masterNode, const int &slaveNode)
  {
    RigidRod *mp= new RigidRod(tag_mp_constraint,masterNode,slaveNode);
    tag_mp_constraint++;
    if(mp)
      {
        mp->setup(getDomain());
        getDomain()->addMP_Constraint(mp);
        preprocessor->UpdateSets(mp);
      }
    else
      std::cerr << "ConstraintLoader::newRigidRod; se produjo un error al crear la coacción." << std::endl;
    return mp;
  }

//! @grief Agrega al modelo una condición de contorno multipunto.
XC::MRMP_Constraint *XC::ConstraintLoader::newMRMPConstraint(const ID &retainedNodes, const int &constrainedNode, const ID &constrainedDOF)
  {
    MRMP_Constraint *mrmp= new MRMP_Constraint(tag_mrmp_constraint,retainedNodes,constrainedNode,constrainedDOF);
    tag_mrmp_constraint++;
    if(mrmp)
      {
        getDomain()->addMRMP_Constraint(mrmp);
        preprocessor->UpdateSets(mrmp);
      }
    else
      std::cerr << "ConstraintLoader::newMPConstraint; se produjo un error al crear la coacción." << std::endl;
    return mrmp;
  }

XC::MRMP_Constraint *XC::ConstraintLoader::newGlueNodeToElement(const Node &constrainedNode, const Element &e, const ID &constrainedDOF)
  {
    GlueNodeToElement *mrmp= new GlueNodeToElement(tag_mrmp_constraint,constrainedNode,e,constrainedDOF);
    tag_mrmp_constraint++;
    if(mrmp)
      {
        getDomain()->addMRMP_Constraint(mrmp);
        preprocessor->UpdateSets(mrmp);
      }
    else
      std::cerr << "ConstraintLoader::newGlueNodeToElement; se produjo un error al crear la coacción." << std::endl;
    return mrmp;
  }


//! @brief Elimina la coacción del dominio.
void XC::ConstraintLoader::removeSPConstraint(const int &tagC)
  {
    bool sp= getDomain()->removeSP_Constraint(tagC);
    if(!sp)
      std::cerr << "ConstraintLoader::removeSPConstraint; se produjo un error al borrar la coacción." << std::endl;
  }

XC::ConstraintLoader::~ConstraintLoader(void)
  { clearAll(); }

//! @brief Borra todos los objetos.
void XC::ConstraintLoader::clearAll(void)
  {
    tag_sp_constraint= 0;
    tag_mp_constraint= 0;
  }

//! @brief returns number of single node constraints.
int XC::ConstraintLoader::getNumSPs(void) const
  { return getDomain()->getConstraints().getNumSPs(); }

//! @brief returns numbr of multiple node constraints.
int XC::ConstraintLoader::getNumMPs(void) const
  { return getDomain()->getConstraints().getNumMPs(); }

//! @brief returns numbr of multiple retained node constraints.
int XC::ConstraintLoader::getNumMRMPs(void) const
  { return getDomain()->getConstraints().getNumMRMPs(); }

//! @brief returns number of load patterns.
int XC::ConstraintLoader::getNumLPs(void) const
  { return getDomain()->getConstraints().getNumLoadPatterns(); }


