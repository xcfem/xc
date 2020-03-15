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

// $Revision: 1.24 $
// $Date: 2006/01/10 18:41:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/brick/Brick.cpp,v $

// Ed "C++" Love
//
// Eight node Brick element
//

#include "Brick.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <domain/mesh/element/Element.h>
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/element/volumetric/brick/shp3d.h>
#include <utility/recorder/response/ElementResponse.h>

#include "utility/actor/actor/MatrixCommMetaData.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "domain/load/ElementalLoad.h"

//static data
const int XC::Brick::numberGauss; //!< Number of Gauss points.
const int XC::Brick::nShape;
//Gauss points coordinates.
double XC::Brick::gaussPoint[XC::Brick::numberGauss][XC::BrickBase::ndm]= {{-one_over_root3,-one_over_root3,-one_over_root3}, {one_over_root3,-one_over_root3,-one_over_root3}, {one_over_root3,one_over_root3,-one_over_root3}, {-one_over_root3,one_over_root3,-one_over_root3}, {-one_over_root3,-one_over_root3,+one_over_root3}, {one_over_root3,-one_over_root3,+one_over_root3}, {one_over_root3,one_over_root3,+one_over_root3}, {-one_over_root3,one_over_root3,+one_over_root3}};
double XC::Brick::dvol[XC::Brick::numberGauss];
double XC::Brick::shp[XC::Brick::nShape][XC::BrickBase::numberNodes];
double XC::Brick::Shape[XC::Brick::nShape][XC::BrickBase::numberNodes][XC::Brick::numberGauss];


XC::Matrix  XC::Brick::stiff(24,24) ;
XC::Vector  XC::Brick::resid(24) ;
XC::Matrix  XC::Brick::mass(24,24) ;


//quadrature data

const double XC::Brick::wg[] = { 1.0, 1.0, 1.0, 1.0,
                              1.0, 1.0, 1.0, 1.0  } ;


static XC::Matrix B(6,3) ;

const int brick_nstress= 6;

//! @brief Default constructor
XC::Brick::Brick(void)
  :BrickBase(ELE_TAG_Brick), applyLoad(false), Ki(nullptr)
  {
    B.Zero();
  }


//! @brief full constructor
XC::Brick::Brick(int tag,const NDMaterial *ptr_mat)
  :BrickBase(tag, ELE_TAG_Brick,NDMaterialPhysicalProperties(8,ptr_mat)), applyLoad(false), Ki(nullptr)
  {
    B.Zero();
  }

//! @brief full constructor
XC::Brick::Brick( int tag, int node1,int node2,int node3,int node4,int node5,int node6,int node7,int node8, NDMaterial &theMaterial,const BodyForces3D &bForces)
  :BrickBase(tag,ELE_TAG_Brick,node1,node2,node3,node4,node5,node6,node7,node8,NDMaterialPhysicalProperties(8,&theMaterial)), bf(bForces), applyLoad(false), Ki(nullptr)
  {
    B.Zero();
  }

//! @brief Virtual constructor.
XC::Element* XC::Brick::getCopy(void) const
  { return new Brick(*this); }


//! @brief destructor
XC::Brick::~Brick(void)
  {
    if(Ki)
      {
        delete Ki;
        Ki= nullptr;
      }
  }


//! @brief set domain
void  XC::Brick::setDomain(Domain *theDomain)
  {
    BrickBase::setDomain(theDomain);
  }

//! @brief return number of dofs
int XC::Brick::getNumDOF(void) const
  { return 24 ; }

//! @brief Return the average stress in the element.
XC::Vector XC::Brick::getAvgStress(void) const
  { return physicalProperties.getCommittedAvgStress(); }

//! Returns the index of the (i,j) component of the strain tensor
//! (resp. stress tensor) in the vector that stores these components.
size_t XC::Brick::getVectorIndex(const size_t &i,const size_t &j)
  {
    size_t retval= 0;
    if(i<=j)
      {
        if(i==j)
          retval= (i-1);
        else if(i==1)
          retval= (j==2) ? 3 : 5;
        else
          retval= 4;
      }
    else
      retval= getVectorIndex(j,i);
    return retval;
  }

//! @brief Return the (i,j) component (i,j) of the average stress
//! in the element.
double XC::Brick::getAvgStress(const size_t &i,const size_t &j) const
  {
    const size_t iComp= getVectorIndex(i,j);
    return physicalProperties.getCommittedAvgStress(iComp);
  }

//! @brief Return the average strain in the element.
XC::Vector XC::Brick::getAvgStrain(void) const
  { return physicalProperties.getCommittedAvgStrain(); }

//! @brief Return the (i,j) component of the average strain in the element.
double XC::Brick::getAvgStrain(const size_t &i,const size_t &j) const
  {
    const size_t iComp= getVectorIndex(i,j);
    return physicalProperties.getCommittedAvgStress(iComp);
  }



//! @brief Return stiffness matrix
const XC::Matrix &XC::Brick::getTangentStiff(void) const
  {
    int tang_flag = 1; //get the tangent
    //do tangent and residual here
    formResidAndTangent( tang_flag );
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }

//! @brief Return the coordinates of the Gauss points.
XC::Matrix XC::Brick::getGaussPointsPositions(void) const
  {
    Matrix retval(numberGauss,ndm);
    for(int i= 0;i<numberGauss; i++)
      {
        retval(i,0)= gaussPoint[i][0];
        retval(i,1)= gaussPoint[i][1];
        retval(i,2)= gaussPoint[i][2];
      }
    return retval;
  }

//! @brief Return the values of the shape functions.
XC::Vector XC::Brick::getShapeFunctionsValues(const double &r, const double &s, const double &t) const
  {
    Vector retval(8);
    double natural_coord[3];
    natural_coord[0]= r;
    natural_coord[1]= s;
    natural_coord[2]= t;
    double xsj= 0.0; // determinant jacobian matrix at point (r,s,t)
    //get shape functions values
    shp3d(natural_coord, xsj, shp, xl);
    for(int i= 0;i<8;i++)
      retval[i]= shp[3][i];
    return retval;
  }

//! @brief Gauss loop to compute and save shape functions
void XC::Brick::shape_functions_loop(void) const
  {
    //compute basis vectors and local nodal coordinates
    computeBasis();

    double xsj= 0.0;  // determinant jacobian matrix
    int count = 0; //Gauss point index
    for(int i= 0; i < 2; i++ )
      {
	for(int j= 0; j < 2; j++ )
	  {
	    for(int k= 0; k < 2; k++ )
	      {
		//get shape functions values at Gauss points
		shp3d( gaussPoint[count], xsj, shp, xl);

		//save shape functions
		for(int p = 0; p < nShape; p++ )
		  {
		    for(int q = 0; q < numberNodes; q++ )
		      Shape[p][q][count] = shp[p][q];
		  } // end for p

		//volume element to also be saved
		dvol[count] = wg[count] * xsj;
		//volume+= dvol[count];
		count++;
	      } //end for k
	  } //end for j
      } // end for i
  }

//! @brief Return initial stiffness matrix.
const XC::Matrix &XC::Brick::getInitialStiff(void) const
  {
    if(!Ki)
      {
	//strains ordered : eps11, eps22, eps33, 2*eps12, 2*eps23, 2*eps31

	static Vector strain(brick_nstress);  //strain
	static Matrix stiffJK(ndf,ndf); //nodeJK stiffness
	static Matrix dd(brick_nstress,brick_nstress);  //material tangent


	//---------B-matrices------------------------------------

	static Matrix BJ(brick_nstress,ndf);      // B matrix node J
	static Matrix BJtran(ndf,brick_nstress);
	static Matrix BK(brick_nstress,ndf);      // B matrix node k
	static Matrix BJtranD(ndf,brick_nstress);

	//-------------------------------------------------------


	//zero stiffness and residual
	stiff.Zero( );

	//gauss loop to compute and save shape functions
	shape_functions_loop();

	//gauss loop
	for(int i= 0; i < numberGauss; i++ )
	  {
	    //extract shape functions from saved array
	    for(int p = 0; p < nShape; p++ )
	      {
	         for(int q = 0; q < numberNodes; q++ )
		    shp[p][q]= Shape[p][q][i];
	      } // end for p

	    dd= physicalProperties[i]->getInitialTangent( );
	    dd*= dvol[i];

	    int jj= 0;
	    for(int j= 0; j < numberNodes; j++ )
	      {

		BJ = computeB( j, shp );

		//transpose
		//BJtran = transpose( brick_nstress, ndf, BJ );
		for(int p=0; p<ndf; p++)
		  {
		    for(int q=0; q<brick_nstress; q++)
		      BJtran(p,q) = BJ(q,p);
		  }//end for p

		//BJtranD = BJtran * dd;
		BJtranD.addMatrixProduct(0.0,  BJtran, dd, 1.0);

		int kk= 0;
		for(int k= 0; k < numberNodes; k++ )
		  {
		    BK = computeB( k, shp );
		    //stiffJK =  BJtranD * BK ;
		    stiffJK.addMatrixProduct(0.0,  BJtranD, BK, 1.0);
		    for(int p = 0; p < ndf; p++ )
		      {
			for(int q = 0; q < ndf; q++ )
			  stiff( jj+p, kk+q )+= stiffJK( p, q );
		      } //end for p
		    kk+= ndf;
		  } // end for k loop
		jj+= ndf;
	      } // end for j loop
  	  } //end for i gauss loop

	  Ki= new Matrix(stiff);
        }
      if(isDead())
        stiff*=dead_srf;
    return stiff;
  }


//! @brief Return mass matrix.
const XC::Matrix &XC::Brick::getMass(void) const
  {
    int tangFlag = 1;
    formInertiaTerms(tangFlag);
    if(isDead())
      mass*=dead_srf;
    return mass;
  }

//! Remove element loads. 
void XC::Brick::zeroLoad(void)
  {
    BrickBase::zeroLoad();
    bf[0]= 0.0;
    bf[1]= 0.0;
    bf[2]= 0.0;
    applyLoad= false;

    appliedB[0]= 0.0;
    appliedB[1]= 0.0;
    appliedB[2]= 0.0;
  }

int XC::Brick::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    int retval= -1;
    int type;
    const Vector &data= theLoad->getData(type, loadFactor);

    if(type==LOAD_TAG_BrickSelfWeight)
      {
	applyLoad= true;
	appliedB[0]+= loadFactor*bf[0];
	appliedB[1]+= loadFactor*bf[1];
	appliedB[2]+= loadFactor*bf[2];
        retval= 0;
      }
    else if(type==LOAD_TAG_SelfWeight)
      {
	// added compatibility with selfWeight class implemented for all continuum elements, C.McGann, U.W.
	applyLoad= true;
	appliedB[0]+= loadFactor*data(0)*bf[0];
	appliedB[1]+= loadFactor*data(1)*bf[1];
	appliedB[2]+= loadFactor*data(2)*bf[2];
	retval= 0;
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; ele with tag: " << this->getTag()
		  << " does not deal with load type: " << type << "\n";
        retval= -1;
      }
    return retval;
  }

//! @brief Add inertia load due to acceleration argument.
int XC::Brick::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // check to see if have mass
    bool haveRho= physicalProperties.haveRho();
    int retval= 0;
    if(haveRho)
      {
	// Compute mass matrix
	int tangFlag = 1;
	formInertiaTerms( tangFlag );

	// store computed RV from nodes in resid vector
	int count = 0;
	for(int i=0; i<numberNodes; i++)
	  {
	    const Vector &Raccel = theNodes[i]->getRV(accel);
	    for(int j=0; j<ndf; j++)
	      resid(count++) = Raccel(j);
	  }

	// create the load vector if one does not exist
	if(load.isEmpty())
	  load.reset(numberNodes*ndf);
	// add -M * RV(accel) to the load vector
	load.addMatrixVector(1.0, mass, resid, -1.0);
      }
    return retval;
  }


//! @brief Get residual
const XC::Vector &XC::Brick::getResistingForce(void) const
  {
    int tang_flag = 0; //don't get the tangent
    formResidAndTangent( tang_flag );
    if(!load.isEmpty())
      resid-= load;
    if(isDead())
      resid*=dead_srf;
    return resid;
  }


//! @brief Get residual with inertia terms.
const XC::Vector &XC::Brick::getResistingForceIncInertia(void) const
  {
    static Vector res(24);

    int tang_flag = 0; //don't get the tangent

    //do tangent and residual here
    formResidAndTangent( tang_flag );
    formInertiaTerms( tang_flag );

    res = resid;

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      res+= this->getRayleighDampingForces();

    if(!load.isEmpty())
      res-= load;
    if(isDead())
      res*=dead_srf;
    return res;
  }


//! @brief Form inertia terms
void XC::Brick::formInertiaTerms( int tangFlag ) const
  {
    static const int massIndex = nShape - 1;

    static Vector momentum(ndf);

    double temp, rho, massJK;

    //zero mass
    mass.Zero( );

    //gauss loop to compute and save shape functions
    shape_functions_loop();

    //gauss loop
    for(int i= 0; i < numberGauss; i++ )
      {

        //extract shape functions from saved array
        for(int p = 0; p < nShape; p++ )
	  {
	    for(int q = 0; q < numberNodes; q++ )
	      shp[p][q]  = Shape[p][q][i];
          } // end for p

      //node loop to compute acceleration
      momentum.Zero( );
      for(int j= 0; j < numberNodes; j++ )
	//momentum+= shp[massIndex][j] * ( theNodes[j]->getTrialAccel()  );
	momentum.addVector( 1.0,
			    theNodes[j]->getTrialAccel(),
			    shp[massIndex][j] );


      //density
      rho = physicalProperties[i]->getRho();


      //multiply acceleration by density to form momentum
      momentum*= rho;


      //residual and tangent calculations node loops
      int jj= 0;
      for(int j= 0; j < numberNodes; j++ )
	{

	temp = shp[massIndex][j] * dvol[i];

	for(int p = 0; p < ndf; p++ )
	  resid(jj+p )+= ( temp * momentum(p) ) ;


	if(tangFlag == 1 )
	  {
	    //multiply by density
	    temp *= rho;

	    //node-node mass
	    int kk= 0;
	    for(int k= 0; k < numberNodes; k++ )
	      {
		 massJK = temp * shp[massIndex][k];
		 for(int p = 0; p < ndf; p++ )
		   mass(jj+p, kk+p )+= massJK;
		 kk+= ndf;
	       } // end for k loop
	  } // end if tang_flag

	jj+= ndf;
      } // end for j loop


    } //end for i gauss loop

  }

//! @brief Form residual and tangent
int XC::Brick::update(void)
  {

    //strains ordered : eps11, eps22, eps33, 2*eps12, 2*eps23, 2*eps31

    int success;

    static Vector strain(brick_nstress);  //strain

    //---------B-matrices------------------------------------
    static Matrix BJ(brick_nstress,ndf);      // B matrix node J
    static Matrix BJtran(ndf,brick_nstress);
    static Matrix BK(brick_nstress,ndf);      // B matrix node k
    static Matrix BJtranD(ndf,brick_nstress);
    //-------------------------------------------------------


    //gauss loop to compute and save shape functions
    shape_functions_loop();

    //gauss loop
    for(int i= 0; i < numberGauss; i++ )
      {

	//extract shape functions from saved array
	for(int p = 0; p < nShape; p++ )
	  {
	    for(int q = 0; q < numberNodes; q++ )
	      shp[p][q]  = Shape[p][q][i];
	  } // end for p


	//zero the strains
	strain.Zero( );

	// j-node loop to compute strain
	for(int j= 0; j < numberNodes; j++ )
	  {
	    /**************** fmk - unwinding for performance
	    //compute B matrix
	    BJ = computeB( j, shp );

	    //nodal displacements
	    const XC::Vector &ul = theNodes[j]->getTrialDisp( );

	    //compute the strain
	    //strain+= (BJ*ul);
	    strain.addMatrixVector(1.0,  BJ,ul,1.0 );
	    ***************************************************/


	    //               | N,1      0     0    |
	    //   B       =   |   0     N,2    0    |
	    //               |   0      0     N,3  |   (6x3)
	    //               | N,2     N,1     0   |
	    //               |   0     N,3    N,2  |
	    //               | N,3      0     N,1  |

	    //      B(0,0) = shp[0][node];
	    //      B(1,1) = shp[1][node];
	    //      B(2,2) = shp[2][node];
	    //      B(3,0) = shp[1][node];
	    //      B(3,1) = shp[0][node];
	    //      B(4,1) = shp[2][node];
	    //      B(4,2) = shp[1][node];
	    //      B(5,0) = shp[2][node];
	    //      B(5,2) = shp[0][node];

	    const double b00 = shp[0][j];
	    const double b11 = shp[1][j];
	    const double b22 = shp[2][j];
	    const double b30 = shp[1][j];
	    const double b31 = shp[0][j];
	    const double b41 = shp[2][j];
	    const double b42 = shp[1][j];
	    const double b50 = shp[2][j];
	    const double b52 = shp[0][j];

	    const Vector &ul = theNodes[j]->getTrialDisp();

	    const double ul0 = ul(0);
	    const double ul1 = ul(1);
	    const double ul2 = ul(2);

	    strain(0)+= b00 * ul0;
	    strain(1)+= b11 * ul1;
	    strain(2)+= b22 * ul2;
	    strain(3)+= b30 * ul0 + b31 * ul1;
	    strain(4)+= b41 * ul1 + b42 * ul2;
	    strain(5)+= b50 * ul0 + b52 * ul2;
	  } // end for j
	//send the strain to the material
	success= physicalProperties[i]->setTrialStrain( strain );
      } //end for i gauss loop
    return 0;
  }


//form residual and tangent
void  XC::Brick::formResidAndTangent( int tang_flag ) const
  {

    //strains ordered : eps11, eps22, eps33, 2*eps12, 2*eps23, 2*eps31

    //int success;

    static Vector residJ(ndf); //nodeJ residual
    static Matrix stiffJK(ndf,ndf); //nodeJK stiffness
    static Vector stress(brick_nstress);  //stress
    static Matrix dd(brick_nstress,brick_nstress);  //material tangent


    //---------B-matrices------------------------------------

      static Matrix BJ(brick_nstress,ndf);      // B matrix node J
      static Matrix BJtran(ndf,brick_nstress);
      static Matrix BK(brick_nstress,ndf);      // B matrix node k
      static Matrix BJtranD(ndf,brick_nstress);

    //-------------------------------------------------------


    //zero stiffness and residual
    stiff.Zero( );
    resid.Zero( );

    //gauss loop to compute and save shape functions
    shape_functions_loop();

    //gauss loop
    for(int i= 0; i < numberGauss; i++ )
      {
      //extract shape functions from saved array
      for(int p = 0; p < nShape; p++ )
	{
	  for(int q = 0; q < numberNodes; q++ )
	     shp[p][q]  = Shape[p][q][i];
	} // end for p


      //compute the stress
      stress = physicalProperties[i]->getStress( );


      //multiply by volume element
      stress  *= dvol[i];

      if(tang_flag == 1 )
	{
	  dd = physicalProperties[i]->getTangent( );
	  dd *= dvol[i];
	} //end if tang_flag


      const double stress0= stress(0);
      const double stress1= stress(1);
      const double stress2= stress(2);
      const double stress3= stress(3);
      const double stress4= stress(4);
      const double stress5= stress(5);

      //residual and tangent calculations node loops

      int jj= 0;
      for(int j= 0; j < numberNodes; j++ )
	{

	  /* ************** fmk - unwinding for performance
	  ************************************************* */

	  //               | N,1      0     0    |
	  //   B       =   |   0     N,2    0    |
	  //               |   0      0     N,3  |   (6x3)
	  //               | N,2     N,1     0   |
	  //               |   0     N,3    N,2  |
	  //               | N,3      0     N,1  |

	  //      B(0,0) = shp[0][node];
	  //      B(1,1) = shp[1][node];
	  //      B(2,2) = shp[2][node];
	  //      B(3,0) = shp[1][node];
	  //      B(3,1) = shp[0][node];
	  //      B(4,1) = shp[2][node];
	  //      B(4,2) = shp[1][node];
	  //      B(5,0) = shp[2][node];
	  //      B(5,2) = shp[0][node];

	  const double b00 = shp[0][j];
	  const double b11 = shp[1][j];
	  const double b22 = shp[2][j];
	  const double b30 = shp[1][j];
	  const double b31 = shp[0][j];
	  const double b41 = shp[2][j];
	  const double b42 = shp[1][j];
	  const double b50 = shp[2][j];
	  const double b52 = shp[0][j];

	  residJ(0) = b00 * stress0 + b30 * stress3 + b50 * stress5;
	  residJ(1) = b11 * stress1 + b31 * stress3 + b41 * stress4;
	  residJ(2) = b22 * stress2 + b42 * stress4 + b52 * stress5;

	  BJ = computeB( j, shp );

	  //transpose
	  //BJtran = transpose( brick_nstress, ndf, BJ );
	  for(int p=0; p<ndf; p++)
	    {
	      for(int q=0; q<brick_nstress; q++)
		BJtran(p,q) = BJ(q,p);
	    }//end for p


	  //residual
	  for(int p = 0; p < ndf; p++ )
	    {
	      resid(jj + p )+= residJ(p) ;
		resid(jj + p ) -= dvol[i]*bf[p]*shp[3][j];
	    }

	  if(tang_flag == 1 )
	    {
	      //BJtranD = BJtran * dd;
	      BJtranD.addMatrixProduct(0.0,  BJtran,dd,1.0);

	      int kk= 0;
	      for(int k= 0; k < numberNodes; k++ )
		{
		   BK = computeB( k, shp );

		   //stiffJK =  BJtranD * BK ;
		   stiffJK.addMatrixProduct(0.0,  BJtranD,BK,1.0);

		   for(int p = 0; p < ndf; p++ )
		     {
		       for(int q = 0; q < ndf; q++ )
			 stiff(jj+p, kk+q )+= stiffJK( p, q );
		     } //end for p
		   kk+= ndf;
		} // end for k loop
	    } // end if tang_flag
	  jj+= ndf;
      } // end for j loop


    } //end for i gauss loop
  }

//! @brief Compute B matrix.
const XC::Matrix &XC::Brick::computeB( int node, const double shp[4][8]) const
  {
    //---B matrix in standard {1,2,3} mechanics notation---------
    //
    //                -                   -
    //               | N,1      0     0    |
    //   B       =   |   0     N,2    0    |
    //               |   0      0     N,3  |   (6x3)
    //               | N,2     N,1     0   |
    //               |   0     N,3    N,2  |
    //               | N,3      0     N,1  |
    //                -                   -
    //
    //-------------------------------------------------------------------

      B(0,0) = shp[0][node];
      B(1,1) = shp[1][node];
      B(2,2) = shp[2][node];

      B(3,0) = shp[1][node];
      B(3,1) = shp[0][node];

      B(4,1) = shp[2][node];
      B(4,2) = shp[1][node];

      B(5,0) = shp[2][node];
      B(5,2) = shp[0][node];

      return B;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::Brick::sendData(CommParameters &cp)
  {
    int res= BrickBase::sendData(cp);
    res+= cp.sendDoubles(bf[0],bf[1],bf[2],getDbTagData(),CommMetaData(8));
    res+= cp.sendMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(11,12,13,14));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::Brick::recvData(const CommParameters &cp)
  {
    int res= BrickBase::recvData(cp);
    res+= cp.receiveDoubles(bf[0],bf[1],bf[2],getDbTagData(),CommMetaData(8));
    Ki= cp.receiveMatrixPtr(Ki,getDbTagData(),MatrixCommMetaData(11,12,13,14));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Brick::sendSelf(CommParameters &cp)
  {
    inicComm(15);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; could not send data.\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Brick::recvSelf(const CommParameters &cp)
  {
    const int dataTag= getDbTag();
    ID data(15);
    int res = cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data.\n";
    else
      res+= recvData(cp);
    return res;
  }


XC::Response *XC::Brick::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
{
  if(argv[0] == "force" || argv[0] == "forces")
    return new ElementResponse(this, 1, resid);

  else if(argv[0] == "stiff" || argv[0] == "stiffness")
    return new ElementResponse(this, 2, stiff);

  else if(argv[0] == "material" || argv[0] == "integrPoint") {
    int pointNum = atoi(argv[1]);
    if(pointNum > 0 && pointNum <= 8)
      return setMaterialResponse(physicalProperties[pointNum-1],argv,2,eleInfo);
    else
      return 0;
  } else if(argv[0] == "stresses") {
    return new ElementResponse(this, 3, Vector(48));
  }

  // otherwise response quantity is unknown for the brick class
  else
    return 0;
}

int XC::Brick::getResponse(int responseID, Information &eleInfo)
  {
    static XC::Vector stresses(48);
    if(responseID == 1)
      return eleInfo.setVector(this->getResistingForce());
    else if(responseID == 2)
      return eleInfo.setMatrix(this->getTangentStiff());
    else if(responseID == 3)
      {
        // Loop over the integration points
        int cnt = 0;
        for(int i = 0; i < 8; i++)
          {
            // Get material stress response
            const Vector &sigma = physicalProperties[i]->getStress();
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

//! @brief Print out element data
void XC::Brick::Print(std::ostream &s, int flag ) const
  {
    if(flag == 2)
      {

	s << "#Brick\n";

	const int numNodes = 8;

	for(int i=0; i<numNodes; i++)
	  {
	    const Vector &nodeCrd = theNodes[i]->getCrds();
	    const Vector &nodeDisp = theNodes[i]->getDisp();
	    s << "#NODE " << nodeCrd(0) << " " << nodeCrd(1) << " " << nodeCrd(2)
	      << " " << nodeDisp(0) << " " << nodeDisp(1) << " " << nodeDisp(2) << std::endl;
	 }

	// spit out the section location & invoke print on the scetion
	static Vector avgStress(brick_nstress);
	static Vector avgStrain(brick_nstress);
	avgStress= physicalProperties.getCommittedAvgStress();
	avgStrain= physicalProperties.getCommittedAvgStrain();

	s << "#AVERAGE_STRESS ";
	for(int i=0; i<brick_nstress; i++)
	  s << avgStress(i) << " ";
	s << std::endl;

	s << "#AVERAGE_STRAIN ";
	for(int i=0; i<brick_nstress; i++)
	  s << avgStrain(i) << " ";
	s << std::endl;

	/*
	physicalProperties.Print(s,flag);
	*/

      }
    else
      {
	s << "Standard Eight Node Brick \n";
	s << "Element Number: " << this->getTag() << std::endl;
	s << "Nodes: " << theNodes;

	s << "Material Information : \n ";
	physicalProperties.Print( s, flag );

	s << std::endl;
	s << this->getTag() << " " << theNodes.getTagNode(0)
  	                    << " " << theNodes.getTagNode(1)
			    << " " << theNodes.getTagNode(2)
			    << " " << theNodes.getTagNode(3)
			    << " " << theNodes.getTagNode(4)
			    << " " << theNodes.getTagNode(5)
			    << " " << theNodes.getTagNode(6)
			    << " " << theNodes.getTagNode(7)
	                    << std::endl;

	s << "Resisting Force (no inertia): " << this->getResistingForce();
      }
  }
