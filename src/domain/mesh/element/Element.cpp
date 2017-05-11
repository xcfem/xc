//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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

// $Revision: 1.17 $
// $Date: 2005/02/17 22:29:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/Element.cpp,v $


// Written: fmk 11/95
//
// Purpose: This file contains the class definition for Element.
// Element is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.
//
// The interface:
//

#include <cstdlib>

#include "Element.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include "utility/recorder/response/ElementResponse.h"
#include <utility/matrix/Vector.h>
#include "domain/mesh/node/Node.h"
#include "domain/mesh/node/NodeTopology.h"
#include <domain/domain/Domain.h>
#include "utility/matrix/nDarray/basics.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/SetMeshComp.h"
#include "boost/any.hpp"


#include "xc_utils/src/geom/pos_vec/TritrizPos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/sis_coo/SisCooRect3d3d.h"
#include "material/Material.h"
#include "utility/matrix/DqMatrices.h"
#include "utility/matrix/DqVectors.h"
#include "utility/tagged/DefaultTag.h"
#include "med.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "utility/med_xc/MEDGaussModel.h"
#include "utility/actor/actor/CommMetaData.h"
#include "vtkCellType.h"

std::deque<XC::Matrix> XC::Element::theMatrices;
std::deque<XC::Vector> XC::Element::theVectors1;
std::deque<XC::Vector> XC::Element::theVectors2;
double XC::Element::dead_srf= 1e-6;//Stiffness reduction factor for dead (non active) elements.
XC::DefaultTag XC::Element::defaultTag;

//! @brief Constructor that takes the element's unique tag and the number
//! of external nodes for the element.
XC::Element::Element(int tag, int cTag)
  :MeshComponent(tag, cTag), nodeIndex(-1), rayFactors() 
  { defaultTag= tag+1; }

//! @brief Returns next element's tag value by default.
XC::DefaultTag &XC::Element::getDefaultTag(void)
  { return defaultTag; }

//! @brief Returns number of edges (it must be overloaded for elements that
//! have nodes inside edges.
int XC::Element::getNumEdges(void) const
  { return getNumExternalNodes(); }

//! @brief Consuma el estado of the element.
int XC::Element::commitState(void)
  {
    if(!Kc.Nula())
      Kc= getTangentStiff();
    return 0;
  }

//! @brief Actualiza el estado of the element.
int XC::Element::update(void)
  { return 0; }

int XC::Element::revertToStart(void)
  { return 0; }

//! @brief Asigna valores a los coeficientes de amortiguamiento de Rayleigh.
int XC::Element::setRayleighDampingFactors(const RayleighDampingFactors &rF) const
  {
    rayFactors= rF;

    // check that memory has been allocated to store compute/return
    // damping matrix & residual force calculations
    if(index == -1)
      {
        int numDOF = this->getNumDOF();

        setup_matrices(theMatrices,numDOF);
        if(theVectors1.size()<theMatrices.size())
          {
            Vector theVector1(numDOF);
            Vector theVector2(numDOF);
            theVectors1.push_back(theVector1);
            theVectors2.push_back(theVector2);
         }
      }
    // if need storage for Kc go get it
    if(rayFactors.getBetaKc() != 0.0)
      Kc= Matrix(this->getTangentStiff());
    else // if don't need storage for Kc & have allocated some for it, free the memory
      Kc= Matrix();
    return 0;
  }

//! @brief Returns the element dimension (0, 1, 3 or 3).
size_t XC::Element::getDimension(void) const
  {
    std::cerr << "Element::getDimension not implemented para '"
              << nombre_clase() << "'\n";
    return 0;
  }

//! @brief Asigna los nodos.
void XC::Element::setIdNodos(const std::vector<int> &inodos)
  { getNodePtrs().set_id_nodes(inodos); }

//! @brief Asigna los nodos.
void XC::Element::setIdNodos(const ID &inodos)
  { getNodePtrs().set_id_nodes(inodos); }

//! @brief Sets the domain for the element.
void XC::Element::setDomain(Domain *theDomain)
  {
    MeshComponent::setDomain(theDomain);
    if(!theDomain)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
	          << " the domain parameter is null\n";
        getNodePtrs().inic();
      }
    else
      getNodePtrs().set_node_ptrs(theDomain);
  }


//! @brief Anula el load vector aplicadas of the element.
void XC::Element::zeroLoad(void)
  { load.Zero(); }

//! @brief Forma la matriz de amortiguamiento.
void XC::Element::compute_damping_matrix(Matrix &theMatrix) const
  {
    theMatrix.Zero();
    if(rayFactors.getAlphaM() != 0.0)
      theMatrix.addMatrix(0.0, this->getMass(), rayFactors.getAlphaM());
    if(rayFactors.getBetaK() != 0.0)
      theMatrix.addMatrix(1.0, this->getTangentStiff(), rayFactors.getBetaK());
    if(rayFactors.getBetaK0() != 0.0)
      theMatrix.addMatrix(1.0, this->getInitialStiff(), rayFactors.getBetaK0());
    if(rayFactors.getBetaKc() != 0.0)
      theMatrix.addMatrix(1.0, Kc, rayFactors.getBetaKc());
  }

//! @brief Returns the matriz de amortiguamiento.
const XC::Matrix &XC::Element::getDamp(void) const
  {
    if(index == -1)
      setRayleighDampingFactors(RayleighDampingFactors()); //Anula los factores de amortiguamiento.

    // now compute the damping matrix
    Matrix &theMatrix= theMatrices[index];
    compute_damping_matrix(theMatrix);
    // return the computed matrix
    return theMatrix;
  }


//! @brief Returns the mass matrix.
const XC::Matrix &XC::Element::getMass(void) const
  {
    if(index  == -1)
      setRayleighDampingFactors(RayleighDampingFactors()); //Anula los factores de amortiguamiento.

    // zero the matrix & return it
    Matrix &theMatrix= theMatrices[index];
    theMatrix.Zero();
    return theMatrix;
  }

//! @brief Returns the action of the element over its attached nodes.
//! Computes damping matrix.
const XC::Vector &XC::Element::getResistingForceIncInertia(void) const
  {
    if(index == -1)
      setRayleighDampingFactors(RayleighDampingFactors()); //Zeroes dumping factors.

    Matrix &theMatrix= theMatrices[index];
    Vector &theVector= theVectors2[index];
    Vector &theVector2= theVectors1[index];

    //
    // perform: R = P(U) - Pext(t);
    //

    theVector= this->getResistingForce();

    //
    // perform: R = R - M * a
    //

    int loc = 0;
    const NodePtrsWithIDs &theNodes= getNodePtrs();
    const int numNodes = this->getNumExternalNodes();

    int i;
    for(i=0;i<numNodes;i++)
      {
        const Vector &acc = theNodes[i]->getAccel();
        for(int i=0; i<acc.Size(); i++)
          { theVector2(loc++) = acc(i); }
      }
    theVector.addMatrixVector(1.0, this->getMass(), theVector2, +1.0);

    //
    // perform: R = R + (rayFactors.getAlphaM() * M + rayFactors.getBetaK0() * K0 + betaK * K) * v
    //            = R + D * v
    //

    // determine the vel vector from ele nodes
    loc = 0;
    for(i=0; i<numNodes; i++)
      {
        const Vector &vel = theNodes[i]->getTrialVel();
        for(int i=0; i<vel.Size(); i++)
          { theVector2(loc++) = vel[i]; }
      }

    // now compute the damping matrix
    compute_damping_matrix(theMatrix);

    // finally the D * v
    theVector.addMatrixVector(1.0, theMatrix, theVector2, 1.0);
    if(isDead())
      theVector*=dead_srf;

    return theVector;
  }

//! @brief Returns the generalized force of the element over the iNod-th
//! node.
const XC::Vector &XC::Element::getNodeResistingComponents(const size_t &iNod,const Vector &rf) const
  {
    static Vector retval;
    const int ngdl= getNodePtrs()[iNod]->getNumberDOF(); // number of DOFs in the node.
    retval.resize(ngdl);
    for(int i=0;i<ngdl;i++)
      retval[i]= rf(iNod*ngdl+i);
    return retval; 
  }

//! @brief Returns the generalized force of the element over the iNod-th
//! node.
const XC::Vector &XC::Element::getNodeResistingForce(const size_t &iNod) const
  {
    const Vector &rf= getResistingForce();
    return getNodeResistingComponents(iNod,rf);
  }

//! @brief Returns the fuerza generalizada (incluyendo fuerzas de inercia)
//! of the element sobre el nodo which index is being passed as parameter.
const XC::Vector &XC::Element::getNodeResistingForceIncInertia(const size_t &iNod) const
  {
    const Vector &rf= getResistingForceIncInertia();
    return getNodeResistingComponents(iNod,rf);
  }

//! @brief Returns the fuerza generalizada of the element sobre el nodo pointed
//! by the parameter.
const XC::Vector &XC::Element::getNodeResistingForce(const Node *ptrNod) const
  {
    const int iNodo= getNodePtrs().getIndiceNodo(ptrNod);
    assert(iNodo>=0);
    return getNodeResistingForce(iNodo);
  }

//! @brief Returns the fuerza generalizada (incluyendo fuerzas de inercia) of the element 
//! over the node pointed by the parameter.
const XC::Vector &XC::Element::getNodeResistingForceIncInertia(const Node *ptrNod) const
  {
    const int iNodo= getNodePtrs().getIndiceNodo(ptrNod);
    assert(iNodo>=0);
    return getNodeResistingForceIncInertia(iNodo);
  }

//! @brief Returns the equivalent static load for the mode
//! being passed as parameter and the acceleration that corresponding that mode.
XC::Vector XC::Element::getEquivalentStaticLoad(int mode,const double &accel_mode) const
  {
    const Matrix &matriz_masas= getMass();
    const Vector distrib_factors= getNodePtrs().getDistributionFactor(mode);
    Vector retval= matriz_masas*distrib_factors;
    retval*=(accel_mode);
    return retval;
  }

//! @brief Returns the equivalent static load on each node for the mode
//! being passed as parameter and the corresponding acceleration to the mode.
XC::Matrix XC::Element::getEquivalentStaticNodalLoads(int mode,const double &accel_mode) const
  {
    const Vector element_load= getEquivalentStaticLoad(mode,accel_mode);
    return getNodePtrs().getNodeVectors(element_load);
  }

//! @brief Returns element Rayleigh damping forces.
const XC::Vector &XC::Element::getRayleighDampingForces(void) const
  {

    if(index == -1)
      setRayleighDampingFactors(RayleighDampingFactors()); //Anula los factores de amortiguamiento.

    Matrix &theMatrix= theMatrices[index];
    Vector &theVector= theVectors2[index];
    Vector &theVector2= theVectors1[index];

    //
    // perform: R = (rayFactors.getAlphaM() * M + rayFactors.getBetaK0() * K0 + rayFactors.getBetaK() * K) * v
    //            = D * v
    //

    // determine the vel vector from ele nodes
    const NodePtrs &theNodes = getNodePtrs();
    const int numNodes = this->getNumExternalNodes();
    int loc = 0;
    for(int i=0; i<numNodes; i++)
      {
        const Vector &vel = theNodes[i]->getTrialVel();
        for(int j=0; j<vel.Size(); j++)
          { theVector2(loc++) = vel[j]; }
      }

    // now compute the damping matrix
    compute_damping_matrix(theMatrix);

    // finally the D * v
    theVector.addMatrixVector(0.0, theMatrix, theVector2, 1.0);

    return theVector;
  }

bool XC::Element::isSubdomain(void)
  { return false; }

XC::Response *XC::Element::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    if(argv[0] == "force" || argv[0] == "forces" ||
        argv[0] == "globalForce" || argv[0] == "globalForces")
      return new ElementResponse(this, 1, this->getResistingForce());
    return 0;
  }

int XC::Element::getResponse(int responseID, Information &eleInfo)
  {
    switch (responseID)
      {
      case 1: // global forces
        return eleInfo.setVector(this->getResistingForce());
      default:
        return -1;
      }
  }

// AddingSensitivity:BEGIN //////////////////////////////////////////
int XC::Element::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return -1; }

int XC::Element::updateParameter(int parameterID, Information &info)
  { return -1; }

int XC::Element::activateParameter(int parameterID)
  { return -1; }

const XC::Vector &XC::Element::getResistingForceSensitivity(int gradNumber)
  {
    static XC::Vector dummy(1);
    return dummy;
  }

const XC::Matrix &XC::Element::getInitialStiffSensitivity(int gradNumber)
  {
    static XC::Matrix dummy(1,1);
    return dummy;
  }

const XC::Matrix &XC::Element::getMassSensitivity(int gradNumber)
  {
    static XC::Matrix dummy(1,1);
    return dummy;
  }

int XC::Element::commitSensitivity(int gradNumber, int numGrads)
  { return -1; }

int XC::Element::addInertiaLoadSensitivityToUnbalance(const XC::Vector &accel, bool tag)
  {  return -1; }


// AddingSensitivity:END ///////////////////////////////////////////

const XC::Matrix &XC::Element::getDampSensitivity(int gradNumber)
  {
    if(index  == -1)
      setRayleighDampingFactors(RayleighDampingFactors()); //Anula los factores de amortiguamiento.

    // now compute the damping matrix
    Matrix &theMatrix= theMatrices[index];
    theMatrix.Zero();
    if(rayFactors.getAlphaM() != 0.0)
      theMatrix.addMatrix(0.0, this->getMassSensitivity(gradNumber), rayFactors.getAlphaM());
    if(rayFactors.getBetaK() != 0.0)
      {
        theMatrix.addMatrix(1.0, this->getTangentStiff(), 0.0); // Don't use this and DDM
        std::cerr << "Rayleigh damping with non-zero betaCurrentTangent is not compatible with DDM sensitivity analysis" << std::endl;
      }
    if(rayFactors.getBetaK0() != 0.0)
      theMatrix.addMatrix(1.0, this->getInitialStiffSensitivity(gradNumber), rayFactors.getBetaK0());
    if(rayFactors.getBetaKc() != 0.0)
      {
        theMatrix.addMatrix(1.0, Kc, 0.0);      // Don't use this and DDM
          std::cerr << "Rayleigh damping with non-zero betaCommittedTangent is not compatible with DDM sensitivity analysis" << std::endl;
      }

    // return the computed matrix
    return theMatrix;
  }


//! @brief Adds nodal reactions.
int XC::Element::addResistingForceToNodalReaction(bool inclInertia)
  {
    int result = 0;
    int numNodes = this->getNumExternalNodes();
    NodePtrs &theNodes= getNodePtrs();

    //
    // first we create the nodes in static arrays as presume
    // we are going to do this many times & save new/delete
    //
    if(nodeIndex == -1)
      {
        int numLastDOF = -1;
        const size_t numMatrices= theMatrices.size();
        for(int i=0; i<numNodes; i++)
          {
            const int numNodalDOF = theNodes[i]->getNumberDOF();

            if(numNodalDOF != 0 && numNodalDOF != numLastDOF)
              {
                // see if an existing vector will do
                size_t j;
                for(j=0; j<numMatrices; j++)
                  {
                    if(theVectors1[j].Size() == numNodalDOF)
                    nodeIndex = j;
                    j = numMatrices+2;
                  }

                // if not we need to enlarge the bool of temp vectors, matrices
                if(j != (numMatrices+2))
                  {
                    Matrix theMatrix(numNodalDOF, numNodalDOF);
                    theMatrices.push_back(theMatrix);

                    Vector theVector1(numNodalDOF);
                    Vector theVector2(numNodalDOF);
                    theVectors1.push_back(theVector1);
                    theVectors2.push_back(theVector2);

                    nodeIndex= numMatrices;
                  }
              }
            numLastDOF = numNodalDOF;
          }
      }

    //
    // now determine the resisting force
    //

    const Vector *theResistingForce= nullptr;
    if(inclInertia == 0)
      theResistingForce= &(getResistingForce());
    else
      theResistingForce= &(getResistingForceIncInertia());

    if(nodeIndex == -1)
      {
        std::cerr << "LOGIC ERROR Element::addResistingForceToNodalReaction() -HUH!\n";
        return -1;
      }

    //
    // iterate over the elements nodes; determine nodes contribution & add it
    //

    int nodalDOFCount = 0;

    const size_t numMatrices= theMatrices.size();
    for(int i=0; i<numNodes; i++)
      {
        Node *theNode= theNodes[i];

        int numNodalDOF= theNode->getNumberDOF();
        Vector &theVector= theVectors1[nodeIndex];
        if(theVector.Size() != numNodalDOF)
          {
            for(size_t j=0; j<numMatrices; j++)
            if(theVectors1[j].Size() == numNodalDOF)
              {
                j = numMatrices;
                theVector= theVectors1[j];
              }
          }
        for(int j=0; j<numNodalDOF; j++)
          {
            theVector(j) = (*theResistingForce)(nodalDOFCount);
            nodalDOFCount++;
          }
        result+=theNode->addReactionForce(theVector, 1.0);
      }
    return result;
  }

//! @brief Returns interpolattion factors for a material point.
XC::Vector XC::Element::getInterpolationFactors(const ParticlePos3d &) const
  {
    std::cerr << "getInterpolationFactors must be implemented in the subclass."
              << std::endl;
    static const int numberNodes= getNumExternalNodes();
    return Vector(numberNodes);
  }

//! @brief Returns interpolattion factors for a material point.
XC::Vector XC::Element::getInterpolationFactors(const Pos3d &) const
  {
    std::cerr << "getInterpolationFactors must be implemented in the subclass."
              << std::endl;
    static const int numberNodes= getNumExternalNodes();
    return Vector(numberNodes);
  }

//! @brief Interfaz con VTK.
int XC::Element::getVtkCellType(void) const
  {
    std::cerr << "Element::getVtkCellType: function getVtkCellType must be overloaded in derived classes." << std::endl;
    return VTK_EMPTY_CELL;
  }

//! @brief Interfaz con el formato MED de Salome.
int XC::Element::getMEDCellType(void) const
  {
    std::cerr << "Element::getMEDCellType: function getMEDCellType must be overloaded in derived classes." << std::endl;
    return ::MED_NONE;
  }

//! @brief Returns the Gauss integration model of the element.
const XC::GaussModel &XC::Element::getGaussModel(void) const
  {
    std::cerr << "Function Element::getMEDCellType must be overloaded in derived classes." << std::endl;
    return gauss_model_empty;
  }

//! @brief Returns the Gauss integration model of the element for MED library.
XC::MEDGaussModel XC::Element::getMEDGaussModel(void) const
  {
    MEDGaussModel retval(nombre_clase(),getMEDCellType(),getGaussModel());
    return retval;
  }

//! @brief Returns the nodos del borde (o arista) of the element
//!which index is being passed as parameter.
XC::Element::NodesEdge XC::Element::getNodesEdge(const size_t &) const
  {
    NodesEdge retval;
    std::cerr << nombre_clase()
              << "; no se ha definido getNodesEdge()."
              << std::endl;
    return retval;
  }

//! @brief Returns the borde (o arista) of the element
//! que tiene por extremos los nodos being passed as parameters.
int XC::Element::getEdgeNodes(const Node *,const Node *) const
  {
    std::cerr << nombre_clase()
              << "; no se ha definido getEdgeNodes()."
              << std::endl;
    return -1;
  }

//! @brief Returns the borde of the element
//! que tiene por extremos los nodos being passed as parameters.
int XC::Element::getEdgeNodes(const int &iN1,const int &iN2) const
  {
    const Domain *dom= this->getDomain();
    const Node *n1= dom->getNode(iN1);
    const Node *n2= dom->getNode(iN2);
    return getEdgeNodes(n1,n2);
  }
 
//! @brief Returns the bordes of the element
//! que tienen por extremo el nodo being passed as parameter.
XC::ID XC::Element::getEdgesNode(const Node *) const
  {
    ID retval;
    std::cerr << nombre_clase()
              << "; no se ha definido getEdgesNode()."
              << std::endl;
    return retval;
  }

//! @brief Returns the bordes of the element que tienen ambos extremos
//! en el node set being passed as parameter.
std::set<int> XC::Element::getEdgesNodes(const NodePtrSet &nodos) const
  {
    std::set<int> retval;
    for(NodePtrSet::const_iterator i= nodos.begin();i!=nodos.end();i++)
      {
        const Node *nodo= *i;
        const ID edgeIds= getEdgesNode(nodo);
        const int sz= edgeIds.Size();
        for(int i=0;i<sz;i++)
          {
            NodesEdge nodosEdge= getNodesEdge(edgeIds(i));
            if(in(nodos,nodosEdge))
              retval.insert(edgeIds(i));
          }
      }
    return retval;
  }

//! @brief Returns the bordes of the element
//! que tienen por extremo el nodo cuyo tag is being passed as parameter.
XC::ID XC::Element::getEdgesNodeByTag(const int &iN) const
  {
    const Domain *dom= this->getDomain();
    const Node *n= dom->getNode(iN);
    return getEdgesNode(n);
  }
 
//! @brief Returns the local indexes of the element nodes
//! that lie over the i-th edge.
XC::ID XC::Element::getLocalIndexNodesEdge(const size_t &i) const
  {
    ID retval;
    std::cerr << nombre_clase() << "::" << __FUNCTION__
              << "; not defined."
              << std::endl;
    return retval;
  }

//! @brief Returns the sets to which the element belongs.
std::set<XC::SetBase *> XC::Element::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << nombre_clase() << __FUNCTION__
	        << "; preprocessor needed." << std::endl;
    return retval;
  }

//! @brief Adds the element to the sets being passed as parameters.
void XC::Element::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        SetMeshComp *s= dynamic_cast<SetMeshComp *>(*i);
        if(s) s->addElement(this);
      }
  }

XC::Response* XC::Element::setMaterialResponse(Material *theMaterial,const std::vector<std::string> &argv,const size_t &offset, Information &info)
  {
    Response *retval= nullptr;
    if(theMaterial)
      {
        std::vector<std::string> argvOffset(argv);
        argvOffset.erase(argvOffset.begin(),argvOffset.begin()+offset);
        retval= theMaterial->setResponse(argvOffset,info);
      }
    return retval;
  }

int XC::Element::setMaterialParameter(Material *theMaterial,const std::vector<std::string> &argv,const size_t &offset, Parameter &param)
  {
    int retval= -1;
    if(theMaterial)
      {
        std::vector<std::string> argvOffset(argv);
        argvOffset.erase(argvOffset.begin(),argvOffset.begin()+offset);
        retval= theMaterial->setParameter(argvOffset,param);
      }
    return retval;
  }

std::vector<int> XC::Element::getIdxNodes(void) const
  { return getNodePtrs().getIdx(); }

//! @brief Returns the valor máximo de la coordenada i of the nodes of the element.
double XC::Element::MaxCooNod(int icoo) const
  { return getNodePtrs().MaxCooNod(icoo); }

//! @brief Returns the minimum value de la coordenada i of the nodes of the element.
double XC::Element::MinCooNod(int icoo) const
  { return getNodePtrs().MinCooNod(icoo); }

//! @brief Returns the coordinates of the nodes.
const XC::Matrix &XC::Element::getCooNodos(void) const
  { return getNodePtrs().getCoordinates(); }

//! @brief Returns the coordinates of the nodes.
std::list<Pos3d> XC::Element::getPosNodos(bool initialGeometry) const
  { return getNodePtrs().getPosiciones(initialGeometry); }

//! @brief Returs a matrix with the axes of the element as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
XC::Matrix XC::Element::getLocalAxes(bool initialGeometry) const
  {
    Matrix retval;
    std::cerr << nombre_clase() << "::" << __FUNCTION__
              << "must be implemented in derived classes." << std::endl;
    return retval;
  }

//! @brief Returns a base vector in the direction of the local i-th axis
//! from the i-th row of the matrix returned by getLocalAxes.
XC::Vector XC::Element::getBaseVector(size_t i,bool initialGeometry) const
  {
    const Matrix localAxes= getLocalAxes(initialGeometry);
    Vector retval(3,0.0);
    const size_t nCols= localAxes.noCols();
    for(size_t j= 0;j<nCols;i++)
      retval(j)= localAxes(i,j);
    return retval;
  }    

//! @brief Returns a base vector in the direction of the local i-th axis
//! from the i-th row of the matrix returned by getLocalAxes.
Vector3d XC::Element::getBaseVector3d(size_t i,bool initialGeometry) const
  {
    const Vector base= getBaseVector(i);
    return Vector3d(base(0),base(1),base(2));
  }    

//! @brief Returns a vector in the direction of the local x axis
//! from the first row of the matrix returned by getLocalAxes.
Vector3d XC::Element::getIVector3d(bool initialGeometry) const
  { return getBaseVector3d(0); }

//! @brief Returns a vector in the direction of the local y axis
//! from the second row of the matrix returned by getLocalAxes.
Vector3d XC::Element::getJVector3d(bool initialGeometry) const
  { return getBaseVector3d(1); }

//! @brief Returns a vector in the direction of the local z axis
//! from the third row of the matrix returned by getLocalAxes.
Vector3d XC::Element::getKVector3d(bool initialGeometry) const
  { return getBaseVector3d(2); }

//! @brief Returns the element coordinate system from the
//! matrix returned by getLocalAxes.
SisCooRect3d3d XC::Element::getSisCoo(bool initialGeometry) const
  {
    const Matrix localAxes= getLocalAxes(initialGeometry);
    Matrix trfMatrix(3,3);
    trfMatrix.Zero();
    const size_t nRows= localAxes.noRows();
    const size_t nCols= localAxes.noCols();
    for(size_t i= 0;i<nRows;i++)
      for(size_t j= 0;j<nCols;i++)
	trfMatrix(i,j)= localAxes(i,j);
    const Vector3d x(trfMatrix(0,0), trfMatrix(0,1), trfMatrix(0,2));
    const Vector3d y(trfMatrix(1,0), trfMatrix(1,1), trfMatrix(1,2));
    const Vector3d z(trfMatrix(2,0), trfMatrix(2,1), trfMatrix(2,2));
    return SisCooRect3d3d(x,y,z);
  }    


//! @brief Returns the position of the i-th node.
//! @param i: index of the node in the element.
//! @param initialGeometry: if true initial position of the node is returned.
Pos3d XC::Element::getPosNodo(const size_t &i,bool initialGeometry) const
  { return getNodePtrs().getPosNodo(i,initialGeometry); }

//! @brief Returns a grid of points distributed along the line.
TritrizPos3d XC::Element::getPuntos(const size_t &ni,const size_t &nj,const size_t &nk,bool initialGeometry)
  {
    TritrizPos3d retval;
    std::cerr << "Function getPuntos must be implemented in derived classes."
              << std::endl;
    return retval;
  }

//! @brief Resets tributary areas of connected nodes.
void XC::Element::resetTributaries(void) const
  { getNodePtrs().resetTributaries(); }

//! @brief Adds to the tributary magnitude of each node
//! the vector being passed as parameter.
void XC::Element::dumpTributaries(const std::vector<double> &t) const
  { getNodePtrs().dumpTributaries(t); }

//! @brief Computes the tributary lengths that corresponds to each
//! node of the element
void XC::Element::computeTributaryLengths(bool initialGeometry) const
  {
    std::cerr << "Function computeTributaryLengths "
              << "must be overloaded in derived classes."
              << std::endl;
  }

//! @brief Returns the tributary length corresponding to the node being passed
//! as parameter.
double XC::Element::getTributaryLength(const Node *) const
  { return 0; }

//! @brief Returns the tributary length corresponding to the node cuyo tag se pasa
//! as parameter.
double XC::Element::getTributaryLengthByTag(const int &tag) const
  {
    const Node *nod= getDomain()->getNode(tag);
    return getTributaryLength(nod);
  }

//! @brief Compute tributary areas for each element node.
void XC::Element::computeTributaryAreas(bool initialGeometry) const
  {
    std::cerr << "Function computeTributaryAreas "
              << "must be overloaded in derived classes."
              << std::endl;
  }

//! @brief Returns the tributary area corresponding to the node.
double XC::Element::getTributaryArea(const Node *) const
  { return 0; }

//! @brief Returns the tributary area corresponding to the node.
double XC::Element::getTributaryAreaByTag(const int &tag) const
  {
    const Node *nod= getDomain()->getNode(tag);
    return getTributaryArea(nod);
  }

//! @brief Computes the tributary volumes that corresponds to each
//! node of the element
void XC::Element::computeTributaryVolumes(bool initialGeometry) const
  {
    std::cerr << "Function computeTributaryVolumes "
              << "must be overloaded in derived classes."
              << std::endl;
  }

//! @brief Returns the tributary volume corresponding to the node being passed
//! as parameter.
double XC::Element::getTributaryVolume(const Node *) const
  { return 0; }

//! @brief Returns the tributary volume corresponding to the node cuyo tag se pasa
//! as parameter.
double XC::Element::getTributaryVolumeByTag(const int &tag) const
  {
    const Node *nod= getDomain()->getNode(tag);
    return getTributaryVolume(nod);
  }

//! @brief Returns the squared distance from the element to the point
//! being passed as parameter.
double XC::Element::getDist2(const Pos2d &p,bool initialGeometry) const
  {
    std::cerr << "Function getDist2(Pos2d) is not defined for element of type: '"
              << nombre_clase() << "'" << std::endl;
    return 0.0;
  }

//! @brief Returns the the distance from the element to the point
//! being passed as parameter.
double XC::Element::getDist(const Pos2d &p,bool initialGeometry) const
  {
    std::cerr << "Function getDist(Pos2d) is not defined for element of type: '"
              << nombre_clase() << "'" << std::endl;
    return 0.0;
  }

//! @brief Returns the squared distance from the element to the point
//! being passed as parameter.
double XC::Element::getDist2(const Pos3d &p,bool initialGeometry) const
  {
    std::cerr << "Function getDist2(Pos3d) is not defined for element of type: '"
              << nombre_clase() << "'" << std::endl;
    return 0.0;
  }

//! @brief Returns the the distance from the element to the point
//! being passed as parameter.
double XC::Element::getDist(const Pos3d &p,bool initialGeometry) const
  {
    std::cerr << "Function getDist(Pos3d) is not defined for element of type: '"
              << nombre_clase() << "'" << std::endl;
    return 0.0;
  }

//! @brief Returns the coordinates del centro de gravedad of the element.
Pos3d XC::Element::getPosCdg(bool initialGeometry) const
  {
    std::cerr << __FUNCTION__ << " not implemented for type: "
              << nombre_clase() << " elements." << std::endl;
    static Pos3d retval;
    return retval;
  }

//! @brief Returns the coordinates del centro de gravedad of the element.
XC::Vector XC::Element::getCooCdg(bool initialGeometry) const
  {
    const Pos3d cdg= getPosCdg(initialGeometry);
    Vector retval(3);
    retval(0)= cdg.x();
    retval(1)= cdg.y();
    retval(2)= cdg.z();
    return retval;
  }

//! @brief Places the element on the mesh.
XC::TritrizPtrElem XC::Element::put_on_mesh(const XC::TritrizPtrNod &,meshing_dir) const
  {
    std::cerr << "Método put_on_mesh no implementado" << std::endl;
    return TritrizPtrElem();
  }

XC::TritrizPtrElem XC::Element::cose(const SetEstruct &f1,const SetEstruct &f2) const
  {
    std::cerr << "Método cose no implementado" << std::endl;
    return TritrizPtrElem();
  }

//! @brief Sends object members through the channel being passed as parameter.
int XC::Element::sendData(CommParameters &cp)
  {
    int res= MeshComponent::sendData(cp);
    setDbTagDataPos(4,nodeIndex);
    res+= cp.sendVector(load,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Element::recvData(const CommParameters &cp)
  {
    int res= MeshComponent::recvData(cp);
    nodeIndex= getDbTagDataPos(4);
    res+= cp.receiveVector(load,getDbTagData(),CommMetaData(5));
    return res;
  }
