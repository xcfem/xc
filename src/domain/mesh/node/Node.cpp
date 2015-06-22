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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.20 $
// $Date: 2005/11/23 22:48:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/mesh/node/Node.cpp,v $


// Written: fmk
// Created: 11/96
// Revision: A
//
// This file contains the implementation of the XC::Node class
//
// What: "@(#) Node.h, revA"

#include <domain/mesh/node/Node.h>
#include "domain/component/Parameter.h"
#include <cstdlib>
#include <domain/component/ContinuaReprComponent.h>
#include <utility/matrix/Vector.h>
#include "xc_basic/src/matrices/matrizExpr.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/actor/MovableMatrix.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <cstring>
#include <domain/mesh/element/Information.h>
#include "domain/constraints/SP_Constraint.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/pos_vec/SVD3d.h"
#include "modelador/cad/trf/TrfGeom.h"
#include "modelador/Modelador.h"
#include "modelador/loaders/LoadLoader.h"
#include "modelador/set_mgmt/SetMeshComp.h"
#include "domain/load/NodalLoad.h"
#include "domain/load/pattern/NodeLocker.h"

#include <domain/domain/Domain.h>
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/ElementIter.h"

//
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/nucleo/CmdParser.h"
#include "xc_utils/src/nucleo/python_utils.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
#include "utility/tagged/DefaultTag.h"

std::deque<XC::Matrix> XC::Node::theMatrices;
XC::DefaultTag XC::Node::defaultTag;

// for FEM_Object Broker to use
XC::Node::Node(int theClassTag)
 :MeshComponent(defaultTag++,theClassTag),numberDOF(0), theDOF_GroupPtr(nullptr), 
  disp(), vel(), accel(), unbalLoad(numberDOF), unbalLoadWithInertia(numberDOF), reaction(numberDOF),
  alphaM(0.0), tributaria(0.0)
  {
    // for FEM_ObjectBroker, recvSelf() must be invoked on object
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////
  }


XC::Node::Node(int tag, int theClassTag)
  :MeshComponent(tag,theClassTag),
   numberDOF(0), theDOF_GroupPtr(nullptr),
   disp(), vel(), accel(), 
   unbalLoad(numberDOF), unbalLoadWithInertia(numberDOF), reaction(numberDOF),
   alphaM(0.0), tributaria(0.0)
  {
    defaultTag= tag+1;
    // AddingSensitivity:BEGIN /////////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////
  }

XC::Node::Node(int tag, int ndof, double Crd1)
  :MeshComponent(tag,NOD_TAG_Node),
   numberDOF(ndof), theDOF_GroupPtr(nullptr),
   Crd(1), disp(), vel(), accel(),
   mass(ndof,ndof), unbalLoad(numberDOF), unbalLoadWithInertia(numberDOF), reaction(numberDOF),
   alphaM(0.0), tributaria(0.0)
  {
    defaultTag= tag+1;
    // AddingSensitivity:BEGIN /////////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////

    Crd(0) = Crd1;

    index = -1;
    setup_matrices(theMatrices,ndof);
  }


//! @brief Constructor for 2d nodes
XC::Node::Node(int tag, int ndof, double Crd1, double Crd2)
  :MeshComponent(tag,NOD_TAG_Node),numberDOF(ndof), theDOF_GroupPtr(nullptr),
   Crd(2), disp(), vel(), accel(),
   mass(ndof,ndof), unbalLoad(numberDOF), unbalLoadWithInertia(numberDOF), reaction(numberDOF),
   alphaM(0.0), tributaria(0.0)
  {
    defaultTag= tag+1;
    // AddingSensitivity:BEGIN /////////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////

    Crd(0)= Crd1;
    Crd(1)= Crd2;

    index = -1;
    setup_matrices(theMatrices,ndof);
  }


//! @brief constructor for 3d nodes
XC::Node::Node(int tag, int ndof, double Crd1, double Crd2, double Crd3)
  :MeshComponent(tag,NOD_TAG_Node), numberDOF(ndof), theDOF_GroupPtr(nullptr),
   Crd(3), disp(), vel(), accel(),
   mass(ndof,ndof), unbalLoad(numberDOF), unbalLoadWithInertia(numberDOF), reaction(numberDOF),
   alphaM(0.0), tributaria(0.0)
  {
    defaultTag= tag+1;
    // AddingSensitivity:BEGIN /////////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////

    Crd(0) = Crd1;
    Crd(1) = Crd2;
    Crd(2) = Crd3;

    index= -1;
    setup_matrices(theMatrices,ndof);
  }

//! @brief Constructor.
XC::Node::Node(int tag, int ndof, const Vector &crds)
  :MeshComponent(tag,NOD_TAG_Node),
   numberDOF(ndof), theDOF_GroupPtr(nullptr),
   Crd(crds), disp(), vel(), accel(),
   mass(ndof,ndof), unbalLoad(numberDOF), unbalLoadWithInertia(numberDOF), reaction(numberDOF),
    alphaM(0.0), tributaria(0.0)
  {
    defaultTag= tag+1;
    // AddingSensitivity:BEGIN /////////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////

    index= -1;
    setup_matrices(theMatrices,ndof);
  }

//! @brief  used for domain decomposition & external nodes
//!  copy everything but the mass
//!  we should really set the mass to 0.0
XC::Node::Node(const Node &otherNode, bool copyMass)
  :MeshComponent(otherNode), numberDOF(otherNode.numberDOF), theDOF_GroupPtr(nullptr),
   Crd(otherNode.Crd), disp(otherNode.disp), vel(otherNode.vel), accel(otherNode.accel),
   R(otherNode.R), unbalLoad(otherNode.unbalLoad),
   unbalLoadWithInertia(otherNode.unbalLoadWithInertia), reaction(otherNode.reaction),
   alphaM(otherNode.alphaM), tributaria(otherNode.tributaria), theEigenvectors(otherNode.theEigenvectors),
   conectados(otherNode.conectados), coacciones_freeze(otherNode.coacciones_freeze)
  {
    // AddingSensitivity:BEGIN /////////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////////

    unbalLoad.Zero();

    if(copyMass == true)
      mass= otherNode.mass;

    index = -1;
    setup_matrices(theMatrices,numberDOF);
  }

//! @brief Agrega una componente a la lista de conectados.
void XC::Node::conecta(ContinuaReprComponent *el) const
  { 
    if(el)
      conectados.insert(el);
    else
      std::cerr << "Node::conecta; se intentó conectar un puntero nuelo." << std::endl;
  }

//! @brief Elimina una componente de la lista de conectados.
void XC::Node::desconecta(ContinuaReprComponent *el) const
  {
    std::set<ContinuaReprComponent *>::const_iterator i= conectados.find(el);
    if(i!=conectados.end())
      conectados.erase(i);
  }

XC::Node *XC::Node::getCopy(void) const
  { return new Node(*this,true); }

const bool XC::Node::isDead(void) const
  { return !isAlive(); }

const bool XC::Node::isAlive(void) const
  {
    bool retval= false;
    if(conectados.empty())
      {
        std::cerr << "Node: " << getTag() << " is free." << std::endl;
        retval= true;
      }
    else
      {
        for(std::set<ContinuaReprComponent *>::const_iterator i= conectados.begin();i!=conectados.end();i++)
          {
            const ContinuaReprComponent *ptr= *i;
            if(ptr)
              {
                if(ptr->isAlive())
                  {
                    retval= true;
                    break;
                  }
              }
            else
	      std::cerr << "Node::isAlive; puntero nulo en lista de conectados." << std::endl;
          }
      }
    return retval;
  }

//! @brief Impone valor nulo en todos los grados
//! de libertad del nodo.
void XC::Node::freeze_if_dead(NodeLocker *locker)
  {
    if(isDead())
      {
        const size_t tag_nodo= getTag();
        for(int id_gdl= 0;id_gdl<numberDOF;id_gdl++)
          {
            SP_Constraint *sp= locker->addSP_Constraint(tag_nodo,id_gdl,0.0);
            if(sp)
              coacciones_freeze.insert(sp->getTag());
          }
      }
  }

//! @brief Devuelve un vector con los identificadores de las coacciones 
const XC::ID &XC::Node::get_id_coacciones(void) const
  {
    static ID retval;
    size_t cont= 0;
    retval.resize(coacciones_freeze.size());
    for(std::set<int>::const_iterator i= coacciones_freeze.begin();i!=coacciones_freeze.end();i++,cont++)
      retval[cont]= *i;
    return retval;
  }

void XC::Node::set_id_coacciones(const ID &coacciones)
  {
    const int sz= coacciones.Size();
    for(int i= 0;i<sz;i++)
      coacciones_freeze.insert(coacciones(i));
  }

//! @brief Elimina las coacciones sobre los grados
//! de libertad del nodo creadas por freeze.
void XC::Node::melt_if_alive(NodeLocker *locker)
  {
    if(isAlive())
      {
        if(!coacciones_freeze.empty())
          {
            for(std::set<int>::const_iterator i= coacciones_freeze.begin();i!=coacciones_freeze.end();i++)
              locker->removeSP_Constraint(*i);
            coacciones_freeze.clear();
	    reaction.Zero();
          }
      }
  }

const bool XC::Node::isFrozen(void) const
  { return !coacciones_freeze.empty(); }

const bool XC::Node::isFree(void) const
  { return conectados.empty(); }

void XC::Node::kill(void)
  { std::cerr << "No se debe llamar a Node::kill" << std::endl; }

void XC::Node::alive(void)
  { std::cerr << "No se debe llamar a Node::alive" << std::endl; }

//! @brief Procesa los comandos correspondientes a cargas sobre nodo
bool XC::Node::procesa_load(const std::string &cmd,CmdStatus &status)
  {
    Modelador *mdlr= GetModelador();
    if(mdlr)
      {
        MapLoadPatterns &casos= mdlr->getLoadLoader().getLoadPatterns();

        if(cmd == "load")
          {
            const Vector v= Vector(convert_to_vector_double(interpretaVectorAny(status.GetString())));
            const int sz= v.Size();
            if(sz==numberDOF)
              {
                LoadPattern *lp= casos.getCurrentLoadPatternPtr();
                if(lp)
                  lp->newNodalLoad(getTag(),v);
                else
		  std::cerr << "Node::procesa_load; no se encontó el caso de carga: "
                            << casos.getCurrentLoadPatternId() << std::endl;
              }
            else
              std::cerr << "(Node) Procesando comando: " << cmd 
                        << "; el vector: '" << v 
                        << "' debería ser de dimensión " << numberDOF << ".\n";
            return true;
          }
        else if(cmd == "load_svd3d") //Sistema de vectores deslizantes en espacio3d.
          {
            const SVD3d tmp= interpretaSVD3d(status.GetString());
            const Vector3d RT= tmp.getResultante();
            const Pos3d pt= getPosInicial3d();
            const Vector3d M= tmp.getMomento(pt);
            Vector tmp_load(6);
            tmp_load[0]= RT.x();
            tmp_load[1]= RT.y();
            tmp_load[2]= RT.z();
            tmp_load[3]= M.x();
            tmp_load[4]= M.y();
            tmp_load[5]= M.z();
            if(numberDOF==6)
              {
                LoadPattern *lp= casos.getCurrentLoadPatternPtr();
                if(lp)
                  lp->newNodalLoad(getTag(),tmp_load);
                else
		  std::cerr << "Node::procesa_load; no se encontó el caso de carga: "
                            << casos.getCurrentLoadPatternId() << std::endl;
              }
            else
              std::cerr << "(Node) Procesando comando: " << cmd << "; la carga debe ser de dimensión " << numberDOF << ".\n";
            return true;
          }
        else
          return false;
      }
    return false;
  }

//! @brief Lee un objeto XC::Node desde archivo
//!
//! Soporta los comandos:
//! -coo: Lee las coordenadas del nodo.
bool XC::Node::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(Node) Procesando comando: " << cmd << std::endl;
    if(cmd == "coo") //Coordenadas del nodo.
      {
        std::vector<double> tmp= crea_vector_double(status.GetString());
        const int nc= std::min(tmp.size(),static_cast<size_t>(Crd.Size())); //No. de coordenadas.
        if(nc!=Crd.Size())
          {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
            std::cerr << "Node::procesa_comando; error procesando comando: "
                      << cmd << " se leyeron " << nc
                      << " valores, se esperaban: " << Crd.Size() << posLectura << std::endl;
          }
        for(int i= 0;i<nc;i++)
          Crd(i)= tmp[i];
        return true;
      }
    else if(cmd == "pos3d") //Posición del nodo.
      {
        const Pos3d tmp= interpretaPos3d(status.GetString());
        const int nc= std::min(Crd.Size(),3); //No de coordenadas.
        for(int i= 0;i<nc;i++)
          Crd(i)= tmp(i+1);
        return true;
      }
//     else if(cmd == "pos2d") //Posición del nodo.
//       {
//         const Pos3d tmp= interpretaPos2d(status.GetString());
//         const int nc= std::min(Crd.Size(),2); //No de coordenadas.
//         for(int i= 0;i<nc;i++)
//           Crd(i)= tmp(i+1);
//         return true;
//       }
    else if(cmd == "mass")
      {
        m_double tmp= interpretaMDouble(status.GetString());
        setMass(m_double_to_matrix(tmp));
        return true;
      }
    else if(cmd == "fix")
      {
        const std::vector<int> idGdls= status.Parser().getIndicesAsInts(this);
        const std::vector<double> valores= convert_to_vector_double(interpretaVectorAny(status.GetString()));
        fix(idGdls,valores);
        return true;
      }
    else if(procesa_load(cmd,status))
      { return true; }
    else if(cmd == "setTrialDisp")
      {
        std::clog << "Node; el comando: " << cmd
                  << " está pensado para pruebas." << std::endl; 
        const std::vector<double> despl_nodo= crea_vector_double(status.GetString());
        const int sz= std::min(int(despl_nodo.size()),numberDOF);
        for(int i= 0;i<sz;i++)
          setTrialDispComponent(despl_nodo[i],i);
        return true;
      }
    else if(cmd == "setNumColR")
      {
        std::clog << "Node; el comando: " << cmd
                  << " está pensado para pruebas." << std::endl; 
        const size_t nc= interpretaSize_t(status.GetString());
        setNumColR(nc);
        return true;
      }
    else if(cmd == "disp")
      {
        disp.LeeCmd(status);
        return true;
      }
    else if(cmd == "vel")
      {
        vel.LeeCmd(status);
        return true;
      }
    else if(cmd == "accel")
      {
        accel.LeeCmd(status);
        return true;
      }
    else
      return MeshComponent::procesa_comando(status);
  }

//! @brief destructor
XC::Node::~Node(void)
  {
    if(theDOF_GroupPtr) theDOF_GroupPtr->resetNodePtr();
  }

XC::DefaultTag &XC::Node::getDefaultTag(void)
  { return defaultTag; }

//! @brief Introduce en el nodo una condición de contorno
//! como la que se pasa como parámetro.
XC::SP_Constraint *XC::Node::fix(const SP_Constraint &semilla)
  { return GetModelador()->getConstraintLoader().addSP_Constraint(getTag(),semilla); }

//! @brief Impone desplazamientos prescritos en los grados de libertad
//! que se pasan como parámetro.
void XC::Node::fix(const std::vector<int> &idGdls,const std::vector<double> &valores)
  {
    if(getDomain())
      {
        ConstraintLoader &cl= GetModelador()->getConstraintLoader();
        const int sz= std::min(idGdls.size(),valores.size());
        if(valores.size()<idGdls.size())
	  std::cerr << "El vector de desplazamientos prescritos"
                    << " debería tener dimensión " << idGdls.size()
                    << std::endl;
        if(sz)
          {
            for(int i= 0;i<sz;i++)
              cl.addSP_Constraint(getTag(),idGdls[i],valores[i]);
          }
        else
          std::cerr << "Node::coarta_movs; "
                    << "la lista de grados de libertad está vacia." << std::endl;
      }
    else
      std::cerr << "Node::coarta_movs; "
                << "el puntero al dominio es nulo. Se ignora la coacción." << std::endl;
  }

//! @brief Impone desplazamientos prescritos en los grados de libertad
//! que se pasan como parámetro.
void XC::Node::fix(const ID &idGdls,const Vector &valores)
  {
    std::vector<int> ids= id_to_std_vector(idGdls);
    std::vector<double> vs= vector_to_std_vector(valores);
    fix(ids,vs);
  }

//! @brief Devuelve el número de grados de libertad del nodo.
int XC::Node::getNumberDOF(void) const
  { return numberDOF; }


void XC::Node::setDOF_GroupPtr(DOF_Group *theDOF_Grp)
  {
    // set the DOF_Group pointer
    theDOF_GroupPtr= theDOF_Grp;
  }


XC::DOF_Group *XC::Node::getDOF_GroupPtr(void)
  {
    // return the XC::DOF_Group pointer
    return theDOF_GroupPtr;
  }


//! @brief Devuelve la dimensión del nodo (1,2 ó 3).
size_t XC::Node::getDim(void) const
  { return Crd.Size(); }

//! @brief return the vector of nodal coordinates
const XC::Vector &XC::Node::getCrds(void) const
  { return Crd; }

//! @brief return the vector of nodal coordinates
XC::Vector &XC::Node::getCrds(void)
  { return Crd; }

//! @brief Devuelve las coordenadas del nodo en un espacio 3d.
XC::Vector XC::Node::getCrds3d(void) const
  {
    Vector retval(3,0.0);
    const size_t sz= getDim();
    if(sz>0)
      {
        retval[0]= Crd[0];
        if(sz>1)
          {
            retval[1]= Crd[1];
            if(sz>2)
              retval[2]= Crd[2];
          }
      }
    return retval;
  }

//! @brief Devuelve la posición inicial del nodo.
Pos2d XC::Node::getPosInicial2d(void) const
  {
    Pos2d retval;
    const size_t sz= getDim();
    if(sz==1)
      retval= Pos2d(Crd[0],0.0);
    else if(sz>=2)
      retval= Pos2d(Crd[0],Crd[1]);
    return retval;
  }

//! @brief Devuelve la posición inicial del nodo.
Pos3d XC::Node::getPosInicial3d(void) const
  {
    Pos3d retval;
    const size_t sz= getDim();
    if(sz==1)
      retval= Pos3d(Crd[0],0.0,0.0);
    else if(sz==2)
      retval= Pos3d(Crd[0],Crd[1],0.0);
    else if(sz==3)
      retval= Pos3d(Crd[0],Crd[1],Crd[2]);
    return retval;
  }

//! @brief Devuelve la posición final del nodo.
Pos2d XC::Node::getPosFinal2d(void) const
  {
    const Vector &d= getDisp();
    Pos2d retval;
    const size_t sz= getDim();
    if(sz==1)
      {
        if(numberDOF==1)
          retval= Pos2d(Crd[0]+d[0],0.0);
        else if(numberDOF==2)
          retval= Pos2d(Crd[0]+d[0],d[1]);
      }
    else if(sz==2)
      {
        if(numberDOF==1)
          retval= Pos2d(Crd[0]+d[0],Crd[1]);
        else if((numberDOF==2) || (numberDOF==3) || (numberDOF==6))
          retval= Pos2d(Crd[0]+d[0],Crd[1]+d[1]);
      }
    return retval;
  }

//! @brief Devuelve la posición final del nodo.
Pos3d XC::Node::getPosFinal3d(void) const
  {
    const Vector &d= getDisp();
    Pos3d retval;
    const size_t sz= getDim();
    if(sz==1)
      {
        if(numberDOF==1)
          retval= Pos3d(Crd[0]+d[0],0.0,0.0);
        else if(numberDOF==2)
          retval= Pos3d(Crd[0]+d[0],d[1],0.0);
        else if((numberDOF==3) || (numberDOF==6))
          retval= Pos3d(Crd[0]+d[0],d[1],d[2]);
      }
    else if(sz==2)
      {
        if(numberDOF==1)
          retval= Pos3d(Crd[0]+d[0],Crd[1],0.0);
        else if((numberDOF==2) || (numberDOF==3))
          retval= Pos3d(Crd[0]+d[0],Crd[1]+d[1],0.0);
      }
    else if(sz==3)
      {
        if(numberDOF==1)
          retval= Pos3d(Crd[0]+d[0],Crd[1],Crd[2]);
        else if(numberDOF==2)
          retval= Pos3d(Crd[0]+d[0],Crd[1]+d[1],Crd[2]);
        else if((numberDOF==3) || (numberDOF==6))
          retval= Pos3d(Crd[0]+d[0],Crd[1]+d[1],Crd[2]+d[2]);
      }
    return retval;
  }

//! @brief Extrae del vector d las componentes XYZ de la traslación del nodo
XC::Vector extrae_traslacion(const XC::Vector &d,const size_t &dim,const size_t numDOF)
  {
    XC::Vector retval(3,0.0);
    if(dim==1)
      {
        if(numDOF==1) //Elast 1D.
          { retval[0]= d[0]; }
        else if(numDOF==2)
          { retval[0]= d[0]; retval[1]=d[1]; }
        else if((numDOF==3) || (numDOF==6)) //Elast 3D o RM 3D.
          { retval[0]= d[0]; retval[1]=d[1]; retval[2]=d[2]; }
      }
    else if(dim==2)
      {
        if(numDOF==1) //Elast 1D.
          { retval[0]= d[0]; }
        else if((numDOF==2) || (numDOF==3)) //Elast 2D o RM 2D.
          { retval[0]= d[0]; retval[1]=d[1]; }
      }
    else if(dim==3)
      {
        if(numDOF==1) //Elast 1D.
          { retval[0]= d[0]; }
        else if(numDOF==2) //Elast 2D.
          { retval[0]= d[0]; retval[1]=d[1]; }
        else if((numDOF==3) || (numDOF==6)) //Elast 3D o RM 3D.
          { retval[0]= d[0]; retval[1]=d[1]; retval[2]=d[2]; }
      }
    return retval;
  }

//! @brief Extrae del vector d las componentes XYZ de la rotación del nodo
XC::Vector extrae_rotacion(const XC::Vector &d,const size_t &dim,const size_t numDOF)
  {
    XC::Vector retval(3,0.0);
    if((dim==2) && (numDOF==3)) //RM 2D.
      {retval[3]= d[2];}
    else if ((dim==3) && (numDOF==6)) //RM 3D.
      {retval[0]= d[3]; retval[1]=d[4]; retval[2]=d[5];}
    return retval;
  }

//! @brief Devuelve las componentes XYZ de la traslación del nodo
XC::Vector XC::Node::getDispXYZ(void) const
  { return extrae_traslacion(getDisp(),getDim(),numberDOF); }

//! @brief Devuelve las componentes XYZ de la rotación del nodo
XC::Vector XC::Node::getRotXYZ(void) const
  { return extrae_rotacion(getDisp(),getDim(),numberDOF); }

//! @brief Devuelve las componentes XYZ de la velocidad de traslación del nodo
XC::Vector XC::Node::getVelXYZ(void) const
  { return extrae_traslacion(getVel(),getDim(),numberDOF); }

//! @brief Devuelve las componentes XYZ de la velocidad de rotación del nodo
XC::Vector XC::Node::getOmegaXYZ(void) const
  { return extrae_rotacion(getVel(),getDim(),numberDOF); }

//! @brief Devuelve las componentes XYZ de la aceleración de traslación del nodo
XC::Vector XC::Node::getAccelXYZ(void) const
  { return extrae_traslacion(getAccel(),getDim(),numberDOF); }

//! @brief Devuelve las componentes XYZ de la aceleración de rotación del nodo
XC::Vector XC::Node::getAlphaXYZ(void) const
  { return extrae_rotacion(getAccel(),getDim(),numberDOF); }

//! @brief Devuelve el cuadrado de la distancia desde el nodo al punto que
//! se pasa como parámetro.
double XC::Node::getDist2(const Pos2d &p,bool geomInicial) const
  { return getDist2(To3dXY2d(p),geomInicial); }

//! @brief Devuelve la distancia desde el nodo al punto que
//! se pasa como parámetro.
double XC::Node::getDist(const Pos2d &p,bool geomInicial) const
  { return sqrt(getDist2(p,geomInicial)); }

//! @brief Devuelve el cuadrado de la distancia desde el nodo al punto que
//! se pasa como parámetro.
double XC::Node::getDist2(const Pos3d &p,bool geomInicial) const
  {
    if(geomInicial)
      return ::dist2(getPosInicial3d(),p);
    else
      return ::dist2(getPosFinal3d(),p);
  }

//! @brief Devuelve la distancia desde el nodo al punto que
//! se pasa como parámetro.
double XC::Node::getDist(const Pos3d &p,bool geomInicial) const
  { return sqrt(getDist2(p,geomInicial)); }

//! @brief Asigna la posición del nodo.
void XC::Node::setPos(const Pos3d &p)
  {
    const size_t sz= getDim();
    if(sz==1)
      Crd[0]= p.x();
    else if(sz==2)
      {
        Crd[0]= p.x();
        Crd[1]= p.y();
      }
    else if(sz==3)
      {
        Crd[0]= p.x();
        Crd[1]= p.y();
        Crd[2]= p.z();
      }
  }

//! @brief Aplica a nodo la transformación cuyo índice se pasa como parámetro.
void XC::Node::Transforma(const TrfGeom &trf)
  {
    static Pos3d p;
    setPos(trf.Transforma(getPosInicial3d()));
  }

//! @brief Devuelve la posición de un nodo.
Pos3d XC::pos_nodo(const Node &nod,bool geomInicial)
  {
    if(geomInicial)
      return nod.getPosInicial3d();
    else
      return nod.getPosFinal3d();
  }


//! @brief Devuelve el desplazamiento del nodo
const XC::Vector &XC::Node::getDisp(void) const
  { return disp.getData(numberDOF); }

const XC::Vector &XC::Node::getVel(void) const
  { return vel.getData(numberDOF); }

//! @brief Devuelve la aceleración
const XC::Vector &XC::Node::getAccel(void) const
  { return accel.getData(numberDOF); }


/* *********************************************************************
**
**   Methods to return the trial response quantities similar to committed
**
** *********************************************************************/

const XC::Vector &XC::Node::getTrialDisp(void) const
  { return disp.getTrialData(numberDOF); }

const XC::Vector &XC::Node::getTrialVel(void) const
  { return vel.getTrialData(numberDOF); }

const XC::Vector &XC::Node::getTrialAccel(void) const
  { return accel.getTrialData(numberDOF); }

const XC::Vector &XC::Node::getIncrDisp(void) const
  { return disp.getIncrDisp(numberDOF); }

const XC::Vector &XC::Node::getIncrDeltaDisp(void) const
  { return disp.getIncrDeltaDisp(numberDOF); }

int XC::Node::setTrialDispComponent(double value, int dof)
  { return disp.setTrialDispComponent(numberDOF,value,dof); }

int XC::Node::setTrialDisp(const Vector &newTrialDisp)
  { return disp.setTrialDisp(numberDOF,newTrialDisp); }

int XC::Node::setTrialVel(const Vector &newTrialVel)
  { return vel.setTrialData(numberDOF,newTrialVel); }

int XC::Node::setTrialAccel(const Vector &newTrialAccel)
  { return accel.setTrialData(numberDOF,newTrialAccel); }

int XC::Node::incrTrialDisp(const Vector &incrDispl)
  { return disp.incrTrialDisp(numberDOF,incrDispl); }

int XC::Node::incrTrialVel(const Vector &incrVel)
  { return vel.incrTrialData(numberDOF,incrVel); }

int XC::Node::incrTrialAccel(const Vector &incrAccel)
  { return accel.incrTrialData(numberDOF,incrAccel); }

void XC::Node::zeroUnbalancedLoad(void)
  { unbalLoad.Zero(); }

//! @brief Añade al vector de carga desequilibrada el que se para 
int XC::Node::addUnbalancedLoad(const Vector &add, double fact)
  {
    int retval= -1;
    // check vector arg is of correct size
    if(add.Size() != numberDOF)
      {
        std::cerr << "Node::addunbalLoad - load to add of incorrect size ";
        std::cerr << add.Size() << " should be " <<  numberDOF << std::endl;
      }
    else
      {
        if(isDead())
          {
            std::cerr << nombre_clase() 
                      << "; se intentó cargar el nodo "
                      << getTag() << " que está desactivado." 
                      << std::endl;
          }
        else // add fact*add to the unbalanced load
          {
            unbalLoad.addVector(1.0, add,fact);
            retval= 0;
          }
      }
    return retval;
  }



//! @brief Añade la fuerza de inercia al vector de carga desequilibrada.
int XC::Node::addInertiaLoadToUnbalance(const Vector &accelG, double fact)
  {
    // simply return if node has no mass or R matrix
    if(R.Nula() || isDead())
      return 0;

    // otherwise we must determine MR accelG
    if(accelG.Size() != R.noCols())
      {
        std::cerr << "Node::addInertiaLoadToUnbalance - accelG not of correct dimension";
        return -1;
      }

    // form - fact * M*R*accelG and add it to the unbalanced load
    //unbalLoad-= ((mass) * R * accelG)*fact;
    Matrix MR(mass.noRows(), R.noCols());
    MR.addMatrixProduct(0.0, mass, R, 1.0);
    unbalLoad.addMatrixVector(1.0, MR, accelG, -fact);
    return 0;
  }



int XC::Node::addInertiaLoadSensitivityToUnbalance(const XC::Vector &accelG, double fact, bool somethingRandomInMotions)
  {
    // simply return if node has no R matrix
    if(R.Nula())
      return 0;

    // otherwise we must determine MR accelG
    if(accelG.Size() != R.noCols())
      {
        std::cerr << "Node::addInertiaLoadToUnbalance - accelG not of correct dimension";
        return -1;
      }


    // form - fact * M*R*accelG and add it to the unbalanced load
    //unbalLoad-= ((mass) * R * accelG)*fact;

    Matrix massSens(mass.noRows(),mass.noCols());
    if(parameterID != 0)
      { massSens(parameterID-1,parameterID-1) = 1.0; }

    Matrix MR(mass.noRows(), R.noCols());

    if(somethingRandomInMotions)
      { MR.addMatrixProduct(0.0,mass,R, 1.0); }
    else
      { MR.addMatrixProduct(0.0, massSens, R, 1.0); }
    unbalLoad.addMatrixVector(1.0, MR, accelG, -fact);
    return 0;
  }


//! @brief Crea el vector de carga desequilibrada.
const XC::Vector &XC::Node::getUnbalancedLoad(void)
  { return unbalLoad; }


//! @brief Devuelve la carga no equilibrada incluyendo las fuerzas de inercia.
const XC::Vector &XC::Node::getUnbalancedLoadIncInertia(void)
  {
    unbalLoadWithInertia= this->getUnbalancedLoad();

    if(!mass.Nula())
      {
        const Vector &theAccel = getTrialAccel(); // in case accel not created
        unbalLoadWithInertia.addMatrixVector(1.0, mass, theAccel, -1.0);

        if(alphaM != 0.0)
          {
            const Vector &theVel= getTrialVel(); // in case vel not created
            unbalLoadWithInertia.addMatrixVector(1.0, mass, theVel, -alphaM);
          }
      }
    return unbalLoadWithInertia;
  }

//! @brief Consuma el estado del nodo.
int XC::Node::commitState(void)
  {
    disp.commitState(numberDOF);
    vel.commitState(numberDOF);
    accel.commitState(numberDOF);
    return 0;
  }


//! @brief Devuelve el nodo al estado correspondiente al último consumado.
int XC::Node::revertToLastCommit(void)
  {
    disp.revertToLastCommit(numberDOF);
    vel.revertToLastCommit(numberDOF);
    accel.revertToLastCommit(numberDOF);
    reaction.Zero();
    return 0;
  }


//! @brief Devuelve el nodo a su estado inicial.
int XC::Node::revertToStart(void)
  {
    disp.revertToStart(numberDOF);
    vel.revertToStart(numberDOF);
    accel.revertToStart(numberDOF);

    unbalLoad.Zero();
    reaction.Zero();

    // AddingSensitivity: BEGIN /////////////////////////////////
    dispSensitivity.Zero();
    velSensitivity.Zero();
    accSensitivity.Zero();
    // AddingSensitivity: END ///////////////////////////////////

    // if we get here we are done
    return 0;
  }

//! @brief Devuelve la matriz de masas del nodo.
const XC::Matrix &XC::Node::getMass(void)
  { return mass; }

//! @brief Devuelve el coeficiente de amortiguamiento de Rayleigh.
int XC::Node::setRayleighDampingFactor(double alpham)
  {
    alphaM = alpham;
    return 0;
  }


//! @brief Devuelve la matriz de amortiguamiento del nodo.
const XC::Matrix &XC::Node::getDamp(void)
  {
    // make sure it was created before we return it
    if(alphaM == 0.0)
      {
        theMatrices[index].Zero();
        return theMatrices[index];
      }
    else
      {
        Matrix &result= theMatrices[index];
        result= mass;
        result*= alphaM;
        return result;
      }
  }

//! @brief Agrega a la magnitud tributaria la longitud,
//! área o volumen que se pasa como parámetro.
void XC::Node::addTributaria(const double &t) const
  { tributaria+= t; }

//! @brief Anula la magnitud tributaria (longitud, área o volumen).
void XC::Node::resetTributaria(void) const
  { tributaria= 0; }

//! @brief Devuelve la magnitud tributaria (longitud, área o volumen).
const double &XC::Node::getTributaria(void) const
  { return tributaria; }

const XC::Matrix &XC::Node::getDampSensitivity(void)
  {
    // make sure it was created before we return it
    if(alphaM == 0.0)
      {
        theMatrices[index].Zero();
        return theMatrices[index];
      }
    else
      {
        Matrix &result= theMatrices[index];
        result.Zero();
        //result = *mass;
        //result *= alphaM;
        return result;
      }
  }

//! @brief Asigna la matriz de masas al nodo.
int XC::Node::setMass(const Matrix &newMass)
  {
    // check right size
    if(newMass.noRows() != numberDOF || newMass.noCols() != numberDOF)
      {
        std::cerr << "Node::setMass - matrices de dimensiones incompatibles"
                  << " se esperaba una matriz de dimensiones: " << numberDOF << 'x' << numberDOF
                  << std::endl;
        return -1;
      }
    mass= newMass;
    return 0;
  }

int XC::Node::setNumColR(int numCol)
  {
    if(R.noCols() != numCol)
      R= Matrix(numberDOF, numCol);
    R.Zero();
    return 0;
  }

int XC::Node::setR(int row, int col, double Value)
  {
    // ensure row, col in range (matrix assignment will catch this - extra work)
    if(row < 0 || row > numberDOF || col < 0 || col > R.noCols())
      {
        std::cerr << "Node:setR() - row, col index out of range\n";
        return -1;
      }
    // do the assignment
    R(row,col)= Value;
    return 0;
  }



const XC::Vector &XC::Node::getRV(const Vector &V)
  {
    // we store the product of RV in unbalLoadWithInertia

    // see if quick return , i.e. R == 0
    if(R.Nula())
      {
        unbalLoadWithInertia.Zero();
        return unbalLoadWithInertia;
      }

    // check dimesions of R and V
    if(R.noCols() != V.Size())
      {
        std::cerr << "WARNING XC::Node::getRV() - R and V of incompatable dimesions\n";
        std::cerr << "R: " << R << "V: " << V;
        unbalLoadWithInertia.Zero();
        return unbalLoadWithInertia;
      }

    // determine the product
    unbalLoadWithInertia.addMatrixVector(0.0, R, V, 1.0);
    return unbalLoadWithInertia;
  }


//! @brief Dimensiona la matriz que contiene a los autovectores.
int XC::Node::setNumEigenvectors(int numVectorsToStore)
  {
    // ensure a positive number of vectors
    if(numVectorsToStore <= 0)
      {
        std::cerr << "Node::setNumEigenvectors() - " << numVectorsToStore << " < 0\n";
        return -1;
      }

    // if matrix not yet assigned or not of correct size delete old and create new
    if(getNumModes() != numVectorsToStore)
      theEigenvectors= Matrix(numberDOF, numVectorsToStore);
    else // zero the eigenvector matrix
      theEigenvectors.Zero();
    return 0;
  }

//! @brief Asigna el autovector eigenvector al modo mode.
int XC::Node::setEigenvector(int mode, const Vector &eigenvector)
  {
    if(getNumModes() < mode)
      {
        std::cerr << "Node::setEigenvectors() - mode " << mode << " invalid\n";
        return -1;
      }

    if(eigenvector.Size() != numberDOF)
      {
        std::cerr << "Node::setEigenvectors() - eigenvector of incorrect size\n";
        return -2;
      }
    // set the values
    for(int i=0; i<numberDOF; i++)
      theEigenvectors(i, mode-1)= eigenvector(i);
    return 0;
  }

//! @brief Devuelve la pulsación correspondiente al modo i.
double XC::Node::getPulsacion(int mode) const
  {
    const Domain *dom= getDomain();
    assert(dom);
    return dom->getPulsacion(mode);
  }

//! @brief Devuelve las pulsaciones obtenidas en el análisis modal.
XC::Vector XC::Node::getPulsaciones(void) const
  {
    const Domain *dom= getDomain();
    assert(dom);
    const int nm= getNumModes();
    Vector retval(nm);
    for(int i= 1;i<=nm;i++)
      retval[i-1]= dom->getPulsacion(i);
    return retval;
  }

//! @brief Devuelve los autovectores correspondientes al nodo.
const XC::Matrix &XC::Node::getEigenvectors(void)
  { return theEigenvectors; }

//! @brief Devuelve el autovector correspondiente al modo i
XC::Vector XC::Node::getEigenvector(int mode) const
  { return theEigenvectors.getCol(mode-1); }

//! @brief Devuelve el autovector correspondiente al modo i
//! normalizado de modo que la componente máxima valga 1 (norma_infinito).
XC::Vector XC::Node::getNormalizedEigenvector(int mode) const
  { return normalize_inf(getEigenvector(mode)); }

//! @brief Devuelve una matriz con los autovectores normalizados colocados
//! por columnas (norma_infinito).
XC::Matrix XC::Node::getNormalizedEigenvectors(void) const
  {
    const int nFilas= theEigenvectors.noRows();
    const int nm= getNumModes();
    Matrix retval(nFilas,nm);
    for(int j= 0;j<nm;j++)
      {
        const Vector eigenvector= getNormalizedEigenvector(j+1);
        for(int i= 0;i<nFilas;i++)
          retval(i,j)= eigenvector(i);
      }
    return retval;
  }

//! @brief Devuelve el factor de participación modal
//! correspondiente al modo i.
double XC::Node::getModalParticipationFactor(int mode) const
  {
    const Domain *dom= getDomain();
    assert(dom);
    return dom->getModalParticipationFactor(mode);
  }

//! @brief Devuelve los factores de participación modal.
XC::Vector XC::Node::getModalParticipationFactors(void) const
  {
    const int nm= getNumModes();
    Vector retval(nm);
    for(int i= 1;i<=nm;i++)
      retval[i-1]= getModalParticipationFactor(i);
    return retval;
  }

//! @brief Devuelve el factor de participación modal
//! correspondiente al modo i. Si el parámetro gdls
//! no está vacío «proyecta» el modo i sobre
//! los gdls indicados en el conjunto. Se escribió para poder
//! expresar ls participación según un gdl
//! (ver test de verificación test_cqc_01.xc).
//! LOS RESULTADOS DE ESTA FUNCIÓN NO COINCIDEN CON LOS
//! OBTENIDOS DEL PROBLEMA DE AUTOVALORES
double XC::Node::getModalParticipationFactor(int mode,const std::set<int> &gdls) const
  {
    double retval= 0;
    if(gdls.empty())
      { retval= getModalParticipationFactor(mode); }
    else
      {
        const Vector ev= getEigenvector(mode);
        const int sz= ev.Size();
        double num= 0;
        if((mass.noRows()!=sz) || (mass.noCols()!=sz))
          std::cerr << "Node::getModalParticipationFactor; ERROR "
                    << "el autovector es de dimensión " << sz
                    << " y la matriz de masas de " << mass.noRows()
                    << "x" << mass.noCols() << ".\n";
        Vector J(sz,0.0);
        for(std::set<int>::const_iterator i= gdls.begin();i!=gdls.end();i++)
          J[*i]= 1;
        num= dot(ev,(mass*J));
        if(num!=0)
          {
            const double denom= dot(ev,(mass*ev));
            retval= num/denom;
          }
      }
    return retval;
  }

//! @brief Devuelve los factores de participación modal. Si el parámetro gdls
//! no está vacío «proyecta» el modo i sobre
//! los gdls indicados en el conjunto.
XC::Vector XC::Node::getModalParticipationFactors(const std::set<int> &gdls) const
  {
    const int nm= getNumModes();
    Vector retval(nm);
    for(int i= 1;i<=nm;i++)
      retval[i-1]= getModalParticipationFactor(i,gdls);
    return retval;
  }

//! @brief Devuelve los factores de participación modal. Si el parámetro gdls
//! no está vacío «proyecta» el modo i sobre
//! los gdls indicados en el conjunto.
XC::Vector XC::Node::getModalParticipationFactorsForGdls(const boost::python::list &l) const
  {
    std::set<int> tmp= set_int_from_py_list(l);
    return getModalParticipationFactors(tmp);
  }

//! @brief Devuelve el factor de distribución correspondiente al modo
//! que se pasa como parámetro.
XC::Vector XC::Node::getDistributionFactor(int mode) const
  { return getModalParticipationFactor(mode)*getEigenvector(mode); }

//! @brief Devuelve el factor de distribución correspondiente al modo
//! que se pasa como parámetro. Si el parámetro gdls
//! no está vacío «proyecta» el modo i sobre
//! los gdls indicados en el conjunto.
XC::Vector XC::Node::getDistributionFactor(int mode,const std::set<int> &gdls) const
  { return getModalParticipationFactor(mode,gdls)*getEigenvector(mode); }

//! @brief Devuelve una matriz con los factores de distribución
//! calculados colocados por columnas.
XC::Matrix XC::Node::getDistributionFactors(void) const
  {
    Matrix retval;
    const int nm= getNumModes();
    if(nm>0)
      {
        Vector distribFactor= getDistributionFactor(1);
        const int nFilas= distribFactor.Size();
        retval= Matrix(nFilas,nm);
        for(int i= 0;i<nFilas;i++)
          retval(i,0)= distribFactor(i);
        for(int j= 2;j<=nm;j++)
          {
            distribFactor= getDistributionFactor(j);
            for(int i= 0;i<nFilas;i++)
              retval(i,j-1)= distribFactor(i);
          }
      }
    return retval;
  }

//! @brief Devuelve la masa modal efectiva 
//! correspondiente al modo i.
double XC::Node::getEffectiveModalMass(int mode) const
  {
    double retval= 0;
    const Vector ev= getEigenvector(mode);
    const int sz= ev.Size();
    const double tau= getModalParticipationFactor(mode);
    const Vector J(sz,1.0);
    retval= tau*dot(ev,(mass*J));
    return retval;
  }

//! @brief Devuelve las masas modales efectivas.
XC::Vector XC::Node::getEffectiveModalMasses(void) const
  {
    const int nm= getNumModes();
    Vector retval(nm);
    for(int i= 1;i<=nm;i++)
      retval[i-1]= getEffectiveModalMass(i);
    return retval;
  }

//! @brief Devuelve la fuerza estática equivalente para el modo
//! que se pasa como parámetro y la aceleración correspondiente a dicho modo.
XC::Vector XC::Node::getEquivalentStaticLoad(int mode,const double &accel_mode) const
  {
    Vector retval= mass*getDistributionFactor(mode);
    retval*=(accel_mode);
    return retval;
  }

//! @brief Devuelve el desplazamiento modal máximo para el modo
//! que se pasa como parámetro y la aceleración correspondiente a dicho modo.
XC::Vector XC::Node::getMaxModalDisplacement(int mode,const double &accel_mode) const
  {
    return getMaxModalAcceleration(mode,accel_mode)/sqr(getPulsacion(mode));
  }

//! @brief Devuelve la velocidad modal máxima para el modo
//! que se pasa como parámetro y la aceleración correspondiente a dicho modo.
XC::Vector XC::Node::getMaxModalVelocity(int mode,const double &accel_mode) const
  {
    return getMaxModalAcceleration(mode,accel_mode)/getPulsacion(mode);
  }

//! @brief Devuelve la aceleración modal máxima para el modo
//! que se pasa como parámetro y la aceleración correspondiente a dicho modo.
XC::Vector XC::Node::getMaxModalAcceleration(int mode,const double &accel_mode) const
  {
    return accel_mode*getDistributionFactor(mode);
  }

//! @brief Devuelve el desplazamiento modal máximo en los gdls y modo que se pasan
//! como parámetro y la aceleración correspondiente a dicho modo.
XC::Vector XC::Node::getMaxModalDisplacementForGdls(int mode,const double &accel_mode,const std::set<int> &gdls) const
  {
    return getMaxModalAccelerationForGdls(mode,accel_mode,gdls)/sqr(getPulsacion(mode));
  }

XC::Vector XC::Node::getMaxModalDisplacementForGdls(int mode,const double &accel_mode,const boost::python::list &gdls) const
  {
    std::set<int> tmp= set_int_from_py_list(gdls);
    return getMaxModalDisplacementForGdls(mode,accel_mode,tmp);
  }


//! @brief Devuelve al velocidad modal máxima en los gdls y modo que se pasan
//! como parámetro y la aceleración correspondiente a dicho modo.
XC::Vector XC::Node::getMaxModalVelocityForGdls(int mode,const double &accel_mode,const std::set<int> &gdls) const
  {
    return getMaxModalAccelerationForGdls(mode,accel_mode,gdls)/getPulsacion(mode);
  }

XC::Vector XC::Node::getMaxModalVelocityForGdls(int mode,const double &accel_mode,const boost::python::list &gdls) const
  {
    std::set<int> tmp= set_int_from_py_list(gdls);
    return getMaxModalVelocityForGdls(mode,accel_mode,tmp);
  }

//! @brief Devuelve la aceleración modal máxima en los gdls y modo que se pasan
//! como parámetro y la aceleración correspondiente a dicho modo.
XC::Vector XC::Node::getMaxModalAccelerationForGdls(int mode,const double &accel_mode,const std::set<int> &gdls) const
  {
    return accel_mode*getDistributionFactor(mode,gdls);
  }

XC::Vector XC::Node::getMaxModalAccelerationForGdls(int mode,const double &accel_mode,const boost::python::list &gdls) const
  {
    std::set<int> tmp= set_int_from_py_list(gdls);
    return getMaxModalAccelerationForGdls(mode,accel_mode,tmp);
  }


//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::Node::getDbTagData(void) const
  {
    static DbTagData retval(22);
    return retval;
  }

//! @brief Envia los miembros del objeto a través del canal que se pasa como parámetro.
int XC::Node::sendData(CommParameters &cp)
  {
    int res= MeshComponent::sendData(cp);
    res+= cp.sendInt(numberDOF,getDbTagData(),CommMetaData(4));
    res+= cp.sendMatrix(mass,getDbTagData(),CommMetaData(5));
    res+= cp.sendVector(reaction,getDbTagData(),CommMetaData(6));
    res+= cp.sendVector(unbalLoad,getDbTagData(),CommMetaData(7));
    res+= cp.sendVector(unbalLoadWithInertia,getDbTagData(),CommMetaData(8));
    res+= cp.sendVector(Crd,getDbTagData(),CommMetaData(9));
    res+= cp.sendMatrix(R,getDbTagData(),CommMetaData(10));
    res+= cp.sendDoubles(alphaM,tributaria,getDbTagData(),CommMetaData(11)); //Arreglar.
    res+= cp.sendMatrix(theEigenvectors,getDbTagData(),CommMetaData(12));
    res+=cp.sendMovable(disp,getDbTagData(),CommMetaData(13));
    res+=cp.sendMovable(vel,getDbTagData(),CommMetaData(14));
    res+=cp.sendMovable(accel,getDbTagData(),CommMetaData(15));
    res+= cp.sendID(get_id_coacciones(),getDbTagData(),CommMetaData(16));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::Node::recvData(const CommParameters &cp)
  {
    int res= MeshComponent::recvData(cp);
    res+= cp.receiveInt(numberDOF,getDbTagData(),CommMetaData(4));
    res+= cp.receiveMatrix(mass,getDbTagData(),CommMetaData(5));
    res+= cp.receiveVector(reaction,getDbTagData(),CommMetaData(6));
    res+= cp.receiveVector(unbalLoad,getDbTagData(),CommMetaData(7));
    res+= cp.receiveVector(unbalLoadWithInertia,getDbTagData(),CommMetaData(8));
    res+= cp.receiveVector(Crd,getDbTagData(),CommMetaData(9));
    res+= cp.receiveMatrix(R,getDbTagData(),CommMetaData(10));
    res+= cp.receiveDoubles(alphaM,tributaria,getDbTagData(),CommMetaData(11));
    res+= cp.receiveMatrix(theEigenvectors,getDbTagData(),CommMetaData(12));
    index= -1;
    res+= cp.receiveMovable(disp,getDbTagData(),CommMetaData(13));
    res+= cp.receiveMovable(vel,getDbTagData(),CommMetaData(14));
    res+= cp.receiveMovable(accel,getDbTagData(),CommMetaData(15));
    ID tmp;
    res+= cp.receiveID(tmp,getDbTagData(),CommMetaData(16));
    set_id_coacciones(tmp);
    setup_matrices(theMatrices,numberDOF);
    return res;
  }

//! @brief Envia el objeto a través del canal que se pasa como parámetro.
int XC::Node::sendSelf(CommParameters &cp)
  {
    inicComm(22);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Node::sendSelf() - failed to send ID data\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::Node::recvSelf(const CommParameters &cp)
  {
    const int dataTag= getDbTag();
    inicComm(22);
    int res = cp.receiveIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "Node::recvSelf() - failed to receive ID data\n";
    else
      res+= recvData(cp);
    return res;
  }

//! @brief Devuelve los conjuntos a los que pertenece este nodo.
std::set<XC::SetBase *> XC::Node::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Modelador *mdlr= GetModelador();
    if(mdlr)
      {
        MapSet &sets= const_cast<MapSet &>(mdlr->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << "Node::get_sets; no se ha definido el modelador." << std::endl;
    return retval;
  }

//! @brief Agrega el nodo a los conjuntos que se pasan como parámetro.
void XC::Node::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        SetMeshComp *s= dynamic_cast<SetMeshComp *>(*i);
        if(s) s->agregaNodo(this);
      }
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
//! nnod: Devuelve el número de nodos del dominio.
any_const_ptr XC::Node::GetProp(const std::string &cod) const
  {
    if(verborrea>4)
      std::clog << "Node::GetProp (" << nombre_clase() 
                << "::GetProp) Buscando propiedad: " << cod << std::endl;
    if(cod=="disp")
      {
        return any_const_ptr(&disp);
      }
    else if(cod=="getCommitDisp")
      return get_prop_vector(disp.getCommitData());
    else if(cod=="getTrialDisp")
      return get_prop_vector(disp.getTrialData());
    else if(cod=="vel")
      {
        return any_const_ptr(&vel);
      }
    else if(cod=="getCommitVel")
      return get_prop_vector(vel.getCommitData());
    else if(cod=="getTrialVel")
      return get_prop_vector(vel.getTrialData());
    else if(cod=="accel")
      {
        return any_const_ptr(&accel);
      }
    else if(cod=="getCommitAccel")
      return get_prop_vector(accel.getCommitData());
    else if(cod=="getTrialAccel")
      return get_prop_vector(accel.getTrialData());
    else if(cod == "isAlive")
      {
        tmp_gp_bool= isAlive();
        return any_const_ptr(tmp_gp_bool);
      }
    else if(cod == "isDead")
      {
        tmp_gp_bool= isDead();
        return any_const_ptr(tmp_gp_bool);
      }
    else if(cod == "isFrozen")
      {
        tmp_gp_bool= isFrozen();
        return any_const_ptr(tmp_gp_bool);
      }
    else if(cod == "isFree")
      {
        tmp_gp_bool= isFree();
        return any_const_ptr(tmp_gp_bool);
      }
    else if(cod=="getUnbalLoad")
      return get_prop_vector(unbalLoad);
    else if(cod=="reac")
      return get_prop_vector(reaction);
    else if(cod=="mass")
      { return any_const_ptr(mass); }
    else if(cod=="getEigenvectors")
      {
        tmp_gp_mdbl= matrix_to_m_double(theEigenvectors);
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getEigenvector")
      {
        const int modo= popInt(cod);
        tmp_gp_mdbl= vector_to_m_double(getEigenvector(modo));
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getNormalizedEigenvector")
      {
        const int modo= popInt(cod);
        tmp_gp_mdbl= vector_to_m_double(getNormalizedEigenvector(modo));
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getNormalizedEigenvectors")
      {
	std::cerr << "DEPRECATED; use Python" << std::endl;
        //tmp_gp_mdbl= getNormalizedEigenvectors();
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getModalParticipationFactor")
      {
        const int modo= popInt(cod);
        tmp_gp_dbl= getModalParticipationFactor(modo);
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getModalParticipationFactors")
      {
        tmp_gp_mdbl= vector_to_m_double(getModalParticipationFactors());
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getModalParticipationFactorInGdls")
      {
	std::set<int> gdls;
        const std::vector<int> iGdls= popVectorInt(cod);
        for(std::vector<int>::const_iterator i= iGdls.begin();i!=iGdls.end();i++)
          gdls.insert(*i);
        const int modo= popInt(cod);
        tmp_gp_dbl= getModalParticipationFactor(modo,gdls);
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getModalParticipationFactorsInGdls")
      {
	std::set<int> gdls;
        const std::vector<int> iGdls= popVectorInt(cod);
        for(std::vector<int>::const_iterator i= iGdls.begin();i!=iGdls.end();i++)
          gdls.insert(*i);
        tmp_gp_mdbl= vector_to_m_double(getModalParticipationFactors(gdls));
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getEffectiveModalMass")
      {
        const int modo= popInt(cod);
        tmp_gp_dbl= getEffectiveModalMass(modo);
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getEffectiveModalMasses")
      {
        tmp_gp_mdbl= vector_to_m_double(getEffectiveModalMasses());
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getDistributionFactor")
      {
        const int modo= popInt(cod);
        tmp_gp_mdbl= vector_to_m_double(getDistributionFactor(modo));
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getDistributionFactors")
      {
	std::cerr << "DEPRECATED; use Python" << std::endl;
        //tmp_gp_mdbl= getDistributionFactors();
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getEquivalentStaticLoad")
      {
        int modo= 0;
        double accel= 0.0;
        if(InterpreteRPN::Pila().size()>1)
          {
            accel= convert_to_double(InterpreteRPN::Pila().Pop());
            modo= convert_to_int(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,2,"GetProp",cod);
        tmp_gp_mdbl= vector_to_m_double(getEquivalentStaticLoad(modo,accel));
        return any_const_ptr(tmp_gp_mdbl);
      }
    // else if(cod=="getEquivalentStaticLoadInGdls")
    //   {
    //     int modo= 0;
    //     double accel= 0.0;
    // 	std::set<int> gdls;
    //     if(InterpreteRPN::Pila().size()>1)
    //       {
    //         const std::vector<int> iGdls= convert_to_vector_int(InterpreteRPN::Pila().Pop());
    //         for(std::vector<int>::const_iterator i= iGdls.begin();i!=iGdls.end();i++)
    //           gdls.insert(*i);
    //         accel= convert_to_double(InterpreteRPN::Pila().Pop());
    //         modo= convert_to_int(InterpreteRPN::Pila().Pop());
    //       }
    //     else
    //       err_num_argumentos(std::cerr,2,"GetProp",cod);
    //     tmp_gp_mdbl= vector_to_m_double(getEquivalentStaticLoad(modo,accel,gdls));
    //     return any_const_ptr(tmp_gp_mdbl);
    //   }
    else if(cod=="getMaxModalDisplacement")
      {
        int modo= 0;
        double accel= 0.0;
        if(InterpreteRPN::Pila().size()>1)
          {
            accel= convert_to_double(InterpreteRPN::Pila().Pop());
            modo= convert_to_int(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,2,"GetProp",cod);
        tmp_gp_mdbl= vector_to_m_double(getMaxModalDisplacement(modo,accel));
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getMaxModalVelocity")
      {
        int modo= 0;
        double accel= 0.0;
        if(InterpreteRPN::Pila().size()>1)
          {
            accel= convert_to_double(InterpreteRPN::Pila().Pop());
            modo= convert_to_int(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,2,"GetProp",cod);
        tmp_gp_mdbl= vector_to_m_double(getMaxModalVelocity(modo,accel));
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getMaxModalAcceleration")
      {
        int modo= 0;
        double accel= 0.0;
        if(InterpreteRPN::Pila().size()>1)
          {
            accel= convert_to_double(InterpreteRPN::Pila().Pop());
            modo= convert_to_int(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,2,"GetProp",cod);
        tmp_gp_mdbl= vector_to_m_double(getMaxModalAcceleration(modo,accel));
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getMaxModalDisplacementInGdls")
      {
        int modo= 0;
        double accel= 0.0;
	std::set<int> gdls;
        if(InterpreteRPN::Pila().size()>2)
          {
            const std::vector<int> iGdls= convert_to_vector_int(InterpreteRPN::Pila().Pop());
            for(std::vector<int>::const_iterator i= iGdls.begin();i!=iGdls.end();i++)
              gdls.insert(*i);
            accel= convert_to_double(InterpreteRPN::Pila().Pop());
            modo= convert_to_int(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,2,"GetProp",cod);
        tmp_gp_mdbl= vector_to_m_double(getMaxModalDisplacementForGdls(modo,accel,gdls));
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getMaxModalVelocityInGdls")
      {
        int modo= 0;
        double accel= 0.0;
	std::set<int> gdls;
        if(InterpreteRPN::Pila().size()>1)
          {
            const std::vector<int> iGdls= convert_to_vector_int(InterpreteRPN::Pila().Pop());
            for(std::vector<int>::const_iterator i= iGdls.begin();i!=iGdls.end();i++)
              gdls.insert(*i);
            accel= convert_to_double(InterpreteRPN::Pila().Pop());
            modo= convert_to_int(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,2,"GetProp",cod);
        tmp_gp_mdbl= vector_to_m_double(getMaxModalVelocityForGdls(modo,accel,gdls));
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="getMaxModalAccelerationInGdls")
      {
	std::set<int> gdls;
        int modo= 0;
        double accel= 0.0;
        if(InterpreteRPN::Pila().size()>1)
          {
            const std::vector<int> iGdls= convert_to_vector_int(InterpreteRPN::Pila().Pop());
            for(std::vector<int>::const_iterator i= iGdls.begin();i!=iGdls.end();i++)
              gdls.insert(*i);
            accel= convert_to_double(InterpreteRPN::Pila().Pop());
            modo= convert_to_int(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,2,"GetProp",cod);
        tmp_gp_mdbl= vector_to_m_double(getMaxModalAccelerationForGdls(modo,accel,gdls));
        return any_const_ptr(tmp_gp_mdbl);
      }
    else if(cod=="ID")
      {
        if(theDOF_GroupPtr)
          return any_const_ptr(theDOF_GroupPtr->getID());
        else
          {
            std::cerr << "Error en Node::GetProp(ID), el nodo no tiene DOF_GroupPtr." << std::endl;
            return any_const_ptr();
          }
      }
    else if(cod=="getNumGdl")
      {
        tmp_gp_int= getNumberDOF();
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="coord")
      return get_prop_vector(Crd);
    else if(cod=="coordXYZ")
      return get_prop_vector(getCrds3d());
    else if(cod=="getDispXYZ")
      return get_prop_vector(getDispXYZ());
    else if(cod=="getRotXYZ")
      return get_prop_vector(getRotXYZ());
    else if(cod=="getPosInicial3d")
      {
        tmp_gp_pos3d= getPosInicial3d();
        return any_const_ptr(tmp_gp_pos3d);
      }
    else if(cod=="getPosFinal3d")
      {
        tmp_gp_pos3d= getPosFinal3d();
        return any_const_ptr(tmp_gp_pos3d);
      }
    else if(cod == "getDist2")
      {
        const Pos3d p= popPos3d(cod);
        tmp_gp_dbl= getDist2(p);
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod == "getDist")
      {
        const Pos3d p= popPos3d(cod);
        tmp_gp_dbl= getDist(p);
        return any_const_ptr(tmp_gp_dbl);
      }
    else if((cod=="getLongTributaria") || (cod=="getAreaTributaria") || (cod=="getVolTributario"))
      { return any_const_ptr(tributaria); }
    else
      return MeshComponent::GetProp(cod);
  }

void XC::Node::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      { // print out everything
        s << "\n XC::Node: " << getTag() << std::endl;
        s << "\tCoordinates  : " << Crd;
        disp.Print(s,flag);
        vel.Print(s,flag);
        accel.Print(s,flag);
        s << "\t unbalanced load: " << unbalLoad;
        s << "\t reaction: " << reaction;
        s << "\tMass : " << mass;
        s << "\t Eigenvectors: " << theEigenvectors;
        if(theDOF_GroupPtr != 0)
          s << "\tID : " << theDOF_GroupPtr->getID();
        s << "\n";
      }
  }


// AddingSensitivity:BEGIN ///////////////////////////////////////

XC::Matrix XC::Node::getMassSensitivity(void)
  {
    Matrix massSens(mass.noRows(),mass.noCols());
    if( (parameterID == 1) || (parameterID == 2) || (parameterID == 3) )
     { massSens(parameterID-1,parameterID-1) = 1.0; }
    return massSens;
  }


int XC::Node::getCrdsSensitivity(void)
  {
    if( (parameterID == 4) || (parameterID == 5) || (parameterID == 6) )
      { return (parameterID-3);        }
    else
      { return 0; }
  }


int XC::Node::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // The following parameterID map is being used:
    // 1: nodal mass in direction 1
    // 2: nodal mass in direction 2
    // 3: nodal mass in direction 3
    // 4: coordinate in direction 1
    // 5: coordinate in direction 2
    // 6: coordinate in direction 3


    //const int argc= argv.size();
    if(argv.size() < 2)
      return -1;

    if((strstr(argv[0].c_str(),"mass") != 0) || (strstr(argv[0].c_str(),"-mass") != 0))
      { 
        int direction = 0; // atoi(argv[1]);
        if((argv[1] == "x")||(argv[1] == "X")||(argv[1] == "1"))
          direction = 1;
        else if((argv[1] == "y")||(argv[1] == "Y")||(argv[1] == "2"))
          direction = 2;
        else if((argv[1] == "z")||(argv[1] == "Z")||(argv[1] == "3"))                                        
          direction = 3;
        else if((argv[1] == "xy")||(argv[1] == "XY"))
          direction = 7;
        else if((argv[1] == "xyz")||(argv[1] == "XYZ"))
          direction = 8;
        if((direction >= 1 && direction <= 3) || direction == 7 || direction == 8)
          return param.addObject(direction, this);
      }
    else if(strstr(argv[0].c_str(),"coord") != 0)
      {
        int direction = atoi(argv[1]);
        if(direction >= 1 && direction <= 3)
          return param.addObject(direction+3, this);
      }
    else
      std::cerr << "WARNING: Could not set parameter in Node. " << std::endl;
    return -1;
  }


int XC::Node::updateParameter(int pparameterID, Information &info)
  {
    if( (pparameterID == 1) || (pparameterID == 2) || (pparameterID == 3) )
      { mass(pparameterID-1,pparameterID-1) = info.theDouble; }
    else
      if( (pparameterID == 4) || (pparameterID == 5) || (pparameterID == 6) )
        {
          if(Crd(pparameterID-4) != info.theDouble)
            {
              //Asigna el valor de la coordenada.
              Crd(pparameterID-4) = info.theDouble;

              // Need to "setDomain" to make the change take effect.
              Domain *theDomain = this->getDomain();
              ElementIter &theElements = theDomain->getElements();
              Element *theElement;
              while((theElement = theElements()) != 0)
                { theElement->setDomain(theDomain); }
            }
          else
            {
            // No change in nodal coordinate
            }
        }
    return 0;
  }


int XC::Node::activateParameter(int passedParameterID)
  {
    parameterID = passedParameterID;
    return 0;
  }


int XC::Node::saveSensitivity(Vector *v,Vector *vdot,Vector *vdotdot, int gradNum, int numGrads)
  {
    // Initial declarations
    int i;

    // If the sensitivity matrices are not already created:
    if(dispSensitivity.Nula())
      { dispSensitivity= Matrix(numberDOF,numGrads ); }
    if((vdot!=0) && (vdotdot!=0) )
      {
        if(velSensitivity.Nula())
          { velSensitivity= Matrix(numberDOF,numGrads ); }
        if(accSensitivity.Nula())
          { accSensitivity= Matrix(numberDOF,numGrads ); }
      }


    // Put GRADIENT VECTORS into COLUMNS of matrices
    for(i=0; i<numberDOF; i++ )
      { dispSensitivity(i,gradNum-1) = (*v)(i); }
    if( (vdot!=0) && (vdotdot!=0) )
      {
        for(i=0; i<numberDOF; i++ )
          { velSensitivity(i,gradNum-1) = (*vdot)(i); }
        for(i=0; i<numberDOF; i++ )
          { accSensitivity(i,gradNum-1) = (*vdotdot)(i); }
      }
    return 0;
  }

double XC::Node::getDispSensitivity(int dof, int gradNum)
  {
    double result= 0.0;
    if(!dispSensitivity.Nula())
      result= dispSensitivity(dof-1,gradNum-1);
    return result;
  }

double XC::Node::getVelSensitivity(int dof, int gradNum)
  {
    double result= 0.0;
    if(!velSensitivity.Nula())
      result= velSensitivity(dof-1,gradNum-1);
    return result;
  }

double XC::Node::getAccSensitivity(int dof, int gradNum)
  {
    double result= 0.0;
    if(!accSensitivity.Nula())
      return accSensitivity(dof-1,gradNum-1);
    return result;
  }
// AddingSensitivity:END /////////////////////////////////////////

//! @brief Devuelve un conjunto de apuntadores a elementos conectados.
std::set<const XC::Element *> XC::Node::getElementosConectados(void) const
  {
    std::set<const Element *> retval;
    for(std::set<ContinuaReprComponent *>::const_iterator i= conectados.begin();i!=conectados.end();i++)
      {
        const Element *ptrElem= dynamic_cast<const Element *>(*i);
        if(ptrElem)
          retval.insert(ptrElem);
      }
    return retval;
  }

//! @brief Devuelve un conjunto de apuntadores a elementos conectados.
std::set<XC::Element *> XC::Node::getElementosConectados(void)
  {
    std::set<Element *> retval;
    for(std::set<ContinuaReprComponent *>::const_iterator i= conectados.begin();i!=conectados.end();i++)
      {
        Element *ptrElem= dynamic_cast<Element *>(*i);
        if(ptrElem)
          retval.insert(ptrElem);
      }
    return retval;
  }

//! @brief Devuelve la acción de los elementos del conjunto sobre el nodo.
const XC::Vector &XC::Node::getResistingForce(const std::set<const Element *> &elementos,const bool &inc_inertia) const
  {
    static Vector retval;
    retval.resize(numberDOF);
    retval.Zero();
    if(isAlive())
      {
        for(std::set<ContinuaReprComponent *>::const_iterator i= conectados.begin();i!=conectados.end();i++)
          if((*i)->isAlive())
            {
              const Element *ptrElem= dynamic_cast<const Element *>(*i);
              if(ptrElem)
                {
                  if(elementos.count(ptrElem)>0)
                    {
                      if(!inc_inertia)
                        retval+= ptrElem->getNodeResistingForce(this);
                      else
                        retval+= ptrElem->getNodeResistingForceIncInertia(this);
                    }
                }
              else
		std::cerr << "Node::getResistingForce; el nodo: " << getTag()
                          << " tiene una condición de contorno." << std::endl;
            }
      }
    return retval; 
  }

//! @brief Devuelve el sistema de vectores deslizantes que representa
//! la acción de los elementos del conjunto sobre el nodo.
SVD3d XC::Node::getResistingSVD3d(const std::set<const Element *> &elementos,const bool &inc_inertia) const
  {
    SVD3d retval;
    const Pos3d o= getPosInicial3d();
    const Vector &v= getResistingForce(elementos,inc_inertia);
    if(numberDOF==2)
      retval= SVD3d(o,Vector3d(v[0],v[1],0));
    else if(numberDOF==3)
      retval= SVD3d(o,Vector3d(v[0],v[1],0),Vector3d(0,0,v[2]));
    else if(numberDOF==6)
      retval= SVD3d(o,Vector3d(v[0],v[1],v[2]),Vector3d(v[3],v[4],v[5]));
    else
      {
        retval= SVD3d(o);
        std::cerr << "Node::getResistingSVD3d; numberDOF= "
                  << numberDOF << " no contemplado."
                  << std::endl;
      }
    return retval; 
  }

//! @brief Devuelve la reacción en el nodo.
const XC::Vector &XC::Node::getReaction(void) const
  { return reaction; }

//! @brief Incrementa la reacción en el nodo.
int XC::Node::addReactionForce(const Vector &add, double factor)
  {
    // check vector of appropraie size
    if(add.Size() != numberDOF)
      {
        std::cerr << "WARNING Node::addReactionForce() - vector not of correct size\n";
        return -1;
      }

    if(factor == 1.0)
      reaction+= add;
    else
     if(factor == -1.0)
       reaction-= add;
     else
       reaction+= add*factor;
    return 0;
  }

//! @brief Chequea la reacción en el nodo.
void XC::Node::checkReactionForce(const double &tol)
  {
    const CondContorno &cc= getDomain()->getCondsContorno();
    const double norm2= reaction.Norm2();
    if(norm2>tol)
      {
        if(!cc.nodoAfectadoSPsOMPs(getTag()) && !isFrozen())
          std::cerr << "Node::addReactionForce el nodo: " << getTag()
                    << " no esta sujeto a condiciones de contorno y sin embargo"
                    << " tiene una reacción de valor: " << reaction 
                    << " y norma: " << sqrt(norm2)
                    << " el método de solución empleado no es adecuado al problema. "
                    << std::endl;
      }
  }

//! @brief Calcula las reacciones en este nodo (se emplea en Domain::calculateNodalReactions).
int XC::Node::resetReactionForce(bool inclInertia)
  {
    reaction.Zero();

    // add unbalance, the negative of applied forces hence the -=
    if(!inclInertia)
      { reaction-= getUnbalancedLoad(); }
    else
      { reaction-= getUnbalancedLoadIncInertia(); }
    return 0;
  }

//! @brief Desplaza la posición del nodo (sólo esta previsto que se use desde XC::Set).
void XC::Node::Mueve(const std::vector<ExprAlgebra> &desplaz)
  {
    const size_t sz= std::min(desplaz.size(),getDim());
    for(size_t i= 0;i<sz;i++)
      Crd(i)+= desplaz[i].ToNum();
    return;
  }
