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
//ClosedTriangleMesh.cc

#include "ClosedTriangleMesh.h"
#include "xc_utils/src/geom/d2/Triang3dMesh.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d2/Triangulo3d.h"
#include "xc_basic/src/util/mchne_eps.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/d1/Segmento3d.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/gnu_gts/TriangleMap.h"

XC::ClosedTriangleMesh::iterator XC::ClosedTriangleMesh::begin(void) { return trihedrons.begin(); }
XC::ClosedTriangleMesh::iterator XC::ClosedTriangleMesh::end(void) { return trihedrons.end(); }
XC::ClosedTriangleMesh::const_iterator XC::ClosedTriangleMesh::begin() const { return trihedrons.begin(); }
XC::ClosedTriangleMesh::const_iterator XC::ClosedTriangleMesh::end() const { return trihedrons.end(); }
size_t XC::ClosedTriangleMesh::size(void) const { return trihedrons.size(); }

size_t XC::ClosedTriangleMesh::GetNumFacetas(void) const { return size(); }


//! @brief Default constructor.
XC::ClosedTriangleMesh::ClosedTriangleMesh(void)
  : GeomObj3d(), MovableObject(0), trihedrons(0), tol(0.0), rMax(0.0),rMin(0.0) {}

XC::ClosedTriangleMesh::ClosedTriangleMesh(const Pos3d &org,const Triang3dMesh &mll)
  : GeomObj3d(), MovableObject(0), trihedrons(mll.GetNumCaras(),Trihedron()), tol(0.0), rMax(0.0), rMin(0.0)
  {
    const size_t nf= mll.GetNumCaras();
    static const Pos3d org3d;
    if(nf<4) return;
    for(Triang3dMesh::Point_const_iterator i= mll.points_begin();i!=mll.points_end();i++)
      rMax= std::max(rMax,dist(Pos3d(*i),org3d));
    rMin= 10*rMax;
    for(Triang3dMesh::Point_const_iterator i= mll.points_begin();i!=mll.points_end();i++)
      rMin= std::min(rMin,dist(Pos3d(*i),org3d));
    rMin/=3; //Radius of the sphere inscribed on the tetrahedron which 
             //circunscribed sphere passes through
             //the point closest to the origin.
    size_t cont= 0;
    for(Triang3dMesh::Facet_const_iterator i= mll.facets_begin();i!=mll.facets_end();i++)
      {
        const Triangulo3d tf= mll.GetTrianguloCara(i);
        trihedrons[cont]= Trihedron(org,tf);
        cont++;
      }
    //const double longDiagonalBND= Abs(Bnd().Diagonal());
    tol= 0.0;//longDiagonalBND/1e6;
  }

//! @brief Copy constructor.
XC::ClosedTriangleMesh::ClosedTriangleMesh(const ClosedTriangleMesh &otro)
  : GeomObj3d(otro), MovableObject(otro), trihedrons(otro.trihedrons), tol(otro.tol), rMax(otro.rMax),rMin(otro.rMin)
  {}

//! @brief Assignment operator.
XC::ClosedTriangleMesh &XC::ClosedTriangleMesh::operator=(const ClosedTriangleMesh &otro)
  {
    GeomObj3d::operator=(otro);
    trihedrons= otro.trihedrons;
    tol= otro.tol;
    rMax= otro.rMax;
    rMin= otro.rMin;
    return *this;
  }

void XC::ClosedTriangleMesh::writeTo(const std::string &fName)
  {
    std::ofstream out(fName.c_str(), std::ios::out | std::ios::binary);
    write(out);
    out.close();
  }

//! @brief Read from file.
void XC::ClosedTriangleMesh::readFrom(const std::string &fName)
  {
    std::ifstream input(fName.c_str(), std::ios::in | std::ios::binary);
    if(input)
      {
        input.seekg(0);
        read(input);
        input.close();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; can't open file: '"
                << fName << "'\n";
  }
  

//! @brief Virtual constructor.
XC::ClosedTriangleMesh *XC::ClosedTriangleMesh::clon(void) const
  { return new ClosedTriangleMesh(*this); }

double XC::ClosedTriangleMesh::GetMax(short unsigned int i) const
  {
    double retval= NAN;
    if(size()<1) return retval;
    v_trihedrons::const_iterator j= trihedrons.begin();
    retval= j->GetMax(i);
    j++;
    for(;j!=trihedrons.end();j++)
      retval= std::max(retval,j->GetMax(i));
    return retval;
  }
double XC::ClosedTriangleMesh::GetMin(short unsigned int i) const
  {
    double retval= NAN;
    if(size()<1) return retval;
    v_trihedrons::const_iterator j= trihedrons.begin();
    retval= j->GetMin(i);
    j++;
    for(;j!=trihedrons.end();j++)
      retval= std::min(retval,j->GetMin(i));
    return retval;
  }

Pos3d XC::ClosedTriangleMesh::Cdg(void) const
  { return Pos3d(0,0,0); }
double XC::ClosedTriangleMesh::Longitud(void) const
  { return 0.0; }
double XC::ClosedTriangleMesh::Area(void) const
  { return 0.0; }
double XC::ClosedTriangleMesh::Volumen(void) const
  { return 0.0; }
double XC::ClosedTriangleMesh::Ix(void) const
  { return 0.0; }
double XC::ClosedTriangleMesh::Iy(void) const
  { return 0.0; }
double XC::ClosedTriangleMesh::Iz(void) const
  { return 0.0; }

TriangleMap XC::ClosedTriangleMesh::getTriangleMap(void) const
  {
    TriangleMap retval;
    const double tol= Bnd().Diagonal().GetModulus()/1.0e5;
    VerticesKDTree kdtree;
    VerticesMap &vertices= retval.getVertices();
    TriangleFaces &faces= retval.getFaces();
    //Vertices.
    int counter= 0;
    for(v_trihedrons::const_iterator i= trihedrons.begin();i!=trihedrons.end();i++)
      {
        const Pos3d p1= i->Vertice(1);
        int nearest1= kdtree.getNearestBallPoint(p1,tol);
        if(nearest1<0) //New vertex.
          {
            nearest1= counter;
            kdtree.insert(nearest1,p1);
            vertices.insert(nearest1,p1);
            counter++;
          }
        const Pos3d p2= i->Vertice(2);
        int nearest2= kdtree.getNearestBallPoint(p2,tol);
        if(nearest2<0) //New vertex.
          {
            nearest2= counter;
            kdtree.insert(nearest2,p2);
            vertices.insert(nearest2,p2);
            counter++;
          }
        const Pos3d p3= i->Vertice(3);
        int nearest3= kdtree.getNearestBallPoint(p3,tol);
        if(nearest3<0) //New vertex.
          {
            nearest3= counter;
            kdtree.insert(nearest3,p3);
            vertices.insert(nearest3,p3);
            counter++;
          }
        faces.Append(nearest1,nearest2,nearest3);
      }
    return retval;
  }

//! @brief Search for the trihedron that contains the point
//! being passed as parameter.
XC::ClosedTriangleMesh::const_iterator XC::ClosedTriangleMesh::findTrihedron(const Pos3d &p) const
  {
    XC::ClosedTriangleMesh::const_iterator retval= end();
    for(XC::ClosedTriangleMesh::const_iterator i= begin();i!=end();i++)
      {
        if((*i).In(p,tol))
          {
            retval= i;
            break;
          }
      }
    return retval;
  }

//! @brief Search for the trihedron that contains the point
//! being passed as parameter.
const Trihedron *XC::ClosedTriangleMesh::findTrihedronPtr(const Pos3d &p) const
  {
    const Trihedron *retval= nullptr;
    if(trihedrons.empty())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; trihedron list is emplty."
                  << std::endl;
        return retval;
      }
    else
      {
        for(XC::ClosedTriangleMesh::const_iterator i= begin();i!=end();i++)
          {
            if((*i).In(p,tol))
              {
                retval= &(*i);
                break;
              }
          }
        if(!retval) //Not found, we search the one with the nearest axis.
          {
	    ClosedTriangleMesh::const_iterator i= begin();
            const Trihedron *tr= &(*i);
            SemiRecta3d rayo(tr->Cuspide(),p);
            Recta3d axis= tr->Axis();
            double rayAxisAngle= angulo(axis,rayo);
            double angTmp= rayAxisAngle;
            retval= tr;
            i++;
            for(;i!=end();i++)
              {
                tr= &(*i);
                rayo= SemiRecta3d(tr->Cuspide(),p);
                angTmp= angulo(tr->Axis(),rayo);
                if(angTmp<rayAxisAngle)
                  {
                    rayAxisAngle= angTmp;
                    retval= tr;
                  }
              }
          }
//     if(!retval) //Not found yet, we search for a point in the same line.
//       {
//         const GEOM_FT denom= 100.0; //Abs(p.VectorPos())/100;
//         const Pos3d perturb= Pos3d(p.x()/denom,p.y()/denom,p.z()/denom); // + Vector3d(tol/1e2,tol/1e2,tol/1e2);
//         for(XC::ClosedTriangleMesh::const_iterator i= begin();i!=end();i++)
//           {
//             if((*i).In(perturb,tol))
//               {
//                 retval= &(*i);
//                 break;
//               }
//           }
//       }
      }
    return retval;
  }

//! @brief Return the intersection of triangle mesh with the ray
//! from origin (0,0,0) to p.
GeomObj::list_Pos3d XC::ClosedTriangleMesh::get_intersection(const Pos3d &p) const
  {
    GeomObj::list_Pos3d lst_intersec;
    const Pos3d O= Pos3d(0.0,0.0,0.0);
    //Search for the trihedron that contains p.
    const Trihedron *i= findTrihedronPtr(p);
    if(!i)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; bounding trihedron for :"
                  << p << " not found. Quadrant: "
		  << p.Cuadrante() << std::endl;
      }
    else
      {
        Triangulo3d triang(i->Vertice(1),i->Vertice(2),i->Vertice(3));
        //Plane of the triangle.
        const Plano3d plano= triang.GetPlano();
        SemiRecta3d Op(O,p);
        //Itersection of the half-line and the plane.
        lst_intersec= interseccion(plano,Op);
        if(lst_intersec.empty())
          {
            
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; doesn't intersect. " << std::endl
                      << " Triangle area: " << triang.Area() << std::endl
                      << " vertex 1: " << i->Vertice(1)
                      << " vertex 2: " << i->Vertice(2)
                      << " vertex 3: " << i->Vertice(3) << std::endl
                      << " terna: " << p << std::endl;
            exit(-1);
          }
      }
    return lst_intersec;
  }

//! @brief Print stuff.
void XC::ClosedTriangleMesh::Print(std::ostream &os) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
  }

//! @brief Return a matrix with the coordinates of the points
//! that define each one of the trihedrons.
void XC::ClosedTriangleMesh::getPositionsMatrix(Matrix &m)
  {
    const int sz= size();
    size_t row= 0;
    m= Matrix(sz,12);
    for(const_iterator i= begin();i!=end();i++,row++)
      {
        const Trihedron &t= *i;
        const Pos3d &c= t.Cuspide();
        const Triangulo3d &b= t.Base();
        const Pos3d p1= b.Vertice(1);
        const Pos3d p2= b.Vertice(2);
        const Pos3d p3= b.Vertice(3);
        m(row,0)= c.x(); m(row,1)= c.y(); m(row,2)= c.z();
        m(row,3)= p1.x(); m(row,4)= p1.y(); m(row,5)= p1.z();
        m(row,6)= p2.x(); m(row,7)= p2.y(); m(row,8)= p2.z();
        m(row,9)= p3.x(); m(row,10)= p3.y(); m(row,11)= p3.z();
      }
  }

//! @brief Create the trihedrons that define the diagram from the matrix
//! that contains the points that define each trihedron.
void XC::ClosedTriangleMesh::setPositionsMatrix(const Matrix &m)
  {
    const int numberOfRows= m.noRows();
    assert(m.noCols()==12);
    trihedrons.resize(numberOfRows);
    for(int i= 0;i<numberOfRows;i++)
      {
        const Pos3d c(m(i,0),m(i,1),m(i,2));
        const Pos3d p1(m(i,3),m(i,4),m(i,5));
        const Pos3d p2(m(i,6),m(i,7),m(i,8));
        const Pos3d p3(m(i,9),m(i,10),m(i,11));
        trihedrons[i]= Trihedron(c,p1,p2,p3);
      }
  }

//! @brief Escribe la matriz en un archivo binario.
void XC::ClosedTriangleMesh::write(std::ofstream &os)
  {
    os.write((char *) &tol,sizeof tol);
    os.write((char *) &rMax,sizeof rMax);
    os.write((char *) &rMin,sizeof rMin); 
    Matrix m;
    getPositionsMatrix(m);
    m.write(os);
  }

//! @brief Lee la matriz de un archivo binario.
void XC::ClosedTriangleMesh::read(std::ifstream &is)
  {
    is.read((char *) &tol,sizeof tol);
    is.read((char *) &rMax,sizeof rMax); 
    is.read((char *) &rMin,sizeof rMin); 
    Matrix m;
    m.read(is);
    setPositionsMatrix(m);
  }

//! @brief Sends object members through the channel being passed as parameter.
int XC::ClosedTriangleMesh::sendData(CommParameters &cp)
  {
    int res= 0; //MovableObject::sendData(cp);
    res+= cp.sendDoubles(tol,rMax,rMin,getDbTagData(),CommMetaData(1));
    Matrix m;
    getPositionsMatrix(m);
    res+= cp.sendMatrix(m,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::ClosedTriangleMesh::recvData(const CommParameters &cp)
  {
    int res= 0;
    res+= cp.receiveDoubles(tol,rMax,rMin,getDbTagData(),CommMetaData(1));
    Matrix m;
    res+= cp.receiveMatrix(m,getDbTagData(),CommMetaData(2));
    setPositionsMatrix(m);
    return res;
  }

//! @brief Send the object through the channel being passed as parameter.
int XC::ClosedTriangleMesh::sendSelf(CommParameters &cp)
  {
    inicComm(3);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send ID data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::ClosedTriangleMesh::recvSelf(const CommParameters &cp)
  {
    const int dataTag= getDbTag();
    inicComm(3);
    int res = cp.receiveIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data\n";
    else
      res+= recvData(cp);
    return res;
  }



