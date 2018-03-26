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
                                                                       
// $Revision: 1.00 $
// $Date: 2010/09/08 20:01:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/triangular/Tri31.h,v $
                                                                       
// Written: Roozbeh Geraili Mikola (roozbehg@berkeley.edu)
// Created: Sep 2010
// Revised: --------
//
// Description: This file contains the class definition for Tri31.

#include "Tri31.h"
#include <domain/mesh/node/Node.h>
#include <material/nD/NDMaterial.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/ElementResponse.h>


#include "domain/component/Parameter.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"

double XC::Tri31::matrixData[36];
XC::Matrix XC::Tri31::K(matrixData, 6, 6);
XC::Vector XC::Tri31::P(6);
double XC::Tri31::shp[3][3];

XC::Tri31::Tri31(int tag, int nd1, int nd2, int nd3,
             NDMaterial &m, const std::string &type, double t,
             double p, double r, const BodyForces2D &bForces)
  : TriBase3N<SolidMech2D>(tag,ELE_TAG_Tri31,SolidMech2D(1,m,type,t,r)),
    bf(bForces), pressureLoad(6), pressure(p), Ki(nullptr)
  {
    load.reset(6);
  }

XC::Tri31::Tri31(void)
  :TriBase3N<SolidMech2D>(0,ELE_TAG_Tri31,SolidMech2D(1,nullptr,1.0,0.0)),
   pressureLoad(6), pressure(0.0), Ki(0)
  { }

//! @brief Constructor.
XC::Tri31::Tri31(int tag,const NDMaterial *ptr_mat)
  :TriBase3N<SolidMech2D>(tag,ELE_TAG_Tri31,SolidMech2D(4,ptr_mat,1.0,0.0)), pressureLoad(8), pressure(0.0), Ki(nullptr)
  {load.reset(8);}

//! @brief Virtual constructor.
XC::Element* XC::Tri31::getCopy(void) const
  { return new Tri31(*this); }

XC::Tri31::~Tri31(void)
  {    
    if(Ki)
      {
        delete Ki;
        Ki= nullptr;
      }
  }

int XC::Tri31::getNumDOF() const
  { return 6; }

void XC::Tri31::setDomain(Domain *theDomain)
  {
    TriBase3N<SolidMech2D>::setDomain(theDomain);

    const int dofNd1 = theNodes[0]->getNumberDOF();
    const int dofNd2 = theNodes[1]->getNumberDOF();
    const int dofNd3 = theNodes[2]->getNumberDOF();
   
    if(dofNd1 != 2 && dofNd1 != 3)
      {
      std::cerr << "WARNING Tri31::setDomain() element " << this->getTag() << " does not have 2 or 3 DOF at node " << theNodes[0]->getTag() << std::endl;
      return;
    }
    if(dofNd2 != 2 && dofNd2 != 3) {
      std::cerr << "WARNING Tri31::setDomain() element " << this->getTag() << " does not have 2 or 3 DOF at node " << theNodes[1]->getTag() << std::endl;
      return;
    }
    if(dofNd3 != 2 && dofNd3 != 3) {
      std::cerr << "WARNING Tri31::setDomain() element " << this->getTag() << " does not have 2 or 3 DOF at node " << theNodes[2]->getTag() << std::endl;
      return;
    }

    // Compute consistent nodal loads due to pressure
    this->setPressureLoadAtNodes();
  }



int XC::Tri31::update()
  {
    const Vector &disp1 = theNodes[0]->getTrialDisp();
    const Vector &disp2 = theNodes[1]->getTrialDisp();
    const Vector &disp3 = theNodes[2]->getTrialDisp();
       
    static double u[2][3];

    u[0][0] = disp1(0);
    u[1][0] = disp1(1);
    u[0][1] = disp2(0);
    u[1][1] = disp2(1);
    u[0][2] = disp3(0);
    u[1][2] = disp3(1);

    static Vector eps(3);

    int ret = 0;

    // Loop over the integration points
    for(size_t i = 0; i < physicalProperties.size(); i++)
      {

        // Determine Jacobian for this integration point
        const GaussPoint &gp= getGaussModel().getPuntosGauss()[i];
        this->shapeFunction(gp);

        // Interpolate strains
        //eps = B*u;
        //eps.addMatrixVector(0.0, B, u, 1.0);
        eps.Zero();
        for(int beta = 0; beta < numNodes(); beta++)
          {
            eps(0) += shp[0][beta]*u[0][beta];
            eps(1) += shp[1][beta]*u[1][beta];
            eps(2) += shp[0][beta]*u[1][beta] + shp[1][beta]*u[0][beta];
          }

        // Set the material strain
        ret += physicalProperties[i]->setTrialStrain(eps);
      }
    return ret;
  }

const XC::Matrix &XC::Tri31::getTangentStiff(void) const                                                                    
  {
     K.Zero();

     double dvol;
     double DB[3][2];

     // Loop over the integration points
     for(size_t i = 0; i < physicalProperties.size(); i++)
       {
         // Determine Jacobian for this integration point
        const GaussPoint &gp= getGaussModel().getPuntosGauss()[i];
         dvol= this->shapeFunction(gp);
         dvol*= (physicalProperties.getThickness()*gp.weight());
         
         // Get the material tangent
         const Matrix &D= physicalProperties[i]->getTangent();
         
         // Perform numerical integration
         //K = K + (B^ D * B) * intWt(i)*intWt(j) * detJ;
         //K.addMatrixTripleProduct(1.0, B, D, intWt(i)*intWt(j)*detJ);
      
         double D00 = D(0,0); double D01 = D(0,1); double D02 = D(0,2);
         double D10 = D(1,0); double D11 = D(1,1); double D12 = D(1,2);
         double D20 = D(2,0); double D21 = D(2,1); double D22 = D(2,2);

         //for(int beta = 0, ib = 0, colIb =0, colIbP1 = 8;
         //   beta < 4;
         //   beta++, ib += 2, colIb += 16, colIbP1 += 16) {
        
         for(int alpha = 0, ia = 0; alpha < numNodes(); alpha++, ia += 2)
           {
             for(int beta = 0, ib = 0; beta < numNodes(); beta++, ib += 2)
               {
                 DB[0][0]= dvol * (D00 * shp[0][beta] + D02 * shp[1][beta]);
                 DB[1][0]= dvol * (D10 * shp[0][beta] + D12 * shp[1][beta]);
                 DB[2][0]= dvol * (D20 * shp[0][beta] + D22 * shp[1][beta]);
                 DB[0][1]= dvol * (D01 * shp[1][beta] + D02 * shp[0][beta]);
                 DB[1][1]= dvol * (D11 * shp[1][beta] + D12 * shp[0][beta]);
                 DB[2][1]= dvol * (D21 * shp[1][beta] + D22 * shp[0][beta]);
          
                 K(ia,ib)+= shp[0][alpha]*DB[0][0] + shp[1][alpha]*DB[2][0];
                 K(ia,ib+1)+= shp[0][alpha]*DB[0][1] + shp[1][alpha]*DB[2][1];
                 K(ia+1,ib)+= shp[1][alpha]*DB[1][0] + shp[0][alpha]*DB[2][0];
                 K(ia+1,ib+1)+= shp[1][alpha]*DB[1][1] + shp[0][alpha]*DB[2][1];
                 //matrixData[colIb   +   ia] += shp[0][alpha]*DB[0][0] + shp[1][alpha]*DB[2][0];
                 //matrixData[colIbP1 +   ia] += shp[0][alpha]*DB[0][1] + shp[1][alpha]*DB[2][1];
                 //matrixData[colIb   + ia+1] += shp[1][alpha]*DB[1][0] + shp[0][alpha]*DB[2][0];
                 //matrixData[colIbP1 + ia+1] += shp[1][alpha]*DB[1][1] + shp[0][alpha]*DB[2][1];
             }
        }
     }
    if(isDead())
      K*=dead_srf;    
    return K;
  }


const XC::Matrix &XC::Tri31::getInitialStiff(void) const                                                              
  {
    if(!Ki)
      {
        K.Zero();
 
        double dvol;
        double DB[3][2];
 
        // Loop over the integration points
        for(size_t i = 0; i < physicalProperties.size(); i++)
          {
            // Determine Jacobian for this integration point
            const GaussPoint &gp= getGaussModel().getPuntosGauss()[i];
            dvol = this->shapeFunction(gp);
            dvol *= (physicalProperties.getThickness()*gp.weight());
   
           // Get the materialmgp tangent
           const Matrix &D = physicalProperties[i]->getInitialTangent();

           double D00 = D(0,0); double D01 = D(0,1); double D02 = D(0,2);
           double D10 = D(1,0); double D11 = D(1,1); double D12 = D(1,2);
           double D20 = D(2,0); double D21 = D(2,1); double D22 = D(2,2);
   
           // Perform numerical integration
           //K = K + (B^ D * B) * intWt(i)*intWt(j) * detJ;
           //K.addMatrixTripleProduct(1.0, B, D, intWt(i)*intWt(j)*detJ);
           //for(int beta = 0, ib = 0, colIb =0, colIbP1 = 2*numNodes(); beta < numNodes(); beta++, ib += 2, colIb += numNodes()*numNodes(), colIbP1 += numNodes()*numNodes()) {
            //    for(int alpha = 0, ia = 0; alpha < numNodes(); alpha++, ia += 2) {

            for(int alpha = 0, ia = 0; alpha < numNodes(); alpha++, ia += 2)
              {
                for(int beta = 0, ib = 0; beta < numNodes(); beta++, ib += 2)
                  {
                    DB[0][0] = dvol * (D00 * shp[0][beta] + D02 * shp[1][beta]);
                    DB[1][0] = dvol * (D10 * shp[0][beta] + D12 * shp[1][beta]);
                    DB[2][0] = dvol * (D20 * shp[0][beta] + D22 * shp[1][beta]);
                    DB[0][1] = dvol * (D01 * shp[1][beta] + D02 * shp[0][beta]);
                    DB[1][1] = dvol * (D11 * shp[1][beta] + D12 * shp[0][beta]);
                    DB[2][1] = dvol * (D21 * shp[1][beta] + D22 * shp[0][beta]);

                    K(ia,ib)     += shp[0][alpha]*DB[0][0] + shp[1][alpha]*DB[2][0];
                    K(ia,ib+1)   += shp[0][alpha]*DB[0][1] + shp[1][alpha]*DB[2][1];
                    K(ia+1,ib)   += shp[1][alpha]*DB[1][0] + shp[0][alpha]*DB[2][0];
                    K(ia+1,ib+1) += shp[1][alpha]*DB[1][1] + shp[0][alpha]*DB[2][1];
       
                    //matrixData[colIb   +   ia] += shp[0][alpha]*DB[0][0] + shp[1][alpha]*DB[2][0];
                    //matrixData[colIbP1 +   ia] += shp[0][alpha]*DB[0][1] + shp[1][alpha]*DB[2][1];
                    //matrixData[colIb   + ia+1] += shp[1][alpha]*DB[1][0] + shp[0][alpha]*DB[2][0];
                    //matrixData[colIbP1 + ia+1] += shp[1][alpha]*DB[1][1] + shp[0][alpha]*DB[2][1];
                  }
              }
	  }
         Ki= new Matrix(K);
      }
    if(isDead())
      K*=dead_srf;
    return K;
  }

const XC::Matrix &XC::Tri31::getMass(void) const
  {
    K.Zero();

    static Vector rhoi; //numgp
    rhoi= physicalProperties.getRhoi();
    double sum = 0.0;
    for(int i = 0; i < rhoi.Size(); i++)
      sum += rhoi[i];

    if(sum == 0.0) return K;

    double rhodvol, Nrho;

    // Compute a lumped mass matrix
    for(size_t i = 0; i < physicalProperties.size(); i++)
      {
       
        // Determine Jacobian for this integration point
        const GaussPoint &gp= getGaussModel().getPuntosGauss()[i];
        rhodvol = this->shapeFunction(gp);

        // Element plus material density ... MAY WANT TO REMOVE ELEMENT DENSITY
        rhodvol *= (rhoi[i]*physicalProperties.getThickness()*gp.weight());

        for(int alpha = 0, ia = 0; alpha < numNodes(); alpha++, ia++)
           {
              Nrho = shp[2][alpha]*rhodvol;
              K(ia,ia) += Nrho;
              ia++;
              K(ia,ia) += Nrho;
           }
      }
    return K;
  }

//! @brief Returns the puntos de Gauss of the element.
const XC::GaussModel &XC::Tri31::getGaussModel(void) const
  { return gauss_model_tria1; }

int XC::Tri31::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << "Tri31::addLoad - load type unknown for ele with tag: " << this->getTag() << std::endl;
    return -1;
  }

int XC::Tri31::addInertiaLoadToUnbalance(const Vector &accel)
  {
    static Vector rhoi; //numgp
    rhoi= physicalProperties.getRhoi();
    double sum = 0.0;
    for(size_t i = 0; i < physicalProperties.size(); i++)
      sum += rhoi[i];

    if(sum == 0.0)
    return 0;

    // Get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    const Vector &Raccel3 = theNodes[2]->getRV(accel);

    if(2 != Raccel1.Size() || 2 != Raccel2.Size() || 2 != Raccel3.Size()) {
            std::cerr << "Tri31::addInertiaLoadToUnbalance matrix and vector sizes are incompatable\n";
        return -1;
    }

    static double ra[6];

    ra[0] = Raccel1(0);
    ra[1] = Raccel1(1);
    ra[2] = Raccel2(0);
    ra[3] = Raccel2(1);
    ra[4] = Raccel3(0);
    ra[5] = Raccel3(1);

    // Compute mass matrix
    this->getMass();

    // Want to add ( - fact * M R * accel ) to unbalance
    // Take advantage of lumped mass matrix
    for(int i= 0; i < 2*numNodes(); i++)
      load(i)+= -K(i,i)*ra[i];
    return 0;
 }

const XC::Vector &XC::Tri31::getResistingForce(void) const
  {
    P.Zero();

    double dvol;

    // Loop over the integration points
    for(size_t i = 0; i < physicalProperties.size(); i++)
      {
        // Determine Jacobian for this integration point
        const GaussPoint &gp= getGaussModel().getPuntosGauss()[i];
        dvol = this->shapeFunction(gp);
        dvol *= (physicalProperties.getThickness()*gp.weight());

        // Get material stress response
        const Vector &sigma = physicalProperties[i]->getStress();

        // Perform numerical integration on internal force
        //P = P + (B^ sigma) * intWt(i)*intWt(j) * detJ;
        //P.addMatrixTransposeVector(1.0, B, sigma, intWt(i)*intWt(j)*detJ);
        for(int alpha = 0, ia = 0; alpha < numNodes(); alpha++, ia += 2) {                                    
               
                P(ia) += dvol*(shp[0][alpha]*sigma(0) + shp[1][alpha]*sigma(2));
               
                P(ia+1) += dvol*(shp[1][alpha]*sigma(1) + shp[0][alpha]*sigma(2));

                // Subtract equiv. body forces from the nodes
                //P = P - (N^ b) * intWt(i)*intWt(j) * detJ;
                //P.addMatrixTransposeVector(1.0, N, b, -intWt(i)*intWt(j)*detJ);
                P(ia)   -= dvol*(shp[2][alpha]*bf[0]);
                P(ia+1) -= dvol*(shp[2][alpha]*bf[1]);
        }
      }

    // Subtract pressure loading from resisting force
    if(pressure != 0.0)
      {
        //P = P - pressureLoad;
        P.addVector(1.0, pressureLoad, -1.0);
      }
   
    // Subtract other external nodal loads ... P_res = P_int - P_ext
    //P = P - load;
    P.addVector(1.0, load, -1.0);

    return P;
  }

const XC::Vector &XC::Tri31::getResistingForceIncInertia() const
  {
    static Vector rhoi; //numgp
    rhoi= physicalProperties.getRhoi();
    double sum = 0.0;
    for(int i = 0; i < rhoi.Size(); i++)
      sum += rhoi[i];

    // if no mass terms .. just add damping terms
    if(sum == 0.0)
      {
        this->getResistingForce();

        // add the damping forces if rayleigh damping
        if(!rayFactors.nullKValues())
          P += this->getRayleighDampingForces();
        return P;
      }

    const Vector &accel1 = theNodes[0]->getTrialAccel();
    const Vector &accel2 = theNodes[1]->getTrialAccel();
    const Vector &accel3 = theNodes[2]->getTrialAccel();
   
    static double a[6];

    a[0] = accel1(0);
    a[1] = accel1(1);
    a[2] = accel2(0);
    a[3] = accel2(1);
    a[4] = accel3(0);
    a[5] = accel3(1);

    // Compute the current resisting force
    this->getResistingForce();

    // Compute the mass matrix
    this->getMass();

    // Take advantage of lumped mass matrix
    for(int i = 0; i < 2*numNodes(); i++) P(i) += K(i,i)*a[i];

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      P+= this->getRayleighDampingForces();
    if(isDead())
      P*=dead_srf;
    return P;
 }

//! @brief Send object members through the channel being passed as parameter.
int XC::Tri31::sendData(CommParameters &cp)
  {
    int res= TriBase3N<SolidMech2D>::sendData(cp);
    res+=cp.sendDoubles(bf[0],bf[1],pressure,getDbTagData(),CommMetaData(8));
    res+= cp.sendVector(pressureLoad,getDbTagData(),CommMetaData(9));
    res+= cp.sendMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(10,11,12,13));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Tri31::recvData(const CommParameters &cp)
  {
    int res= TriBase3N<SolidMech2D>::recvData(cp);
    res+=cp.receiveDoubles(bf[0],bf[1],pressure,getDbTagData(),CommMetaData(8));
    res+= cp.receiveVector(pressureLoad,getDbTagData(),CommMetaData(9));
    Ki= cp.receiveMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(10,11,12,13));
    return res;
  }
//! @brief Sends object through the channel being passed as parameter.
int XC::Tri31::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(14);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::Tri31::recvSelf(const CommParameters &cp)
  {
    inicComm(14);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::Tri31::Print(std::ostream &s, int flag)                                                                
  {
    if(flag == 2) 
      {
            s << "#Tri31\n";

            const int nstress = physicalProperties.size();

        for(int i=0; i<numNodes(); i++)
          {
            const XC::Vector &nodeCrd = theNodes[i]->getCrds();
            //const XC::Vector &nodeDisp = theNodes[i]->getDisp();
            s << "#NODE " << nodeCrd(0) << " " << nodeCrd(1) << " " << std::endl;
          }

            // spit out the section location & invoke print on the scetion
            const int numMaterials = physicalProperties.size();

            static Vector avgStress(nstress);
            static Vector avgStrain(nstress);
            avgStress.Zero();
            avgStrain.Zero();
            for(int i=0; i<numMaterials; i++) {
                    avgStress += physicalProperties[i]->getStress();
                    avgStrain += physicalProperties[i]->getStrain();
            }
            avgStress /= numMaterials;
            avgStrain /= numMaterials;

            s << "#AVERAGE_STRESS ";
            for(int i=0; i<nstress; i++) s << avgStress(i) << " " ;
            s << std::endl;

            s << "#AVERAGE_STRAIN ";
            for(int i=0; i<nstress; i++) s << avgStrain(i) << " " ;
            s << std::endl;

    } else {
            s << "\nTri31, element id:  " << this->getTag() << std::endl;
        s << "\tConnected external nodes:  " << theNodes;
        s << "\tphysicalProperties.getThickness():  " << physicalProperties.getThickness() << std::endl;
        s << "\tsurface pressure:  " << pressure << std::endl;
        s << "\tbody forces:  " << bf << std::endl;
        physicalProperties.Print(s,flag);
        s << "\tStress (xx yy xy)" << std::endl;
        for(size_t i = 0; i<physicalProperties.size(); i++) s << "\t\tGauss point " << i+1 << ": " << physicalProperties[i]->getStress();
    }
}

XC::Response *XC::Tri31::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
     if(argv[0] == "force" || argv[0] == "forces")
      return new ElementResponse(this, 1, P);
    else if(argv[0] == "stiff" || argv[0] == "stiffness")
      return new ElementResponse(this, 2, K);
    else if(argv[0] == "material" || argv[0] == "integrPoint")
      {
        size_t pointNum = atoi(argv[1]);
        if(pointNum > 0 && pointNum <= physicalProperties.size())
          return setMaterialResponse(physicalProperties[pointNum-1],argv,2,eleInfo);
        else
          return nullptr;
      }
    else if(argv[0] == "stresses" || argv[0] == "stress")
      { return new ElementResponse(this, 3, P); }
    else // otherwise response quantity is unknown for the quad class
      return nullptr;
  }

int XC::Tri31::getResponse(int responseID, Information &eleInfo)                                        
  {
    if(responseID == 1)
      { return eleInfo.setVector(this->getResistingForce()); }
    else if(responseID == 2)
      { return eleInfo.setMatrix(this->getTangentStiff()); }
    else if(responseID == 3)
      {
        // Loop over the integration points
        int cnt = 0;
        for(size_t i= 0;i<physicalProperties.size(); i++)
          {
            // Get material stress response
            const XC::Vector &sigma = physicalProperties[i]->getStress();
            P(cnt) = sigma(0);
            P(cnt+1) = sigma(1);
            cnt += 2;
          }
        return eleInfo.setVector(P);
      }
    else
      return -1;
  }

int XC::Tri31::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // quad mass density per unit volume
    if(argv[0] == "rho")
      return param.addObject(1, this);
    else if(argv[0] == "pressure")
      return param.addObject(2, this);
    else if(argv[0] == "material") // a material parameter
      {
        size_t pointNum = atoi(argv[1]);
        if(pointNum > 0 && pointNum <= physicalProperties.size())
          {
            int ok =  setMaterialParameter(physicalProperties[pointNum-1],argv,2,param);
            if(ok < 0)
              return -1;
            else if(ok >= 0 && ok < 100)
              return pointNum*100 + ok;
            else
              return -1;
          }
        else
          return -1;
      }
    else // otherwise parameter is unknown for the Tri31 class
      return -1;
  }
   
int XC::Tri31::updateParameter(int parameterID, Information &info)                                          
  {
    switch(parameterID)
      {
      case -1:
        return -1;
      case 1:
        physicalProperties.setRho(info.theDouble);
        this->getMass();        // update mass matrix
        return 0;
      case 2:
        pressure = info.theDouble;
        this->setPressureLoadAtNodes();        // update consistent nodal loads
        return 0;
      default:
        if(parameterID >= 100)
          { // material parameter
            size_t pointNum = parameterID/100;
            if(pointNum > 0 && pointNum<= physicalProperties.size())
              return physicalProperties[pointNum-1]->updateParameter(parameterID-100*pointNum, info);
            else
              return -1;
          }
        else // unknown
          return -1;
      }
  }

double XC::Tri31::shapeFunction(const GaussPoint &gp) const
  {
    const double &xi= gp.r_coordinate();
    const double &eta= gp.s_coordinate();
    const Vector &nd1Crds = theNodes[0]->getCrds();
    const Vector &nd2Crds = theNodes[1]->getCrds();
    const Vector &nd3Crds = theNodes[2]->getCrds();

    shp[2][0] = xi;         // N_1
    shp[2][1] = eta;                // N_2
    shp[2][2] = 1-xi-eta;   // N_3

    double J[2][2];

    // See p 180 "A First Course in Finite Elements" by Fish and Belytschko.
    J[0][0] = (nd1Crds(0) - nd3Crds(0));
    J[0][1] = (nd2Crds(0) - nd3Crds(0));
    J[1][0] = (nd1Crds(1) - nd3Crds(1));
    J[1][1] = (nd2Crds(1) - nd3Crds(1));

    double detJ = J[0][0]*J[1][1] - J[0][1]*J[1][0];
    double oneOverdetJ = 1.0/detJ;
    double L[2][2];

    // L = inv(J)
    L[0][0] =  J[1][1]*oneOverdetJ;
    L[1][0] = -J[0][1]*oneOverdetJ;
    L[0][1] = -J[1][0]*oneOverdetJ;
    L[1][1] =  J[0][0]*oneOverdetJ;

    // See Cook, Malkus, Plesha p. 169 for the derivation of these terms
    shp[0][0] = L[0][0];                    // N_1,1
    shp[0][1] = L[0][1];                        // N_2,1
    shp[0][2] = -(L[0][0] + L[0][1]);   // N_3,1
       
    shp[1][0] = L[1][0];                    // N_1,2
    shp[1][1] = L[1][1];                        // N_2,2
    shp[1][2] = -(L[1][0] + L[1][1]);   // N_3,2

    return detJ;
  }

void XC::Tri31::setPressureLoadAtNodes(void)
  {
    pressureLoad.Zero();

    if(pressure == 0.0) return;

    const Vector &node1 = theNodes[0]->getCrds();
    const Vector &node2 = theNodes[1]->getCrds();
    const Vector &node3 = theNodes[2]->getCrds();

    double x1 = node1(0);
    double y1 = node1(1);
    double x2 = node2(0);
    double y2 = node2(1);
    double x3 = node3(0);
    double y3 = node3(1);

    double dx12 = x2-x1;
    double dy12 = y2-y1;
    double dx23 = x3-x2;
    double dy23 = y3-y2;
    double dx31 = x1-x3;
    double dy31 = y1-y3;

    double pressureOver2 = pressure/2.0;

    // Contribution from side 12
    pressureLoad(0) += pressureOver2*dy12;
    pressureLoad(2) += pressureOver2*dy12;
    pressureLoad(1) += pressureOver2*-dx12;
    pressureLoad(3) += pressureOver2*-dx12;

    // Contribution from side 23
    pressureLoad(2) += pressureOver2*dy23;
    pressureLoad(4) += pressureOver2*dy23;
    pressureLoad(3) += pressureOver2*-dx23;
    pressureLoad(5) += pressureOver2*-dx23;

    // Contribution from side 31
    pressureLoad(4) += pressureOver2*dy31;
    pressureLoad(0) += pressureOver2*dy31;
    pressureLoad(5) += pressureOver2*-dx31;
    pressureLoad(1) += pressureOver2*-dx31;

    //pressureLoad = pressureLoad*physicalProperties.getThickness();
  }
