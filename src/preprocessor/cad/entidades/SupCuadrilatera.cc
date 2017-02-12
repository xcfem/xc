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
//SupCuadrilatera.cc

#include "SupCuadrilatera.h"
#include "Pnt.h"
#include "preprocessor/cad/Cad.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "xc_basic/src/matrices/matrizT.h"

//! @brief Constructor.
XC::SupCuadrilatera::SupCuadrilatera(Preprocessor *m,const size_t &ndivI, const size_t &ndivJ)
  : Face(m,ndivI,ndivJ) {}

//! @brief Virtual constructor.
XC::SetEstruct *XC::SupCuadrilatera::getCopy(void) const
  { return new SupCuadrilatera(*this); }

size_t calc_ndiv(const XC::Edge *edgeA,const XC::Edge *edgeB,const size_t &ndj)
  {
    size_t retval= 0;
    const size_t ndA= edgeA->NDiv();
    const size_t ndB= edgeB->NDiv();
    if(edgeA->TieneNodos() && edgeB->TieneNodos())
      {
        if(ndA==ndB)
          retval= ndA;
        else
	  std::cerr << "calc_ndiv, los lados: "
                    << edgeA->GetNombre() << " y " << edgeB->GetNombre() 
                    << " are already meshed, and they have different number of divisions ("
                    << ndA << " y " << ndB << std::endl;
      }
    else if(edgeA->TieneNodos()) //A edge already meshed.
      {
        if(ndA!=ndj)
          {
	    std::clog << "calc_ndiv, el borde: "
                      << edgeA->GetNombre()
                      << " is already meshed, division number can't be changed."
                      << " to " << ndj << " keeping NDiv= " << ndA << std::endl;
            retval= ndA;
          }
      }
    else if(edgeB->TieneNodos()) //B edge already meshed.
      {
        if(ndB!=ndj)
          {
	    std::clog << "calc_ndiv, el borde: "
                      << edgeB->GetNombre()
                      << " is already meshed, division number can't be changed."
                      << " to " << ndj << " keeping NDiv= " << ndB << std::endl;
            retval= ndB;
          }
      }
    else //Nor A nor B are meshed.
      retval= ndj;
    return retval;
  }

//! @brief Devuelve el lado homólogo al being passed as parameter.
const XC::Edge *XC::SupCuadrilatera::get_lado_homologo(const Edge *l) const
  {
    const Edge *retval= nullptr;   
    const size_t indice= IndiceEdge(l);
    if(indice!=0)
      {
        const size_t ind0= indice-1;
        assert(ind0<4);
        if(ind0==0)
          retval= lineas[2].Borde();
        else if(ind0==2)
          retval= lineas[0].Borde();
        else if(ind0==1)
          retval= lineas[3].Borde();
        else if(ind0==3)
          retval= lineas[1].Borde();
      }
    else //No la encuentra.
      std::cerr << "La línea :" << l->GetNombre() 
                << " no es un borde de la superficie: " << GetNombre() << std::endl;    
    return retval;
  }


//! @brief Asigna el número de divisiones en el eje i.
void XC::SupCuadrilatera::SetNDivI(const size_t &ndi)
  {
    if(lineas.size()<4)
      std::cerr << "XC::SupCuadrilatera::SetNDivI, la superficie no es un cuadrilatero, tiene " 
                << lineas.size() << " lados." << std::endl;
    else
      {
        Edge *edge0= lineas[0].Borde();
        Edge *edge2= lineas[2].Borde();
        const size_t ndc= calc_ndiv(edge0,edge2,ndi);
        if(ndc>0)
          {
            Face::SetNDivI(ndc);
            edge0->SetNDiv(ndc);
            edge2->SetNDiv(ndc);
          }
      }
  }

//! @brief Asigna el número de divisiones en el eje j.
void XC::SupCuadrilatera::SetNDivJ(const size_t &ndj)
  {
    if(lineas.size()<4)
      std::cerr << "SupCuadrilatera::SetNDivJ, la superficie no es un cuadrilatero., tiene " 
                << lineas.size() << " lados." << std::endl;
    else
      {
        Edge *edge1= lineas[1].Borde();
        Edge *edge3= lineas[3].Borde();
        const size_t ndc= calc_ndiv(edge1,edge3,ndj);
        if(ndc>0)
          {
            Face::SetNDivJ(ndc);
            edge1->SetNDiv(ndc);
            edge3->SetNDiv(ndc);
          }
      }
  }

//! @brief Concilia el numero de divisiones de las líneas
//! con los de la superficie.
void XC::SupCuadrilatera::ConciliaNDivIJ(void)
  {
    if(checkNDivs())
      {
	Face::SetNDivI(lineas[0].Borde()->NDiv());
	Face::SetNDivJ(lineas[1].Borde()->NDiv());
      }
  }

//! @brief Comprueba que los números de divisiones de las líneas son compatibles.
bool XC::SupCuadrilatera::checkNDivs(const size_t &i,const size_t &j) const
  {
    const size_t ndivA= lineas[i].Borde()->NDiv();
    const size_t ndivB= lineas[j].Borde()->NDiv();
    if(ndivA!=ndivB)
      {
        std::cerr << "SupCuadrilatera::checkNDivs, las líneas: "
                  << lineas[i].Borde()->GetNombre() << " y "
                  << lineas[j].Borde()->GetNombre() 
                  << " de la superficie: " << GetNombre()
                  << " tienen diferente número de divisiones ("
                  << ndivA << " y " << ndivB << ')' << std::endl;
        return false;
      }
    else
      return true;
  }

//! @brief Comprueba que los números de divisiones de las líneas son compatibles.
bool XC::SupCuadrilatera::checkNDivs(void) const
  { return (checkNDivs(0,2) && checkNDivs(1,3)); }

//! @brief Calcula el número de divisiones en el eje i para que
//! el tamaño del lado I del elemento sea aproximadamente el being passed as parameter.
void XC::SupCuadrilatera::SetElemSizeI(const double &sz)
  {
    const double l1= lineas[0].getLongitud();
    const double l2= lineas[2].getLongitud();
    const size_t n= ceil(std::max(l1,l2)/sz);
    SetNDivI(n);
  }

//! @brief Calcula el número de divisiones en el eje i para que
//! el tamaño del lado I del elemento sea aproximadamente el being passed as parameter.
void XC::SupCuadrilatera::SetElemSizeJ(const double &sz)
  {
    const double l1= lineas[1].getLongitud();
    const double l2= lineas[3].getLongitud();
    const size_t n= ceil(std::max(l1,l2)/sz);
    SetNDivJ(n);
  }


//! @brief Calcula el número de divisiones en los ejes para que
//! el tamaño de los lados del elemento sea aproximadamente el being passed as parameter.
void XC::SupCuadrilatera::SetElemSizeIJ(const double &szI,const double &szJ)
  {
    SetElemSizeI(szI);
    SetElemSizeJ(szJ);
  }

//! @brief Crea e inserta las líneas a partir de los puntos cuyos índices se pasan
//! como parámetro.
void XC::SupCuadrilatera::setPuntos(const ID &indices_ptos)
  {
    const size_t np= indices_ptos.Size(); //No. de índices leídos.
    if(np!=4)
      std::cerr << "XC::SupCuadrilatera::setPuntos; para definir la superficie se necesitan"
                << 4 << " puntos, se pasaron: " << np << ".\n";
    else
      {
        if(NumEdges()>0)
          std::cerr << "SupCuadrilatera::setPuntos; ojo se redefine la superficie: "
                    << GetNombre() << ".\n";

	Face::addPoints(indices_ptos);
        cierra();
      }
    int tagV1= GetVertice(1)->GetTag();
    if(tagV1!=indices_ptos(0))
      std::cerr << "La superficie: " << GetTag()
                << "está invertida." << std::endl;
  }

//! @brief Crea e inserta las líneas a partir de los puntos cuyos índices se pasan
//! como parámetro. Si el algún índice es negativo quiere decir que esa posición
//! no se usa para definir la superficie.
void XC::SupCuadrilatera::setPuntos(const MatrizPtrPnt &pntPtrs)
  {
    const size_t nf= pntPtrs.getNumFilas(); //No. de filas de puntos.
    if(nf<2)
      {
        std::cerr << "SupCuadrilatera::setPuntos; la matriz de pointers debe tener al menos dos filas." << std::endl;
        return;
      }
    const size_t nc= pntPtrs.getNumCols(); //No. de columnas de puntos.
    if(nc<2)
      {
        std::cerr << "SupCuadrilatera::setPuntos; la matriz de pointers debe tener al menos dos columnas." << std::endl;
        return;
      }
    if(nf==2)
      {
        if(nc==2)
          {
            NuevaLinea(pntPtrs(1,1),pntPtrs(1,2));
            NuevaLinea(pntPtrs(1,2),pntPtrs(2,2));
            NuevaLinea(pntPtrs(2,2),pntPtrs(2,1));
            NuevaLinea(pntPtrs(2,1),pntPtrs(1,1));
          }
        else //nc>= 3
          {
            NuevaLinea(pntPtrs(1,1),pntPtrs(1,2),pntPtrs(1,3));
            NuevaLinea(pntPtrs(1,3),pntPtrs(2,3));
            NuevaLinea(pntPtrs(2,3),pntPtrs(2,2),pntPtrs(2,1));
            NuevaLinea(pntPtrs(2,1),pntPtrs(1,1));
          }
      }
    else //nf>=3
      {
        if(nc==2)
          {
            NuevaLinea(pntPtrs(1,1),pntPtrs(1,2));
            NuevaLinea(pntPtrs(1,2),pntPtrs(2,2),pntPtrs(3,2));
            NuevaLinea(pntPtrs(3,2),pntPtrs(3,1));
            NuevaLinea(pntPtrs(3,1),pntPtrs(2,1),pntPtrs(1,1));
          }
        else //nc>= 3
          {
            NuevaLinea(pntPtrs(1,1),pntPtrs(1,2),pntPtrs(1,3));
            NuevaLinea(pntPtrs(1,3),pntPtrs(2,3),pntPtrs(3,3));
            NuevaLinea(pntPtrs(3,3),pntPtrs(3,2),pntPtrs(3,1));
            NuevaLinea(pntPtrs(3,1),pntPtrs(2,1),pntPtrs(1,1));
          }
      }
  }

//! @brief Crea e inserta las líneas a partir de los puntos cuyos índices se pasan
//! como parámetro. Si el algún índice es negativo quiere decir que esa posición
//! no se usa para definir la superficie.
void XC::SupCuadrilatera::setPuntos(const m_int &indices_ptos)
  {
    const size_t nf= indices_ptos.getNumFilas(); //No. de filas de puntos.
    const size_t nc= indices_ptos.getNumCols(); //No. de columnas de puntos.
    if(nf<2)
      {
        std::cerr << "La matriz de índices: '"
                  << indices_ptos 
                  << "' debe tener al menos dos filas." << std::endl;
        return;
      }
    if(nc<2)
      {
        std::cerr << "La matriz de índices: '"
                  << indices_ptos 
                  << "' debe tener al menos dos columnas." << std::endl;
        return;
      }
    MatrizPtrPnt puntos(nf,nc);
    for(size_t i= 1;i<=nf;i++)
      for(size_t j= 1;j<=nc;j++)
        {
          const int iPunto= indices_ptos(i,j);
          if(iPunto>=0)
            {
              Pnt *p= BuscaPnt(iPunto);
              if(p)
                puntos(i,j)= p;
              else
	        std::cerr << "SupCuadrilatera::setPuntos; no se encontró el punto de índices: ("
                          << i << ',' << j << ") para definir la superficie: '"
                          << GetNombre() << "'" << std::endl;
            }
        }
    setPuntos(puntos);
  }

void XC::SupCuadrilatera::defGridPoints(const boost::python::list &l)
  {
    int nRows= len(l);
    boost::python::list row0= boost::python::extract<boost::python::list>(l[0]);
    int nCols= len(row0);
    // copy the components
    m_int tmp(nRows,nCols);
    for(int i=1; i<=nRows; i++)
      {
        boost::python::list rowI= boost::python::extract<boost::python::list>(l[i-1]);
        for(int j= 1; j<=nCols;j++)
          tmp(i,j)= boost::python::extract<double>(rowI[j-1]);
      }
    setPuntos(tmp);
  }

//! @brief Devuelve (ndivI+1)*(ndivJ+1) posiciones para los nodos.
MatrizPos3d XC::SupCuadrilatera::get_posiciones(void) const
  {
    MatrizPos3d retval;
    const int numEdges= NumEdges();
    if(numEdges!=4)
      {
        std::cerr << "Can't mesh surfaces with: "
	          << numEdges << " edges." << std::endl;
        return retval;
      }

    MatrizPos3d ptos_l1= lineas[0].GetPosNodosDir();
    MatrizPos3d ptos_l2= lineas[1].GetPosNodosDir();
    MatrizPos3d ptos_l3= lineas[2].GetPosNodosInv(); //Ordenados al revés.
    MatrizPos3d ptos_l4= lineas[3].GetPosNodosInv(); //Ordenados al revés.
    retval= MatrizPos3d(ptos_l1,ptos_l2,ptos_l3,ptos_l4);
    retval.Trn();
    return retval;
  }

Vector3d XC::SupCuadrilatera::getIVector(void) const
  {
    const Pos3d p1= GetVertice(1)->GetPos();
    const Pos3d p2= GetVertice(2)->GetPos();
    const Pos3d p3= GetVertice(3)->GetPos();
    const Pos3d p4= GetVertice(4)->GetPos();
    const Vector3d retval= 0.5*((p2-p1)+(p3-p4));
    return retval;
  }

Vector3d XC::SupCuadrilatera::getJVector(void) const
  {
    const Pos3d p1= GetVertice(1)->GetPos();
    const Pos3d p2= GetVertice(2)->GetPos();
    const Pos3d p3= GetVertice(3)->GetPos();
    const Pos3d p4= GetVertice(4)->GetPos();
    const Vector3d retval= 0.5*((p4-p1)+(p3-p2));
    return retval;
  }

Vector3d XC::SupCuadrilatera::getKVector(void) const
  {
    const Vector3d vI= getIVector();
    const Vector3d vJ= getJVector();
    return vI.getCross(vJ);
  }

//! @brief Crea los nodos de la superficie.
void XC::SupCuadrilatera::crea_nodos(void)
  {

    checkNDivs();
    if(nodos.Null())
      {
        crea_nodos_lineas();

        const size_t filas= NDivJ()+1;
        const size_t cols= NDivI()+1;
        nodos = TritrizPtrNod(1,filas,cols);


        //j=1
        for(size_t k=1;k<=cols;k++)
          {
            Lado &ll= lineas[0];
            Node *nn= ll.GetNodo(k);
            nodos(1,1,k)= nn;
          }

        //j=filas.
        for(size_t k=1;k<=cols;k++) //En sentido inverso.
          nodos(1,filas,k)= lineas[2].GetNodoInv(k);


        //k=1
        for(size_t j=2;j<filas;j++) //En sentido inverso.
          nodos(1,j,1)= lineas[3].GetNodoInv(j);
        //k=cols.
        for(size_t j=2;j<filas;j++)
          nodos(1,j,cols)= lineas[1].GetNodo(j);


        MatrizPos3d pos_nodos= get_posiciones(); //Posiciones de los nodos.
        for(size_t j= 2;j<filas;j++) //Filas interiores.
          for(size_t k= 2;k<cols;k++) //Columnas interiores.
            crea_nodo(pos_nodos(j,k),1,j,k);
      }
    else
      if(verborrea>2)
        std::clog << "SupCuadrilatera::crea_nodos; los nodos de la entidad: '" << GetNombre() << "' ya existen." << std::endl;      
  }

//! @brief Triggers mesh creation.
void XC::SupCuadrilatera::genMesh(meshing_dir dm)
  {
    if(verborrea>3)
      std::clog << "Meshing SupCuadrilátera...(" << GetNombre() << ")...";
    crea_nodos();
    if(elementos.Null())
      crea_elementos(dm);
    else
      if(verborrea>2)
        std::clog << "SupCuadrilatera::genMesh; los elementos de la entidad: '" << GetNombre() << "' ya existen." << std::endl;      
    if(verborrea>3)
      std::clog << "hecho." << std::endl;
  }
