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

//! @brief Return the dimension del vector fuerza.
size_t XC::Beam2dPointLoad::getDimVectorFuerza(void) const
  { return 2; }

//! @brief Return the dimension del vector momento.
size_t XC::Beam2dPointLoad::getDimVectorMomento(void) const
  { return 1; }

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

//! @brief Fuerzas aplicadas sobre las secciones de un elemento debidas a esta carga actuando sobre el mismo.
const XC::Matrix &XC::Beam2dPointLoad::getAppliedSectionForces(const double &L,const XC::Matrix &xi,const double &loadFactor)
  {
    const size_t nSections= xi.noCols();
    static Matrix retval(3,nSections); //Sólo se ejecuta una vez.
    retval.resize(3,nSections);
    retval.Zero();
    const double aOverL= X();


    if(aOverL > 0.0 && aOverL < 1.0) //Carga dentro del elemento.
      {
        const double a = aOverL*L; //Abcisa de la carga.
        const double P= pTrans()*loadFactor;
        const double N= pAxial()*loadFactor;
        const double V1 = P*(1.0-aOverL); //Reacción en extremo dorsal.
        const double V2 = P*aOverL; //Reacción en extremo frontal.

        // Accumulate applied section forces due to element loads
        for(size_t i=0;i<nSections;i++)
          {
            const double x= xi(i,0)*L; //Abcisa de la sección.
            if(x<=a) //Abcisa menor que la de la carga.
              {
                retval(0,i)= N;
                retval(1,i)= -x*V1; //Momento para x<=a.
                retval(2,i)= -V1; //Cortante para x<=a.
              }
            else //Abcisa mayor que la de la carga.
              {
                retval(1,i)= -(L-x)*V2; //Momento para x>=a.
                retval(2,i)= V2; //Cortante para x>=a.
              }
          }
      }
    return retval;
  }

//! @brief Añade la carga al vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param L Longitud del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param p0 Vector de cargas del elemento.
void XC::Beam2dPointLoad::addReactionsInBasicSystem(const double &L,const double &loadFactor,FVector &p0)
  {
    const double aOverL= X();

    if(aOverL < 0.0 || aOverL > 1.0)
      {
        std::cerr << "XC::Beam2dPointLoad::addReactionsInBasicSystem; el valor de x ("
                  << aOverL << ") es incorrecto, debe estar entre 0 y 1. Se ignora la carga."
                  << std::endl;
        return;
      }
    else
      {
        const double P= pTrans()*loadFactor; //Carga transversal ponderada.
        const double N= pAxial()*loadFactor; //Carga axial ponderada.
        const double V2= P*aOverL; //Reacción en extremo dorsal.
        const double V1= P-V2; //Reacción en extremo frontal.

        // Accumulate reactions in basic system
        p0[0]-= N; //Resta el axil.
        p0[1]-= V1; //Resta la reacción en el extremo dorsal.
        p0[2]-= V2; //Resta la reacción en el extremo frontal.
      }
  }

//! @brief ??
//! @param L Longitud del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param q0 ??
void XC::Beam2dPointLoad::addFixedEndForcesInBasicSystem(const double &L,const double &loadFactor,FVector &q0)
  {
    const double aOverL= X();

    if(aOverL < 0.0 || aOverL > 1.0)
      {
        std::cerr << "XC::Beam2dPointLoad::fixedEndForcesInBasicSystem; el valor de x ("
                  << aOverL << ") es incorrecto, debe estar entre 0 y 1. Se ignora la carga." << std::endl;
      }
    else
      {
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
        q0[1]+= M1;
        q0[2]+= M2;
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

//! @brief Sends object through the channel being passed as parameter.
int XC::Beam2dPointLoad::sendSelf(CommParameters &cp)
  {
    inicComm(7);
    int result= sendData(cp);

    const int dbTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << "Beam2dPointLoad::sendSelf() - failed to send extra data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Beam2dPointLoad::recvSelf(const CommParameters &cp)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Beam2dPointLoad::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }

void XC::Beam2dPointLoad::Print(std::ostream &s, int flag)
  {
    s << "Beam2dPointLoad - reference load : (" << pTrans()
      << ", " << pAxial() << ") acting at : " << X() << " relative to length\n";
    s << "  elements acted on: " << this->getElementTags();
  }
