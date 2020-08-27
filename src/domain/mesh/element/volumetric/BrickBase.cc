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
//BrickBase.cc

#include "BrickBase.h"
#include "preprocessor/multi_block_topology/matrices/NodePtrArray3d.h"
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "domain/mesh/node/Node.h"
#include "vtkCellType.h"
#include "xc_utils/src/geom/d3/3d_polyhedrons/Tetrahedron3d.h"
#include "domain/mesh/element/utils/ParticlePos3d.h"

const int XC::BrickBase::numberNodes; //!< Number of nodes.
const int XC::BrickBase::ndm; //!< Space dimension
const int XC::BrickBase::ndf; //!< Number of DOFs per node.
double XC::BrickBase::xl[XC::BrickBase::ndm][XC::BrickBase::numberNodes];
const double XC::BrickBase::mNodesR[]= {-1, +1, +1, -1, -1, +1, +1, -1};
const double XC::BrickBase::mNodesS[]= {-1, -1, +1, +1, -1, -1, +1, +1};
const double XC::BrickBase::mNodesT[]= {-1, -1, -1, -1, +1, +1, +1, +1};

//! @brief Constructor
XC::BrickBase::BrickBase(int classTag)
  : ElemWithMaterial<8,NDMaterialPhysicalProperties>(0,classTag,NDMaterialPhysicalProperties(8,nullptr))
  {}


//! @brief Constructor.
XC::BrickBase::BrickBase(int tag,int classTag,const NDMaterialPhysicalProperties &pp)
  :ElemWithMaterial<8,NDMaterialPhysicalProperties>(tag,classTag,pp)
  {}

//! @brief Constructor.
XC::BrickBase::BrickBase(int tag, int classTag, int node1, int node2, int node3, int node4,int node5,int node6,int node7,int node8, const NDMaterialPhysicalProperties &pp) 
  : ElemWithMaterial<8,NDMaterialPhysicalProperties>(tag,classTag,pp)
  {
    theNodes.set_id_nodes(node1,node2,node3,node4,node5,node6,node7,node8);
  }

//! @brief Return the element dimension (0, 1, 2 o3 3).
size_t XC::BrickBase::getDimension(void) const
  { return 3; }

//! @brief Return the element volume.
double XC::BrickBase::getVolume(bool initialGeometry) const
  {
    double factor= 1.0;
    if(initialGeometry)
      factor= 0.0;
    const Pos3d p1= theNodes[0]->getCurrentPosition3d(factor);
    const Pos3d p2= theNodes[1]->getCurrentPosition3d(factor);
    const Pos3d p3= theNodes[2]->getCurrentPosition3d(factor);
    const Pos3d p4= theNodes[3]->getCurrentPosition3d(factor);
    const Pos3d p5= theNodes[4]->getCurrentPosition3d(factor);
    const Pos3d p6= theNodes[5]->getCurrentPosition3d(factor);
    const Pos3d p7= theNodes[6]->getCurrentPosition3d(factor);
    const Pos3d p8= theNodes[7]->getCurrentPosition3d(factor);

    double retval= 0.0;
    const Tetrahedron3d tet1(p8,p6,p5,p1);
    retval+= tet1.getVolume();
    const Tetrahedron3d tet2(p8,p7,p6,p3); 
    retval+= tet2.getVolume();
    const Tetrahedron3d tet3(p8,p6,p1,p3); 
    retval+= tet3.getVolume();
    const Tetrahedron3d tet4(p1,p3,p4,p8); 
    retval+= tet4.getVolume();
    const Tetrahedron3d tet5(p1,p2,p3,p6); 
    retval+= tet5.getVolume();
    return retval;
  }

//! @brief Return a grid of booleans, one for each of the
//! element nodes. If there is a node that doesn't exist
//! for a position the correspondin value will be false.
BoolArray3d XC::BrickBase::getNodePattern(void) const
  {
    BoolArray3d retval(2,2,2,true); //Two layers, two rows, two columns.
    return retval;
  }

//! @brief Put the element on the mesh being passed as parameter.
XC::ElemPtrArray3d XC::BrickBase::put_on_mesh(const XC::NodePtrArray3d &nodes,meshing_dir dm) const
  {
    const size_t numberOfLayers= nodes.getNumberOfLayers();
    const size_t numberOfRows= nodes.getNumberOfRows();
    const size_t numberOfColumns= nodes.getNumberOfColumns();
    const size_t mesh_dim= nodes.GetDim();
    ElemPtrArray3d retval(numberOfLayers-1,numberOfRows-1,numberOfColumns-1);
    if(mesh_dim<3)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; three-dimensional mesh needed, can't create elements."
		<< std::endl;
    else
      {
        for(size_t i=1;i<numberOfLayers;i++)
          for(size_t j=1;j<numberOfRows;j++)
            for(size_t k=1;k<numberOfColumns;k++)
              {
	        XC::Element *tmp= this->getCopy();
                const Node *Nd1= nodes(i,j,k);
                const Node *Nd2= nodes(i,j+1,k);
                const Node *Nd3= nodes(i,j+1,k+1);
                const Node *Nd4= nodes(i,j,k+1);
                const Node *Nd5= nodes(i+1,j,k);
                const Node *Nd6= nodes(i+1,j+1,k);
                const Node *Nd7= nodes(i+1,j+1,k+1);
                const Node *Nd8= nodes(i+1,j,k+1);
                bool changed= tmp->getNodePtrs().set_id_nodes(Nd1->getTag(),Nd2->getTag(),Nd3->getTag(),Nd4->getTag(),Nd5->getTag(),Nd6->getTag(),Nd7->getTag(),Nd8->getTag());
		if(changed)
		  std::cerr << getClassName() << "::" << __FUNCTION__
	                    << "; nodes were already assingned.."
		             << std::endl;

                retval(i,j,k)= tmp;
              }
      }
    return retval;
  }

int XC::BrickBase::getVtkCellType(void) const
  { return VTK_HEXAHEDRON; }

//! @brief Create the matrix that can be used to extrapolate
//! the results from the Gauss points to the element nodes.
//!
//! Reference: INTRODUCTION TO THE FINITE ELEMENT METHOD
//! G. P. Nikishkov
//! 2004 Lecture Notes. University of Aizu, Aizu-Wakamatsu
//! 965-8580, Japanniki@u-aizu.ac.jp section 4.3.7
XC::Matrix &XC::BrickBase::compute_extrapolation_matrix(void)
  {
    const double sqrt3= sqrt(3);
    const double A= (5.0+sqrt3)/4.0;
    const double B= -(sqrt3+1)/4.0;
    const double C= (sqrt3-1)/4.0;
    const double D= (5-sqrt3)/4.0;
    static Matrix retval(8,8); // 8 nodes 8 gauss points
    // Fill in transformation matrix
    retval(0,0)=A; retval(0,1)=B; retval(0,2)=C; retval(0,3)=B; retval(0,4)=B; retval(0,5)=C; retval(0,6)=D; retval(0,7)=C;
    retval(1,0)=B; retval(1,1)=A; retval(1,2)=B; retval(1,3)=C; retval(1,4)=C; retval(1,5)=B; retval(1,6)=C; retval(1,7)=D;
    retval(2,0)=C; retval(2,1)=B; retval(2,2)=A; retval(2,3)=B; retval(2,4)=D; retval(2,5)=C; retval(2,6)=B; retval(2,7)=C;
    retval(3,0)=B; retval(3,1)=C; retval(3,2)=B; retval(3,3)=A; retval(3,4)=C; retval(3,5)=D; retval(3,6)=C; retval(3,7)=B;
    retval(4,0)=B; retval(4,1)=C; retval(4,2)=D; retval(4,3)=C; retval(4,4)=A; retval(4,5)=B; retval(4,6)=C; retval(4,7)=B;
    retval(5,0)=C; retval(5,1)=B; retval(5,2)=C; retval(5,3)=D; retval(5,4)=B; retval(5,5)=A; retval(5,6)=B; retval(5,7)=C;
    retval(6,0)=D; retval(6,1)=C; retval(6,2)=B; retval(6,3)=C; retval(6,4)=C; retval(6,5)=B; retval(6,6)=A; retval(6,7)=B;
    retval(7,0)=C; retval(7,1)=D; retval(7,2)=C; retval(7,3)=B; retval(7,4)=B; retval(7,5)=C; retval(7,6)=B; retval(7,7)=A;
    return retval;
  }

//! @brief Return the matrix that can be used to extrapolate
//! the results from the Gauss points to the element nodes.
//!
//! Reference: INTRODUCTION TO THE FINITE ELEMENT METHOD
//! G. P. Nikishkov
//! 2004 Lecture Notes. University of Aizu, Aizu-Wakamatsu
//! 965-8580, Japanniki@u-aizu.ac.jp section 4.3.7
const XC::Matrix &XC::BrickBase::getExtrapolationMatrix(void) const
  {
    static const Matrix retval= compute_extrapolation_matrix();
    return retval;
  }

// Taken from the file trilinearintepolator.c
// created by Dirk-Philip van Herwaarden on 4/21/17.

////////////////////// Linear Algebra Helper Functions ///////////////////////////
// Transpose of 3 by 3 matrix
void transpose(double m[3][3], double mTranspose[3][3])
  {
    for(int i=0;i<3;i++)
      {
        for(int j=0;j<3; j++)
            mTranspose[j][i] = m[i][j];
      }
  }

//determinant of a 3 by 3 matrix
double determinant(double m[3][3])
{
    double det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
                 m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
                 m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    return det;
}

// Inverse of a 3 by 3 matrix
void mInverse(double m[3][3], double minv[3][3], double det)
{
    double invdet = 1 / det;
    minv[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invdet;
    minv[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invdet;
    minv[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invdet;
    minv[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invdet;
    minv[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invdet;
    minv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invdet;
    minv[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invdet;
    minv[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invdet;
    minv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invdet;
}

// Dot product of Dn and xl to compute jacobian
void dot_product_matrix_matrix(double Dn[3][8], double xl[3][8], double jac[3][3])
  {
    double sum= 0.0;
    for(int q= 0;q<3; q++)
      {
        for(int j=0;j<3;j++)
          {
            sum= 0.0;
            for(int i= 0;i<8;i++)
                sum+= Dn[q][i] * xl[j][i];
            jac[q][j] = sum;
          }
      }
  }

// dot product of jac_t and objective function to get update to solution
void dot_product_matrix_vector(double Jac_transpose[3][3],
                               double objective_function[3], double solution[3])
  {
    double sum= 0.0;
    for(int i=0;i<3; i++)
      {
        sum= 0.0;
        for(int j=0;j<3;j++)
          sum+= Jac_transpose[i][j] * objective_function[j];
        solution[i] = sum;
      }
  }

double referenceToElementMapping(double v0, double v1, double v2, double v3,
                                 double v4, double v5, double v6, double v7,
                                 double r, double s, double t)
 {
    return v0 + 0.5 * (r + 1.0) * (-v0 + v3) +
                0.5 * (s + 1.0) * (-v0 + v1 - 0.5 * (r + 1.0) * (-v0 + v3) +
                0.5 * (r + 1.0) * (-v1 + v2)) +
                0.5 * (t + 1.0) * (-v0 + v4 - 0.5 * (r + 1.0) * (-v0 + v3) +
                0.5 * (r + 1.0) * (-v4 + v5) -
                0.5 * (s + 1.0) * (-v0 + v1 - 0.5 * (r + 1.0) * (-v0 + v3) +
                0.5 * (r + 1.0) * (-v1 + v2)) +
                0.5 * (s + 1.0) * (-v4 + v7 - 0.5 * (r + 1.0) * (-v4 + v5) +
                0.5 * (r + 1.0) * (v6 - v7)));
  }

// Taken from the file trilinearintepolator.c
// created by Dirk-Philip van Herwaarden on 4/21/17.
// https://github.com/eth-csem/csemlibcsemlib/src/trilinearinterpolator.c

void coordinateTransform(double pnt[], double xl[3][8], double solution[])
  {
    
    solution[0]= referenceToElementMapping(xl[0][0], xl[0][1], xl[0][2], xl[0][3], xl[0][4],
                                           xl[0][5], xl[0][6], xl[0][7], pnt[0], pnt[1], pnt[2]);
    solution[1]= referenceToElementMapping(xl[1][0], xl[1][1], xl[1][2], xl[1][3], xl[1][4],
                                           xl[1][5], xl[1][6], xl[1][7], pnt[0], pnt[1], pnt[2]);
    solution[2]= referenceToElementMapping(xl[2][0], xl[2][1], xl[2][2], xl[2][3], xl[2][4],
                                           xl[2][5], xl[2][6], xl[2][7], pnt[0], pnt[1], pnt[2]);
  }

void interpolateAtPoint(double pnt[], double interpolator[])
  {
    double r, s, t;
    r = pnt[0];
    s = pnt[1];
    t = pnt[2];

    interpolator[0] = -0.125 * r * s * t + 0.125 * r * s + 0.125 * r * t - 0.125 * r + \
		      0.125 * s * t - 0.125 * s - 0.125 * t + 0.125;
    interpolator[1] = +0.125 * r * s * t - 0.125 * r * s + 0.125 * r * t - 0.125 * r - \
		      0.125 * s * t + 0.125 * s - 0.125 * t + 0.125;
    interpolator[2] = -0.125 * r * s * t + 0.125 * r * s - 0.125 * r * t + 0.125 * r - \
		      0.125 * s * t + 0.125 * s - 0.125 * t + 0.125;
    interpolator[3] = +0.125 * r * s * t - 0.125 * r * s - 0.125 * r * t + 0.125 * r + \
		      0.125 * s * t - 0.125 * s - 0.125 * t + 0.125;
    interpolator[4] = +0.125 * r * s * t + 0.125 * r * s - 0.125 * r * t - 0.125 * r - \
		      0.125 * s * t - 0.125 * s + 0.125 * t + 0.125;
    interpolator[5] = -0.125 * r * s * t - 0.125 * r * s + 0.125 * r * t + 0.125 * r - \
		      0.125 * s * t - 0.125 * s + 0.125 * t + 0.125;
    interpolator[6] = +0.125 * r * s * t + 0.125 * r * s + 0.125 * r * t + 0.125 * r + \
		      0.125 * s * t + 0.125 * s + 0.125 * t + 0.125;
    interpolator[7] = -0.125 * r * s * t - 0.125 * r * s - 0.125 * r * t - 0.125 * r + \
		      0.125 * s * t + 0.125 * s + 0.125 * t + 0.125;
  }
double dNdR(int N, double S, double T)
  {
    return 0.125 * XC::BrickBase::mNodesR[N] * (S * XC::BrickBase::mNodesS[N] + 1) * (T * XC::BrickBase::mNodesT[N] + 1);
  }

double dNdS(int N, double R, double T)
  {
    return 0.125 * XC::BrickBase::mNodesS[N] * (R * XC::BrickBase::mNodesR[N] + 1) * (T * XC::BrickBase::mNodesT[N] + 1);
  }

double dNdT(int N, double R, double S)
  {
    return 0.125 * XC::BrickBase::mNodesT[N] * (R * XC::BrickBase::mNodesR[N] + 1) * (S * XC::BrickBase::mNodesS[N] + 1);
  }

// Computes inverse jacobian
void inverseJacobianAtPoint(double pnt[3], double xl[3][8], double invJac[3][3])
  {

      // Initializing variables
      const double &R= pnt[0];
      const double &S= pnt[1];
      const double &T= pnt[2];
      double det = 0;
      double Dn[XC::BrickBase::ndm][XC::BrickBase::numberNodes];
      double jac[3][3];

      for(int J= 0;J< XC::BrickBase::numberNodes;J++)
        {
	  for(int I= 0;I<XC::BrickBase::ndm; I++)
	    {
	      if (I == 0)
		  Dn[I][J] = dNdR(J, S, T);
	      else if (I == 1)
		  Dn[I][J] = dNdS(J, R, T);
	      else if (I == 2)
		  Dn[I][J] = dNdT(J, R, S);
	    }
        }
      dot_product_matrix_matrix(Dn, xl, jac); //places product into jac
      det = determinant(jac);                  //computes determinant of jac
      mInverse(jac, invJac, det);              //computes inverse of jac and places into invJac
  }

// Gets reference coordinates for pnt in xl and stores them in solution
int inverseCoordinateTransform(double pnt[3], double xl[3][8], double solution[3])
  {
    double scalexy;
    double scale;
    int max_iter = 15;
    double tol;
    int num_iter = 0;
    double update[3];
    double jacobian_inverse[3][3];
    double jacobian_inverse_t[3][3];
    double T[3];
    double objective_function[3];
    int i;

    // Initialize solution with zeros
    for (i = 0; i < XC::BrickBase::ndm;i++)
        solution[i] = 0;

    scalexy = fabs((xl[0][1] - xl[0][0])) > fabs((xl[1][1] - xl[1][0])) ?
              fabs(xl[0][1] - xl[0][0]) : fabs(xl[1][1] - xl[1][0]);
    scale = fabs((xl[2][1] - xl[2][0])) > scalexy ? fabs(xl[2][1] - xl[2][0]) : scalexy;

    tol = 1e-10 * scale;
    while(num_iter < max_iter)
      {
        coordinateTransform(solution, xl, T);

        for (i = 0; i < XC::BrickBase::ndm;i++)
            objective_function[i] = pnt[i] - T[i];

        if ((fabs(objective_function[0]) < tol) && (fabs(objective_function[1]) < tol)
                && (fabs(objective_function[0]) < tol))
            return 1;
        else
          {
            inverseJacobianAtPoint(solution, xl, jacobian_inverse);  //compute inverse of jacobian
            transpose(jacobian_inverse, jacobian_inverse_t);
            dot_product_matrix_vector(jacobian_inverse_t, objective_function, update);

            for (i = 0; i < XC::BrickBase::ndm;i++)
                solution[i] = solution[i] + update[i];
          }
        num_iter = num_iter + 1;
      }
    return 0;
  }

//! @brief compute local coordinates and basis
void XC::BrickBase::computeBasis(void) const
  {
    //nodal coordinates
    for(int i = 0; i < 8; i++ )
      {
         const Vector &coorI= theNodes[i]->getCrds();
         xl[0][i] = coorI(0);
         xl[1][i] = coorI(1);
         xl[2][i] = coorI(2);
      }  //end for i
  }

//! @brief Return the element local axes.
XC::Matrix XC::BrickBase::getLocalAxes(bool initialGeometry) const
  {
    Matrix retval(3,3);
    if(!initialGeometry)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; for deformed geometry not implemented."
                << std::endl;
    
    const Vector &coor1= theNodes[1]->getCrds();
    const Vector &coor2= theNodes[2]->getCrds();
    const Vector &coor3= theNodes[3]->getCrds();
    const Vector &coor4= theNodes[4]->getCrds();
    const Vector &coor5= theNodes[5]->getCrds();
    const Vector &coor6= theNodes[6]->getCrds();
    const Vector &coor7= theNodes[7]->getCrds();

    const Vector pR= (coor1+coor2+coor6+coor5)*0.25; //R face.
    const Vector pS= (coor2+coor3+coor7+coor6)*0.25; //S face.
    const Vector pT= (coor4+coor5+coor6+coor7)*0.25; //T face.
    const Vector vCenter= getCenterOfMassCoordinates();

    const Vector r= pR-vCenter;
    const Vector s= pS-vCenter;
    const Vector t= pT-vCenter;

    // Fill in matrix
    retval(0,0)= r(0); retval(0,1)= r(1); retval(0,2)= r(2);
    retval(1,0)= s(0); retval(1,1)= s(1); retval(1,2)= s(2);
    retval(2,0)= t(0); retval(2,1)= t(1); retval(2,2)= t(2);
    return retval;
  }

Pos3d XC::BrickBase::getGlobalCoordinates(const double &r, const double &s, const double &t) const
  {
    Pos3d retval;
    double pnt[ndm]= {r,s,t};
    double solution[ndm]= {0.0,0.0,0.0};
    computeBasis();
    coordinateTransform(pnt,xl,solution);
    retval= Pos3d(solution[0],solution[1],solution[2]);
    return retval;
  }
  
XC::ParticlePos3d XC::BrickBase::getNaturalCoordinates(const Pos3d &pos) const
  {
    ParticlePos3d retval;
    double pnt[ndm]= {pos.x(),pos.y(),pos.z()};
    double solution[ndm]= {0.0,0.0,0.0}; // {r,s,t}
    computeBasis();
    if(inverseCoordinateTransform(pnt, xl, solution))
      retval= ParticlePos3d(solution[0],solution[1],solution[2]);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; natural coordinates of point: "
                << pos << " not found." << std::endl;
    return retval;
  }
