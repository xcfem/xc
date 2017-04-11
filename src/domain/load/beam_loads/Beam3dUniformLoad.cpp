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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/beam_loads/Beam3dUniformLoad.cpp,v $
                                                                        

// Written: fmk 
//
// Purpose: This file contains the class implementation of XC::Beam3dUniformLoad.

#include <domain/load/beam_loads/Beam3dUniformLoad.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "utility/matrix/ID.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "boost/lexical_cast.hpp"

#include "domain/mesh/element/utils/fvectors/FVector.h"
#include "material/section/repres/CrossSectionProperties3d.h"
#include "domain/mesh/element/Element1D.h"

XC::Vector XC::Beam3dUniformLoad::data(4);

XC::Beam3dUniformLoad::Beam3dUniformLoad(int tag, double wY, double wZ, double wX, double tX,const XC::ID &theElementTags)
  :BeamUniformLoad(tag, LOAD_TAG_Beam3dUniformLoad,wY,wX, theElementTags), wz(wZ), tx(tX) {}

XC::Beam3dUniformLoad::Beam3dUniformLoad(int tag, const Vector &Fxyz, double tX,const ID &theElementTags)
  :BeamUniformLoad(tag, LOAD_TAG_Beam3dUniformLoad,Fxyz[1],Fxyz[0], theElementTags), wz(Fxyz[2]), tx(tX) {}

XC::Beam3dUniformLoad::Beam3dUniformLoad(int tag)
  :BeamUniformLoad(tag, LOAD_TAG_Beam3dUniformLoad), wz(0.0), tx(0.0) {}

XC::Beam3dUniformLoad::Beam3dUniformLoad(void)
  :BeamUniformLoad(0,LOAD_TAG_Beam3dUniformLoad), wz(0.0), tx(0.0) {}

int XC::Beam3dUniformLoad::getType(void)
  { return LOAD_TAG_Beam3dUniformLoad; }

//! @brief Applied section forces due to element uniform load
const XC::Matrix &XC::Beam3dUniformLoad::getAppliedSectionForces(const double &L,const Matrix &xi_pt,const double &loadFactor) const
  {
    const size_t nSections=  xi_pt.noRows();
    static Matrix retval(5,nSections); //Sólo se ejecuta una vez.
    retval.resize(5,nSections);
    retval.Zero();
    const double wy = Wy()*loadFactor;  // Transverse
    const double wz = Wz()*loadFactor;  // Transverse
    const double wx = Wx()*loadFactor;  // Axial (+ve from node I to J)
    //const double tx= Tx()*loadFactor;  // Torsor

    // Accumulate applied section forces due to element loads
    for(size_t i = 0; i < nSections; i++)
      {
        const double x= xi_pt(i,0)*L;
        
        retval(0,i)= wx*(L-x);// Axial
        
        retval(1,i)= wy*0.5*x*(x-L); // Moment
        retval(2,i)= wy*(x-0.5*L); // Shear
        
        retval(3,i)= wz*0.5*x*(L-x);// Moment
        retval(4,i)= wz*(x-0.5*L);// Shear
      }
    return retval;
  }

//! @brief Adds the load al consistent load vector (ver página 108 libro Eugenio Oñate).
//! @param L Length of the element.
//! @param loadFactor Load factor.
//! @param p0 element load vector.
void XC::Beam3dUniformLoad::addReactionsInBasicSystem(const double &L,const double &loadFactor,FVector &p0) const
  {
    const double wy= Wy()*loadFactor;  // Transverse
    const double wz= Wz()*loadFactor;  // Transverse
    const double wx= Wx()*loadFactor;  // Axial (+ve from node I to J)
    //const double tx= Tx()*loadFactor;  // Torsor

    //Forces over the element.
    const double Vy = 0.5*wy*L; // Cortante en y en los extremos of the element.
    const double Vz = 0.5*wz*L; // Cortante en z.
    const double P = wx*L; //Axil.

    // Reactions in basic system
    p0[0]-= P;
    p0[1]-= Vy;
    p0[2]-= Vy;
    p0[3]-= Vz;
    p0[4]-= Vz;
  }

//! @brief ??
//! @param L Length of the element.
//! @param loadFactor Load factor.
//! @param q0 ??
void XC::Beam3dUniformLoad::addFixedEndForcesInBasicSystem(const double &L,const double &loadFactor,FVector &q0) const
  {
    const double wy = Wy()*loadFactor;  // Transverse
    const double wz = Wz()*loadFactor;  // Transverse
    const double wx = Wx()*loadFactor;  // Axial (+ve from node I to J)
    //const double tx= Tx()*loadFactor;  // Torsor

    //Forces over the element.
    const double Mz = wy*L*L/12.0; // Momento flector en z at the ends of the element: wy*L*L/12
    const double My = wz*L*L/12.0; // Momento flector en y at the ends of the element: wz*L*L/12
    const double P = wx*L; //Axil.

    // Fixed end forces in basic system
    q0[0]+= -0.5*P; //Axil en el extremo dorsal.
    q0[1]+= -Mz;  
    q0[2]+= Mz;
    q0[3]+= My;
    q0[4]+= -My;
  }

//! @brief Adds elastic strains to the v0 vector.
void XC::Beam3dUniformLoad::addElasticDeformations(const double &L,const CrossSectionProperties3d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0)
  {
    // Length of elastic interior
    const double Le = L-lpI-lpJ;
    if(Le>0.0)
      {
        const double oneOverL = 1.0/L;
        const double wy= Wy()*loadFactor;  // Transverse
        const double wz= Wz()*loadFactor;  // Transverse
        const double wx= Wx()*loadFactor;  // Axial
        //const double tx= Tx()*loadFactor;  // Torsor

        // Accumulate basic deformations due to uniform load on interior
        // Midpoint rule for axial
        v0[0] += wx*0.5*(L-lpI+lpJ)/(ctes_scc.EA())*Le;

        // Two point Gauss for bending ... will not be exact when
        // hinge lengths are not equal, but this is not a big deal!!!
        const double x1 = lpI + 0.5*Le*(1.0-1.0/sqrt(3.0));
        const double x2 = lpI + 0.5*Le*(1.0+1.0/sqrt(3.0));

        const double Mz1 = 0.5*wy*x1*(x1-L);
        const double Mz2 = 0.5*wy*x2*(x2-L);

        const double My1 = -0.5*wz*x1*(x1-L);
        const double My2 = -0.5*wz*x2*(x2-L);

        const double Le2EIz = Le/(2*ctes_scc.EIz());
        const double Le2EIy = Le/(2*ctes_scc.EIy());

        double b1, b2;
        b1 = x1*oneOverL;
        b2 = x2*oneOverL;
        v0[2] += Le2EIz*(b1*Mz1+b2*Mz2);
        v0[4] += Le2EIy*(b1*My1+b2*My2);

        b1 -= 1.0;
        b2 -= 1.0;
        v0[1] += Le2EIz*(b1*Mz1+b2*Mz2);
        v0[3] += Le2EIy*(b1*My1+b2*My2);
      }
  }



//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::Beam3dUniformLoad::getDbTagData(void) const
  {
    static DbTagData retval(7);
    return retval;
  }

//! @brief Send data through the channel being passed as parameter.
int XC::Beam3dUniformLoad::sendData(CommParameters &cp)
  {
    int res= BeamUniformLoad::sendData(cp);
    res+= cp.sendDoubles(wz,tx,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::Beam3dUniformLoad::recvData(const CommParameters &cp)
  {
    int res= BeamUniformLoad::recvData(cp);
    res+= cp.receiveDoubles(wz,tx,getDbTagData(),CommMetaData(6));
    return res;
  }

int XC::Beam3dUniformLoad::sendSelf(CommParameters &cp)
  {
    inicComm(7);
    int result= sendData(cp);
    const int dbTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << "Beam3dUniformLoad::sendSelf() - failed to send extra data\n";
    return result;
  }

int XC::Beam3dUniformLoad::recvSelf(const CommParameters &cp)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Beam3dUniformLoad::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }

void  XC::Beam3dUniformLoad::Print(std::ostream &s, int flag)
  {
    s << "Beam2dUniformLoad - Reference load" << std::endl;
    s << "  Transverse (y): " << Trans << std::endl;
    s << "  Transverse (z): " << wz << std::endl;
    s << "  Axial (x):      " << Axial << std::endl;
    s << "  Elements acted on: " << getElementTags();
  }

//! @brief Return the dimension del vector fuerza.
size_t XC::Beam3dUniformLoad::getDimVectorFuerza(void) const
  { return 3; }

//! @brief Return the dimension del vector momento.
size_t XC::Beam3dUniformLoad::getDimVectorMomento(void) const
  { return 3; }

//! @brief Returns force expressed in local coordinates.
XC::Vector XC::Beam3dUniformLoad::getLocalForce(void) const
  {
    Vector retval(3);
    retval(0)= Wx();
    retval(1)= Wy();
    retval(2)= Wz();
    return retval;
  }

//! @brief Returns moment expressed in local coordinates.
XC::Vector XC::Beam3dUniformLoad::getLocalMoment(void) const
  {
    Vector retval(3);
    retval(0)= Tx();
    retval(1)= 0.0;
    retval(2)= 0.0;
    return retval;
  }

//! @brief Returns distributed force vectors (one for each element) expressed in local coordinates.
const XC::Matrix &XC::Beam3dUniformLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= Wx();
        retval(i,1)= Wy();
        retval(i,2)= Wz();
      }
    return retval;
  }

//! @brief Returns distributed moment vectors (one for each element) expressed in local coordinates.
const XC::Matrix &XC::Beam3dUniformLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= Tx();
        retval(i,1)= 0.0;
        retval(i,2)= 0.0;
      }
    return retval;
  }
