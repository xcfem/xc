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
//ShellMITC4Base.cpp

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "ShellMITC4Base.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableVector.h"
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "preprocessor/multi_block_topology/aux_meshing.h"
#include <domain/mesh/node/Node.h>
#include <material/section/SectionForceDeformation.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/element/plane/shell/R3vectors.h>


#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/actor/MovableMatrix.h"
#include "utility/actor/actor/MovableVectors.h"


#include "xc_utils/src/geom/d2/Poligono3d.h"
#include "domain/load/plane/ShellUniformLoad.h"
#include "domain/load/plane/ShellStrainLoad.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/prep_handlers/LoadHandler.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"


//static data
XC::Matrix XC::ShellMITC4Base::stiff(24,24);
XC::Vector XC::ShellMITC4Base::resid(24);
XC::Matrix XC::ShellMITC4Base::mass(24,24);

XC::ShellBData XC::ShellMITC4Base::BData;

void XC::ShellMITC4Base::free_mem(void)
  {
    if(theCoordTransf)
      {
        delete theCoordTransf;
        theCoordTransf= nullptr;
      }
  }

void XC::ShellMITC4Base::alloc(const ShellCrdTransf3dBase *crdTransf)
  {
    free_mem();
    if(crdTransf)
      theCoordTransf= crdTransf->getCopy();
  }

//! @brief Constructor
XC::ShellMITC4Base::ShellMITC4Base(int classTag, const ShellCrdTransf3dBase *crdTransf)
  : QuadBase4N<SectionFDPhysicalProperties>(0,classTag,SectionFDPhysicalProperties(4,nullptr)), Ktt(0.0),theCoordTransf(nullptr),
    p0(), inicDisp(4,Vector()) 
  { alloc(crdTransf); }

//! @brief Constructor
XC::ShellMITC4Base::ShellMITC4Base(int tag, int classTag,const SectionForceDeformation *ptr_mat, const ShellCrdTransf3dBase *crdTransf)
  : QuadBase4N<SectionFDPhysicalProperties>(tag,classTag,SectionFDPhysicalProperties(4,ptr_mat)), Ktt(0.0), theCoordTransf(nullptr),
    p0(), inicDisp(4,Vector()) 
  { alloc(crdTransf); }

//! @brief Constructor
XC::ShellMITC4Base::ShellMITC4Base(int tag, int classTag,int node1,int node2,int node3,int node4,const SectionFDPhysicalProperties &physProp, const ShellCrdTransf3dBase *crdTransf)
  : QuadBase4N<SectionFDPhysicalProperties>(tag,classTag,physProp), theCoordTransf(nullptr),
    p0(), inicDisp(4,Vector())
  {
    theNodes.set_id_nodes(node1,node2,node3,node4);
    alloc(crdTransf);
    theCoordTransf->initialize(theNodes);
  }

//! @brief Copy constructor.
XC::ShellMITC4Base::ShellMITC4Base(const ShellMITC4Base &other)
  : QuadBase4N<SectionFDPhysicalProperties>(other), theCoordTransf(nullptr),
    p0(other.p0), inicDisp(other.inicDisp)
  { alloc(other.theCoordTransf); }

//! @brief Assignment operator.
XC::ShellMITC4Base &XC::ShellMITC4Base::operator=(const ShellMITC4Base &other)
  {
    QuadBase4N<SectionFDPhysicalProperties>::operator=(other);
    alloc(other.theCoordTransf);
    p0= other.p0;
    inicDisp= other.inicDisp;
    return *this;
  }

//! @brief Destructor.
XC::ShellMITC4Base::~ShellMITC4Base(void)
  { free_mem(); }

//! @brief Defines a load over the element from a vector in local coordinates.
const XC::ShellUniformLoad *XC::ShellMITC4Base::vector3dUniformLoadLocal(const Vector &v)
  {
    ShellUniformLoad *retval= nullptr;
    Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        MapLoadPatterns &casos= preprocessor->getLoadHandler().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Load for this element.
        const int &loadTag= casos.getCurrentElementLoadTag(); //Load identifier.

        const size_t sz= v.Size();
        if(sz>2)
          {
            LoadPattern *lp= casos.getCurrentLoadPatternPtr();
            if(lp)
              {
                retval= new ShellUniformLoad(loadTag,v,eTags);
                lp->addElementalLoad(retval);
                casos.setCurrentElementLoadTag(loadTag+1);
              }
            else
	      std::cerr << "ShellMITC4Base::vector3dUniformLoadLocal; "
                        << " there is no current load pattern."
                        << " Load ignored." << std::endl; 
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
                    << "; a vector of dimension 3 was expected." << std::endl;
      }
    else
      std::cerr << "ShellMITC4Base::vector3dUniformLoadLocal; modeler not defined." << std::endl;
    return retval;
  }

//! @brief Defines a load over the element from a vector in global coordinates.  
const XC::ShellUniformLoad *XC::ShellMITC4Base::vector3dUniformLoadGlobal(const Vector &v)
  {
    const ShellUniformLoad *retval= nullptr;
    const size_t sz= v.Size();
    if(sz>2)
      {
        assert(theCoordTransf);
        const Vector vTrf= theCoordTransf->getVectorLocalCoordFromGlobal(v);
        retval= vector3dUniformLoadLocal(vTrf);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; a vector of dimension 3 was expected." << std::endl;
    return retval;
  }

//! @brief  
void XC::ShellMITC4Base::strainLoad(const Matrix &strains)
  {
    Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        MapLoadPatterns &casos= preprocessor->getLoadHandler().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Load for this element.
        const int &loadTag= casos.getCurrentElementLoadTag(); //Load identifier.
        LoadPattern *lp= casos.getCurrentLoadPatternPtr();
        if(lp)
          {
            ShellStrainLoad *tmp= new ShellStrainLoad(loadTag,eTags);
            tmp->setStrains(strains);
            if(tmp)
              {
                tmp->set_owner(this);
                lp->addElementalLoad(tmp);
              }
            else
              std::cerr << "ShellMITC4Base::strainLoad; can't create load." << std::endl;
          }
        else
	  std::cerr << "ShellMITC4Base::vector3dUniformLoadLocal; "
                    << " there is no current load pattern."
                    << " Load ignored." << std::endl; 
      }
    else
      std::cerr << "Modeler not defined." << std::endl;
  }


//! @brief set domain
void XC::ShellMITC4Base::setDomain(Domain *theDomain)
  {
    QuadBase4N<SectionFDPhysicalProperties>::setDomain(theDomain);

    static Vector eig(3);
    static Matrix ddMembrane(3,3);

    //compute drilling stiffness penalty parameter
    const Matrix &dd= physicalProperties[0]->getInitialTangent();

    //assemble ddMembrane;
    for(int i= 0;i<3;i++)
      {
        for(int j= 0;j < 3;j++)
          ddMembrane(i,j)= dd(i,j);
      } //end for i

    //eigenvalues of ddMembrane
    eig= LovelyEig( ddMembrane );

    //set ktt
    //Ktt= dd(2,2);  //shear modulus
    Ktt= std::min( eig(2), std::min( eig(0), eig(1) ) );
    //Ktt= dd(2,2);

    //basis vectors and local coordinates
    computeBasis(); 
    setupInicDisp();
  }

void XC::ShellMITC4Base::setupInicDisp(void)
  {
    capturaInicDisp(); //Sets suitable sizes.
    zeroInicDisp(); //Initializes all.
  }

void XC::ShellMITC4Base::capturaInicDisp(void)
  {
    for(size_t i= 0;i<4;i++)
      inicDisp[i]= theCoordTransf->getBasicTrialDisp(i);
  }

void XC::ShellMITC4Base::zeroInicDisp(void)
  {
    for(std::vector<Vector>::iterator i= inicDisp.begin();i!=inicDisp.end();i++)
      (*i).Zero();
  }

//! @brief return number of dofs
int XC::ShellMITC4Base::getNumDOF(void) const
  { return 24; }

//! @brief Reactivates the element.
void XC::ShellMITC4Base::alive(void)
  {
    if(isDead())
      {
        QuadBase4N<SectionFDPhysicalProperties>::alive();
        Ki.Zero();
        revertToStart(); //Eliminate possible strains and stresses
	                 //on the element (melt and then solidify).
        capturaInicDisp(); //Node displacements at element activation.
      }
    else
      std::cerr << "Trying to activate an element already active."
                << std::endl;
  }

//! @brief print out element data
void XC::ShellMITC4Base::Print(std::ostream &s, int flag )
  {
    if(flag == -1)
      {
        int eleTag= this->getTag();
        s << "EL_QUAD4\t" << eleTag << "\t";
        s << eleTag << "\t" << 1;
        s  << "\t" << theNodes.getTagNode(0) << "\t" << theNodes.getTagNode(1);
        s  << "\t" << theNodes.getTagNode(2) << "\t" << theNodes.getTagNode(3) << "\t0.00";
        s << std::endl;
        s << "PROP_2D\t" << eleTag << "\t";
        s << eleTag << "\t" << 1;
        s  << "\t" << -1 << "\tSHELL\t1.0\0.0";
        s << std::endl;
      }
    else if(flag < -1)
      {
        int counter= (flag + 1) * -1;
        int eleTag= this->getTag();
        int i,j;
        for(i= 0;i < 4;i++)
          {
            const XC::Vector &stress= physicalProperties[i]->getStressResultant();
            s << "STRESS\t" << eleTag << "\t" << counter << "\t" << i << "\tTOP";
            for(j=0; j<6; j++)
              s << "\t" << stress(j);
            s << std::endl;
          }
      }
    else
      {
        s << std::endl;
        s << "MITC4 Bbar Non-Locking Four Node Shell \n";
        s << "Element Number: " << this->getTag() << std::endl;
        s << "Node 1 : " << theNodes.getTagNode(0) << std::endl;
        s << "Node 2 : " << theNodes.getTagNode(1) << std::endl;
        s << "Node 3 : " << theNodes.getTagNode(2) << std::endl;
        s << "Node 4 : " << theNodes.getTagNode(3) << std::endl;

        s << "Material XC::Information : \n ";
        physicalProperties.Print( s, flag );
        s << std::endl;
      }
  }

//! @brief Update state variables.
int XC::ShellMITC4Base::update(void)
  {
    theCoordTransf->update();
    return QuadBase4N<SectionFDPhysicalProperties>::update();
  }

//! @brief return stiffness matrix
const XC::Matrix &XC::ShellMITC4Base::getTangentStiff(void) const
  {
    theCoordTransf->update();

    const int tang_flag= 1; //get the tangent
    formResidAndTangent(tang_flag); //do tangent and residual here
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }

//! @brief Computes the matrix G.
const XC::Matrix XC::ShellMITC4Base::calculateG(void) const
  {
    const double dx34= xl[0][2]-xl[0][3];
    const double dy34= xl[1][2]-xl[1][3];

    const double dx21= xl[0][1]-xl[0][0];
    const double dy21= xl[1][1]-xl[1][0];

    const double dx32= xl[0][2]-xl[0][1];
    const double dy32= xl[1][2]-xl[1][1];

    const double dx41= xl[0][3]-xl[0][0];
    const double dy41= xl[1][3]-xl[1][0];

    Matrix G(4,12);
    G.Zero();
    double one_over_four= 0.25;
    G(0,0)=-0.5;
    G(0,1)=-dy41*one_over_four;
    G(0,2)=dx41*one_over_four;
    G(0,9)=0.5;
    G(0,10)=-dy41*one_over_four;
    G(0,11)=dx41*one_over_four;
    G(1,0)=-0.5;
    G(1,1)=-dy21*one_over_four;
    G(1,2)=dx21*one_over_four;
    G(1,3)=0.5;
    G(1,4)=-dy21*one_over_four;
    G(1,5)=dx21*one_over_four;
    G(2,3)=-0.5;
    G(2,4)=-dy32*one_over_four;
    G(2,5)=dx32*one_over_four;
    G(2,6)=0.5;
    G(2,7)=-dy32*one_over_four;
    G(2,8)=dx32*one_over_four;
    G(3,6)=0.5;
    G(3,7)=-dy34*one_over_four;
    G(3,8)=dx34*one_over_four;
    G(3,9)=-0.5;
    G(3,10)=-dy34*one_over_four;
    G(3,11)=dx34*one_over_four;
    return G;
  }

//! @brief return secant matrix
const XC::Matrix &XC::ShellMITC4Base::getInitialStiff(void) const
  {
    if(!Ki.isEmpty())
      return Ki;

    static const int ndf= 6; //two membrane plus three bending plus one drill
    static const int nstress= 8; //three membrane, three moment, two shear
    static const int ngauss= 4;
    static const int numnodes= 4;

    int i,  j,  k, p, q;
    int jj, kk;

    double volume= 0.0;

    static double xsj;  // determinant of the jacobian matrix 
    static double dvol[ngauss]; //volume element
    static double shp[3][numnodes];  //shape functions at a gauss point

    //  static double Shape[3][numnodes][ngauss]; //all the shape functions

    static Matrix stiffJK(ndf,ndf); //nodeJK stiffness 
    static Matrix dd(nstress,nstress);  //material tangent
    static Matrix J0(2,2);  //Jacobian at center
    static Matrix J0inv(2,2); //inverse of Jacobian at center

    //---------B-matrices------------------------------------
    static Matrix BJ(nstress,ndf);      // B matrix node J
    static Matrix BJtran(ndf,nstress);
    static Matrix BK(nstress,ndf);      // B matrix node k
    static Matrix BJtranD(ndf,nstress);
    static Matrix Bbend(3,3);  // bending B matrix
    static Matrix Bshear(2,3); // shear B matrix
    static Matrix Bmembrane(3,2); // membrane B matrix
    static double BdrillJ[ndf]; //drill B matrix
    static double BdrillK[ndf];  

    double *drillPointer;

    static double saveB[nstress][ndf][numnodes];

    //-------------------------------------------------------

    stiff.Zero( );
 
    Matrix G= calculateG();


    Matrix Ms(2,4);
    Ms.Zero();
    Matrix Bsv(2,12);
    Bsv.Zero();

    const double Ax= -xl[0][0]+xl[0][1]+xl[0][2]-xl[0][3];
    const double Bx=  xl[0][0]-xl[0][1]+xl[0][2]-xl[0][3];
    const double Cx= -xl[0][0]-xl[0][1]+xl[0][2]+xl[0][3];

    const double Ay= -xl[1][0]+xl[1][1]+xl[1][2]-xl[1][3];
    const double By=  xl[1][0]-xl[1][1]+xl[1][2]-xl[1][3];
    const double Cy= -xl[1][0]-xl[1][1]+xl[1][2]+xl[1][3];

    double alph= atan2(Ay,Ax);
    double beta= 3.141592653589793/2-atan2(Cx,Cy);
    Matrix Rot(2,2);
    Rot.Zero();
    Rot(0,0)=sin(beta);
    Rot(0,1)=-sin(alph);
    Rot(1,0)=-cos(beta);
    Rot(1,1)=cos(alph);
    Matrix Bs(2,12);
  
    double r1= 0;
    double r2= 0;
    double r3= 0;

    //gauss loop 
    for(i=0;i<ngauss;i++)
      {
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
        r1= Cx + gp.r_coordinate()*Bx;
        r3= Cy + gp.r_coordinate()*By;
        r1= r1*r1 + r3*r3;
        r1= sqrt (r1);
        r2= Ax + gp.s_coordinate()*Bx;
        r3= Ay + gp.s_coordinate()*By;
        r2= r2*r2 + r3*r3;
        r2= sqrt (r2);

        //get shape functions    
        shape2d( gp.r_coordinate(), gp.s_coordinate(), xl, shp, xsj );
        //volume element to also be saved
        dvol[i]= gp.weight() * xsj;  
        volume += dvol[i];

        Ms(1,0)=1-gp.r_coordinate();
        Ms(0,1)=1-gp.s_coordinate();
        Ms(1,2)=1+gp.r_coordinate();
        Ms(0,3)=1+gp.s_coordinate();
        Bsv= Ms*G;

        for( j= 0; j < 12; j++ )
          {
            Bsv(0,j)=Bsv(0,j)*r1/(8*xsj);
            Bsv(1,j)=Bsv(1,j)*r2/(8*xsj);
          }
        Bs=Rot*Bsv;
    
        // j-node loop to compute strain 
        for( j= 0; j < numnodes; j++ )
          {
            //compute B matrix 
            Bmembrane= computeBmembrane( j, shp );
            Bbend= computeBbend( j, shp );
            for(p= 0; p < 3; p++)
              {
                Bshear(0,p)= Bs(0,j*3+p);
                Bshear(1,p)= Bs(1,j*3+p);
              }//end for p
            BJ= assembleB( Bmembrane, Bbend, Bshear );

	    //save the B-matrix
           for(p=0; p<nstress; p++)
             {
               for(q=0; q<ndf; q++ )
                 saveB[p][q][j]= BJ(p,q);
             }//end for p

           //drilling B matrix
           drillPointer= computeBdrill( j, shp );
           for(p=0; p<ndf; p++ )
             {
               //BdrillJ[p]= *drillPointer++;
               BdrillJ[p]= *drillPointer; //set p-th component
               drillPointer++;             //pointer arithmetic
             }//end for p
          } // end for j

        dd= physicalProperties[i]->getInitialTangent( );
        dd*= dvol[i];

        //tangent calculations node loops

        jj= 0;
        for(j=0;j<numnodes;j++)
          {
            //extract BJ
            for(p=0; p<nstress; p++)
              {
                for(q=0; q<ndf; q++ )
                  BJ(p,q)= saveB[p][q][j];
              }//end for p

            //multiply bending terms by (-1.0) for correct statement
            // of equilibrium  
            for(p=3;p<6;p++)
              {
                for(q=3;q<6;q++)
                  BJ(p,q) *= (-1.0);
              } //end for p

            //transpose 
            //BJtran= transpose( 8, ndf, BJ );
            for(p=0; p<ndf; p++)
              {
                for(q=0; q<nstress; q++) 
                  BJtran(p,q)= BJ(q,p);
              }//end for p

            //drilling B matrix
            drillPointer= computeBdrill( j, shp );
            for(p=0; p<ndf; p++ )
              {
                BdrillJ[p]= *drillPointer;
                drillPointer++;
              }//end for p

            //BJtranD= BJtran * dd;
            BJtranD.addMatrixProduct(0.0, BJtran,dd,1.0 );
      
            for(p=0; p<ndf; p++) 
              BdrillJ[p]*= ( Ktt*dvol[i] );
      
            kk= 0;
            for(k= 0; k < numnodes; k++ )
              {
                //extract BK
                for(p=0; p<nstress; p++)
                  {
                    for(q=0; q<ndf; q++ )
                      BK(p,q)= saveB[p][q][k];
                  }//end for p
       
                //stiffJK= BJtranD * BK;
                // +  transpose( 1,ndf,BdrillJ ) * BdrillK; 
                stiffJK.addMatrixProduct(0.0, BJtranD,BK,1.0 );

                //drilling B matrix
                drillPointer= computeBdrill( k, shp );
                for(p=0; p<ndf; p++ )
                  {
                    BdrillK[p]= *drillPointer;
                    drillPointer++;
                  }//end for p
        
                for(p=0;p<ndf;p++)
                  {
                   for(q=0;q<ndf;q++)
                     {
                       stiff(jj+p,kk+q)+=stiffJK(p,q) + (BdrillJ[p]*BdrillK[q]);
                     }//end for q
                  }//end for p   
                kk += ndf;
              } // end for k loop
          jj += ndf;
        } // end for j loop
 
      } //end for i gauss loop
    theCoordTransf->getGlobalTangent(stiff);
    Ki= stiff;
    return stiff;
  }


//! @brief return mass matrix
const XC::Matrix& XC::ShellMITC4Base::getMass(void) const
  {
    int tangFlag= 1;
    formInertiaTerms( tangFlag );
    if(isDead())
      mass*=dead_srf;
    return mass;
  }

//! @brief Return the element Gauss points.
const XC::GaussModel &XC::ShellMITC4Base::getGaussModel(void) const
  { return gauss_model_quad4; }

//! @brief Zeroes the element load vector.
void XC::ShellMITC4Base::zeroLoad(void)
  {
    QuadBase4N<SectionFDPhysicalProperties>::zeroLoad();
    p0.zero();
  }

//! @brief Applies on the element the load being passed as parameter.
int XC::ShellMITC4Base::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << getClassName() 
                << "; load over inactive element: "
                << getTag() << std::endl;
    else
      {
        const double area= getPolygon().getArea();

        // Accumulate elastic deformations in basic system
        if(ShellMecLoad *shellMecLoad= dynamic_cast<ShellMecLoad *>(theLoad))
          {
            shellMecLoad->addReactionsInBasicSystem(area,loadFactor,p0); // Accumulate reactions in basic system
          }
        else
          return QuadBase4N<SectionFDPhysicalProperties>::addLoad(theLoad,loadFactor);
      }
    return 0;
  }



int XC::ShellMITC4Base::addInertiaLoadToUnbalance(const Vector &accel)
  {
    int tangFlag= 1;

    int i;

    const bool haveRho= physicalProperties.haveRho();
    if(!haveRho)
      return 0;

    int count= 0;
    for(i=0;i<4;i++)
      {
        const Vector &Raccel= theNodes[i]->getRV(accel);
        //XXX ¿cambiar de base Raccel al sistema local?
        for(int j=0;j<6;j++)
          resid(count++)= Raccel(i);
      }

    formInertiaTerms(tangFlag);
    if(load.isEmpty())
      load.reset(24);
    load.addMatrixVector(1.0, mass, resid, -1.0);
    return 0;
  }

//! @brief get residual
const XC::Vector &XC::ShellMITC4Base::getResistingForce(void) const
  {
    theCoordTransf->update();

    const int tang_flag= 0; //don't get the tangent
    formResidAndTangent(tang_flag);
    // subtract external loads
    if(!load.isEmpty())
      resid-= load;
    resid+= theCoordTransf->getGlobalResistingForce(p0.getVector());

    if(isDead())
      resid*=dead_srf;
    return resid;
  }

//! @brief get residual with inertia terms
const XC::Vector &XC::ShellMITC4Base::getResistingForceIncInertia(void) const
  {
    static Vector res(24);
    res= getResistingForce();

    formInertiaTerms(0);

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      res+= this->getRayleighDampingForces();

    return res;
  }

double XC::ShellMITC4Base::getMeanInternalForce(const std::string &code) const
  { return physicalProperties.getMeanInternalForce(code); }

double XC::ShellMITC4Base::getMeanInternalDeformation(const std::string &code) const
  { return physicalProperties.getMeanInternalDeformation(code); }

//! @brief Consuma la coordinate transformation de acuerdo con el estado actual.
int XC::ShellMITC4Base::commitState(void)
  {
    int retval= theCoordTransf->commitState();
    retval+= QuadBase4N<SectionFDPhysicalProperties>::commitState();
    return retval;
  }

//! @brief Returns to the last commited state.
int XC::ShellMITC4Base::revertToLastCommit(void)
  {
    int retval= theCoordTransf->revertToLastCommit();
    retval+= QuadBase4N<SectionFDPhysicalProperties>::revertToLastCommit();
    return retval;
  }

//! @brief Return the initial state.
int XC::ShellMITC4Base::revertToStart(void)
  {
    zeroLoad();
    int retval= theCoordTransf->revertToStart();
    retval+= QuadBase4N<SectionFDPhysicalProperties>::revertToStart();
    return retval;
  }

//! @brief Returns interpolattion factors for a material point.
XC::Vector XC::ShellMITC4Base::getInterpolationFactors(const ParticlePos3d &pos) const
  {
    static const int numberNodes= 4;
    static const int nShape= 3;
    static const int massIndex= nShape - 1;

    double xsj;  // determinant of the jacobian matrix
    static double shp[nShape][numberNodes];  //shape functions at a gauss point
    Vector retval(numberNodes);


    //compute basis vectors and local nodal coordinates
    //theCoordTransf->update( );


    shape2d(pos.r_coordinate(), pos.s_coordinate(), xl, shp, xsj );

    //node loop to compute displacements
    retval.Zero( );
    for(int j= 0; j < numberNodes; j++ )
      retval[j]= shp[massIndex][j];

    return retval;
  }

//! @brief Returns interpolation factors for a material point.
XC::Vector XC::ShellMITC4Base::getInterpolationFactors(const Pos3d &pos) const
  { return getInterpolationFactors(getNaturalCoordinates(pos)); }

XC::ParticlePos3d XC::ShellMITC4Base::getLocalCoordinatesOfNode(const int &i) const
  { return ParticlePos3d(xl[0][i],xl[1][i],0.0); }

XC::ParticlePos3d XC::ShellMITC4Base::getNaturalCoordinates(const Pos3d &p) const
  { return theCoordTransf->getNaturalCoordinates(p,xl); }

//! @brief Returns interpolated displacements for a material point.
XC::Vector XC::ShellMITC4Base::getInterpolatedDisplacements(const ParticlePos3d &pos) const
  {
    //  six(6) nodal dof's ordered :
    //
    //    -        - 
    //   |    u1    |   <---plate membrane
    //   |    u2    |
    //   |----------|
    //   |  w= u3  |   <---plate bending
    //   |  theta1  | 
    //   |  theta2  | 
    //   |----------|
    //   |  theta3  |   <---drill 
    //    -        -  


    static const int ndf= 6;
    static const int numberNodes= 4;
    Vector displacement(ndf);


    //compute basis vectors and local nodal coordinates
    //theCoordTransf->update( );


    const Vector factors= getInterpolationFactors(pos);

    //node loop to compute displacements
    displacement.Zero( );
    for(int j= 0; j < numberNodes; j++ )
      //displacement += ( factor[j] * theNodes[j]->getTrialAccel() );
      displacement.addVector(1.0, theCoordTransf->getBasicTrialAccel(j), factors[j] );

    return displacement;
  }

//! @brief Returns interpolattion factors for a material point.
XC::Vector XC::ShellMITC4Base::getInterpolatedDisplacements(const Pos3d &pos) const
  { return getInterpolatedDisplacements(getNaturalCoordinates(pos)); }

//! @brief form inertia terms
void XC::ShellMITC4Base::formInertiaTerms( int tangFlag ) const
  {

    //translational mass only
    //rotational inertia terms are neglected


    static const int ndf= 6;
    static const int numberNodes= 4;
    static const int numberGauss= 4;
    static const int nShape= 3;
    static const int massIndex= nShape - 1;

    double xsj;  // determinant of the jacobian matrix
    double dvol; //volume element
    static double shp[nShape][numberNodes];  //shape functions at a gauss point
    static Vector momentum(ndf);


    double temp, rhoH, massJK;


    //zero mass
    mass.Zero( );

    //compute basis vectors and local nodal coordinates
    //theCoordTransf->update( );


    //gauss loop
    for(int i= 0; i < numberGauss; i++ )
      {
        //get shape functions
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
        shape2d(gp.r_coordinate(), gp.s_coordinate(), xl, shp, xsj );

        //volume element to also be saved
        dvol= gp.weight() * xsj;


        //node loop to compute accelerations
        momentum.Zero( );
        for(int j= 0; j < numberNodes; j++ )
          //momentum += ( shp[massIndex][j] * theNodes[j]->getTrialAccel() );
          momentum.addVector(1.0, theCoordTransf->getBasicTrialAccel(j), shp[massIndex][j] );

        //density on the Gauss point i.
        rhoH= physicalProperties[i]->getRho();

        //multiply acceleration by density to form momentum
        momentum*= rhoH;


        //residual and tangent calculations node loops
        for(int j=0, jj=0; j<numberNodes; j++, jj+=ndf )
          {
            temp= shp[massIndex][j] * dvol;
            for(int p= 0;p<3;p++)
              resid(jj+p )+= (temp*momentum(p));


            if(tangFlag == 1 && rhoH != 0.0)
              {
               //multiply by density
               temp *= rhoH;

               for(int k=0, kk=0; k<numberNodes; k++, kk+=ndf )
                 {
                   massJK= temp * shp[massIndex][k];
                   //node-node translational mass
                   for(int p= 0;p<3;p++)
                     mass(jj+p,kk+p)+= massJK;
                   //we put a very small rotational mass
                   //(error is negligible) but not zero.
                   //This way we avoid singularity of M.
                   for(int p= 3;p<6;p++)
                     mass(jj+p,kk+p)+= massJK*1.0e-10;
                 } // end for k loop
              } // end if tang_flag
          } // end for j loop
      } //end for i gauss loop
  }

//! @brief form residual and tangent
void XC::ShellMITC4Base::formResidAndTangent(int tang_flag) const
  {
    //
    //  six(6) nodal dof's ordered :
    //
    //    -        - 
    //   |    u1    |   <---plate membrane
    //   |    u2    |
    //   |----------|
    //   |  w= u3  |   <---plate bending
    //   |  theta1  | 
    //   |  theta2  | 
    //   |----------|
    //   |  theta3  |   <---drill 
    //    -        -  
    //
    // membrane strains ordered :
    //
    //            strain(0)=   eps00     i.e.   (11)-strain
    //            strain(1)=   eps11     i.e.   (22)-strain
    //            strain(2)=   gamma01   i.e.   (12)-shear
    //
    // curvatures and shear strains ordered  :
    //
    //            strain(3)=     kappa00  i.e.   (11)-curvature
    //            strain(4)=     kappa11  i.e.   (22)-curvature
    //            strain(5)=   2*kappa01  i.e. 2*(12)-curvature 
    //
    //            strain(6)=     gamma02  i.e.   (13)-shear
    //            strain(7)=     gamma12  i.e.   (23)-shear
    //
    //  same ordering for moments/shears but no 2 
    //  
    //  Then, 
    //              epsilon00= -z * kappa00      +    eps00_membrane
    //              epsilon11= -z * kappa11      +    eps11_membrane
    //  gamma01= 2*epsilon01= -z * (2*kappa01)  +  gamma01_membrane 
    //
    //  Shear strains gamma02, gamma12 constant through cross section
    //

    static const int ndf= 6; //two membrane plus three bending plus one drill
    static const int nstress= 8; //three membrane, three moment, two shear
    static const int ngauss= 4;
    static const int numnodes= 4;

    int i,  j,  k, p, q;
    int jj, kk;

    int success;
    
    double volume= 0.0;

    static double xsj;  // determinant jacaobian matrix 
    static double dvol[ngauss]; //volume element
    static Vector strain(nstress);  //strain
    static double shp[3][numnodes];  //shape functions at a gauss point

    //  static double Shape[3][numnodes][ngauss]; //all the shape functions
    static Vector residJ(ndf); //nodeJ residual 
    static Matrix stiffJK(ndf,ndf); //nodeJK stiffness 
    static Vector stress(nstress);  //stress resultants
    static Matrix dd(nstress,nstress);  //material tangent
    static Matrix J0(2,2);  //Jacobian at center
    static Matrix J0inv(2,2); //inverse of Jacobian at center

    double epsDrill= 0.0;  //drilling "strain"
    double tauDrill= 0.0; //drilling "stress"

    //---------B-matrices------------------------------------
    static Matrix BJ(nstress,ndf);      // B matrix node J
    static Matrix BJtran(ndf,nstress);
    static Matrix BK(nstress,ndf);      // B matrix node k
    static Matrix BJtranD(ndf,nstress);
    static Matrix Bbend(3,3);  // bending B matrix
    static Matrix Bshear(2,3); // shear B matrix
    static Matrix Bmembrane(3,2); // membrane B matrix
    static double BdrillJ[ndf]; //drill B matrix
    static double BdrillK[ndf];  

    double *drillPointer;

    static double saveB[nstress][ndf][numnodes];

    //------------------------------------------------------- 

    //zero stiffness and residual 
    stiff.Zero( );
    resid.Zero( );

    const Matrix G= calculateG();

    Matrix Ms(2,4);
    Ms.Zero();
    Matrix Bsv(2,12);
    Bsv.Zero();

    const double Ax= -xl[0][0]+xl[0][1]+xl[0][2]-xl[0][3];
    const double Bx=  xl[0][0]-xl[0][1]+xl[0][2]-xl[0][3];
    const double Cx= -xl[0][0]-xl[0][1]+xl[0][2]+xl[0][3];

    const double Ay= -xl[1][0]+xl[1][1]+xl[1][2]-xl[1][3];
    const double By=  xl[1][0]-xl[1][1]+xl[1][2]-xl[1][3];
    const double Cy= -xl[1][0]-xl[1][1]+xl[1][2]+xl[1][3];

    const double alph= atan2(Ay,Ax);
    const double beta= 3.141592653589793/2-atan2(Cx,Cy);
    Matrix Rot(2,2);
    Rot.Zero();
    Rot(0,0)=sin(beta);
    Rot(0,1)=-sin(alph);
    Rot(1,0)=-cos(beta);
    Rot(1,1)=cos(alph);
    Matrix Bs(2,12);
    
    double r1= 0;
    double r2= 0;
    double r3= 0;

    //gauss loop 
    for(i= 0;i<ngauss;i++)
      {
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
        r1= Cx + gp.r_coordinate()*Bx;
        r3= Cy + gp.r_coordinate()*By;
        r1= r1*r1 + r3*r3;
        r1= sqrt (r1);
        r2= Ax + gp.s_coordinate()*Bx;
        r3= Ay + gp.s_coordinate()*By;
        r2= r2*r2 + r3*r3;
        r2= sqrt (r2);

        //get shape functions    
        shape2d( gp.r_coordinate(), gp.s_coordinate(), xl, shp, xsj );
        //volume element to also be saved
        dvol[i]= gp.weight() * xsj;  
        volume += dvol[i];

        Ms(1,0)=1-gp.r_coordinate();
        Ms(0,1)=1-gp.s_coordinate();
        Ms(1,2)=1+gp.r_coordinate();
        Ms(0,3)=1+gp.s_coordinate();
        Bsv= Ms*G;

        for(j=0;j<12;j++)
          {
            Bsv(0,j)=Bsv(0,j)*r1/(8*xsj);
            Bsv(1,j)=Bsv(1,j)*r2/(8*xsj);
          }
        Bs=Rot*Bsv;

        //zero the strains
        strain.Zero( );
        epsDrill= 0.0;

        // j-node loop to compute strain 
        for(j=0;j<numnodes;j++)
          {
            //compute B matrix 
            Bmembrane= computeBmembrane( j, shp );
            Bbend= computeBbend( j, shp );

            for(p=0;p<3;p++)
              {
                Bshear(0,p)= Bs(0,j*3+p);
                Bshear(1,p)= Bs(1,j*3+p);
              }//end for p
            BJ= assembleB( Bmembrane, Bbend, Bshear );

            //save the B-matrix
            for(p=0; p<nstress; p++)
              {
                for(q=0; q<ndf; q++ )
                  saveB[p][q][j]= BJ(p,q);
              }//end for p

            //nodal "displacements" 
            const Vector &ul= theCoordTransf->getBasicTrialDisp(j);

            //compute the strain
            //strain += (BJ*ul); 
            strain.addMatrixVector(1.0, BJ,ul,1.0 );

            //drilling B matrix
            drillPointer= computeBdrill( j, shp );
            for(p=0; p<ndf; p++ )
              {
                BdrillJ[p]= *drillPointer; //set p-th component
                drillPointer++;             //pointer arithmetic
              }//end for p

            //drilling "strain" 
            for(p=0;p<ndf;p++)
              epsDrill +=  BdrillJ[p]*ul(p);
          } // end for j
    

        //send the strain to the material
        success= const_cast<SectionForceDeformation *>(physicalProperties[i])->setTrialSectionDeformation( strain );

        //compute the stress
        stress= physicalProperties[i]->getStressResultant( );

        //drilling "stress" 
        tauDrill= Ktt * epsDrill;

        //multiply by volume element
        stress*= dvol[i];
        tauDrill*= dvol[i];

        if( tang_flag == 1 )
          {
            dd= physicalProperties[i]->getSectionTangent( );
            dd *= dvol[i];
          } //end if tang_flag


        //residual and tangent calculations node loops

        jj= 0;
        for(j=0;j<numnodes;j++)
          {
            //extract BJ
            for(p=0; p<nstress;p++)
              {
                for(q=0;q<ndf;q++)
                  BJ(p,q)= saveB[p][q][j];
              }//end for p

            //multiply bending terms by (-1.0) for correct statement
            // of equilibrium  
            for(p= 3;p<6;p++)
              {
                for(q=3;q<6;q++) 
                  BJ(p,q) *= (-1.0);
              } //end for p

            //transpose 
            for(p=0; p<ndf; p++)
              {
                for(q=0; q<nstress; q++) 
                  BJtran(p,q)= BJ(q,p);
              }//end for p

            residJ.addMatrixVector(0.0, BJtran,stress,1.0 );

            //drilling B matrix
            drillPointer= computeBdrill( j, shp );
            for(p=0; p<ndf; p++ )
              {
                BdrillJ[p]= *drillPointer;
                drillPointer++;
              }//end for p

            //residual including drill
            for(p=0;p<ndf;p++)
              resid( jj + p ) += ( residJ(p) + BdrillJ[p]*tauDrill );

            if(tang_flag==1)
              {
                BJtranD.addMatrixProduct(0.0, BJtran,dd,1.0 );
                for(p=0; p<ndf; p++) 
                  BdrillJ[p] *= ( Ktt*dvol[i] );

                kk= 0;
                for(k=0;k<numnodes;k++)
                  {
                    //extract BK
                    for(p=0; p<nstress; p++)
                      {
                        for(q=0; q<ndf; q++ )
                          BK(p,q)= saveB[p][q][k];
                      }//end for p
            
                     //drilling B matrix
                     drillPointer= computeBdrill( k, shp );
                     for(p=0; p<ndf; p++ )
                       {
                         BdrillK[p]= *drillPointer;
                         drillPointer++;
                       }//end for p
 
                    //stiffJK= BJtranD * BK;
                    // +  transpose( 1,ndf,BdrillJ ) * BdrillK; 
                    stiffJK.addMatrixProduct(0.0, BJtranD,BK,1.0);

                    for(p=0;p<ndf;p++)
                      {
                        for(q=0;q<ndf;q++)
                          {
                            stiff(jj+p,kk+q) += stiffJK(p,q) + (BdrillJ[p]*BdrillK[q]);
                          }//end for q
                      }//end for p
                    kk+= ndf;
                  } // end for k loop
              } // end if tang_flag 
          jj+= ndf;
        } // end for j loop
      } //end for i gauss loop     
    theCoordTransf->getGlobalResidAndTangent(resid,stiff);
    return;
  }

//! @brief compute local coordinates and basis
void XC::ShellMITC4Base::computeBasis(void)
  {
    theCoordTransf->initialize(theNodes);
    theCoordTransf->setup_nodal_local_coordinates(xl);
  }

//! @brief compute Bdrill
double *XC::ShellMITC4Base::computeBdrill( int node, const double shp[3][4]) const
  {

    //static Matrix Bdrill(1,6);
    static double Bdrill[6];

    static double B1;
    static double B2;
    static double B6;


//---Bdrill Matrix in standard {1,2,3} mechanics notation---------
//
//             -                                       -
//   Bdrill= | -0.5*N,2   +0.5*N,1    0    0    0   -N |   (1x6)
//             -                                       -
//
//----------------------------------------------------------------

    //  Bdrill.Zero( );

    //Bdrill(0,0)= -0.5*shp[1][node];

    //Bdrill(0,1)= +0.5*shp[0][node];

    //Bdrill(0,5)=     -shp[2][node];


    B1= -0.5*shp[1][node];
    B2= +0.5*shp[0][node];
    B6= -shp[2][node];
   
    const Vector &g1= theCoordTransf->G1();
    const Vector &g2= theCoordTransf->G2();

    Bdrill[0]= B1*g1[0] + B2*g2[0];
    Bdrill[1]= B1*g1[1] + B2*g2[1];
    Bdrill[2]= B1*g1[2] + B2*g2[2];

    const Vector &g3= theCoordTransf->G3();
    Bdrill[3]= B6*g3[0];
    Bdrill[4]= B6*g3[1];
    Bdrill[5]= B6*g3[2];

    return Bdrill;
  }

//! @brief compute Bmembrane matrix
const XC::Matrix &XC::ShellMITC4Base::computeBmembrane( int node, const double shp[3][4] ) const
  {

    static Matrix Bmembrane(3,2);

//---Bmembrane matrix in standard {1,2,3} mechanics notation---------
//
//                -             -
//               | +N,1      0   |
// Bmembrane=    |   0     +N,2  |    (3x2)
//               | +N,2    +N,1  |
//                -             -
//
//  three(3) strains and two(2) displacements (for plate)
//-------------------------------------------------------------------

    Bmembrane.Zero( );

    Bmembrane(0,0)= shp[0][node];
    Bmembrane(1,1)= shp[1][node];
    Bmembrane(2,0)= shp[1][node];
    Bmembrane(2,1)= shp[0][node];

    return Bmembrane;
  }

//! @brief assemble a B matrix
//! @param Bmembrane: membrane B matrix (3x2)
//! @param Bbend:  plate bending B matrix (3x2)
//! @param Bshear: plate shear B matrix (2x3)
const XC::Matrix &XC::ShellMITC4Base::assembleB(const Matrix &Bmembrane, const Matrix &Bbend, const Matrix &Bshear) const
  {

    static Matrix B(8,6);
    static Matrix BmembraneShell(3,3);
    static Matrix BbendShell(3,3);
    static Matrix BshearShell(2,6);
    static Matrix Gmem(2,3);
    static Matrix Gshear(3,6);

//
// For Shell :
//
//---B Matrices in standard {1,2,3} mechanics notation---------
//
//            -                     _
//           | Bmembrane  |     0    |
//           | --------------------- |
//    B=     |     0      |  Bbend   |   (8x6)
//           | --------------------- |
//           |         Bshear        |
//            -           -         -
//
//-------------------------------------------------------------
//
//


    //shell modified membrane terms

    const Vector &g1= theCoordTransf->G1();
    Gmem(0,0)= g1[0];
    Gmem(0,1)= g1[1];
    Gmem(0,2)= g1[2];

    const Vector &g2= theCoordTransf->G2();
    Gmem(1,0)= g2[0];
    Gmem(1,1)= g2[1];
    Gmem(1,2)= g2[2];

    //BmembraneShell= Bmembrane * Gmem;
    BmembraneShell.addMatrixProduct(0.0, Bmembrane,Gmem,1.0 );


    //shell modified bending terms
    Matrix &Gbend= Gmem;

    //BbendShell= Bbend * Gbend;
    BbendShell.addMatrixProduct(0.0, Bbend,Gbend,1.0 );


    //shell modified shear terms
    Gshear.Zero( );

    const Vector &g3= theCoordTransf->G3();
    Gshear(0,0)= g3[0];
    Gshear(0,1)= g3[1];
    Gshear(0,2)= g3[2];

    Gshear(1,3)= g1[0];
    Gshear(1,4)= g1[1];
    Gshear(1,5)= g1[2];

    Gshear(2,3)= g2[0];
    Gshear(2,4)= g2[1];
    Gshear(2,5)= g2[2];

    //BshearShell= Bshear * Gshear;
    BshearShell.addMatrixProduct(0.0, Bshear,Gshear,1.0 );

    B.Zero( );

    //assemble B from sub-matrices

    //membrane terms
    for(int p= 0; p < 3; p++ )
      {
        for(int q= 0; q < 3; q++ )
          B(p,q)= BmembraneShell(p,q);
      } //end for p


    //bending terms
    int pp;
    for(int p= 3; p < 6; p++ )
      {
        pp= p - 3;
        for(int q= 3; q < 6; q++ )
          B(p,q)= BbendShell(pp,q-3);
      } // end for p


    //shear terms
    for(int p= 0; p < 2; p++ )
      {
        pp= p + 6;
        for(int q= 0; q < 6; q++ )
          { B(pp,q)= BshearShell(p,q); } // end for q
      } //end for p

    return B;
  }

//! @brief compute Bbend matrix
const XC::Matrix &XC::ShellMITC4Base::computeBbend( int node, const double shp[3][4] ) const
  {

      static XC::Matrix Bbend(3,2);

//---Bbend matrix in standard {1,2,3} mechanics notation---------
//
//            -             -
//   Bbend=  |    0    -N,1  |
//           |  +N,2     0   |    (3x2)
//           |  +N,1   -N,2  |
//            -             -
//
//  three(3) curvatures and two(2) rotations (for plate)
//----------------------------------------------------------------

      Bbend.Zero( );

      Bbend(0,1)= -shp[0][node];
      Bbend(1,0)=  shp[1][node];
      Bbend(2,0)=  shp[0][node];
      Bbend(2,1)= -shp[1][node];

      return Bbend;
  }

//! @brief shape function routine for MITC4 elements.
//! @param ss "s" natural coordinate of the point.
//! @param tt "t" natural coordinate of the point.
//! @param x[2][4]
//! @param shp[3][4] shape function values at the gauss points.
//! @param xsj
void XC::ShellMITC4Base::shape2d(const double &ss, const double &tt,const double x[2][4],double shp[3][4],double &xsj)
  {

    static const double s[]= { -0.5,  0.5, 0.5, -0.5 };
    static const double t[]= { -0.5, -0.5, 0.5,  0.5 };

    static double xs[2][2];
    static double sx[2][2];

    for(int i= 0; i < 4; i++ )
      {
        shp[2][i]= ( 0.5 + s[i]*ss )*( 0.5 + t[i]*tt );
        shp[0][i]= s[i] * ( 0.5 + t[i]*tt );
        shp[1][i]= t[i] * ( 0.5 + s[i]*ss );
      } // end for i


    // Construct jacobian and its inverse

    for(int i= 0; i < 2; i++ )
      {
        for(int j= 0; j < 2; j++ )
          {
            xs[i][j]= 0.0;
            for(int k= 0;k<4;k++)
              xs[i][j]+=  x[i][k] * shp[j][k];
          } //end for j
      }  // end for i

    xsj= xs[0][0]*xs[1][1] - xs[0][1]*xs[1][0];

    //inverse jacobian
    double jinv= 1.0 / xsj;
    sx[0][0]=  xs[1][1] * jinv;
    sx[1][1]=  xs[0][0] * jinv;
    sx[0][1]= -xs[0][1] * jinv;
    sx[1][0]= -xs[1][0] * jinv;


    //form global derivatives
    double temp= 0.0;
    for(int i= 0; i < 4; i++ )
      {
        temp= shp[0][i]*sx[0][0] + shp[1][i]*sx[1][0];
        shp[1][i]= shp[0][i]*sx[0][1] + shp[1][i]*sx[1][1];
        shp[0][i]= temp;
      } // end for i
    return;
  }

//! @brief Envía la coordinate transformation through the channel being passed as parameter.
int XC::ShellMITC4Base::sendCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,CommParameters &cp)
  {
    int retval= 0;
    DbTagData &data= getDbTagData();
    if(!theCoordTransf)
      data.setDbTagDataPos(posFlag,1);
    else
      {
        data.setDbTagDataPos(posFlag,0);
        data.setDbTagDataPos(posClassTag,theCoordTransf->getClassTag());
        cp.sendMovable(*theCoordTransf,data,CommMetaData(posDbTag));
      }
    return retval;
  }

//! @brief Recibe la coordinate transformation through the channel being passed as parameter.
int XC::ShellMITC4Base::recvCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,const CommParameters &cp)
  {
    int res= 0;
    DbTagData &data= getDbTagData();
    const int flag= data.getDbTagDataPos(posFlag);
    if(flag == 0)
      {
        if(!theCoordTransf)
          {
            theCoordTransf= cp.getNewShellCrdTransf3d(data.getDbTagDataPos(posClassTag));
            if(!theCoordTransf)
              std::cerr << "receiveCoordTransf -- ran out of memory\n";
          }
        if(theCoordTransf)
          {
            cp.receiveMovable(*theCoordTransf,data,CommMetaData(posDbTag));
            theCoordTransf->revertToLastCommit();// Revert the crdtransf to its last committed state
            if(res<0)
              std::cerr << "receiveCoordTransf - failed to receive vector data\n";
          }
      }
    return res;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::ShellMITC4Base::sendData(CommParameters &cp)
  {
    int res= QuadBase4N<SectionFDPhysicalProperties>::sendData(cp);
    res+=cp.sendDoubles(Ktt,xl[0][0],xl[0][1],xl[0][2],xl[0][3],getDbTagData(),CommMetaData(8));
    res+=cp.sendDoubles(xl[1][0],xl[1][1],xl[1][2],xl[1][3],getDbTagData(),CommMetaData(9));
    res+= p0.sendData(cp,getDbTagData(),CommMetaData(10));
    res+= cp.sendMatrix(Ki,getDbTagData(),CommMetaData(11));
    res+= cp.sendVectors(inicDisp,getDbTagData(),CommMetaData(12));
    res+= sendCoordTransf(13,14,15,cp);
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::ShellMITC4Base::recvData(const CommParameters &cp)
  {
    int res= QuadBase4N<SectionFDPhysicalProperties>::recvData(cp);
    res+=cp.receiveDoubles(Ktt,xl[0][0],xl[0][1],xl[0][2],xl[0][3],getDbTagData(),CommMetaData(8));
    res+=cp.receiveDoubles(xl[1][0],xl[1][1],xl[1][2],xl[1][3],getDbTagData(),CommMetaData(9));
    res+= p0.receiveData(cp,getDbTagData(),CommMetaData(10));
    res+= cp.receiveMatrix(Ki,getDbTagData(),CommMetaData(11));
    res+= cp.receiveVectors(inicDisp,getDbTagData(),CommMetaData(12));
    res+= recvCoordTransf(13,14,15,cp);
    return res;
  }

//! @brief Returs a matrix with the axes of the element as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
XC::Matrix XC::ShellMITC4Base::getLocalAxes(bool initialGeometry) const
  { return theCoordTransf->getLocalAxes(initialGeometry); }

//! @brief Returns a pointer to the coordinate transformation.
XC::ShellCrdTransf3dBase *XC::ShellMITC4Base::getCoordTransf(void)
  { return theCoordTransf; }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
const XC::ShellCrdTransf3dBase *XC::ShellMITC4Base::getCoordTransf(void) const
  { return theCoordTransf; }


