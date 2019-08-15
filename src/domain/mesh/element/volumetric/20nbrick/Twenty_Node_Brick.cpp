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
/* *********************************************************************
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
//
// by Jinchi Lu and Zhaohui Yang (May 2004)
//
// 20NodeBrick element
//

#include "Twenty_Node_Brick.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/Element.h>
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/element/volumetric/brick/shp3d.h>
#include <domain/mesh/element/volumetric/UP-ucsd/shp3dv.h>
#include <utility/recorder/response/ElementResponse.h>
#include <material/nD/NDMaterial.h>
#include "utility/actor/actor/MatrixCommMetaData.h"


#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

namespace XC{
//static data
double  XC::Twenty_Node_Brick::xl[3][20] ;

XC::Matrix  XC::Twenty_Node_Brick::stiff(60,60) ;
XC::Vector  XC::Twenty_Node_Brick::resid(60) ;
XC::Matrix  XC::Twenty_Node_Brick::mass(60,60) ;
XC::Matrix  XC::Twenty_Node_Brick::damp(60,60) ;

const int XC::Twenty_Node_Brick::nintu=27;
const int XC::Twenty_Node_Brick::nenu=20;
double XC::Twenty_Node_Brick::shgu[4][20][27];
double XC::Twenty_Node_Brick::shlu[4][20][27];
double XC::Twenty_Node_Brick::wu[27];
double XC::Twenty_Node_Brick::dvolu[27];
}

//null constructor
XC::Twenty_Node_Brick::Twenty_Node_Brick(void)
  : Twenty_Node_Brick_Base(0, ELE_TAG_Twenty_Node_Brick,NDMaterialPhysicalProperties(27)),
  Ki(0)//, kc(0), rho(0)
  {
    // calculate local shape functions and derivatives
    compuLocalShapeFunction();
  }


//*********************************************************************
//full constructor
XC::Twenty_Node_Brick::Twenty_Node_Brick(int tag,int node1, int node2, int node3, int node4,
                                         int node5, int node6, int node7, int node8,
                                         int node9, int node10, int node11, int node12,
                                         int node13, int node14, int node15, int node16,
                                         int node17, int node18, int node19, int node20,
                                         NDMaterial &theMaterial, const BodyForces3D &bForces)
  : Twenty_Node_Brick_Base(tag, ELE_TAG_Twenty_Node_Brick,NDMaterialPhysicalProperties(27,&theMaterial)), bf(bForces),
  Ki(nullptr)//, kc(bulk), rho(rhof)
  {
    theNodes.set_id_nodes(node1 ,node2 ,node3 ,node4 ,node5 ,node6 ,node7 ,node8 ,node9 ,node10 ,node11 ,node12 ,node13 ,node14 ,node15 ,node16 ,node17 ,node18 ,node19 ,node20);

//  printf("b %15.6e %15.6e %15.6e \n", b1, b2,b3);
// calculate local shape functions and derivatives
    compuLocalShapeFunction();
  }
//******************************************************************

//! @brief Virtual constructor.
XC::Element* XC::Twenty_Node_Brick::getCopy(void) const
  { return new Twenty_Node_Brick(*this); }


//destructor
XC::Twenty_Node_Brick::~Twenty_Node_Brick(void)
  {
    if(Ki)
      {
        delete Ki;
        Ki= nullptr;
      }
  }


//set domain
void  XC::Twenty_Node_Brick::setDomain( Domain *theDomain )
  {
    Twenty_Node_Brick_Base::setDomain(theDomain);
    theNodes.checkNumDOF(3,getTag());
  }


//return number of dofs
int XC::Twenty_Node_Brick::getNumDOF(void) const
  { return 60; }


//print out element data
void  XC::Twenty_Node_Brick::Print( std::ostream &s, int flag )
{

        if(flag == 2) {

                s << "#20NodeBrick\n";

                int i;
                const int numNodes = 20;
                const int nstress = 6 ;

                for(i=0; i<numNodes; i++) {
                        const XC::Vector &nodeCrd = theNodes[i]->getCrds();
                        const XC::Vector &nodeDisp = theNodes[i]->getDisp();
                        s << "#NODE " << nodeCrd(0) << " " << nodeCrd(1) << " " << nodeCrd(2)
                                << " " << nodeDisp(0) << " " << nodeDisp(1) << " " << nodeDisp(2) << std::endl;
                }

                // spit out the section location & invoke print on the scetion
                static Vector avgStress(7);
                static Vector avgStrain(nstress);
                avgStress= physicalProperties.getCommittedAvgStress();
                avgStrain= physicalProperties.getCommittedAvgStrain();

                s << "#AVERAGE_STRESS ";
                for(i=0; i<7; i++)
                        s << avgStress(i) << " " ;
                s << std::endl;

                s << "#AVERAGE_STRAIN ";
                for(i=0; i<nstress; i++)
                        s << avgStrain(i) << " " ;
                s << std::endl;

                /*
                phisicalProperties.Print(s,flag);
                */

        } else {

                s << std::endl ;
                s << "20NodeBrick XC::Twenty_Node_Brick \n" ;
                s << "Element Number: " << this->getTag() << std::endl ;
                s << "Node 1 : " << theNodes.getTagNode(0) << std::endl ;
                s << "Node 2 : " << theNodes.getTagNode(1) << std::endl ;
                s << "Node 3 : " << theNodes.getTagNode(2) << std::endl ;
                s << "Node 4 : " << theNodes.getTagNode(3) << std::endl ;
                s << "Node 5 : " << theNodes.getTagNode(4) << std::endl ;
                s << "Node 6 : " << theNodes.getTagNode(5) << std::endl ;
                s << "Node 7 : " << theNodes.getTagNode(6) << std::endl ;
                s << "Node 8 : " << theNodes.getTagNode(7) << std::endl ;
                s << "Node 9 : " << theNodes.getTagNode(8) << std::endl ;
                s << "Node 10 : " << theNodes.getTagNode(9) << std::endl ;
                s << "Node 11 : " << theNodes.getTagNode(10) << std::endl ;
                s << "Node 12 : " << theNodes.getTagNode(11) << std::endl ;
                s << "Node 13 : " << theNodes.getTagNode(12) << std::endl ;
                s << "Node 14 : " << theNodes.getTagNode(13) << std::endl ;
                s << "Node 15 : " << theNodes.getTagNode(14) << std::endl ;
                s << "Node 16 : " << theNodes.getTagNode(15) << std::endl ;
                s << "Node 17 : " << theNodes.getTagNode(16) << std::endl ;
                s << "Node 18 : " << theNodes.getTagNode(17) << std::endl ;
                s << "Node 19 : " << theNodes.getTagNode(18) << std::endl ;
                s << "Node 20 : " << theNodes.getTagNode(19) << std::endl ;

                s << "Material XC::Information : \n " ;
                physicalProperties.Print( s, flag ) ;

                s << std::endl ;
        }
}

int
XC::Twenty_Node_Brick::update()
{
        int i, j;
        static double u[3][20];
        static double xsj;
        static XC::Matrix B(6, 3);
        double volume = 0.;

        for(i = 0; i < nenu; i++) {
             const XC::Vector &disp = theNodes[i]->getTrialDisp();
             u[0][i] = disp(0);
             u[1][i] = disp(1);
             u[2][i] = disp(2);
    }

        static XC::Vector eps(6);

        int ret = 0;

        //compute basis vectors and local nodal coordinates
        computeBasis( ) ;

        for( i = 0; i < nintu; i++ ) {
                // compute Jacobian and global shape functions
                Jacobian3d(i, xsj, 0);
                //volume element to also be saved
                dvolu[i] = wu[i] * xsj ;
                volume += dvolu[i];
        } // end for i
    //printf("volume = %f\n", volume);

        // Loop over the integration points
        for(i = 0; i < nintu; i++) {

                // Interpolate strains
                //eps = B*u;
                //eps.addMatrixVector(0.0, B, u, 1.0);
                eps.Zero();
                for( j = 0; j < nenu; j++) {


                        B(0,0) = shgu[0][j][i];
                        B(0,1) = 0.;
                        B(0,2) = 0.;
                        B(1,0) = 0.;
                        B(1,1) = shgu[1][j][i];
                        B(1,2) = 0.;
                        B(2,0) = 0.;
                        B(2,1) = 0.;
                        B(2,2) = shgu[2][j][i];
                        B(3,0) = shgu[1][j][i];
                        B(3,1) = shgu[0][j][i];
                        B(3,2) = 0.;
                        B(4,0) = 0.;
                        B(4,1) = shgu[2][j][i];
                        B(4,2) = shgu[1][j][i];
                        B(5,0) = shgu[2][j][i];
                        B(5,1) = 0.;
                        B(5,2) = shgu[0][j][i];

                        //BJ = computeB( j, shp ) ;

                        //nodal displacements
                        const XC::Vector &ul = theNodes[j]->getTrialDisp( ) ;
                        Vector ul3(3);
                        ul3(0) = ul(0);
                        ul3(1) = ul(1);
                        ul3(2) = ul(2);
                        //compute the strain
                        //strain += (BJ*ul) ;
                        eps.addMatrixVector(1.0,B,ul3,1.0 ) ;

                        /* for( k = 0; k < 6; k++) {
                        for( k1 = 0; k1 < 3; k1++) {
                        eps[k] += BJ(k, k1)*u[k1][j];
                        }
                }        */


                }

                // Set the material strain
                ret += physicalProperties[i]->setTrialStrain(eps);
        }

        return ret;
}

//return tangent stiffness matrix
const XC::Matrix &XC::Twenty_Node_Brick::getTangentStiff(void) const
  {
    static Matrix K;
    K= getStiff(1);
    if(isDead())
      K*=dead_srf;
    return K;
  }

// return initial stiffness matrix
const XC::Matrix &XC::Twenty_Node_Brick::getInitialStiff(void) const
  {
    static Matrix K;
    K= getStiff(0);
    if(isDead())
      K*=dead_srf;
    return K;
  }

// compute stiffness matrix
const XC::Matrix&  XC::Twenty_Node_Brick::getStiff(int flag) const
{
        if(flag != 0 && flag != 1) {
                std::cerr << "FATAL XC::Twenty_Node_Brick::getStiff() - illegal use\n";
                exit(-1);
        }

        if(flag == 0 && Ki != 0)
                return *Ki;

        int i, j ;

        static double xsj ;  // determinant jacaobian matrix
        double volume = 0.;
        //-------------------------------------------------------
        int j3, j3m1, j3m2;
        static XC::Matrix B(6,nenu*3);
        static XC::Matrix BTDB(nenu*3,nenu*3);
        static XC::Matrix D(6, 6);
        B.Zero();
        BTDB.Zero();
        stiff.Zero();
//                FILE *fp;
//                fp = fopen("stiff.dat","w");

        //compute basis vectors and local nodal coordinates
        computeBasis( ) ;

        for( i = 0; i < nintu; i++ ) {
                // compute Jacobian and global shape functions
                Jacobian3d(i, xsj, 0);
                //volume element to also be saved
                dvolu[i] = wu[i] * xsj ;
                volume += dvolu[i];
        } // end for i
    //printf("volume = %f\n", volume);

//        for( i = 0; i < nintu; i++ ) {
//                for(int j = 0; j < nenu; j++ ) {
//                        printf("%5d %5d %15.6e %15.6e %15.6e %15.6e\n", i,j,
//                                shgu[0][j][i], shgu[1][j][i], shgu[2][j][i], shgu[3][j][i]);
//                }
//        }
//        exit(-1);

        // Loop over the integration points
        for(i = 0; i < nintu; i++) {

                // Get the material tangent
                if( flag == 0 )
                        D = physicalProperties[i]->getInitialTangent();
                else
                        D = physicalProperties[i]->getTangent();
                //const Matrix &D = physicalProperties[i]->getTangent();


                for(j=0; j<nenu; j++) {

                        j3   = 3*j+2;
                        j3m1 = j3 - 1;
                        j3m2 = j3 - 2;

                        B(0,j3m2) = shgu[0][j][i];
                        B(0,j3m1) = 0.;
                        B(0,j3  ) = 0.;

                        B(1,j3m2) = 0.;
                        B(1,j3m1) = shgu[1][j][i];
                        B(1,j3  ) = 0.;

                        B(2,j3m2) = 0.;
                        B(2,j3m1) = 0.;
                        B(2,j3  ) = shgu[2][j][i];

                        B(3,j3m2) = shgu[1][j][i];
                        B(3,j3m1) = shgu[0][j][i];
                        B(3,j3  ) = 0.;

                        B(4,j3m2) = 0.;
                        B(4,j3m1) = shgu[2][j][i];
                        B(4,j3  ) = shgu[1][j][i];

                        B(5,j3m2) = shgu[2][j][i];
                        B(5,j3m1) = 0.;
                        B(5,j3  ) = shgu[0][j][i];

                }

                // Perform numerical integration
                //K = K + (B^ D * B) * intWt(i) * detJ;
                BTDB.addMatrixTripleProduct(1.0, B, D, dvolu[i]);
        }
    for( i = 0; i < 60; i++)
                for( j = 0; j < 60; j++)
                        stiff(i,j) = BTDB(i,j);

        if( flag == 1) {
                return stiff;
        }
        Ki = new Matrix(stiff);
        if(Ki == 0) {
                std::cerr << "FATAL XC::Twenty_Node_Brick::getStiff() -";
                std::cerr << "ran out of memory\n";
                exit(-1);
        }

        return *Ki;
}


//return mass matrix
const XC::Matrix&  XC::Twenty_Node_Brick::getMass(void) const
  {
    int tangFlag = 1 ;
    formInertiaTerms( tangFlag ) ;
    if(isDead())
      mass*=dead_srf;
    return mass ;
  }


//return mass matrix
const XC::Matrix&  XC::Twenty_Node_Brick::getDamp(void) const
  {
    int tangFlag = 1 ;
    formDampingTerms( tangFlag ) ;
    if(isDead())
      damp*=dead_srf;
    return damp ;
  }

void XC::Twenty_Node_Brick::formDampingTerms( int tangFlag ) const
  {
    //static double xsj ;  // determinant jacaobian matrix
        int i, j;
        //double volume = 0.;
        //zero damp
        damp.Zero( ) ;

        if(rayFactors.getBetaK() != 0.0)
          damp.addMatrix(1.0, this->getTangentStiff(), rayFactors.getBetaK());
        if(rayFactors.getBetaK0() != 0.0)
          damp.addMatrix(1.0, this->getInitialStiff(), rayFactors.getBetaK0());
        if(rayFactors.getBetaKc() != 0.0)
          damp.addMatrix(1.0, Kc, rayFactors.getBetaKc());

        if(rayFactors.getAlphaM() != 0.0)
          {
            this->getMass();
            for( i = 0; i < 60; i++)
              for( j = 0; j < 60; j++)
                damp(i,j) += mass(i,j) * rayFactors.getAlphaM();
          }

        return; /////////

}


int XC::Twenty_Node_Brick::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
        std::cerr << "XC::Twenty_Node_Brick::addLoad - load type unknown for truss with tag: " << this->getTag() << std::endl;
        return -1;
  }

int XC::Twenty_Node_Brick::addInertiaLoadToUnbalance(const XC::Vector &accel)
{
        static XC::Vector ra(60);
//        printf("calling addInertiaLoadToUnbalance()\n");
        ra.Zero();

        for(int i = 0;i < nenu;i++)
          {
            const XC::Vector &Raccel = theNodes[i]->getRV(accel);
            if(3 != Raccel.Size())
              {
                std::cerr << "XC::Twenty_Node_Brick::addInertiaLoadToUnbalance matrix and vector sizes are incompatible\n";
                return -1;
              }
            ra[i*3] = Raccel(0);
            ra[i*3+1] = Raccel(1);
            ra[i*3+2] = Raccel(2);
          }

        // Compute mass matrix
        int tangFlag = 1 ;
        formInertiaTerms( tangFlag ) ;

        // create the load vector if one does not exist
        if(load.isEmpty())
          load.reset(60);

        // add -M * RV(accel) to the load vector
        load.addMatrixVector(1.0, mass, ra, -1.0);
        //for( i = 0; i < 60; i++) {
        //        for( j = 0; j < 60; j++)
        //                                load(i) += -mass(i,j)*ra[j];
        //}

        return 0;
}


//get residual
const XC::Vector &XC::Twenty_Node_Brick::getResistingForce(void) const
  {
        int i, j, k, k1;
        double xsj;
        static XC::Matrix B(6, 3);
        double volume = 0.;

//        printf("calling getResistingForce()\n");
        resid.Zero();

        //compute basis vectors and local nodal coordinates
        computeBasis( ) ;
        //gauss loop to compute and save shape functions
        for( i = 0; i < nintu; i++ ) {
                // compute Jacobian and global shape functions
                Jacobian3d(i, xsj, 0);
                //volume element to also be saved
                dvolu[i] = wu[i] * xsj ;
                volume += dvolu[i];
        } // end for i
        //printf("volume = %f\n", volume);

        // Loop over the integration points
        for(i = 0; i < nintu; i++) {

                // Get material stress response
                const XC::Vector &sigma = physicalProperties[i]->getStress();

                // Perform numerical integration on internal force
                //P = P + (B^ sigma) * intWt(i)*intWt(j) * detJ;
                //P.addMatrixTransposeVector(1.0, B, sigma, intWt(i)*intWt(j)*detJ);
                for(j = 0; j < nenu; j++) {

                        B(0,0) = shgu[0][j][i];
                        B(0,1) = 0.;
                        B(0,2) = 0.;
                        B(1,0) = 0.;
                        B(1,1) = shgu[1][j][i];
                        B(1,2) = 0.;
                        B(2,0) = 0.;
                        B(2,1) = 0.;
                        B(2,2) = shgu[2][j][i];
                        B(3,0) = shgu[1][j][i];
                        B(3,1) = shgu[0][j][i];
                        B(3,2) = 0.;
                        B(4,0) = 0.;
                        B(4,1) = shgu[2][j][i];
                        B(4,2) = shgu[1][j][i];
                        B(5,0) = shgu[2][j][i];
                        B(5,1) = 0.;
                        B(5,2) = shgu[0][j][i];


                        for(k = 0; k < 3; k++) {
                                for(k1 = 0; k1 < 6; k1++)
                                        resid(j*3+k) += dvolu[i]*(B(k1,k)*sigma(k1));
                        }
                        // Subtract equiv. body forces from the nodes
                        //P = P - (N^ b) * intWt(i)*intWt(j) * detJ;
                        //P.addMatrixTransposeVector(1.0, N, b, -intWt(i)*intWt(j)*detJ);
                        double r = mixtureRho(i);
                        resid(j*3) -= dvolu[i]*(shgu[3][j][i]*r*bf[0]);
                        resid(j*3+1) -= dvolu[i]*(shgu[3][j][i]*r*bf[1]);
                        resid(j*3+2) -= dvolu[i]*(shgu[3][j][i]*r*bf[2]);
                }
        }

        // Subtract other external nodal loads ... P_res = P_int - P_ext
//        std::cerr<<"resid before:"<<resid<<std::endl;

        if(!load.isEmpty())
          resid-= load;

//        std::cerr<<"resid "<<resid<<std::endl;

    if(isDead())
      resid*=dead_srf;
    return resid ;
  }


//get residual with inertia terms
const XC::Vector &XC::Twenty_Node_Brick::getResistingForceIncInertia(void) const
  {
        static XC::Vector res(60);

//        printf("getResistingForceIncInertia()\n");

        int i, j;
        static double a[60];

        for(i=0; i<nenu; i++) {
                const XC::Vector &accel = theNodes[i]->getTrialAccel();
                if( 3 != accel.Size() ) {
                        std::cerr << "XC::Twenty_Node_Brick::getResistingForceIncInertia matrix and vector sizes are incompatible\n";
                        exit(-1);
                }

                a[i*3] = accel(0);
                a[i*3+1] = accel(1);
                a[i*3+2] = accel(2);
        }
        // Compute the current resisting force
        this->getResistingForce();
//        std::cerr<<"K "<<resid<<std::endl;

        // Compute the mass matrix
        this->getMass();

        for(i = 0; i < 60; i++) {
                for(j = 0; j < 60; j++){
                        resid(i) += mass(i,j)*a[j];
                }
        }
//        printf("\n");
        //std::cerr<<"K+M "<<P<<std::endl;


        for(i=0; i<nenu; i++) {
                const XC::Vector &vel = theNodes[i]->getTrialVel();
                if( 3!= vel.Size() ) {
                        std::cerr << "XC::Twenty_Node_Brick::getResistingForceIncInertia matrix and vector sizes are incompatible\n";
                        exit(-1);
                }
                a[i*3] = vel(0);
                a[i*3+1] = vel(1);
                a[i*3+2] = vel(2);
        }

        this->getDamp();

        for(i = 0; i < 60; i++) {
                for(j = 0; j < 60; j++) {
                        resid(i) += damp(i,j)*a[j];
                }
        }
//        std::cerr<<"Pd"<<Pd<<std::endl;

        res = resid;
//        std::cerr<<"res "<<res<<std::endl;

//        exit(-1);
    if(isDead())
      res*=dead_srf;
    return res;
  }


//*********************************************************************
//form inertia terms

void   XC::Twenty_Node_Brick::formInertiaTerms( int tangFlag ) const
{
        static double xsj ;  // determinant jacaobian matrix
        int i, j, k, m;
        double Nrho;

        //zero mass
        mass.Zero( ) ;

        //compute basis vectors and local nodal coordinates
        computeBasis( ) ;
        //gauss loop to compute and save shape functions
        for( i = 0; i < nintu; i++ ) {
                // compute Jacobian and global shape functions
                Jacobian3d(i, xsj, 0);
                //volume element to also be saved
                dvolu[i] = wu[i] * xsj ;
        } // end for i

        // Compute consistent mass matrix
        for(i = 0; i < nenu; i++) {
                for(j = 0; j < nenu; j++) {
                        for(m = 0; m < nintu; m++) {
                                Nrho = dvolu[m]*mixtureRho(m)*shgu[3][i][m]*shgu[3][j][m];
                                for( k = 0; k < 3; k++) {
                                        mass(i*3+k,j*3+k) += Nrho;
                                }
                        }
                }
        }

}


double XC::Twenty_Node_Brick::mixtureRho(int i) const
  {
        const double rhoi= physicalProperties[i]->getRho();
        //e = 0.7;  //physicalProperties[i]->getVoidRatio();
        //n = e / (1.0 + e);
        //return n * rho + (1.0-n) * rhoi;
        return rhoi;
  }

//************************************************************************
//compute local coordinates and basis

void   XC::Twenty_Node_Brick::computeBasis(void) const
{

        //nodal coordinates

        int i ;
        for( i = 0; i < nenu; i++ )
          {

                const XC::Vector &coorI = theNodes[i]->getCrds( ) ;

                xl[0][i] = coorI(0) ;
                xl[1][i] = coorI(1) ;
                xl[2][i] = coorI(2) ;

        }  //end for i

}

//! @brief Send object members through the channel being passed as parameter.
int XC::Twenty_Node_Brick::sendData(CommParameters &cp)
  {
    int res= Twenty_Node_Brick_Base::sendData(cp);
    res+= cp.sendDoubles(bf[0],bf[1],bf[2],getDbTagData(),CommMetaData(8));
    res+= cp.sendMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(11,12,13,14));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Twenty_Node_Brick::recvData(const CommParameters &cp)
  {
    int res= Twenty_Node_Brick_Base::recvData(cp);
    res+= cp.receiveDoubles(bf[0],bf[1],bf[2],getDbTagData(),CommMetaData(8));
    Ki= cp.receiveMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(11,12,13,14));
    return res;
  }//**********************************************************************

//! @brief Sends object through the channel being passed as parameter.
int XC::Twenty_Node_Brick::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(15);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Twenty_Node_Brick::recvSelf(const CommParameters &cp)
  {
    inicComm(15);
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


XC::Response *XC::Twenty_Node_Brick::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    if(argv[0] == "force" || argv[0] == "forces")
      return new ElementResponse(this, 1, resid);
    else if(argv[0] == "stiff" || argv[0] == "stiffness")
      return new ElementResponse(this, 2, stiff);
    else if(argv[0] == "mass")
      return new ElementResponse(this, 3, mass);
    else if(argv[0] == "damp")
      return new ElementResponse(this, 4, damp);
    else if(argv[0] == "material" || argv[0] == "integrPoint")
      {
        const int pointNum = atoi(argv[1]);
        if(pointNum > 0 && pointNum <= nintu)
          return setMaterialResponse(physicalProperties[pointNum-1],argv,2,eleInfo);
        else
          return 0;
      }
    else if(argv[0] == "stresses")
      { return new ElementResponse(this, 5, Vector(162)); }

    // otherwise response quantity is unknown for the XC::Twenty_Node_Brick class
    else
      return 0;
  }

int XC::Twenty_Node_Brick::getResponse(int responseID, Information &eleInfo)
  {
        static Vector stresses(162);

        if(responseID == 1)
                return eleInfo.setVector(this->getResistingForce());

        else if(responseID == 2)
                return eleInfo.setMatrix(this->getTangentStiff());

    else if(responseID == 3)
        return eleInfo.setMatrix(this->getMass());

    else if(responseID == 4)
        return eleInfo.setMatrix(this->getDamp());

        else if(responseID == 5) {

                // Loop over the integration points
                int cnt = 0;
                for(int i = 0; i < nintu; i++) {

                        // Get material stress response
                        const XC::Vector &sigma = physicalProperties[i]->getStress();
                        stresses(cnt++) = sigma(0);
                        stresses(cnt++) = sigma(1);
                        stresses(cnt++) = sigma(2);
                        stresses(cnt++) = sigma(3);
                        stresses(cnt++) = sigma(4);
                        stresses(cnt++) = sigma(5);
                }
                return eleInfo.setVector(stresses);

        }
        else

                return -1;
}

// calculate local shape functions
void XC::Twenty_Node_Brick::compuLocalShapeFunction()
  {

        int i, k, j;
        static double shl[4][20][27], w[27];
        // solid phase
        brcshl(shl, w, nintu, nenu);
        for(k = 0; k < nintu; k++) {
                wu[k] = w[k];
                for( j = 0; j < nenu; j++)
                        for( i = 0; i < 4; i++)
                                shlu[i][j][k] = shl[i][j][k];
        }

  }

void XC::Twenty_Node_Brick::Jacobian3d(int gaussPoint, double& xsj, int mode)
{
        int i, j, k, nint, nen;
        double rxsj, c1, c2, c3;
        static double xs[3][3];
        static double ad[3][3];
        static double shp[4][20];

        if( mode == 0 ) { // solid
                nint = nintu;
                nen = nenu;
        }
        else {
                std::cerr <<"XC::Twenty_Node_Brick::Jacobian3d - illegal mode: " << mode << "\n";
                exit(-1);
        } //end if

        for( j = 0; j < nen; j++) {
                for( i = 0; i < 4; i++) {
                        if( mode == 0 )
                                shp[i][j] = shlu[i][j][gaussPoint];
                        else {
                                std::cerr <<"XC::Twenty_Node_Brick::Jacobian3d - illegal mode: " << mode << "\n";
                                exit(-1);
                        } //end if
                }
        }




        //Compute jacobian transformation

        for( j=0; j<3; j++ ) {
                for( k = 0; k < 3; k++ ) {
                        xs[j][k] = 0;
                        for( i = 0; i < nen; i++ ) {
                                xs[j][k] += xl[j][i] * shp[k][i];
                        }
                }
        }


        //Compute adjoint to jacobian

        ad[0][0] = xs[1][1]*xs[2][2] - xs[1][2]*xs[2][1] ;
        ad[0][1] = xs[2][1]*xs[0][2] - xs[2][2]*xs[0][1] ;
        ad[0][2] = xs[0][1]*xs[1][2] - xs[0][2]*xs[1][1] ;

        ad[1][0] = xs[1][2]*xs[2][0] - xs[1][0]*xs[2][2] ;
        ad[1][1] = xs[2][2]*xs[0][0] - xs[2][0]*xs[0][2] ;
        ad[1][2] = xs[0][2]*xs[1][0] - xs[0][0]*xs[1][2] ;

        ad[2][0] = xs[1][0]*xs[2][1] - xs[1][1]*xs[2][0] ;
        ad[2][1] = xs[2][0]*xs[0][1] - xs[2][1]*xs[0][0] ;
        ad[2][2] = xs[0][0]*xs[1][1] - xs[0][1]*xs[1][0] ;

        //Compute determinant of jacobian

        xsj  = xs[0][0]*ad[0][0] + xs[0][1]*ad[1][0] + xs[0][2]*ad[2][0] ;
        if(xsj<=0) {
                std::cerr <<"XC::Twenty_Node_Brick::Jacobian3d - Non-positive Jacobian: " << xsj << "\n";
                for( i = 0; i < nen; i++ ) {
                        printf("%5d %15.6e %15.6e %15.6e %15.6e\n", i,
                                shp[0][i], shp[1][i], shp[2][i], shp[3][i]);
                }

                exit(-1);
        }

        rxsj = 1.0/xsj ;

        //Compute jacobian inverse

        for( j=0; j<3; j++ ) {

        for( i=0; i<3; i++ )
                        xs[i][j] = ad[i][j]*rxsj ;

        } //end for j


        //Compute derivatives with respect to global coords.

        for( k=0; k<nen; k++) {

                c1 = shp[0][k]*xs[0][0] + shp[1][k]*xs[1][0] + shp[2][k]*xs[2][0] ;
        c2 = shp[0][k]*xs[0][1] + shp[1][k]*xs[1][1] + shp[2][k]*xs[2][1] ;
        c3 = shp[0][k]*xs[0][2] + shp[1][k]*xs[1][2] + shp[2][k]*xs[2][2] ;

        shp[0][k] = c1 ;
        shp[1][k] = c2 ;
        shp[2][k] = c3 ;

        } //end for k

        for( j = 0; j < nen; j++) {
                for( i = 0; i < 4; i++) {
                        if( mode == 0 )
                                shgu[i][j][gaussPoint] = shp[i][j];
                        else {
                                std::cerr <<"XC::Twenty_Node_Brick::Jacobian3d - illegal mode: " << mode << "\n";
                                exit(-1);
                        } //end if
                }
        }


}


