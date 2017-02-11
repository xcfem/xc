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

// $Revision: 1.15 $
// $Date: 2004/10/30 00:05:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/elasticBeamColumn/ElasticBeam3d.cpp,v $


// File: ~/model/ElasticBeam3d.C
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for XC::ElasticBeam3d.
// ElasticBeam3d is a 3d beam element. As such it can only
// connect to a node with 6-dof.

#include <domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam3d.h>
#include <domain/domain/Domain.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include <domain/mesh/element/utils/coordTransformation/CrdTransf3d.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/ElementResponse.h>
#include "domain/load/beam_loads/BeamMecLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"
#include <domain/mesh/node/Node.h>
#include <material/section/SectionForceDeformation.h>
#include <utility/matrix/ID.h>

#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableVector.h"

XC::Matrix XC::ElasticBeam3d::K(12,12);
XC::Vector XC::ElasticBeam3d::P(12);
XC::Matrix XC::ElasticBeam3d::kb(6,6);

void XC::ElasticBeam3d::set_transf(const CrdTransf *trf)
  {
    if(theCoordTransf)
      {
        delete theCoordTransf;
        theCoordTransf= nullptr;
      }
    if(trf)
      {
        const CrdTransf3d *tmp= dynamic_cast<const CrdTransf3d *>(trf);
        if(tmp)
          theCoordTransf = tmp->getCopy();
        else
          {
            std::cerr << "ElasticBeam3d::ElasticBeam3d -- failed to get copy of coordinate transformation\n";
            exit(-1);
          }
      }
    else
      std::cerr << "ElasticBeam3d::set_transf; se pasó a null pointer." << std::endl;
  }

//! @brief Default constructor.
XC::ElasticBeam3d::ElasticBeam3d(int tag)
  :ProtoBeam3d(tag,ELE_TAG_ElasticBeam3d), eInic(3), rho(0.0),
   sectionTag(0), q(), q0(), p0(), theCoordTransf(0)
  { load.reset(12); }

//! @brief Constructor.
XC::ElasticBeam3d::ElasticBeam3d(int tag,const Material *m,const CrdTransf *trf)
  :ProtoBeam3d(tag,ELE_TAG_ElasticBeam3d,m), eInic(3), rho(0.0),
   sectionTag(0), q(), q0(), p0(), theCoordTransf(nullptr)
  { load.reset(12); set_transf(trf); }

//! @brief Constructor.
XC::ElasticBeam3d::ElasticBeam3d(int tag, double a, double e, double g,
                             double jx, double iy, double iz, int Nd1, int Nd2,
                             CrdTransf3d &coordTransf, double r, int sectTag)
  :ProtoBeam3d(tag,ELE_TAG_ElasticBeam3d,a,e,g,jx,iy,iz,Nd1,Nd2), eInic(3),
   rho(r), sectionTag(sectTag), q(), q0(), p0(), theCoordTransf(0)
  {
    load.reset(12);
    set_transf(&coordTransf);
  }

//! @brief Constructor.
XC::ElasticBeam3d::ElasticBeam3d(int tag, int Nd1, int Nd2, SectionForceDeformation *section,CrdTransf3d &coordTransf, double r)
  :ProtoBeam3d(tag,ELE_TAG_ElasticBeam3d,Nd1,Nd2), eInic(3), rho(0.0), q(), theCoordTransf(0)
  {
    load.reset(12);
    if(section)
      {
        sectionTag= section->getTag();
        ctes_scc.E() = 1.0;
        ctes_scc.G() = 1.0;
        ctes_scc.J() = 0.0;
        rho = r;

        const Matrix &sectTangent = section->getSectionTangent();
        const ID &sectCode = section->getType();
        for(int i=0; i<sectCode.Size(); i++)
          {
            int code = sectCode(i);
            switch(code)
              {
              case SECTION_RESPONSE_P:
                ctes_scc.A() = sectTangent(i,i);
                break;
              case SECTION_RESPONSE_MZ:
                ctes_scc.Iz() = sectTangent(i,i);
                break;
              case SECTION_RESPONSE_MY:
                ctes_scc.Iy() = sectTangent(i,i);
                break;
              case SECTION_RESPONSE_T:
                ctes_scc.J() = sectTangent(i,i);
                break;
              default:
                break;
              }
          }
      }

    if(ctes_scc.J()==0.0)
      {
        std::cerr << "XC::ElasticBeam3d::ElasticBeam3d -- no torsion in section -- setting GJ = 1.0e10\n";
        ctes_scc.J() = 1.0e10;
      }

    theCoordTransf = coordTransf.getCopy();

    if(!theCoordTransf)
      {
        std::cerr << "XC::ElasticBeam3d::ElasticBeam3d -- failed to get copy of coordinate transformation\n";
        exit(-1);
      }

    q0.zero();
    p0.zero();
  }

//! @brief Constructor de copia.
XC::ElasticBeam3d::ElasticBeam3d(const XC::ElasticBeam3d &otro)
  :ProtoBeam3d(otro), eInic(otro.eInic), rho(otro.rho),
   sectionTag(otro.sectionTag), q(otro.q), theCoordTransf(nullptr)
  {
    set_transf(otro.theCoordTransf);

    q0= otro.q0;
    p0= otro.p0;
  }

//! @brief Oparador asignación.
XC::ElasticBeam3d &XC::ElasticBeam3d::operator=(const XC::ElasticBeam3d &otro)
  {
    ProtoBeam3d::operator=(otro);
    eInic= otro.eInic;
    rho= otro.rho;
    sectionTag= otro.sectionTag;
    q= otro.q;
    set_transf(otro.theCoordTransf);

    q0= otro.q0;
    p0= otro.p0;
    return *this;
  }

//! @brief Constructor virtual.
XC::Element* XC::ElasticBeam3d::getCopy(void) const
  { return new ElasticBeam3d(*this); }

//! @brief Constructor.
XC::ElasticBeam3d::~ElasticBeam3d(void)
  { if(theCoordTransf) delete theCoordTransf;  }

//! @brief Devuelve (si puede) un apuntador a la coordinate transformation.
XC::CrdTransf *XC::ElasticBeam3d::getCoordTransf(void)
  { return theCoordTransf; }

//! @brief Devuelve (si puede) un apuntador a la coordinate transformation.
const XC::CrdTransf *XC::ElasticBeam3d::getCoordTransf(void) const
  { return theCoordTransf; }

void XC::ElasticBeam3d::setDomain(Domain *theDomain)
  {
    ProtoBeam3d::setDomain(theDomain);

    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();

    if(dofNd1 != 6)
      {
        std::cerr << "XC::ElasticBeam3d::setDomain -- Node 1: " << theNodes.getTagNode(0)
                  << " has incorrect number of DOF\n";
        exit(-1);
      }
    if(dofNd2 != 6)
      {
        std::cerr << "XC::ElasticBeam3d::setDomain -- Node 2: " << theNodes.getTagNode(1)
                  << " has incorrect number of DOF\n";
        exit(-1);
      }

    if(theCoordTransf->initialize(theNodes[0], theNodes[1]) != 0)
      {
        std::cerr << "XC::ElasticBeam3d::setDomain -- Error initializing coordinate transformation\n";
        exit(-1);
      }

    const double L = theCoordTransf->getInitialLength();

    if(L == 0.0)
      {
        std::cerr << "XC::ElasticBeam3d::setDomain -- Element has zero length\n";
        exit(-1);
      }
  }

int XC::ElasticBeam3d::setInitialSectionDeformation(const Vector &def)
  {
    eInic= def;
    return 0;
  }

//! @brief Devuelve la deformación de la sección del elemento.
const XC::Vector &XC::ElasticBeam3d::getSectionDeformation(void) const
  {
    static Vector retval(5);
    theCoordTransf->update();
    const double L = theCoordTransf->getInitialLength();
    // retval(0)= dx2-dx1: Elongación sufrida por el elemento/L.
    // retval(1)= (dy1-dy2)/L+gz1: Giro en torno a z/L.
    // retval(2)= (dy1-dy2)/L+gz2: Giro en torno a z/L.
    // retval(3)= (dz2-dz1)/L+gy1: Giro en torno a y/L.
    // retval(4)= (dz2-dz1)/L+gy2: Giro en torno a y/L.
    // retval(5)= dx2-dx1: Torsión sufrida por el elemento/L.
    retval= theCoordTransf->getBasicTrialDisp()/L;
    retval(0)-= eInic(0);
    retval(1)-= eInic(1);
    retval(2)-= eInic(1);
    retval(1)-= eInic(2);
    retval(2)-= eInic(2);
    return retval;
  }


int XC::ElasticBeam3d::commitState(void)
  {
    int retVal = 0;
    // call element commitState to do any base class stuff
    if((retVal = this->XC::Element::commitState()) != 0)
      { std::cerr << "XC::ElasticBeam3d::commitState () - failed in base class"; }
    retVal += theCoordTransf->commitState();
    return retVal;
  }

int XC::ElasticBeam3d::revertToLastCommit()
  { return theCoordTransf->revertToLastCommit(); }

int XC::ElasticBeam3d::revertToStart()
  { return theCoordTransf->revertToStart(); }

int XC::ElasticBeam3d::update(void)
  { return theCoordTransf->update(); }

//! @brief Devuelve la matriz de rigidez tangente expresada en coordenadas globales.
const XC::Matrix &XC::ElasticBeam3d::getTangentStiff(void) const
  {
    const Vector &v= getSectionDeformation();

    //No considera secciones que tengan productos de
    //inercia no nulos.
    const double eiyz= ctes_scc.EIyz();
    const double eimax= std::max(ctes_scc.EIz(),ctes_scc.EIy());
    if(std::abs(eiyz/eimax)>1e-5) //Producto de inercia no nulo.
      std::cerr << "ElasticBeam3d::getTangentStiff; este elemento no"
                << " debe usarse con secciones que tienen"
                << " su producto de inercia no nulo."
                << std::endl;

    const double E= ctes_scc.E();
    const double EA= ctes_scc.A()*E; // EA
    const double EIz2= 2.0*ctes_scc.Iz()*E; // 2EIz
    const double EIz4= 2.0*EIz2; // 4EIz
    const double EIy2= 2.0*ctes_scc.Iy()*E; // 2EIy
    const double EIy4= 2.0*EIy2; // 4EIy
    const double GJ= ctes_scc.GJ(); // GJ

    //Esfuerzos en el elemento debidos a las acciones de la estructura sobre sus nodos.
    q.N()= EA*v(0); //Axil en el elemento.
    q.Mz1()= EIz4*v(1) + EIz2*v(2); //Momento z en el nodo 1: 4EI/L*v(1)+2EI/L*v(2).
    q.Mz2()= EIz2*v(1) + EIz4*v(2); //Momento z en el nodo 2: 2EI/L*v(1)+4EI/L*v(2).
    q.My1()= EIy4*v(3) + EIy2*v(4); //Momento y en el nodo 1: 4EI/L*v(3)+2EI/L*v(4).
    q.My2()= EIy2*v(3) + EIy4*v(4); //Momento y en el nodo 2: 2EI/L*v(3)+4EI/L*v(4).
    q.T() = GJ*v(5); //Torsor en el elemento.

    q.N()+= q0[0];
    q.Mz1()+= q0[1];
    q.Mz2()+= q0[2];
    q.My1()+= q0[3];
    q.My2()+= q0[4];

    const double L = theCoordTransf->getInitialLength();
    kb(0,0) = EA/L;
    kb(1,1) = kb(2,2)= EIz4/L;
    kb(2,1) = kb(1,2)= EIz2/L;
    kb(3,3) = kb(4,4)= EIy4/L;
    kb(4,3) = kb(3,4)= EIy2/L;
    kb(5,5) = GJ/L;

    static Matrix retval;
    retval= theCoordTransf->getGlobalStiffMatrix(kb,q);
    if(isDead())
      retval*=dead_srf;

    return retval;
  }


const XC::Matrix &XC::ElasticBeam3d::getInitialStiff(void) const
  {
    //No considera secciones que tengan productos de
    //inercia no nulos.
    const double eiyz= ctes_scc.EIyz();
    const double eimax= std::max(ctes_scc.EIz(),ctes_scc.EIy());
    if(std::abs(eiyz/eimax)>1e-5) //Producto de inercia no nulo.
      std::cerr << "ElasticBeam3d::getTangentStiff; este elemento no"
                << " debe usarse con secciones que tienen"
                << " su producto de inercia no nulo."
                << std::endl;

    const double L = theCoordTransf->getInitialLength();
    const double oneOverL = 1.0/L;
    const double EoverL   = ctes_scc.E()*oneOverL;
    const double EAoverL  = ctes_scc.A()*EoverL; // EA/L
    const double EIzoverL2 = 2.0*ctes_scc.Iz()*EoverL; // 2EIz/L
    const double EIzoverL4 = 2.0*EIzoverL2; // 4EIz/L
    const double EIyoverL2 = 2.0*ctes_scc.Iy()*EoverL; // 2EIy/L
    const double EIyoverL4 = 2.0*EIyoverL2; // 4EIy/L
    const double GJoverL = ctes_scc.GJ()*oneOverL; // GJ/L

    kb(0,0) = EAoverL;
    kb(1,1) = kb(2,2) = EIzoverL4;
    kb(2,1) = kb(1,2) = EIzoverL2;
    kb(3,3) = kb(4,4) = EIyoverL4;
    kb(4,3) = kb(3,4) = EIyoverL2;
    kb(5,5) = GJoverL;

    static Matrix retval;
    retval= theCoordTransf->getInitialGlobalStiffMatrix(kb);
    if(isDead())
      retval*=dead_srf;

    return retval;
  }

//! @brief Devuelve la matriz de masas del elemento.
const XC::Matrix &XC::ElasticBeam3d::getMass(void) const
  {
    K.Zero();

    if(rho > 0.0)
      {
        const double L= theCoordTransf->getInitialLength();
        const double m= 0.5*rho*L;
        const double mpeq= m*1.0e-10;

        K(0,0)= m;
        K(1,1)= m;
        K(2,2)= m;
        K(3,3)= mpeq; //Modificado por LCPT para evitar matriz M singular.
        K(4,4)= mpeq; //Modificado por LCPT para evitar matriz M singular.
        K(5,5)= mpeq; //Modificado por LCPT para evitar matriz M singular.

        K(6,6)= m;
        K(7,7)= m;
        K(8,8)= m;
        K(9,9)= mpeq; //Modificado por LCPT para evitar matriz M singular.
        K(10,10)= mpeq; //Modificado por LCPT para evitar matriz M singular.
        K(11,11)= mpeq; //Modificado por LCPT para evitar matriz M singular.
      }
    if(isDead())
      K*=dead_srf;
    return K;
  }

void XC::ElasticBeam3d::zeroLoad(void)
  {
    ProtoBeam3d::zeroLoad();
    q0.zero();
    p0.zero();
    return;
  }

int XC::ElasticBeam3d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << nombre_clase() 
                << "; se intentó cargar el elemento "
                << getTag() << " que está desactivado." 
                << std::endl;
    else
      {
        if(BeamMecLoad *beamMecLoad= dynamic_cast<BeamMecLoad *>(theLoad))
          {
            const double L = theCoordTransf->getInitialLength();
            beamMecLoad->addReactionsInBasicSystem(L,loadFactor,p0); // Accumulate reactions in basic system
            beamMecLoad->addFixedEndForcesInBasicSystem(L,loadFactor,q0); // Fixed end forces in basic system
          }
        else if(BeamStrainLoad *strainLoad= dynamic_cast<BeamStrainLoad *>(theLoad)) //Deformaciones impuestas.
          {
            const int order= 3;
            const RespPMzMy code;
            const Vector &e1= strainLoad->getSection1Deformation(order,code)*loadFactor;
            const Vector &e2= strainLoad->getSection2Deformation(order,code)*loadFactor;
            Vector ezero= getInitialSectionDeformation();
            ezero+= (e2+e1)/2;
            setInitialSectionDeformation(ezero);
          }
        else
          {
            std::cerr << "XC::ElasticBeam3d::addLoad()  -- load type unknown for element with tag: " << this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }


int XC::ElasticBeam3d::addInertiaLoadToUnbalance(const XC::Vector &accel)
  {
    if(rho == 0.0) return 0;

    // Get R * accel from the nodes
    const XC::Vector &Raccel1 = theNodes[0]->getRV(accel);
    const XC::Vector &Raccel2 = theNodes[1]->getRV(accel);

    if(6 != Raccel1.Size() || 6 != Raccel2.Size())
      {
        std::cerr << "XC::ElasticBeam3d::addInertiaLoadToUnbalance matrix and vector sizes are incompatable\n";
        return -1;
      }

    // Want to add ( - fact * M R * accel ) to unbalance
    // Take advantage of lumped mass matrix
    const double L = theCoordTransf->getInitialLength();
    const double m = 0.5*rho*L;

    load(0)-= m * Raccel1(0);
    load(1)-= m * Raccel1(1);
    load(2)-= m * Raccel1(2);

    load(6)-= m * Raccel2(0);
    load(7)-= m * Raccel2(1);
    load(8)-= m * Raccel2(2);

    return 0;
  }


const XC::Vector &XC::ElasticBeam3d::getResistingForceIncInertia(void) const
  {
    P = this->getResistingForce();

    // add the damping forces if rayleigh damping
    if(!rayFactors.Nulos())
      P += this->getRayleighDampingForces();

    if(rho == 0.0)
      return P;
    else
      {
        const Vector &accel1= theNodes[0]->getTrialAccel();
        const Vector &accel2= theNodes[1]->getTrialAccel();

        const double L= theCoordTransf->getInitialLength();
        const double m= 0.5*rho*L;

        P(0) += m * accel1(0);
        P(1) += m * accel1(1);
        P(2) += m * accel1(2);

        P(6) += m * accel2(0);
        P(7) += m * accel2(1);
        P(8) += m * accel2(2);
        return P;
      }
    if(isDead())
      P*=dead_srf;
  }

const XC::Vector &XC::ElasticBeam3d::getResistingForce(void) const
  {
    const Vector &v= getSectionDeformation();

    const double E= ctes_scc.E();
    const double EA= ctes_scc.A()*E; // EA
    const double EIz2= 2.0*ctes_scc.Iz()*E; // 2EIz
    const double EIz4= 2.0*EIz2; // 4EIz
    const double EIy2= 2.0*ctes_scc.Iy()*E; // 2EIy
    const double EIy4= 2.0*EIy2; // 4EIy
    const double GJ= ctes_scc.GJ(); // GJ

    //Esfuerzos en el elemento debidos a las acciones de la estructura sobre sus nodos.
    q.N()= EA*v(0); //Axil en el elemento.
    q.Mz1()= EIz4*v(1) + EIz2*v(2); //Momento z en el nodo 1: 4EI/L*v(1)+2EI/L*v(2).
    q.Mz2()= EIz2*v(1) + EIz4*v(2); //Momento z en el nodo 2: 2EI/L*v(1)+4EI/L*v(2).
    q.My1()= EIy4*v(3) + EIy2*v(4); //Momento y en el nodo 1: 4EI/L*v(3)+2EI/L*v(4).
    q.My2()= EIy2*v(3) + EIy4*v(4); //Momento y en el nodo 2: 2EI/L*v(3)+4EI/L*v(4).
    q.T()= GJ*v(5); //Torsor en el elemento.

    q.N()+= q0[0];
    q.Mz1()+= q0[1];
    q.Mz2()+= q0[2];
    q.My1()+= q0[3];
    q.My2()+= q0[4];

    Vector p0Vec= p0.getVector();

    //  std::cerr << q;

    P = theCoordTransf->getGlobalResistingForce(q, p0Vec);

    // std::cerr << P;

    // P = P - load;
    P.addVector(1.0, load, -1.0);

    if(isDead())
      P*=dead_srf;
    return P;
  }


//! @brief Returns the direction vector of element strong axis
//! expressed in the global coordinate system.
const XC::Vector &XC::ElasticBeam3d::getVDirEjeFuerteGlobales(void) const
  {
    if(theCoordTransf)
      {
        const Vector eF= getVDirEjeFuerteLocales();
        return theCoordTransf->getVectorGlobalCoordFromLocal(eF);
      }
    else
      {
        std::cerr << "ElasticBeam3d::getVDirEjeFuerteGlobales; no se ha asignado una coordinate transformation."
                  << std::endl;
        return P;
      }
  }

//! @brief Returns the direction vector of element weak axis
//! expressed in the global coordinate system.
const XC::Vector &XC::ElasticBeam3d::getVDirEjeDebilGlobales(void) const
  {
    if(theCoordTransf)
      {
        const Vector eD= getVDirEjeDebilLocales();
        return theCoordTransf->getVectorGlobalCoordFromLocal(eD);
      }
    else
      {
        std::cerr << "ElasticBeam3d::getVDirEjeDebilGlobales; no se ha asignado una coordinate transformation."
                  << std::endl;
        return P;
      }
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::ElasticBeam3d::getDbTagData(void) const
  {
    static DbTagData retval(17);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::ElasticBeam3d::sendData(CommParameters &cp)
  {
    DbTagData &dt= getDbTagData();
    int res= ProtoBeam3d::sendData(cp);
    res+= sendCoordTransf(8,9,10,cp);
    res+= cp.sendVector(eInic,dt,CommMetaData(11));
    res+= cp.sendDouble(rho,dt,CommMetaData(12));
    res+= cp.sendInt(sectionTag,dt,CommMetaData(13));
    res+= sendEsfBeamColumn3d(q,14,dt,cp);
    res+= p0.sendData(cp,dt,CommMetaData(15));
    res+= q0.sendData(cp,dt,CommMetaData(16));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::ElasticBeam3d::recvData(const CommParameters &cp)
  {
    DbTagData &dt= getDbTagData();
    int res= ProtoBeam3d::recvData(cp);
    theCoordTransf= recvCoordTransf3d(8,9,10,cp);
    res+= cp.receiveVector(eInic,dt,CommMetaData(11));
    res+= cp.receiveDouble(rho,dt,CommMetaData(12));
    res+= cp.receiveInt(sectionTag,dt,CommMetaData(13));
    res+= receiveEsfBeamColumn3d(q,14,dt,cp);
    res+= p0.receiveData(cp,dt,CommMetaData(15));
    res+= q0.receiveData(cp,dt,CommMetaData(16));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::ElasticBeam3d::sendSelf(CommParameters &cp)
  {
    inicComm(17);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ElasticBeam3d::sendSelf -- could not send data Vector\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::ElasticBeam3d::recvSelf(const CommParameters &cp)
  {
    inicComm(17);
    const int dataTag= getDbTag();
    int res = cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ElasticBeam3d::recvSelf() - failed to send ID data\n";
    else
      res+= recvData(cp);
    return res;
  }

void XC::ElasticBeam3d::Print(std::ostream &s, int flag)
  {
   if(flag == -1)
     {
       int eleTag = this->getTag();
       s << "EL_BEAM\t" << eleTag << "\t";
       s << sectionTag << "\t" << sectionTag;
       s  << "\t" << theNodes.getTagNode(0) << "\t" << theNodes.getTagNode(1);
       s << "\t0\t0.0000000\n";
     }
   else
     {
       this->getResistingForce(); // in case linear algo
       const double L = theCoordTransf->getInitialLength();
       if(flag < -1)
         {
           int counter = (flag + 1) * -1;
           int eleTag = this->getTag();
           s << "FORCE\t" << eleTag << "\t" << counter << "\t0";
           s << "\t" << q.AN1()+p0[0] << "\t"  <<  q.Vy(L)+p0[1] << "\t"  << -q.Vz(L)+p0[3]  << std::endl;
           s << "FORCE\t" << eleTag << "\t" << counter << "\t1";
           s << "\t"  << q.AN2() << ' '  << -q.Vy(L)+p0[2] << ' ' << q.Vz(L)+p0[4] << std::endl;
           s << "MOMENT\t" << eleTag << "\t" << counter << "\t0";
           s << "\t" << q.T1() << "\t"  << q.My1() << "\t" << q.Mz1() << std::endl;
           s << "MOMENT\t" << eleTag << "\t" << counter << "\t1";
           s << "\t" << q.T2() << ' ' << q.My2() << ' '  <<  q.Mz2() << std::endl;
         }
       else if(flag == 2)
         {
           static XC::Vector xAxis(3);
           static XC::Vector yAxis(3);
           static XC::Vector zAxis(3);

           theCoordTransf->getLocalAxes(xAxis, yAxis, zAxis);

           s << "#ElasticBeamColumn3D\n";
           s << "#LocalAxis " << xAxis(0) << " " << xAxis(1) << " " << xAxis(2);
           s << " " << yAxis(0) << " " << yAxis(1) << " " << yAxis(2) << " ";
           s << zAxis(0) << " " << zAxis(1) << " " << zAxis(2) << std::endl;

           const XC::Vector &node1Crd = theNodes[0]->getCrds();
           const XC::Vector &node2Crd = theNodes[1]->getCrds();
           const XC::Vector &node1Disp = theNodes[0]->getDisp();
           const XC::Vector &node2Disp = theNodes[1]->getDisp();

           s << "#NODE " << node1Crd(0) << " " << node1Crd(1) << " " << node1Crd(2)
             << " " << node1Disp(0) << " " << node1Disp(1) << " " << node1Disp(2)
             << " " << node1Disp(3) << " " << node1Disp(4) << " " << node1Disp(5) << std::endl;

           s << "#NODE " << node2Crd(0) << " " << node2Crd(1) << " " << node2Crd(2)
             << " " << node2Disp(0) << " " << node2Disp(1) << " " << node2Disp(2)
             << " " << node2Disp(3) << " " << node2Disp(4) << " " << node2Disp(5) << std::endl;


           s << "#END_FORCES " << q.AN1()+p0[0] << ' ' <<  q.Vy(L)+p0[1] << ' ' << q.Vz(L)+p0[3] << ' '
             << q.T1() << ' ' << q.My1() << ' ' <<  q.Mz1() << std::endl;
           s << "#END_FORCES " <<  q.AN2() << ' ' << -q.Vy(L)+p0[2] << ' ' << -q.Vz(L)+p0[4] << ' '
             << q.T2() << ' ' << q.My2() << ' ' << q.Mz2() << std::endl;
          }
        else
          {

            s << "\nElasticBeam3d: " << this->getTag() << std::endl;
            s << "\tConnected Nodes: " << theNodes;
            s << "\tCoordTransf: " << theCoordTransf->getTag() << std::endl;


            s << "\tEnd 1 Forces (N Mz Vy My Vz T): "
              << q.AN1()+p0[0] << ' ' << q.Mz1() << ' ' <<  q.Vy(L)+p0[1] << ' ' << q.My1() << ' ' <<  q.Vz(L)+p0[3] << ' ' << q.T1() << std::endl;
            s << "\tEnd 2 Forces (N Mz Vy My Vz T): "
              <<  q.AN2() << ' ' << q.Mz2() << ' ' << -q.Vy(L)+p0[2] << ' ' << q.My2() << ' ' << -q.Vz(L)+p0[4] << ' ' <<  q.T2() << std::endl;
          }
     }
  }

XC::Response *XC::ElasticBeam3d::setResponse(const std::vector<std::string> &argv, Information &info)
  {
    // stiffness
    if(argv[0] == "stiffness")
                return new ElementResponse(this, 1, K);

    // global forces
    else if(argv[0] == "force" || argv[0] == "forces" ||
                argv[0] == "globalForce" || argv[0] == "globalForces")
                return new ElementResponse(this, 2, P);

        // local forces
    else if(argv[0] == "localForce" || argv[0] == "localForces")
                return new ElementResponse(this, 3, P);

    else
                return 0;
  }

int XC::ElasticBeam3d::getResponse(int responseID, Information &eleInfo)
  {
    double V;
    const double L = theCoordTransf->getInitialLength();

    switch(responseID)
      {
      case 1: // stiffness
        return eleInfo.setMatrix(getTangentStiff());
      case 2: // global forces
        return eleInfo.setVector(getResistingForce());
      case 3: // local forces
        // Axial
        P(6) =  q.AN2();
        P(0) = q.AN1()+p0[0];

        // Torsion
        P(9) = q.T2();
        P(3) = q.T1();

        // Moments about z and shears along y
        P(5)  = q.Mz1();
        P(11) = q.Mz2();
        V = q.Vy(L);
        P(1) =  V+p0[1];
        P(7) = -V+p0[2];

        // Moments about y and shears along z
        P(4)  = q.My1();
        P(10) = q.My2();
        V = q.Vz(L);
        P(2) = -V+p0[3];
        P(8) =  V+p0[4];
        return eleInfo.setVector(P);
      default:
        return -1;
      }
  }
