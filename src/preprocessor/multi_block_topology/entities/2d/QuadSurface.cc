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
//QuadSurface.cc

#include "QuadSurface.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/multi_block_topology/MultiBlockTopology.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/pos_vec/Pos3dArray.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "utility/matrices/TMatrix.h"
#include "preprocessor/Preprocessor.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::QuadSurface::QuadSurface(Preprocessor *m,const size_t &ndivI, const size_t &ndivJ)
  : Face(m,ndivI,ndivJ) {}

//! @brief Virtual constructor.
XC::SetEstruct *XC::QuadSurface::getCopy(void) const
  { return new QuadSurface(*this); }

//! @brief Creates and inserts the lines from the points identified
//! by the indexes being passed as parameter.
void XC::QuadSurface::setPoints(const ID &point_indexes)
  {
    const size_t np= point_indexes.Size(); //Number of indexes.
    if(np!=4)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; surface definition needs "
                << 4 << " points, we got: " << np << "."
		<< Color::def << std::endl;
    else
      {
        if(getNumberOfEdges()>0)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << "; warning redefinition of surface: '"
                    << getName() << "'."
		    << Color::def << std::endl;

	Face::addPoints(point_indexes);
        close();
      }
    int tagV1= getVertex(1)->getTag();
    if(tagV1!=point_indexes(0))
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; surface: " << getTag()
                << "is inverted."
		<< Color::def << std::endl;
  }

//! @brief Creates and inserts the lines from the points being
//! passed as parameter.
void XC::QuadSurface::setPoints(const PntPtrArray &pntPtrs)
  {
    const size_t nRows= pntPtrs.getNumberOfRows(); //No. de rows of points.
    if(nRows<2)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; pointer matrix must have at least two rows."
		  << Color::def << std::endl;
        return;
      }
    const size_t nColumns= pntPtrs.getNumberOfColumns(); //No. de columns of points.
    if(nColumns<2)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; pointer matrix must have at least two columns."
		  << Color::def << std::endl;
        return;
      }
    if(nRows==2)
      {
        if(nColumns==2)
          {
	    std::deque<Pnt *> loop= pntPtrs.getLoop({1,2},{1,2});
            newLine(loop[0],loop[1]); // 3 +-----------+ 2
            newLine(loop[1],loop[2]); //  |            |
            newLine(loop[2],loop[3]); //  |            |
            newLine(loop[3],loop[1]); // 0 +-----------+ 1
          }
        else //nColumns>= 3
          {
	    std::deque<Pnt *> loop= pntPtrs.getLoop({1,2},{1,2,3});
            newLine(loop[0],loop[1],loop[2]); // 5 +-----+-----+ 3
            newLine(loop[2],loop[3]);         //   |     4     |
            newLine(loop[3],loop[4],loop[5]); //   |     1     |
            newLine(loop[5],loop[0]);         // 0 +-----+-----+ 2
          }
      }
    else //nRows>=3
      {
        if(nColumns==2)
          {
	    std::deque<Pnt *> loop= pntPtrs.getLoop({1,2,3},{1,2});
	                                       // 4 +--------+ 3
            newLine(loop[0],loop[1]);          //   |        |
            newLine(loop[1],loop[2],loop[3]);  // 5 +        + 2
            newLine(loop[3],loop[4]);          //   |        |
            newLine(loop[4],loop[5],loop[0]);  // 0 +--------+ 1
          }
        else //nColumns>= 3
          {
	    std::deque<Pnt *> loop= pntPtrs.getLoop({1,2,3},{1,2,3});
	                                       // 6 +----+----+ 4
            newLine(loop[0],loop[1],loop[2]);  //   |    5    |
            newLine(loop[2],loop[3],loop[4]);  // 7 +         + 3
            newLine(loop[4],loop[5],loop[6]);  //   |         |
            newLine(loop[6],loop[7],loop[0]);  // 0 +----+----+ 2
          }                                    //        1
      }
  }

//! @brief Creates and inserts the lines from the points being passed as parameter.
//! If some of the indices is negative it means that this position is not needed
//! to define the surface.
void XC::QuadSurface::setPoints(const m_int &point_indexes)
  {
    const size_t nRows= point_indexes.getNumberOfRows(); //No. de rows of points.
    const size_t nColumns= point_indexes.getNumberOfColumns(); //No. de columns of points.
    if(nRows<2)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; matrix of indexes: '"
                  << point_indexes 
                  << "' must have at least two rows."
		  << Color::def << std::endl;
        return;
      }
    if(nColumns<2)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; matrix of indexes: '"
                  << point_indexes 
                  << "' must have at least two columns."
		  << Color::def << std::endl;
        return;
      }
    PntPtrArray points(nRows,nColumns);
    for(size_t i= 1;i<=nRows;i++)
      for(size_t j= 1;j<=nColumns;j++)
        {
          const int iPoint= point_indexes(i,j);
          if(iPoint>=0)
            {
              Pnt *p= BuscaPnt(iPoint);
              if(p)
                points(i,j)= p;
              else
	        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			  << "; NULL pointer to point in position: ("
                          << i << ',' << j << ") in definition of surface: '"
                          << getName() << "'"
			  << Color::def << std::endl;
            }
        }
    setPoints(points);
  }

void XC::QuadSurface::defGridPoints(const boost::python::list &l)
  {
    int nRows= len(l);
    boost::python::list row0= boost::python::extract<boost::python::list>(l[0]);
    int numberOfColumns= len(row0);
    // copy the components
    m_int tmp(nRows,numberOfColumns);
    for(int i=1; i<=nRows; i++)
      {
        boost::python::list rowI= boost::python::extract<boost::python::list>(l[i-1]);
        for(int j= 1; j<=numberOfColumns;j++)
          tmp(i,j)= boost::python::extract<double>(rowI[j-1]);
      }
    setPoints(tmp);
  }

//! @brief Returns (ndivI+1)*(ndivJ+1) positions to place the nodes.
Pos3dArray XC::QuadSurface::get_positions(void) const
  {
    Pos3dArray retval;
    const int numEdges= getNumberOfEdges();
    if(numEdges!=4)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; can't mesh surfaces with: "
	          << numEdges << " edges."
		  << Color::def << std::endl;
        return retval;
      }

    Pos3dArray ptos_l1= lines[0].getNodePosForward();
    Pos3dArray ptos_l2= lines[1].getNodePosForward();
    Pos3dArray ptos_l3= lines[2].getNodePosReverse(); //Ordenados al revés.
    Pos3dArray ptos_l4= lines[3].getNodePosReverse(); //Ordenados al revés.
    retval= Pos3dArray(ptos_l1,ptos_l2,ptos_l3,ptos_l4);
    retval.Trn();
    return retval;
  }

//! @brief Returns a vector in the direction of the local
//! X axis.
Vector3d XC::QuadSurface::getIVector(void) const
  {
    const Pos3d p1= getVertex(1)->getPos();
    const Pos3d p2= getVertex(2)->getPos();
    const Pos3d p3= getVertex(3)->getPos();
    const Pos3d p4= getVertex(4)->getPos();
    Vector3d retval= 0.5*((p2-p1)+(p3-p4));
    retval.Normalize();
    return retval;
  }

//! @brief Returns a vector in the direction of the local
//! Y axis.
Vector3d XC::QuadSurface::getJVector(void) const
  {
    const Pos3d p1= getVertex(1)->getPos();
    const Pos3d p2= getVertex(2)->getPos();
    const Pos3d p3= getVertex(3)->getPos();
    const Pos3d p4= getVertex(4)->getPos();
    Vector3d i= 0.5*((p2-p1)+(p3-p4));
    i.Normalize();
    Vector3d j= 0.5*((p4-p1)+(p3-p2));
    j.Normalize();
    Vector3d k= i.getCross(j);
    return k.getCross(i);
  }

//! @brief Creates surface nodes.
void XC::QuadSurface::create_nodes(void)
  {
    checkNDivs();
    if(ttzNodes.Null())
      {
        create_line_nodes();

        Side &ll0= lines[0]; // columns.
	Side &ll1= lines[1]; // rows.
	Side &ll2= lines[2]; // columns.
	Side &ll3= lines[3]; // rows.
	if(ll0.NDiv()!=NDivI())
	  {
	    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	              << "; wrong number of divisions on I side of surface: "
	              << getTag()
	              << " (different from number of divisions in lines 0 and 2)"
		      << ". Using nDivI= " <<  ll0.NDiv()
		      << Color::def << std::endl;
	  }
	if(ll1.NDiv()!=NDivJ())
	  {
	    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	              << "; wrong number of divisions on J side of surface: "
	              << getTag()
	              << " (different from number of divisions in lines 1 and 3)"
		      << ". Using nDivJ= " <<  ll1.NDiv()
		      << Color::def << std::endl;
	  }
        const size_t n_rows= ll1.NDiv()+1;
        const size_t n_cols= ll0.NDiv()+1;
        ttzNodes= NodePtrArray3d(1,n_rows,n_cols);


        //Set the pointers of the contour nodes.
        //j=1
        for(size_t k=1;k<=n_cols;k++)
          {
            Node *nn= ll0.getNode(k);
            ttzNodes(1,1,k)= nn;
          }
        //j=n_rows.
        for(size_t k=1;k<=n_cols;k++) // Reverse.
	  {
	    XC::Node *ptr= ll2.getNodeReverse(k);
            ttzNodes(1,n_rows,k)= ptr;
	  }
	
        //k=1
        for(size_t j=2;j<n_rows;j++) // Reverse.
	  {
	    XC::Node *ptr= ll3.getNodeReverse(j);
            ttzNodes(1,j,1)= ptr;
	  }
        //k=n_cols.
        for(size_t j=2;j<n_rows;j++)
          ttzNodes(1,j,n_cols)= ll1.getNode(j);

        //Populate the interior nodes.
        Pos3dArray node_pos= get_positions(); //Node positions.
        for(size_t j= 2;j<n_rows;j++) //interior rows.
          for(size_t k= 2;k<n_cols;k++) //interior columns.
            create_node(node_pos(j,k),1,j,k);
      }
    else
      if(verbosity>2)
        std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	          << "; nodes of entity: '" << getName()
		  << "' already exist."
		  << Color::def << std::endl;
    
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
        std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	          << "; elements for surface: '" << getName()
		  << "' already exist."
		  << Color::def << std::endl;      
    if(verbosity>3)
      std::clog << "done." << std::endl;
  }
