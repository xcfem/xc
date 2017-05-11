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
//Element0D.cc

#include "Element0D.h"
#include <domain/mesh/node/Node.h>
#include "preprocessor/cad/matrices/TritrizPtrNod.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "preprocessor/set_mgmt/SetEstruct.h"

#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/sis_coo/SisCooRect3d3d.h"
#include "utility/actor/actor/MovableMatrix.h"
#include "med.h"
#include "vtkCellType.h"

const double XC::Element0D::LenTol= 1.0e-6; // Tolerance for zero length of element

XC::Element0D::Vxy::Vxy(void)
  : x(3),y(3)
  {
    x(0)= 1.0;
    y(1)= 1.0;
  }
 
XC::Element0D::Vxy::Vxy(const Vector &a,const Vector &b)
  : x(a),y(b) {}

//! @brief Chequea los vectores.
bool XC::Element0D::Vxy::check(void) const
  {
    bool retval= true;
    const double nx= x.Norm();
    if(nx<1e-6)
      {
        std::cerr << "Element0D::check; vector I= " << x
                  << " has a very small modulus: "
                  << nx << std::endl;
        retval= false;
      }
    const double ny= y.Norm();
    if(ny<1e-6)
      {
        std::cerr << "Element0D::check; vector J= " << y
                  << " has a very small modulus: "
                  << ny << std::endl;
        retval= false;
      }
    Vector3d vx(x[0],x[1],x[2]);
    Vector3d vy(y[0],y[1],y[2]);
    if(paralelos(vx,vy))
      {
        std::cerr << "Element0D::check; vector I= " << x
                  << " and J= " << y << " are almost parallel, angle: "
                  << angulo(vx,vy) << " radians." << std::endl;
        retval= false;
      }
    return retval;
  }

//! Constructor.
XC::Element0D::Element0D(int tag, int classTag,int Nd1,int Nd2)
  : ElementBase<2>(tag,classTag),dimension(1), numDOF(0), transformation(3,3)
  {
    theNodes.set_id_nodes(Nd1,Nd2);
    Vxy vxy; //Default vectors.
    setUp(Nd1,Nd2,vxy.getX(),vxy.getY());
  }

//! Constructor.
XC::Element0D::Element0D(int tag, int classTag,int Nd1,int Nd2,int dim)
  : ElementBase<2>(tag,classTag),dimension(dim), numDOF(0), transformation(3,3)
  {
    theNodes.set_id_nodes(Nd1,Nd2);
    Vxy vxy; //Default vectors.
    setUp(Nd1,Nd2,vxy.getX(),vxy.getY());
  }

//! Constructor.
XC::Element0D::Element0D(int tag, int classTag,int Nd1,int Nd2,int dim, const Vector& x, const Vector& yprime)
  : ElementBase<2>(tag,classTag),dimension(dim), numDOF(0), transformation(3,3)
  {
    theNodes.set_id_nodes(Nd1,Nd2);
    setUp(Nd1,Nd2,x,yprime);
  }

//! @brief Return the element dimension (0, 1, 2 o3 3).
size_t XC::Element0D::getDimension(void) const
  { return 0; }

int XC::Element0D::getNumDOF(void) const
  { return numDOF; }

//! @brief Returns the direction vector of local X axis (first row of the transformation).
const XC::Vector &XC::Element0D::getX(void) const
  {
    static Vector retval(3);
    retval(0)= transformation(0,0);
    retval(1)= transformation(0,1);
    retval(2)= transformation(0,2);
    return retval;
  }

//! @brief Returns the direction vector of local Y axis (second row of the transformation).
const XC::Vector &XC::Element0D::getY(void) const
  {
    static Vector retval(3);
    retval(0)= transformation(1,0);
    retval(1)= transformation(1,1);
    retval(2)= transformation(1,2);
    return retval;
  }

//! @brief Returns the direction vector of local Z axis (third row of the transformation).
const XC::Vector &XC::Element0D::getZ(void) const
  {
    static Vector retval(3);
    retval(0)= transformation(2,0);
    retval(1)= transformation(2,1);
    retval(2)= transformation(2,2);
    return retval;
  }

//! @brief Returs a matrix with the axes of the element as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
XC::Matrix XC::Element0D::getLocalAxes(bool initialGeometry) const
  {
    if(!initialGeometry)
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "not implemented for deformed geometry."
                << std::endl;
    return transformation;
  }

//! @brief Returns the element coordinate system.
SisCooRect3d3d XC::Element0D::getSisCoo(bool initialGeometry) const
  {
    if(!initialGeometry)
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "not implemented for deformed geometry."
                << std::endl;
    const Vector3d x(transformation(0,0), transformation(0,1), transformation(0,2));
    const Vector3d y(transformation(1,0), transformation(1,1), transformation(1,2));
    const Vector3d z(transformation(2,0), transformation(2,1), transformation(2,2));
    return SisCooRect3d3d(x,y,z);
  }    


//! @brief Set up the transformation matrix for orientation
//! @param x: vector components in global coordinates defining local x-axis (vector x)
//! @param y: vector components in global coordinates defining vector yp which lies in the local x-y plane for the element. The local z-axis is defined by the cross product between the vectors x and yp
void XC::Element0D::setUpVectors(const Vector &x, const Vector &yp)
  { 
    // check that vectors for orientation are correct size
    if(x.Size() != 3 || yp.Size() != 3 )
      std::cerr << "Element0D::setUp -- incorrect dimension of orientation vectors\n";
			
    // establish orientation of element for the transformation matrix
    // z = x cross yp
    static Vector z(3);
    z(0)= x(1)*yp(2) - x(2)*yp(1);
    z(1)= x(2)*yp(0) - x(0)*yp(2);
    z(2)= x(0)*yp(1) - x(1)*yp(0);

    // y = z cross x
    static Vector y(3);
    y(0)= z(1)*x(2) - z(2)*x(1);
    y(1)= z(2)*x(0) - z(0)*x(2);
    y(2)= z(0)*x(1) - z(1)*x(0);

    // compute length(norm) of vectors
    const double xn= x.Norm();
    const double yn= y.Norm();
    const double zn= z.Norm();

    // check valid x and y vectors, i.e. not parallel and of zero length
    if(xn == 0 || yn == 0 || zn == 0)
      std::cerr << "Element0D::setUp -- invalid vectors to constructor\n";
    
    // create transformation matrix of direction cosines
    for(int i= 0;i<3;i++)
      {
	transformation(0,i)= x(i)/xn;
	transformation(1,i)= y(i)/yn;
	transformation(2,i)= z(i)/zn;
      }
  }

//! @brief Establish the external nodes and set up the transformation matrix for orientation
//! @param Nd1: Tag del nodo dorsal.
//! @param Nd2: Tag del nodo frontal.
//! @param x: Componentes del eje x local, expresadas en coordenadas globales.
//! @param y: Componentes de un vector contenido en el plano xy local, expresadas en coordenadas globales.
void XC::Element0D::setUp(int Nd1, int Nd2, const Vector &x, const Vector &yp)
  { 
    // ensure the connectedExternalNode ID is of correct size & set values
    if(theNodes.size() != 2)
      {
	std::cerr << "Element0D::setUp -- failed to create an ID of correct size\n";
        exit(-1);
      }
    
    theNodes.set_id_nodes(Nd1,Nd2);
    theNodes.inic();
    setUpVectors(x,yp);
  }

XC::TritrizPtrElem XC::Element0D::cose(const SetEstruct &f1,const SetEstruct &f2) const
  {
    const size_t ncapas= f1.getNumNodeLayers();
    const size_t nfilas= f1.getNumNodeRows();
    const size_t ncols= f1.getNumNodeColumns();
    TritrizPtrElem retval(ncapas,nfilas,ncols);
    const size_t dimf1= f1.Dimension();
    const size_t dimf2= f2.Dimension();
    if(dimf1!=dimf2)
      {
	std::cerr << "the sets: " << f1.GetNombre() << " y " << f2.GetNombre()
                  << " have different dimension (" << dimf1  << " y " << dimf2
                  << ")." << std::endl;
        return retval;
      }
    if(dimf1==1)
      {
        if(f1.EsFilaI())
          {
            if(f2.EsFilaI())
              {
                const size_t n= std::min(ncapas,f2.getNumNodeLayers());
                for(size_t i=1;i<=n;i++)
                  {
                    Element *tmp= getCopy();
                    const int Nd1= f1.GetNodo(i,1,1)->getTag();
                    const int Nd2= f2.GetNodo(i,1,1)->getTag();
                    tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
                    retval(i,1,1)= tmp;
                  }
              }
            if(f2.EsFilaJ())
              {
                const size_t n= std::min(ncapas,f2.getNumNodeRows());
                for(size_t i=1;i<=n;i++)
                  {
                    Element *tmp= getCopy();
                    const int Nd1= f1.GetNodo(i,1,1)->getTag();
                    const int Nd2= f2.GetNodo(1,i,1)->getTag();
                    tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
                    retval(i,1,1)= tmp;
                  }
              }
            if(f2.EsFilaK())
              {
                const size_t n= std::min(ncapas,f2.getNumNodeColumns());
                for(size_t i=1;i<=n;i++)
                  {
                    Element *tmp= getCopy();
                    const int Nd1= f1.GetNodo(i,1,1)->getTag();
                    const int Nd2= f2.GetNodo(1,1,i)->getTag();
                    tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
                    retval(i,1,1)= tmp;
                  }
              }
          }
        if(f1.EsFilaJ())
          {
            if(f2.EsFilaI())
              {
                const size_t n= std::min(nfilas,f2.getNumNodeLayers());
                for(size_t i=1;i<=n;i++)
                  {
                    Element *tmp= getCopy();
                    const int Nd1= f1.GetNodo(1,i,1)->getTag();
                    const int Nd2= f2.GetNodo(i,1,1)->getTag();
                    tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
                    retval(1,i,1)= tmp;
                  }
              }
            if(f2.EsFilaJ())
              {
                const size_t n= std::min(nfilas,f2.getNumNodeRows());
                for(size_t i=1;i<=n;i++)
                  {
                    Element *tmp= getCopy();
                    const int Nd1= f1.GetNodo(1,i,1)->getTag();
                    const int Nd2= f2.GetNodo(1,i,1)->getTag();
                    tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
                    retval(1,i,1)= tmp;
                  }
              }
            if(f2.EsFilaK())
              {
                const size_t n= std::min(nfilas,f2.getNumNodeColumns());
                for(size_t i=1;i<=n;i++)
                  {
                    Element *tmp= getCopy();
                    const int Nd1= f1.GetNodo(1,i,1)->getTag();
                    const int Nd2= f2.GetNodo(1,1,i)->getTag();
                    tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
                    retval(1,i,1)= tmp;
                  }
              }
          }
        if(f1.EsFilaK())
          {
            if(f2.EsFilaI())
              {
                const size_t n= std::min(ncols,f2.getNumNodeLayers());
                for(size_t i=1;i<=n;i++)
                  {
                    Element *tmp= getCopy();
                    const int Nd1= f1.GetNodo(1,1,i)->getTag();
                    const int Nd2= f2.GetNodo(i,1,1)->getTag();
                    tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
                    retval(1,1,i)= tmp;
                  }
              }
            if(f2.EsFilaJ())
              {
                const size_t n= std::min(ncols,f2.getNumNodeRows());
                for(size_t i=1;i<=n;i++)
                  {
                    Element *tmp= getCopy();
                    const int Nd1= f1.GetNodo(1,1,i)->getTag();
                    const int Nd2= f2.GetNodo(1,i,1)->getTag();
                    tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
                    retval(1,1,i)= tmp;
                  }
              }
            if(f2.EsFilaK())
              {
                const size_t n= std::min(ncols,f2.getNumNodeColumns());
                for(size_t i=1;i<=n;i++)
                  {
                    Element *tmp= getCopy();
                    const int Nd1= f1.GetNodo(1,1,i)->getTag();
                    const int Nd2= f2.GetNodo(1,1,i)->getTag();
                    tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
                    retval(1,1,i)= tmp;
                  }
              }
          }
      }
    if(dimf1>1)
      {
	std::cerr << "Sewing in 2 or 3 dimensions not implemented." << std::endl;
      }
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::Element0D::sendData(CommParameters &cp)
  {
    int res= ElementBase<2>::sendData(cp);
    res+= cp.sendInts(dimension,numDOF,getDbTagData(),CommMetaData(7));
    res+= cp.sendMatrix(transformation,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::Element0D::recvData(const CommParameters &cp)
  {
    int res= ElementBase<2>::recvData(cp);
    res+= cp.receiveInts(dimension,numDOF,getDbTagData(),CommMetaData(7));
    res+= cp.receiveMatrix(transformation,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Interfaz con VTK.
int XC::Element0D::getVtkCellType(void) const
  { return VTK_VERTEX; }

//! @brief Interfaz con el formato MED de Salome.
int XC::Element0D::getMEDCellType(void) const
  { return MED_SEG2; }

