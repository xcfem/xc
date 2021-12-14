//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//ShellUpBasisCrdTransf3d.cc

#include "ShellUpBasisCrdTransf3d.h"
#include "utility/matrix/Matrix.h"
#include <domain/mesh/node/Node.h>

#include "domain/mesh/element/utils/NodePtrs.h"
#include "R3vectors.h"
#include "domain/mesh/element/plane/shell/ShellMITC4Base.h"

//! @brief Default constructor
XC::ShellUpBasisCrdTransf3d::ShellUpBasisCrdTransf3d(void)
  : ShellLinearCrdTransf3d() {}

//! @brief Constructor.
XC::ShellUpBasisCrdTransf3d::ShellUpBasisCrdTransf3d(const Vector &v1,const Vector &v2,const Vector &v3)
  : ShellLinearCrdTransf3d(v1,v2,v3) {}

//! @brief Computes basic vectors from node coordinates.
XC::ShellUpBasisCrdTransf3d::ShellUpBasisCrdTransf3d(const NodePtrs &theNodes)
  : ShellLinearCrdTransf3d()
  { initialize(theNodes); }


//! @brief Virtual constructor.
XC::ShellCrdTransf3dBase *XC::ShellUpBasisCrdTransf3d::getCopy(void) const
  { return new ShellUpBasisCrdTransf3d(*this); }

//! @brief Updates the transformation (needs debugging).
int XC::ShellUpBasisCrdTransf3d::update(void)
  {
    //could compute derivatives \frac{ \partial {\bf x} }{ \partial L_1 } 
    //                     and  \frac{ \partial {\bf x} }{ \partial L_2 }
    //and use those as basis vectors but this is easier 
    //and the shell is flat anyway.

    static Vector temp(3);

    static Vector v1(3);
    static Vector v2(3);
    static Vector v3(3);

    //get two vectors (v1, v2) in plane of shell by 
    // nodal coordinate differences
    const ShellMITC4Base *element= dynamic_cast<const ShellMITC4Base *>(Owner());
    const std::vector<Vector> &initDisp= element->getInitDisp();
    const Vector &coor0= (*theNodes)[0]->getCrds( ) + (*theNodes)[0]->getTrialDisp() - initDisp[0];
    const Vector &coor1= (*theNodes)[1]->getCrds( ) + (*theNodes)[1]->getTrialDisp() - initDisp[1];
    const Vector &coor2= (*theNodes)[2]->getCrds( ) + (*theNodes)[2]->getTrialDisp() - initDisp[2];
    const Vector &coor3= (*theNodes)[3]->getCrds( ) + (*theNodes)[3]->getTrialDisp() - initDisp[3];

    v1.Zero( );
    //v1= 0.5 * ( coor2 + coor1 - coor3 - coor0 );
    v1= coor2;
    v1+= coor1;
    v1-= coor3;
    v1-= coor0;
    v1*= 0.50;

    v2.Zero( );
    //v2= 0.5 * ( coor3 + coor2 - coor1 - coor0 );
    v2= coor3;
    v2+= coor2;
    v2-= coor1;
    v2-= coor0;
    v2*= 0.50;

    //normalize v1 
    //double length= LovelyNorm( v1 );
    double length= v1.Norm( );
    v1/= length;

    //Gram-Schmidt process for v2 

    //double alpha= LovelyInnerProduct( v2, v1 );
    double alpha= v2^v1;

    //v2 -= alpha*v1;
    temp= v1;
    temp*= alpha;
    v2-= temp;

    //normalize v2 
    //length= LovelyNorm( v2 );
    length= v2.Norm( );
    v2/= length;

    //cross product for v3  
    v3= LovelyCrossProduct( v1, v2 );

    //local nodal coordinates in plane of shell
    Shell4NBase::pointer_to_xl xl= element->get_xl();
    for(int i= 0; i < 4; i++ )
      {
    	 const Vector &coorI= (*theNodes)[i]->getCrds( );
    	 xl[0][i]= coorI^v1;
    	 xl[1][i]= coorI^v2;

      }//end for i 

    //basis vectors stored as array of doubles
    for(int i= 0; i < 3; i++ )
      {
    	g1[i]= v1(i);
    	g2[i]= v2(i);
    	g3[i]= v3(i);
      }  //end for i 
    return 0;
  }

