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

// $Source: /usr/local/cvs/OpenSees/SRC/element/zeroLength/ZeroLengthContact2D.cpp,v $
// $Revision: 1.4 $
// $Date: 2008/06/13 21:01:03 $

// Written: Gang Wang  (wang@ce.berkeley.edu)
//          Prof. Nicholas Sitar (nsitar@ce.berkeley.edu)
//
// Created: 27/08/2003

/*
 element zeroLengthContact2D  $eleID $sNdID $mNdID $Kn $Kt $fs -normal $Nx $Ny
 Description: This file contains the implementation for the ZeroLengthContact2D class.
*/

#include "ZeroLengthContact2D.h"
#include "domain/mesh/element/utils/Information.h"
#include "utility/recorder/response/ElementResponse.h"
#include "domain/mesh/node/Node.h"

// static data for 2D
XC::Matrix  XC::ZeroLengthContact2D::stiff(2*XC::ZeroLengthContact2D::numNodos(),2*XC::ZeroLengthContact2D::numNodos()) ;
XC::Vector  XC::ZeroLengthContact2D::resid(2*XC::ZeroLengthContact2D::numNodos()) ;
XC::Matrix  XC::ZeroLengthContact2D::zeroMatrix(2*XC::ZeroLengthContact2D::numNodos(),2*XC::ZeroLengthContact2D::numNodos()) ;

//! @brief Constructor.
XC::ZeroLengthContact2D::ZeroLengthContact2D(int tag,int Nd1, int Nd2,double Knormal, double Ktangent, double frictionRatio,  const Vector& normal )
  :ZeroLengthContact(tag,ELE_TAG_ZeroLengthContact2D,2,Nd1,Nd2,Knormal,Ktangent,frictionRatio),
   lambda(0.0), T(2*numNodos()), ContactNormal(2), stickPt(0.0), xi(0.0)
  {
    // assign outward contact normal of master block
    ContactNormal(0) = normal(0)/normal.Norm();
    ContactNormal(1) = normal(1)/normal.Norm();
  }

//! @brief Default constructor.
XC::ZeroLengthContact2D::ZeroLengthContact2D(int tag)
  :ZeroLengthContact(tag,ELE_TAG_ZeroLengthContact2D,2), lambda(0.0), T(2*numNodos()), ContactNormal(2), stickPt(0.0), xi(0.0)
  {}

//! @brief Virtual constructor.
XC::Element* XC::ZeroLengthContact2D::getCopy(void) const
  { return new ZeroLengthContact2D(*this); }

//! @brief to set a link to the enclosing Domain and to set the node pointers.
//!    also determines the number of dof associated
//!    with the ZeroLengthContact2D element
void XC::ZeroLengthContact2D::setDomain(Domain *theDomain)
  {

    // call the base class method
    ZeroLengthContact::setDomain(theDomain);


    // now determine the number of dof and the dimension
    const int dofNd1 = theNodes[0]->getNumberDOF();
    const int dofNd2 = theNodes[1]->getNumberDOF();

    // if differing dof at the ends - print a warning message
    if( dofNd1 != dofNd2 )
      {
        std::cerr << "WARNING XC::ZeroLengthContact2D::setDomain(): nodes " 
                  << theNodes[0]->getTag() << " and " << theNodes[1]->getTag()
                  << " have differing dof at ends for ZeroLengthContact2D "
                  << this->getTag() << std::endl;
        return;
      }

    // Check that length is zero within tolerance
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    Vector diff = end1Crd - end2Crd;
    double L  = diff.Norm();
    double v1 = end1Crd.Norm();
    double v2 = end2Crd.Norm();
    double vm= (v1<v2) ? v2 : v1;
    const double lentol_vm= LenTol*vm;


    if(L > lentol_vm)
      std::cerr << "WARNING XC::ZeroLengthContact2D::setDomain(): Element "
                << this->getTag() << " has L= " << L
                << ", which is greater than the tolerance\n";


    if(dofNd1 == 2 && dofNd2 == 2)
      { numDOF = 4;}
    else
      {
        std::cerr << "WARNING XC::ZeroLengthContact2D::setDomain cannot handle "
                  << dofNd1 << "dofs at nodes in " << dofNd1 << " d problem\n";
        return;
      }
  }

int XC::ZeroLengthContact2D::commitState(void)
  {
    // need to update stick point here
    if(ContactFlag == 2 )   // slide case, update stick point
      stickPt=xi;

    gap_n = gap;

    ////////////////////////////////////
    // initialize lagrange mulitplier zero for next iterations.
    // lambda = pressure; // using for augmented lagrange
    lambda = 0; // using penalty method only
    ////////////////////////////////////

    return 0;
  }



int XC::ZeroLengthContact2D::revertToLastCommit(void)
  {

    ///////////////////////////////////////////
    // need to revert the stickPoint??
    //int code=0;
    // revert state for 1d materials
    //code+= physicalProperties.revertToLastCommit();
    //return code;

    //xi=stickPt;

    return 0;
  }


int XC::ZeroLengthContact2D::revertToStart(void)
  {

    // need to rezero stickPoint??
    //int code=0;
    // revert to start for 1d materials
    //code+= physicalProperties.revertToStart();
    //return code;
    // zero stickPt

    stickPt=0;
    return 0;
  }





//! @ brief update
//! calculate stress-strain relation -- M. Frank
int XC::ZeroLengthContact2D::update(void)
  {
    //////////////////////////////////////////////////////////
    //std::cerr<<"contact2D update " << std::endl;
    //lambda   = 990 ; // add for augmented lagrange
    //std::cerr<<"contact2D lambda = " << lambda <<std::endl;
    ///////////////////////////////////////////////////////////
    return 0;
  }

const XC::Matrix &XC::ZeroLengthContact2D::getTangentStiff(void) const
  {
    //std::cerr<< this->getTag()<< " XC::ZeroLengthContact2D::getTangentStiff()" <<std::endl;
    int tang_flag = 1 ; //get the tangent

    //do tangent and residual here
    formResidAndTangent(tang_flag);

    //std::cerr<< stiff ;
    if(isDead())
      stiff*=dead_srf;
    return stiff ;
  }

const XC::Matrix &XC::ZeroLengthContact2D::getInitialStiff(void) const
  {
    //std::cerr<<this->getTag()<< " XC::ZeroLengthContact2D::getInitialStiff()" <<std::endl;
    int tang_flag = 1 ; //get the tangent
    //do tangent and residual here
    formResidAndTangent(tang_flag);
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }

const XC::Matrix &XC::ZeroLengthContact2D::getDamp(void) const
  {
    // no damp
    zeroMatrix.Zero();
    return zeroMatrix;
  }

const XC::Matrix &XC::ZeroLengthContact2D::getMass(void) const
  {
    // no mass
    zeroMatrix.Zero();
    return zeroMatrix;
  }

const XC::Vector &XC::ZeroLengthContact2D::getResistingForce(void) const
  {
    //std::cerr<< this->getTag()<< " XC::ZeroLengthContact2D::getResistingForce()" <<std::endl;
    int tang_flag = 0 ; //don't get the tangent
    formResidAndTangent( tang_flag ) ;
    //std::cerr<< "resid="<<resid;
    if(isDead())
      resid*=dead_srf;
    return resid ;
  }


const XC::Vector &XC::ZeroLengthContact2D::getResistingForceIncInertia(void) const
  {
    // there is no Inertia
    //std::cerr<< this->getTag()<< " XC::ZeroLengthContact2D::getResistingForceIncInertia()" <<std::endl;
    int tang_flag = 0 ; //don't get the tangent
    formResidAndTangent( tang_flag ) ;
    //std::cerr<< resid;
    if(isDead())
      resid*=dead_srf;
    return  resid ;
  }


int XC::ZeroLengthContact2D::sendSelf(CommParameters &cp)
  {
    // doing nothing here
    return 0;
  }

int XC::ZeroLengthContact2D::recvSelf(const CommParameters &cp)
  {
    // doing nothing here
    return 0;
  }


void XC::ZeroLengthContact2D::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      { // print everything
        s << "Element: " << this->getTag()
          << " type: ZeroLengthContact2D  Nodes: "
          << theNodes << std::endl;
      }
    else if(flag == 1)
      {
        s << this->getTag() << "  ";
      }
  }

XC::Response *XC::ZeroLengthContact2D::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    if(argv[0] == "force" || argv[0] == "forces")
      return new ElementResponse(this, 1, resid);
    else if (argv[0] == "stiff" || argv[0] == "stiffness")// tangent stiffness matrix
      return new ElementResponse(this, 2, stiff);
    else if (argv[0] == "pressure") // contact pressure
      {
        //std::cerr<<"Contact2DsetResponse p="<<this->pressure <<std::endl;
        return new ElementResponse(this, 3, pressure);
      }
    else if(argv[0] == "gap")
      return new ElementResponse(this, 4, gap);
    else
      return 0;
  }


int XC::ZeroLengthContact2D::getResponse(int responseID, Information &eleInfo)
  {
    if(responseID == 1)
      return eleInfo.setVector(this->getResistingForce());
    else if(responseID == 2)
      return eleInfo.setMatrix(this->getTangentStiff());
    else if(responseID == 3)
      {
        //std::cerr<<"Contact2D getResponse p="<<this->pressure<<std::endl;
        return eleInfo.setDouble(this->pressure);
      }
    else if(responseID == 4)
      return eleInfo.setDouble(this->gap);
    else
      return -1;
  }


//! @brief determine the slave/master pair in contact, and setup Vectors (N,T1,T2)
int XC::ZeroLengthContact2D::contactDetect(void) const
  {
    //std::cerr<< this->getTag()<< " XC::ZeroLengthContact2D::contactDetect" <<std::endl;

    //+--------------+-----------------+----------------+----------------+---------------+
    // NOTES: some methods to get displacements from nodes
    //+--------------+-----------------+----------------+----------------+---------------+
    // getDisp() :         get commit(k-1) disp, will be commit(k) after commit
    // getTrialDisp():     get Trial(k) disp
    // getIncrDisp():      get Trial(k)-Commit(k-1), will be 0 after commit
    // getIncrDeltaDisp(): get Trial(k)-Trial(k-1),  will be 0 after commit
    //+--------------+-----------------+----------------+----------------+--------------

    ////////////////////////////// for transient gap ///////////////////////////////////
    // DEFINE:
    // gap = (U_master-U_slave) \dot (ContactNormal),
    // defines overlapped normal distance, always keep positive (+) when contacted
    ///*
    const Vector &U_slave = theNodes[0]->getCrds() + theNodes[0]->getTrialDisp(); // get current trial position
    const Vector &U_master= theNodes[1]->getCrds() + theNodes[1]->getTrialDisp();
    gap=0;
    for(int i=0; i<2; i++)
      { gap+= (U_master(i)-U_slave(i))* ContactNormal(i); }

    //*////////////////////////////// for transient gap ///////////////////////////////
    // we have another way to define the gap, can replace previous code block if want
    /*/////////////// for dynamic gap //////////////////////
    const Vector   // get current trial incremental position
        &U_slave = theNodes[0]->getCrds() + theNodes[0]->getIncrDisp();
    const Vector
        &U_master= theNodes[1]->getCrds() + theNodes[1]->getIncrDisp();
    gap=0;
    int i;
    for(i=0; i<2; i++)
      { gap += (U_master(i)-U_slave(i))* ContactNormal(i); }
    gap+=gap_n;
    ///////////////// for dynamic gap //////////////////////*/


    if(gap < 0)   // Not in contact
      return 0;
    else
      {
        N(0)=  ContactNormal(0);
        N(1)=  ContactNormal(1);
        N(2)= -N(0) ;
        N(3)= -N(1);

        T(0)= N(1);
        T(1)= -N(0);
        T(2)= -T(0);
        T(3)= -T(1);
        return 1;
      }
  }

//! @brief form residual and tangent
void  XC::ZeroLengthContact2D::formResidAndTangent( int tang_flag ) const
  {
    //std::cerr<<this->getTag()<< " XC::ZeroLengthContact2D:: formResidAndTangent()" <<std::endl;

    // trial displacement vectors
    Vector DispTrialS(2); // trial disp for slave node
    Vector DispTrialM(2); // trial disp for master node
    // trial frictional force vectors (in local coordinate)
    double t_trial;
    double TtrNorm;

    // Coulomb friction law surface
    double Phi;

    int i, j;

    //zero stiffness and residual
    stiff.Zero( ) ;
    resid.Zero( ) ;

    pressure= 0;
    t_trial=0;

    //int IsContact;
    // detect contact and set flag
    ContactFlag= contactDetect();

    //std::cerr<<this->getTag()<< " ZeroLengthContact2D::ContactFlag=" << ContactFlag<<std::endl;
    if(ContactFlag == 1) // contacted
        //if (gap >= 0) // contacted
    //if ((lambda + Kn*gap) >= 0)  // changed for augmented lagrange
      {
        // contact presure;
        pressure = Kn*gap ; // pressure is positive if in contact
        // pressure = Kn*gap + lambda; // changed for augmented lagrange

        DispTrialS=theNodes[0]->getTrialDisp();
        DispTrialM= theNodes[1]->getTrialDisp();

        //std::cerr<<"DispTrialS " << DispTrialS;
        //std::cerr<<"DispTrialM " << DispTrialM;

        //nodal displacements
        double ul[4];

        ul[0]=DispTrialS(0);
        ul[1]=DispTrialS(1);
        ul[2]=DispTrialM(0);
        ul[3]=DispTrialM(1);

        t_trial = 0;
        xi=0;

        // relative slide displacement
        // xi = T_tran * u    eq. (3.5)
        for(i=0; i<4; i++)
          { xi  += T (i)*ul[i]; }

        //for(i=0; i<2; i++){ t_trial(i)=Kt * (xi(i)-stickPt(i));}  //3D
        t_trial=Kt*(xi-stickPt); // trial shear force
        /// t_trial=Kt*(xi); // no update of stickPt, updated Jan 26, 2004
        // Coulomb friction law, trial state
        //TtrNorm=t_trial.Norm();
        TtrNorm=sqrt(t_trial*t_trial);
        Phi = TtrNorm - fs * pressure;

        if(Phi<=0)
          { // stick case
            //std::cerr<< "stick ...." << std::endl;
            if(tang_flag == 1)
              {
                // stiff
                for(i=0; i<4; i++)
                  {
                    for(j=0; j<4; j++)
                      {
                                                //stiff(i,j) = Kn*(N(i)*N(j)) + Kt*(T(i)*T1(j)+T2(i)*T2(j));// 3D
                        stiff(i,j) =   Kn*(N(i)*N(j)) + Kt*(T(i)*T(j)); //2D
                                        }
                                }
                        } //endif tang_flag
                        // force
                        for(i=0; i<4; i++)
                                 resid(i)= (-1*pressure)*N(i) + t_trial*T(i); //2D
                        //         resid(i)= (-1*pressure)*N(i) + t_trial(0)*T1(i) + t_trial(1)*T2(i) ;%3D

                } // end if stick
                else {           // slide case, non-symmetric stiff
            ContactFlag=2; // set the contactFlag for sliding

                         //std::cerr<< "sliding ...." << std::endl;


            if( tang_flag == 1 ) {
                                // stiff
                                for(i=0; i<4; i++) {
                                        for(j=0; j<4; j++) {
                                        // 3D
                                        //        define: Pt1=t_trial(0)/TtrNorm;
                                    //  define: Pt2=t_trial(1)/TtrNorm;
                                        //        define: C1=fs*Kn;
                    //  C2 term will be zero in two dimensional formulation
                                        // stiff(i,j) = Kn*(N(i)*N(j)) - C1*(Pt1*T1(i)*N(j)+Pt2*T2(i)*N(j))
                                        //            + C2*( (1-Pt1*Pt1)*T1(i)*T1(j) -    Pt1*Pt2 *T1(i)*T2(j)
                                        //      - Pt1*Pt2 *T2(i)*T1(j) + (1-Pt1*Pt2)*T2(i)*T2(j)  ); //3D

                                        // 2D                 ???? - or + ????
                                        stiff(i,j) = Kn*(N(i)*N(j)) - fs*Kn* (t_trial/TtrNorm)*T(i)*N(j); //2D
                                        } //endfor i
                                } //endfor j
                        } // endif tang_flag

                        // force
                        double shear=fs*pressure* (t_trial/TtrNorm);

                        for(i=0; i<4; i++) {
                                resid(i) = (-1*pressure)*N(i) + shear *T (i) ; //2D
                        //        resid(i) = (-1*pressure)*N(i) + t1*T1(i)+t2*T2(i) ; //3D
                        }
                } //endif slide

        }  // endif ContactFlag==1



        //std::cerr<<"gap=" << gap <<std::endl;
    //std::cerr<<"pressure= "<<pressure <<std::endl;
    //std::cerr<<"lambda=   "<<lambda <<std::endl;
        //std::cerr<<"t_trial= "<<t_trial <<std::endl;
    //std::cerr<<"stickPt= "<<stickPt <<std::endl;
        //std::cerr<<"residue= "<<resid <<std::endl;

        // for NOT contact, do nothing, stiff and resid are zeroes




    /* my notes:
           the direction of residual force is always confusing ...
              R=KU-Fext
           is defined as the resisting force that could provided by the element


       Let p,shear be absolute value of pressure and shear force, always positive

       thus,

                                Rx(1)=p*(-n)
                                 ||
                                                            \||/
                                                      ___\/____
                                                     /         \       /\
             Rx(1)     ---\ /    (1)    \     /||\n    Note: (t,n) follows RightHand rule
                 =shear*t  ---/ \   slave   /      ||
                                                         \_________/       ||_____\t
                                                -----------------------*------/
                        |                      |
                        |                      |
                                                |    (2) Master        |/---- Rx(2) = shear*(-t)
                                                |                      |\----
                                                ------------------------
                                  /\
                                                                 /||\
                                  ||
                                                                 Rx(2)=pn

     Denote :  N={n; -n}; T={t; -t}; arrange resid={R(1); R(2)}

     Finally,  resid(i) = (- p)*N(i) + shear *T (i) ;


  */

}


