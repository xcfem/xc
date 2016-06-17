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
// $Revision: 1.19 $
// $Date: 2004/06/15 18:58:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/FluidSolidPorousMaterial.cpp,v $
                                                                        
// Written: ZHY

//
// FluidSolidPorousMaterial.cpp
// -------------------
//
#include <cmath>
#include <cstdlib>
#include <material/nD/soil/FluidSolidPorousMaterial.h>
#include <domain/mesh/element/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <utility/matrix/ID.h>

#include <material/nD/soil/MultiYieldSurface.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/nDarray/Tensor.h>

#include "material/nD/TipoMaterialND.h"

int* XC::FluidSolidPorousMaterial::loadStagex = 0;
int* XC::FluidSolidPorousMaterial::ndmx = 0;
double* XC::FluidSolidPorousMaterial::combinedBulkModulusx = 0;
int XC::FluidSolidPorousMaterial::matCount = 0;
double XC::FluidSolidPorousMaterial::pAtm = 101;

XC::Vector XC::FluidSolidPorousMaterial::workV3(3);
XC::Vector XC::FluidSolidPorousMaterial::workV6(6);
XC::Matrix XC::FluidSolidPorousMaterial::workM3(3,3);
XC::Matrix XC::FluidSolidPorousMaterial::workM6(6,6);

void XC::FluidSolidPorousMaterial::libera(void)
  {
    if(theSoilMaterial) delete theSoilMaterial;
    theSoilMaterial= nullptr;
  }

void XC::FluidSolidPorousMaterial::alloc(const NDMaterial *s)
  {
    libera();
    if(s)
      theSoilMaterial= s->getCopy();
  }

XC::FluidSolidPorousMaterial::FluidSolidPorousMaterial(int tag, int nd,const NDMaterial &soilMat,double combinedBulkModul, double atm)
  :NDMaterial(tag, ND_TAG_FluidSolidPorousMaterial), theSoilMaterial(nullptr)
  {
    if(combinedBulkModul < 0)
      {
        std::cerr << "WARNING: FluidSolidPorousMaterial::FluidSolidPorousMaterial: "
	          << " combinedBulkModulus < 0" << std::endl
                  << "Will reset to 0." <<std::endl;
        combinedBulkModul= 0.;
      }

    if(matCount%20 == 0)
      {
        int *temp1= loadStagex;
        int *temp2= ndmx;
        double *temp3= combinedBulkModulusx;
        loadStagex = new int[matCount+20];
        ndmx= new int[matCount+20];
        combinedBulkModulusx= new double[matCount+20];
        for(int i=0; i<matCount; i++)
          {
             loadStagex[i] = temp1[i];
             ndmx[i] = temp2[i];
             combinedBulkModulusx[i] = temp3[i];
          }
        if(matCount > 0)
          { delete [] temp1; delete [] temp2; delete [] temp3; }
      }

    ndmx[matCount] = nd;
    loadStagex[matCount] = 0;  //default
    combinedBulkModulusx[matCount] = combinedBulkModul;
    matN = matCount;
    matCount ++;
    pAtm = atm;

    alloc(&soilMat);
    trialExcessPressure = currentExcessPressure = 0.;
    trialVolumeStrain = currentVolumeStrain = 0.;
    initMaxPress = 0.;
    e2p = 0;
  }
   
XC::FluidSolidPorousMaterial::FluidSolidPorousMaterial(int tag) 
 : NDMaterial(tag,ND_TAG_FluidSolidPorousMaterial), theSoilMaterial(nullptr)
  {
    trialExcessPressure = currentExcessPressure = 0.;
    trialVolumeStrain = currentVolumeStrain = 0.;
    initMaxPress = 0.;
    e2p = 0;
  }

//! @brief Constructor de copia.
XC::FluidSolidPorousMaterial::FluidSolidPorousMaterial(const FluidSolidPorousMaterial &a)
 : NDMaterial(a), theSoilMaterial(nullptr)
  {
    matN = a.matN;
    alloc(a.theSoilMaterial);
    trialExcessPressure = a.trialExcessPressure;
    currentExcessPressure = a.currentExcessPressure;
    trialVolumeStrain = a.trialVolumeStrain;
    currentVolumeStrain = a.currentVolumeStrain;
    initMaxPress = a.initMaxPress;
    e2p = a.e2p;
  }

//! @brief Operador asignación.
XC::FluidSolidPorousMaterial &XC::FluidSolidPorousMaterial::operator=(const FluidSolidPorousMaterial &a)
  {
    NDMaterial::operator=(a);
    matN = a.matN;
    alloc(a.theSoilMaterial);
    trialExcessPressure = a.trialExcessPressure;
    currentExcessPressure = a.currentExcessPressure;
    trialVolumeStrain = a.trialVolumeStrain;
    currentVolumeStrain = a.currentVolumeStrain;
    initMaxPress = a.initMaxPress;
    e2p = a.e2p;
    return *this;
  }

//! @brief Destructor.
XC::FluidSolidPorousMaterial::~FluidSolidPorousMaterial(void)
  { libera(); }


int XC::FluidSolidPorousMaterial::setTrialStrain(const Vector &strain)
{
    int ndm = ndmx[matN];

    if(ndm==2 && strain.Size()==3)
            trialVolumeStrain = strain[0]+strain[1];
    else if(ndm==3 && strain.Size()==6)
            trialVolumeStrain = strain[0]+strain[1]+strain[2];
    else {
            std::cerr << "Fatal:XC::FluidSolidPorousMaterial:: Material dimension is: " << ndm << std::endl;
            std::cerr << "But strain vector size is: " << strain.Size() << std::endl;
            exit(-1);
    }

  return theSoilMaterial->setTrialStrain(strain);
}


int XC::FluidSolidPorousMaterial::setTrialStrain(const XC::Vector &strain, const XC::Vector &rate)
{
    int ndm = ndmx[matN];

    if(ndm==2 && strain.Size()==3)
            trialVolumeStrain = strain[0]+strain[1];
    else if(ndm==3 && strain.Size()==6)
            trialVolumeStrain = strain[0]+strain[1]+strain[2];
    else {
            std::cerr << "Fatal:XC::FluidSolidPorousMaterial:: Material dimension is: " << ndm << std::endl;
            std::cerr << "But strain vector size is: " << strain.Size() << std::endl;
            exit(-1);
    }

  return theSoilMaterial->setTrialStrain(strain, rate);
}


int XC::FluidSolidPorousMaterial::setTrialStrainIncr(const XC::Vector &strain)
{
    int ndm = ndmx[matN];

    if(ndm==2 && strain.Size()==3)
            trialVolumeStrain = currentVolumeStrain + strain[0]+strain[1];
    else if(ndm==3 && strain.Size()==6)
            trialVolumeStrain = currentVolumeStrain + strain[0]+strain[1]+strain[2];
    else {
            std::cerr << "Fatal:XC::FluidSolidPorousMaterial:: Material dimension is: " << ndm << std::endl;
            std::cerr << "But strain vector size is: " << strain.Size() << std::endl;
            exit(-1);
    }

  return theSoilMaterial->setTrialStrainIncr(strain);
}


int XC::FluidSolidPorousMaterial::setTrialStrainIncr(const XC::Vector &strain, const XC::Vector &rate)
{
    int ndm = ndmx[matN];

    if(ndm==2 && strain.Size()==3)
            trialVolumeStrain = currentVolumeStrain + strain[0]+strain[1];
    else if(ndm==3 && strain.Size()==6)
            trialVolumeStrain = currentVolumeStrain + strain[0]+strain[1]+strain[2];
    else {
            std::cerr << "Fatal:XC::FluidSolidPorousMaterial:: Material dimension is: " << ndm << std::endl;
            std::cerr << "But strain vector size is: " << strain.Size() << std::endl;
            exit(-1);
    }

  return theSoilMaterial->setTrialStrainIncr(strain, rate);
}


const XC::Matrix &XC::FluidSolidPorousMaterial::getTangent(void) const
{
    int ndm = ndmx[matN];
    int loadStage = loadStagex[matN];
    double combinedBulkModulus = combinedBulkModulusx[matN];

    XC::Matrix *workM = (ndm == 2) ? &workM3 : &workM6;
  
    *workM = theSoilMaterial->getTangent();

    if(loadStage != 0) { 
      for(int i=0; i<ndm; i++) 
              for(int j=0; j<ndm; j++) 
                      (*workM)(i,j) = (*workM)(i,j) + combinedBulkModulus;
  }

    return *workM;
}

const XC::Matrix &XC::FluidSolidPorousMaterial::getInitialTangent(void) const
  {
    int ndm = ndmx[matN];

    XC::Matrix *workM = (ndm == 2) ? &workM3 : &workM6;
  
    *workM = theSoilMaterial->getInitialTangent();

    return *workM;
  }

double XC::FluidSolidPorousMaterial::getRho(void) const
  { return theSoilMaterial->getRho(); }

const XC::Vector & XC::FluidSolidPorousMaterial::getStress(void) const
{
    int ndm = ndmx[matN];
    int loadStage = loadStagex[matN];
    double combinedBulkModulus = combinedBulkModulusx[matN];

    XC::Vector *workV = (ndm == 2) ? &workV3 : &workV6;

    *workV = theSoilMaterial->getStress();

    if(loadStage != 0) { 
            if(e2p==0) {
                    e2p = 1;
                    initMaxPress = ((*workV)[0] < (*workV)[1]) ? (*workV)[0] : (*workV)[1]; 
                    if(ndm == 3)
                            initMaxPress = (initMaxPress < (*workV)[2]) ? initMaxPress : (*workV)[2];
            }
            trialExcessPressure = currentExcessPressure;
        trialExcessPressure += 
                           (trialVolumeStrain - currentVolumeStrain) * combinedBulkModulus;
            if(trialExcessPressure > pAtm-initMaxPress) 
                    trialExcessPressure = pAtm-initMaxPress;
            //if(trialExcessPressure < initMaxPress)
            //        trialExcessPressure = initMaxPress;
      for(int i=0; i<ndm; i++) 
      (*workV)[i] += trialExcessPressure;
    }

  return *workV;
}


int XC::FluidSolidPorousMaterial::updateParameter(int responseID, Information &info)
{
    if(responseID<10)
            loadStagex[matN] = responseID;
    else {
            if(responseID==11) combinedBulkModulusx[matN]=info.theDouble;
    }

  return 0;
}


const XC::Vector & XC::FluidSolidPorousMaterial::getCommittedStress(void)
{
    return theSoilMaterial->getCommittedStress();
}


const XC::Vector & XC::FluidSolidPorousMaterial::getCommittedStrain(void)
{
    return theSoilMaterial->getCommittedStrain();
}


const XC::Vector & XC::FluidSolidPorousMaterial::getCommittedPressure(void)
  {
    //int ndm = ndmx[matN];
    static XC::Vector temp(2);
    temp[0] = currentExcessPressure;
    temp[1] = temp[0]/initMaxPress;
    return temp;
  }


const XC::Vector & XC::FluidSolidPorousMaterial::getStrain(void) const
  { return theSoilMaterial->getStrain(); }


int XC::FluidSolidPorousMaterial::commitState(void)
{
    int loadStage = loadStagex[matN];

    currentVolumeStrain = trialVolumeStrain;
    if(loadStage != 0) 
            currentExcessPressure = trialExcessPressure;
    else
        currentExcessPressure = 0.;

    return theSoilMaterial->commitState();
}


int XC::FluidSolidPorousMaterial::revertToLastCommit(void)
{
    return theSoilMaterial->revertToLastCommit();
}

int XC::FluidSolidPorousMaterial::revertToStart(void)
{
    return theSoilMaterial->revertToStart();
}

XC::NDMaterial * XC::FluidSolidPorousMaterial::getCopy(void) const
  { return new FluidSolidPorousMaterial(*this); }


XC::NDMaterial * XC::FluidSolidPorousMaterial::getCopy(const std::string &code) const
  {
    NDMaterial *retval= nullptr;
    if((code==strTipoFluidSolidPorous) || (code==strTipoPlaneStrain) ||
            (code==strTipoThreeDimensional))
      retval= getCopy();
    return retval;
  }


const std::string &XC::FluidSolidPorousMaterial::getType(void) const
  {
    int ndm = ndmx[matN];
    return (ndm == 2) ? strTipoPlaneStrain : strTipoThreeDimensional;
  }


int XC::FluidSolidPorousMaterial::getOrder(void) const
{
    int ndm = ndmx[matN];

    return (ndm == 2) ? 3 : 6;
}


//! @brief Send members del objeto through the channel being passed as parameter.
int XC::FluidSolidPorousMaterial::sendData(CommParameters &cp)
  {
    int res= NDMaterial::sendData(cp);
    res+= cp.sendDoubles(trialExcessPressure,currentExcessPressure,trialVolumeStrain,getDbTagData(),CommMetaData(1));
    res+= cp.sendDoubles(currentVolumeStrain,initMaxPress,getDbTagData(),CommMetaData(2));
    res+= cp.sendInts(matN,e2p,getDbTagData(),CommMetaData(3));
    res+= cp.sendBrokedPtr(theSoilMaterial,getDbTagData(),BrokedPtrCommMetaData(4,5,6));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::FluidSolidPorousMaterial::recvData(const CommParameters &cp)
  {
    int res= NDMaterial::recvData(cp);
    res+= cp.receiveDoubles(trialExcessPressure,currentExcessPressure,trialVolumeStrain,getDbTagData(),CommMetaData(1));
    res+= cp.receiveDoubles(currentVolumeStrain,initMaxPress,getDbTagData(),CommMetaData(2));
    res+= cp.receiveInts(matN,e2p,getDbTagData(),CommMetaData(3));
    theSoilMaterial= cp.getBrokedMaterial(theSoilMaterial,getDbTagData(),BrokedPtrCommMetaData(4,5,6));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::FluidSolidPorousMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::FluidSolidPorousMaterial::recvSelf(const CommParameters &cp)    
  {
    inicComm(7);
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


XC::Response *XC::FluidSolidPorousMaterial::setResponse(const std::vector<std::string> &argv, Information &matInfo)
  {
    if(argv[0] == "stress" || argv[0] == "stresses")
      return new XC::MaterialResponse(this, 1, this->getCommittedStress());
    else if(argv[0] == "strain" || argv[0] == "strains")
      return new XC::MaterialResponse(this, 2, this->getCommittedStrain());
    else if(argv[0] == "tangent")
      return new XC::MaterialResponse(this, 3, this->getTangent());
    else if(argv[0] == "backbone")
      return theSoilMaterial->setResponse(argv, matInfo);
    else if(argv[0] == "pressure")
      return new MaterialResponse(this, 5, this->getCommittedPressure());
    else
      return 0;
  }


int XC::FluidSolidPorousMaterial::getResponse(int responseID, Information &matInfo)
{
    switch(responseID) {
            case 1:
                    return matInfo.setVector(this->getCommittedStress());

            case 2:
                    return matInfo.setVector(this->getCommittedStrain());

            case 3:
                    return matInfo.setMatrix(this->getTangent());
                    
            case 4:
                    return theSoilMaterial->getResponse(responseID, matInfo);

            case 5:
                    return matInfo.setVector(this->getCommittedPressure());

            default:
                    return -1;
    }
}


void XC::FluidSolidPorousMaterial::Print(std::ostream &s, int flag )
  { s << "FluidSolidPorousMaterial" << std::endl; }










