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

// $Revision: 1.2 $
// $Date: 2009/11/03 23:12:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/frictionBearing/FlatSliderSimple2d.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the implementation of the
// FlatSliderSimple2d class.

#include "FlatSliderSimple2d.h"
#include "material/uniaxial/UniaxialMaterial.h"
#include "frictionModel/FrictionModel.h"
#include "utility/actor/actor/CommMetaData.h"

void XC::ElemFriccionBase::libera_friction_model(void)
  {
    if(theFrnMdl)
      delete theFrnMdl;
    theFrnMdl= nullptr;
  }

void XC::ElemFriccionBase::alloc_friction_model(const FrictionModel &thefrnmdl)
  {
    libera_friction_model();
    theFrnMdl = thefrnmdl.getCopy();
    if(!theFrnMdl)
      {
	std::cerr << "ElemFriccionBase::ElemFriccionBase() - "
		  << "failed to get copy of the friction model.\n";
	exit(-1);
      }
  }

XC::ElemFriccionBase::ElemFriccionBase(int tag, int classTag,int Nd1, int Nd2,const size_t &dim,
					const FrictionModel &thefrnmdl, const UniaxialMatPhysicalProperties &physProp, const double &_uy,
				       const Vector &_y, const Vector &_x, const double &m,const int &maxiter, const double &_tol)
  : Element0D(tag, classTag,Nd1,Nd2,2), theFrnMdl(nullptr), physicalProperties(physProp),
    uy(_uy), x(_x), y(_y), mass(m), maxIter(maxiter), tol(_tol),
    L(0.0), ub(dim), qb(dim), kb(dim,dim), ul(dim*2),
    Tgl(dim*2,dim*2), Tlb(dim,dim*2), kbInit(dim,dim)
  {
    alloc_friction_model(thefrnmdl);
  }


XC::ElemFriccionBase::ElemFriccionBase(int classTag,const size_t &dim)
  : Element0D(0,classTag,0,0,0), theFrnMdl(nullptr), physicalProperties(0,nullptr),
    uy(0.0), x(), y(), mass(0.0), maxIter(20), tol(1E-8),
    L(0.0), ub(dim), qb(dim), kb(dim,dim), ul(dim*2),
    Tgl(dim*2,dim*2), Tlb(dim,dim*2), kbInit(dim,dim)
  {}

XC::ElemFriccionBase::ElemFriccionBase(const ElemFriccionBase &otro)
  :Element0D(otro), physicalProperties(otro.physicalProperties), uy(otro.uy), x(otro.x), y(otro.y), mass(otro.mass), maxIter(otro.maxIter), tol(otro.tol),
    L(otro.L), ub(otro.ub), qb(otro.qb), kb(otro.kb), ul(otro.ul),
    Tgl(otro.Tgl), Tlb(otro.Tlb), kbInit(otro.kbInit)
  {
    if(otro.theFrnMdl)
      alloc_friction_model(*otro.theFrnMdl);
  }

XC::ElemFriccionBase &XC::ElemFriccionBase::operator=(const ElemFriccionBase &otro)
  {
    Element0D::operator=(otro);
    physicalProperties= otro.physicalProperties;
    uy= otro.uy;
    x= otro.x;
    y= otro.y;
    mass= otro.mass;
    maxIter= otro.maxIter;
    tol= otro.tol;
    L= otro.L;
    ub= otro.ub;
    qb= otro.qb;
    kb= otro.kb;
    ul= otro.ul;
    Tgl= otro.Tgl;
    Tlb= otro.Tlb;
    kbInit= otro.kbInit;
    if(otro.theFrnMdl)
      alloc_friction_model(*otro.theFrnMdl);
    return *this;
  }
XC::ElemFriccionBase::~ElemFriccionBase()
  {
    libera_friction_model();
  }


//! @brief Envía los miembros por el canal que se pasa como parámetro.
int XC::ElemFriccionBase::sendData(CommParameters &cp)
  {
    int res= Element0D::sendData(cp);
    std::cerr<< "ElemFriccionBase::sendData incompleta (falta modelo friccion y materiales)" << std::endl;
    res+= cp.sendDoubles(uy,mass,tol,L,getDbTagData(),CommMetaData(9));
    res+= cp.sendVector(x,getDbTagData(),CommMetaData(10));
    res+= cp.sendVector(y,getDbTagData(),CommMetaData(11));
    res+= cp.sendInt(maxIter,getDbTagData(),CommMetaData(12));
    res+= cp.sendVector(ub,getDbTagData(),CommMetaData(13));
    res+= cp.sendVector(qb,getDbTagData(),CommMetaData(14));
    res+= cp.sendMatrix(kb,getDbTagData(),CommMetaData(15));
    res+= cp.sendVector(ul,getDbTagData(),CommMetaData(16));
    res+= cp.sendMatrix(Tgl,getDbTagData(),CommMetaData(17));
    res+= cp.sendMatrix(Tlb,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Recibe los miembros por el canal que se pasa como parámetro.
int XC::ElemFriccionBase::recvData(const CommParameters &cp)
  {
    int res= Element0D::recvData(cp);
    std::cerr<< "ElemFriccionBase::recvData incompleta (falta modelo friccion y materiales)" << std::endl;
    res+= cp.receiveDoubles(uy,mass,tol,L,getDbTagData(),CommMetaData(9));
    res+= cp.receiveVector(x,getDbTagData(),CommMetaData(10));
    res+= cp.receiveVector(y,getDbTagData(),CommMetaData(11));
    res+= cp.receiveInt(maxIter,getDbTagData(),CommMetaData(12));
    res+= cp.receiveVector(ub,getDbTagData(),CommMetaData(13));
    res+= cp.receiveVector(qb,getDbTagData(),CommMetaData(14));
    res+= cp.receiveMatrix(kb,getDbTagData(),CommMetaData(15));
    res+= cp.receiveVector(ul,getDbTagData(),CommMetaData(16));
    res+= cp.receiveMatrix(Tgl,getDbTagData(),CommMetaData(17));
    res+= cp.receiveMatrix(Tlb,getDbTagData(),CommMetaData(18));
    return res;
  }
