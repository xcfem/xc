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
#include "NodePtrsWithIDs.h"
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
#include "material/Material.h"
#include "utility/matrix/DqMatrices.h"
#include "utility/matrix/DqVectors.h"
#include "utility/tagged/DefaultTag.h"
#include "med.h"
#include "domain/mesh/element/gauss_models/GaussModel.h"
#include "utility/med_xc/MEDGaussModel.h"
#include "utility/actor/actor/CommMetaData.h"
#include "vtkCellType.h"

std::deque<XC::Matrix> XC::Element::theMatrices;
std::deque<XC::Vector> XC::Element::theVectors1;
std::deque<XC::Vector> XC::Element::theVectors2;
double XC::Element::dead_srf= 1e-6;//Factor de reducción de la rigidez para elementos desactivados.
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

//! @brief Consuma el estado del elemento.
int XC::Element::commitState(void)
  {
    if(!Kc.Nula())
      Kc= getTangentStiff();
    return 0;
  }

//! @brief Actualiza el estado del elemento.
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

//! @brief Devuelve la dimensión del elemento.
size_t XC::Element::getDimension(void) const
  {
    std::cerr << "Element::getDimension no implementada para '"
              << nombre_clase() << "'\n";
    return 0;
  }

//! @brief Asigna los nodos.
void XC::Element::setIdNodos(const std::vector<int> &inodos)
  { getNodePtrs().set_id_nodos(inodos); }

//! @brief Asigna los nodos.
void XC::Element::setIdNodos(const ID &inodos)
  { getNodePtrs().set_id_nodos(inodos); }

//! @brief Asigna el dominio al elemento.
void XC::Element::setDomain(Domain *theDomain)
  {
    MeshComponent::setDomain(theDomain);
    if(!theDomain)
      {
        std::cerr << "Element::setDomain -- Domain is null\n";
        getNodePtrs().inic();
      }
    else
      getNodePtrs().set_ptr_nodos(theDomain);
  }


//! @brief Anula el vector de cargas aplicadas del elemento.
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

//! @brief Devuelve la matriz de amortiguamiento.
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


//! @brief Devuelve la matriz de masas.
const XC::Matrix &XC::Element::getMass(void) const
  {
    if(index  == -1)
      setRayleighDampingFactors(RayleighDampingFactors()); //Anula los factores de amortiguamiento.

    // zero the matrix & return it
    Matrix &theMatrix= theMatrices[index];
    theMatrix.Zero();
    return theMatrix;
  }

//! @brief Devuelve la acción del elemento sobre los nodos Forma la matriz de amortiguamiento.
const XC::Vector &XC::Element::getResistingForceIncInertia(void) const
  {
    if(index == -1)
      setRayleighDampingFactors(RayleighDampingFactors()); //Anula los factores de amortiguamiento.

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

//! @brief Devuelve la fuerza generalizada del elemento sobre el nodo cuyo
//! índice se pasa como parámetro.
const XC::Vector &XC::Element::getNodeResistingComponents(const size_t &iNod,const Vector &rf) const
  {
    static Vector retval;
    const int ngdl= getNodePtrs()[iNod]->getNumberDOF(); //Grados de libertad del nodo.
    retval.resize(ngdl);
    for(int i=0;i<ngdl;i++)
      retval[i]= rf(iNod*ngdl+i);
    return retval; 
  }

//! @brief Devuelve la fuerza generalizada del elemento sobre el nodo cuyo
//! índice se pasa como parámetro.
const XC::Vector &XC::Element::getNodeResistingForce(const size_t &iNod) const
  {
    const Vector &rf= getResistingForce();
    return getNodeResistingComponents(iNod,rf);
  }

//! @brief Devuelve la fuerza generalizada (incluyendo fuerzas de inercia)
//! del elemento sobre el nodo cuyo índice se pasa como parámetro.
const XC::Vector &XC::Element::getNodeResistingForceIncInertia(const size_t &iNod) const
  {
    const Vector &rf= getResistingForceIncInertia();
    return getNodeResistingComponents(iNod,rf);
  }

//! @brief Devuelve la fuerza generalizada del elemento sobre el nodo al
//! que apunta el parámetro.
const XC::Vector &XC::Element::getNodeResistingForce(const Node *ptrNod) const
  {
    const int iNodo= getNodePtrs().getIndiceNodo(ptrNod);
    assert(iNodo>=0);
    return getNodeResistingForce(iNodo);
  }

//! @brief Devuelve la fuerza generalizada (incluyendo fuerzas de inercia) del elemento sobre el nodo al
//! que apunta el parámetro.
const XC::Vector &XC::Element::getNodeResistingForceIncInertia(const Node *ptrNod) const
  {
    const int iNodo= getNodePtrs().getIndiceNodo(ptrNod);
    assert(iNodo>=0);
    return getNodeResistingForceIncInertia(iNodo);
  }

//! @brief Devuelve la fuerza estática equivalente para el modo
//! que se pasa como parámetro y la aceleración correspondiente a dicho modo.
XC::Vector XC::Element::getEquivalentStaticLoad(int mode,const double &accel_mode) const
  {
    const Matrix &matriz_masas= getMass();
    const Vector distrib_factors= getNodePtrs().getDistributionFactor(mode);
    Vector retval= matriz_masas*distrib_factors;
    retval*=(accel_mode);
    return retval;
  }

//! @brief Devuelve la fuerza estática equivalente en cada nodo para el modo
//! que se pasa como parámetro y la aceleración correspondiente a dicho modo.
XC::Matrix XC::Element::getEquivalentStaticNodalLoads(int mode,const double &accel_mode) const
  {
    const Vector element_load= getEquivalentStaticLoad(mode,accel_mode);
    return getNodePtrs().getNodeVectors(element_load);
  }

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


//! @brief Agrega las reacciones a los nodos.
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
    std::cerr << "Element::getVtkCellType: la función getVtkCellType debe definirse en las clases derivadas." << std::endl;
    return VTK_EMPTY_CELL;
  }

//! @brief Interfaz con el formato MED de Salome.
int XC::Element::getMEDCellType(void) const
  {
    std::cerr << "Element::getMEDCellType: la función getMEDCellType debe definirse en las clases derivadas." << std::endl;
    return ::MED_NONE;
  }

//! @brief Devuelve el modelo de integración por Gauss del elemento.
const XC::GaussModel &XC::Element::getGaussModel(void) const
  {
    std::cerr << "La función Element::getMEDCellType debe redefinirse en cada clase de elemento." << std::endl;
    return gauss_model_empty;
  }

//! @brief Devuelve el modelo de integración por Gauss de la biblioteca MED.
XC::MEDGaussModel XC::Element::getMEDGaussModel(void) const
  {
    MEDGaussModel retval(nombre_clase(),getMEDCellType(),getGaussModel());
    return retval;
  }

//! @brief Devuelve los nodos del borde (o arista) del elemento
//! cuyo índice se pasa como parámetro.
XC::Element::NodesEdge XC::Element::getNodesEdge(const size_t &) const
  {
    NodesEdge retval;
    std::cerr << nombre_clase()
              << "; no se ha definido getNodesEdge()."
              << std::endl;
    return retval;
  }

//! @brief Devuelve el borde (o arista) del elemento
//! que tiene por extremos los nodos que se pasan como parámetros.
int XC::Element::getEdgeNodes(const Node *,const Node *) const
  {
    std::cerr << nombre_clase()
              << "; no se ha definido getEdgeNodes()."
              << std::endl;
    return -1;
  }

//! @brief Devuelve el borde del elemento
//! que tiene por extremos los nodos que se pasan como parámetros.
int XC::Element::getEdgeNodes(const int &iN1,const int &iN2) const
  {
    const Domain *dom= this->getDomain();
    const Node *n1= dom->getNode(iN1);
    const Node *n2= dom->getNode(iN2);
    return getEdgeNodes(n1,n2);
  }
 
//! @brief Devuelve los bordes del elemento
//! que tienen por extremo el nodo que se pasa como parámetro.
XC::ID XC::Element::getEdgesNode(const Node *) const
  {
    ID retval;
    std::cerr << nombre_clase()
              << "; no se ha definido getEdgesNode()."
              << std::endl;
    return retval;
  }

//! @brief Devuelve los bordes del elemento que tienen ambos extremos
//! en el conjunto de nodos que se pasa como parámetro.
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

//! @brief Devuelve los bordes del elemento
//! que tienen por extremo el nodo cuyo tag se pasa como parámetro.
XC::ID XC::Element::getEdgesNodeByTag(const int &iN) const
  {
    const Domain *dom= this->getDomain();
    const Node *n= dom->getNode(iN);
    return getEdgesNode(n);
  }
 
//! @brief Devuelve los índices locales de los nodos del elemento
//! situados sobre el borde (o arista) que se pasa como parámetros.
XC::ID XC::Element::getLocalIndexNodesEdge(const size_t &i) const
  {
    ID retval;
    std::cerr << nombre_clase()
              << "; no se ha definido getLocalIndexNodesEdge()."
              << std::endl;
    return retval;
  }

//! @brief Devuelve los conjuntos a los que pertenece este elemento.
std::set<XC::SetBase *> XC::Element::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Preprocessor *preprocessor= GetPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << "Element::get_sets; no se ha definido el preprocesador." << std::endl;
    return retval;
  }

//! @brief Agrega el elemento a los conjuntos que se pasan como parámetro.
void XC::Element::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        SetMeshComp *s= dynamic_cast<SetMeshComp *>(*i);
        if(s) s->agregaElemento(this);
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

//! @brief Devuelve el valor máximo de la coordenada i de los nodos del elemento.
double XC::Element::MaxCooNod(int icoo) const
  { return getNodePtrs().MaxCooNod(icoo); }

//! @brief Devuelve el valor mínimo de la coordenada i de los nodos del elemento.
double XC::Element::MinCooNod(int icoo) const
  { return getNodePtrs().MinCooNod(icoo); }

//! @brief Devuelve las coordenadas de los nodos.
const XC::Matrix &XC::Element::getCooNodos(void) const
  { return getNodePtrs().getCoordinates(); }

//! @brief Devuelve las coordenadas de los nodos.
std::list<Pos3d> XC::Element::getPosNodos(bool initialGeometry) const
  { return getNodePtrs().getPosiciones(initialGeometry); }

//! @brief Returs a matrix with the axes of the element as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
XC::Matrix XC::Element::getLocalAxes(bool initialGeometry) const
  {
    Matrix retval;
    std::cerr << "Function getLocalAxes must be implemented in derived class:" 
              << nombre_clase() << std::endl;
    return retval;
  }


//! @brief Devuelve la posición del nodo cuyo índice se
//! pasa como parámetro.
Pos3d XC::Element::getPosNodo(const size_t &i,bool initialGeometry) const
  { return getNodePtrs().getPosNodo(i,initialGeometry); }

//! @brief Devuelve puntos distribuidos en el elemento.
TritrizPos3d XC::Element::getPuntos(const size_t &ni,const size_t &nj,const size_t &nk,bool initialGeometry)
  {
    TritrizPos3d retval;
    std::cerr << "La función getPuntos debe redefinirse en la clase hija."
              << std::endl;
    return retval;
  }

//! @brief Resets tributary areas of connected nodes.
void XC::Element::resetTributarias(void) const
  { getNodePtrs().resetTributarias(); }

//! @brief Agrega al la magnitud tributaria de cada nodo i
//! la componente i del vector que se pasa como parámetro.
void XC::Element::vuelcaTributarias(const std::vector<double> &t) const
  { getNodePtrs().vuelcaTributarias(t); }

//! @brief Calcula las longitudes tributarias correspondientes a cada
//! nodo del elemento
void XC::Element::calculaLongsTributarias(bool initialGeometry) const
  {
    std::cerr << "La función calculaLongsTributarias "
              << "debe redefinirse en la clase hija."
              << std::endl;
  }

//! @brief Devuelve la longitud tributaria correspondiente al nodo que se pasa
//! como parámetro.
double XC::Element::getLongTributaria(const Node *) const
  { return 0; }

//! @brief Devuelve la longitud tributaria correspondiente al nodo cuyo tag se pasa
//! como parámetro.
double XC::Element::getLongTributariaByTag(const int &tag) const
  {
    const Node *nod= getDomain()->getNode(tag);
    return getLongTributaria(nod);
  }

//! @brief Calcula las áreas tributarias correspondientes a cada
//! nodo del elemento
void XC::Element::calculaAreasTributarias(bool initialGeometry) const
  {
    std::cerr << "La función calculaAreasTributarias "
              << "debe redefinirse en la clase hija."
              << std::endl;
  }

//! @brief Devuelve el área tributaria correspondiente al nodo que se pasa
//! como parámetro.
double XC::Element::getAreaTributaria(const Node *) const
  { return 0; }

//! @brief Devuelve el área tributaria correspondiente al nodo cuyo tag se pasa
//! como parámetro.
double XC::Element::getAreaTributariaByTag(const int &tag) const
  {
    const Node *nod= getDomain()->getNode(tag);
    return getAreaTributaria(nod);
  }

//! @brief Calcula los volúmenes tributarios correspondientes a cada
//! nodo del elemento
void XC::Element::calculaVolsTributarios(bool initialGeometry) const
  {
    std::cerr << "La función calculaVolsTributarios "
              << "debe redefinirse en la clase hija."
              << std::endl;
  }

//! @brief Devuelve el volumen tributario correspondiente al nodo que se pasa
//! como parámetro.
double XC::Element::getVolTributario(const Node *) const
  { return 0; }

//! @brief Devuelve el volumen tributario correspondiente al nodo cuyo tag se pasa
//! como parámetro.
double XC::Element::getVolTributarioByTag(const int &tag) const
  {
    const Node *nod= getDomain()->getNode(tag);
    return getVolTributario(nod);
  }

//! @brief Devuelve el cuadrado de la distancia desde el elemento al punto que
//! se pasa como parámetro.
double XC::Element::getDist2(const Pos2d &p,bool initialGeometry) const
  {
    std::cerr << "La función getDist2(Pos2d) no se ha definido para los elementos de clase: '"
              << nombre_clase() << "'" << std::endl;
    return 0.0;
  }

//! @brief Devuelve la distancia desde el elemento al punto que
//! se pasa como parámetro.
double XC::Element::getDist(const Pos2d &p,bool initialGeometry) const
  {
    std::cerr << "La función getDist(Pos2d) no se ha definido para los elementos de clase: '"
              << nombre_clase() << "'" << std::endl;
    return 0.0;
  }

//! @brief Devuelve el cuadrado de la distancia desde el elemento al punto que
//! se pasa como parámetro.
double XC::Element::getDist2(const Pos3d &p,bool initialGeometry) const
  {
    std::cerr << "La función getDist2(Pos3d) no se ha definido para los elementos de clase: '"
              << nombre_clase() << "'" << std::endl;
    return 0.0;
  }

//! @brief Devuelve la distancia desde el elemento al punto que
//! se pasa como parámetro.
double XC::Element::getDist(const Pos3d &p,bool initialGeometry) const
  {
    std::cerr << "La función getDist(Pos3d) no se ha definido para los elementos de clase: '"
              << nombre_clase() << "'" << std::endl;
    return 0.0;
  }

//! @brief Devuelve las coordenadas del centro de gravedad del elemento.
const Pos3d &XC::Element::getPosCdg(bool initialGeometry) const
  {
    std::cerr << "getPosCdg no implementada para los elementos de tipo: "
              << nombre_clase() << std::endl;
    static Pos3d retval;
    return retval;
  }

//! @brief Devuelve las coordenadas del centro de gravedad del elemento.
const XC::Vector &XC::Element::getCooCdg(bool initialGeometry) const
  {
    const Pos3d &cdg= getPosCdg(initialGeometry);
    static Vector retval(3);
    retval(0)= cdg.x();
    retval(1)= cdg.y();
    retval(2)= cdg.z();
    return retval;
  }

XC::TritrizPtrElem XC::Element::coloca_en_malla(const XC::TritrizPtrNod &,dir_mallado) const
  {
    std::cerr << "Método coloca_en_malla no implementado" << std::endl;
    return TritrizPtrElem();
  }

XC::TritrizPtrElem XC::Element::cose(const SetEstruct &f1,const SetEstruct &f2) const
  {
    std::cerr << "Método cose no implementado" << std::endl;
    return TritrizPtrElem();
  }

//! @brief Envia los miembros del objeto a través del canal que se pasa como parámetro.
int XC::Element::sendData(CommParameters &cp)
  {
    int res= MeshComponent::sendData(cp);
    setDbTagDataPos(4,nodeIndex);
    res+= cp.sendVector(load,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::Element::recvData(const CommParameters &cp)
  {
    int res= MeshComponent::recvData(cp);
    nodeIndex= getDbTagDataPos(4);
    res+= cp.receiveVector(load,getDbTagData(),CommMetaData(5));
    return res;
  }
