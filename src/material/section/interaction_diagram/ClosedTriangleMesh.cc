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
#include "xc_utils/src/geom/d2/MallaTriang3d.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d2/Triangulo3d.h"
#include "xc_basic/src/util/mchne_eps.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/d1/Segmento3d.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/gnu_gts/TriangleMap.h"

XC::ClosedTriangleMesh::iterator XC::ClosedTriangleMesh::begin(void) { return triedros.begin(); }
XC::ClosedTriangleMesh::iterator XC::ClosedTriangleMesh::end(void) { return triedros.end(); }
XC::ClosedTriangleMesh::const_iterator XC::ClosedTriangleMesh::begin() const { return triedros.begin(); }
XC::ClosedTriangleMesh::const_iterator XC::ClosedTriangleMesh::end() const { return triedros.end(); }
size_t XC::ClosedTriangleMesh::size(void) const { return triedros.size(); }

size_t XC::ClosedTriangleMesh::GetNumFacetas(void) const { return size(); }


//! @brief Constructor por defecto.
XC::ClosedTriangleMesh::ClosedTriangleMesh(void)
  : GeomObj3d(), MovableObject(0), triedros(0), tol(0.0), rMax(0.0),rMin(0.0) {}

XC::ClosedTriangleMesh::ClosedTriangleMesh(const Pos3d &org,const MallaTriang3d &mll)
  : GeomObj3d(), MovableObject(0), triedros(mll.GetNumCaras(),Triedro3d()), tol(0.0), rMax(0.0), rMin(0.0)
  {
    const size_t nf= mll.GetNumCaras();
    static const Pos3d org3d;
    if(nf<4) return;
    for(MallaTriang3d::Point_const_iterator i= mll.points_begin();i!=mll.points_end();i++)
      rMax= std::max(rMax,dist(Pos3d(*i),org3d));
    rMin= 10*rMax;
    for(MallaTriang3d::Point_const_iterator i= mll.points_begin();i!=mll.points_end();i++)
      rMin= std::min(rMin,dist(Pos3d(*i),org3d));
    rMin/=3; //Radio de la esfera inscrita en el tetraedro cuya
             //esfera circunscrita pasa por el punto más cercano al origen.
    size_t cont= 0;
    for(MallaTriang3d::Facet_const_iterator i= mll.facets_begin();i!=mll.facets_end();i++)
      {
        const Triangulo3d tf= mll.GetTrianguloCara(i);
        triedros[cont]= Triedro3d(org,tf);
        cont++;
      }
    //const double longDiagonalBND= Abs(Bnd().Diagonal());
    tol= 0.0;//longDiagonalBND/1e6;
  }

//! @brief Constructor de copia.
XC::ClosedTriangleMesh::ClosedTriangleMesh(const ClosedTriangleMesh &otro)
  : GeomObj3d(otro), MovableObject(otro), triedros(otro.triedros), tol(otro.tol), rMax(otro.rMax),rMin(otro.rMin)
  {}

//! @brief Operador de asignación.
XC::ClosedTriangleMesh &XC::ClosedTriangleMesh::operator=(const ClosedTriangleMesh &otro)
  {
    GeomObj3d::operator=(otro);
    triedros= otro.triedros;
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
      std::cerr << "No se pudo abrir el archivo: '"
                << fName << "'\n";
  }
  

//! @brief Constructor virtual.
XC::ClosedTriangleMesh *XC::ClosedTriangleMesh::clon(void) const
  { return new ClosedTriangleMesh(*this); }

double XC::ClosedTriangleMesh::GetMax(short unsigned int i) const
  {
    double retval= NAN;
    if(size()<1) return retval;
    v_triedros::const_iterator j= triedros.begin();
    retval= j->GetMax(i);
    j++;
    for(;j!=triedros.end();j++)
      retval= std::max(retval,j->GetMax(i));
    return retval;
  }
double XC::ClosedTriangleMesh::GetMin(short unsigned int i) const
  {
    double retval= NAN;
    if(size()<1) return retval;
    v_triedros::const_iterator j= triedros.begin();
    retval= j->GetMin(i);
    j++;
    for(;j!=triedros.end();j++)
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
    for(v_triedros::const_iterator i= triedros.begin();i!=triedros.end();i++)
      {
        const Pos3d p1= i->Vertice(1);
        int nearest1= kdtree.getNearestBallPoint(p1,tol);
        if(nearest1<0) //New vertex.
          {
            nearest1= counter;
            kdtree.insert(nearest1,p1);
            vertices.Inserta(nearest1,p1);
            counter++;
          }
        const Pos3d p2= i->Vertice(2);
        int nearest2= kdtree.getNearestBallPoint(p2,tol);
        if(nearest2<0) //New vertex.
          {
            nearest2= counter;
            kdtree.insert(nearest2,p2);
            vertices.Inserta(nearest2,p2);
            counter++;
          }
        const Pos3d p3= i->Vertice(3);
        int nearest3= kdtree.getNearestBallPoint(p3,tol);
        if(nearest3<0) //New vertex.
          {
            nearest3= counter;
            kdtree.insert(nearest3,p3);
            vertices.Inserta(nearest3,p3);
            counter++;
          }
        faces.Append(nearest1,nearest2,nearest3);
      }
    return retval;
  }

//! @brief Busca el triedro que contiene al punto being passed as parameter.
XC::ClosedTriangleMesh::const_iterator XC::ClosedTriangleMesh::BuscaTriedro(const Pos3d &p) const
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

//! @brief Busca el triedro que contiene al punto being passed as parameter.
const Triedro3d *XC::ClosedTriangleMesh::BuscaPtrTriedro(const Pos3d &p) const
  {
    const Triedro3d *retval= nullptr;
    if(triedros.empty())
      {
	std::cerr << "ClosedTriangleMesh::BuscaPtrTriedro; la lista de triedros está vacía."
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
        if(!retval) //Sigue sin encontrarlo, buscamos aquel cuyo eje está más próximo.
          {
	    ClosedTriangleMesh::const_iterator i= begin();
            const Triedro3d *tr= &(*i);
            SemiRecta3d rayo(tr->Cuspide(),p);
            Recta3d eje= tr->Eje();
            double angEjeRayo= angulo(eje,rayo);
            double angTmp= angEjeRayo;
            retval= tr;
            i++;
            for(;i!=end();i++)
              {
                tr= &(*i);
                rayo= SemiRecta3d(tr->Cuspide(),p);
                angTmp= angulo(tr->Eje(),rayo);
                if(angTmp<angEjeRayo)
                  {
                    angEjeRayo= angTmp;
                    retval= tr;
                  }
              }
          }
//     if(!retval) //Si sigue sin encontrarlo buscamos un punto en la misma línea.
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

//! @brief Devuelve la intersección de la semirrecta que une el origen (0,0,0) y el
//! el punto p con la superficie.
GeomObj::list_Pos3d XC::ClosedTriangleMesh::get_interseccion(const Pos3d &p) const
  {
    GeomObj::list_Pos3d lst_intersec;
    const Pos3d O= Pos3d(0.0,0.0,0.0);
    //Buscamos el triedro que contiene a p.
    const Triedro3d *i= BuscaPtrTriedro(p);
    if(!i)
      {
	std::cerr << "ClosedTriangleMesh::get_interseccion: no se encontró un triedro que contuviera a:"
                  << p << " cuadrante: " << p.Cuadrante() << std::endl;
      }
    else
      {
        Triangulo3d triang(i->Vertice(1),i->Vertice(2),i->Vertice(3));
        //Calculamos el plano que contiene al triangulo.
        const Plano3d plano= triang.GetPlano();
        SemiRecta3d Op(O,p);
        //Calculamos la intersección entre la semirrecta y dicho plano.
        lst_intersec= interseccion(plano,Op);
        if(lst_intersec.empty())
          {
            
	    std::cerr << "Doesn't intersect. " << std::endl
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

void XC::ClosedTriangleMesh::Print(std::ostream &os) const
  {
    std::cerr << "ClosedTriangleMesh::Print no implementada." << std::endl;
  }

//! @brief Devuelve una matriz con las coordenadas de los puntos
//! que definen cada uno de los triedros.
void XC::ClosedTriangleMesh::getMatrizPosiciones(Matrix &m)
  {
    const int sz= size();
    size_t fila= 0;
    m= Matrix(sz,12);
    for(const_iterator i= begin();i!=end();i++,fila++)
      {
        const Triedro3d &t= *i;
        const Pos3d &c= t.Cuspide();
        const Triangulo3d &b= t.Base();
        const Pos3d p1= b.Vertice(1);
        const Pos3d p2= b.Vertice(2);
        const Pos3d p3= b.Vertice(3);
        m(fila,0)= c.x(); m(fila,1)= c.y(); m(fila,2)= c.z();
        m(fila,3)= p1.x(); m(fila,4)= p1.y(); m(fila,5)= p1.z();
        m(fila,6)= p2.x(); m(fila,7)= p2.y(); m(fila,8)= p2.z();
        m(fila,9)= p3.x(); m(fila,10)= p3.y(); m(fila,11)= p3.z();
      }
  }

//! @brief Crea los triedros que definen el diagrama a partir
//! de una matriz con las coordenadas de los puntos
//! que definen cada uno de los triedros.
void XC::ClosedTriangleMesh::setMatrizPosiciones(const Matrix &m)
  {
    const int nfilas= m.noRows();
    assert(m.noCols()==12);
    triedros.resize(nfilas);
    for(int i= 0;i<nfilas;i++)
      {
        const Pos3d c(m(i,0),m(i,1),m(i,2));
        const Pos3d p1(m(i,3),m(i,4),m(i,5));
        const Pos3d p2(m(i,6),m(i,7),m(i,8));
        const Pos3d p3(m(i,9),m(i,10),m(i,11));
        triedros[i]= Triedro3d(c,p1,p2,p3);
      }
  }

//! @brief Escribe la matriz en un archivo binario.
void XC::ClosedTriangleMesh::write(std::ofstream &os)
  {
    os.write((char *) &tol,sizeof tol);
    os.write((char *) &rMax,sizeof rMax);
    os.write((char *) &rMin,sizeof rMin); 
    Matrix m;
    getMatrizPosiciones(m);
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
    setMatrizPosiciones(m);
  }

//! @brief Envia los miembros del objeto through the channel being passed as parameter.
int XC::ClosedTriangleMesh::sendData(CommParameters &cp)
  {
    int res= 0; //MovableObject::sendData(cp);
    res+= cp.sendDoubles(tol,rMax,rMin,getDbTagData(),CommMetaData(1));
    Matrix m;
    getMatrizPosiciones(m);
    res+= cp.sendMatrix(m,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::ClosedTriangleMesh::recvData(const CommParameters &cp)
  {
    int res= 0;
    res+= cp.receiveDoubles(tol,rMax,rMin,getDbTagData(),CommMetaData(1));
    Matrix m;
    res+= cp.receiveMatrix(m,getDbTagData(),CommMetaData(2));
    setMatrizPosiciones(m);
    return res;
  }

//! @brief Envia el objeto through the channel being passed as parameter.
int XC::ClosedTriangleMesh::sendSelf(CommParameters &cp)
  {
    inicComm(3);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ClosedTriangleMesh::sendSelf() - failed to send ID data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::ClosedTriangleMesh::recvSelf(const CommParameters &cp)
  {
    const int dataTag= getDbTag();
    inicComm(3);
    int res = cp.receiveIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "ClosedTriangleMesh::recvSelf() - failed to receive ID data\n";
    else
      res+= recvData(cp);
    return res;
  }



