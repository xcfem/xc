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
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/element/fvectors/FVector.h"
#include "material/section/repres/ConstantesSecc3d.h"
#include "utility/actor/actor/MovableVector.h"

XC::Vector XC::Beam3dPointLoad::data(4);

XC::Beam3dPointLoad::Beam3dPointLoad(int tag, double py, double pz, double dist,const XC::ID &theElementTags, double px)
  :BeamPointLoad(tag, LOAD_TAG_Beam3dPointLoad,py,dist,theElementTags,px), Pz(pz) {}

XC::Beam3dPointLoad::Beam3dPointLoad(int tag)
  :BeamPointLoad(tag,LOAD_TAG_Beam3dPointLoad),Pz(0.0) {}

XC::Beam3dPointLoad::Beam3dPointLoad(void)
  :BeamPointLoad(0,LOAD_TAG_Beam3dPointLoad),Pz(0.0) {}

//! @brief Devuelve la dimension del vector fuerza.
size_t XC::Beam3dPointLoad::getDimVectorFuerza(void) const
  { return 3; }

//! @brief Devuelve la dimension del vector momento.
size_t XC::Beam3dPointLoad::getDimVectorMomento(void) const
  { return 3; }

//! @brief Devuelve las componentes de los vectores fuerza.
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

//! @brief Devuelve las componentes de los vectores momento.
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
const XC::Matrix &XC::Beam3dPointLoad::getAppliedSectionForces(const double &L,const XC::Matrix &xi,const double &loadFactor)
  {
    const size_t nSections= xi.noCols();
    static Matrix retval(5,nSections); //Sólo se ejecuta una vez.
    retval.resize(5,nSections);
    retval.Zero();
    const double Py= py()*loadFactor;
    const double Pz= pz()*loadFactor;
    const double N= px()*loadFactor;
    const double aOverL= X();

    if(aOverL > 0.0 && aOverL < 1.0)
      {
        const double a= aOverL*L;
        const double Vy2 = Py*aOverL; //Reacción y en extremo frontal.
        const double Vy1 = Py-Vy2; //Reacción y en extremo dorsal.
        const double Vz2 = Pz*aOverL; //Reacción z en extremo frontal.
        const double Vz1 = Pz-Vz2; //Reacción z en extremo dorsal.
    
    
        // Accumulate applied section forces due to element loads
        for(size_t i = 0; i < nSections; i++)
          {
            const double x= xi(i,0)*L;
            if(x<=a)
              {
                retval(0,i)= N;
                retval(1,i)= -x*Vy1; //Momento z para x<=a.
                retval(2,i)= -Vy1; //Cortante y para x<=a.
                retval(3,i)= x*Vz1; //Momento y para x<=a.
                retval(4,i)= -Vz1; //Cortante z para x<=a.
              }
            else
              {
                retval(1,i)= -(L-x)*Vy2; //Momento z para x>=a.
                retval(2,i)= Vy2; //Cortante y para x>=a.
                retval(3,i)= (L-x)*Vz2; //Momento y para x>=a.
                retval(4,i)= Vz2; //Cortante z para x>=a.
              }
          }
      }
    return retval;
  }

//! @brief Añade la carga al vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param L Longitud del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param p0 Vector de cargas del elemento.
void XC::Beam3dPointLoad::addReactionsInBasicSystem(const double &L,const double &loadFactor,FVector &p0)
  {
    const double aOverL= X();

    if(aOverL < 0.0 || aOverL > 1.0)
      {
        std::cerr << "XC::Element1D::addReactionsInBasicSystem; el valor de x ("
                  << aOverL << ") es incorrecto, debe estar entre 0 y 1. Se ignora la carga." << std::endl;
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

//! @brief ??
//! @param L Longitud del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param q0 ??.
void XC::Beam3dPointLoad::addFixedEndForcesInBasicSystem(const double &L,const double &loadFactor,FVector &q0)
  {
    const double aOverL= X();

    if(aOverL < 0.0 || aOverL > 1.0)
      {
        std::cerr << "XC::Element1D::fixedEndForcesInBasicSystem; el valor de x ("
                  << aOverL << ") es incorrecto, debe estar entre 0 y 1. Se ignora la carga." << std::endl;
      }
    else
      {
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
void XC::Beam3dPointLoad::addElasticDeformations(const double &L,const ConstantesSecc3d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0)
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

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::Beam3dPointLoad::getDbTagData(void) const
  {
    static DbTagData retval(8);
    return retval;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::Beam3dPointLoad::sendSelf(CommParameters &cp)
  {
    inicComm(8);
    int result= sendData(cp);
    result+= cp.sendDouble(Pz,getDbTagData(),CommMetaData(7));

    const int dbTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << "Beam3dPointLoad::sendSelf() - failed to send extra data\n";
    return result;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::Beam3dPointLoad::recvSelf(const CommParameters &cp)
  {
    inicComm(8);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "Beam3dPointLoad::recvSelf() - data could not be received\n" ;
    else
      {
        res+= recvData(cp);
        res+= cp.receiveDouble(Pz,getDbTagData(),CommMetaData(7));
      }
    return res;
  }

//! @brief Imprime la carga.
void XC::Beam3dPointLoad::Print(std::ostream &s, int flag) const
  {
    s << "Beam3dPointLoad - Reference load" << std::endl;
    s << "  Transverse (y): " << Trans << std::endl;
    s << "  Transverse (z): " << Pz << std::endl;
    s << "  Axial (x):      " << Axial << std::endl;
    s << "  Elements acted on: " << this->getElementTags();
  }
