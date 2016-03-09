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

// $Revision: 1.9 $
// $Date: 2003/02/25 23:33:13 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/zeroLength/ZeroLengthSection.cpp,v $

// Written: MHS
// Created: Sept 2000
//
// Description: This file contains the implementation for the
// ZeroLengthSection class.

#include <domain/mesh/element/zeroLength/ZeroLengthSection.h>
#include <domain/mesh/element/Information.h>

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "utility/actor/actor/MovableMatrix.h"
#include <material/section/SectionForceDeformation.h>
#include <utility/recorder/response/ElementResponse.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "material/section/ResponseId.h"
#include "utility/matrix/Matrix.h"
#include "utility/actor/actor/MatrixCommMetaData.h"

XC::Matrix XC::ZeroLengthSection::K6(6,6);
XC::Matrix XC::ZeroLengthSection::K12(12,12);

XC::Vector XC::ZeroLengthSection::P6(6);
XC::Vector XC::ZeroLengthSection::P12(12);

void XC::ZeroLengthSection::setup_section(const Material *sec)
  {
    libera();
    if(!sec)
      std::cerr << "ZeroLengthSection::setup_section; se pasó un puntero a material nulo." << std::endl;
    else
      {
        const SectionForceDeformation *ptr= dynamic_cast<const SectionForceDeformation *>(sec);
        if(!ptr)
          std::cerr << "ZeroLengthSection::setup_section; se pasó un puntero a material no válido."
                    << std::endl;
        else
          {
            theSection = ptr->getCopy();// Obtain copy of section model
            if(!theSection)
              {
                std::cerr << "ZeroLengthSection::ZeroLengthSection -- failed to get copy of section\n";
                exit(-1);
              }
            // Get the section order
            order = ptr->getOrder();
          }
      }
  }

//! @brief Crea la matriz A y el vector v.
void XC::ZeroLengthSection::inicAv(void)
  {
    // Allocate transformation matrix
    A= Matrix(order, numDOF);
    // Allocate section deformation vector
    v= Vector(order);
  }

//! @brief invoke the destructor on any objects created by the object
//! that the object still holds a pointer to
void XC::ZeroLengthSection::libera(void)
  {
    if(theSection) delete theSection;
    theSection=nullptr;
  }

//! @brief Constructor:
XC::ZeroLengthSection::ZeroLengthSection(int tag, int dim, int Nd1, int Nd2,
                                         const Vector &x, const Vector &yprime,
                                         SectionForceDeformation& sec)
  : Element0D(tag, ELE_TAG_ZeroLengthSection,Nd1,Nd2,dim,x,yprime),
    K(nullptr), P(nullptr), theSection(nullptr), order(0)
  { setup_section(&sec); }

//! @brief Constructor:
XC::ZeroLengthSection::ZeroLengthSection(int tag, int dimension,const Material *sec)
  : Element0D(tag, ELE_TAG_ZeroLengthSection,0,0,dimension),
    K(nullptr), P(nullptr), theSection(nullptr), order(0)
  {
    if(sec)
      setup_section(sec);
  }

//! @brief Constructor de copia.
XC::ZeroLengthSection::ZeroLengthSection(const ZeroLengthSection &otro)
  : Element0D(otro), A(otro.A), v(otro.v), K(nullptr), P(nullptr), theSection(nullptr), order(0)
  {
    if(otro.theSection)
      setup_section(otro.theSection);
  }

//! @brief Operador asignación.
XC::ZeroLengthSection &XC::ZeroLengthSection::operator=(const ZeroLengthSection &otro)
  {
    Element0D::operator=(otro);
    if(otro.theSection)
      setup_section(otro.theSection);
    A= otro.A;
    v= otro.v;
    return *this;
  }

//! @brief Constructor por defecto.
XC::ZeroLengthSection::ZeroLengthSection(void)
  : XC::Element0D(0, ELE_TAG_ZeroLengthSection,0,0,0),
    K(nullptr), P(nullptr), theSection(nullptr), order(0) {}

//! @brief Constructor virtual.
XC::Element *XC::ZeroLengthSection::getCopy(void) const
  { return new ZeroLengthSection(*this); }

//! @brief Destructor.
XC::ZeroLengthSection::~ZeroLengthSection(void)
  { libera(); }

// method: setDomain()
//    to set a link to the enclosing XC::Domain and to set the node pointers.
//    also determines the number of dof associated
//    with the XC::ZeroLengthSection element, we set matrix and vector pointers,
//    allocate space for t matrix and define it as the basic deformation-
//    displacement transformation matrix.
void XC::ZeroLengthSection::setDomain(Domain *theDomain)
  {
    Element0D::setDomain(theDomain);
    // check XC::Domain is not null - invoked when object removed from a domain

    // first set the node pointers
    int Nd1 = theNodes.getTagNode(0);
    int Nd2 = theNodes.getTagNode(1);

    // if can't find both - send a warning message
    if(theNodes[0] == 0 || theNodes[1] == 0)
      {
        if(theNodes[0] == 0)
          std::cerr << "ZeroLengthSection::setDomain() -- Nd2: " << Nd2 << " does not exist in ";
        else
          std::cerr << "ZeroLengthSection::setDomain() -- Nd2: " << Nd2 << " does not exist in ";
        std::cerr << "model for XC::ZeroLengthSection with id " << this->getTag() << std::endl;
        return;
      }

    // now determine the number of dof and the dimension
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();

    // if differing dof at the ends - print a warning message
    if(dofNd1 != dofNd2)
      {
        std::cerr << "ZeroLengthSection::setDomain() -- nodes " << Nd1 << " and " << Nd2
                  << "have differing dof at ends for XC::ZeroLengthSection " << this->getTag() << std::endl;
        return;
      }

    numDOF = 2*dofNd1;

    if(numDOF != 6 && numDOF != 12)
      std::cerr << "ZeroLengthSection::setDomain() -- el material sólo funciona con 3 (2d) ó 6 (3d) gdl por nodo.\n";

    // Set pointers to class wide objects
    if(numDOF == 6)
      {
        P= &P6;
        K= &K6;
      }
    else
      {
        P= &P12;
        K= &K12;
      }

    // Check that length is zero within tolerance
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    const Vector diff = end1Crd - end2Crd;
    double L  = diff.Norm();
    double v1 = end1Crd.Norm();
    double v2 = end2Crd.Norm();
    double vm;

    vm = (v1<v2) ? v2 : v1;

    if(L > LenTol*vm)
      std::cerr << "ZeroLengthSection::setDomain() -- Element " << this->getTag()
                << "has L= " << L << ", which is greater than the tolerance\n";

    // call the base class method
    XC::Element0D::setDomain(theDomain);

    setTransformation(); // Set up the A matrix
  }

int XC::ZeroLengthSection::commitState()
  {
    int retVal=0;

    // call element commitState to do any base class stuff
    if((retVal = this->XC::Element::commitState()) != 0)
      {
        std::cerr << "ZeroLength::commitState () - failed in base class\n";
      }
    // Commit the section
    retVal += theSection->commitState();
    return retVal;
  }

int XC::ZeroLengthSection::revertToLastCommit()
  {
    // Revert the section
    return theSection->revertToLastCommit();
  }

int XC::ZeroLengthSection::revertToStart()
  {
    // Revert the section to start
    return theSection->revertToStart();
  }

const XC::Matrix &XC::ZeroLengthSection::getTangentStiff(void) const
  {
    // Compute section deformation vector
    this->computeSectionDefs();

    // Set trial section deformation
    theSection->setTrialSectionDeformation(v);

    // Get section tangent stiffness, the element basic stiffness
    const XC::Matrix &kb = theSection->getSectionTangent();

    // Compute element stiffness ... K = A^*kb*A
    K->addMatrixTripleProduct(0.0,A, kb, 1.0);

    if(isDead())
      (*K)*=dead_srf;
    return *K;
  }

const XC::Matrix &XC::ZeroLengthSection::getInitialStiff(void) const
  {
    // Get section tangent stiffness, the element basic stiffness
    const XC::Matrix &kb = theSection->getInitialTangent();
    // Compute element stiffness ... K = A^*kb*A
    K->addMatrixTripleProduct(0.0,A, kb, 1.0);
    if(isDead())
      (*K)*=dead_srf;
    return *K;
  }

int XC::ZeroLengthSection::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "ZeroLengthSection::addLoad - load type unknown for truss with tag: "
              << this->getTag() << std::endl;
    return -1;
  }


int XC::ZeroLengthSection::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // does nothing as element has no mass yet!
    return 0;
  }

const XC::Vector &XC::ZeroLengthSection::getResistingForce(void) const
  {
    // Compute section deformation vector
    this->computeSectionDefs();

    // Set trial section deformation
    theSection->setTrialSectionDeformation(v);

    // Get section stress resultants, the element basic forces
    const XC::Vector &q = theSection->getStressResultant();

    // Compute element resisting force ... P = A^*q
    P->addMatrixTransposeVector(0.0,A, q, 1.0);

    if(isDead())
      (*P)*=dead_srf;
    return *P;
  }


const XC::Vector &XC::ZeroLengthSection::getResistingForceIncInertia(void) const
  {
    this->getResistingForce();

    // add the damping forces if rayleigh damping
    if(!rayFactors.KNulos())
      *P += this->getRayleighDampingForces();
    if(isDead())
      (*P)*=dead_srf;
    return *P;
  }

//! @brief Envía los miembros por el canal que se pasa como parámetro.
int XC::ZeroLengthSection::sendData(CommParameters &cp)
  {

    int res= Element0D::sendData(cp);
    res+= cp.sendInt(order,getDbTagData(),CommMetaData(9));
    res+= cp.sendMatrix(A,getDbTagData(),CommMetaData(10));
    res+= cp.sendVector(v,getDbTagData(),CommMetaData(11));
    res+= cp.sendMatrixPtr(K,getDbTagData(),MatrixCommMetaData(17,18,19,20));
    res+= cp.sendVectorPtr(P,getDbTagData(),ArrayCommMetaData(21,22,23));
    res+= cp.sendBrokedPtr(theSection,getDbTagData(),BrokedPtrCommMetaData(24,25,26));
    return res;
  }

//! @brief Recibe los miembros por el canal que se pasa como parámetro.
int XC::ZeroLengthSection::recvData(const CommParameters &cp)
  {
    int res= Element0D::recvData(cp);
    res+= cp.receiveInt(order,getDbTagData(),CommMetaData(9));
    res+= cp.receiveMatrix(A,getDbTagData(),CommMetaData(10));
    res+= cp.receiveVector(v,getDbTagData(),CommMetaData(14));
    K= cp.receiveMatrixPtr(K,getDbTagData(),MatrixCommMetaData(17,18,19,20));
    P= cp.receiveVectorPtr(P,getDbTagData(),ArrayCommMetaData(21,22,23));
    theSection= cp.getBrokedMaterial(theSection,getDbTagData(),BrokedPtrCommMetaData(24,25,26));
    return res;
  }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
int XC::ZeroLengthSection::sendSelf(CommParameters &cp)
  {
    inicComm(27);

    int res= sendData(cp);

    const int dataTag= getDbTag();
    res += cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "ZeroLength::sendSelf -- failed to send ID data\n";
    return res;
  }

//! @brief Recibe el objeto por el canal que se pasa como parámetro.
int XC::ZeroLengthSection::recvSelf(const CommParameters &cp)
  {
    inicComm(27);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::ZeroLengthSection::Print(std::ostream &s, int flag)
  {
    s << "ZeroLengthSection, tag: " << this->getTag() << std::endl;
    s << "\tConnected Nodes: " << theNodes << std::endl;
    s << "\tSection, tag: " << theSection->getTag() << std::endl;
  }

XC::Response* XC::ZeroLengthSection::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    // element forces
    if(argv[0] == "force" || argv[0] == "forces")
      return new ElementResponse(this, 1, *P);
    // element stiffness matrix
    else if(argv[0] == "stiff" || argv[0] == "stiffness")
      return new ElementResponse(this, 2, *K);
    else if(argv[0] == "defo" || argv[0] == "deformations" ||
                argv[0] == "deformation")
      { return new ElementResponse(this, 3, Vector(order)); }
    else if(argv[0] == "section")
      return setMaterialResponse(theSection,argv,1,eleInformation);
    else
      return 0;
  }

int XC::ZeroLengthSection::getResponse(int responseID, Information &eleInfo)
  {
    switch(responseID)
      {
      case 1:
        return eleInfo.setVector(this->getResistingForce());
      case 2:
        return eleInfo.setMatrix(this->getTangentStiff());
      case 3:
        this->computeSectionDefs();
        return eleInfo.setVector(v);
      default:
        return -1;
      }
  }

// Private methods

// Set basic deformation-displacement transformation matrix for section
void XC::ZeroLengthSection::setTransformation(void)
  {
    inicAv();

    // Get the section code
    const ID &code= theSection->getType();

    // Set a reference to make the syntax nicer
    Matrix &tran= A;
    tran.Zero();

    // Loop over the section code
    for(int i= 0;i<order;i++)
      {
        // Fill in row i of A based on section code
        switch(code(i))
          {
          // The in-plane transformations
          case SECTION_RESPONSE_MZ:
            if(numDOF == 6)
              {
                tran(i,3) = 0.0;
                tran(i,4) = 0.0;
                tran(i,5) = transformation(2,2);
              }
            else if(numDOF == 12)
              {
                tran(i,9) = transformation(2,0);
                tran(i,10) = transformation(2,1);
                tran(i,11) = transformation(2,2);
              }
            break;
          case SECTION_RESPONSE_P:
            if(numDOF == 6)
              {
                tran(i,3) = transformation(0,0);
                tran(i,4) = transformation(0,1);
                tran(i,5) = 0.0;
              }
            else if(numDOF == 12)
              {
                tran(i,6) = transformation(0,0);
                tran(i,7) = transformation(0,1);
                tran(i,8) = transformation(0,2);
              }
            break;
          case SECTION_RESPONSE_VY:
            if(numDOF == 6)
              {
                tran(i,3) = transformation(1,0);
                tran(i,4) = transformation(1,1);
                tran(i,5) = 0.0;
              }
            else if(numDOF == 12)
              {
                tran(i,6) = transformation(1,0);
                tran(i,7) = transformation(1,1);
                tran(i,8) = transformation(1,2);
              }
            break;
          // The out-of-plane transformations
          case SECTION_RESPONSE_MY:
            if(numDOF == 12)
              {
                tran(i,9) = transformation(1,0);
                tran(i,10) = transformation(1,1);
                tran(i,11) = transformation(1,2);
              }
            break;
          case SECTION_RESPONSE_VZ:
            if(numDOF == 12)
              {
                tran(i,6) = transformation(2,0);
                tran(i,7) = transformation(2,1);
                tran(i,8) = transformation(2,2);
              }
            break;
          case SECTION_RESPONSE_T:
            if(numDOF == 12)
              {
                tran(i,9) = transformation(0,0);
                tran(i,10) = transformation(0,1);
                tran(i,11) = transformation(0,2);
              }
            break;
          default:
            break;
          }
        // Fill in first half of transformation matrix with negative sign
        for(int j=0;j<numDOF/2;j++)
          tran(i,j) = -tran(i,j+numDOF/2);
      }
  }

//! @brief Set up the transformation matrix for orientation
//! @param x: vector components in global coordinates defining local x-axis (vector x)
//! @param y: vector components in global coordinates defining vector yp which lies in the local x-y plane for the element. The local z-axis is defined by the cross product between the vectors x and yp
void XC::ZeroLengthSection::setUpVectors(const Vector &x, const Vector &yp)
  {
    Element0D::setUpVectors(x,yp);
    setTransformation(); // Set up the A matrix
  }

void XC::ZeroLengthSection::computeSectionDefs(void) const
  {
    // Get nodal displacements
    const Vector &u1 = theNodes[0]->getTrialDisp();
    const Vector &u2 = theNodes[1]->getTrialDisp();

    // Compute differential displacements
    const Vector diff = u2 - u1;

    // Set some references to make the syntax nicer
    Vector &def= v;
    const Matrix &tran= A;

    def.Zero();

    // Compute element basic deformations ... v = A*(u2-u1)
    for(int i= 0;i<order;i++)
      for(int j= 0;j<numDOF/2;j++)
        def(i)+= -diff(j)*tran(i,j);
  }

