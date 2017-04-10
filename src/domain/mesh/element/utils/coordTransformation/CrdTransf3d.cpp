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

// $Revision: 1.2 $
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/CrdTransf3d.cpp,v $


// File: ~/crdTransf/CrdTransf3d.C
//
// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the implementation for the XC::CrdTransf3d class.
// CrdTransf3d provides the abstraction of a 3d frame 
// coordinate transformation. It is an abstract base class and 
// thus no objects of  it's type can be instatiated. 

#include "CrdTransf3d.h"
#include <utility/matrix/Vector.h>
#include <domain/mesh/node/Node.h>

#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/sis_ref/Ref3d3d.h"
#include "utility/actor/actor/MovableMatrix.h"
#include "xc_basic/src/matrices/giros.h"

XC::Vector XC::CrdTransf3d::vectorI(3);
XC::Vector XC::CrdTransf3d::vectorJ(3);
XC::Vector XC::CrdTransf3d::vectorK(3);
XC::Vector XC::CrdTransf3d::vectorCoo(3);

//! @brief Asigna el valor del vector contenido en el plano XZ local.
void XC::CrdTransf3d::set_xz_vector(const XC::Vector &vecInLocXZPlane)
  {
    for(register int i = 0; i < 2; i++)
      for(register int j = 0; j < 3; j++)
        R(i,j) = 0.0;
    R(2,0) = vecInLocXZPlane(0);
    R(2,1) = vecInLocXZPlane(1);
    R(2,2) = vecInLocXZPlane(2);
  }

//! @brief Returns the vector contenido en el plano XZ local.
XC::Vector XC::CrdTransf3d::get_xz_vector(void) const
  {
    Vector retval(3);
    retval(0)= R(2,0);
    retval(1)= R(2,1);
    retval(2)= R(2,2);
    return retval;
  }

//! brief Rellena el vector de desplazamiento globales of the nodes.
void XC::CrdTransf3d::inic_ug(const Vector &d1,const Vector &d2,double *ug)
  {
    for(register int i=0;i<6;i++) //Desplazamiento obtenido del cálculo.
      {
        ug[i]   = d1(i); //Nodo dorsal.
        ug[i+6] = d2(i); //Nodo frontal.
      }
  }

//! brief Modifical el vector de desplazamiento globales of the nodes
//! de acuerdo con los desplazamientos iniciales.
void XC::CrdTransf3d::modif_ug_init_disp(double *ug) const
  {
    if(!nodeIInitialDisp.empty())
      {
        for(int j=0;j<6;j++)
          ug[j]-= nodeIInitialDisp[j];
      }
    
    if(!nodeJInitialDisp.empty())
      {
        for(int j=0;j<6;j++)
          ug[j+6]-= nodeJInitialDisp[j];
      }
  }

//! @brief Returns node displacements expressed in local coordinates.
void XC::CrdTransf3d::global_to_local(const double *ug,double *ul) const
  {
    ul[0]  = R(0,0)*ug[0] + R(0,1)*ug[1] + R(0,2)*ug[2];
    ul[1]  = R(1,0)*ug[0] + R(1,1)*ug[1] + R(1,2)*ug[2];
    ul[2]  = R(2,0)*ug[0] + R(2,1)*ug[1] + R(2,2)*ug[2];

    ul[3]  = R(0,0)*ug[3] + R(0,1)*ug[4] + R(0,2)*ug[5];
    ul[4]  = R(1,0)*ug[3] + R(1,1)*ug[4] + R(1,2)*ug[5];
    ul[5]  = R(2,0)*ug[3] + R(2,1)*ug[4] + R(2,2)*ug[5];

    ul[6]  = R(0,0)*ug[6] + R(0,1)*ug[7] + R(0,2)*ug[8];
    ul[7]  = R(1,0)*ug[6] + R(1,1)*ug[7] + R(1,2)*ug[8];
    ul[8]  = R(2,0)*ug[6] + R(2,1)*ug[7] + R(2,2)*ug[8];

    ul[9]  = R(0,0)*ug[9] + R(0,1)*ug[10] + R(0,2)*ug[11];
    ul[10] = R(1,0)*ug[9] + R(1,1)*ug[10] + R(1,2)*ug[11];
    ul[11] = R(2,0)*ug[9] + R(2,1)*ug[10] + R(2,2)*ug[11];
  }

void XC::CrdTransf3d::calc_Wu(const double *ug,double *ul,double *Wu) const
  {
    Wu[0] =  nodeIOffset(2)*ug[4] - nodeIOffset(1)*ug[5];
    Wu[1] = -nodeIOffset(2)*ug[3] + nodeIOffset(0)*ug[5];
    Wu[2] =  nodeIOffset(1)*ug[3] - nodeIOffset(0)*ug[4];

    ul[0] += R(0,0)*Wu[0] + R(0,1)*Wu[1] + R(0,2)*Wu[2];
    ul[1] += R(1,0)*Wu[0] + R(1,1)*Wu[1] + R(1,2)*Wu[2];
    ul[2] += R(2,0)*Wu[0] + R(2,1)*Wu[1] + R(2,2)*Wu[2];

    Wu[0] =  nodeJOffset(2)*ug[10] - nodeJOffset(1)*ug[11];
    Wu[1] = -nodeJOffset(2)*ug[9]  + nodeJOffset(0)*ug[11];
    Wu[2] =  nodeJOffset(1)*ug[9]  - nodeJOffset(0)*ug[10];

    ul[6] += R(0,0)*Wu[0] + R(0,1)*Wu[1] + R(0,2)*Wu[2];
    ul[7] += R(1,0)*Wu[0] + R(1,1)*Wu[1] + R(1,2)*Wu[2];
    ul[8] += R(2,0)*Wu[0] + R(2,1)*Wu[1] + R(2,2)*Wu[2];
  }

//! Sean dx1,dy1,dz1,gx1,gy1,gz1 los desplazamientos y giros del nodo dorsal y dx2,dy2,dz2,gx2,gy2,gz2
//! los del nodo frontal expresados en local coordinates. This function gets the following displacements
//! on the ul parameter and returns the following magnitudes on the ub vector:
//! -ub(0)= dx2-dx1: element elongation.
//! -ub(1)= (dy1-dy2)/L+gz1: rotation of node 1 about z axis.
//! -ub(2)= (dy1-dy2)/L+gz2: rotation of node 2 about z axis.
//! -ub(3)= (dz2-dz1)/L+gy1: rotation of node 1 about y axis.
//! -ub(4)= (dz2-dz1)/L+gy2: rotation of node 2 about y axis.
//! -ub(5)= dx2-dx1: element torsion.
const XC::Vector &XC::CrdTransf3d::calc_ub(const double *ul,Vector &ub) const
  {
    const double oneOverL = 1.0/L;
    ub(0) = ul[6]-ul[0]; //Length increment.
    double tmp= oneOverL*(ul[1]-ul[7]); //Rotation of the element about z axis.
    ub(1) = ul[5] + tmp; //Rotation of node 1 about z axis.
    ub(2) = ul[11] + tmp; //Rotation of node 2 about z axis.
    tmp = oneOverL*(ul[8]-ul[2]); //Rotation of element about y axis.
    ub(3) = ul[4] + tmp; //Rotation of node 1 about y axis.
    ub(4) = ul[10] + tmp; //Rotation of node 2 about y axis.
    ub(5) = ul[9] - ul[3]; //Twist.

    return ub;
  }

//! @brief Default constructor
XC::CrdTransf3d::CrdTransf3d(int tag, int classTag)
  : CrdTransf(tag, classTag,3), R(3,3)
  {
    for(int i = 0; i < 3; i++)
      for(int j = 0; j < 3; j++)
        R(i,j) = 0.0;
  }

//! @brief Constructor:
XC::CrdTransf3d::CrdTransf3d(int tag, int class_tag, const Vector &vecInLocXZPlane)
  : CrdTransf(tag, class_tag,3)
  { set_xz_vector(vecInLocXZPlane); }

//! @brief check rigid joint offset for node I
void XC::CrdTransf3d::set_rigid_joint_offsetI(const Vector &rigJntOffset1)
  {
    if(&rigJntOffset1 == 0 || rigJntOffset1.Size() != 3 )
      {
        std::cerr << "CrdTransf3d:  Invalid rigid joint offset vector for node I\n";
        std::cerr << "Size must be 3\n";      
      }
    else
      if(rigJntOffset1.Norm() > 0.0)
        {
          nodeIOffset(0) = rigJntOffset1(0);
          nodeIOffset(1) = rigJntOffset1(1);
          nodeIOffset(2) = rigJntOffset1(2);
        }
  }

//! @brief check rigid joint offset for node J
void XC::CrdTransf3d::set_rigid_joint_offsetJ(const Vector &rigJntOffset2)
  {
    if(&rigJntOffset2 == 0 || rigJntOffset2.Size() != 3 )
      {
        std::cerr << "XC::CrdTransf3d::CrdTransf3d:  Invalid rigid joint offset vector for node J\n";
        std::cerr << "Size must be 3\n";      
      }
    else
      if(rigJntOffset2.Norm() > 0.0)
        {
          nodeJOffset(0) = rigJntOffset2(0);
          nodeJOffset(1) = rigJntOffset2(1);
          nodeJOffset(2) = rigJntOffset2(2);
        }
  }

int XC::CrdTransf3d::initialize(Node *nodeIPointer, Node *nodeJPointer)
  {
    int error;
    set_node_ptrs(nodeIPointer,nodeJPointer);

    // Chequea si existe algún desplazamiento inicial of the nodes.
    if(initialDispChecked == false)
      {
        const Vector &nodeIDisp = nodeIPtr->getDisp();
        const Vector &nodeJDisp = nodeJPtr->getDisp();
        for(int i=0;i<6;i++)
          if(nodeIDisp(i)!=0.0) //Existe desplazamiento del nodo I.
            {
              nodeIInitialDisp.resize(6);
              for(int j=0;j<6;j++)
                nodeIInitialDisp[j]= nodeIDisp(j); //Asignamos desplazamientos iniciales.
              i= 6; //salimos.
            }

        for(int j=0; j<6; j++)
          if(nodeJDisp(j) != 0.0)
            {
              nodeJInitialDisp.resize(6);
              for(int i=0; i<6; i++)
                nodeJInitialDisp[i] = nodeJDisp(i); //Asignamos desplazamientos iniciales.
              j = 6;
            }
        initialDispChecked = true;
      }

    // get element length and orientation
    if((error = this->computeElemtLengthAndOrient()))
      return error;

    static Vector XAxis(3);
    static Vector YAxis(3);
    static Vector ZAxis(3);

    // get 3by3 rotation matrix
    if((error = this->getLocalAxes(XAxis, YAxis, ZAxis)))
      return error;
    return 0;
  }

//! @brief Returns the $\vec{i}$ unit vector of the local axis
//! expressed in global coordinates for the current geometry.
const XC::Vector &XC::CrdTransf3d::getI(void) const
  {
    calculaEjesLocales();
    return vectorI;
  }

//! @brief Returns the $\vec{j}$ unit vector of the local axis
//! expressed in global coordinates for the current geometry.
const XC::Vector &XC::CrdTransf3d::getJ(void) const
  {
    calculaEjesLocales();
    return vectorJ;
  }

//! @brief Returns the $\vec{k}$ unit vector of the local axis
//! expressed in global coordinates for the current geometry.
const XC::Vector &XC::CrdTransf3d::getK(void) const
  {
    calculaEjesLocales();
    return vectorK;
  }

//| @brief Returns of the directions vectors of the local axis.
int XC::CrdTransf3d::getLocalAxes(Vector &XAxis, Vector &YAxis, Vector &ZAxis) const
  {
    int retval= calculaEjesLocales();
    if(retval==0)
      {
        XAxis(0)= vectorI(0); XAxis(1)= vectorI(1); XAxis(2)= vectorI(2);
        YAxis(0)= vectorJ(0); YAxis(1)= vectorJ(1); YAxis(2)= vectorJ(2);
        ZAxis(0)= vectorK(0); ZAxis(1)= vectorK(1); ZAxis(2)= vectorK(2);
      }
    return retval;
  }

//! @brief Returs a matrix with the axes of the element as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
XC::Matrix XC::CrdTransf3d::getLocalAxes(bool initialGeometry) const
  {
    Matrix retval(3,3);
    int err= calculaEjesLocales();
    if(err==0)
      {
        retval(0,0)= vectorI(0); retval(0,1)= vectorI(1); retval(0,2)= vectorI(2);
        retval(1,0)= vectorJ(0); retval(1,1)= vectorJ(1); retval(1,2)= vectorJ(2);
        retval(2,0)= vectorK(0); retval(2,1)= vectorK(1); retval(2,2)= vectorK(2);
      }
    return retval;
  }


//! @brief Returns the position of node I.
Pos3d XC::CrdTransf3d::getPosNodeI(void) const
  {
    Pos3d retval= nodeIPtr->getPosFinal3d();
    retval+= Vector3d(nodeIOffset(0),nodeIOffset(1),nodeIOffset(2));
    if(!nodeIInitialDisp.empty())
      retval-= Vector3d(nodeIInitialDisp[0],nodeIInitialDisp[1],nodeIInitialDisp[2]);
    return retval;
  }

//! @brief Return the position of node J.
Pos3d XC::CrdTransf3d::getPosNodeJ(void) const
  {
    Pos3d retval= nodeJPtr->getPosFinal3d();
    retval+= Vector3d(nodeJOffset(0),nodeJOffset(1),nodeJOffset(2));
    if(!nodeJInitialDisp.empty())
      retval-= Vector3d(nodeJInitialDisp[0],nodeJInitialDisp[1],nodeJInitialDisp[2]);
    return retval;
  }

//! @brief Returns the local reference system.
Ref3d3d XC::CrdTransf3d::getLocalReference(void) const
  {
    const Vector vI= getI();
    const Vector vJ= getJ();
    return Ref3d3d(getPosNodeI(),Vector3d(vI[0],vI[1],vI[2]),Vector3d(vJ[0],vJ[1],vJ[2]));
  }

//! @brief Returns the local coordinates del punto a partir de las globales.
XC::Vector XC::CrdTransf3d::getPointLocalCoordFromGlobal(const Vector &xg) const
  {
    Ref3d3d ref= getLocalReference();
    Pos3d pl= ref.GetPosLocal(Pos3d(xg[0],xg[1],xg[2]));
    Vector retval(3);
    retval[0]= pl.x(); retval[1]= pl.y(); retval[2]= pl.z();
    return retval;  
  }

//! @brief Returns the punto expresado en global coordinates.
const XC::Vector &XC::CrdTransf3d::getPointGlobalCoordFromBasic(const double &xi) const
  {
    static Vector local_coord(3),global_coord(3);
    local_coord.Zero();
    local_coord[0]= xi*getDeformedLength();
    global_coord= getPointGlobalCoordFromLocal(local_coord);
    return global_coord;
  }

//! @brief Returns the puntos expresados en global coordinates.
const XC::Matrix &XC::CrdTransf3d::getPointsGlobalCoordFromBasic(const Vector &basicCoords) const
  {
    static Matrix retval;
    const size_t numPts= basicCoords.Size(); //Número de puntos a transformar.
    retval.resize(numPts,3);
    Vector xg(3);
    for(size_t i= 0;i<numPts;i++)
      {
        xg= getPointGlobalCoordFromBasic(basicCoords(i));
        for(size_t j= 0;j<3;j++)
          retval(i,j)= xg[j];
      }
    return retval;
  }

//! @brief Returns the vector expressed in global coordinates.
const XC::Vector &XC::CrdTransf3d::getVectorGlobalCoordFromLocal(const Vector &localCoords) const
  {
    calculaEjesLocales(); //Actualiza la matriz R.
    // vectorCoo = Rlj'*localCoords (Multiplica el vector por R traspuesta).
    vectorCoo(0)= R(0,0)*localCoords(0) + R(1,0)*localCoords(1) + R(2,0)*localCoords(2);
    vectorCoo(1)= R(0,1)*localCoords(0) + R(1,1)*localCoords(1) + R(2,1)*localCoords(2);
    vectorCoo(2)= R(0,2)*localCoords(0) + R(1,2)*localCoords(1) + R(2,2)*localCoords(2);
    return vectorCoo;
  }

//! @brief Returns the vectores formados por las filas de la matriz
//! expresados en global coordinates y colocados en otra matriz.
const XC::Matrix &XC::CrdTransf3d::getVectorGlobalCoordFromLocal(const Matrix &localCoords) const
  {
    calculaEjesLocales(); //Actualiza la matriz R.
    static Matrix retval;
    const size_t numPts= localCoords.noRows(); //Número de vectores a transformar.
    retval.resize(numPts,3);
    for(size_t i= 0;i<numPts;i++)
      {
        // retval = Rlj'*localCoords (Multiplica el vector por R traspuesta).
        retval(i,0)= R(0,0)*localCoords(i,0) + R(1,0)*localCoords(i,1) + R(2,0)*localCoords(i,2);
        retval(i,1)= R(0,1)*localCoords(i,0) + R(1,1)*localCoords(i,1) + R(2,1)*localCoords(i,2);
        retval(i,2)= R(0,2)*localCoords(i,0) + R(1,2)*localCoords(i,1) + R(2,2)*localCoords(i,2);
      }
    return retval;
  }

//! @brief Returns the vector expresado en local coordinates.
const XC::Vector &XC::CrdTransf3d::getVectorLocalCoordFromGlobal(const Vector &globalCoords) const
  {
    calculaEjesLocales(); //Actualiza la matriz R.
    vectorCoo[0]= R(0,0)*globalCoords[0] + R(0,1)*globalCoords[1] + R(0,2)*globalCoords[2];
    vectorCoo[1]= R(1,0)*globalCoords[0] + R(1,1)*globalCoords[1] + R(1,2)*globalCoords[2];
    vectorCoo[2]= R(2,0)*globalCoords[0] + R(2,1)*globalCoords[1] + R(2,2)*globalCoords[2];
    return vectorCoo;
  }

//! @brief Returns the coordinates of the nodes.
const XC::Matrix &XC::CrdTransf3d::getCooNodos(void) const
  {
    static Matrix retval;
    retval= Matrix(2,3);

    retval(0,0)= nodeIPtr->getCrds()[0];
    retval(0,1)= nodeIPtr->getCrds()[1];
    retval(0,2)= nodeIPtr->getCrds()[2];

    retval(1,0)= nodeJPtr->getCrds()[0];
    retval(1,1)= nodeJPtr->getCrds()[1];
    retval(1,2)= nodeJPtr->getCrds()[2];

    return retval;
  }

//! @brief Returns puntos distribuidos entre los nodos extremos.
const XC::Matrix &XC::CrdTransf3d::getCooPuntos(const size_t &ndiv) const
  {
    const Pos3d p0= nodeIPtr->getPosInicial3d();
    const Pos3d p1= nodeJPtr->getPosInicial3d();
    MatrizPos3d linea(p0,p1,ndiv);
    static Matrix retval;
    retval= Matrix(ndiv+1,3);
    Pos3d tmp;
    for(size_t i= 0;i<ndiv+1;i++)
      {
        tmp= linea(i+1,1);
        retval(i,0)= tmp.x();
        retval(i,1)= tmp.y();
        retval(i,2)= tmp.z();
      }
    return retval;
  }

//! @brief Returns the punto correspondiente a la coordenada 0<=xrel<=1.
const XC::Vector &XC::CrdTransf3d::getCooPunto(const double &xrel) const
  {
    const Pos3d p0= nodeIPtr->getPosInicial3d();
    const Pos3d p1= nodeJPtr->getPosInicial3d();
    const Vector3d v= p1-p0;
    static Vector retval(3);
    const Pos3d tmp= p0+xrel*v;
    retval(0)= tmp.x();
    retval(1)= tmp.y();
    retval(2)= tmp.z();
    return retval;
  }

//! @brief Hace girar el la coordinate transformation en sentido
//! antihorario el ángulo being passed as parameter.
void XC::CrdTransf3d::gira(const double &theta)
  { set_xz_vector(m_double_to_matrix(GiroX(-theta))*get_xz_vector()); }

//! @brief Sends object members through the channel being passed as parameter.
int XC::CrdTransf3d::sendData(CommParameters &cp)
  {
    int res= CrdTransf::sendData(cp);
    res+=cp.sendMatrix(R,getDbTagData(),CommMetaData(9));
    if(res<0)
      std::cerr << "CrdTransf3d::sendData - failed to send data.\n";
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::CrdTransf3d::recvData(const CommParameters &cp)
  {
    int res= CrdTransf::recvData(cp);
    res+= cp.receiveMatrix(R,getDbTagData(),CommMetaData(9));
    if(res<0)
      std::cerr << "CrdTransf::recvData - failed to receive data.\n";
    return res;    
  }

