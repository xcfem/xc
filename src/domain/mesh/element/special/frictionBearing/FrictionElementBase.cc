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
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/FlatSliderSimple2d.cpp,v $

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

void XC::FrictionElementBase::free_friction_model(void)
  {
    if(theFrnMdl)
      delete theFrnMdl;
    theFrnMdl= nullptr;
  }

void XC::FrictionElementBase::alloc_friction_model(const FrictionModel &thefrnmdl)
  {
    free_friction_model();
    theFrnMdl = thefrnmdl.getCopy();
    if(!theFrnMdl)
      {
	std::cerr << "FrictionElementBase::FrictionElementBase() - "
		  << "failed to get copy of the friction model.\n";
	exit(-1);
      }
  }

XC::FrictionElementBase::FrictionElementBase(int tag, int classTag,int Nd1, int Nd2,const size_t &dim,
					const FrictionModel &thefrnmdl, const UniaxialMatPhysicalProperties &physProp, const double &_uy,
				       const Vector &_y, const Vector &_x, const double &m,const int &maxiter, const double &_tol)
  : Element0D(tag, classTag,Nd1,Nd2,2), theFrnMdl(nullptr), physicalProperties(physProp),
    uy(_uy), x(_x), y(_y), mass(m), maxIter(maxiter), tol(_tol),
    L(0.0), ub(dim), qb(dim), kb(dim,dim), ul(dim*2),
    Tgl(dim*2,dim*2), Tlb(dim,dim*2), kbInit(dim,dim)
  {
    alloc_friction_model(thefrnmdl);
  }


XC::FrictionElementBase::FrictionElementBase(int classTag,const size_t &dim)
  : Element0D(0,classTag,0,0,0), theFrnMdl(nullptr), physicalProperties(0,nullptr),
    uy(0.0), x(), y(), mass(0.0), maxIter(20), tol(1E-8),
    L(0.0), ub(dim), qb(dim), kb(dim,dim), ul(dim*2),
    Tgl(dim*2,dim*2), Tlb(dim,dim*2), kbInit(dim,dim)
  {}

XC::FrictionElementBase::FrictionElementBase(const FrictionElementBase &other)
  :Element0D(other), physicalProperties(other.physicalProperties), uy(other.uy), x(other.x), y(other.y), mass(other.mass), maxIter(other.maxIter), tol(other.tol),
    L(other.L), ub(other.ub), qb(other.qb), kb(other.kb), ul(other.ul),
    Tgl(other.Tgl), Tlb(other.Tlb), kbInit(other.kbInit)
  {
    if(other.theFrnMdl)
      alloc_friction_model(*other.theFrnMdl);
  }

XC::FrictionElementBase &XC::FrictionElementBase::operator=(const FrictionElementBase &other)
  {
    Element0D::operator=(other);
    physicalProperties= other.physicalProperties;
    uy= other.uy;
    x= other.x;
    y= other.y;
    mass= other.mass;
    maxIter= other.maxIter;
    tol= other.tol;
    L= other.L;
    ub= other.ub;
    qb= other.qb;
    kb= other.kb;
    ul= other.ul;
    Tgl= other.Tgl;
    Tlb= other.Tlb;
    kbInit= other.kbInit;
    if(other.theFrnMdl)
      alloc_friction_model(*other.theFrnMdl);
    return *this;
  }
XC::FrictionElementBase::~FrictionElementBase()
  {
    free_friction_model();
  }


//! @brief Send members through the communicator argument.
int XC::FrictionElementBase::sendData(Communicator &comm)
  {
    int res= Element0D::sendData(comm);
    std::cerr<< "FrictionElementBase::sendData incomplete (materials and friction model undefined)." << std::endl;
    res+= comm.sendDoubles(uy,mass,tol,L,getDbTagData(),CommMetaData(9));
    res+= comm.sendVector(x,getDbTagData(),CommMetaData(10));
    res+= comm.sendVector(y,getDbTagData(),CommMetaData(11));
    res+= comm.sendInt(maxIter,getDbTagData(),CommMetaData(12));
    res+= comm.sendVector(ub,getDbTagData(),CommMetaData(13));
    res+= comm.sendVector(qb,getDbTagData(),CommMetaData(14));
    res+= comm.sendMatrix(kb,getDbTagData(),CommMetaData(15));
    res+= comm.sendVector(ul,getDbTagData(),CommMetaData(16));
    res+= comm.sendMatrix(Tgl,getDbTagData(),CommMetaData(17));
    res+= comm.sendMatrix(Tlb,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::FrictionElementBase::recvData(const Communicator &comm)
  {
    int res= Element0D::recvData(comm);
    std::cerr<< "FrictionElementBase::recvData incomplete (materials and friction model undefined)." << std::endl;
    res+= comm.receiveDoubles(uy,mass,tol,L,getDbTagData(),CommMetaData(9));
    res+= comm.receiveVector(x,getDbTagData(),CommMetaData(10));
    res+= comm.receiveVector(y,getDbTagData(),CommMetaData(11));
    res+= comm.receiveInt(maxIter,getDbTagData(),CommMetaData(12));
    res+= comm.receiveVector(ub,getDbTagData(),CommMetaData(13));
    res+= comm.receiveVector(qb,getDbTagData(),CommMetaData(14));
    res+= comm.receiveMatrix(kb,getDbTagData(),CommMetaData(15));
    res+= comm.receiveVector(ul,getDbTagData(),CommMetaData(16));
    res+= comm.receiveMatrix(Tgl,getDbTagData(),CommMetaData(17));
    res+= comm.receiveMatrix(Tlb,getDbTagData(),CommMetaData(18));
    return res;
  }
