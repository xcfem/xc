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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:00:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/beam_loads/Beam3dPointLoad.cpp,v $
                                                                        
// Written: fmk 

// Purpose: This file contains the class implementation XC::Beam3dPointLoad.

#include <domain/load/beam_loads/Beam3dPointLoad.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "utility/matrix/ID.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include "domain/mesh/element/utils/fvectors/FVector.h"
#include "material/section/repres/CrossSectionProperties3d.h"
#include "utility/actor/actor/MovableVector.h"

XC::Vector XC::Beam3dPointLoad::data(4);

XC::Beam3dPointLoad::Beam3dPointLoad(int tag, double py, double pz, double dist,const XC::ID &theElementTags, double px)
  :BeamPointLoad(tag, LOAD_TAG_Beam3dPointLoad,py,dist,theElementTags,px), Pz(pz) {}

XC::Beam3dPointLoad::Beam3dPointLoad(int tag)
  :BeamPointLoad(tag,LOAD_TAG_Beam3dPointLoad),Pz(0.0) {}

XC::Beam3dPointLoad::Beam3dPointLoad(void)
  :BeamPointLoad(0,LOAD_TAG_Beam3dPointLoad),Pz(0.0) {}

//! @brief Return the dimension of the force vector.
size_t XC::Beam3dPointLoad::getForceVectorDimension(void) const
  { return 3; }

//! @brief Return the dimension of the moment vector.
size_t XC::Beam3dPointLoad::getMomentVectorDimension(void) const
  { return 3; }

//! @brief Returns force expressed in local coordinates.
XC::Vector XC::Beam3dPointLoad::getLocalForce(void) const
  {
    Vector retval(3);
    retval(0)= px();
    retval(1)= py();
    retval(2)= pz();
    return retval;
  }

//! @brief Returns the bending moment expressed in local coordinates.
XC::Vector XC::Beam3dPointLoad::getLocalMoment(void) const
  {
    Vector retval(3);
    retval(0)= 0.0;
    retval(1)= 0.0;
    retval(2)= 0.0;
    return retval;
  }

const XC::Vector &XC::Beam3dPointLoad::getData(int &type, const double &loadFactor) const
  {
    type = getClassTag();
    data(0) = py();
    data(1) = pz();
    data(2) = px();
    data(3) = x;
    return data;
  }

//! @brief Returns force vectors (one for each element) expressed in local coordinates.
const XC::Matrix &XC::Beam3dPointLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= px();
        retval(i,1)= py();
        retval(i,2)= pz();
      }
    return retval;
  }

//! @brief Returns moment vectors (one for each element) expressed in local coordinates.
const XC::Matrix &XC::Beam3dPointLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= 0.0;
        retval(i,1)= 0.0;
        retval(i,2)= 0.0;
      }
    return retval;
  }

//! @brief Applied section forces due to element point load
const XC::Matrix &XC::Beam3dPointLoad::getAppliedSectionForces(const double &L,const XC::Matrix &xi,const double &loadFactor) const
  {
    const size_t nSections= xi.noCols();
    static Matrix retval(5,nSections); //Executed once only.
    retval.resize(5,nSections);
    retval.Zero();
    const double Py= py()*loadFactor;
    const double Pz= pz()*loadFactor;
    const double N= px()*loadFactor;
    const double aOverL= X();

    if(aOverL > 0.0 && aOverL < 1.0)
      {
        const double a= aOverL*L;
        const double Vy2 = Py*aOverL; //Y reaction at front end.
        const double Vy1 = Py-Vy2; //Y reaction at back end.
        const double Vz2 = Pz*aOverL; //Z reaction at front end.
        const double Vz1 = Pz-Vz2; //Z reaction at back end.
    
    
        // Accumulate applied section forces due to element loads
        for(size_t i = 0; i < nSections; i++)
          {
            const double x= xi(i,0)*L;
            if(x<=a)
              {
                retval(0,i)= N;
                retval(1,i)= -x*Vy1; //Moment about z axis para x<=a.
                retval(2,i)= -Vy1; //y shear para x<=a.
                retval(3,i)= x*Vz1; //Moment about y axis para x<=a.
                retval(4,i)= -Vz1; //z shear para x<=a.
              }
            else
              {
                retval(1,i)= -(L-x)*Vy2; //Moment about z axis para x>=a.
                retval(2,i)= Vy2; //y shear para x>=a.
                retval(3,i)= (L-x)*Vz2; //Moment about y axis para x>=a.
                retval(4,i)= Vz2; //z shear para x>=a.
              }
          }
      }
    return retval;
  }

//! @brief Adds the load al consistent load vector (see page 108 libro Eugenio Oñate).
//! @param L Length of the element.
//! @param loadFactor Load factor.
//! @param p0 element load vector.
void XC::Beam3dPointLoad::addReactionsInBasicSystem(const std::vector<double> &,const double &loadFactor,FVector &p0) const
  {
    const double aOverL= X();

    if(aOverL < 0.0 || aOverL > 1.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; the value of x ("
                  << aOverL << ") out of range"
	          << "; it must be between 0 and 1. Load ignored."
		  << std::endl;
        return;
      }
    else
      {
        const double Py= py()*loadFactor;
        const double Pz= pz()*loadFactor;
        const double N= px()*loadFactor;

        const double Vy2 = Py*aOverL;
        const double Vy1 = Py-Vy2;

        const double Vz2 = Pz*aOverL;
        const double Vz1 = Pz-Vz2;

        // Accumulate reactions in basic system
        p0[0]-= N;
        p0[1]-= Vy1;
        p0[2]-= Vy2;
        p0[3]-= Vz1;
        p0[4]-= Vz2;
      }
  }

//! @brief Returns the consistent load vector (see page 108 Eugenio Oñate book).
//! @param L Length of the element.
//! @param loadFactor Load factor.
//! @param q0 Consistent load vector.
//! @param release Moment release: 0=none, 1=I, 2=J, 3=I,J
void XC::Beam3dPointLoad::addFixedEndForcesInBasicSystem(const std::vector<double> &Li,const double &loadFactor,FVector &q0, int release) const
  {
    if(release!=0)
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; release effect not implemented yet."
		  << std::endl;
      
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
        const double Py= py()*loadFactor;
        const double Pz= pz()*loadFactor;
        const double N= px()*loadFactor;

        // Fixed end forces in basic system
        q0[0]+= -N*aOverL;
        double M1= -a * b2 * Py * L2;
        double M2= a2 * b * Py * L2;
        q0[1]+= M1;
        q0[2]+= M2;
        M1= -a * b2 * Pz * L2;
        M2= a2 * b * Pz * L2;
        q0[3]+= -M1;
        q0[4]+= -M2;
      }
  }
void XC::Beam3dPointLoad::addElasticDeformations(const double &L,const CrossSectionProperties3d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0)
  {
    const double Le = L-lpI-lpJ;
    if(Le>0.0)
      {
        const double oneOverL= 1.0/L;
        const double Py= py()*loadFactor;
        const double Pz= pz()*loadFactor;
        const double N= px()*loadFactor;
        const double aOverL= X();
        const double a= aOverL*L;

        const double Vy2 = Py*aOverL;
        const double Vy1 = Py-Vy2;

        const double Vz2 = Pz*aOverL;
        const double Vz1 = Pz-Vz2;

        // Accumulate basic deformations of interior due to point load
        double M1, M2, M3;
        double b1, b2, b3;

        // Point load is on left hinge
        if(a < lpI)
          {
            M1 = (lpI-L)*Vy2;
            M2 = -lpJ*Vy2;

            double Le_6EI= Le/(6*ctes_scc.EIz());

            b1 = lpI*oneOverL;
            b2 = 1.0-lpJ*oneOverL;
            v0[2] += Le_6EI*(M1*(2*b1+b2)+M2*(b1+2*b2));

            b1 -= 1.0;
            b2 -= 1.0;
            v0[1] += Le_6EI*(M1*(2*b1+b2)+M2*(b1+2*b2));

            M1 = (L-lpI)*Vz2;
            M2 = lpJ*Vz2;

            Le_6EI = Le/(6*ctes_scc.EIy());

            b1= lpI*oneOverL;
            b2= 1.0-lpJ*oneOverL;
            v0[4] += Le_6EI*(M1*(2*b1+b2)+M2*(b1+2*b2));

            b1 -= 1.0;
            b2 -= 1.0;
            v0[3] += Le_6EI*(M1*(2*b1+b2)+M2*(b1+2*b2));

            // Nothing to do for axial
            //v0[0] += 0.0;
          }
        // Point load is on right hinge
        else if (a > L-lpJ)
          {
            M1 = -lpI*Vy1;
            M2 = (lpJ-L)*Vy1;

            double Le_6EI= Le/(6*ctes_scc.EIz());

            b1 = lpI*oneOverL;
            b2 = 1.0-lpJ*oneOverL;
            v0[2] += Le_6EI*(M1*(2*b1+b2)+M2*(b1+2*b2));

            b1 -= 1.0;
            b2 -= 1.0;
            v0[1] += Le_6EI*(M1*(2*b1+b2)+M2*(b1+2*b2));

            M1 = lpI*Vz1;
            M2 = (L-lpJ)*Vz1;

            Le_6EI = Le/(6*ctes_scc.EIy());

            b1 = lpI*oneOverL;
            b2 = 1.0-lpJ*oneOverL;
            v0[4] += Le_6EI*(M1*(2*b1+b2)+M2*(b1+2*b2));

            b1 -= 1.0;
            b2 -= 1.0;
            v0[3] += Le_6EI*(M1*(2*b1+b2)+M2*(b1+2*b2));
            v0[0] += N*Le/(ctes_scc.EA());      
          }
        // Point load is on elastic interior
        else
          {
            M1 = -lpI*Vy1;
            M2 = -lpJ*Vy2;
            M3 = -a*Vy1;

            double L1_6EI= (a-lpI)/(6*ctes_scc.EIz());
            double L2_6EI= (Le-a+lpI)/(6*ctes_scc.EIz());

            b1 = lpI*oneOverL;
            b2 = 1.0-lpJ*oneOverL;
            b3 = a*oneOverL;
            v0[2] += L1_6EI*(M1*(2*b1+b3)+M3*(b1+2*b3));
            v0[2] += L2_6EI*(M2*(2*b2+b3)+M3*(b2+2*b3));

            b1 -= 1.0;
            b2 -= 1.0;
            b3 -= 1.0;
            v0[1] += L1_6EI*(M1*(2*b1+b3)+M3*(b1+2*b3));
            v0[1] += L2_6EI*(M2*(2*b2+b3)+M3*(b2+2*b3));

            M1 = lpI*Vz1;
            M2 = lpJ*Vz2;
            M3 = a*Vz1;

            L1_6EI = (a-lpI)/(6*ctes_scc.EIy());
            L2_6EI = (Le-a+lpI)/(6*ctes_scc.EIy());

            b1 = lpI*oneOverL;
            b2 = 1.0-lpJ*oneOverL;
            b3 = a*oneOverL;
            v0[4] += L1_6EI*(M1*(2*b1+b3)+M3*(b1+2*b3));
            v0[4] += L2_6EI*(M2*(2*b2+b3)+M3*(b2+2*b3));

            b1 -= 1.0;
            b2 -= 1.0;
            b3 -= 1.0;
            v0[3] += L1_6EI*(M1*(2*b1+b3)+M3*(b1+2*b3));
            v0[3] += L2_6EI*(M2*(2*b2+b3)+M3*(b2+2*b3));

            v0[0] += N*(a-lpI)/(ctes_scc.EA());
          }
      }
  }

//! @brief Returns a vector to store the dbTags
//! de los miembros of the clase.
XC::DbTagData &XC::Beam3dPointLoad::getDbTagData(void) const
  {
    static DbTagData retval(8);
    return retval;
  }

//! @brief Sends object through the communicator argument.
int XC::Beam3dPointLoad::sendSelf(Communicator &comm)
  {
    inicComm(8);
    int result= sendData(comm);
    result+= comm.sendDouble(Pz,getDbTagData(),CommMetaData(7));

    const int dbTag= getDbTag();
    result+= comm.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << "Beam3dPointLoad::sendSelf() - failed to send extra data\n";
    return result;
  }

//! @brief Receives object through the communicator argument.
int XC::Beam3dPointLoad::recvSelf(const Communicator &comm)
  {
    inicComm(8);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Beam3dPointLoad::recvSelf() - data could not be received\n" ;
    else
      {
        res+= recvData(comm);
        res+= comm.receiveDouble(Pz,getDbTagData(),CommMetaData(7));
      }
    return res;
  }

//! @brief Print load information.
void XC::Beam3dPointLoad::Print(std::ostream &s, int flag) const
  {
    s << "Beam3dPointLoad - Reference load" << std::endl;
    s << "  Transverse (y): " << Trans << std::endl;
    s << "  Transverse (z): " << Pz << std::endl;
    s << "  Axial (x):      " << Axial << std::endl;
    s << "  Elements acted on: " << this->getElementTags();
  }
