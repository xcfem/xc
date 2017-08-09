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
//QuadSurface.cc

#include "QuadSurface.h"
#include "Pnt.h"
#include "preprocessor/cad/Cad.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "xc_basic/src/matrices/matrizT.h"

//! @brief Constructor.
XC::QuadSurface::QuadSurface(Preprocessor *m,const size_t &ndivI, const size_t &ndivJ)
  : Face(m,ndivI,ndivJ) {}

//! @brief Virtual constructor.
XC::SetEstruct *XC::QuadSurface::getCopy(void) const
  { return new QuadSurface(*this); }

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
	  std::cerr << __FUNCTION__ << "; sides: "
                    << edgeA->getName() << " y " << edgeB->getName() 
                    << " are already meshed, and they have different number of divisions ("
                    << ndA << " y " << ndB << std::endl;
      }
    else if(edgeA->TieneNodos()) //A edge already meshed.
      {
        if(ndA!=ndj)
          {
	    std::clog << __FUNCTION__ << "; edge: "
                      << edgeA->getName()
                      << " is already meshed, division number can't be changed."
                      << " to " << ndj << " keeping NDiv= " << ndA << std::endl;
            retval= ndA;
          }
      }
    else if(edgeB->TieneNodos()) //B edge already meshed.
      {
        if(ndB!=ndj)
          {
	    std::clog << __FUNCTION__ << "; edge: "
                      << edgeB->getName()
                      << " is already meshed, division number can't be changed."
                      << " to " << ndj << " keeping NDiv= " << ndB << std::endl;
            retval= ndB;
          }
      }
    else //Nor A nor B are meshed.
      retval= ndj;
    return retval;
  }

//! @brief Returns the lado homólogo al being passed as parameter.
const XC::Edge *XC::QuadSurface::get_lado_homologo(const Edge *l) const
  {
    const Edge *retval= nullptr;   
    const size_t indice= IndiceEdge(l);
    if(indice!=0)
      {
        const size_t ind0= indice-1;
        assert(ind0<4);
        if(ind0==0)
          retval= lineas[2].getEdge();
        else if(ind0==2)
          retval= lineas[0].getEdge();
        else if(ind0==1)
          retval= lineas[3].getEdge();
        else if(ind0==3)
          retval= lineas[1].getEdge();
      }
    else //No la encuentra.
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; line :" << l->getName() 
                << " is not an edge of the surface: "
		<< getName() << std::endl;    
    return retval;
  }


//! @brief Asigna el number of divisions en el eje i.
void XC::QuadSurface::SetNDivI(const size_t &ndi)
  {
    if(lineas.size()<4)
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; not a quadrilateral surface, it has " 
                << lineas.size() << " sides." << std::endl;
    else
      {
        Edge *edge0= lineas[0].getEdge();
        Edge *edge2= lineas[2].getEdge();
        const size_t ndc= calc_ndiv(edge0,edge2,ndi);
        if(ndc>0)
          {
            Face::SetNDivI(ndc);
            edge0->SetNDiv(ndc);
            edge2->SetNDiv(ndc);
          }
      }
  }

//! @brief Asigna el number of divisions en el eje j.
void XC::QuadSurface::SetNDivJ(const size_t &ndj)
  {
    if(lineas.size()<4)
      std::cerr << "XC::QuadSurface::SetNDivJ, surface is not a quadrilateral, it has " 
                << lineas.size() << " sides." << std::endl;
    else
      {
        Edge *edge1= lineas[1].getEdge();
        Edge *edge3= lineas[3].getEdge();
        const size_t ndc= calc_ndiv(edge1,edge3,ndj);
        if(ndc>0)
          {
            Face::SetNDivJ(ndc);
            edge1->SetNDiv(ndc);
            edge3->SetNDiv(ndc);
          }
      }
  }

//! @brief Conciliate lines division numbers with
//! those of the surface.
void XC::QuadSurface::ConciliaNDivIJ(void)
  {
    if(checkNDivs())
      {
	Face::SetNDivI(lineas[0].getEdge()->NDiv());
	Face::SetNDivJ(lineas[1].getEdge()->NDiv());
      }
  }

//! @brief Verifies that the number of divisions of the lines are
//! compatible.
bool XC::QuadSurface::checkNDivs(const size_t &i,const size_t &j) const
  {
    const size_t ndivA= lineas[i].getEdge()->NDiv();
    const size_t ndivB= lineas[j].getEdge()->NDiv();
    if(ndivA!=ndivB)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; lines: "
                  << lineas[i].getEdge()->getName() << " and "
                  << lineas[j].getEdge()->getName() 
                  << " of surface: " << getName()
                  << " have different number of divisions ("
                  << ndivA << " y " << ndivB << ')' << std::endl;
        return false;
      }
    else
      return true;
  }

//! @brief Verifies that the number of divisions of the lines are
//! compatible.
bool XC::QuadSurface::checkNDivs(void) const
  { return (checkNDivs(0,2) && checkNDivs(1,3)); }

//! @brief Computes the number of divisions on the i axis
//! for an element edge size equal or lesser than the
//! size being passed as parameter.
void XC::QuadSurface::SetElemSizeI(const double &sz)
  {
    const double l1= lineas[0].getLongitud();
    const double l2= lineas[2].getLongitud();
    const size_t n= ceil(std::max(l1,l2)/sz);
    SetNDivI(n);
  }

//! @brief Computes the number of divisions on the j axis
//! for an element edge size equal or lesser than the
//! size being passed as parameter.
void XC::QuadSurface::SetElemSizeJ(const double &sz)
  {
    const double l1= lineas[1].getLongitud();
    const double l2= lineas[3].getLongitud();
    const size_t n= ceil(std::max(l1,l2)/sz);
    SetNDivJ(n);
  }


//! @brief Computes the number of divisions on the i and j axis
//! for an element edges sizes equal or lesser than the
//! sizes being passed as parameter.
void XC::QuadSurface::SetElemSizeIJ(const double &szI,const double &szJ)
  {
    SetElemSizeI(szI);
    SetElemSizeJ(szJ);
  }

//! @brief Creates and inserts the lines from the points identified
//! by the indexes being passed as parameter.
void XC::QuadSurface::setPuntos(const ID &indices_ptos)
  {
    const size_t np= indices_ptos.Size(); //Number of indexes.
    if(np!=4)
      std::cerr << nombre_clase() << __FUNCTION__
	        << "; surface definition needs "
                << 4 << " points, we got: " << np << ".\n";
    else
      {
        if(NumEdges()>0)
          std::cerr << nombre_clase() << __FUNCTION__
	            << "; warning redefinition of surface: '"
                    << getName() << "'.\n";

	Face::addPoints(indices_ptos);
        cierra();
      }
    int tagV1= GetVertice(1)->GetTag();
    if(tagV1!=indices_ptos(0))
      std::cerr << nombre_clase() << "::" << __FUNCTION__
		<< "; surface: " << GetTag()
                << "is inverted." << std::endl;
  }

//! @brief Creates and inserts the lines from the points being passed as parameter.
void XC::QuadSurface::setPuntos(const MatrizPtrPnt &pntPtrs)
  {
    const size_t nf= pntPtrs.getNumFilas(); //No. de filas de puntos.
    if(nf<2)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; pointer matrix must have at least two rows." << std::endl;
        return;
      }
    const size_t nc= pntPtrs.getNumCols(); //No. de columnas de puntos.
    if(nc<2)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; pointer matrix must have at least two columns." << std::endl;
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

//! @brief Creates and inserts the lines from the points being passed as parameter.
//! If some of the indices is negative it means that this position is not needed
//! to define the surface.
void XC::QuadSurface::setPuntos(const m_int &indices_ptos)
  {
    const size_t nf= indices_ptos.getNumFilas(); //No. de filas de puntos.
    const size_t nc= indices_ptos.getNumCols(); //No. de columnas de puntos.
    if(nf<2)
      {
        std::cerr << "Matrix of indexes: '"
                  << indices_ptos 
                  << "' must have at least two rows." << std::endl;
        return;
      }
    if(nc<2)
      {
        std::cerr << "Matrix of indexes: '"
                  << indices_ptos 
                  << "' must have at least two columns." << std::endl;
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
	        std::cerr << nombre_clase() << "::" << __FUNCTION__
			  << "; NULL pointer to point in position: ("
                          << i << ',' << j << ") in definition of surface: '"
                          << getName() << "'" << std::endl;
            }
        }
    setPuntos(puntos);
  }

void XC::QuadSurface::defGridPoints(const boost::python::list &l)
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

//! @brief Returns (ndivI+1)*(ndivJ+1) positions to place the nodes.
MatrizPos3d XC::QuadSurface::get_posiciones(void) const
  {
    MatrizPos3d retval;
    const int numEdges= NumEdges();
    if(numEdges!=4)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
		  << "; can't mesh surfaces with: "
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

//! @brief Returns a vector int the direction of the local
//! X axis.
Vector3d XC::QuadSurface::getIVector(void) const
  {
    const Pos3d p1= GetVertice(1)->GetPos();
    const Pos3d p2= GetVertice(2)->GetPos();
    const Pos3d p3= GetVertice(3)->GetPos();
    const Pos3d p4= GetVertice(4)->GetPos();
    const Vector3d retval= 0.5*((p2-p1)+(p3-p4));
    return retval;
  }

//! @brief Returns a vector int the direction of the local
//! Y axis.
Vector3d XC::QuadSurface::getJVector(void) const
  {
    const Pos3d p1= GetVertice(1)->GetPos();
    const Pos3d p2= GetVertice(2)->GetPos();
    const Pos3d p3= GetVertice(3)->GetPos();
    const Pos3d p4= GetVertice(4)->GetPos();
    const Vector3d retval= 0.5*((p4-p1)+(p3-p2));
    return retval;
  }

//! @brief Returns a vector int the direction of the local
//! Z axis.
Vector3d XC::QuadSurface::getKVector(void) const
  {
    const Vector3d vI= getIVector();
    const Vector3d vJ= getJVector();
    return vI.getCross(vJ);
  }

//! @brief Creates surface nodes.
void XC::QuadSurface::create_nodes(void)
  {

    checkNDivs();
    if(ttzNodes.Null())
      {
        create_nodes_lineas();

        const size_t filas= NDivJ()+1;
        const size_t cols= NDivI()+1;
        ttzNodes = TritrizPtrNod(1,filas,cols);


        //j=1
        for(size_t k=1;k<=cols;k++)
          {
            Lado &ll= lineas[0];
            Node *nn= ll.GetNodo(k);
            ttzNodes(1,1,k)= nn;
          }

        //j=filas.
        for(size_t k=1;k<=cols;k++) //En sentido inverso.
          ttzNodes(1,filas,k)= lineas[2].GetNodoInv(k);


        //k=1
        for(size_t j=2;j<filas;j++) //En sentido inverso.
          ttzNodes(1,j,1)= lineas[3].GetNodoInv(j);
        //k=cols.
        for(size_t j=2;j<filas;j++)
          ttzNodes(1,j,cols)= lineas[1].GetNodo(j);


        MatrizPos3d pos_nodes= get_posiciones(); //Posiciones of the nodes.
        for(size_t j= 2;j<filas;j++) //Filas interiores.
          for(size_t k= 2;k<cols;k++) //Columnas interiores.
            create_node(pos_nodes(j,k),1,j,k);
      }
    else
      if(verbosity>2)
        std::clog << nombre_clase() << "::" << __FUNCTION__
	          << "; nodes of entity: '" << getName()
		  << "' already exist." << std::endl;      
  }

//! @brief Triggers mesh creation.
void XC::QuadSurface::genMesh(meshing_dir dm)
  {
    if(verbosity>3)
      std::clog << "Meshing quadrilateral surface...("
		<< getName() << ")...";
    create_nodes();
    if(ttzElements.Null())
      create_elements(dm);
    else
      if(verbosity>2)
        std::clog << nombre_clase() << "::" << __FUNCTION__
	          << "; elements for surface: '" << getName()
		  << "' already exist." << std::endl;      
    if(verbosity>3)
      std::clog << "done." << std::endl;
  }
