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
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/element/fvectors/FVector.h"
#include "material/section/repres/ConstantesSecc3d.h"
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

//! @brief Lee un objeto Beam3dUniformLoad desde archivo
bool XC::Beam3dUniformLoad::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(Beam3dUniformLoad) Procesando comando: " << cmd << std::endl;
    if(cmd == "wx")
      {
        Axial= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "wy")
      {
        Trans= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "wz")
      {
        wz= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "tx")
      {
        tx= interpretaDouble(status.GetString());
        return true;
      }
    else
      return BeamUniformLoad::procesa_comando(status);
  }

int XC::Beam3dUniformLoad::getType(void)
  { return LOAD_TAG_Beam3dUniformLoad; }

//! @brief Applied section forces due to element uniform load
const XC::Matrix &XC::Beam3dUniformLoad::getAppliedSectionForces(const double &L,const Matrix &xi_pt,const double &loadFactor)
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

//! @brief Añade la carga al vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param L Longitud del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param p0 Vector de cargas del elemento.
void XC::Beam3dUniformLoad::addReactionsInBasicSystem(const double &L,const double &loadFactor,FVector &p0)
  {
    const double wy= Wy()*loadFactor;  // Transverse
    const double wz= Wz()*loadFactor;  // Transverse
    const double wx= Wx()*loadFactor;  // Axial (+ve from node I to J)
    //const double tx= Tx()*loadFactor;  // Torsor

    //Esfuerzos sobre elemento.
    const double Vy = 0.5*wy*L; // Cortante en y en los extremos del elemento.
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
//! @param L Longitud del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param q0 ??
void XC::Beam3dUniformLoad::addFixedEndForcesInBasicSystem(const double &L,const double &loadFactor,FVector &q0)
  {
    const double wy = Wy()*loadFactor;  // Transverse
    const double wz = Wz()*loadFactor;  // Transverse
    const double wx = Wx()*loadFactor;  // Axial (+ve from node I to J)
    //const double tx= Tx()*loadFactor;  // Torsor

    //Esfuerzos sobre elemento.
    const double Mz = wy*L*L/12.0; // Momento flector en z en los extremos del elemento: wy*L*L/12
    const double My = wz*L*L/12.0; // Momento flector en y en los extremos del elemento: wz*L*L/12
    const double P = wx*L; //Axil.

    // Fixed end forces in basic system
    q0[0]+= -0.5*P; //Axil en el extremo dorsal.
    q0[1]+= -Mz;  
    q0[2]+= Mz;
    q0[3]+= My;
    q0[4]+= -My;
  }

//! @brief Agrega las deformaciones elásticas al vector v0.
void XC::Beam3dUniformLoad::addElasticDeformations(const double &L,const ConstantesSecc3d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0)
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

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::Beam3dUniformLoad::getDbTagData(void) const
  {
    static DbTagData retval(7);
    return retval;
  }

//! @brief Envía los datos a través del canal que se pasa como parámetro.
int XC::Beam3dUniformLoad::sendData(CommParameters &cp)
  {
    int res= BeamUniformLoad::sendData(cp);
    res+= cp.sendDoubles(wz,tx,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
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

//! @brief Devuelve la dimension del vector fuerza.
size_t XC::Beam3dUniformLoad::getDimVectorFuerza(void) const
  { return 3; }

//! @brief Devuelve la dimension del vector momento.
size_t XC::Beam3dUniformLoad::getDimVectorMomento(void) const
  { return 3; }

//! @brief Devuelve las componentes de los vectores fuerza.
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

//! @brief Devuelve las componentes de los vectores momento.
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

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::Beam3dUniformLoad::GetProp(const std::string &cod) const
  {
    if(cod == "wx")
      return any_const_ptr(Axial);
    else if(cod == "wy")
      return any_const_ptr(Trans);
    else if(cod == "wz")
      return any_const_ptr(wz);
    else if(cod == "tx")
      return any_const_ptr(tx);
    else
      return BeamUniformLoad::GetProp(cod);
  }
