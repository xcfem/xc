//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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

// $Revision: 1.24 $
// $Date: 2003/10/07 21:18:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/FourNodeQuad.cpp,v $

// Written: MHS
// Created: Feb 2000
// Revised: Dec 2000 for efficiency
//
// Description: This file contains the class definition for XC::FourNodeQuad.

#include "FourNodeQuad.h"
#include <domain/mesh/node/Node.h>
#include <material/nD/NDMaterial.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>

#include "domain/component/Parameter.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "domain/load/volumetric/SelfWeight.h"
#include "domain/load/plane/QuadRawLoad.h"


double XC::FourNodeQuad::matrixData[64];
XC::Matrix XC::FourNodeQuad::K(matrixData, 8, 8);
XC::Matrix XC::FourNodeQuad::mass(8,8);
XC::Vector XC::FourNodeQuad::P(8);
double XC::FourNodeQuad::shp[3][4]; //Values of shape functions.

//! @brief Constructor.
XC::FourNodeQuad::FourNodeQuad(int tag,const NDMaterial *ptr_mat)
  :SolidMech4N(tag,ELE_TAG_FourNodeQuad,SolidMech2D(4,ptr_mat,1.0)), pressureLoad(8), pressure(0.0), p0(), eps(4)
  {
    load.reset(8);
  }

//! @brief Constructor.
//!
//! @param nd1, nd2, nd3, nd3: four nodes defining element boundaries,
//!                            input in counterclockwise order around
//!                            the element.
//! @param t: element thickness.
//! @param type: string representing material behavior. Valid options
//!              depend on the NDMaterial object and its available material
//!              formulations. The type parameter can be either "PlaneStrain"
//!              or "PlaneStress."
//! @param m: element material.
XC::FourNodeQuad::FourNodeQuad(int tag, int nd1, int nd2, int nd3, int nd4,
                               NDMaterial &m, const std::string &type, double t,
                           double p, const BodyForces2D &bForces)
  : SolidMech4N(tag,ELE_TAG_FourNodeQuad,nd1,nd2,nd3,nd4,SolidMech2D(4,m,type,t)), bf(bForces), pressureLoad(8), pressure(p), p0(), eps(4)
  {
    load.reset(8);
  }

//! @brief Virtual constructor.
XC::Element* XC::FourNodeQuad::getCopy(void) const
  { return new FourNodeQuad(*this); }

//! @brief Destructor.
XC::FourNodeQuad::~FourNodeQuad(void)
  {}

//! @brief Returns the value of the persistent (does not get wiped out by
//! zeroLoad) initial deformation of the element.
const std::vector<XC::Vector> &XC::FourNodeQuad::getPersistentInitialDeformation(void) const
  { return persistentInitialDeformation; }

//! @brief Increments the persistent (does not get wiped out by zeroLoad)
//! initial deformation of the section. It's used to store the deformation
//! of the material during the periods in which their elements are deactivated.
//! (see alive() method).
void XC::FourNodeQuad::incrementPersistentInitialDeformationWithCurrentDeformation(void)
  {
    if(persistentInitialDeformation.empty()) // Not yet initialized.
      persistentInitialDeformation= this->eps; // not yet initialized.
    else
      for(int i= 0; i<4; i++)
        persistentInitialDeformation[i]+= this->eps[i]; // increment the current value.
  }

//! @brief Return the number of element DOFs.
int XC::FourNodeQuad::getNumDOF(void) const
  { return 8; }

//! @brief Reinitialize values that depend on the nodal coordinates (for
//! example after a "manual" change in the nodal coordinates, to impose
//! an imperfect shape or a precamber.
int XC::FourNodeQuad::resetNodalCoordinates(void)
  {
    // Compute consistent nodal loads due to pressure
    this->setPressureLoadAtNodes();
    return 0;
  }

//! @brief Sets domain pointer and computes the consistent load vector due to pressure.
void XC::FourNodeQuad::setDomain(Domain *theDomain)
  {
    SolidMech4N::setDomain(theDomain);

    if(!theNodes.checkNumDOF(2,getTag()))
      std::cerr << theNodes << std::endl;
       
    // Update values depending on nodal coordinates.
    this->resetNodalCoordinates();
  }

//! @brief Update the values of the state variables.
int XC::FourNodeQuad::update(void)
  {
    const Vector &disp1= theNodes[0]->getTrialDisp();
    const Vector &disp2= theNodes[1]->getTrialDisp();
    const Vector &disp3= theNodes[2]->getTrialDisp();
    const Vector &disp4= theNodes[3]->getTrialDisp();

    static double u[2][4];

    u[0][0] = disp1(0);
    u[1][0] = disp1(1);
    u[0][1] = disp2(0);
    u[1][1] = disp2(1);
    u[0][2] = disp3(0);
    u[1][2] = disp3(1);
    u[0][3] = disp4(0);
    u[1][3] = disp4(1);

    int ret = 0;

    // Loop over the integration points
    for(size_t i= 0;i<physicalProperties.size();i++)
      {
        //Determine Jacobian for this integration point
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
        this->shapeFunction(gp);

        // Interpolate strains
        //eps = B*u;
        //eps.addMatrixVector(0.0, B, u, 1.0);
	Vector &eps= this->eps[i];
	eps= Vector(3, 0.0); //zero the strains.	
        for(int beta= 0;beta<4;beta++)
          {
            eps(0)+= shp[0][beta]*u[0][beta];
            eps(1)+= shp[1][beta]*u[1][beta];
            eps(2)+= shp[0][beta]*u[1][beta] + shp[1][beta]*u[0][beta];
          }
        // Set the material strain
	if(!persistentInitialDeformation.empty())
	  {
	    eps-= persistentInitialDeformation[i];
	  }		 
        ret+= physicalProperties[i]->setTrialStrain(eps);
      }
    return ret;
  }

//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::FourNodeQuad::getTangentStiff(void) const
  {
    K.Zero();

    double dvol;
    double DB[3][2];

    //Loop over the integration points
    for(size_t i = 0;i<physicalProperties.size();i++)
      {
        // Determine Jacobian for this integration point
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
        dvol= this->shapeFunction(gp);
        dvol*= (physicalProperties.getThickness()*gp.weight());
	if(dvol<0.0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; negative Jacobian-determinant ("
	            << "dvol= " << dvol << ")"
	            << std::endl;

        // Get the material tangent
        const Matrix &D = physicalProperties[i]->getTangent();

        // Perform numerical integration
        //K = K + (B^ D * B) * intWt(i)*intWt(j) * detJ;
        //K.addMatrixTripleProduct(1.0, B, D, intWt(i)*intWt(j)*detJ);

        double D00 = D(0,0); double D01 = D(0,1); double D02 = D(0,2);
        double D10 = D(1,0); double D11 = D(1,1); double D12 = D(1,2);
        double D20 = D(2,0); double D21 = D(2,1); double D22 = D(2,2);

        //          for(int beta = 0, ib = 0, colIb =0, colIbP1 = 8;
        //   beta < 4;
        //   beta++, ib += 2, colIb += 16, colIbP1 += 16){

        for(int alpha= 0,ia=0;alpha<4; alpha++,ia+=2)
          {
            for(int beta= 0,ib=0; beta<4; beta++,ib+=2)
              {
                DB[0][0] = dvol * (D00 * shp[0][beta] + D02 * shp[1][beta]);
                DB[1][0] = dvol * (D10 * shp[0][beta] + D12 * shp[1][beta]);
                DB[2][0] = dvol * (D20 * shp[0][beta] + D22 * shp[1][beta]);
                DB[0][1] = dvol * (D01 * shp[1][beta] + D02 * shp[0][beta]);
                DB[1][1] = dvol * (D11 * shp[1][beta] + D12 * shp[0][beta]);
                DB[2][1] = dvol * (D21 * shp[1][beta] + D22 * shp[0][beta]);

                K(ia,ib) += shp[0][alpha]*DB[0][0] + shp[1][alpha]*DB[2][0];
                K(ia,ib+1) += shp[0][alpha]*DB[0][1] + shp[1][alpha]*DB[2][1];
                K(ia+1,ib) += shp[1][alpha]*DB[1][0] + shp[0][alpha]*DB[2][0];
                K(ia+1,ib+1) += shp[1][alpha]*DB[1][1] + shp[0][alpha]*DB[2][1];
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

//! @brief Return the initial tangent stiffness matrix.
const XC::Matrix &XC::FourNodeQuad::getInitialStiff(void) const
  {
    if(Ki.isEmpty())
      {
	K.Zero();

	double dvol;
	double DB[3][2];

	// Loop over the integration points
	for(size_t i= 0;i<physicalProperties.size();i++)
	  {
	    // Determine Jacobian for this integration point
	    const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
	    dvol= this->shapeFunction(gp);
	    dvol*= (physicalProperties.getThickness()*gp.weight());
	    if(dvol<0.0)
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; negative Jacobian-determinant ("
			<< "dvol= " << dvol << ")"
			<< std::endl;

	    // Get the material tangent
	    const XC::Matrix &D = physicalProperties[i]->getInitialTangent();

	    double D00 = D(0,0); double D01 = D(0,1); double D02 = D(0,2);
	    double D10 = D(1,0); double D11 = D(1,1); double D12 = D(1,2);
	    double D20 = D(2,0); double D21 = D(2,1); double D22 = D(2,2);

	    // Perform numerical integration
	    //K = K + (B^ D * B) * intWt(i)*intWt(j) * detJ;
	    //K.addMatrixTripleProduct(1.0, B, D, intWt(i)*intWt(j)*detJ);
	    for(int beta= 0,ib= 0,colIb= 0,colIbP1= 8;beta<4;beta++,ib+=2,colIb+=16,colIbP1+=16)
	      {
		for(int alpha= 0,ia = 0;alpha<4;alpha++,ia += 2)
		  {
		    DB[0][0] = dvol * (D00 * shp[0][beta] + D02 * shp[1][beta]);
		    DB[1][0] = dvol * (D10 * shp[0][beta] + D12 * shp[1][beta]);
		    DB[2][0] = dvol * (D20 * shp[0][beta] + D22 * shp[1][beta]);
		    DB[0][1] = dvol * (D01 * shp[1][beta] + D02 * shp[0][beta]);
		    DB[1][1] = dvol * (D11 * shp[1][beta] + D12 * shp[0][beta]);
		    DB[2][1] = dvol * (D21 * shp[1][beta] + D22 * shp[0][beta]);

		    matrixData[colIb   +   ia] += shp[0][alpha]*DB[0][0] + shp[1][alpha]*DB[2][0];
		    matrixData[colIbP1 +   ia] += shp[0][alpha]*DB[0][1] + shp[1][alpha]*DB[2][1];
		    matrixData[colIb   + ia+1] += shp[1][alpha]*DB[1][0] + shp[0][alpha]*DB[2][0];
		    matrixData[colIbP1 + ia+1] += shp[1][alpha]*DB[1][1] + shp[0][alpha]*DB[2][1];
		  }
	      }
	   }
	Ki= Matrix(K);
      }
    if(isDead())
      K*= dead_srf;
    return K;
  }

//! @brief Return the mass matrix.
const XC::Matrix &XC::FourNodeQuad::getMass(void) const
  {
    int tangFlag= 1;
    this->formInertiaTerms(tangFlag);
    if(isDead())
      mass*= dead_srf;
    return mass;
  }

//! @brief Return the Gauss points of the element.
const XC::GaussModel &XC::FourNodeQuad::getGaussModel(void) const
  { return gauss_model_quad4; }

//! @brief Reactivates the element.
void XC::FourNodeQuad::alive(void)
  {
    if(isDead())
      {
	// Store the current deformation.
        this->incrementPersistentInitialDeformationWithCurrentDeformation();
	this->update();
        SolidMech4N::alive();
      }
  }

//! @brief Sets loads to zero.
void XC::FourNodeQuad::zeroLoad(void)
  {
    SolidMech4N::zeroLoad();
    bf[0] = 0.0;
    bf[1] = 0.0;
    bf[2] = 0.0;
    p0.zero();
  }

//! @brief Adds a load over element.
int XC::FourNodeQuad::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << getClassName() << "::" << __FUNCTION__ 
                << "; load over inactive element: "
                << getTag() << std::endl;
    else
      {
        if(SelfWeight *quadLoad= dynamic_cast<SelfWeight *>(theLoad))
	  {
	    // added compatibility with selfWeight class implemented
	    // for all continuum elements, C.McGann, U.W.
	    // applyLoad = 1;
	    bf[0]+= loadFactor*quadLoad->getXFact();
	    bf[1]+= loadFactor*quadLoad->getYFact();
	    bf[2]+= loadFactor*quadLoad->getZFact();
	  }
        else if(QuadRawLoad *quadRawLoad= dynamic_cast<QuadRawLoad *>(theLoad))
          {
            const std::vector<double> ones(4, 1.0);
            quadRawLoad->addReactionsInBasicSystem(ones,loadFactor,p0); // Accumulate reactions in basic system
          }
        else if(QuadMecLoad *quadMecLoad= dynamic_cast<QuadMecLoad *>(theLoad))
          {
   	    computeTributaryAreas();
            const std::vector<double> areas= getTributaryAreas();
            quadMecLoad->addReactionsInBasicSystem(areas,loadFactor,p0); // Accumulate reactions in basic system
          }
        else
          return SolidMech4N::addLoad(theLoad,loadFactor);
      }
    return 0;
  }

//! @brief Adds inertia loads.
int XC::FourNodeQuad::addInertiaLoadToUnbalance(const Vector &accel)
  {
    const double sum = this->physicalProperties.getRho();
    
    if(sum == 0.0)
      return 0;

    // Get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    const Vector &Raccel3 = theNodes[2]->getRV(accel);
    const Vector &Raccel4 = theNodes[3]->getRV(accel);

    if(2 != Raccel1.Size() || 2 != Raccel2.Size() || 2 != Raccel3.Size() || 2 != Raccel4.Size())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; matrix and vector sizes are incompatible.\n";
        return -1;
      }

    static double ra[8];

    ra[0] = Raccel1(0);
    ra[1] = Raccel1(1);
    ra[2] = Raccel2(0);
    ra[3] = Raccel2(1);
    ra[4] = Raccel3(0);
    ra[5] = Raccel3(1);
    ra[6] = Raccel4(0);
    ra[7] = Raccel4(1);

    // Compute mass matrix
    this->getMass();

    // Want to add ( - fact * M R * accel ) to unbalance
    // Take advantage of lumped mass matrix
    for(int i= 0; i < 2*numNodes(); i++)
      load(i)+= -mass(i,i)*ra[i];
    return 0;
  }

//! @brief Return the element resisting force.
const XC::Vector &XC::FourNodeQuad::getResistingForce(void) const
  {
    P.Zero();

    double dvol;

    // Loop over the integration points
    for(size_t i= 0;i<physicalProperties.size();i++)
      {
        // Determine Jacobian for this integration point
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
        dvol= this->shapeFunction(gp);
        dvol*= (physicalProperties.getThickness()*gp.weight());

        // Get material stress response
        const Vector &sigma= physicalProperties[i]->getStress();

        // Perform numerical integration on internal force
        //P = P + (B^ sigma) * intWt(i)*intWt(j) * detJ;
        //P.addMatrixTransposeVector(1.0, B, sigma, intWt(i)*intWt(j)*detJ);
        for(int alpha = 0,ia = 0;alpha<numNodes(); alpha++,ia += 2)
          {

            P(ia)+= dvol*(shp[0][alpha]*sigma(0) + shp[1][alpha]*sigma(2));
            P(ia+1)+= dvol*(shp[1][alpha]*sigma(1) + shp[0][alpha]*sigma(2));

            // Subtract equiv. body forces from the nodes
            //P = P - (N^ b) * intWt(i)*intWt(j) * detJ;
            //P.addMatrixTransposeVector(1.0, N, b, -intWt(i)*intWt(j)*detJ);
            P(ia)-= dvol*(shp[2][alpha]*bf[0]);
            P(ia+1)-= dvol*(shp[2][alpha]*bf[1]);
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

    P+= p0.getVector();

    if(isDead())
      P*=dead_srf;
    return P;
  }

//! @brief Return the resisting force of the element including
//! inertia.
const XC::Vector &XC::FourNodeQuad::getResistingForceIncInertia(void) const
  {
    const double sum = this->physicalProperties.getRho();

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
    const Vector &accel4 = theNodes[3]->getTrialAccel();

    static double a[8];

    a[0] = accel1(0);
    a[1] = accel1(1);
    a[2] = accel2(0);
    a[3] = accel2(1);
    a[4] = accel3(0);
    a[5] = accel3(1);
    a[6] = accel4(0);
    a[7] = accel4(1);

    // Compute the current resisting force
    this->getResistingForce();

    // Compute the mass matrix
    this->getMass();

    //Take advantage of lumped mass matrix
    for(int i= 0;i<8;i++)
      P(i)+= mass(i,i)*a[i];

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      P+= this->getRayleighDampingForces();
    if(isDead())
      P*=dead_srf;
    return P;
  }

//! @brief form inertia terms
void XC::FourNodeQuad::formInertiaTerms(int tangFlag) const
  {
    mass.Zero();

    static Vector rhoi(4);
    const double sum= physicalProperties.getArealRho();

    if(sum != 0.0)
      {
        double rhodvol, Nrho;
        rhoi= physicalProperties.getRhoi();
	const double thickness= physicalProperties.getThickness();

        // Compute a lumped mass matrix
        for(size_t i= 0;i<physicalProperties.size();i++)
          {
            // Determine Jacobian for this integration point
            const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
            rhodvol = this->shapeFunction(gp);

            // Element plus material density ... MAY WANT TO REMOVE ELEMENT DENSITY
            //const double tmp = physicalProperties.getRho() + rhoi[i];
            const double tmp = rhoi[i];
            rhodvol*= (tmp*thickness*gp.weight());

            for(int alpha = 0, ia = 0; alpha<numNodes(); alpha++, ia++)
              {
                Nrho = shp[2][alpha]*rhodvol;
                mass(ia,ia)+= Nrho;
                ia++;
                mass(ia,ia)+= Nrho;
              }
          }
      }
  }

//! @brief Creates the inertia load that corresponds to the
//! acceleration argument.
//!
//! @param accel: acceleration vector.
XC::ElementalLoad *XC::FourNodeQuad::createInertiaLoad(const Vector &accel)
  {
    ElementalLoad *retval= nullptr;
    const bool haveRho= physicalProperties.haveRho();
    if(haveRho)
      {
	// Create 4x2= 8 rows acceleration vector.
        Vector nodeAccel(8);
	nodeAccel.Zero();
	for(int i= 0; i<4; i++)
	  for(int j= 0; j<2;j++)
	    {
	      const int k= 2*i+j;
	      nodeAccel(k)= accel(j);
	    }
	const int tangFlag= 1;
	formInertiaTerms(tangFlag);
	Vector force(8);
	force.addMatrixVector(1.0, mass, nodeAccel, -1.0);//= -mass*nodeAccel;
	const double norm= force.Norm2();
	if(norm>0.0)
	  {
	    // Extract nodal loads.
	    std::vector<Vector> nLoads(4);
	    for(int i=0;i<4;i++)
	      {
		Vector nLoad(2);
		for(int j= 0; j<2;j++)
		   {
		     const int k= 2*i+j;
		     nLoad(j)+= force(k);
		   }
		nLoads[i]= nLoad;
	      }
	    retval= vector2dRawLoadGlobal(nLoads);
	  }
      }
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::FourNodeQuad::sendData(Communicator &comm)
  {
    int res= SolidMech4N::sendData(comm);
    res+=comm.sendDoubles(bf[0],bf[1],pressure,getDbTagData(),CommMetaData(9));
    res+= comm.sendVector(pressureLoad,getDbTagData(),CommMetaData(10));
    res+= p0.sendData(comm,getDbTagData(),CommMetaData(11));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::FourNodeQuad::recvData(const Communicator &comm)
  {
    int res= SolidMech4N::recvData(comm);
    res+=comm.receiveDoubles(bf[0],bf[1],pressure,getDbTagData(),CommMetaData(9));
    res+= comm.receiveVector(pressureLoad,getDbTagData(),CommMetaData(10));
    res+= p0.receiveData(comm,getDbTagData(),CommMetaData(11));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::FourNodeQuad::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(11);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Receives object through the communicator argument.
int XC::FourNodeQuad::recvSelf(const Communicator &comm)
  {
    inicComm(11);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }


//! @brief Prints element information.
void XC::FourNodeQuad::Print(std::ostream &s, int flag) const
  {
    if(flag == 2)
      {
        s << "#FourNodeQuad\n";

        //const int numNodes = 4;
        const int nstress = 3 ;

        for(int i=0; i<numNodes(); i++)
          {
            const Vector &nodeCrd = theNodes[i]->getCrds();
            //const XC::Vector &nodeDisp = theNodes[i]->getDisp();
            s << "#NODE " << nodeCrd(0) << " " << nodeCrd(1) << " " << std::endl;
          }

        // spit out the section location & invoke print on the scetion

        static const Vector &avgStress= physicalProperties.getCommittedAvgStress();
        static const Vector &avgStrain= physicalProperties.getCommittedAvgStrain();

        s << "#AVERAGE_STRESS ";
        for(int i=0; i<nstress; i++)
          s << avgStress(i) << " " ;
        s << std::endl;

        s << "#AVERAGE_STRAIN ";
        for(int i=0;i<nstress; i++)
          s << avgStrain(i) << " " ;
        s << std::endl;
      }
    else
      {
        s << "\nFourNodeQuad, element id:  " << this->getTag() << std::endl;
        s << "\tConnected external nodes:  " << theNodes;
        s << "\tphysicalProperties.getThickness():  " << physicalProperties.getThickness() << std::endl;
        s << "\tmass density:  " << physicalProperties.getRho() << std::endl;
        s << "\tsurface pressure:  " << pressure << std::endl;
        s << "\tbody forces:  " << bf << std::endl;
        physicalProperties.Print(s,flag);
        s << "\tStress (xx yy xy)" << std::endl;
        for(size_t i = 0; i < physicalProperties.size(); i++)
          s << "\t\tGauss point " << i+1 << ": " << physicalProperties[i]->getStress();
      }
  }


int XC::FourNodeQuad::setParameter(const std::vector<std::string> &argv, Parameter &param)
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
    else // otherwise parameter is unknown for the XC::FourNodeQuad class
      return -1;
  }

int XC::FourNodeQuad::updateParameter(int parameterID, Information &info)
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
        this->setPressureLoadAtNodes(); // update consistent nodal loads
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

//! @brief Return the determinant of the jacobian at the position
//! being passed as parameters.
double XC::FourNodeQuad::shapeFunction(const GaussPoint &gp) const
  {
    const double &xi= gp.r_coordinate();
    const double &eta= gp.s_coordinate();
    const Vector &nd1Crds = theNodes[0]->getCrds();
    const Vector &nd2Crds = theNodes[1]->getCrds();
    const Vector &nd3Crds = theNodes[2]->getCrds();
    const Vector &nd4Crds = theNodes[3]->getCrds();

    const double oneMinuseta = 1.0-eta;
    const double onePluseta = 1.0+eta;
    const double oneMinusxi = 1.0-xi;
    const double onePlusxi = 1.0+xi;

    shp[2][0] = 0.25*oneMinusxi*oneMinuseta; // N_1
    shp[2][1] = 0.25*onePlusxi*oneMinuseta;  // N_2
    shp[2][2] = 0.25*onePlusxi*onePluseta;   // N_3
    shp[2][3] = 0.25*oneMinusxi*onePluseta;  // N_4

    double J[2][2];

    J[0][0] = 0.25 * (-nd1Crds(0)*oneMinuseta + nd2Crds(0)*oneMinuseta +
                nd3Crds(0)*(onePluseta) - nd4Crds(0)*(onePluseta));

    J[0][1] = 0.25 * (-nd1Crds(0)*oneMinusxi - nd2Crds(0)*onePlusxi +
                nd3Crds(0)*onePlusxi + nd4Crds(0)*oneMinusxi);

    J[1][0] = 0.25 * (-nd1Crds(1)*oneMinuseta + nd2Crds(1)*oneMinuseta +
                nd3Crds(1)*onePluseta - nd4Crds(1)*onePluseta);

    J[1][1] = 0.25 * (-nd1Crds(1)*oneMinusxi - nd2Crds(1)*onePlusxi +
                nd3Crds(1)*onePlusxi + nd4Crds(1)*oneMinusxi);

    const double detJ = J[0][0]*J[1][1] - J[0][1]*J[1][0];
    const double oneOverdetJ = 1.0/detJ;
    double L[2][2];

    // L = inv(J)
    L[0][0] =  J[1][1]*oneOverdetJ;
    L[1][0] = -J[0][1]*oneOverdetJ;
    L[0][1] = -J[1][0]*oneOverdetJ;
    L[1][1] =  J[0][0]*oneOverdetJ;

    double L00 = 0.25*L[0][0];
    double L10 = 0.25*L[1][0];
    double L01 = 0.25*L[0][1];
    double L11 = 0.25*L[1][1];

    double L00oneMinuseta = L00*oneMinuseta;
    double L00onePluseta  = L00*onePluseta;
    double L01oneMinusxi  = L01*oneMinusxi;
    double L01onePlusxi   = L01*onePlusxi;

    double L10oneMinuseta = L10*oneMinuseta;
    double L10onePluseta  = L10*onePluseta;
    double L11oneMinusxi  = L11*oneMinusxi;
    double L11onePlusxi   = L11*onePlusxi;

    // See Cook, Malkus, Plesha p. 169 for the derivation of these terms
    shp[0][0] = -L00oneMinuseta - L01oneMinusxi;    // N_1,1
    shp[0][1] =  L00oneMinuseta - L01onePlusxi;     // N_2,1
    shp[0][2] =  L00onePluseta  + L01onePlusxi;     // N_3,1
    shp[0][3] = -L00onePluseta  + L01oneMinusxi;    // N_4,1

    shp[1][0] = -L10oneMinuseta - L11oneMinusxi;    // N_1,2
    shp[1][1] =  L10oneMinuseta - L11onePlusxi;     // N_2,2
    shp[1][2] =  L10onePluseta  + L11onePlusxi;     // N_3,2
    shp[1][3] = -L10onePluseta  + L11oneMinusxi;    // N_4,2

    return detJ;
  }


//! @brief Return the cartesian coordinates corresponding to the given natural
//! coordinates.
Pos2d XC::FourNodeQuad::getCartesianCoordinates(const ParticlePos2d &pt, bool initialGeometry) const
  {
    const double &xi= pt.r_coordinate();
    const double &eta= pt.s_coordinate();
   
    const double oneMinuseta = 1.0-eta;
    const double onePluseta = 1.0+eta;
    const double oneMinusxi = 1.0-xi;
    const double onePlusxi = 1.0+xi;

    // Shape functions.
    const double N1= 0.25*oneMinusxi*oneMinuseta; // N_1
    const double N2= 0.25*onePlusxi*oneMinuseta; // N_2
    const double N3= 0.25*onePlusxi*onePluseta;  // N_3
    const double N4= 0.25*oneMinusxi*onePluseta; // N_4
    
    //Node positions
    double factor= 1.0;
    if(initialGeometry)
      factor= 0.0;
    const Pos3d n1Pos= theNodes[0]->getCurrentPosition3d(factor);
    const Pos3d n2Pos= theNodes[1]->getCurrentPosition3d(factor);
    const Pos3d n3Pos= theNodes[2]->getCurrentPosition3d(factor);
    const Pos3d n4Pos= theNodes[3]->getCurrentPosition3d(factor);
    
    const double x= N1*n1Pos.x()+N2*n2Pos.x()+N3*n3Pos.x()+N4*n4Pos.x();
    const double y= N1*n1Pos.y()+N2*n2Pos.y()+N3*n3Pos.y()+N4*n4Pos.y();
    return Pos2d(x,y);
  }

//! @brief Define the values of the pressure load at the element nodes.
void XC::FourNodeQuad::setPressureLoadAtNodes(void)
  {
    pressureLoad.Zero();

    if(pressure != 0.0)
      {
	const Vector &node1 = theNodes[0]->getCrds();
	const Vector &node2 = theNodes[1]->getCrds();
	const Vector &node3 = theNodes[2]->getCrds();
	const Vector &node4 = theNodes[3]->getCrds();

	double x1 = node1(0);
	double y1 = node1(1);
	double x2 = node2(0);
	double y2 = node2(1);
	double x3 = node3(0);
	double y3 = node3(1);
	double x4 = node4(0);
	double y4 = node4(1);

	double dx12 = x2-x1;
	double dy12 = y2-y1;
	double dx23 = x3-x2;
	double dy23 = y3-y2;
	double dx34 = x4-x3;
	double dy34 = y4-y3;
	double dx41 = x1-x4;
	double dy41 = y1-y4;

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

	// Contribution from side 34
	pressureLoad(4) += pressureOver2*dy34;
	pressureLoad(6) += pressureOver2*dy34;
	pressureLoad(5) += pressureOver2*-dx34;
	pressureLoad(7) += pressureOver2*-dx34;

	// Contribution from side 41
	pressureLoad(6) += pressureOver2*dy41;
	pressureLoad(0) += pressureOver2*dy41;
	pressureLoad(7) += pressureOver2*-dx41;
	pressureLoad(1) += pressureOver2*-dx41;
      }

    //pressureLoad = pressureLoad*physicalProperties.getThickness();
  }

double XC::FourNodeQuad::detJ(const double &xi,const double &eta) const
  { return shapeFunction(GaussPoint(Pos2d(xi,eta),0)); }
