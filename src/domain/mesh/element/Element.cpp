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
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/SetMeshComp.h"
#include "boost/any.hpp"


#include "xc_utils/src/geom/pos_vec/Pos3dArray3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/coo_sys/Rect3d3dCooSys.h"
#include "material/Material.h"
#include "utility/matrix/DqMatrices.h"
#include "utility/matrix/DqVectors.h"
#include "utility/tagged/DefaultTag.h"

#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "utility/actor/actor/CommMetaData.h"
#include "vtkCellType.h"

std::deque<XC::Matrix> XC::Element::theMatrices;
std::deque<XC::Vector> XC::Element::theVectors1;
std::deque<XC::Vector> XC::Element::theVectors2;
double XC::Element::dead_srf= 1e-6;//Stiffness reduction factor for dead (non active) elements.
XC::DefaultTag XC::Element::defaultTag;

//! @brief Constructor that takes the element's unique tag and the number
//! of external nodes for the element.
//!
//! @param tag: element identifier.
//! @param cTag: element class identifier.
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

//! @brief Commit the current element state.
//!
//! The element is to commit its current state. To return 0 if
//! successful, a negative number if not.
int XC::Element::commitState(void)
  {
    if(!Kc.isEmpty())
      Kc= getTangentStiff();
    return 0;
  }

//! @brief Updates the element state.
//!
//! This method is invoked after the response quantities have been updated
//! in the Domain, but not necessarily committed, e.g. during a
//! non-linear Newton-Raphson solution algorithm. To return \f$0\f$ if
//! successful, a negative number if not. This base class implementation
//! returns 0.
int XC::Element::update(void)
  { return 0; }

//! @brief Reverts the element to its initial state.
//!
//! The element is to set it's current state to the state it was at before
//! the analysis started. To return 0 if successful, a negative number
//! if not. 
int XC::Element::revertToStart(void)
  { return 0; }

//! @brief Set Rayleigh damping factors.
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
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "is not implemented.\n";
    return 0;
  }

//! @brief Set the nodes.
void XC::Element::setIdNodes(const std::vector<int> &inodes)
  { getNodePtrs().set_id_nodes(inodes); }

//! @brief Set the nodes.
void XC::Element::setIdNodes(const ID &inodes)
  { getNodePtrs().set_id_nodes(inodes); }

//! @brief Sets the domain for the element.
void XC::Element::setDomain(Domain *theDomain)
  {
    MeshComponent::setDomain(theDomain);
    if(!theDomain)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << " the domain parameter is null\n";
        getNodePtrs().inic();
      }
    else
      getNodePtrs().set_node_ptrs(theDomain);
  }

//! @brief Creates the inertia load that corresponds to the
//! acceleration argument.
void XC::Element::createInertiaLoad(const Vector &)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
  }

//! @brief Zeroes the loads over the element.
//!
//! This is a method invoked to zero the element load contributions to the
//! residual, i.e. \f$ P_e = 0 \f$ 
void XC::Element::zeroLoad(void)
  { load.Zero(); }

//! @brief Computes the damping matrix.
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

//! @brief Returns the damping matrix.
//!
//! To return the damping matrix. The element is to compute its
//! damping matrix based on the original location of the nodes and the
//! current trial response quantities at the nodes. 
//! \f[
//! C_e= {\frac{\partial R_i}{\partial \dot U} \vert}_{U_{trial}}
//! \f]
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
//!
//! Returns the mass matrix. The element is to compute its
//! mass matrix based on the original location of the nodes and the
//! current trial response quantities at the nodes. 
//! \f[
//! M_e= {\frac{\partial I_i}{\partial \ddot U} \vert}_{U_{trial}}
//! \f]
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
    const int ndof= getNodePtrs()[iNod]->getNumberDOF(); // number of DOFs in the node.
    retval.resize(ndof);
    for(int i=0;i<ndof;i++)
      retval[i]= rf(iNod*ndof+i);
    return retval; 
  }

//! @brief Returns the generalized force of the element over the iNod-th
//! node.
const XC::Vector &XC::Element::getNodeResistingForce(const size_t &iNod) const
  {
    const Vector &rf= getResistingForce();
    return getNodeResistingComponents(iNod,rf);
  }

//! @brief Returns the generalized force (including inertia forces)
//! of the element over the node identified by the argument.
const XC::Vector &XC::Element::getNodeResistingForceIncInertia(const size_t &iNod) const
  {
    const Vector &rf= getResistingForceIncInertia();
    return getNodeResistingComponents(iNod,rf);
  }

//! @brief Returns the generalized force (including inertia forces)
//! of the element over the node identified by the argument.
const XC::Vector &XC::Element::getNodeResistingForce(const Node *ptrNod) const
  {
    const int iNode= getNodePtrs().getNodeIndex(ptrNod);
    assert(iNode>=0);
    return getNodeResistingForce(iNode);
  }

//! @brief Returns the generalized force (including inertia forces)
//! of the element over the node identified by the argument.
const XC::Vector &XC::Element::getNodeResistingForceIncInertia(const Node *ptrNod) const
  {
    const int iNode= getNodePtrs().getNodeIndex(ptrNod);
    assert(iNode>=0);
    return getNodeResistingForceIncInertia(iNode);
  }

//! @brief Returns the equivalent static load for the mode
//! being passed as parameter and the acceleration that corresponding that mode.
XC::Vector XC::Element::getEquivalentStaticLoad(int mode,const double &accel_mode) const
  {
    const Matrix &mass_matrix= getMass();
    const Vector distrib_factors= getNodePtrs().getDistributionFactor(mode);
    Vector retval= mass_matrix*distrib_factors;
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

//! @brief Returns true if the element is a subdomain.
bool XC::Element::isSubdomain(void)
  { return false; }

//! setResponse() is a method invoked to determine if the element
//! will respond to a request for a certain of information. The
//! information requested of the element is passed in the array of char
//! pointers \p argv of length {em argc}. If the element does not
//! respond to the request a \f$-1\f$ is returned. If it does respond, an
//! integer value greater than or equal to \f$0\f$ is returned. This is the
//! \p responseID passed in the getResponse() method. In addition
//! the Element object is responsible for setting the Information object
//! \p eleInformation with the type of the return, i.e. {\em IntType,
//! DoubleType, MatrixType, VectorType, IDType}, and for creating a Matrix,
//! Vector or ID object for the Information object, if the information to
//! be returned is of any of these types. The information object is
//! responsible for invoking the destructor on these objects. The base
//! class responds to no requests and will always return \f$-1\f$.
XC::Response *XC::Element::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    if(argv[0] == "force" || argv[0] == "forces" ||
        argv[0] == "globalForce" || argv[0] == "globalForces")
      return new ElementResponse(this, 1, this->getResistingForce());
    return 0;
  }

//! @brief Obtain information from an analysis.
//!
//! getResponse is a method invoked to obtain information from an
//! analysis. The method is invoked with the integer argument returned and
//! the Information object that was prepared in a successful {\em
//! setResponse()} method invocation. To return \f$0\f$ if successful, a
//! negative number if not. The base class implementation will always
//! return \f$-1\f$. 
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
        std::cerr << getClassName() << __FUNCTION__
		  << "; Rayleigh damping with non-zero betaCurrentTangent"
	          << " is not compatible with DDM sensitivity analysis"
		  << std::endl;
      }
    if(rayFactors.getBetaK0() != 0.0)
      theMatrix.addMatrix(1.0, this->getInitialStiffSensitivity(gradNumber), rayFactors.getBetaK0());
    if(rayFactors.getBetaKc() != 0.0)
      {
        theMatrix.addMatrix(1.0, Kc, 0.0);      // Don't use this and DDM
        std::cerr << getClassName() << __FUNCTION__
                  << "Rayleigh damping with non-zero betaCommittedTangent"
	          << " is not compatible with DDM sensitivity analysis"
		  << std::endl;
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
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; HUH!\n";
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
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; must be overloaded in derived classes."
              << std::endl;
    static const int numberNodes= getNumExternalNodes();
    return Vector(numberNodes);
  }

//! @brief Returns interpolattion factors for a material point.
XC::Vector XC::Element::getInterpolationFactors(const Pos3d &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; must be overloaded in derived classes."
              << std::endl;
    static const int numberNodes= getNumExternalNodes();
    return Vector(numberNodes);
  }

//! @brief Interfaz con VTK.
int XC::Element::getVtkCellType(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; must be overloaded in derived classes."
              << std::endl;
    return VTK_EMPTY_CELL;
  }

//! @brief Returns the Gauss integration model of the element.
const XC::GaussModel &XC::Element::getGaussModel(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; must be overloaded in derived classes."
              << std::endl;
    return gauss_model_empty;
  }

//! @brief Returns the nodes of the element edge.
XC::Element::NodesEdge XC::Element::getNodesEdge(const size_t &) const
  {
    NodesEdge retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; is not defined."
              << std::endl;
    return retval;
  }

//! @brief Returns the edge of the element
//! that ends in the nodes being passed as parameters.
int XC::Element::getEdgeNodes(const Node *,const Node *) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; is not defined."
              << std::endl;
    return -1;
  }

//! @brief Returns the edge of the element
//! that ends in the nodes with the tags being passed as parameters.
int XC::Element::getEdgeNodes(const int &iN1,const int &iN2) const
  {
    const Domain *dom= this->getDomain();
    const Node *n1= dom->getNode(iN1);
    const Node *n2= dom->getNode(iN2);
    return getEdgeNodes(n1,n2);
  }
 
//! @brief Returns the edges of the element
//! that ends in the node being passed as parameter.
XC::ID XC::Element::getEdgesNode(const Node *) const
  {
    ID retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; is not defined."
              << std::endl;
    return retval;
  }

//! @brief Returns the element edges that have both ends
//! in the node set being passed as parameter.
std::set<int> XC::Element::getEdgesNodes(const NodePtrSet &nodes) const
  {
    std::set<int> retval;
    for(NodePtrSet::const_iterator i= nodes.begin();i!=nodes.end();i++)
      {
        const Node *node= *i;
        const ID edgeIds= getEdgesNode(node);
        const int sz= edgeIds.Size();
        for(int i=0;i<sz;i++)
          {
            NodesEdge nodesEdge= getNodesEdge(edgeIds(i));
            if(in(nodes,nodesEdge))
              retval.insert(edgeIds(i));
          }
      }
    return retval;
  }

//! @brief Returns the edges of the element
//! that have an en in the node with the tag being passed as parameter.
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
    std::cerr << getClassName() << "::" << __FUNCTION__
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
      std::cerr << getClassName() << __FUNCTION__
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

//! @brief Return the indexes of the nodes (used when creating
//! VTK meshes).
std::vector<int> XC::Element::getIdxNodes(void) const
  { return getNodePtrs().getIdx(); }

//! @brief Returns the maximum value of the i coordinate of the element nodes.
double XC::Element::MaxCooNod(int icoo) const
  { return getNodePtrs().MaxCooNod(icoo); }

//! @brief Returns the minimum value of the i coordinate of the element nodes.
double XC::Element::MinCooNod(int icoo) const
  { return getNodePtrs().MinCooNod(icoo); }

//! @brief Returns the coordinates of the nodes.
const XC::Matrix &XC::Element::getCooNodes(void) const
  { return getNodePtrs().getCoordinates(); }

//! @brief Returns the coordinates of the nodes.
std::deque<Pos3d> XC::Element::getPosNodes(bool initialGeometry) const
  { return getNodePtrs().getPositions(initialGeometry); }

bool XC::Element::In(const GeomObj3d &obj,const double &factor, const double &tol) const
  { return getNodePtrs().In(obj,factor,tol); }
bool XC::Element::Out(const GeomObj3d &obj,const double &factor, const double &tol) const
  { return getNodePtrs().Out(obj,factor,tol); }
bool XC::Element::In(const GeomObj2d &obj,const double &factor, const double &tol) const
  { return getNodePtrs().In(obj,factor,tol); }
bool XC::Element::Out(const GeomObj2d &obj,const double &factor, const double &tol) const
  { return getNodePtrs().Out(obj,factor,tol); }


//! @brief Returns a matrix with the axes of the element as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
XC::Matrix XC::Element::getLocalAxes(bool initialGeometry) const
  {
    Matrix retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " must be implemented in derived classes." << std::endl;
    return retval;
  }

//! @brief Returns a base vector in the direction of the local i-th axis
//! from the i-th row of the matrix returned by getLocalAxes.
XC::Vector XC::Element::getBaseVector(size_t i,bool initialGeometry) const
  {
    const Matrix localAxes= getLocalAxes(initialGeometry);
    Vector retval(3,0.0);
    const size_t nCols= localAxes.noCols();
    for(size_t j= 0;j<nCols;j++)
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
Rect3d3dCooSys XC::Element::getCooSys(bool initialGeometry) const
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
    return Rect3d3dCooSys(x,y,z);
  }    


//! @brief Returns the position of the i-th node.
//! @param i: index of the node in the element.
//! @param initialGeometry: if true initial position of the node is returned.
Pos3d XC::Element::getPosNode(const size_t &i,bool initialGeometry) const
  { return getNodePtrs().getPosNode(i,initialGeometry); }

//! @brief Returns a grid of points distributed along the line.
Pos3dArray3d XC::Element::getPoints(const size_t &ni,const size_t &nj,const size_t &nk,bool initialGeometry)
  {
    Pos3dArray3d retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; must be implemented in derived classes."
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
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " must be overloaded in derived classes."
              << std::endl;
  }

//! @brief Returns the tributary length corresponding to the node being passed
//! as parameter.
double XC::Element::getTributaryLength(const Node *) const
  { return 0; }

//! @brief Returns the tributary length corresponding to the node which tag se pasa
//! as parameter.
double XC::Element::getTributaryLengthByTag(const int &tag) const
  {
    const Node *nod= getDomain()->getNode(tag);
    return getTributaryLength(nod);
  }

//! @brief Compute tributary areas for each element node.
void XC::Element::computeTributaryAreas(bool initialGeometry) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; must be implemented in derived classes."
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
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; must be implemented in derived classes."
              << std::endl;
  }

//! @brief Returns the tributary volume corresponding to the node being passed
//! as parameter.
double XC::Element::getTributaryVolume(const Node *) const
  { return 0; }

//! @brief Returns the tributary volume corresponding to the node which tag se pasa
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
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " is not implemented." << std::endl;
    return 0.0;
  }

//! @brief Returns the the distance from the element to the point
//! being passed as parameter.
double XC::Element::getDist(const Pos2d &p,bool initialGeometry) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " is not implemented." << std::endl;
    return 0.0;
  }

//! @brief Returns the squared distance from the element to the point
//! being passed as parameter.
double XC::Element::getDist2(const Pos3d &p,bool initialGeometry) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " is not implemented." << std::endl;
    return 0.0;
  }

//! @brief Returns the the distance from the element to the point
//! being passed as parameter.
double XC::Element::getDist(const Pos3d &p,bool initialGeometry) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " is not implemented." << std::endl;
    return 0.0;
  }

//! @brief Returns the coordinates del center of gravity of the element.
Pos3d XC::Element::getCenterOfMassPosition(bool initialGeometry) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " is not implemented." << std::endl;
    static Pos3d retval;
    return retval;
  }

//! @brief Returns the coordinates del center of gravity of the element.
XC::Vector XC::Element::getCenterOfMassCoordinates(bool initialGeometry) const
  {
    const Pos3d center_of_mass= getCenterOfMassPosition(initialGeometry);
    Vector retval(3);
    retval(0)= center_of_mass.x();
    retval(1)= center_of_mass.y();
    retval(2)= center_of_mass.z();
    return retval;
  }

//! @brief Return a grid of booleans, one for each of the
//! element nodes. If there is a node that doesn't exist
//! for a position the correspondin value will be false.
BoolArray3d XC::Element::getNodePattern(void) const
  {
    BoolArray3d retval(1,1,1,false);
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return retval;
  }


//! @brief Places the element on the mesh.
XC::ElemPtrArray3d XC::Element::put_on_mesh(const XC::NodePtrArray3d &,meshing_dir) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " is not implemented." << std::endl;
    return ElemPtrArray3d();
  }

XC::ElemPtrArray3d XC::Element::sew(const SetEstruct &f1,const SetEstruct &f2) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " is not implemented." << std::endl;
    return ElemPtrArray3d();
  }

//! @brief Return the names of the material(s) of the element.
std::set<std::string> XC::Element::getMaterialNames(void) const
  {
    std::set<std::string> retval;    
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return retval;
  }
  
//! @brief Return the names of the material(s) of the element in a Python list.
boost::python::list XC::Element::getMaterialNamesPy(void) const
  {
    boost::python::list retval;
    std::set<std::string> tmp= getMaterialNames();
    for(std::set<std::string>::const_iterator i= tmp.begin();i!=tmp.end();i++)
        retval.append(*i);
    return retval;
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
