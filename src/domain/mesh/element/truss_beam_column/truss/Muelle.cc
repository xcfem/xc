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
//Muelle.cpp

#include "domain/mesh/element/truss_beam_column/truss/Muelle.h"
#include <domain/mesh/element/Information.h>
#include <utility/recorder/response/ElementResponse.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <material/uniaxial/UniaxialMaterial.h>

#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Pone a cero los punteros a nodo y los cosenos directores.
void XC::Muelle::inicializa(void)
  {
    // set node pointers to nullptr
    theNodes.inic();
    cosX[0]= 0.0;
    cosX[1]= 0.0;
    cosX[2]= 0.0;
  }

//! @brief constructor:
//!  responsible for allocating the necessary space needed by each object
//!  and storing the tags of the Muelle end nodes.
XC::Muelle::Muelle(int tag,int dim,int Nd1, int Nd2, UniaxialMaterial &theMat,double a)
  :ProtoTruss(tag,ELE_TAG_Muelle,Nd1,Nd2,0,dim), theMaterial(nullptr)
  {
    // get a copy of the material and check we obtained a valid copy
    theMaterial= theMat.getCopy();
    if(!theMaterial)
      {
        std::cerr << "FATAL XC::Muelle::Muelle - " << tag <<
          "failed to get a copy of material with tag " << theMat.getTag() << std::endl;
        exit(-1);
      }

    inicializa();
  }

//! @brief constructor:
//!  responsible for allocating the necessary space needed by each object
//!  and storing the tags of the truss end nodes.
XC::Muelle::Muelle(int tag,int dim,const Material *ptr_mat)
  :ProtoTruss(tag,ELE_TAG_Muelle,0,0,0,dim), theMaterial(nullptr)
  {
    theMaterial= cast_material<UniaxialMaterial>(ptr_mat);
    inicializa();
  }


//! constructor:
//!   invoked by a FEM_ObjectBroker - blank object that recvSelf needs
//!   to be invoked upon
XC::Muelle::Muelle(void)
  :ProtoTruss(0,ELE_TAG_Muelle,0,0,0,0),
   theMaterial(nullptr)
  {
    inicializa();
  }

//! @brief Constructor de copia.
XC::Muelle::Muelle(const Muelle &otro)
  :ProtoTruss(otro), theMaterial(nullptr)
  {
    if(otro.theMaterial)
      theMaterial= otro.theMaterial->getCopy();
  }

//! @brief Operador asignación.
XC::Muelle &XC::Muelle::operator=(const Muelle &otro)
  {
    ProtoTruss::operator=(otro);
    if(theMaterial) delete theMaterial;
    theMaterial= nullptr;
    if(otro.theMaterial)
      theMaterial= otro.theMaterial->getCopy();
    return *this;
  }


//! @brief Constructor virtual.
XC::Element* XC::Muelle::getCopy(void) const
  { return new Muelle(*this); }

//! @brief Lee un objeto XC::Muelle desde archivo
bool XC::Muelle::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(Muelle) Procesando comando: " << cmd << std::endl;
    return ProtoTruss::procesa_comando(status);
  }

//!  destructor
//!     delete must be invoked on any objects created by the object
//!     and on the matertial object.
XC::Muelle::~Muelle(void)
  {
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if(theMaterial) delete theMaterial;
  }

//! method: setDomain()
//!    to set a link to the enclosing XC::Domain and to set the node pointers.
//!    also determines the number of dof associated
//!    with the Muelle element, we set matrix and vector pointers,
//!    allocate space for t matrix, determine the length
//!    and set the transformation matrix.
void XC::Muelle::setDomain(Domain *theDomain)
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
            std::cerr <<"WARNING XC::Muelle::setDomain(): nodes " << theNodes[0]->getTag()
                      << " and " <<  theNodes[1]->getTag()
                      << "have differing dof at ends for Muelle " << this->getTag() << std::endl;

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
            std::cerr <<"WARNING XC::Muelle::setDomain cannot handle " << getNumDIM() << " dofs at nodes in " <<
              dofNd1  << " problem\n";

            numDOF= 2;
            theMatrix= &trussM2;
            theVector= &trussV2;
            return;
          }

        if(load.Nulo())
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
                std::cerr <<"WARNING XC::Muelle::setDomain() - Muelle "
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
                std::cerr <<"WARNING XC::Muelle::setDomain() - Muelle "
                          << this->getTag() << " has zero length\n";
                return;
              }
            cosX[0]= dx/L;
            cosX[1]= dy/L;
            cosX[2]= dz/L;
          }
      }
  }

//! @brief Consuma el estado del elemento.
int XC::Muelle::commitState(void)
  {
    int retVal= 0;
    // call element commitState to do any base class stuff
    if((retVal= this->XC::ProtoTruss::commitState()) != 0)
      { std::cerr << "XC::Muelle::commitState () - failed in base class"; }
    retVal= theMaterial->commitState();
    return retVal;
  }

//! @brief Devuelve el estado del elemento al último consumado.
int XC::Muelle::revertToLastCommit(void)
  { return theMaterial->revertToLastCommit(); }

//! @brief Devuelve el estado del elemento al inicial.
int XC::Muelle::revertToStart(void)
  { return theMaterial->revertToStart(); }

//! @brief Calcula la deformación actual a partir de los desplazamientos de prueba en los nodos.
int XC::Muelle::update(void)
  {
    double strain= this->computeCurrentStrain();
    double rate= this->computeCurrentStrainRate();
    return theMaterial->setTrialStrain(strain, rate);
  }

//! @brief Devuelve la matriz de rigidez tangente.
const XC::Matrix &XC::Muelle::getTangentStiff(void) const
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

//! @brief Devuelve la matriz de rigidez inicial.
const XC::Matrix &XC::Muelle::getInitialStiff(void) const
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

//! @brief Devuelve la matriz de amortiguamiento.
const XC::Matrix &XC::Muelle::getDamp(void) const
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

const XC::Material *XC::Muelle::getMaterial(void) const
  { return theMaterial; }
XC::Material *XC::Muelle::getMaterial(void)
  { return theMaterial; }
//! @brief Devuelve la densidad del material.
double XC::Muelle::getRho(void) const
  { return theMaterial->getRho(); }

//! @brief Devuelve la matriz de masas.
const XC::Matrix &XC::Muelle::getMass(void) const
  {
    // zero the matrix
    Matrix &mass= *theMatrix;
    mass.Zero();

    const double M= getRho();//Aquí rho será la masa concentrada.
    // check for XC::quick return
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

//! @brief Añade una carga.
int XC::Muelle::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr <<"XC::Muelle::addLoad - load type unknown for Muelle with tag: " << this->getTag() << std::endl;
    return -1;
  }

//! @brief Añade las fuerzas de inercia.
int XC::Muelle::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    const double M= getRho(); //Aqui rho es la masa concentrada.
    // check for a quick return
    if(M==0.0) return 0;

    // get R * accel from the nodes
    const XC::Vector &Raccel1= theNodes[0]->getRV(accel);
    const XC::Vector &Raccel2= theNodes[1]->getRV(accel);

    int nodalDOF= numDOF/2;

#ifdef _G3DEBUG
    if(nodalDOF != Raccel1.Size() || nodalDOF != Raccel2.Size())
      {
        std::cerr <<"XC::Muelle::addInertiaLoadToUnbalance " <<
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

//! @brief Devuelve la reacción del elemento.
const XC::Vector &XC::Muelle::getResistingForce(void) const
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

//! @brief Devuelve la reacción del elemento incluyendo fuerzas de inercia.
const XC::Vector &XC::Muelle::getResistingForceIncInertia(void) const
  {
    this->getResistingForce();

    const double M= getRho(); //Aqui rho es la masa concentrada.
    // now include the mass portion
    if(M!=0.0)
      {

        const XC::Vector &accel1= theNodes[0]->getTrialAccel();
        const XC::Vector &accel2= theNodes[1]->getTrialAccel();

        int numDOF2= numDOF/2;
        for(int i= 0;i<getNumDIM();i++)
          {
            (*theVector)(i) += M*accel1(i);
            (*theVector)(i+numDOF2) += M*accel2(i);
          }

        // add the damping forces if rayleigh damping
        if(!rayFactors.Nulos())
          (*theVector) += this->getRayleighDampingForces();
      }
    else
      {
        // add the damping forces if rayleigh damping
        if(!rayFactors.KNulos())
          (*theVector) += this->getRayleighDampingForces();
      }
    if(isDead())
      (*theVector)*=dead_srf;
    return *theVector;
  }


void XC::Muelle::Print(std::ostream &s, int flag)
  {
    // compute the strain and axial force in the member
    const double strain= theMaterial->getStrain();
    const double force= theMaterial->getStress();

    if(flag == 0)
      { // print everything
        s << "Element: " << this->getTag();
        s << " type: Muelle  iNode: " << theNodes.getTagNode(0);
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

double XC::Muelle::getAxil(void) const
  { return theMaterial->getStress(); }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
//! strain: Devuelve la deformación del elemento.
any_const_ptr XC::Muelle::GetProp(const std::string &cod) const
  {
    if(cod=="getStrain")
      {
        tmp_gp_dbl= theMaterial->getStrain();
        return any_const_ptr(tmp_gp_dbl);
      }
    if(cod=="getAxil")
      {
        tmp_gp_dbl= getAxil();
        return any_const_ptr(tmp_gp_dbl);
      }
    else
      return ProtoTruss::GetProp(cod);
  }

double XC::Muelle::computeCurrentStrain(void) const
  {
    // determine the strain
    const Vector &disp1= theNodes[0]->getTrialDisp();
    const Vector &disp2= theNodes[1]->getTrialDisp();

    double dLength= 0.0;
    for(int i= 0;i<getNumDIM();i++)
      dLength+= (disp2(i)-disp1(i))*cosX[i];
    return dLength;
  }

double XC::Muelle::computeCurrentStrainRate(void) const
  {
    // determine the strain
    const XC::Vector &vel1= theNodes[0]->getTrialVel();
    const XC::Vector &vel2= theNodes[1]->getTrialVel();

    double dLength= 0.0;
    for(int i= 0; i < getNumDIM();i++)
      dLength += (vel2(i)-vel1(i))*cosX[i];
    return dLength;
  }

XC::Response *XC::Muelle::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    //
    // we compare argv[0] for known response types for the XC::Muelle
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

int XC::Muelle::getResponse(int responseID, Information &eleInfo)
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

//! @brief Envía los miembros por el canal que se pasa como parámetro.
int XC::Muelle::sendData(CommParameters &cp)
  {
    int res= ProtoTruss::sendData(cp);
    res+= cp.sendBrokedPtr(theMaterial,getDbTagData(),BrokedPtrCommMetaData(16,17,18));
    res+= cp.sendDoubles(cosX[0],cosX[1],cosX[2],getDbTagData(),CommMetaData(19));
    return res;
  }

//! @brief Recibe los miembros por el canal que se pasa como parámetro.
int XC::Muelle::recvData(const CommParameters &cp)
  {
    int res= ProtoTruss::recvData(cp);
    theMaterial= cp.getBrokedMaterial(theMaterial,getDbTagData(),BrokedPtrCommMetaData(16,17,18));
    res+= cp.receiveDoubles(cosX[0],cosX[1],cosX[2],getDbTagData(),CommMetaData(19));
    return res;
  }

int XC::Muelle::sendSelf(CommParameters &cp)
  {
    inicComm(21);
    int res= sendData(cp);

    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "Muelle::sendSelf -- failed to send ID data\n";
    return res;
  }

int XC::Muelle::recvSelf(const CommParameters &cp)
  {
    inicComm(21);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Muelle::recvSelf() - failed to recv ID data";
    else
      res+= recvData(cp);
    return res;
  }
