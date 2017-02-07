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

// $Revision: 1.7 $
// $Date: 2003/04/02 22:02:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/feap/fElement.cpp,v $


// File: ~/element/fortran/fElement.C
//
// Written: fmk
// Created: 07/98
// Revision: A
//
// Description: This file contains the implementation for the XC::fElement class.
//
// What: "@(#) fElement.C, revA"

#include "domain/mesh/element/special/feap/fElement.h"
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <material/uniaxial/UniaxialMaterial.h>

#include <cmath>
#include <cstdlib>

// initialise all class wise pointers to 0 and numfElements to 0
int    XC::fElement::numfElements(0);

static double *work = 0;
static int sizeWork = 0;

const int MAX_NST=64;
std::vector<XC::Matrix *> XC::fElement::fElementM(MAX_NST+1,nullptr);
std::vector<XC::Vector *> XC::fElement::fElementV(MAX_NST+1,nullptr);
std::vector<double> XC::fElement::r(MAX_NST+1);
std::vector<double> XC::fElement::s((MAX_NST+1)*(MAX_NST+1));
std::vector<double> XC::fElement::ul((MAX_NST+1)*6);
std::vector<double> XC::fElement::xl(MAX_NST+1);
std::vector<double> XC::fElement::tl(MAX_NST+1);
std::vector<int> XC::fElement::ix(MAX_NST+1);

// constructor:
//  responsible for allocating the necessary space needed by each object
//  and storing the tags of the XC::fElement end nodes.
XC::fElement::fElement(int tag,
                   int classTag,
                   int EleType,
                   int sizeD, int NEN,
                   int NDM, int NDF,
                   int numNh1, int numNh3)
:Element(tag,classTag), nh1(numNh1), nh3(numNh3), h(0), eleType(EleType),
  theNodes(0), u(0), nen(NEN), ndf(NDF), ndm(NDM), d(0), data(0),
 connectedNodes(0),nrCount(0), Ki(0)

{
    // allocate space for h array
    if(nh1 < 0) nh1 = 0;
    if(nh3 < 0) nh3 = 0;
    if(nh1 != 0 || nh3 != 0) {
        int sizeH = 2*nh1 + nh3;
        h = new double[sizeH];
        if(sizeWork < sizeH) {
          if(work != 0)
            delete [] work;
          work = new double[sizeH];
          if(work == 0) {
            std::cerr << "FATAL: XC::fElement::fElement() - eleTag: " << tag;
            std::cerr << " ran out of memory creating h of size " << 2*nh1+nh3 << std::endl;
            exit(-1);
          }
          sizeWork = sizeH;
        }
        if(h == 0 || work == 0) {
            std::cerr << "FATAL: XC::fElement::fElement() - eleTag: " << tag;
            std::cerr << " ran out of memory creating h of size " << 2*nh1+nh3 << std::endl;
            exit(-1);
        }

        for(int i=0; i<sizeH; i++)
          h[i] = 0.0;
    }

    connectedNodes = new XC::ID(NEN);
    d = new double[sizeD];
    for(int i=0; i<sizeD; i++) d[i] = 0.0;
    data = new XC::Vector(d, sizeD);

    // allocate space for static varaibles on creation of first instance
    if(numfElements == 0)
      {
        fElementM[0] = new XC::Matrix(1,1); // dummy for error
        fElementV[0] = new XC::Vector(1);
      }

    // increment number of elements
    numfElements++;
  }


XC::fElement::fElement(int tag,
                   int classTag,
                   int EleType,
                   int sizeD, int NEN,
                   int NDM, int NDF, int iow)
:Element(tag,classTag), nh1(0), nh3(0), h(0), eleType(EleType),
  theNodes(0), u(0), nen(NEN), ndf(NDF), ndm(NDM), d(0), data(0),
 connectedNodes(0), nrCount(0), Ki(0)
{
    connectedNodes = new XC::ID(NEN);
    d = new double[sizeD];
    data = new XC::Vector(d, sizeD);
    if(d == 0 || data == 0) {
        std::cerr << "FATAL: XC::fElement::fElement() - eleTag: " << tag;
        std::cerr << " ran out of memory creating d of size " << sizeD << std::endl;
        exit(-1);
    }
    for(int i=0; i<sizeD; i++) d[i] = 0.0;

    // invoke the elmt() routine with isw == 1 to read in the element data
    // and create room for the h array stuff needed by the element
    this->invokefInit(1, iow);

    // allocate space for h array
    if(nh1 < 0) nh1 = 0;
    if(nh3 < 0) nh3 = 0;
    if(nh1 != 0 || nh3 != 0) {
        int sizeH = 2*nh1+nh3;
        h = new double[sizeH];
        if(sizeWork < sizeH) {
          if(work != 0)
            delete [] work;
          work = new double[sizeH];
          if(work == 0) {
            std::cerr << "FATAL: XC::fElement::fElement() - eleTag: " << tag;
            std::cerr << " ran out of memory creating h of size " << 2*nh1+nh3 << std::endl;
            exit(-1);
          }
          sizeWork = sizeH;
        }
        if(h == 0 || work == 0) {
            std::cerr << "FATAL: XC::fElement::fElement() - eleTag: " << this->getTag();
            std::cerr << " ran out of memory creating h of size " << sizeH << std::endl;
            exit(-1);
        }
        else
            for(int i=0; i<sizeH; i++) h[i] = 0.0;
    }

    // allocate space for static varaibles on creation of first instance
    if(numfElements == 0)
      {
        fElementM[0] = new XC::Matrix(1,1); // dummy for error
        fElementV[0] = new XC::Vector(1);
      }

    // increment number of elements
    numfElements++;
  }

// constructor:
//   invoked by a FEM_ObjectBroker - blank object that recvSelf needs
//   to be invoked upon
XC::fElement::fElement(int classTag)
:Element(0, classTag), nh1(0), nh3(0), h(0),
 theNodes(0), u(0), nen(0), ndf(0), ndm(0), d(0), data(0), connectedNodes(0),
 Ki(0)
{
    // does nothing
}

//  destructor
//     delete must be invoked on any objects created by the object
//     and on the matertial object.
XC::fElement::~fElement()
  {
    // clear up any space allocated for individual object

    if(h != 0)
        delete [] h;
    if(u != 0)
        delete [] u;
    if(theNodes != 0)
        delete [] theNodes;

    if(data != 0)
        delete  data;
    if(connectedNodes != 0)
        delete  connectedNodes;
    if(d != 0)
        delete [] d;

    if(Ki != 0)
      delete Ki;

    // if last element - clear up space allocated

    numfElements --;
    if(numfElements == 0)
      {
        for(int i=0; i<MAX_NST+1; i++)
          {
            if(fElementM[i] != 0) delete fElementM[i];
            if(fElementV[i] != 0) delete fElementV[i];
          }
      }
  }

int XC::fElement::getNumExternalNodes(void) const
  {  return connectedNodes->Size(); }

const XC::ID &XC::fElement::getExternalNodes(void) const
  { return *connectedNodes; }

int XC::fElement::getNumDOF(void) const
  { return ndf*nen; }

void XC::fElement::setDomain(Domain *theDomain)
  {
    // check XC::Domain is not null - invoked when object removed from a domain
    if(theDomain == 0) {
        ndm = 0;
        nen = 0;
        ndf = 0;
        if(theNodes)
          {
            delete [] theNodes;
            theNodes= nullptr;
          }
        return;
    }

    // set up the pointers to the nodes
    const XC::ID &theNodeTags = this->getExternalNodes();

    int numNodes = theNodeTags.Size();
    theNodes = new XC::Node *[numNodes];
    for(int i=0; i<numNodes; i++) {
        Node *theNode = theDomain->getNode(theNodeTags(i));
        if(theNode == 0) {
            std::cerr << "WARNING XC::fElement::setDomain(Domain *theDomain) - node: ";
            std::cerr << theNodeTags(i) << " does not exist in domain for ele " << *this;
            ndm = 0; nen = 0; ndf = 0;
            return;
        }
        // set up the pointer to the node
        theNodes[i] = theNode;


        // check the dimension and number of dof at the node
        // is the same as all the other nodes of the element
        if(i == 0) {
            const XC::Vector &crds = theNode->getCrds();
            ndm = crds.Size();                      // ndm = dimesion of mesh
            ndf = theNode->getNumberDOF();    // ndf = number of dof at nodes
        } else {
            const XC::Vector &crds = theNode->getCrds();
            if(ndm != crds.Size()) {
                std::cerr << "WARNING XC::fElement::setDomain(Domain *theDomain) - node: ";
                std::cerr << theNodeTags(i) << " not in correct dimension " << *this;
                ndm = 0; nen = 0; ndf = 0;
                return;
            }
            if(ndf != theNode->getNumberDOF()) {
                std::cerr << "WARNING XC::fElement::setDomain(Domain *theDomain) - node: ";
                std::cerr << theNodeTags(i) << " does not have correct #DOF " << *this;
                ndm = 0; nen = 0; ndf = 0;
                return;
            }
        }
    }


    // call the XC::DomainComponent class method THIS IS VERY IMPORTANT
    this->XC::DomainComponent::setDomain(theDomain);

    // set nen - the number of element nodes
    nen = numNodes;

    // allocate memory for u
    int nst = ndf*nen;
    u = new double[nst];
    if(u == 0) {
        std::cerr << "WARNING XC::fElement::setDomain(Domain *theDomain) -  ";
        std::cerr << " ran out of memory creating u of size: " << nen*ndf << *this;
        ndm = 0; nen = 0; ndf = 0;
        return;
    }
    // zero u
    for(int ii=0; ii<nst; ii++)
        u[ii] = 0.0;


    load.reset(nst);

    // allocate the XC::Matrix and XC::Vector objects if none yet for this size nst
   if(fElementM[nst] == 0)
     {
       fElementM[nst] = new XC::Matrix(s,nst,nst);
       fElementV[nst] = new XC::Vector(r,nst);

       if((fElementM[nst] == 0) || (fElementV[nst] == 0))
         {
           std::cerr << "WARNING XC::fElement::setDomain(Domain *theDomain) -  ";
           std::cerr << " ran out of memory creating XC::Matrix and XC::Vector for " << *this;
           ndm = 0; nen = 0; ndf = 0;
           return;
         }
     }
  }


int XC::fElement::commitState()
{
  int retVal = 0;
  // call element commitState to do any base class stuff
  if((retVal = this->XC::Element::commitState()) != 0) {
    std::cerr << "XC::fElement::commitState () - failed in base class";
  }

  if(nh1 != 0)
    for(int i=0; i<nh1; i++)
      h[i] = h[i+nh1];

  nrCount = 0;
  return retVal;
}

int
XC::fElement::revertToLastCommit()
{
    if(nh1 != 0)
        for(int i=0; i<nh1; i++)
            h[i+nh1] = h[i];

    nrCount = 0;
    return 0;
}

int XC::fElement::revertToStart()
  {
    // does nothing
    return 0;
  }


const XC::Matrix &XC::fElement::getTangentStiff(void) const
  {
    static Matrix K;
    // check for XC::quick return
    if(nen == 0)
      K= (*fElementM[0]);
    else
      {

        // get the current load factor
        Domain *theDomain=this->getDomain();
        const double dm = theDomain->getTimeTracker().getCurrentTime();

        // set ctan, ior and iow
        double ctan[3];
        ctan[0] = 1.0; ctan[1] = 0.0; ctan[2] = 0.0;
        int ior = 0; int iow = 0;

        // call the ready routine to set ul, xl, tl and ix, NH1, NH2 and NH3
        int nstR = this->readyfRoutine(false);

        // zero the matrix
        fElementM[nstR]->Zero();

        // invoke the fortran subroutine
        int isw = 3;
        int nstI = this->invokefRoutine(ior, iow, ctan, isw);

        // check nst is as determined in readyfRoutine()
        if(nstI != nstR)
          {
            std::cerr << "FATAL XC::fElement::getTangentStiff() problems with incompatable nst";
            std::cerr << " ready: " << nstR << " invoke: " << nstI << std::endl;
            exit(-1);
          }
        // return the matrix
        K= *(fElementM[nstR]);
      }
    if(isDead())
      K*=dead_srf;
    return K;
  }


const XC::Matrix &XC::fElement::getDamp(void) const
  {
    // check for XC::quick return
    if(nen == 0)
      return (*fElementM[0]);

    // get the current load factor
    Domain *theDomain=this->getDomain();
    const double dm= theDomain->getTimeTracker().getCurrentTime();

    // set ctan, ior and iow
    double ctan[3];
    ctan[0] = 0.0; ctan[1] = 1.0; ctan[2] = 0.0;
    int ior = 0; int iow = 0;

    // call the ready routine to set ul, xl, tl and ix, NH1, NH2 and NH3
    int NH1, NH2, NH3;
    int nstR = this->readyfRoutine(true);

    // zero the matrix
    fElementM[nstR]->Zero();

    // invoke the fortran subroutine
    int isw = 3; int nst = nen*ndf; int n = this->getTag();


    int nstI = this->invokefRoutine(ior, iow, ctan, isw);

    // check nst is as determined in readyfRoutine()
    if(nstI != nstR) {
        std::cerr << "FATAL XC::fElement::getTangentStiff() problems with incompatable nst";
        std::cerr << " ready: " << nstR << " invoke: " << nstI << std::endl;
        exit(-1);
    }

    // return the matrix
    return *(fElementM[nstR]);
  }


const XC::Matrix &XC::fElement::getMass(void) const
  {
    // check for XC::quick return
    if(nen == 0)
        return (*fElementM[0]);

    // get the current load factor
    Domain *theDomain=this->getDomain();
    const double dm= theDomain->getTimeTracker().getCurrentTime();

    // set ctan, ior and iow
    double ctan[3];
    ctan[0] = 0.0; ctan[1] = 0.0; ctan[2] = 1.0;
    int ior = 0; int iow = 0;

    // call the ready routine to set ul, xl, tl and ix, NH1, NH2 and NH3
    int NH1, NH2, NH3;
    int nstR = this->readyfRoutine(true);

    // zero the matrix and vector (consistant and lumped)
    fElementM[nstR]->Zero();
    fElementV[nstR]->Zero();

    // invoke the fortran subroutine
    int isw = 5; int nst = nen*ndf; int n = this->getTag();
    int nstI = this->invokefRoutine(ior, iow, ctan, isw);

    // check nst is as determined in readyfRoutine()
    if(nstI != nstR)
      {
        std::cerr << "FATAL XC::fElement::getTangentStiff() problems with incompatable nst";
        std::cerr << " ready: " << nstR << " invoke: " << nstI << std::endl;
        exit(-1);
      }
    // return the matrix
    return *(fElementM[nstR]);
  }



int XC::fElement::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr <<"XC::fElement::addLoad - load type unknown for truss with tag: " << this->getTag() << std::endl;
    return -1;
  }


int XC::fElement::addInertiaLoadToUnbalance(const Vector &accel)
  {
    const Matrix &mass = this->getMass();
    int nstR = nen*ndf;
    Vector &resid = *(fElementV[nstR]);
  static const int numberNodes = 4 ;

  // store computed RV fro nodes in resid vector
  int count = 0;
  for(int i=0; i<nen; i++) {
    const XC::Vector &Raccel = theNodes[i]->getRV(accel);
    for(int j=0; j<ndf; j++)
      resid(count++) = Raccel(i);
  }

  // create the load vector if one does not exist
  if(load.Nulo())
    load.reset(nstR);

  // add -M * RV(accel) to the load vector
  load.addMatrixVector(1.0, mass, resid, -1.0);


  return 0;
}


const XC::Vector &XC::fElement::getResistingForce(void) const
  {
    // check for XC::quick return
    if(nen == 0)
      return (*fElementV[0]);

    // get the current load factor
    Domain *theDomain=this->getDomain();
    const double dm= theDomain->getTimeTracker().getCurrentTime();

    // set ctan, ior and iow
    double ctan[3];
    ctan[0] = 0.0; ctan[1] = 0.0; ctan[2] = 0.0;
    int ior = 0; int iow = 0;

    // call the ready routine to set ul, xl, tl and ix, NH1, NH2 and NH3
    int NH1, NH2, NH3;
    int nstR = this->readyfRoutine(false);

    // zero the vector
    fElementV[nstR]->Zero();

    // invoke the fortran subroutine
    int isw = 6; int nst = nen*ndf; int n = this->getTag();
    int nstI = this->invokefRoutine(ior, iow, ctan, isw);

    // check nst is as determined in readyfRoutine()
    if(nstI != nstR) {
        std::cerr << "FATAL XC::fElement::getTangentStiff() problems with incompatable nst";
        std::cerr << " ready: " << nstR << " invoke: " << nstI << std::endl;
        exit(-1);
    }

    // negate the sign of the loads -- feap elements return -ku
    (*fElementV[nstR]) *= -1.0;

    // add the applied loads from other sources
    (*fElementV[nstR])-= load;

    // return the matrix
    if(isDead())
      std::cerr << "No está implementada la respuesta con el elemento desactivado." << std::endl;
    return *(fElementV[nstR]);
  }


const XC::Vector &XC::fElement::getResistingForceIncInertia(void) const
  {
    // check for XC::quick return
    if(nen == 0)
        return (*fElementV[0]);

    // get the current load factor
    Domain *theDomain=this->getDomain();
    const double dm = theDomain->getTimeTracker().getCurrentTime();

    // set ctan, ior and iow
    double ctan[3];
    ctan[0] = 0.0; ctan[1] = 0.0; ctan[2] = 0.0;
    int ior = 0; int iow = 0;

    // call the ready routine to set ul, xl, tl and ix, NH1, NH2 and NH3
    int NH1, NH2, NH3;
    int nstR = this->readyfRoutine(true);

    // zero the vector
    fElementV[nstR]->Zero();

    // invoke the fortran subroutine
    int isw = 6; int nst = nen*ndf; int n = this->getTag();
    int nstI = this->invokefRoutine(ior, iow, ctan, isw);

    // check nst is as determined in readyfRoutine()
    if(nstI != nstR)
      {
        std::cerr << "FATAL XC::fElement::getTangentStiff() problems with incompatable nst";
        std::cerr << " ready: " << nstR << " invoke: " << nstI << std::endl;
        exit(-1);
      }

    // negate the sign of the loads -- feap elements return -ku
    (*fElementV[nstR]) *= -1.0;

    // return the matrix
    if(isDead())
      std::cerr << "No está implementada la respuesta con el elemento desactivado." << std::endl;
    return *(fElementV[nstR]);
  }


int XC::fElement::sendSelf(CommParameters &cp)
  {
    std::cerr << "XC::fElement::sendSelf() - not yet implemented\n";
    return -1;
  }

int XC::fElement::recvSelf(const CommParameters &cp)
  {
    std::cerr << "XC::fElement::recvSelf() - not yet implemented\n";
    return -1;
  }


void XC::fElement::Print(std::ostream &s, int flag)
  {
    int ior = 0; int iow = 1;
    ior = -1;

    s << "XC::fElement::Print() - can only print to cerr at present\n";
    ior = -1;

    // get the current load factor
    Domain *theDomain=this->getDomain();
    double dm = theDomain->getTimeTracker().getCurrentTime();

    // set ctan, ior and iow
    double ctan[3];
    ctan[0] = 0.0; ctan[1] = 0.0; ctan[2] = 0.0;


    // call the ready routine to set ul, xl, tl and ix, NH1, NH2 and NH3
    int NH1, NH2, NH3;
    int nstR = this->readyfRoutine(false);

    // invoke the fortran subroutine
    int isw = 4; int nst = nen*ndf; int n = this->getTag();
    int nstI = this->invokefRoutine(ior, iow, ctan, isw);
  }

#ifdef _WIN32

extern "C" int GETCOMMON(int *mynh1, int *mynh3, int *sizeH,
                                  double *myh);


extern "C" int FILLCOMMON(int *mynen, double *mydm, int *myn,
                                   int *myior, int *myiow, int *mynh1,
                                   int *mynh2, int *mynh3, int *sumnh,
                                   double *myh, double *myctan,
                                   int *nrCount);

extern "C" int ELMT01(double *d, double *ul, double *xl, int *ix,
                               double *tl, double *s, double *r, int *ndf,
                               int *ndm, int *nst, int *isw);

extern "C" int ELMT02(double *d, double *ul, double *xl, int *ix,
                               double *tl, double *s, double *r, int *ndf,
                               int *ndm, int *nst, int *isw);

extern "C" int ELMT03(double *d, double *ul, double *xl, int *ix,
                               double *tl, double *s, double *r, int *ndf,
                               int *ndm, int *nst, int *isw);

extern "C" int ELMT04(double *d, double *ul, double *xl, int *ix,
                               double *tl, double *s, double *r, int *ndf,
                               int *ndm, int *nst, int *isw);

extern "C" int ELMT05(double *d, double *ul, double *xl, int *ix,
                               double *tl, double *s, double *r, int *ndf,
                               int *ndm, int *nst, int *isw);

#define getcommon_         GETCOMMON
#define fillcommon_        FILLCOMMON
#define elmt01_                ELMT01
#define elmt02_                ELMT02
#define elmt03_                ELMT03
#define elmt04_                ELMT03
#define elmt05_                ELMT05

#else
extern "C" int getcommon_(int *mynh1, int *mynh3, int *sizeH, double *myh);


extern "C" int fillcommon_(int *mynen, double *mydm, int *myn, int *myior,
                           int *myiow, int *mynh1, int *mynh2, int *mynh3,
                           int *sumnh, double *myh, double *myctan,
                           int *nrCount);

extern "C" int elmt01_(double *d, double *ul, double *xl, int *ix, double *tl,
                       double *s, double *r, int *ndf, int *ndm, int *nst,
                       int *isw);

extern "C" int elmt02_(double *d, double *ul, double *xl, int *ix, double *tl,
                       double *s, double *r, int *ndf, int *ndm, int *nst,
                       int *isw);

extern "C" int elmt03_(double *d, double *ul, double *xl, int *ix, double *tl,
                       double *s, double *r, int *ndf, int *ndm, int *nst,
                       int *isw);

extern "C" int elmt04_(double *d, double *ul, double *xl, int *ix, double *tl,
                       double *s, double *r, int *ndf, int *ndm, int *nst,
                       int *isw);

extern "C" int elmt05_(double *d, double *ul, double *xl, int *ix, double *tl,
                       double *s, double *r, int *ndf, int *ndm, int *nst,
                       int *isw);
#endif

int
XC::fElement::invokefRoutine(int ior, int iow, double *ctan, int isw)
{
    // fill the common blocks
    // determine position in h of nh1, nh2 and nh3 - remember Fortarn indexing
    int NH1, NH2, NH3;
    if(nh1 != 0) {
        NH1 = 1;
        NH2 = nh1 + NH1;
        NH3 = nh1 + NH2;
    } else {
        NH1 = 1;
        NH2 = 1;
        NH3 = 1;
    }

    int NDM = ndm;
    int NDF = ndf;

    int n = this->getTag();
    int sum = 2*nh1 + nh3;
    int count = nrCount;


    double dm = 0.0; // load factor

    fillcommon_(&nen, &dm, &n, &ior, &iow, &NH1, &NH2, &NH3, &sum,
                h, ctan, &count);

    // invoke the fortran subroutine

    int nst = nen*ndf;
    if(nst != 0) {
        if(eleType == 1)
            elmt01_(d,ul,xl,ix,tl,s,r,&NDF,&NDM,&nst,&isw);
        else if(eleType == 2)
            elmt02_(d,ul,xl,ix,tl,s,r,&NDF,&NDM,&nst,&isw);
        else if(eleType == 3)
            elmt03_(d,ul,xl,ix,tl,s,r,&NDF,&NDM,&nst,&isw);
        else if(eleType == 4)
            elmt04_(d,ul,xl,ix,tl,s,r,&NDF,&NDM,&nst,&isw);
        else if(eleType == 5)
            elmt05_(d,ul,xl,ix,tl,s,r,&ndf,&NDM,&nst,&isw);
        else {
            std::cerr << "XC::fElement::invokefRoutine() unknown element type ";
            std::cerr << eleType << std::endl;
        }

        // now copy the stuff from common block to h array
        getcommon_(&NH1,&NH3,&sum,h);
    }

    return nst;
}


int
XC::fElement::invokefInit(int isw, int iow)
{
    // fill the common blocks
    // determine position in h of nh1, nh2 and nh3 - remember Fortarn indexing
    int NH1 =0;
    int NH2 =0;
    int NH3 =0;

    int NDM = ndm;
    int NDF = ndf;
    double ctan[3];

    int n = this->getTag();
    int sum = 0;
    int ior = 0;
    int count = nrCount;

    double dm = 0.0;

    fillcommon_(&nen, &dm, &n, &ior, &iow, &NH1, &NH2, &NH3, &sum,
                h, ctan, &count);

    // invoke the fortran subroutine

    int nst = nen*ndf;
    if(nst != 0) {
        if(eleType == 1)
            elmt01_(d,ul,xl,ix,tl,s,r,&NDF,&NDM,&nst,&isw);
        else if(eleType == 2)
            elmt02_(d,ul,xl,ix,tl,s,r,&NDF,&NDM,&nst,&isw);
        else if(eleType == 3)
            elmt03_(d,ul,xl,ix,tl,s,r,&NDF,&NDM,&nst,&isw);
        else if(eleType == 4)
            elmt04_(d,ul,xl,ix,tl,s,r,&NDF,&NDM,&nst,&isw);
        else if(eleType == 5)
            elmt05_(d,ul,xl,ix,tl,s,r,&NDF,&NDM,&nst,&isw);
        else {
            std::cerr << "XC::fElement::invokefRoutine() unknown element type ";
            std::cerr << eleType << std::endl;
        }

        if(nst < 0) {
            std::cerr << "FATAL: XC::fElement::fElement() - eleTag: " << this->getTag();
            std::cerr << " ran out of memory creating h of size " << nst << std::endl;
            exit(-1);
        }
    }

    // now get the size of the state info needed by the element
    sum = 0;
    getcommon_(&NH1,&NH3,&sum,h);
    nh1 = NH1; nh3=NH3;
        return 0;
}

int
XC::fElement::readyfRoutine(bool incInertia)
{
    // determine nst
    int nst = ndf*nen;

    // loop over nodes - fill in xl, ul, ix as we go
    int posUl = 0;
    int posXl = 0;
    for(int j=0; j<nen; j++) {
        Node *theNode = theNodes[j];

        // add the node tag to ix
        ix[j] = theNode->getTag();

        // add displacement, velocity, accel and  increments to ul
        // Note: we get nodal vel and accel only if inertia is true, this
        // will save memory in static analysis -- look at XC::Node implementation
        const XC::Vector &trialDisp = theNode->getTrialDisp();
        const XC::Vector &commitDisp = theNode->getDisp();
        const XC::Vector &crd = theNode->getCrds();

        // add the coordinates to xl
        int crdSize = crd.Size();

        for(int i=0; i<crdSize; i++) {
            xl[posXl] = crd(i);
            posXl++;
        }

        if(incInertia == true) {
            const XC::Vector &trialVel = theNode->getTrialVel();
            const XC::Vector &trialAccel = theNode->getTrialAccel();
            const XC::Vector &commitVel = theNode->getVel();
            for(int i=0; i<trialDisp.Size(); i++) {
                double trialDispI = trialDisp(i);
                ul[posUl] = trialDispI;
                ul[posUl+nst] = trialDispI - commitDisp(i);
                ul[posUl+2*nst] = trialDispI - u[posUl];
                ul[posUl+3*nst] = trialVel(i);
                ul[posUl+4*nst] = trialAccel(i);
                ul[posUl+5*nst] = commitVel(i);
                u[posUl] = trialDispI; // u(k-1) on next call
                posUl++;
            }
        } else {
            for(int i=0; i<trialDisp.Size(); i++) {
                double trialDispI = trialDisp(i);
                ul[posUl] = trialDispI;
                ul[posUl+nst] = trialDispI - commitDisp(i);
                ul[posUl+2*nst] = trialDispI - u[posUl];
                ul[posUl+3*nst] = 0.0;
                ul[posUl+4*nst] = 0.0;
                ul[posUl+5*nst] = 0.0;
                u[posUl] = trialDispI;
                posUl++;
            }
        }
    }

    // check we have a matrix and vector created for an object of this size
    if(fElementM[nst] == 0) {
        fElementM[nst] = new XC::Matrix(s,nst,nst);
            fElementV[nst] = new XC::Vector(r,nst);

        if(fElementM[nst] == 0 || fElementV[nst] == 0) {
            std::cerr << "FATAL XC::fElement::getTangentStiff() nst: " << nst;
            std::cerr << "ran out of memory\n";
            exit(-1);
        }
    }
    return nst;
}



int XC::fElement::update(void)
  {
    // determine nst
    int nst = ndf*nen;
    // increment the newton-raphson count -- needed for Prof. Fillipou's element
    nrCount++;
    return 0;
  }


const XC::Matrix &XC::fElement::getInitialStiff(void)
  {
    if(!Ki)
      Ki = new Matrix(this->getTangentStiff());

    if(!Ki)
      {
        std::cerr << "FATAL XC::fElement::getInitialStiff() -";
        std::cerr << "ran out of memory\n";
       exit(-1);
     }
    return *Ki;
  }




