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
//TrussBase.cpp

#include "domain/mesh/element/truss_beam_column/truss/TrussBase.h"
#include <domain/mesh/node/Node.h>

#include "utility/actor/actor/ArrayCommMetaData.h"


//! @brief Pone a cero los punteros a nodo, los cosenos directores y los parámetros de sensibilidad.
void XC::TrussBase::inicializa(void)
  {
    // set node pointers to nullptr
    theNodes.inic();
    cosX[0]= 0.0;
    cosX[1]= 0.0;
    cosX[2]= 0.0;
    L= 0.0;
  }

void XC::TrussBase::set_load(const Vector &v)
  {
    // get a copy of the material and check we obtained a valid copy
    load= v;
  }

void XC::TrussBase::alloc_load(const size_t &sz)
  { load.resize(sz); }

//! @brief constructor:
//!  responsible for allocating the necessary space needed by each object
//!  and storing the tags of the truss end nodes.
XC::TrussBase::TrussBase(int classTag,int tag,int dim,int Nd1, int Nd2)
  :ProtoTruss(tag,classTag,Nd1,Nd2,0,dim), L(0.0)
  { inicializa(); }

//! @brief constructor:
//!  responsible for allocating the necessary space needed by each object
//!  and storing the tags of the truss end nodes.
XC::TrussBase::TrussBase(int classTag,int tag,int dim)
  :ProtoTruss(tag,classTag,0,0,0,dim), L(0.0)
  { inicializa(); }


//! constructor:
//!   invoked by a FEM_ObjectBroker - blank object that recvSelf needs
//!   to be invoked upon
XC::TrussBase::TrussBase(int classTag)
  :ProtoTruss(0,classTag,0,0,0,0), L(0.0)
  { inicializa(); }

//! @brief Constructor de copia.
XC::TrussBase::TrussBase(const TrussBase &otro)
  :ProtoTruss(otro), L(otro.L) {}

//! @brief Operador asignación.
XC::TrussBase &XC::TrussBase::operator=(const TrussBase &otro)
  {
    ProtoTruss::operator=(otro);
    L= otro.L;
    cosX[0]= otro.cosX[0];
    cosX[1]= otro.cosX[1];
    cosX[2]= otro.cosX[2];
    return *this;
  }

//! @brief Determine the length, cosines and fill in the transformation
void XC::TrussBase::setup_L_cos_dir(void)
  {
    // NOTE t = -t(every one else uses for residual calc)
    const XC::Vector &end1Crd = theNodes[0]->getCrds();
    const XC::Vector &end2Crd = theNodes[1]->getCrds();	
    if(getNumDIM() == 1)
      {
	double dx = end2Crd(0)-end1Crd(0);	
	L= sqrt(dx*dx);
	
	if(L == 0.0)
          {
	    std::cerr << "WARNING XC::TrussBase::setDomain() - truss " << this->getTag() << " has zero length\n";
	    return;
	  }	
	cosX[0] = 1.0;

      }
    else if(getNumDIM() == 2)
      {
	double dx = end2Crd(0)-end1Crd(0);
	double dy = end2Crd(1)-end1Crd(1);	
    	L = sqrt(dx*dx + dy*dy);
    	if(L == 0.0)
          {
	    std::cerr << "WARNING XC::TrussBase::setDomain() - truss " << this->getTag() << " has zero length\n";
	    return;
	  }
	cosX[0] = dx/L;
	cosX[1] = dy/L;

      }
    else
      {
	double dx = end2Crd(0)-end1Crd(0);
	double dy = end2Crd(1)-end1Crd(1);	
	double dz = end2Crd(2)-end1Crd(2);		
	L = sqrt(dx*dx + dy*dy + dz*dz);
	if(L == 0.0)
          {
	    std::cerr << "WARNING XC::TrussBase::setDomain() - truss " << this->getTag() << " has zero length\n";
	    return;
	  }
	cosX[0] = dx/L;
	cosX[1] = dy/L;
	cosX[2] = dz/L;	
      }
  }

//! @brief Devuelve la longitud del elemento.
const double &XC::TrussBase::getL(void) const
  { return L; }

//! @brief Send members through the channel being passed as parameter.
int XC::TrussBase::sendData(CommParameters &cp)
  {
    int res= ProtoTruss::sendData(cp);
    res+= cp.sendDoubles(L,cosX[0],cosX[1],cosX[2],getDbTagData(),CommMetaData(16));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::TrussBase::recvData(const CommParameters &cp)
  {
    int res= ProtoTruss::recvData(cp);
    res+= cp.receiveDoubles(L,cosX[0],cosX[1],cosX[2],getDbTagData(),CommMetaData(16));
    return res;
  }
