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
//Spring.cpp

#include "domain/mesh/element/truss_beam_column/truss/Spring.h"
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/ElementResponse.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <material/uniaxial/UniaxialMaterial.h>


#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Pone a cero los pointers to node y los cosenos directores.
void XC::Spring::inicializa(void)
  {
    // set node pointers to nullptr
    theNodes.inic();
    cosX[0]= 0.0;
    cosX[1]= 0.0;
    cosX[2]= 0.0;
  }

//! @brief constructor:
//!  responsible for allocating the necessary space needed by each object
//!  and storing the tags of the Spring end nodes.
XC::Spring::Spring(int tag,int dim,int Nd1, int Nd2, UniaxialMaterial &theMat,double a)
  :ProtoTruss(tag,ELE_TAG_Spring,Nd1,Nd2,0,dim), theMaterial(nullptr)
  {
    // get a copy of the material and check we obtained a valid copy
    theMaterial= theMat.getCopy();
    if(!theMaterial)
      {
        std::cerr << "FATAL XC::Spring::Spring - " << tag <<
          "failed to get a copy of material with tag " << theMat.getTag() << std::endl;
        exit(-1);
      }

    inicializa();
  }

//! @brief constructor:
//!  responsible for allocating the necessary space needed by each object
//!  and storing the tags of the truss end nodes.
XC::Spring::Spring(int tag,int dim,const Material *ptr_mat)
  :ProtoTruss(tag,ELE_TAG_Spring,0,0,0,dim), theMaterial(nullptr)
  {
    theMaterial= cast_material<UniaxialMaterial>(ptr_mat);
    inicializa();
  }


//! constructor:
//!   invoked by a FEM_ObjectBroker - blank object that recvSelf needs
//!   to be invoked upon
XC::Spring::Spring(void)
  :ProtoTruss(0,ELE_TAG_Spring,0,0,0,0),
   theMaterial(nullptr)
  {
    inicializa();
  }

//! @brief Copy constructor.
XC::Spring::Spring(const Spring &other)
  :ProtoTruss(other), theMaterial(nullptr)
  {
    if(other.theMaterial)
      theMaterial= other.theMaterial->getCopy();
  }

//! @brief Assignment operator.
XC::Spring &XC::Spring::operator=(const Spring &other)
  {
    ProtoTruss::operator=(other);
    if(theMaterial) delete theMaterial;
    theMaterial= nullptr;
    if(other.theMaterial)
      theMaterial= other.theMaterial->getCopy();
    return *this;
  }


//! @brief Virtual constructor.
XC::Element* XC::Spring::getCopy(void) const
  { return new Spring(*this); }

//!  destructor
//!     delete must be invoked on any objects created by the object
//!     and on the matertial object.
XC::Spring::~Spring(void)
  {
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if(theMaterial) delete theMaterial;
  }

//! method: setDomain()
//!    to set a link to the enclosing XC::Domain and to set the node pointers.
//!    also determines the number of dof associated
//!    with the Spring element, we set matrix and vector pointers,
//!    allocate space for t matrix, determine the length
//!    and set the transformation matrix.
void XC::Spring::setDomain(Domain *theDomain)
  {
    // check XC::Domain is not null - invoked when object removed from a domain
    ProtoTruss::setDomain(theDomain);
    if(theDomain)
      {
        // if can't find both nodes - send a warning message
        if((theNodes[0] == 0) || (theNodes[1] == 0))
          {
            // fill this in so don't segment fault later
            numDOF= 2;
            theMatrix= &trussM2;
            theVector= &trussV2;
            return;
          }

        // now determine the number of dof and the dimesnion
        int dofNd1= theNodes[0]->getNumberDOF();
        int dofNd2= theNodes[1]->getNumberDOF();

        // if differing dof at the ends - print a warning message
        if(dofNd1 != dofNd2)
          {
            std::cerr <<"WARNING XC::Spring::setDomain(): nodes " << theNodes[0]->getTag()
                      << " and " <<  theNodes[1]->getTag()
                      << "have differing dof at ends for Spring " << this->getTag() << std::endl;

            // fill this in so don't segment fault later
            numDOF= 2;
            theMatrix= &trussM2;
            theVector= &trussV2;
            return;
          }

        // now set the number of dof for element and set matrix and vector pointer
        if(getNumDIM() == 1 && dofNd1 == 1)
          {
            numDOF= 2;
            theMatrix= &trussM2;
            theVector= &trussV2;
          }
        else if(getNumDIM() == 2 && dofNd1 == 2)
          {
            numDOF= 4;
            theMatrix= &trussM4;
            theVector= &trussV4;
          }
        else if(getNumDIM() == 2 && dofNd1 == 3)
          {
            numDOF= 6;
            theMatrix= &trussM6;
            theVector= &trussV6;
          }
        else if(getNumDIM() == 3 && dofNd1 == 3)
          {
            numDOF= 6;
            theMatrix= &trussM6;
            theVector= &trussV6;
          }
        else if(getNumDIM() == 3 && dofNd1 == 6)
          {
            numDOF= 12;
            theMatrix= &trussM12;
            theVector= &trussV12;
          }
        else
          {
            std::cerr <<"WARNING XC::Spring::setDomain cannot handle " << getNumDIM() << " dofs at nodes in " <<
              dofNd1  << " problem\n";

            numDOF= 2;
            theMatrix= &trussM2;
            theVector= &trussV2;
            return;
          }

        if(load.isEmpty())
          load.reset(numDOF);
        else
          if(load.Size() != numDOF)
            load= Vector(numDOF);

        // now determine the length, cosines and fill in the transformation
        // NOTE t= -t(every one else uses for residual calc)
        const XC::Vector &end1Crd= theNodes[0]->getCrds();
        const XC::Vector &end2Crd= theNodes[1]->getCrds();

        if(getNumDIM() == 1)
          {
            cosX[0]= 1.0;
          }
        else if(getNumDIM() == 2)
          {
            const double dx= end2Crd(0)-end1Crd(0);
            const double dy= end2Crd(1)-end1Crd(1);
            const double L= sqrt(dx*dx + dy*dy);

            if(L == 0.0)
              {
                std::cerr <<"WARNING XC::Spring::setDomain() - Spring "
                          << this->getTag() << " has zero length\n";
                return;
              }
            cosX[0]= dx/L;
            cosX[1]= dy/L;
          }
        else
          {
            const double dx= end2Crd(0)-end1Crd(0);
            const double dy= end2Crd(1)-end1Crd(1);
            const double dz= end2Crd(2)-end1Crd(2);
            const double L= sqrt(dx*dx + dy*dy + dz*dz);

            if(L == 0.0)
              {
                std::cerr <<"WARNING XC::Spring::setDomain() - Spring "
                          << this->getTag() << " has zero length\n";
                return;
              }
            cosX[0]= dx/L;
            cosX[1]= dy/L;
            cosX[2]= dz/L;
          }
      }
  }

//! @brief Commits its state.
int XC::Spring::commitState(void)
  {
    int retVal= 0;
    // call element commitState to do any base class stuff
    if((retVal= this->XC::ProtoTruss::commitState()) != 0)
      { std::cerr << "XC::Spring::commitState () - failed in base class"; }
    retVal= theMaterial->commitState();
    return retVal;
  }

//! @brief Returns to the last committed state.
int XC::Spring::revertToLastCommit(void)
  { return theMaterial->revertToLastCommit(); }

//! @brief Returns to the initial state.
int XC::Spring::revertToStart(void)
  { return theMaterial->revertToStart(); }

//! @brief Computes current trial strain from trial displacements at the nodes.
int XC::Spring::update(void)
  {
    double strain= this->computeCurrentStrain();
    double rate= this->computeCurrentStrainRate();
    return theMaterial->setTrialStrain(strain, rate);
  }

//! @brief Returns the tangent stiffness matrix.
const XC::Matrix &XC::Spring::getTangentStiff(void) const
  {
    const double K= theMaterial->getTangent();

    // come back later and redo this if too slow
    Matrix &stiff= *theMatrix;

    const int numDOF2= numDOF/2;
    double temp;
    for(register int i= 0;i<getNumDIM();i++)
      {
        for(register int j= 0;j<getNumDIM();j++)
          {
            temp= cosX[i]*cosX[j]*K;
            stiff(i,j)= temp;
            stiff(i+numDOF2,j)= -temp;
            stiff(i,j+numDOF2)= -temp;
            stiff(i+numDOF2,j+numDOF2)= temp;
          }
      }
    if(isDead())
      (*theMatrix)*=dead_srf;
    return stiff;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::Spring::getInitialStiff(void) const
  {
    const double K= theMaterial->getInitialTangent();

    // come back later and redo this if too slow
    Matrix &stiff= *theMatrix;

    const int numDOF2= numDOF/2;
    double temp;
    for(register int i=0;i<getNumDIM();i++)
      {
        for(register int j=0;j<getNumDIM();j++)
          {
            temp= cosX[i]*cosX[j]*K;
            stiff(i,j)= temp;
            stiff(i+numDOF2,j)= -temp;
            stiff(i,j+numDOF2)= -temp;
            stiff(i+numDOF2,j+numDOF2)= temp;
          }
      }
    if(isDead())
      (*theMatrix)*=dead_srf;
    return *theMatrix;
  }

//! @brief Returns the matrix de amortiguamiento.
const XC::Matrix &XC::Spring::getDamp(void) const
  {
    const double eta= theMaterial->getDampTangent();

    // come back later and redo this if too slow
    Matrix &damp= *theMatrix;

    const int numDOF2= numDOF/2;
    double temp;
    const int nd= getNumDIM();
    for(register int i= 0;i<nd;i++)
      {
        for(register int j= 0;j<nd;j++)
          {
            temp= cosX[i]*cosX[j]*eta;
            damp(i,j)= temp;
            damp(i+numDOF2,j)= -temp;
            damp(i,j+numDOF2)= -temp;
            damp(i+numDOF2,j+numDOF2)= temp;
          }
      }
    if(isDead())
      damp*=dead_srf;
    return damp;
  }

const XC::Material *XC::Spring::getMaterial(void) const
  { return theMaterial; }
XC::Material *XC::Spring::getMaterial(void)
  { return theMaterial; }
//! @brief Returns the material density.
double XC::Spring::getRho(void) const
  { return theMaterial->getRho(); }

//! @brief Returns the mass matrix.
const XC::Matrix &XC::Spring::getMass(void) const
  {
    // zero the matrix
    Matrix &mass= *theMatrix;
    mass.Zero();

    const double M= getRho();//Here rho is the concentrated mass.
    // check for quick return
    if(M == 0.0) return mass;

    int numDOF2= numDOF/2;
    for(register int i= 0;i< getNumDIM();i++)
      {
        mass(i,i)= M;
        mass(i+numDOF2,i+numDOF2)= M;
      }
    if(isDead())
      mass*=dead_srf;
    return mass;
  }

//! @brief Adds a load.
int XC::Spring::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr <<"XC::Spring::addLoad - load type unknown for Spring with tag: " << this->getTag() << std::endl;
    return -1;
  }

//! @brief Adds inertia forces.
int XC::Spring::addInertiaLoadToUnbalance(const Vector &accel)
  {
    const double M= getRho(); //Here rho is the concentrated mass.
    // check for a quick return
    if(M==0.0) return 0;

    // get R * accel from the nodes
    const XC::Vector &Raccel1= theNodes[0]->getRV(accel);
    const XC::Vector &Raccel2= theNodes[1]->getRV(accel);

    int nodalDOF= numDOF/2;

#ifdef _G3DEBUG
    if(nodalDOF != Raccel1.Size() || nodalDOF != Raccel2.Size())
      {
        std::cerr <<"XC::Spring::addInertiaLoadToUnbalance " <<
          "matrix and vector sizes are incompatable\n";
        return -1;
      }
#endif

    // want to add ( - fact * M R * accel ) to unbalance
    for(int i=0;i<getNumDIM(); i++)
      {
        double val1= Raccel1(i);
        double val2= Raccel2(i);
        // perform - fact * M*(R * accel) // remember M a diagonal matrix
        val1 *= -M;
        val2 *= -M;
        load(i) += val1;
        load(i+nodalDOF) += val2;
      }
    return 0;
  }

//! @brief Returns the reaction of the element.
const XC::Vector &XC::Spring::getResistingForce(void) const
  {
    // R= Ku - Pext
    // Ku= F * transformation
    double force= theMaterial->getStress();
    int numDOF2= numDOF/2;
    double temp;
    for(int i= 0;i<getNumDIM();i++)
      {
        temp= cosX[i]*force;
        (*theVector)(i)= -temp;
        (*theVector)(i+numDOF2)= temp;
      }

    // subtract external load:  Ku - P
    (*theVector)-= load;
    if(isDead())
      (*theVector)*=dead_srf;
    return *theVector;
  }

//! @brief Returns the reaction of the element including inertia forces.
const XC::Vector &XC::Spring::getResistingForceIncInertia(void) const
  {
    this->getResistingForce();

    const double M= getRho(); // concentrated mass.
    // now include the mass portion
    if(M!=0.0)
      {

        const Vector &accel1= theNodes[0]->getTrialAccel();
        const Vector &accel2= theNodes[1]->getTrialAccel();

        const int numDOF2= numDOF/2;
        for(int i= 0;i<getNumDIM();i++)
          {
            (*theVector)(i) += M*accel1(i);
            (*theVector)(i+numDOF2) += M*accel2(i);
          }

        // add the damping forces if rayleigh damping
        if(!rayFactors.nullValues())
          (*theVector)+= this->getRayleighDampingForces();
      }
    else
      {
        // add the damping forces if rayleigh damping
        if(!rayFactors.nullKValues())
          (*theVector)+= this->getRayleighDampingForces();
      }
    if(isDead())
      (*theVector)*=dead_srf;
    return *theVector;
  }

//! @brief Print spring data.
void XC::Spring::Print(std::ostream &s, int flag)
  {
    // compute the strain and axial force in the member
    const double strain= theMaterial->getStrain();
    const double force= theMaterial->getStress();

    if(flag == 0)
      { // print everything
        s << "Element: " << this->getTag();
        s << " type: Spring  iNode: " << theNodes.getTagNode(0);
        s << " jNode: " << theNodes.getTagNode(1);

        s << " \n\t strain: " << strain;
        s << " axial load: " <<  force;
        const int numDOF2= numDOF/2;
        double temp;
        for(int i= 0; i < getNumDIM(); i++)
          {
            temp= cosX[i]*force;
            (*theVector)(i)= -temp;
            (*theVector)(i+numDOF2)= temp;
          }
        s << " \n\t unbalanced load: " << *theVector;
        s << " \t XC::Material: " << *theMaterial;
        s << std::endl;
      }
    else
     if(flag == 1)
       {
         s << this->getTag() << "  " << strain << "  ";
         s << force << std::endl;
       }
  }

double XC::Spring::getAxialForce(void) const
  { return theMaterial->getStress(); }

double XC::Spring::computeCurrentStrain(void) const
  {
    // determine the strain
    const Vector &disp1= theNodes[0]->getTrialDisp();
    const Vector &disp2= theNodes[1]->getTrialDisp();

    double dLength= 0.0;
    for(int i= 0;i<getNumDIM();i++)
      dLength+= (disp2(i)-disp1(i))*cosX[i];
    return dLength;
  }

double XC::Spring::computeCurrentStrainRate(void) const
  {
    // determine the strain
    const XC::Vector &vel1= theNodes[0]->getTrialVel();
    const XC::Vector &vel2= theNodes[1]->getTrialVel();

    double dLength= 0.0;
    for(int i= 0; i < getNumDIM();i++)
      dLength += (vel2(i)-vel1(i))*cosX[i];
    return dLength;
  }

XC::Response *XC::Spring::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    //
    // we compare argv[0] for known response types for the XC::Spring
    //

    if(argv[0] == "force" || argv[0] == "forces" || argv[0] == "axialForce")
      return new ElementResponse(this, 1, 0.0);

    else if(argv[0] == "defo" || argv[0] == "deformations" ||
           argv[0] == "deformation")
      return new ElementResponse(this, 2, 0.0);
    // tangent stiffness matrix
    else if(argv[0] == "stiff")
      return new ElementResponse(this, 3, *theMatrix);
    // a material quantity
    else if(argv[0] == "material" || argv[0] == "-material")
      return  setMaterialResponse(theMaterial,argv,1,eleInfo);
    else
      return 0;
  }

int XC::Spring::getResponse(int responseID, Information &eleInfo)
  {
    switch (responseID)
      {
      case 1:
        return eleInfo.setDouble(theMaterial->getStress());
      case 2:
        return eleInfo.setDouble(theMaterial->getStrain());
      case 3:
        return eleInfo.setMatrix(this->getTangentStiff());
      default:
        return 0;
      }
  }

//! @brief Send members through the channel being passed as parameter.
int XC::Spring::sendData(CommParameters &cp)
  {
    int res= ProtoTruss::sendData(cp);
    res+= cp.sendBrokedPtr(theMaterial,getDbTagData(),BrokedPtrCommMetaData(16,17,18));
    res+= cp.sendDoubles(cosX[0],cosX[1],cosX[2],getDbTagData(),CommMetaData(19));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::Spring::recvData(const CommParameters &cp)
  {
    int res= ProtoTruss::recvData(cp);
    theMaterial= cp.getBrokedMaterial(theMaterial,getDbTagData(),BrokedPtrCommMetaData(16,17,18));
    res+= cp.receiveDoubles(cosX[0],cosX[1],cosX[2],getDbTagData(),CommMetaData(19));
    return res;
  }

int XC::Spring::sendSelf(CommParameters &cp)
  {
    inicComm(21);
    int res= sendData(cp);

    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "Spring::sendSelf -- failed to send ID data\n";
    return res;
  }

int XC::Spring::recvSelf(const CommParameters &cp)
  {
    inicComm(21);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Spring::recvSelf() - failed to recv ID data";
    else
      res+= recvData(cp);
    return res;
  }
