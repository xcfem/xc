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
                                                                        
// $Source: /usr/local/cvs/OpenSees/SRC/element/zeroLength/ZeroLengthContact3D.cpp,v $
// $Revision: 1.3 $
// $Date: 2008/06/13 21:01:04 $

                                                                        
// Written: Gang Wang  (wang@ce.berkeley.edu)
//          Prof. Nicholas Sitar (nsitar@ce.berkeley.edu)
//
// Created: 27/08/2003
//
// Description: This file contains the implementation for the ZeroLengthContact3D class.


#include "ZeroLengthContact3D.h"
#include "domain/mesh/element/utils/Information.h"
#include "utility/recorder/response/ElementResponse.h"
#include "domain/mesh/node/Node.h"

 
// static data for 3D
XC::Matrix  XC::ZeroLengthContact3D::stiff(3*XC::ZeroLengthContact3D::numNodos(),3*XC::ZeroLengthContact3D::numNodos()) ;
XC::Vector  XC::ZeroLengthContact3D::resid(3*XC::ZeroLengthContact3D::numNodos()) ;
XC::Matrix  XC::ZeroLengthContact3D::zeroMatrix(3*XC::ZeroLengthContact3D::numNodos(),3*XC::ZeroLengthContact3D::numNodos()) ;

//*********************************************************************
//  Full Constructor:

XC::ZeroLengthContact3D::ZeroLengthContact3D(int tag,int Nd1, int Nd2, int direction, double Knormal, double Ktangent, double frictionRatio, double c, double origX, double origY )
  :ZeroLengthContact(tag,ELE_TAG_ZeroLengthContact3D,3,Nd1,Nd2,Knormal,Ktangent,frictionRatio), directionID(direction), 
  stickPt(2), xi(2), origin(2), T1(3*numNodos()), T2(3*numNodos())
  {
  
    if( direction < 0 || direction > 3)
      {
        std::cerr << "WARNING ZeroLengthContact3D:incorrect direction, set to 0\n";
        directionID = 0;
      }
  
    // assign cohesion
    cohesion = c; 
  
    //set origin coord for circular contact, (0,0) by default
    origin(0) = origX;
    origin(1) = origY;
  
    // set stick point cords in LOCAL basis
    stickPt(0)= 0;
    stickPt(1)= 0;
  
  }

//! @brief Default constructor.
XC::ZeroLengthContact3D::ZeroLengthContact3D(int tag)
  :ZeroLengthContact(ELE_TAG_ZeroLengthContact3D,3),     
  stickPt(2),  xi(2), origin(2) ,T1(3*numNodos()), T2(3*numNodos())
  {}

//! @brief Virtual constructor.
XC::Element* XC::ZeroLengthContact3D::getCopy(void) const
  { return new ZeroLengthContact3D(*this); }

//! method: setDomain()
//!    to set a link to the enclosing Domain and to set the node pointers.
//!    also determines the number of dof associated
//!    with the ZeroLengthContact3D element
void XC::ZeroLengthContact3D::setDomain(Domain *theDomain)
  {

    // call the base class method
    ZeroLengthContact::setDomain(theDomain);


    // now determine the number of dof and the dimension    
    const int dofNd1 = theNodes[0]->getNumberDOF();
    const int dofNd2 = theNodes[1]->getNumberDOF();	

    // if differing dof at the ends - print a warning message
    if(dofNd1 != dofNd2 )
      {
        std::cerr << "WARNING XC::ZeroLengthContact3D::setDomain(): nodes " 
                  << theNodes[0]->getTag()<< " and " << theNodes[1]->getTag()
                  << "have differing dof at ends for ZeroLengthContact3D " << this->getTag() << std::endl;
        return;
      }

    // Check that length is zero within tolerance
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();	
    Vector diff = end1Crd - end2Crd;
    double L= diff.Norm();
    double v1= end1Crd.Norm();
    double v2= end2Crd.Norm();
    double vm= (v1<v2) ? v2 : v1;
    const double lentol_vm= LenTol*vm;

    if(L > lentol_vm)
      std::cerr << "WARNING XC::ZeroLengthContact3D::setDomain(): Element " << this->getTag() << " has L= " << L << 
	", which is greater than the tolerance\n";
        
    
    if(dofNd1 == 3 && dofNd2 == 3)
      { numDOF = 6; }
    else
      {
        std::cerr << "WARNING XC::ZeroLengthContact3D::setDomain cannot handle " 
                  << dofNd1 << "dofs at nodes in " << dofNd1 << " d problem\n"; 
        return;
      }
  }   	 

int XC::ZeroLengthContact3D::commitState(void)
  {
    // need to update stick point here 
    if(ContactFlag == 2 )   // update stick point only for slide case
      stickPt=xi; 
    // update gap for "dynamic gap" method
      gap_n = gap; 
    return 0;
  }

int XC::ZeroLengthContact3D::revertToLastCommit(void)
  {
    ///////////////////////////////////////////
    // need to revert the stickPoint??
    xi=stickPt;
    return 0;
  }


int XC::ZeroLengthContact3D::revertToStart(void)
  {
    // need to rezero stickPoint??
    stickPt.Zero();  
    return 0;
  }


// calculate stress-strain relation -- M. Frank
/*
int XC::ZeroLengthContact3D::update(void)
  { return 0; }
*/

const XC::Matrix &XC::ZeroLengthContact3D::getTangentStiff(void) const
  {
    int tang_flag = 1 ; //get the tangent 
    //do tangent and residual here
    formResidAndTangent(tang_flag);
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }


const XC::Matrix &XC::ZeroLengthContact3D::getInitialStiff(void) const
  {
    int tang_flag = 1 ; //get the tangent 
    //do tangent and residual here
    formResidAndTangent(tang_flag) ;  
    if(isDead())
      stiff*=dead_srf;
    return stiff ;
  }
    

const XC::Matrix &XC::ZeroLengthContact3D::getDamp(void) const
  {
    // no mass 
    zeroMatrix.Zero(); 
    return zeroMatrix;
  }


const XC::Matrix &XC::ZeroLengthContact3D::getMass(void) const
  {
    // no mass 
    zeroMatrix.Zero(); 
    return zeroMatrix;
  }

const XC::Vector &XC::ZeroLengthContact3D::getResistingForce(void) const
  {
    int tang_flag= 0 ; //don't get the tangent
    formResidAndTangent(tang_flag);
    if(isDead())
      resid*=dead_srf;
    return resid;   
  }


const XC::Vector &XC::ZeroLengthContact3D::getResistingForceIncInertia(void) const
  {	
    // there is no Inertia 
    int tang_flag = 0 ; //don't get the tangent
    formResidAndTangent(tang_flag);
    if(isDead())
      resid*=dead_srf;
    return resid;
  }


int XC::ZeroLengthContact3D::sendSelf(CommParameters &cp)
  {
    // doing nothing here
    return 0;
  }

int XC::ZeroLengthContact3D::recvSelf(const CommParameters &cp)
  {
    // doing nothing here
    return 0;
  }


void XC::ZeroLengthContact3D::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      { // print everything
	s << "Element: " << this->getTag(); 
	s << " type: ZeroLengthContact3D  Nodes: " << theNodes;
	s << std::endl;
      }
    else if(flag == 1)
      { s << this->getTag() << std::endl; } 
  }

XC::Response *XC::ZeroLengthContact3D::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
     if(argv[0] == "force" || argv[0] == "forces")
       return new ElementResponse(this, 1, resid);
     else if(argv[0] == "stiff" || argv[0] == "stiffness")  // tangent stiffness matrix
       return new ElementResponse(this, 2, stiff);
     else 
       return 0;
  }


int XC::ZeroLengthContact3D::getResponse(int responseID, Information &eleInfo)
  {
    if(responseID == 1)
      return eleInfo.setVector(this->getResistingForce());
    else if(responseID == 2)
      return eleInfo.setMatrix(this->getTangentStiff());
    else
      return -1;
  }


// Private methods
// determine the slave/master pair in contact, and setup Vectors (N,T1,T2)
int XC::ZeroLengthContact3D::contactDetect(void) const
 {
   int transientgap; 
   transientgap = 1;   // 1: transient gap; 0: dynamic gap

   Vector  slaveNd;
   Vector  masterNd;

      //+--------------+-----------------+----------------+----------------+---------------+
      // NOTES: some methods to get displacements from nodes
      //+--------------+-----------------+----------------+----------------+---------------+
      // getDisp() :         get commit(k-1) disp, will be commit(k) after commit
      // getTrialDisp():     get Trial(k) disp
      // getIncrDisp():      get Trial(k)-Commit(k-1), will be 0 after commit
      // getIncrDeltaDisp(): get Trial(k)-Trial(k-1),  will be 0 after commit
      //+--------------+-----------------+----------------+----------------+---------------+

          if(transientgap) 
          {  ///////////// for transient gap //////////////////////////

                   slaveNd = theNodes[0]->getCrds() + theNodes[0]->getTrialDisp();
           masterNd= theNodes[1]->getCrds() + theNodes[1]->getTrialDisp();
          }  else {
         ///////////// for dynamic gap ////////////////////////////
              slaveNd = theNodes[0]->getCrds() + theNodes[0]->getIncrDisp();
          masterNd= theNodes[1]->getCrds() + theNodes[1]->getIncrDisp();
          }
      
      double Xs=slaveNd(0)  - origin(0);
      double Ys=slaveNd(1)  - origin(1);
          double Zs=slaveNd(2);
      double Rs=sqrt(Xs*Xs +Ys*Ys); 

      double Xm=masterNd(0) - origin(0);
          double Ym=masterNd(1) - origin(1);
      double Zm=masterNd(2);

          double Rm=sqrt(Xm*Xm +Ym*Ym);
                        
          switch (directionID) {

         case 0:  // circular contact plane
          
                                if(transientgap) {
                                        gap = Rs-Rm;
                                } else {
                   gap= gap_n + Rs - Rm; // dynamic gap
                                }

                                if(gap< 0) 
                                {  // Not in contact
                                        return 0;
                                } else         { // contact occur, setup contact vectors
                        
                                        N(0)   =  -Xm/Rm ;
                                        N(1)   =  -Ym/Rm ;
                                        N(2)   =   0 ;
                                        N(3)   =   Xm/Rm ;
                                        N(4)   =   Ym/Rm ;
                                        N(5)   =   0 ;

                                        T1(0)  =   0;
                                        T1(1)  =   0;
                                        T1(2)  =   1;
                                        T1(3)  =   0;
                                        T1(4)  =   0;
                                        T1(5)  =  -1;

                                        T2(0)  =  -Ym/Rm ;
                                        T2(1)  =   Xm/Rm ;
                                        T2(2)  =   0 ;
                                        T2(3)  =   Ym/Rm ;
                                        T2(4)  =  -Xm/Rm ;
                                        T2(5)  =   0 ;

                                        return 1; 
                                }
                        

                 case 1:   // normal of master plane pointing to +X direction
                                if(transientgap) {
                                        gap= Xm -Xs;             // transient gap
                                } else {
                    gap= gap_n + Xm - Xs;    // dynamic gap
                                }

                                if(gap< 0)   
                                {// Not in contact
                                        return 0;
                                } else { 
                                // contact occur, setup contact vectors
                        
                                        N(0)   =   1;
                                        N(1)   =   0 ;
                                        N(2)   =   0 ;
                                        N(3)   =  -1 ;
                                        N(4)   =   0 ;
                                        N(5)   =   0 ;

                                        T1(0)  =   0;
                                        T1(1)  =   1;
                                        T1(2)  =   0;
                                        T1(3)  =   0;
                                        T1(4)  =  -1;
                                        T1(5)  =   0;

                                        T2(0)  =   0 ;
                                        T2(1)  =   0 ;
                                        T2(2)  =   1 ;
                                        T2(3)  =   0 ;
                                        T2(4)  =   0 ;
                                        T2(5)  =  -1 ;

                                        return 1; 
                                }
                        

                case 2:  // normal of master plane pointing to +Y direction
                                if(transientgap) {
                                        gap= Ym - Ys;            // transient gap
                                } else {
                                        gap= gap_n + Ym - Ys;    // dynamic gap
                                }

                                if(gap<=0)  { // Not in contact
                                        return 0;
                                } else        { // contact occur, setup contact vectors
                                
                                        N(0)   =   0;
                                        N(1)   =   1 ;
                                        N(2)   =   0 ;
                                        N(3)   =   0 ;
                                        N(4)   =  -1 ;
                                        N(5)   =   0 ;

                                        T1(0)  =   0;
                                        T1(1)  =   0;
                                        T1(2)  =   1;
                                        T1(3)  =   0;
                                        T1(4)  =   0;
                                        T1(5)  =  -1;

                                        T2(0)  =   1 ;
                                        T2(1)  =   0 ;
                                        T2(2)  =   0 ;
                                        T2(3)  =  -1 ;
                                        T2(4)  =   0 ;
                                        T2(5)  =   0 ;

                                        return 1; 
                                }

                case 3:   // normal of master plane pointing to +Z direction
                        //          ___________ 
            //         |           |
                        //         |   slave   |  
                        //         |___________| 
                        //         |           |
                        //         |   Master  |
            //         |           |
                        //          -----------
                        // 
                                if(transientgap) {
                                        gap= Zm - Zs;         // transient gap
                                } else {
                                        gap= gap_n + Zm - Zs; // dynamic gap
                                }


                                if(gap < 0)   // Not in contact
                                        return 0;
                                else {        
                                        N(0)   =   0 ;
                                        N(1)   =   0 ;
                                        N(2)   =   1 ;
                                        N(3)   =   0 ;
                                        N(4)   =   0 ;
                                        N(5)   =  -1 ;

                                        T1(0)  =   1;
                                        T1(1)  =   0;
                                        T1(2)  =   0;
                                        T1(3)  =  -1;
                                        T1(4)  =   0;
                                        T1(5)  =   0;

                                        T2(0)  =   0 ;
                                        T2(1)  =   1 ;
                                        T2(2)  =   0 ;
                                        T2(3)  =   0 ;
                                        T2(4)  =  -1 ;
                                        T2(5)  =   0 ;

                                 return 1;
                                }

          default:
            std::cerr << "ERROR!!!! XC::ZeroLengthContact3D::ZeroLengthContact3D - the only available contact directions are 0,1,2,3\n";
            return -1;
        }  // end switch directionID
 }


void  XC::ZeroLengthContact3D::formResidAndTangent( int tang_flag ) const
{

        // trial displacement vectors
         Vector DispTrialS(3); // trial disp for slave node
        Vector DispTrialM(3); // trial disp for master node
        // trial frictional force vectors (in local coordinate)
    Vector t_trial(2);
    double TtrNorm;

    // Coulomb friction law surface
        double Phi;     

    int i, j;

    //zero stiffness and residual 
    stiff.Zero( ) ;
    resid.Zero( ) ;
   
        // detect contact and set flag
    ContactFlag = contactDetect(); 

        //std::cerr<<this->getTag()<< " XC::ZeroLengthContact3D::ContactFlag=" << ContactFlag<<std::endl;


        if(ContactFlag == 1) // contacted
        {  
       // contact presure;
                pressure = Kn*gap;   // Kn : normal penalty
  
                DispTrialS=theNodes[0]->getTrialDisp();
        DispTrialM=theNodes[1]->getTrialDisp();

       //nodal displacements 
        double ul[6];

                ul[0]=DispTrialS(0);
                ul[1]=DispTrialS(1);
                ul[2]=DispTrialS(2);
                ul[3]=DispTrialM(0);
                ul[4]=DispTrialM(1);
                ul[5]=DispTrialM(2);

        t_trial.Zero();
        xi.Zero();        

                for(i=0; i<6; i++){
                        xi(0) += T1(i)*ul[i];
                        xi(1) += T2(i)*ul[i];
                }

                // Compute trial shear force
                for(i=0; i<2; i++)  t_trial(i)=Kt * (xi(i)-stickPt(i));  //Kt: tangential penalty
        TtrNorm=t_trial.Norm();

        // Coulomb friction law, trial state
                Phi = TtrNorm - (fs * pressure + cohesion);   // add cohesion
                
                if(Phi <= 0 ) { // stick case
                  //std::cerr<< "stick ...." << std::endl;

                         if( tang_flag == 1 ) {
                    // stiff
                                for(i=0; i<6; i++) {
                                        for(j=0; j<6; j++) {
                                                stiff(i,j) = Kn*(N(i)*N(j)) + Kt*(T1(i)*T1(j)+T2(i)*T2(j));        
                                        }
                                }
                        } //endif tang_flag
                        // force
                        for(i=0; i<6; i++) 
                                 resid(i)= (-1*pressure)*N(i) + t_trial(0)*T1(i) + t_trial(1)*T2(i) ;
                        //         resid(i)= (-1*pressure)*N(i) - t_trial(0)*T1(i) - t_trial(1)*T2(i) ;

                } // end if stick
                else {              // slide case, non-symmetric stiff
            ContactFlag=2;  // set the contactFlag for sliding

                //        std::cerr<< "sliding ...." << std::endl;

            if( tang_flag == 1 ) {
                                // stiff
                                double Pt1, Pt2;
                                Pt1=t_trial(0)/TtrNorm;
                                Pt2=t_trial(1)/TtrNorm;
                                double C1=fs*Kn;
                                double C2=Kt*(fs*pressure+cohesion)/TtrNorm;  // add cohesion, sept. 7, 2005

                                for(i=0; i<6; i++) {
                                        for(j=0; j<6; j++) {
                                                stiff(i,j) = Kn*(N(i)*N(j)) - C1*(Pt1*T1(i)*N(j)+Pt2*T2(i)*N(j))
                                                            + C2*( (1-Pt1*Pt1)*T1(i)*T1(j) -    Pt1*Pt2 *T1(i)*T2(j)
                                                    - Pt1*Pt2 *T2(i)*T1(j) + (1-Pt1*Pt2)*T2(i)*T2(j)  );
                                        } //endfor i
                                } //endfor j
                        } // endif tang_flag
                
                        // force
                        double t1, t2;
                        t1 = (fs*pressure+cohesion) * t_trial(0)/TtrNorm;  // add cohesion
            t2 = (fs*pressure+cohesion) * t_trial(1)/TtrNorm;  // add cohesion

                        //std::cerr<<"gap=" << gap <<std::endl;
                        //std::cerr<<"pressure= "<<pressure <<std::endl;

                        for(i=0; i<6; i++) {
                                resid(i) = (-1*pressure)*N(i)+t1*T1(i)+t2*T2(i) ;
                        //        resid(i) = (-1*pressure)*N(i)-t1*T1(i)-t2*T2(i) ;
                        }
                } //endif slide

        }  // endif ContactFlag

        // for NOT contact, do nothing, stiff and resid are zeroes
  }

