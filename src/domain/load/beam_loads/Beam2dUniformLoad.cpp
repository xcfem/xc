//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/beam_loads/Beam2dUniformLoad.cpp,v $
                                                                        

// Written: fmk 
//
// Purpose: This file contains the class implementation of XC::Beam2dUniformLoad.

#include <domain/load/beam_loads/Beam2dUniformLoad.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "boost/any.hpp"
#include "utility/matrix/ID.h"

#include "domain/mesh/element/fvectors/FVector.h"
#include "material/section/repres/CrossSectionProperties3d.h"

XC::Vector XC::Beam2dUniformLoad::data(2);

XC::Beam2dUniformLoad::Beam2dUniformLoad(int tag, double wt, double wa,const XC::ID &theElementTags)
  :BeamUniformLoad(tag, LOAD_TAG_Beam2dUniformLoad,wt,wa,theElementTags) {}

XC::Beam2dUniformLoad::Beam2dUniformLoad(int tag)
  :BeamUniformLoad(tag, LOAD_TAG_Beam2dUniformLoad) {}

XC::Beam2dUniformLoad::Beam2dUniformLoad(void)
  :BeamUniformLoad(0,LOAD_TAG_Beam2dUniformLoad) {}

int XC::Beam2dUniformLoad::getType(void)
  { return LOAD_TAG_Beam2dUniformLoad; }

//! @brief Devuelve la dimension del vector fuerza.
size_t XC::Beam2dUniformLoad::getDimVectorFuerza(void) const
  { return 2; }

//! @brief Devuelve la dimension del vector momento.
size_t XC::Beam2dUniformLoad::getDimVectorMomento(void) const
  { return 1; }

//! @brief Devuelve las componentes de los vectores fuerza.
const XC::Matrix &XC::Beam2dUniformLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,2);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= WAxial();
        retval(i,1)= WTrans();
      }
    return retval;
  }

//! @brief Devuelve las componentes de los vectores momento.
const XC::Matrix &XC::Beam2dUniformLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,1);
    for(size_t i=0; i<sz; i++)
      retval(i,0)= 0.0;
    return retval;
  }

//! @brief Applied section forces due to element uniform load
const XC::Matrix &XC::Beam2dUniformLoad::getAppliedSectionForces(const double &L,const Matrix &xi_pt,const double &loadFactor)
  {
    const size_t nSections= xi_pt.noRows();
    static Matrix retval(3,1); //Compile time definition.
    retval.resize(3,nSections); //Resize.
    retval.Zero();
    const double wa= WAxial()*loadFactor;  // Axial
    const double wy= WTrans()*loadFactor;  // Transverse

    //Calcula las fuerzas aplicadas sobre las secciones
    //debidas a esta carga.
    for(size_t i = 0; i < nSections; i++)
      {
        const double x= xi_pt(i,0)*L;

        retval(0,i)= wa*(L-x); //Integral del axil entre x y L.
        retval(1,i)= wy*0.5*x*(x-L); //Momento en la sección x.
        retval(2,i)= wy*(x-0.5*L); //Cortante en la sección x.
      }
    return retval;
  }

//! @brief Devuelve el vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param L Longitud del borde del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param p0 Vector de cargas del elemento.
void XC::Beam2dUniformLoad::addReactionsInBasicSystem(const double &L,const double &loadFactor,FVector &p0)
  {
    const double wa= WAxial()*loadFactor;  // Axial
    const double wy= WTrans()*loadFactor;  // Transverse

    //Esfuerzos sobre elemento.
    const double V = 0.5*wy*L; // Cortante en los extremos del elemento.
    const double P= wa*L; //Axil.

    // Reactions in basic system
    p0[0]-= P; //Resta el axil. 
    p0[1]-= V; //Resta la reacción en el extremo dorsal.
    p0[2]-= V; //Resta la reacción en el extremo frontal.
  }

//! @brief Devuelve el vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param L Longitud del borde del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param q0 ??.
void XC::Beam2dUniformLoad::addFixedEndForcesInBasicSystem(const double &L,const double &loadFactor,FVector &q0)
  {
    const double wy = WTrans()*loadFactor;  // Transverse
    const double wx = WAxial()*loadFactor;  // Axial (+ve from node I to J)

    //Esfuerzos sobre elemento.
    const double Mz = wy*L*L/12.0; // Momento flector en z en los extremos del elemento: wy*L*L/12
    const double P = wx*L; //Axil en el extremo dorsal.

    // Fixed end forces in basic system
    q0[0]-= 0.5*P;
    q0[1]-= Mz;
    q0[2]+= Mz;
    return;
  }

void XC::Beam2dUniformLoad::addElasticDeformations(const double &L,const CrossSectionProperties2d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0)
  {
    // Length of elastic interior
    const double Le = L-lpI-lpJ;
    if(Le == 0.0)
      return;
    else
      {
        const double oneOverL = 1.0/L;
    
        const double wa= WAxial()*loadFactor;  // Axial
        const double wy= WTrans()*loadFactor;  // Transverse
    
        // Accumulate basic deformations due to uniform load on interior
        // Midpoint rule for axial
        v0[0] += wa*0.5*(L-lpI+lpJ)/(ctes_scc.EA())*Le;
    
        // Two point Gauss for bending ... will not be exact when
        // hinge lengths are not equal, but this is not a big deal!!!
        double x1 = lpI + 0.5*Le*(1.0-1.0/sqrt(3.0));
        double x2 = lpI + 0.5*Le*(1.0+1.0/sqrt(3.0));
    
        double M1 = 0.5*wy*x1*(x1-L);
        double M2 = 0.5*wy*x2*(x2-L);
    
        double b1, b2;
        double Le2EI = Le/(2*ctes_scc.EI());
        b1= x1*oneOverL;
        b2= x2*oneOverL;
        v0[2]+= Le2EI*(b1*M1+b2*M2);
   
        b1-= 1.0;
        b2-= 1.0;
        v0[1]+= Le2EI*(b1*M1+b2*M2);
      }
    return;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Beam2dUniformLoad::sendSelf(CommParameters &cp)
  {
    inicComm(6);
    int result= sendData(cp);
    const int dbTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << "Beam2dUniformLoad::sendSelf() - failed to send extra data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Beam2dUniformLoad::recvSelf(const CommParameters &cp)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Beam2dUniformLoad::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }
