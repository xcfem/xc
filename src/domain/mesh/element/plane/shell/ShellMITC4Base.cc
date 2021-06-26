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
//ShellMITC4Base.cpp

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "ShellMITC4Base.h"
#include "utility/matrix/ID.h"
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "preprocessor/multi_block_topology/aux_meshing.h"
#include <domain/mesh/node/Node.h>
#include <material/section/SectionForceDeformation.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/element/utils/coordTransformation/R3vectors.h>


#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/actor/MovableMatrix.h"
#include "utility/actor/actor/MovableVectors.h"


#include "utility/geom/d2/Polygon3d.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/prep_handlers/LoadHandler.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "domain/load/plane/ShellMecLoad.h"


//static data
XC::ShellBData XC::ShellMITC4Base::BData;

//! @brief Constructor
XC::ShellMITC4Base::ShellMITC4Base(int classTag, const ShellCrdTransf3dBase *crdTransf)
  : Shell4NBase(classTag,crdTransf), Ktt(0.0)
  { }

//! @brief Constructor
//!
//! @param tag: element identifier.
//! @param classTag: class identifier.
//! @param ptr_mat: pointer to the material object.
//! @param crdTransf: coordinate transformation.
XC::ShellMITC4Base::ShellMITC4Base(int tag, int classTag,const SectionForceDeformation *ptr_mat, const ShellCrdTransf3dBase *crdTransf)
  : Shell4NBase(tag,classTag,ptr_mat,crdTransf), Ktt(0.0)
  { }

//! @brief Constructor
XC::ShellMITC4Base::ShellMITC4Base(int tag, int classTag,int node1,int node2,int node3,int node4,const SectionFDPhysicalProperties &physProp, const ShellCrdTransf3dBase *crdTransf)
  : Shell4NBase(tag,node1,node2,node3,node4,classTag,physProp,crdTransf), Ktt(0.0)
  { }

//! @brief Set the element domain.
void XC::ShellMITC4Base::setDomain(Domain *theDomain)
  {
    Shell4NBase::setDomain(theDomain);

    static Vector eig(3);
    static Matrix ddMembrane(3,3);

    //compute drilling stiffness penalty parameter
    const Matrix &dd= physicalProperties[0]->getInitialTangent();

    //assemble ddMembrane;
    for(int i= 0;i<3;i++)
      {
        for(int j= 0;j < 3;j++)
          ddMembrane(i,j)= dd(i,j);
      } //end for i

    //eigenvalues of ddMembrane
    eig= LovelyEig( ddMembrane );

    //set ktt
    //Ktt= dd(2,2);  //shear modulus
    Ktt= std::min( eig(2), std::min( eig(0), eig(1) ) );
    //Ktt= dd(2,2);

    //basis vectors and local coordinates
    computeBasis(); 
  }

//! @brief Reactivates the element.
void XC::ShellMITC4Base::alive(void)
  {
    if(isDead())
      {
        Shell4NBase::alive();
        Ki.Zero();
        revertToStart(); //Eliminate possible strains and stresses
	                 //on the element (melt and then solidify).
        catchInitDisp(); //Node displacements at element activation.
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; trying to activate an element already active."
                << std::endl;
  }

//! @brief Computes the matrix G.
const XC::Matrix XC::ShellMITC4Base::calculateG(void) const
  {
    const double dx34= xl[0][2]-xl[0][3];
    const double dy34= xl[1][2]-xl[1][3];

    const double dx21= xl[0][1]-xl[0][0];
    const double dy21= xl[1][1]-xl[1][0];

    const double dx32= xl[0][2]-xl[0][1];
    const double dy32= xl[1][2]-xl[1][1];

    const double dx41= xl[0][3]-xl[0][0];
    const double dy41= xl[1][3]-xl[1][0];

    Matrix G(4,12);
    G.Zero();
    double one_over_four= 0.25;
    G(0,0)=-0.5;
    G(0,1)=-dy41*one_over_four;
    G(0,2)=dx41*one_over_four;
    G(0,9)=0.5;
    G(0,10)=-dy41*one_over_four;
    G(0,11)=dx41*one_over_four;
    G(1,0)=-0.5;
    G(1,1)=-dy21*one_over_four;
    G(1,2)=dx21*one_over_four;
    G(1,3)=0.5;
    G(1,4)=-dy21*one_over_four;
    G(1,5)=dx21*one_over_four;
    G(2,3)=-0.5;
    G(2,4)=-dy32*one_over_four;
    G(2,5)=dx32*one_over_four;
    G(2,6)=0.5;
    G(2,7)=-dy32*one_over_four;
    G(2,8)=dx32*one_over_four;
    G(3,6)=0.5;
    G(3,7)=-dy34*one_over_four;
    G(3,8)=dx34*one_over_four;
    G(3,9)=-0.5;
    G(3,10)=-dy34*one_over_four;
    G(3,11)=dx34*one_over_four;
    return G;
  }

//! @brief return secant matrix
const XC::Matrix &XC::ShellMITC4Base::getInitialStiff(void) const
  {
    if(!Ki.isEmpty())
      return Ki;

    static const int ndf= 6; //two membrane plus three bending plus one drill
    static const int nstress= 8; //three membrane, three moment, two shear
    static const int ngauss= 4;
    static const int numnodes= 4;

    int i,  j,  k, p, q;
    int jj, kk;

    double volume= 0.0;

    static double xsj;  // determinant of the jacobian matrix 
    static double dvol[ngauss]; //volume element
    static double shp[3][numnodes];  //shape functions at a gauss point

    //  static double Shape[3][numnodes][ngauss]; //all the shape functions

    static Matrix stiffJK(ndf,ndf); //nodeJK stiffness 
    static Matrix dd(nstress,nstress);  //material tangent
    static Matrix J0(2,2);  //Jacobian at center
    static Matrix J0inv(2,2); //inverse of Jacobian at center

    //---------B-matrices------------------------------------
    static Matrix BJ(nstress,ndf);      // B matrix node J
    static Matrix BJtran(ndf,nstress);
    static Matrix BK(nstress,ndf);      // B matrix node k
    static Matrix BJtranD(ndf,nstress);
    static Matrix Bbend(3,3);  // bending B matrix
    static Matrix Bshear(2,3); // shear B matrix
    static Matrix Bmembrane(3,2); // membrane B matrix
    static double BdrillJ[ndf]; //drill B matrix
    static double BdrillK[ndf];  

    double *drillPointer;

    static double saveB[nstress][ndf][numnodes];

    //-------------------------------------------------------

    stiff.Zero( );
 
    Matrix G= calculateG();


    Matrix Ms(2,4);
    Ms.Zero();
    Matrix Bsv(2,12);
    Bsv.Zero();

    const double Ax= -xl[0][0]+xl[0][1]+xl[0][2]-xl[0][3];
    const double Bx=  xl[0][0]-xl[0][1]+xl[0][2]-xl[0][3];
    const double Cx= -xl[0][0]-xl[0][1]+xl[0][2]+xl[0][3];

    const double Ay= -xl[1][0]+xl[1][1]+xl[1][2]-xl[1][3];
    const double By=  xl[1][0]-xl[1][1]+xl[1][2]-xl[1][3];
    const double Cy= -xl[1][0]-xl[1][1]+xl[1][2]+xl[1][3];

    const double alpha= atan2(Ay,Ax);
    double beta= 3.141592653589793/2-atan2(Cx,Cy);
    Matrix Rot(2,2);
    Rot.Zero();
    Rot(0,0)=sin(beta);
    Rot(0,1)=-sin(alpha);
    Rot(1,0)=-cos(beta);
    Rot(1,1)=cos(alpha);
    Matrix Bs(2,12);
  
    double r1= 0;
    double r2= 0;
    double r3= 0;

    double sx[2][2]; //inverse jacobian matrix.
    //gauss loop 
    for(i=0;i<ngauss;i++)
      {
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
        r1= Cx + gp.r_coordinate()*Bx;
        r3= Cy + gp.r_coordinate()*By;
        r1= r1*r1 + r3*r3;
        r1= sqrt (r1);
        r2= Ax + gp.s_coordinate()*Bx;
        r3= Ay + gp.s_coordinate()*By;
        r2= r2*r2 + r3*r3;
        r2= sqrt (r2);

        //get shape functions    
        shape2d( gp.r_coordinate(), gp.s_coordinate(), xl, shp, xsj, sx);
        //volume element to also be saved
        dvol[i]= gp.weight() * xsj;  
        volume += dvol[i];

        Ms(1,0)=1-gp.r_coordinate();
        Ms(0,1)=1-gp.s_coordinate();
        Ms(1,2)=1+gp.r_coordinate();
        Ms(0,3)=1+gp.s_coordinate();
        Bsv= Ms*G;

        for( j= 0; j < 12; j++ )
          {
            Bsv(0,j)=Bsv(0,j)*r1/(8*xsj);
            Bsv(1,j)=Bsv(1,j)*r2/(8*xsj);
          }
        Bs=Rot*Bsv;
    
        // j-node loop to compute strain 
        for( j= 0; j < numnodes; j++ )
          {
            //compute B matrix 
            Bmembrane= computeBmembrane( j, shp );
            Bbend= computeBbend( j, shp );
            for(p= 0; p < 3; p++)
              {
                Bshear(0,p)= Bs(0,j*3+p);
                Bshear(1,p)= Bs(1,j*3+p);
              }//end for p
            BJ= assembleB( Bmembrane, Bbend, Bshear );

	    //save the B-matrix
           for(p=0; p<nstress; p++)
             {
               for(q=0; q<ndf; q++ )
                 saveB[p][q][j]= BJ(p,q);
             }//end for p

           //drilling B matrix
           drillPointer= computeBdrill( j, shp );
           for(p=0; p<ndf; p++ )
             {
               //BdrillJ[p]= *drillPointer++;
               BdrillJ[p]= *drillPointer; //set p-th component
               drillPointer++;             //pointer arithmetic
             }//end for p
          } // end for j

        dd= physicalProperties[i]->getInitialTangent( );
        dd*= dvol[i];

        //tangent calculations node loops

        jj= 0;
        for(j=0;j<numnodes;j++)
          {
            //extract BJ
            for(p=0; p<nstress; p++)
              {
                for(q=0; q<ndf; q++ )
                  BJ(p,q)= saveB[p][q][j];
              }//end for p

            //multiply bending terms by (-1.0) for correct statement
            // of equilibrium  
            for(p=3;p<6;p++)
              {
                for(q=3;q<6;q++)
                  BJ(p,q) *= (-1.0);
              } //end for p

            //transpose 
            //BJtran= transpose( 8, ndf, BJ );
            for(p=0; p<ndf; p++)
              {
                for(q=0; q<nstress; q++) 
                  BJtran(p,q)= BJ(q,p);
              }//end for p

            //drilling B matrix
            drillPointer= computeBdrill( j, shp );
            for(p=0; p<ndf; p++ )
              {
                BdrillJ[p]= *drillPointer;
                drillPointer++;
              }//end for p

            //BJtranD= BJtran * dd;
            BJtranD.addMatrixProduct(0.0, BJtran,dd,1.0 );
      
            for(p=0; p<ndf; p++) 
              BdrillJ[p]*= ( Ktt*dvol[i] );
      
            kk= 0;
            for(k= 0; k < numnodes; k++ )
              {
                //extract BK
                for(p=0; p<nstress; p++)
                  {
                    for(q=0; q<ndf; q++ )
                      BK(p,q)= saveB[p][q][k];
                  }//end for p
       
                //stiffJK= BJtranD * BK;
                // +  transpose( 1,ndf,BdrillJ ) * BdrillK; 
                stiffJK.addMatrixProduct(0.0, BJtranD,BK,1.0 );

                //drilling B matrix
                drillPointer= computeBdrill( k, shp );
                for(p=0; p<ndf; p++ )
                  {
                    BdrillK[p]= *drillPointer;
                    drillPointer++;
                  }//end for p
        
                for(p=0;p<ndf;p++)
                  {
                   for(q=0;q<ndf;q++)
                     {
                       stiff(jj+p,kk+q)+=stiffJK(p,q) + (BdrillJ[p]*BdrillK[q]);
                     }//end for q
                  }//end for p   
                kk += ndf;
              } // end for k loop
          jj += ndf;
        } // end for j loop
 
      } //end for i gauss loop
    theCoordTransf->getGlobalTangent(stiff);
    Ki= stiff;
    return stiff;
  }

//! @brief form residual and tangent
void XC::ShellMITC4Base::formResidAndTangent(int tang_flag) const
  {
    //
    //  six(6) nodal dof's ordered :
    //
    //    -        - 
    //   |    u1    |   <---plate membrane
    //   |    u2    |
    //   |----------|
    //   |  w= u3  |   <---plate bending
    //   |  theta1  | 
    //   |  theta2  | 
    //   |----------|
    //   |  theta3  |   <---drill 
    //    -        -  
    //
    // membrane strains ordered :
    //
    //            strain(0)=   eps00     i.e.   (11)-strain
    //            strain(1)=   eps11     i.e.   (22)-strain
    //            strain(2)=   gamma01   i.e.   (12)-shear
    //
    // curvatures and shear strains ordered  :
    //
    //            strain(3)=     kappa00  i.e.   (11)-curvature
    //            strain(4)=     kappa11  i.e.   (22)-curvature
    //            strain(5)=   2*kappa01  i.e. 2*(12)-curvature 
    //
    //            strain(6)=     gamma02  i.e.   (13)-shear
    //            strain(7)=     gamma12  i.e.   (23)-shear
    //
    //  same ordering for moments/shears but no 2 
    //  
    //  Then, 
    //              epsilon00= -z * kappa00      +    eps00_membrane
    //              epsilon11= -z * kappa11      +    eps11_membrane
    //  gamma01= 2*epsilon01= -z * (2*kappa01)  +  gamma01_membrane 
    //
    //  Shear strains gamma02, gamma12 constant through cross section
    //

    static const int ndf= 6; //two membrane plus three bending plus one drill
    static const int nstress= 8; //three membrane, three moment, two shear
    static const int ngauss= 4;
    static const int numnodes= 4;

    int i,  j,  k, p, q;
    int jj, kk;

    int success;
    
    double volume= 0.0;

    static double xsj;  // determinant jacobian matrix 
    static double dvol[ngauss]; //volume element
    static Vector strain(nstress);  //strain
    static double shp[3][numnodes];  //shape functions at a gauss point

    //  static double Shape[3][numnodes][ngauss]; //all the shape functions
    static Vector residJ(ndf); //nodeJ residual 
    static Matrix stiffJK(ndf,ndf); //nodeJK stiffness 
    static Vector stress(nstress);  //stress resultants
    static Matrix dd(nstress,nstress);  //material tangent
    static Matrix J0(2,2);  //Jacobian at center
    static Matrix J0inv(2,2); //inverse of Jacobian at center

    double epsDrill= 0.0;  //drilling "strain"
    double tauDrill= 0.0; //drilling "stress"

    //---------B-matrices------------------------------------
    static Matrix BJ(nstress,ndf);      // B matrix node J
    static Matrix BJtran(ndf,nstress);
    static Matrix BK(nstress,ndf);      // B matrix node k
    static Matrix BJtranD(ndf,nstress);
    static Matrix Bbend(3,3);  // bending B matrix
    static Matrix Bshear(2,3); // shear B matrix
    static Matrix Bmembrane(3,2); // membrane B matrix
    static double BdrillJ[ndf]; //drill B matrix
    static double BdrillK[ndf];  

    double *drillPointer;

    static double saveB[nstress][ndf][numnodes];

    //------------------------------------------------------- 

    //zero stiffness and residual 
    stiff.Zero( );
    resid.Zero( );

    const Matrix G= calculateG();

    Matrix Ms(2,4);
    Ms.Zero();
    Matrix Bsv(2,12);
    Bsv.Zero();

    const double Ax= -xl[0][0]+xl[0][1]+xl[0][2]-xl[0][3];
    const double Bx=  xl[0][0]-xl[0][1]+xl[0][2]-xl[0][3];
    const double Cx= -xl[0][0]-xl[0][1]+xl[0][2]+xl[0][3];

    const double Ay= -xl[1][0]+xl[1][1]+xl[1][2]-xl[1][3];
    const double By=  xl[1][0]-xl[1][1]+xl[1][2]-xl[1][3];
    const double Cy= -xl[1][0]-xl[1][1]+xl[1][2]+xl[1][3];

    const double alpha= atan2(Ay,Ax);
    const double beta= 3.141592653589793/2-atan2(Cx,Cy);
    Matrix Rot(2,2);
    Rot.Zero();
    Rot(0,0)=sin(beta);
    Rot(0,1)=-sin(alpha);
    Rot(1,0)=-cos(beta);
    Rot(1,1)=cos(alpha);
    Matrix Bs(2,12);
    
    double r1= 0;
    double r2= 0;
    double r3= 0;

    double sx[2][2]; //inverse jacobian matrix.
    //gauss loop 
    for(i= 0;i<ngauss;i++)
      {
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
        r1= Cx + gp.r_coordinate()*Bx;
        r3= Cy + gp.r_coordinate()*By;
        r1= r1*r1 + r3*r3;
        r1= sqrt (r1);
        r2= Ax + gp.s_coordinate()*Bx;
        r3= Ay + gp.s_coordinate()*By;
        r2= r2*r2 + r3*r3;
        r2= sqrt (r2);

        //get shape functions    
        shape2d( gp.r_coordinate(), gp.s_coordinate(), xl, shp, xsj, sx);
        //volume element to also be saved
        dvol[i]= gp.weight() * xsj;  
        volume += dvol[i];

        Ms(1,0)=1-gp.r_coordinate();
        Ms(0,1)=1-gp.s_coordinate();
        Ms(1,2)=1+gp.r_coordinate();
        Ms(0,3)=1+gp.s_coordinate();
        Bsv= Ms*G;

        for(j=0;j<12;j++)
          {
            Bsv(0,j)=Bsv(0,j)*r1/(8*xsj);
            Bsv(1,j)=Bsv(1,j)*r2/(8*xsj);
          }
        Bs=Rot*Bsv;

        //zero the strains
        strain.Zero( );
        epsDrill= 0.0;

        // j-node loop to compute strain 
        for(j=0;j<numnodes;j++)
          {
            //compute B matrix 
            Bmembrane= computeBmembrane( j, shp );
            Bbend= computeBbend( j, shp );

            for(p=0;p<3;p++)
              {
                Bshear(0,p)= Bs(0,j*3+p);
                Bshear(1,p)= Bs(1,j*3+p);
              }//end for p
            BJ= assembleB( Bmembrane, Bbend, Bshear );

            //save the B-matrix
            for(p=0; p<nstress; p++)
              {
                for(q=0; q<ndf; q++ )
                  saveB[p][q][j]= BJ(p,q);
              }//end for p

            //nodal "displacements" 
            const Vector &ul= theCoordTransf->getBasicTrialDisp(j)-initDisp[j];

            //compute the strain
            //strain += (BJ*ul); 
            strain.addMatrixVector(1.0, BJ,ul,1.0 );

            //drilling B matrix
            drillPointer= computeBdrill( j, shp );
            for(p=0; p<ndf; p++ )
              {
                BdrillJ[p]= *drillPointer; //set p-th component
                drillPointer++;             //pointer arithmetic
              }//end for p

            //drilling "strain" 
            for(p=0;p<ndf;p++)
              epsDrill +=  BdrillJ[p]*ul(p);
          } // end for j
    

        //send the strain to the material
        success= const_cast<SectionForceDeformation *>(physicalProperties[i])->setTrialSectionDeformation( strain );

        //compute the stress
        stress= physicalProperties[i]->getStressResultant( );

        //drilling "stress" 
        tauDrill= Ktt * epsDrill;

        //multiply by volume element
        stress*= dvol[i];
        tauDrill*= dvol[i];

        if( tang_flag == 1 )
          {
            dd= physicalProperties[i]->getSectionTangent( );
            dd *= dvol[i];
          } //end if tang_flag


        //residual and tangent calculations node loops

        jj= 0;
        for(j=0;j<numnodes;j++)
          {
            //extract BJ
            for(p=0; p<nstress;p++)
              {
                for(q=0;q<ndf;q++)
                  BJ(p,q)= saveB[p][q][j];
              }//end for p

            //multiply bending terms by (-1.0) for correct statement
            // of equilibrium  
            for(p= 3;p<6;p++)
              {
                for(q=3;q<6;q++) 
                  BJ(p,q) *= (-1.0);
              } //end for p

            //transpose 
            for(p=0; p<ndf; p++)
              {
                for(q=0; q<nstress; q++) 
                  BJtran(p,q)= BJ(q,p);
              }//end for p

            residJ.addMatrixVector(0.0, BJtran,stress,1.0 );

            //drilling B matrix
            drillPointer= computeBdrill( j, shp );
            for(p=0; p<ndf; p++ )
              {
                BdrillJ[p]= *drillPointer;
                drillPointer++;
              }//end for p

            //residual including drill
            for(p=0;p<ndf;p++)
              resid( jj + p ) += ( residJ(p) + BdrillJ[p]*tauDrill );

            if(tang_flag==1)
              {
                BJtranD.addMatrixProduct(0.0, BJtran,dd,1.0 );
                for(p=0; p<ndf; p++) 
                  BdrillJ[p] *= ( Ktt*dvol[i] );

                kk= 0;
                for(k=0;k<numnodes;k++)
                  {
                    //extract BK
                    for(p=0; p<nstress; p++)
                      {
                        for(q=0; q<ndf; q++ )
                          BK(p,q)= saveB[p][q][k];
                      }//end for p
            
                     //drilling B matrix
                     drillPointer= computeBdrill( k, shp );
                     for(p=0; p<ndf; p++ )
                       {
                         BdrillK[p]= *drillPointer;
                         drillPointer++;
                       }//end for p
 
                    //stiffJK= BJtranD * BK;
                    // +  transpose( 1,ndf,BdrillJ ) * BdrillK; 
                    stiffJK.addMatrixProduct(0.0, BJtranD,BK,1.0);

                    for(p=0;p<ndf;p++)
                      {
                        for(q=0;q<ndf;q++)
                          {
                            stiff(jj+p,kk+q) += stiffJK(p,q) + (BdrillJ[p]*BdrillK[q]);
                          }//end for q
                      }//end for p
                    kk+= ndf;
                  } // end for k loop
              } // end if tang_flag 
          jj+= ndf;
        } // end for j loop
      } //end for i gauss loop
    // Self weigth
    if(applyLoad == 1)
      {
	const int nShape= 3;
	const int numberNodes= 4;
	const int massIndex= nShape - 1;
	double temp, rhoH;
	//If defined, apply self-weight
	static Vector momentum(ndf);
	double ddvol = 0;
	for(i = 0;i<ngauss;i++)
	  {
	    //get shape functions    
            const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
            shape2d(gp.r_coordinate(), gp.s_coordinate(), xl, shp, xsj, sx);

	    //volume element to also be saved
	    ddvol= gp.weight() * xsj;  

	    //node loop to compute accelerations
	    momentum.Zero( );
	    momentum(0)= appliedB[0];
	    momentum(1)= appliedB[1];
	    momentum(2)= appliedB[2];

	    //density on the Gauss point i.
            rhoH= physicalProperties[i]->getRho();

	    //multiply acceleration by density to form momentum
	    momentum*= rhoH;


	    //residual and tangent calculations node loops
	    for(j=0, jj=0; j<numberNodes; j++, jj+=ndf )
	      {
  	        temp = shp[massIndex][j] * ddvol;
  	        for( p = 0; p < 3; p++ )
		  resid( jj+p ) += ( temp * momentum(p) );
	      }
	  }
      }
    theCoordTransf->getGlobalResidAndTangent(resid,stiff);
    return;
  }

//! @brief compute Bdrill
double *XC::ShellMITC4Base::computeBdrill( int node, const double shp[3][4]) const
  {

    //static Matrix Bdrill(1,6);
    static double Bdrill[6];

    static double B1;
    static double B2;
    static double B6;


//---Bdrill Matrix in standard {1,2,3} mechanics notation---------
//
//             -                                       -
//   Bdrill= | -0.5*N,2   +0.5*N,1    0    0    0   -N |   (1x6)
//             -                                       -
//
//----------------------------------------------------------------

    //  Bdrill.Zero( );

    //Bdrill(0,0)= -0.5*shp[1][node];

    //Bdrill(0,1)= +0.5*shp[0][node];

    //Bdrill(0,5)=     -shp[2][node];


    B1= -0.5*shp[1][node];
    B2= +0.5*shp[0][node];
    B6= -shp[2][node];
   
    const Vector &g1= theCoordTransf->G1();
    const Vector &g2= theCoordTransf->G2();

    Bdrill[0]= B1*g1[0] + B2*g2[0];
    Bdrill[1]= B1*g1[1] + B2*g2[1];
    Bdrill[2]= B1*g1[2] + B2*g2[2];

    const Vector &g3= theCoordTransf->G3();
    Bdrill[3]= B6*g3[0];
    Bdrill[4]= B6*g3[1];
    Bdrill[5]= B6*g3[2];

    return Bdrill;
  }

//! @brief compute Bmembrane matrix
const XC::Matrix &XC::ShellMITC4Base::computeBmembrane( int node, const double shp[3][4] ) const
  {

    static Matrix Bmembrane(3,2);

//---Bmembrane matrix in standard {1,2,3} mechanics notation---------
//
//                -             -
//               | +N,1      0   |
// Bmembrane=    |   0     +N,2  |    (3x2)
//               | +N,2    +N,1  |
//                -             -
//
//  three(3) strains and two(2) displacements (for plate)
//-------------------------------------------------------------------

    Bmembrane.Zero( );

    Bmembrane(0,0)= shp[0][node];
    Bmembrane(1,1)= shp[1][node];
    Bmembrane(2,0)= shp[1][node];
    Bmembrane(2,1)= shp[0][node];

    return Bmembrane;
  }

//! @brief assemble a B matrix
//! @param Bmembrane: membrane B matrix (3x2)
//! @param Bbend:  plate bending B matrix (3x2)
//! @param Bshear: plate shear B matrix (2x3)
const XC::Matrix &XC::ShellMITC4Base::assembleB(const Matrix &Bmembrane, const Matrix &Bbend, const Matrix &Bshear) const
  {

    static Matrix B(8,6);
    static Matrix BmembraneShell(3,3);
    static Matrix BbendShell(3,3);
    static Matrix BshearShell(2,6);
    static Matrix Gmem(2,3);
    static Matrix Gshear(3,6);

//
// For Shell :
//
//---B Matrices in standard {1,2,3} mechanics notation---------
//
//            -                     _
//           | Bmembrane  |     0    |
//           | --------------------- |
//    B=     |     0      |  Bbend   |   (8x6)
//           | --------------------- |
//           |         Bshear        |
//            -           -         -
//
//-------------------------------------------------------------
//
//


    //shell modified membrane terms

    const Vector &g1= theCoordTransf->G1();
    Gmem(0,0)= g1[0];
    Gmem(0,1)= g1[1];
    Gmem(0,2)= g1[2];

    const Vector &g2= theCoordTransf->G2();
    Gmem(1,0)= g2[0];
    Gmem(1,1)= g2[1];
    Gmem(1,2)= g2[2];

    //BmembraneShell= Bmembrane * Gmem;
    BmembraneShell.addMatrixProduct(0.0, Bmembrane,Gmem,1.0 );


    //shell modified bending terms
    Matrix &Gbend= Gmem;

    //BbendShell= Bbend * Gbend;
    BbendShell.addMatrixProduct(0.0, Bbend,Gbend,1.0 );


    //shell modified shear terms
    Gshear.Zero( );

    const Vector &g3= theCoordTransf->G3();
    Gshear(0,0)= g3[0];
    Gshear(0,1)= g3[1];
    Gshear(0,2)= g3[2];

    Gshear(1,3)= g1[0];
    Gshear(1,4)= g1[1];
    Gshear(1,5)= g1[2];

    Gshear(2,3)= g2[0];
    Gshear(2,4)= g2[1];
    Gshear(2,5)= g2[2];

    //BshearShell= Bshear * Gshear;
    BshearShell.addMatrixProduct(0.0, Bshear,Gshear,1.0 );

    B.Zero( );

    //assemble B from sub-matrices

    //membrane terms
    for(int p= 0; p < 3; p++ )
      {
        for(int q= 0; q < 3; q++ )
          B(p,q)= BmembraneShell(p,q);
      } //end for p


    //bending terms
    int pp;
    for(int p= 3; p < 6; p++ )
      {
        pp= p - 3;
        for(int q= 3; q < 6; q++ )
          B(p,q)= BbendShell(pp,q-3);
      } // end for p


    //shear terms
    for(int p= 0; p < 2; p++ )
      {
        pp= p + 6;
        for(int q= 0; q < 6; q++ )
          { B(pp,q)= BshearShell(p,q); } // end for q
      } //end for p

    return B;
  }

//! @brief compute Bbend matrix
const XC::Matrix &XC::ShellMITC4Base::computeBbend( int node, const double shp[3][4] ) const
  {

      static XC::Matrix Bbend(3,2);

//---Bbend matrix in standard {1,2,3} mechanics notation---------
//
//            -             -
//   Bbend=  |    0    -N,1  |
//           |  +N,2     0   |    (3x2)
//           |  +N,1   -N,2  |
//            -             -
//
//  three(3) curvatures and two(2) rotations (for plate)
//----------------------------------------------------------------

      Bbend.Zero( );

      Bbend(0,1)= -shp[0][node];
      Bbend(1,0)=  shp[1][node];
      Bbend(2,0)=  shp[0][node];
      Bbend(2,1)= -shp[1][node];

      return Bbend;
  }

//! @brief Send members through the communicator argument.
int XC::ShellMITC4Base::sendData(Communicator &comm)
  {
    int res= Shell4NBase::sendData(comm);
    res+=comm.sendDoubles(Ktt,xl[0][0],xl[0][1],xl[0][2],xl[0][3],getDbTagData(),CommMetaData(17));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ShellMITC4Base::recvData(const Communicator &comm)
  {
    int res= Shell4NBase::recvData(comm);
    res+=comm.receiveDoubles(Ktt,xl[0][0],xl[0][1],xl[0][2],xl[0][3],getDbTagData(),CommMetaData(17));
    return res;
  }



