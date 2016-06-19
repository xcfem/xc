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


#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/pos_vec/SVD3d.h"
#include "preprocessor/cad/trf/TrfGeom.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/loaders/LoadLoader.h"
#include "preprocessor/set_mgmt/SetMeshComp.h"
#include "domain/load/NodalLoad.h"
#include "domain/load/pattern/NodeLocker.h"

#include <domain/domain/Domain.h>
#include "domain/mesh/MeshEdge.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/ElementIter.h"

//
#include "xc_utils/src/nucleo/CmdParser.h"
#include "xc_utils/src/nucleo/python_utils.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/actor/actor/MatrixCommMetaData.h"

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
   connected(otherNode.connected), coacciones_freeze(otherNode.coacciones_freeze)
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

//! @brief Inserts a component (element, constraint,...) to the connected component list.
void XC::Node::connect(ContinuaReprComponent *el) const
  { 
    if(el)
      connected.insert(el);
    else
      std::cerr << "Node::connect; null argument." << std::endl;
  }

//! @brief Removes a component (element, constraint,...) from the connected component list.
void XC::Node::disconnect(ContinuaReprComponent *el) const
  {
    std::set<ContinuaReprComponent *>::const_iterator i= connected.find(el);
    if(i!=connected.end())
      connected.erase(i);
  }

XC::Node *XC::Node::getCopy(void) const
  { return new Node(*this,true); }

const bool XC::Node::isDead(void) const
  { return !isAlive(); }

const bool XC::Node::isAlive(void) const
  {
    bool retval= false;
    if(connected.empty())
      {
        std::cerr << "Node: " << getTag() << " is free." << std::endl;
        retval= true;
      }
    else
      {
        for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
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
	      std::cerr << "Node::isAlive; null pointer in the connected list." << std::endl;
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
  { return connected.empty(); }

void XC::Node::kill(void)
  { std::cerr << "No se debe llamar a Node::kill" << std::endl; }

void XC::Node::alive(void)
  { std::cerr << "No se debe llamar a Node::alive" << std::endl; }

//! @brief destructor
XC::Node::~Node(void)
  {
    if(theDOF_GroupPtr) theDOF_GroupPtr->resetNodePtr();
  }

XC::DefaultTag &XC::Node::getDefaultTag(void)
  { return defaultTag; }

//! @brief Introduce en el nodo una constraint
//! como la being passed as parameter.
XC::SP_Constraint *XC::Node::fix(const SP_Constraint &semilla)
  { return GetPreprocessor()->getConstraintLoader().addSP_Constraint(getTag(),semilla); }

//! @brief Impone desplazamientos prescritos en los grados de libertad
//! que se pasan como parámetro.
void XC::Node::fix(const std::vector<int> &idGdls,const std::vector<double> &valores)
  {
    if(getDomain())
      {
        ConstraintLoader &cl= GetPreprocessor()->getConstraintLoader();
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
double XC::Node::getDist2(const Pos2d &p,bool initialGeometry) const
  { return getDist2(To3dXY2d(p),initialGeometry); }

//! @brief Devuelve la distancia desde el nodo al punto que
//! se pasa como parámetro.
double XC::Node::getDist(const Pos2d &p,bool initialGeometry) const
  { return sqrt(getDist2(p,initialGeometry)); }

//! @brief Devuelve el cuadrado de la distancia desde el nodo al punto que
//! se pasa como parámetro.
double XC::Node::getDist2(const Pos3d &p,bool initialGeometry) const
  {
    if(initialGeometry)
      return ::dist2(getPosInicial3d(),p);
    else
      return ::dist2(getPosFinal3d(),p);
  }

//! @brief Devuelve la distancia desde el nodo al punto que
//! se pasa como parámetro.
double XC::Node::getDist(const Pos3d &p,bool initialGeometry) const
  { return sqrt(getDist2(p,initialGeometry)); }

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
Pos3d XC::pos_nodo(const Node &nod,bool initialGeometry)
  {
    if(initialGeometry)
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
//! área o volumen being passed as parameter.
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
//! being passed as parameter.
XC::Vector XC::Node::getDistributionFactor(int mode) const
  { return getModalParticipationFactor(mode)*getEigenvector(mode); }

//! @brief Devuelve el factor de distribución correspondiente al modo
//! being passed as parameter. Si el parámetro gdls
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
//! being passed as parameter y la aceleración correspondiente a dicho modo.
XC::Vector XC::Node::getEquivalentStaticLoad(int mode,const double &accel_mode) const
  {
    Vector retval= mass*getDistributionFactor(mode);
    retval*=(accel_mode);
    return retval;
  }

//! @brief Devuelve el desplazamiento modal máximo para el modo
//! being passed as parameter y la aceleración correspondiente a dicho modo.
XC::Vector XC::Node::getMaxModalDisplacement(int mode,const double &accel_mode) const
  {
    return getMaxModalAcceleration(mode,accel_mode)/sqr(getPulsacion(mode));
  }

//! @brief Devuelve la velocidad modal máxima para el modo
//! being passed as parameter y la aceleración correspondiente a dicho modo.
XC::Vector XC::Node::getMaxModalVelocity(int mode,const double &accel_mode) const
  {
    return getMaxModalAcceleration(mode,accel_mode)/getPulsacion(mode);
  }

//! @brief Devuelve la aceleración modal máxima para el modo
//! being passed as parameter y la aceleración correspondiente a dicho modo.
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

//! @brief Envia los miembros del objeto through the channel being passed as parameter.
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

//! @brief Receives members del objeto through the channel being passed as parameter.
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

//! @brief Envia el objeto through the channel being passed as parameter.
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

//! @brief Receives object through the channel being passed as parameter.
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
    const Preprocessor *preprocessor= GetPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << "Node::get_sets; no se ha definido el preprocesador." << std::endl;
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

//! @brief Return a list of pointers to the elements that are connected with this node.
std::set<const XC::Element *> XC::Node::getConnectedElements(void) const
  {
    std::set<const Element *> retval;
    for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
      {
        const Element *ptrElem= dynamic_cast<const Element *>(*i);
        if(ptrElem)
          retval.insert(ptrElem);
      }
    return retval;
  }

//! @brief Return a list of pointers to the elements that are connected with this node.
std::set<XC::Element *> XC::Node::getConnectedElements(void)
  {
    std::set<Element *> retval;
    for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
      {
        Element *ptrElem= dynamic_cast<Element *>(*i);
        if(ptrElem)
          retval.insert(ptrElem);
      }
    return retval;
  }

//! @brief Returns an edge that has its origin in this node (and is not in visited).
const XC::MeshEdge *XC::Node::next(const std::deque<MeshEdge> &edges, const std::set<const MeshEdge *> &visited) const
  {
    const MeshEdge *retval= nullptr;
    for(std::deque<MeshEdge>::const_iterator i= edges.begin();i!=edges.end();i++)
      {
        const MeshEdge &edge= *i;
        if(visited.find(&edge)==visited.end()) //Not already visited.
          if(edge.getFirstNode()==this) //Edge starts in this node.
            {
              retval= &edge;
              break;
            }
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
        for(std::set<ContinuaReprComponent *>::const_iterator i= connected.begin();i!=connected.end();i++)
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
		std::cerr << "Node::getResistingForce; node: " << getTag()
                          << " has a constraint." << std::endl;
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
    const ConstrContainer &cc= getDomain()->getConstraints();
    const double norm2= reaction.Norm2();
    if(norm2>tol)
      {
        if(!cc.nodoAfectadoSPsOMPs(getTag()) && !isFrozen())
          std::cerr << "Node::checkReactionForce el nodo: " << getTag()
                    << " no esta sujeto a constraints y sin embargo"
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
void XC::Node::Mueve(const Vector3d &desplaz)
  {
    Crd(0)+= desplaz.x();
    Crd(1)+= desplaz.y();
    Crd(2)+= desplaz.z();
  }
