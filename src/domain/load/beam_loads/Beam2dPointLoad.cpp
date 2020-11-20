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

// $Revision: 1.5 $
// $Date: 2003/02/14 23:00:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/beam_loads/Beam2dPointLoad.cpp,v $

// Written: fmk

// Purpose: This file contains the class implementation XC::Beam2dPointLoad.

#include <domain/load/beam_loads/Beam2dPointLoad.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "boost/any.hpp"
#include "utility/matrix/ID.h"

#include "domain/mesh/element/utils/fvectors/FVector.h"
#include "material/section/repres/CrossSectionProperties3d.h"
#include "utility/actor/actor/MovableID.h"

XC::Vector XC::Beam2dPointLoad::data(3);

XC::Beam2dPointLoad::Beam2dPointLoad(int tag, double Pt, double dist, const XC::ID &theElementTags, double Pa)
  :BeamPointLoad(tag, LOAD_TAG_Beam2dPointLoad,Pt,dist,theElementTags,Pa) {}

XC::Beam2dPointLoad::Beam2dPointLoad(int tag)
  :BeamPointLoad(tag, LOAD_TAG_Beam2dPointLoad) {}

XC::Beam2dPointLoad::Beam2dPointLoad(void)
  :BeamPointLoad(0,LOAD_TAG_Beam2dPointLoad) {}

//! @brief Return the dimension of the force vector.
size_t XC::Beam2dPointLoad::getForceVectorDimension(void) const
  { return 2; }

//! @brief Return the dimension of the moment vector.
size_t XC::Beam2dPointLoad::getMomentVectorDimension(void) const
  { return 1; }

const XC::Vector &XC::Beam2dPointLoad::getData(int &type, const double &loadFactor) const
  {
    type = getClassTag();
    data(0) = pTrans();
    data(1) = pAxial();
    data(2) = x;
    return data;
  }

//! @brief Returns force expressed in local coordinates.
XC::Vector XC::Beam2dPointLoad::getLocalForce(void) const
  {
    Vector retval(2);
    retval(0)= pAxial();
    retval(1)= pTrans();
    return retval;
  }

//! @brief Returns the bending moment expressed in local coordinates.
XC::Vector XC::Beam2dPointLoad::getLocalMoment(void) const
  {
    Vector retval(1);
    retval(0)= 0.0;
    retval(1)= 0.0;
    retval(2)= 0.0;
    return retval;
  }

//! @brief Returns force vectors (one for each element) expressed in local coordinates.
const XC::Matrix &XC::Beam2dPointLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,2);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= pAxial();
        retval(i,1)= pTrans();
      }
    return retval;
  }

//! @brief Returns moment vectors (one for each element) expressed in local coordinates.
const XC::Matrix &XC::Beam2dPointLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,1);
    for(size_t i=0; i<sz; i++)
      retval(i,0)= 0.0;
    return retval;
  }

//! @brief Forces over element sections obtained from the load acting over the element.
const XC::Matrix &XC::Beam2dPointLoad::getAppliedSectionForces(const double &L,const XC::Matrix &xi,const double &loadFactor) const
  {
    const size_t nSections= xi.noCols();
    static Matrix retval(3,nSections); //Executed only once.
    retval.resize(3,nSections);
    retval.Zero();
    const double aOverL= X();


    if(aOverL > 0.0 && aOverL < 1.0) //Load inside the element.
      {
        const double a = aOverL*L; //Load abscissa.
        const double P= pTrans()*loadFactor;
        const double N= pAxial()*loadFactor;
        const double V1 = P*(1.0-aOverL); //Back end reaction.
        const double V2 = P*aOverL; //Front end reaction.

        // Accumulate applied section forces due to element loads
        for(size_t i=0;i<nSections;i++)
          {
            const double x= xi(i,0)*L; //Section abscissa.
            if(x<=a) //Abscissa lesser or equal than the load's one.
              {
                retval(0,i)= N;
                retval(1,i)= -x*V1; //Moment for x<=a.
                retval(2,i)= -V1; //Shear for x<=a.
              }
            else //Abscissa greater than the load's one.
              {
                retval(1,i)= -(L-x)*V2; //Moment for x>=a.
                retval(2,i)= V2; //Shear for x>=a.
              }
          }
      }
    return retval;
  }

//! @brief Adds the load to the consistent load vector (see page 108 Eugenio Oñate's book).
//! @param Li: tributary lengths of each node.
//! @param loadFactor Load factor.
//! @param p0 element load vector.
void XC::Beam2dPointLoad::addReactionsInBasicSystem(const std::vector<double> &Li,const double &loadFactor,FVector &p0) const
  {
    const double aOverL= X();

    if(aOverL < 0.0 || aOverL > 1.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; the value of x ("
                  << aOverL
		  << ") es incorrecto, debe estar entre 0 y 1. Load ignored."
                  << std::endl;
        return;
      }
    else
      {
        const double P= pTrans()*loadFactor; //Factored transverse load.
        const double N= pAxial()*loadFactor; //Factored axial load.
        const double V2= P*aOverL; //Back end reaction.
        const double V1= P-V2; //Front end reaction.

        // Accumulate reactions in basic system
        p0[0]-= N; //Subtracts axial load.
        p0[1]-= V1; //Subtracts back end reaction.
        p0[2]-= V2; //Subtracts front end reaction.
      }
  }

//! @brief Returns the consistent load vector (see page 108 Eugenio Oñate book).
//! @param L Length of the element.
//! @param loadFactor Load factor.
//! @param q0 Consistent load vector.
//! @param release Moment release: 0=none, 1=I, 2=J, 3=I,J
void XC::Beam2dPointLoad::addFixedEndForcesInBasicSystem(const std::vector<double> &Li,const double &loadFactor,FVector &q0, int release) const
  {
    const double aOverL= X();

    if(aOverL < 0.0 || aOverL > 1.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; wrong value for x ("
                  << aOverL << "), it must be between 0 and 1. Load ignored."
		  << std::endl;
      }
    else
      {
	const double L= Li[0];
        const double a= aOverL*L;
        const double b= L-a;
        const double L2 = 1.0/(L*L);
        const double a2 = a*a;
        const double b2 = b*b;
        const double Py= pTrans()*loadFactor;
        const double N= pAxial()*loadFactor;

        // Fixed end forces in basic system
        q0[0]-= N*aOverL;
        const double M1= -a * b2 * Py * L2;
        const double M2= a2 * b * Py * L2;
	if(release == 0)
	  {
  	   q0[1]+= M1;
	   q0[2]+= M2;
          }
	else if(release == 1)
	  { q0[2]+= 0.5*Py*a*b*L2*(a+L); }
        else if(release == 2)
	  { q0[1]-= 0.5*Py*a*b*L2*(b+L); }
        else if(release == 3)
	  {
	   // Nothing to do
          }
	else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << " release value (" << release
		    << ") not valid. Must be between 0 and 3."
		    << std::endl;
      }
  }

void XC::Beam2dPointLoad::addElasticDeformations(const double &L,const CrossSectionProperties2d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0)
  {
    // Length of elastic interior
    const double Le= L-lpI-lpJ;
    if(Le==0.0)
      return;
    else
      {
        const double P= pTrans()*loadFactor;
        const double N= pAxial()*loadFactor;
        const double aOverL= X();
        const double a= aOverL*L;
        const double oneOverL= 1.0/L;

        double V1= P*(1.0-aOverL);
        double V2= P*aOverL;

        // Accumulate basic deformations of interior due to point load
        double M1, M2, M3;
        double b1, b2, b3;

        const double SixEI= 6*ctes_scc.EI();
        const double Le_6EI= Le/SixEI;
        // Point load is on left hinge
        if(a < lpI)
          {
            M1= (lpI-L)*V2;
            M2= -lpJ*V2;

            b1= lpI*oneOverL;
            b2= 1.0-lpJ*oneOverL;
            v0[2] += Le_6EI*(M1*(2*b1+b2)+M2*(b1+2*b2));

            b1-= 1.0;
            b2-= 1.0;
            v0[1] += Le_6EI*(M1*(2*b1+b2)+M2*(b1+2*b2));
            // Nothing to do for axial
            //v0[0] += 0.0;
          }
        // Point load is on right hinge
        else if(a > L-lpJ)
          {
            M1= -lpI*V1;
            M2= (lpJ-L)*V1;


            b1= lpI*oneOverL;
            b2= 1.0-lpJ*oneOverL;
            v0[2] += Le_6EI*(M1*(2*b1+b2)+M2*(b1+2*b2));

            b1-= 1.0;
            b2-= 1.0;
            v0[1]+= Le_6EI*(M1*(2*b1+b2)+M2*(b1+2*b2));
            v0[0]+= N*Le/(ctes_scc.EA());
          }
        // Point load is on elastic interior
        else
          {
            M1= -lpI*V1;
            M2= -lpJ*V2;
            M3= -a*V1;

            const double L1_6EI= (a-lpI)/SixEI;
            const double L2_6EI= (Le-a+lpI)/SixEI;

            b1= lpI*oneOverL;
            b2= 1.0-lpJ*oneOverL;
            b3= a*oneOverL;
            v0[2] += L1_6EI*(M1*(2*b1+b3)+M3*(b1+2*b3));
            v0[2] += L2_6EI*(M2*(2*b2+b3)+M3*(b2+2*b3));

            b1-= 1.0;
            b2-= 1.0;
            b3-= 1.0;
            v0[1]+= L1_6EI*(M1*(2*b1+b3)+M3*(b1+2*b3));
            v0[1]+= L2_6EI*(M2*(2*b2+b3)+M3*(b2+2*b3));
            v0[0]+= N*(a-lpI)/(ctes_scc.EA());
          }
      }
  }

//! @brief Sends object through the communicator argument.
int XC::Beam2dPointLoad::sendSelf(Communicator &comm)
  {
    inicComm(7);
    int result= sendData(comm);

    const int dbTag= getDbTag();
    result+= comm.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << "Beam2dPointLoad::sendSelf() - failed to send extra data\n";
    return result;
  }

//! @brief Receives object through the communicator argument.
int XC::Beam2dPointLoad::recvSelf(const Communicator &comm)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Beam2dPointLoad::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(comm);
    return res;
  }

void XC::Beam2dPointLoad::Print(std::ostream &s, int flag) const
  {
    s << "Beam2dPointLoad - reference load : (" << pTrans()
      << ", " << pAxial() << ") acting at : " << X() << " relative to length\n";
    s << "  elements acted on: " << this->getElementTags();
  }
