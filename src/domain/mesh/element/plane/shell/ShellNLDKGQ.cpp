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

// $Revision: 1.10 $
// $Date: 2014/09/30 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/ShellNLDKGQ/ShellNLDKGQ.h,v $

// Written: Lisha Wang, Xinzheng Lu, Linlin Xie, Song Cen & Quan Gu
//
//  four node flat shell element with membrane and drill DOF
//  considering geometric nonlinear, form nonlinear shell element
//  using updated Lagrangian formula
// Ref: Plate Bending Part - DKQ, thin plate element
//      Membrane Part - GQ12, a membrane element with drilling DOF
//

#include "ShellNLDKGQ.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "utility/recorder/response/ElementResponse.h"
//#include "domain/load/plane/ShellMecLoad.h"

XC::ShellNLCrdTransf3d XC::ShellNLDKGQ::non_linear_trf;

//! .@brief Default constructor
XC::ShellNLDKGQ::ShellNLDKGQ(void)
  : Shell4NBase(ELE_TAG_ShellNLDKGQ,&non_linear_trf),
    CstrainGauss(32),TstrainGauss(32) //modify for geometric nonlinearity
  {}

//! @brief full constructor
XC::ShellNLDKGQ::ShellNLDKGQ(int tag,const SectionForceDeformation *ptr_mat)
  : Shell4NBase(tag, ELE_TAG_ShellNLDKGQ, ptr_mat,&non_linear_trf),
    CstrainGauss(32),TstrainGauss(32) //modify for geometric nonlinearity
  {}

//! @brief Full constructor
XC::ShellNLDKGQ::ShellNLDKGQ(int tag,
                             int node1, int node2, int node3, int node4,
                             const SectionForceDeformation &theMaterial)
  : Shell4NBase(tag, ELE_TAG_ShellNLDKGQ,&theMaterial,&non_linear_trf),
    CstrainGauss(32),TstrainGauss(32) //modify for geometric nonlinearity
  {}

//! @brief Virtual constructor.
XC::Element* XC::ShellNLDKGQ::getCopy(void) const
  { return new ShellNLDKGQ(*this); }

//! @brief set domain
void XC::ShellNLDKGQ::setDomain(Domain *theDomain)
  {    
    Shell4NBase::setDomain(theDomain);
    //basis vectors and local coordinates
    computeBasis();
  }

//! @brief commit state
int XC::ShellNLDKGQ::commitState(void)
  {
    int success= Shell4NBase::commitState();
    //added for geometric nonlinearity
    //save the prev. step strain
    CstrainGauss= TstrainGauss;
    return success;
  }

//! @brief revert to last commit
int XC::ShellNLDKGQ::revertToLastCommit(void)
  {
    int success= Shell4NBase::revertToLastCommit();
    //added for geometric nonlinearity
    TstrainGauss= CstrainGauss;
    return success;
  }

//! @brief revert to start
int XC::ShellNLDKGQ::revertToStart(void)
  {
    int success= Shell4NBase::revertToStart();
    //added for geometric nonlinearity
    CstrainGauss.Zero();
    return success;
  }

//! @brief return stiffness matrix
const XC::Matrix &XC::ShellNLDKGQ::getTangentStiff(void) const
  {
    int tang_flag= 1; //get the tangent
    //do tangent and residual here
    formResidAndTangent(tang_flag);
    return stiff;
  }

//! @brief return secant matrix
const XC::Matrix &XC::ShellNLDKGQ::getInitialStiff(void) const
  {
    if(Ki.isEmpty())
      {
        static const int ndf= 6; //two membrane + 3 moment +drill
        static const int nstress= 8; //3 membrane , 3 moment, 2 shear
        static const int ngauss= 4;
        static const int numnodes= 4;

        int success;

        double volume= 0.0;
        static double xsj; //determinant jacobian matrix
        static double dvol[ngauss]; //volume element

        //add for geometric nonlinearity
        static Vector incrDisp(ndf); //total displamcement
        static Vector Cstrain(nstress);//commit strain last step/ add for geometric nonlinearity
        static Vector strain(nstress); //strain
        //add for geometric nonlinearity
        static Vector dstrain(nstress);   //total strain increment
        static Vector dstrain_li(nstress); //linear incr strain
        static Vector dstrain_nl(3);//geometric nonlinear strain

        static double shp[3][numnodes]; //shape function 2d at a gauss point
        static double shpDrill[4][numnodes]; //shape function drilling dof at a gauss point
        static double shpBend[6][12]; //shape function - bending part at a gauss point

        //static Vector residJ(ndf); //nodeJ residual, global coordinates
        static Matrix stiffJK(ndf,ndf);//nodeJK stiffness, global coordinates
        //static Vector residJlocal(ndf); //nodeJ residual, local coordinates
        static Matrix stiffJKlinear(ndf,ndf);//nodeJK stiffness,for linear part
        static Matrix stiffJKgeo(3,3);//nodeJK stiffness,for geometric nonlinearity
        static Matrix stiffJKlocal(ndf,ndf); //nodeJK stiffness, local coordinates
        static Matrix stiffJK1(ndf,ndf);
        static Matrix stiffJK2(ndf,ndf);
        static Matrix stiffJK3(ndf,ndf);

        //static Vector residJ1(ndf);

        static Vector stress(nstress); //stress resultants
        //static Vector dstress(nstress); //add for geometric nonlinearity

        static Matrix dd(nstress,nstress);//material tangent

        //static Matrix J0(2,2); //Jacobian at center

        //static Matrix J0inv(2,2); //inverse of Jacobian at center
        static double sx[2][2];

        //add for geometric nonlinearity
        static Vector dispIncLocal(6);  //incr disp in local coordinates
        static Vector dispIncLocalBend(3);//incr disp of bending part in local coordinates
        //eleForce & eleForceLast: gauss stress
        static Vector stressLast_gauss(8); //eleForceLast
        //static Vector stressNew_gauss(8);  //eleForce
        static Matrix membraneForce(2,2); //membrane force in gauss point

        Matrix Tmat(6,6);  //local-global coordinates transform matrix
        Matrix TmatTran(6,6);
        Matrix Pmat(6,6); //transform dofs order
        Matrix PmatTran(6,6);

        //-------------------B-matrices---------------------------------

        static Matrix BJ(nstress, ndf); // B matrix node J
        static Matrix BJtran(ndf, nstress);
        static Matrix BK(nstress, ndf); // B matrix node K
        static Matrix BJtranD(ndf, nstress); //BJtran * dd
        static Matrix BJP(nstress, ndf); //BJ * Pmat, transform the dof order

        //static Matrix BJPT(nstress,ndf); //BJP * Tmat, from global coordinates to local coordinates

        static Matrix Bmembrane(3,3); //membrane B matrix
        static Matrix Bbend(3,3); //bending B matrix
        static Matrix Bshear(2,3); //shear B matrix (zero)
        static double saveB[nstress][ndf][numnodes];

        //Added for geometric nonlinearity
        //BG
        static Matrix BGJ(2,3);
        static Matrix BGJtran(3,2);
        static Matrix stiffBGM(3,2);// BGJtran * membraneForce
        static Matrix BGK(2,3);
        //---------------------------------------------------------------

        //zero stiffness and residual
        stiff.Zero();
        //resid.Zero();

        //start Yuli Huang & Xinzheng Lu
        updateBasis();
        //end Yuli Huang & Xinzheng Lu

        //define Pmat- transpose the dofs
        Pmat.Zero();
        const double one=1.00;
        Pmat(0,0)= one;
        Pmat(1,1)= one;
        Pmat(2,5)= one;
        Pmat(3,2)= one;
        Pmat(4,3)= one;
        Pmat(5,4)= one;
        //transpose PmatTran=transpose(Pmat)
        for(int p1=0;p1<6;p1++)
          {
            for(int q1=0;q1<6;q1++)
              { PmatTran(p1,q1)= Pmat(q1,p1); }
          }//end for p1

	const Vector &g1= theCoordTransf->G1();
	const Vector &g2= theCoordTransf->G2();
	const Vector &g3= theCoordTransf->G2();
        //define Tmat xl=Tmat * x from global to local coordinates
        Tmat.Zero();
        Tmat(0,0)= g1[0]; Tmat(0,1)= g2[0]; Tmat(0,2)= g3[0];
        Tmat(1,0)= g1[1]; Tmat(1,1)= g2[1]; Tmat(1,2)= g3[1];
        Tmat(2,0)= g1[2]; Tmat(2,1)= g2[2]; Tmat(2,2)= g3[2];
        Tmat(3,3)= g1[0]; Tmat(3,4)= g2[0]; Tmat(3,5)= g3[0];
        Tmat(4,3)= g1[1]; Tmat(4,4)= g2[1]; Tmat(4,5)= g3[1];
        Tmat(5,3)= g1[2]; Tmat(5,4)= g2[2]; Tmat(5,5)= g3[2];

        //transpose TmatTran=transpose(Tmat)
        for(int p2=0;p2<6;p2++)
          {
            for(int q2=0;q2<6;q2++)
              { TmatTran(p2,q2)= Tmat(q2,p2); }
          }//end for p2

        //------------gauss loop--------------------------
        for(int i=0; i<ngauss;i++)
          {
            const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
            //get shape functions
            shape2d(gp.r_coordinate(),gp.s_coordinate(),xl,shp,xsj,sx);
            shapeDrill(gp.r_coordinate(),gp.s_coordinate(),xl,sx,shpDrill);
            shapeBend(gp.r_coordinate(),gp.s_coordinate(),xl,sx,shpBend);

             //volume element to be saved
            dvol[i]= gp.weight() * xsj;
            volume += dvol[i];

            Bshear.Zero();

            //zero the dstrains
            //add for geometric nonlinearity
            dstrain.Zero();
            dstrain_li.Zero();
            dstrain_nl.Zero();

            //add for geometric nonlinearity
            for(int jlast=0;jlast < nstress;jlast++)
              { Cstrain(jlast)= CstrainGauss(i*8+jlast); }
            //std::cerr << CstrainGauss << std::endl;

            // j-node loop to compute strain
            for(int j= 0; j < numnodes; j++)
              {
                //compute B matrix
                Bmembrane= computeBmembrane(j,shp,shpDrill);
                Bbend= computeBbend(j,shpBend);
                BJ= assembleB(Bmembrane, Bbend, Bshear);

                //save the B-matrix
                for(int p=0;p<nstress; p++)
                  {
                    for(int q=0;q<ndf;q++)
                      { saveB[p][q][j]= BJ(p,q); }
                  }//end for p
                //std::cerr << Bmembrane << std::endl;

                //add for geometric nonlinearity
                //BGJ
                BGJ= computeBG(j,shpBend);

                //nodal "displacements"  - need to be modified for geometric nonlinearity
                //delta displacements
                //in global coordinates
                const Vector &TrialDisp= theNodes[j]->getTrialDisp();
                const Vector &CommitDisp= theNodes[j]->getDisp();

                //incrDisp= TrialDisp - CommitDisp;
                for(int p=0;p<ndf;p++)
                  { incrDisp(p)= TrialDisp(p) - CommitDisp(p); }// end for p

                //dispIncLocal= Tmat * dul
                dispIncLocal.addMatrixVector(0.0,Tmat,incrDisp,1.0);

                //bending part incr disp
                dispIncLocalBend(0)= dispIncLocal(2);
                dispIncLocalBend(1)= dispIncLocal(3);
                dispIncLocalBend(2)= dispIncLocal(4);
                //std::cerr << dul << std::endl;

                //compute the strain - modified for geometric nonlinearity:dstrain_li(8)
                //Note: transform the dof's order
                //BJP= BJ * P;
                BJP.addMatrixProduct(0.0, BJ, Pmat,1.0);
                //BJPT.addMatrixProduct(0.0,BJP,Tmat,1.0);//transform ul from global to local coordinates
                //dstrain_li += (BJ*dispIncLocal);
                dstrain_li.addMatrixVector(1.0, BJP, dispIncLocal,1.0);

                //add for geometric nonlinearity: dstrain_nl(3)
                //dstrain_nl += (BGJ*dulbend);
                //note: dstrain should be BGJ * dispIncLocalBend sum from j=0 to j=3
                dstrain_nl += computeNLdstrain(BGJ,dispIncLocalBend);
                //std::cerr << dstrain_nl << std::endl;


                //dstrain= dstrain_li + dstrain_nl
                dstrain(0)= dstrain_li(0) +dstrain_nl(0);
                dstrain(1)= dstrain_li(1) +dstrain_nl(1);
                dstrain(2)= dstrain_li(2) +dstrain_nl(2);
                dstrain(3)= dstrain_li(3);
                dstrain(4)= dstrain_li(4);
                dstrain(5)= dstrain_li(5);
                dstrain(6)= dstrain_li(6);
                dstrain(7)= dstrain_li(7);

              }//end j-node loop

            //strain= Cstrain + dstrain;
            for(int q=0;q<nstress;q++)
              { strain(q)= Cstrain(q) + dstrain(q); }//end for q

            //send the strain to the material
    	    // not very elegant. Maybe formResidAndTangent must not be const. 
	    SectionForceDeformation *mat_no_const= const_cast<SectionForceDeformation *>(physicalProperties[i]);
            success= mat_no_const->setTrialSectionDeformation(strain);

            //compute the stress
            stress= physicalProperties[i]->getStressResultant();

            //std::cerr << dstrain <<std::endl;
            //std::cerr << strain <<std::endl;

            //add for geometric nonlinearity
            //update strain in gauss points
            //define TstrainGauss
            for(int jnew=0;jnew < nstress;jnew++)
              { TstrainGauss(i*8+jnew)= strain(jnew); }
            //CstrainGauss= TstrainGauss;

            //add for geometric nonlinearity
            //from stress(0,1,2) to compute membraneForce at t
            membraneForce(0,0)= stress(0);
            membraneForce(1,1)= stress(1);
            membraneForce(0,1)= stress(2);
            membraneForce(1,0)= stress(2);

            stress *= dvol[i];//for Resid integration
            membraneForce *= dvol[i]; //for stiffJKgeo integration

            dd= physicalProperties[i]->getInitialTangent();
            dd *= dvol[i];//for stiffJKlinear integration

            //std::cerr << tang_flag << std::endl;

            //residual and tangent calculations node loops

            //jj loop----------------
            int jj= 0;
            for(int j=0; j<numnodes;j++)
              {
                //extract BJ
                for(int p=0;p<nstress;p++)
                  {
                    for(int q=0; q<ndf;q++)
                      BJ(p,q)=saveB[p][q][j];
                  }// end for p
                //multiply bending terms by -1.0 for correct statement of equilibrium
                for(int p=3;p<6;p++)
                  {
                    for(int q=3; q<6; q++)
                      BJ(p,q) *= (-1.0);
                  }//end for p

                //transpose BJtran=transpose(BJ);
                for(int p=0;p<ndf;p++)
                  {
                    for(int q=0;q<nstress;q++)
                      BJtran(p,q)=BJ(q,p);
                   }//end for p

                //add for geometric nonlinearity
                BGJ= computeBG(j,shpBend);
                //transpose BGJ
                for(int p3=0; p3<3; p3++)
                  {
                    for(int q3=0; q3<2; q3++)
                      BGJtran(p3,q3)=BGJ(q3,p3);
                  }//end for p3
                stiffBGM.addMatrixProduct(0.0,BGJtran,membraneForce,1.0);

                BJtranD.addMatrixProduct(0.0,BJtran,dd,1.0);


                //k loop------------
                int kk=0;
                for(int k=0; k<numnodes; k++)
                  {
                    //extract BK
                    for(int p=0; p<nstress;p++)
                      {
                        for(int q=0;q<ndf;q++)
                          BK(p,q)= saveB[p][q][k];
                      }// end for p

                    //stiffJKlinear= BJtranD * BK - modify for geometric nonlinearity
                    stiffJKlinear.addMatrixProduct(0.0,BJtranD,BK,1.0);

                    //add for geometric nonlinearity
                    //extract BGK
                    BGK= computeBG(k,shpBend);
                    //stiffGeo= BGJtran * membraneForce *BGK
                    stiffJKgeo.addMatrixProduct(0.0,stiffBGM,BGK,1.0);

                    //std::cerr << stiffJKlinear << std::endl;
                    //std::cerr << stiffJKgeo << std::endl;

                    //stiffJKlocal= stiffJKlinear + stiffJKgeo
                    stiffJKlocal= stiffJKlinear;
                    /*for(int pp=3;pp<6;pp++)
                        {
                          for(int qq=3;qq<6;qq++)
                            stiffJKlocal(pp,qq) += stiffJKgeo(pp-3,qq-3);
                        }//end for pp*/

                    //transpose dof and coordinates
                    stiffJK1.addMatrixProduct(0.0,PmatTran,stiffJKlocal,1.0);
                    stiffJK2.addMatrixProduct(0.0,stiffJK1,Pmat,1.0);
                    stiffJK3.addMatrixProduct(0.0,TmatTran,stiffJK2,1.0);
                    stiffJK.addMatrixProduct(0.0,stiffJK3,Tmat,1.0);
                    //std::cerr << Bbend << std::endl;

                    //generate stiff
                    //modify for geometric nonlinearity
                    for(int p=0;p<ndf;p++)
                      {
                        for(int q=0;q<ndf;q++)
                          { stiff(jj+p,kk+q) += stiffJK(p,q); }//end for q
                      }//end for p

                    kk += ndf;
                  }//end for k loop
                jj += ndf;
              }// end for j loop

          }//end for i gauss loop
        Ki= Matrix(stiff);
      }
    return Ki;
  }

//! @brief return mass matrix
const XC::Matrix &XC::ShellNLDKGQ::getMass(void) const
  {
    const int tangFlag= 1;
    formInertiaTerms(tangFlag);
    return mass;
  }

//! @brief form inertia terms
void XC::ShellNLDKGQ::formInertiaTerms(int tangFlag) const
  {

    //translational mass only
    //rotational inertia terms are neglected
    static const int ndf= 6;
    static const int numberNodes= 4;
    static const int numberGauss= 4;
    static const int nShape= 3;
    static const int massIndex= nShape - 1;

    double xsj;  // determinant jacaobian matrix
    double sx[2][2]; //inverse jacobian matrix
    double dvol; //volume element
    static double shp[nShape][numberNodes]; //shape functions at a gauss point
    static Vector momentum(ndf);

    double temp, rhoH, massJK;

    //zero mass
    mass.Zero();

    //gauss loop
    for(int i= 0; i < numberGauss; i++ )
      {
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];

        //get shape functions
        shape2d( gp.r_coordinate(), gp.s_coordinate(), xl, shp, xsj ,sx);

        //volume element to also be saved
        dvol= gp.weight() * xsj;

        //node loop to compute accelerations
        momentum.Zero();
        for(int j= 0; j < numberNodes; j++)
          {
            //momentum += ( shp[massIndex][j] * theNodes[j]->getTrialAccel() );
            momentum.addVector(1.0, theNodes[j]->getTrialAccel(), shp[massIndex][j] );
          }

        //density
        rhoH= physicalProperties[i]->getRho();

        //multiply acceleration by density to form momentum
        momentum *= rhoH;


        //residual and tangent calculations node loops
        for(int j=0, jj=0; j<numberNodes; j++, jj+=ndf )
          {
            temp= shp[massIndex][j] * dvol;

            for(int p= 0; p < 3; p++ )
              resid( jj+p ) += ( temp * momentum(p) );


            if(tangFlag == 1 && rhoH != 0.0)
              {
               //multiply by density
               temp *= rhoH;

               //node-node translational mass
               for(int k=0, kk=0; k<numberNodes; k++, kk+=ndf )
                 {
                   massJK= temp * shp[massIndex][k];

                   for(int p= 0; p < 3; p++)
                     mass( jj+p, kk+p ) +=  massJK;
                   } // end for k loop

              } // end if tangFlag

          } // end for j loop

      } //end for i gauss loop
  }

//! @brief form residual and tangent
void XC::ShellNLDKGQ::formResidAndTangent(int tangFlag) const
  {
    //
    //six(6) nodal dof's ordered:
    //-----------
    //|    u1    |   <---- plate membrane
    //|    u2    |
    //|----------|
    //|   w= u3 |  <----plate bending
    //|   theta1 |
    //|   theta2 |
    //|----------|
    //|   theta3 |  <- drill (tran from membrane)
    //|----------|

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
    //     neglected in this shell element   Zero();
    //
    static constexpr int ndf= 6; //two membrane + 3 moment +drill
    static constexpr int nstress= 8; //3 membrane , 3 moment, 2 shear
    static constexpr int ngauss= 4;
    static constexpr int numnodes= 4;

    int success;

    double volume= 0.0;
    static double xsj; //determinant jacobian matrix
    static double dvol[ngauss]; //volume element

    //add for geometric nonlinearity
    static Vector incrDisp(ndf); //total displacement
    static Vector Cstrain(nstress);//commit strain last step/ add for geometric nonlinearity
    static Vector strain(nstress); //strain
    //add for geometric nonlinearity
    static Vector dstrain(nstress);   //total strain increment
    static Vector dstrain_li(nstress); //linear incr strain
    static Vector dstrain_nl(3);//geometric nonlinear strain

    static double shp[3][numnodes]; //shape function 2d at a gauss point
    static double shpDrill[4][numnodes]; //shape function drilling dof at a gauss point
    static double shpBend[6][12]; //shape function - bending part at a gauss point
    static Vector residJ(ndf); //nodeJ residual, global coordinates
    static Matrix stiffJK(ndf,ndf);//nodeJK stiffness, global coordinates
    static Vector residJlocal(ndf); //nodeJ residual, local coordinates
    static Matrix stiffJKlinear(ndf,ndf);//nodeJK stiffness,for linear part
    static Matrix stiffJKgeo(3,3);//nodeJK stiffness,for geometric nonlinearity
    static Matrix stiffJKlocal(ndf,ndf); //nodeJK stiffness, local coordinates
    static Matrix stiffJK1(ndf,ndf);
    static Matrix stiffJK2(ndf,ndf);
    static Matrix stiffJK3(ndf,ndf);
    static Vector residJ1(ndf);

    static Vector stress(nstress); //stress resultants
    static Vector Cstress(nstress);
        //static Vector dstress(nstress); //add for geometric nonlinearity

    static Matrix dd(nstress,nstress);//material tangent
    //static Matrix J0(2,2); //Jacobian at center
    //static Matrix J0inv(2,2); //inverse of Jacobian at center
    static double sx[2][2];

    //add for geometric nonlinearity
    static Vector dispIncLocal(6);  //incr disp in local coordinates
    static Vector dispIncLocalBend(3);//incr disp of bending part in local coordinates

    //eleForce & eleForceLast: gauss stress
    static Matrix membraneForce(2,2); //membrane force in gauss point

    Matrix Tmat(6,6);  //local-global coordinates transform matrix
    Matrix TmatTran(6,6);
    Matrix Pmat(6,6); //transform dofs order
    Matrix PmatTran(6,6);

    //-------------------B-matrices---------------------------------
    static Matrix BJ(nstress, ndf); // B matrix node J
    static Matrix BJtran(ndf, nstress);
    static Matrix BK(nstress, ndf); // B matrix node K
    static Matrix BJtranD(ndf, nstress); //BJtran * dd
    static Matrix BJP(nstress, ndf); //BJ * Pmat, transform the dof order
    //static Matrix BJPT(nstress,ndf); //BJP * Tmat, from global coordinates to local coordinates
    static Matrix Bmembrane(3,3); //membrane B matrix
    static Matrix Bbend(3,3); //bending B matrix
    static Matrix Bshear(2,3); //shear B matrix (zero)
    static double saveB[nstress][ndf][numnodes];

    //Added for geometric nonlinearity
    //BG
    static Matrix BGJ(2,3);
    static Matrix BGJtran(3,2);
    static Matrix stiffBGM(3,2);// BGJtran * membraneForce
    static Matrix BGK(2,3);
    //---------------------------------------------------------------

    //zero stiffness and residual
    stiff.Zero();
    resid.Zero();

    //start Yuli Huang & Xinzheng Lu
    updateBasis();
    //end Yuli Huang & Xinzheng Lu

    //define Pmat- transpose the dofs
    Pmat.Zero();
    const double one= 1.00;
    Pmat(0,0)= one;
    Pmat(1,1)= one;
    Pmat(2,5)= one;
    Pmat(3,2)= one;
    Pmat(4,3)= one;
    Pmat(5,4)= one;
    //transpose PmatTran=transpose(Pmat)
    for(int p1=0;p1<6;p1++)
      {
        for(int q1=0;q1<6;q1++)
          { PmatTran(p1,q1)= Pmat(q1,p1); }
      }//end for p1

    const Vector &g1= theCoordTransf->G1();
    const Vector &g2= theCoordTransf->G2();
    const Vector &g3= theCoordTransf->G3();
    //define Tmat xl=Tmat * x from global to local coordinates
    Tmat.Zero();
    Tmat(0,0)= g1[0]; Tmat(0,1)= g1[1]; Tmat(0,2)= g1[2];
    Tmat(1,0)= g2[0]; Tmat(1,1)= g2[1]; Tmat(1,2)= g2[2];
    Tmat(2,0)= g3[0]; Tmat(2,1)= g3[1]; Tmat(2,2)= g3[2];
    Tmat(3,3)= g1[0];
    Tmat(3,4)= g1[1];
    Tmat(3,5)= g1[2];
    Tmat(4,3)= g2[0];
    Tmat(4,4)= g2[1];
    Tmat(4,5)= g2[2];
    Tmat(5,3)= g3[0];
    Tmat(5,4)= g3[1];
    Tmat(5,5)= g3[2];

    //transpose TmatTran=transpose(Tmat)
    for(int p2=0;p2<6;p2++)
      {
        for(int q2=0;q2<6;q2++)
          { TmatTran(p2,q2)= Tmat(q2,p2); }
      }//end for p2

    //------------gauss loop--------------------------
    for(int i=0; i<ngauss;i++)
      {
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
        //get shape functions
        shape2d(gp.r_coordinate(),gp.s_coordinate(),xl,shp,xsj,sx);
        shapeDrill(gp.r_coordinate(),gp.s_coordinate(),xl,sx,shpDrill);
        shapeBend(gp.r_coordinate(),gp.s_coordinate(),xl,sx,shpBend);

        //volume element to be saved
        dvol[i]= gp.weight() * xsj;
        volume += dvol[i];

        Bshear.Zero();

        //zero the dstrains
        //add for geometric nonlinearity
        dstrain.Zero();
        dstrain_li.Zero();
        dstrain_nl.Zero();

        //add for geometric nonlinearity
        for(int jlast=0;jlast<nstress;jlast++)
          { Cstrain(jlast)= CstrainGauss(i*8+jlast); }

        // j-node loop to compute strain
        for(int j= 0; j < numnodes; j++)
          {
            //compute B matrix
            Bmembrane= computeBmembrane(j,shp,shpDrill);
            Bbend= computeBbend(j,shpBend);
            BJ= assembleB(Bmembrane, Bbend, Bshear);

            //save the B-matrix
            for(int p=0;p<nstress; p++)
              {
                for(int q=0;q<ndf;q++)
                  { saveB[p][q][j]= BJ(p,q); }
              }//end for p

            //add for geometric nonlinearity
            //BGJ
            BGJ= computeBG(j,shpBend);
            //std::cerr << BGJ << std::endl;

            //nodal "displacements"  - need to be modified for geometric nonlinearity
            //delta displacements
            //in global coordinates
            /*const Vector &TrialDisp= theNodes[j]->getTrialDisp();
            const Vector &CommitDisp= theNodes[j]->getDisp();

            //incrDisp= TrialDisp - CommitDisp;
            for(p=0;p<ndf;p++){
                    incrDisp(p)= TrialDisp(p) - CommitDisp(p);
            }// end for p*/
            const Vector &incrDisp= theNodes[j]->getIncrDisp();

            //dispIncLocal= Tmat * dul
            dispIncLocal.addMatrixVector(0.0,Tmat,incrDisp,1.0);

            //bending part incr disp
            dispIncLocalBend(0)= dispIncLocal(2);
            dispIncLocalBend(1)= dispIncLocal(3);
            dispIncLocalBend(2)= dispIncLocal(4);

            //compute the strain - modified for geometric nonlinearity:dstrain_li(8)
            //Note: transform the dof's order
            //BJP= BJ * P;
            BJP.addMatrixProduct(0.0, BJ, Pmat,1.0);
            //dstrain_li += (BJ*dispIncLocal);
            dstrain_li.addMatrixVector(1.0, BJP, dispIncLocal,1.0);

            //add for geometric nonlinearity: dstrain_nl(3)
            //dstrain_nl += (BGJ*dulbend);
            //note: dstrain should be BGJ * dispIncLocalBend sum from j=0 to j=3
            dstrain_nl += computeNLdstrain(BGJ,dispIncLocalBend);

          }//end j-node loop
        dstrain_nl.Zero();//add for accelerate the convergence,but not zero in stability analysis problems.
        dstrain(0)= dstrain_li(0)+dstrain_nl(0);
        dstrain(1)= dstrain_li(1)+dstrain_nl(1);
        dstrain(2)= dstrain_li(2)+dstrain_nl(2);
        dstrain(3)= dstrain_li(3);
        dstrain(4)= dstrain_li(4);
        dstrain(5)= dstrain_li(5);
        dstrain(6)= dstrain_li(6);
        dstrain(7)= dstrain_li(7);

        //strain= Cstrain + dstrain;
        for(int q=0;q<nstress;q++)
          { strain(q)= Cstrain(q) + dstrain(q); }//end for q

	// not very elegant. Maybe formResidAndTangent must not be const. 
        SectionForceDeformation *mat_no_const= const_cast<SectionForceDeformation *>(physicalProperties[i]);
        success= mat_no_const->setTrialSectionDeformation(strain);

        //compute the stress
        stress= physicalProperties[i]->getStressResultant();

        //add for geometric nonlinearity
        //update strain in gauss points
        //define TstrainGauss
        for(int jnew=0;jnew < nstress;jnew++)
          { TstrainGauss(i*8+jnew)= strain(jnew); }

        //add for geometric nonlinearity
        //from stress(0,1,2) to compute membraneForce at t
        membraneForce(0,0)= stress(0);
        membraneForce(1,1)= stress(1);
        membraneForce(0,1)= stress(2);
        membraneForce(1,0)= stress(2);

        stress*= dvol[i];//for Resid integration
        membraneForce*= dvol[i]; //for stiffJKgeo integration

        //dd *= dvol[i];
        if(tangFlag == 1)
          {
            dd= physicalProperties[i]->getSectionTangent();
            dd *= dvol[i];//for stiffJKlinear integration
          }//end if tangFlag

        //residual and tangent calculations node loops

        //jj loop----------------
        int jj= 0;
        for(int j=0; j<numnodes;j++)
          {
             //extract BJ
            for(int p=0;p<nstress;p++)
              {
                for(int q=0; q<ndf;q++)
                   BJ(p,q)=saveB[p][q][j];
              }// end for p

            // multiply bending terms by -1.0 for correct statement
            // of equilibrium
            for(int p=3;p<6;p++)
              {
                for(int q=3; q<6; q++)
                  BJ(p,q) *= (-1.0);
              }//end for p

            // transpose BJtran=transpose(BJ);
            for(int p=0;p<ndf;p++)
              {
                for(int q=0;q<nstress;q++)
                  BJtran(p,q)=BJ(q,p);
              }//end for p

            // compute residual force - no need to modify for
            // geometric nonlinearity
            residJlocal.addMatrixVector(0.0,BJtran,stress,1.0);
            residJ1.addMatrixVector(0.0,PmatTran,residJlocal,1.0);
            residJ.addMatrixVector(0.0,TmatTran,residJ1,1.0);

            for(int p=0; p<ndf; p++)
              resid(jj+p) += residJ(p);
            //add for geometric nonlinearity
            BGJ= computeBG(j,shpBend);
            //transpose BGJ
            for(int p3=0; p3<3; p3++)
              {
                for(int q3=0; q3<2; q3++)
                  BGJtran(p3,q3)=BGJ(q3,p3);
              }//end for p3
            stiffBGM.addMatrixProduct(0.0,BGJtran,membraneForce,1.0);

            //BJtranD= BJtran *dd;
            if(tangFlag == 1)
              {
                BJtranD.addMatrixProduct(0.0,BJtran,dd,1.0);

                //k loop------------
                int kk=0;
                for(int k=0; k<numnodes; k++)
                  {
                    //extract BK
                    for(int p=0; p<nstress;p++)
		      {
                        for(int q=0;q<ndf;q++)
                          BK(p,q)= saveB[p][q][k];
                      }// end for p

                    //stiffJKlinear= BJtranD * BK - modify for geometric nonlinearity
                    stiffJKlinear.addMatrixProduct(0.0,BJtranD,BK,1.0);

		    //add for geometric nonlinearity
		    //extract BGK
		    BGK= computeBG(k,shpBend);
		    //stiffGeo= BGJtran * membraneForce *BGK
		    stiffJKgeo.addMatrixProduct(0.0,stiffBGM,BGK,1.0);

		    //stiffJKlocal= stiffJKlinear + stiffJKgeo
		    stiffJKlocal= stiffJKlinear;
		    for(int pp=3;pp<6;pp++)
		      {
			for(int qq=3;qq<6;qq++)
			  stiffJKlocal(pp,qq) += stiffJKgeo(pp-3,qq-3);
		      }//end for pp

                    //transpose dof and coordinates
                    stiffJK1.addMatrixProduct(0.0,PmatTran,stiffJKlocal,1.0);
                    stiffJK2.addMatrixProduct(0.0,stiffJK1,Pmat,1.0);
                    stiffJK3.addMatrixProduct(0.0,TmatTran,stiffJK2,1.0);
                    stiffJK.addMatrixProduct(0.0,stiffJK3,Tmat,1.0);

		    //generate stiff
		    //modify for geometric nonlinearity
		    for(int p=0;p<ndf;p++)
		      {
		        for(int q=0;q<ndf;q++)
			  { stiff(jj+p,kk+q) += stiffJK(p,q); }//end for q
		      }//end for p

                    kk += ndf;
		  }//end for k loop
	      }//end if tangFlag
            jj += ndf;
	  }// end for j loop
      }//end for i gauss loop
  }


//start Yuli Huang (yulihuang@gmail.com) & Xinzheng Lu (luxz@tsinghua.edu.cn)
//************************************************************************
//! @brief compute local coordinates and basis
void XC::ShellNLDKGQ::updateBasis(void) const
  { theCoordTransf->update(); }
//end Yuli Huang (yulihuang@gmail.com) & Xinzheng Lu (luxz@tsinghua.edu.cn)

//! @brief assemble a B matrix
const XC::Matrix &XC::ShellNLDKGQ::assembleB(const Matrix &Bmembrane, const Matrix &Bbend, const Matrix &Bshear) const
  {
    static Matrix B(8,6);

    // For Shell :
    //
    //---B Matrices in standard {1,2,3} mechanics notation---------
    //
    //            -                     _
    //           | Bmembrane  |     0    |
    //           | --------------------- |
    //    B=    |     0      |  Bbend   |   (8x6)
    //           | --------------------- |
    //           |            |  Bshear  |
    //            -           -         -
    //
    //-------------------------------------------------------------
    B.Zero();
    //assemble B from sub-matrices

    //membrane parts
    for(int p=0; p<3; p++)
      {
        for(int q=0; q<3; q++)
          B(p,q)= Bmembrane(p,q);
      }//end for p

    //bending parts
    for(int p=3; p<6; p++)
      {
        int pp= p-3;
	for(int q=3; q<6;q++)
	  B(p,q)=Bbend(pp,q-3);
      }//end for p

    //shear parts
    for(int p=0;p<2;p++)
      {
	int pp= p+6;

	for(int q=3; q<6; q++)
	  B(pp,q)=Bshear(p,q-3);
      }//end for p
    return B;
  }


//! @brief compute Bmembrane matrix
const XC::Matrix &XC::ShellNLDKGQ::computeBmembrane(int node, const double shp[3][4], const double shpDrill[4][4]) const
  {
    static Matrix Bmembrane(3,3);

    // ------Bmembrane Matrix in standard {1,2,3} mechanics notation --------
    //
    //               | N,1    0      Nu,1  |
    //   Bmembrane= | 0     N,2     Nv,2  |
    //               | N,2   N,1  Nv,1+Nu,2|
    // ----------------------------------------------------------------------

    Bmembrane.Zero();
    Bmembrane(0,0)= shp[0][node];
    Bmembrane(0,2)= shpDrill[0][node];
    Bmembrane(1,1)= shp[1][node];
    Bmembrane(1,2)= shpDrill[3][node];
    Bmembrane(2,0)= shp[1][node];
    Bmembrane(2,1)= shp[0][node];
    Bmembrane(2,2)= shpDrill[1][node]+shpDrill[2][node];
    return Bmembrane;
  }


//! @brief compute Bbend matrix
const XC::Matrix &XC::ShellNLDKGQ::computeBbend(int node ,const double shpBend[6][12]) const
  {
     static Matrix Bbend(3,3);

     //----------Bbend Matrix in standard {1,2,3}mechanics notation------------------
     //
     //               |
     //      Bbend=  |  Hx[3*node],1  Hx[3*node + 1],1  Hx[3*node + 2],1  |
     //               |  Hy[3*node],2  Hy[3*node + 1],2  Hy[3*node + 2],2  |
     //               |  Hx[3*node],2  Hx[3*node + 1],2  Hx[3*node + 2],2  |
     //               |  + Hy[3*node],1 +Hy[3*node + 1],1 +Hy[3*node + 2],1|
     //------------------------------------------------------------------------------
     const int i= 3*node;
     const int j= 3*node + 1;
     const int k= 3*node + 2;

     Bbend.Zero();

     Bbend(0,0)= shpBend[2][i];
     Bbend(0,1)= shpBend[2][j];
     Bbend(0,2)= shpBend[2][k];

     Bbend(1,0)= shpBend[5][i];
     Bbend(1,1)= shpBend[5][j];
     Bbend(1,2)= shpBend[5][k];

     Bbend(2,0)= shpBend[3][i] + shpBend[4][i];
     Bbend(2,1)= shpBend[3][j] + shpBend[4][j];
     Bbend(2,2)= shpBend[3][k] + shpBend[4][k];
     /*
     bugfix: Massimo Petracca 02/26/2020. with the original implementation, 
     the curvatures sent to the section had the wrong sign.
     */
     Bbend*= -1.0;
     return Bbend;
   }

//! @brief compute BG matrix
const XC::Matrix &XC::ShellNLDKGQ::computeBG(int node ,const double shpBend[6][12]) const
  {
     static Matrix BG(2,3);

     //----------BG Matrix in standard {1,2,3}mechanics notation------------------
     //
     //               |
     //      BG=     |  -Hx[3*node]  -Hx[3*node + 1]  -Hx[3*node + 2]  |
     //               |  -Hy[3*node]  -Hy[3*node + 1]  -Hy[3*node + 2]  |
     //------------------------------------------------------------------------------
     const int i= 3*node;
     const int j= 3*node + 1;
     const int k= 3*node + 2;

     BG.Zero();

     BG(0,0)= -shpBend[0][i];
     BG(0,1)= -shpBend[0][j];
     BG(0,2)= -shpBend[0][k];

     BG(1,0)= -shpBend[1][i];
     BG(1,1)= -shpBend[1][j];
     BG(1,2)= -shpBend[1][k];
     return BG;
   }

//! @brief compute the nonlinearity strain Increment associated with BG & bending
const XC::Vector &XC::ShellNLDKGQ::computeNLdstrain(const Matrix &BG,const Vector &dispIncLocalBend) const
  {
    static Vector dstrain_nl(3);
    static Vector strainInc(2);

    strainInc.addMatrixVector(0.0,BG,dispIncLocalBend,1.0);

    dstrain_nl(0)= strainInc(0) * strainInc(0) * 0.5;
    dstrain_nl(1)= strainInc(1) * strainInc(1) * 0.5;
    dstrain_nl(2)= strainInc(0) * strainInc(1);

    return dstrain_nl;
  }

//! @brief shape function routine for four node quads
void XC::ShellNLDKGQ::shape2d( double ss, double tt, const double x[2][4],
                           double shp[3][4],
                           double &xsj, double sx[2][2] )
  {
    static const double s[]= { -0.5,  0.5, 0.5, -0.5 };
    static const double t[]= { -0.5, -0.5, 0.5,  0.5 };

    static double xs[2][2];
    // static double sx[2][2];  //have been defined before

    for(int i= 0; i < 4; i++ )
      {
	shp[2][i]= ( 0.5 + s[i]*ss )*( 0.5 + t[i]*tt ); //shape function for 2d isoparametric element
	shp[0][i]= s[i] * ( 0.5 + t[i]*tt ); // derivative to isoparametric coordinates 1
	shp[1][i]= t[i] * ( 0.5 + s[i]*ss ); // derivative to isoparametric coordinates 2
      } // end for i


    // Construct jacobian and its inverse

    for(int i= 0; i < 2; i++ )
      {
        for(int j= 0; j < 2; j++ )
	  {
  	    xs[i][j]= 0.0;
  	    for(int k= 0; k < 4; k++ )
	      xs[i][j] +=  x[i][k] * shp[j][k];
	  } //end for j
      }  // end for i

    xsj= xs[0][0]*xs[1][1] - xs[0][1]*xs[1][0];

    //inverse jacobian
    double jinv= 1.0/xsj;
    sx[0][0]=  xs[1][1] * jinv;
    sx[1][1]=  xs[0][0] * jinv;
    sx[0][1]= -xs[0][1] * jinv;
    sx[1][0]= -xs[1][0] * jinv;

    //form global derivatives, local coordinates

    double temp= 0.0;
    for(int i= 0; i < 4; i++ )
      {
        temp= shp[0][i]*sx[0][0] + shp[1][i]*sx[1][0];
        shp[1][i]= shp[0][i]*sx[0][1] + shp[1][i]*sx[1][1];
        shp[0][i]= temp;
      } // end for i

    return;
  }


/*Matrix
XC::ShellNLDKGQ::transpose( int dim1,
                                       int dim2,
                                       const Matrix &M )
{
  int i;
  int j;

  Matrix Mtran( dim2, dim1 );

  for(int i= 0; i < dim1; i++ ) {
     for(int j= 0; j < dim2; j++ )
         Mtran(j,i)= M(i,j);
  } // end for i

  return Mtran;
}
*/

//! @brief  shape function for drill dof
void XC::ShellNLDKGQ::shapeDrill(double ss, double tt,
                                          const double x[2][4],
                                          double sx[2][2], double shpDrill[4][4])
  {
    double a1, a2, a3;
    double b1, b2, b3;

    static const double s[]= {-1.0, 1.0, 1.0, -1.0};
    static const double t[]= {-1.0, -1.0, 1.0, 1.0};

    double shptemp[4][4];   //derivative to xi,eta

    const double one_over_four= 1/4.0;
    const double one_over_eight= 1/8.0;
    //compute a1-a3,b1-b3
    a1= 0.0;
    a2= 0.0;
    a3= 0.0;
    b1= 0.0;
    b2= 0.0;
    b3= 0.0;
    for(int i=0;i<4;i++)
      {
	a1 += s[i] * x[0][i] * one_over_four;
	a2 += t[i] * x[0][i] * one_over_four;
	a3 += s[i] * t[i] * x[0][i] * one_over_four;
	b1 += s[i] * x[1][i] * one_over_four;
	b2 += t[i] * x[1][i] * one_over_four;
	b3 += s[i] * t[i] * x[1][i] * one_over_four;
      }//end for i

    //compute the derivatives of shape function to xi, eta
    // shptemp[0][j]= Nu,xi   shptemp[1][j]= Nu,eta
    // shptemp[2][j]= Nv,xi   shptemp[3][j]= Nv,eta
    for(int j =0; j<4; j++)
      {
	shptemp[0][j]= one_over_eight * (-2.0*s[j]*ss*(b1+b3*t[j])*(1.0+t[j]*tt)+
			  s[j]*t[j]*(1.0-tt*tt)*(b2+b3*s[j]));
	shptemp[1][j]= one_over_eight *(s[j]*t[j]*(1.0-ss*ss)*(b1+b3*t[j])-
			  2.0*t[j]*tt*(b2+b3*s[j])*(1.0+s[j]*ss));
	shptemp[2][j]= -one_over_eight * (-2.0*s[j]*ss*(a1+a3*t[j])*(1.0+t[j]*tt)+
			  s[j]*t[j]*(1.0-tt*tt)*(a2+a3*s[j]));
	shptemp[3][j]= -one_over_eight *(s[j]*t[j]*(1.0-ss*ss)*(a1+a3*t[j])-
			  2.0*t[j]*tt*(a2+a3*s[j])*(1.0+s[j]*ss));
      }// end for j

    //define shpdrill
    //           |  Nu,1  |
    //shpDrill= |  Nu,2  |
    //           |  Nv,1  |
    //           |  Nv,2  |
    for(int k=0; k<4; k++)
      {
	shpDrill[0][k]= shptemp[0][k] * sx[0][0]+
			    shptemp[1][k] * sx[1][0];
	shpDrill[1][k]= shptemp[0][k] * sx[0][1]+
			    shptemp[1][k] * sx[1][1];
	shpDrill[2][k]= shptemp[2][k] * sx[0][0]+
			    shptemp[3][k] * sx[1][0];
	shpDrill[3][k]= shptemp[2][k] * sx[0][1]+
			    shptemp[3][k] * sx[1][1];
      }//end for k
  }


//*********************************************************************
//! @brief shape function for bending plate
void XC::ShellNLDKGQ::shapeBend(double ss, double tt, const double x[2][4],
                                         double sx[2][2], double shpBend[6][12])
  {
    //static Vector N(8);
    //static Vector Nxi(8);
    //static Vector Neta(8);
    static double N[3][8];

    static double temp[4][12];

    const double one_over_four= 1.0/4.0;

    //define xij,yij,lij
    const double x12= x[0][0] - x[0][1];
    const double x23= x[0][1] - x[0][2];
    const double x34= x[0][2] - x[0][3];
    const double x41= x[0][3] - x[0][0];

    const double y12= x[1][0] - x[1][1];
    const double y23= x[1][1] - x[1][2];
    const double y34= x[1][2] - x[1][3];
    const double y41= x[1][3] - x[1][0];

    const double L12= x12 * x12 + y12 * y12;
    const double L23= x23 * x23 + y23 * y23;
    const double L34= x34 * x34 + y34 * y34;
    const double L41= x41 * x41 + y41 * y41;

    //a5-8,b5-8,c5-8,d5-8,e5-8
    const double a5= - x12/L12;
    const double b5= one_over_four * 3.0 * x12 * y12/L12;
    const double c5= one_over_four *(x12*x12 - 2.0*y12*y12)/L12;
    const double d5= - y12/L12;
    const double e5= one_over_four *(y12*y12 - 2.0*x12*x12)/L12;

    const double a6= - x23/L23;
    const double b6= one_over_four * 3.0 * x23 * y23/L23;
    const double c6= one_over_four *(x23*x23 - 2.0*y23*y23)/L23;
    const double d6= - y23/L23;
    const double e6= one_over_four *(y23*y23 - 2.0*x23*x23)/L23;

    const double a7= - x34/L34;
    const double b7= one_over_four * 3.0 * x34 * y34/L34;
    const double c7= one_over_four *(x34*x34 - 2.0*y34*y34)/L34;
    const double d7= - y34/L34;
    const double e7= one_over_four *(y34*y34 - 2.0*x34*x34)/L34;

    const double a8= - x41/L41;
    const double b8= one_over_four * 3.0 * x41 * y41/L41;
    const double c8= one_over_four *(x41*x41 - 2.0*y41*y41)/L41;
    const double d8= - y41/L41;
    const double e8= one_over_four *(y41*y41 - 2.0*x41*x41)/L41;

    //define 3-d isoparametric shape function
    N[0][0]= - one_over_four * (1.0-ss) * (1.0-tt) * (1.0+ss+tt);
    N[0][1]= - one_over_four * (1.0+ss) * (1.0-tt) * (1.0-ss+tt);
    N[0][2]= - one_over_four * (1.0+ss) * (1.0+tt) * (1.0-ss-tt);
    N[0][3]= - one_over_four * (1.0-ss) * (1.0+tt) * (1.0+ss-tt);
    N[0][4]= (1.0-ss*ss) * (1.0-tt) *2.0 * one_over_four;
    N[0][5]= (1.0+ss) * (1.0-tt*tt) *2.0 * one_over_four;
    N[0][6]= (1.0-ss*ss) * (1.0+tt) *2.0 * one_over_four;
    N[0][7]= (1.0-ss) * (1.0-tt*tt) *2.0 * one_over_four;
    //N,xi - deravatives
    N[1][0]= one_over_four * (1.0-tt) * (tt+2.0*ss);
    N[1][1]= one_over_four * (1.0-tt) * (2.0*ss-tt);
    N[1][2]= one_over_four * (1.0+tt) * (2.0*ss+tt);
    N[1][3]= one_over_four * (1.0+tt) * (2.0*ss-tt);
    N[1][4]= -ss * (1.0-tt);
    N[1][5]= (1.0-tt*tt)*2.0*one_over_four;
    N[1][6]= -ss * (1.0 + tt);
    N[1][7]= -(1.0-tt*tt)*2.0*one_over_four;
    //N,eta - deravitaves
    N[2][0]= (1.0-ss) * (ss+2.0*tt) * one_over_four;
    N[2][1]= (1.0+ss) * (2.0*tt-ss) * one_over_four;
    N[2][2]= (1.0+ss) * (2.0*tt+ss) * one_over_four;
    N[2][3]= (1.0-ss) * (2.0*tt-ss) * one_over_four;
    N[2][4]= -(1.0-ss*ss) * 2.0 * one_over_four;
    N[2][5]= - tt * (1.0 + ss);
    N[2][6]= (1.0-ss*ss) * 2.0 * one_over_four;
    N[2][7]= - tt * (1.0 - ss);
    //Hx, Hy
    const double three_over_two= 3.0/2.0;
    //Hx
    shpBend[0][0]= (a5*N[0][4]-a8*N[0][7]) * three_over_two;
    shpBend[0][1]= b5*N[0][4] + b8*N[0][7];
    shpBend[0][2]= N[0][0] - c5*N[0][4] - c8*N[0][7];

    shpBend[0][3]= (a6*N[0][5]-a5*N[0][4]) * three_over_two;
    shpBend[0][4]= b6*N[0][5] + b5*N[0][4];
    shpBend[0][5]= N[0][1] - c6*N[0][5] - c5*N[0][4];

    shpBend[0][6]= (a7*N[0][6]-a6*N[0][5]) * three_over_two;
    shpBend[0][7]= b7*N[0][6] + b6*N[0][5];
    shpBend[0][8]= N[0][2] - c7*N[0][6] - c6*N[0][5];

    shpBend[0][9]= (a8*N[0][7]-a7*N[0][6]) * three_over_two;
    shpBend[0][10]= b8*N[0][7] + b7*N[0][6];
    shpBend[0][11]= N[0][3] - c8*N[0][7] - c7*N[0][6];
    //Hy
    shpBend[1][0]= (d5*N[0][4]-d8*N[0][7])* three_over_two;
    shpBend[1][1]= -N[0][0] + e5*N[0][4] + e8*N[0][7];
    shpBend[1][2]= -b5*N[0][4] - b8*N[0][7];

    shpBend[1][3]= (d6*N[0][5]-d5*N[0][4])* three_over_two;
    shpBend[1][4]= -N[0][1] + e6*N[0][5] + e5*N[0][4];
    shpBend[1][5]= -b6*N[0][5] - b5*N[0][4];

    shpBend[1][6]= (d7*N[0][6]-d6*N[0][5])* three_over_two;
    shpBend[1][7]= -N[0][2] + e7*N[0][6] + e6*N[0][5];
    shpBend[1][8]= -b7*N[0][6] - b6*N[0][5];

    shpBend[1][9]= (d8*N[0][7]-d7*N[0][6])* three_over_two;
    shpBend[1][10]= -N[0][3] + e8*N[0][7] + e7*N[0][6];
    shpBend[1][11]= -b8*N[0][7] - b7*N[0][6];


    //Hx,xi  Hx,eta  Hy,xi  Hy,eta - temp[4][12]
    //Hx,xi
    temp[0][0]= (a5*N[1][4]-a8*N[1][7]) * three_over_two;
    temp[0][1]= b5*N[1][4] + b8*N[1][7];
    temp[0][2]= N[1][0] - c5*N[1][4] - c8*N[1][7];

    temp[0][3]= (a6*N[1][5]-a5*N[1][4]) * three_over_two;
    temp[0][4]= b6*N[1][5] + b5*N[1][4];
    temp[0][5]= N[1][1] - c6*N[1][5] - c5*N[1][4];

    temp[0][6]= (a7*N[1][6]-a6*N[1][5]) * three_over_two;
    temp[0][7]= b7*N[1][6] + b6*N[1][5];
    temp[0][8]= N[1][2] - c7*N[1][6] - c6*N[1][5];

    temp[0][9]= (a8*N[1][7]-a7*N[1][6]) * three_over_two;
    temp[0][10]= b8*N[1][7] + b7*N[1][6];
    temp[0][11]= N[1][3] - c8*N[1][7] - c7*N[1][6];
    //Hx,eta
    temp[1][0]= (a5*N[2][4]-a8*N[2][7]) * three_over_two;
    temp[1][1]= b5*N[2][4] + b8*N[2][7];
    temp[1][2]= N[2][0] - c5*N[2][4] - c8*N[2][7];

    temp[1][3]= (a6*N[2][5]-a5*N[2][4]) * three_over_two;
    temp[1][4]= b6*N[2][5] + b5*N[2][4];
    temp[1][5]= N[2][1] - c6*N[2][5] - c5*N[2][4];

    temp[1][6]= (a7*N[2][6]-a6*N[2][5]) * three_over_two;
    temp[1][7]= b7*N[2][6] + b6*N[2][5];
    temp[1][8]= N[2][2] - c7*N[2][6] - c6*N[2][5];

    temp[1][9]= (a8*N[2][7]-a7*N[2][6]) * three_over_two;
    temp[1][10]= b8*N[2][7] + b7*N[2][6];
    temp[1][11]= N[2][3] - c8*N[2][7] - c7*N[2][6];
    //Hy,xi
    temp[2][0]= (d5*N[1][4]-d8*N[1][7])* three_over_two;
    temp[2][1]= -N[1][0] + e5*N[1][4] + e8*N[1][7];
    temp[2][2]= -b5*N[1][4] - b8*N[1][7];

    temp[2][3]= (d6*N[1][5]-d5*N[1][4])* three_over_two;
    temp[2][4]= -N[1][1] + e6*N[1][5] + e5*N[1][4];
    temp[2][5]= -b6*N[1][5] - b5*N[1][4];

    temp[2][6]= (d7*N[1][6]-d6*N[1][5])* three_over_two;
    temp[2][7]= -N[1][2] + e7*N[1][6] + e6*N[1][5];
    temp[2][8]= -b7*N[1][6] - b6*N[1][5];

    temp[2][9]= (d8*N[1][7]-d7*N[1][6])* three_over_two;
    temp[2][10]= -N[1][3] + e8*N[1][7] + e7*N[1][6];
    temp[2][11]= -b8*N[1][7] - b7*N[1][6];


    //Hy,eta
    temp[3][0]= (d5*N[2][4]-d8*N[2][7])* three_over_two;
    temp[3][1]= -N[2][0] + e5*N[2][4] + e8*N[2][7];
    temp[3][2]= -b5*N[2][4] - b8*N[2][7];

    temp[3][3]= (d6*N[2][5]-d5*N[2][4])* three_over_two;
    temp[3][4]= -N[2][1] + e6*N[2][5] + e5*N[2][4];
    temp[3][5]= -b6*N[2][5] - b5*N[2][4];

    temp[3][6]= (d7*N[2][6]-d6*N[2][5])* three_over_two;
    temp[3][7]= -N[2][2] + e7*N[2][6] + e6*N[2][5];
    temp[3][8]= -b7*N[2][6] - b6*N[2][5];

    temp[3][9]= (d8*N[2][7]-d7*N[2][6])* three_over_two;
    temp[3][10]= -N[2][3] + e8*N[2][7] + e7*N[2][6];
    temp[3][11]= -b8*N[2][7] - b7*N[2][6];

        //Hx,x   Hx,y    Hy,x   Hy,y
    for(int i=0;i<12;i++)
      {
	shpBend[2][i]= temp[0][i]*sx[0][0] +temp[1][i]*sx[1][0]; //Hx,x
	shpBend[3][i]= temp[0][i]*sx[0][1] +temp[1][i]*sx[1][1]; //Hx,y
	shpBend[4][i]= temp[2][i]*sx[0][0] +temp[3][i]*sx[1][0]; //Hy,x
	shpBend[5][i]= temp[2][i]*sx[0][1] +temp[3][i]*sx[1][1]; //Hy,y
      }//end for i
  }

//! @brief Send members through the communicator argument.
int XC::ShellNLDKGQ::sendData(Communicator &comm)
  {
    int res= Shell4NBase::sendData(comm);
    res+= comm.sendVector(CstrainGauss,getDbTagData(),CommMetaData(16));
    res+= comm.sendVector(TstrainGauss,getDbTagData(),CommMetaData(17));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ShellNLDKGQ::recvData(const Communicator &comm)
  {
    int res= Shell4NBase::recvData(comm);
    res+= comm.receiveVector(CstrainGauss,getDbTagData(),CommMetaData(16));
    res+= comm.receiveVector(TstrainGauss,getDbTagData(),CommMetaData(17));
    return res;
  }

//! @brief Sends object through the communicator argument.
int  XC::ShellNLDKGQ::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(18);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; failed to send data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int  XC::ShellNLDKGQ::recvSelf(const Communicator &comm)
  {
    inicComm(18);

    const int dbTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << getClassName() << "::" << __FUNCTION__
                     << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief print out element data
void XC::ShellNLDKGQ::Print(std::ostream &s, int flag) const
  {
    if(flag == -1)
      {
        int eleTag= this->getTag();
        s << "EL_" << getClassName() << "\t" << eleTag << "\t";
        s << eleTag << "\t" << 1;
        s << "\t" << theNodes(0) << "\t" << theNodes(1);
        s << "\t" << theNodes(2) << "\t" << theNodes(3) << "\t0.00";
        s << std::endl;
        s << "PROP_3D\t" << eleTag << "\t";
        s << eleTag << "\t" << 1;
        s << "\t" << -1 << "\tSHELL\t1.0\0.0";
        s << std::endl;
      }
    if(flag < -1)
      {
        int counter= (flag + 1) * -1;
        int eleTag= this->getTag();
        int i, j;
        for(i= 0; i < 4; i++)
          {
            const Vector &stress= physicalProperties[i]->getStressResultant();

            s << "STRESS\t" << eleTag << "\t" << counter << "\t" << i << "\tTOP";
            for(j= 0; j < 6; j++)
                s << "\t" << stress(j);
            s << std::endl;
          }
      }
    // if(flag == OPS_PRINT_CURRENTSTATE)
    //   {
    //     s << std::endl;
    //     s << "NLDKGQ Non-Locking Four Node Shell \n";
    //     s << "Element Number: " << this->getTag() << std::endl;
    //     s << "Node 1 : " << theNodes(0) << std::endl;
    //     s << "Node 2 : " << theNodes(1) << std::endl;
    //     s << "Node 3 : " << theNodes(2) << std::endl;
    //     s << "Node 4 : " << theNodes(3) << std::endl;

    //     s << "Material Information : \n ";
    //     physicalProperties[0]->Print(s, flag);

    //     s << std::endl;
    //   }
    // if(flag == OPS_PRINT_PRINTMODEL_JSON)
    //   {
    //     s << "\t\t\t{";
    //     s << "\"name\": " << this->getTag() << ", ";
    //     s << "\"type\": \"ShellNLDKGQ\", ";
    //     s << "\"nodes\": [" << theNodes(0) << ", " << theNodes(1) << ", ";
    //     s << theNodes(2) << ", " << theNodes(3) << "], ";
    //     s << "\"section\": \"" << physicalProperties[0]->getTag() << "\"}";
    //   }
  }
