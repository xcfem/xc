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
//PolygonalFace.cc

#include "PolygonalFace.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/multi_block_topology/MultiBlockTopology.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "preprocessor/Preprocessor.h"

//! @brief Constructor.
XC::PolygonalFace::PolygonalFace(Preprocessor *m)
  : Face(m,0,0) {}

//! @brief Virtual constructor.
XC::SetEstruct *XC::PolygonalFace::getCopy(void) const
  { return new PolygonalFace(*this); }

//! @brief Computes the number of divisions on the i axis
//! for an element edge size equal or lesser than the
//! size being passed as parameter.
//!
//! @param sz: length of the element side.
//! @param mustBeEver: true if the number of divisions of each
//!                    line must be even.
void XC::PolygonalFace::SetElemSize(const double &sz, bool mustBeEven)
  {
    const size_t numSides= getNumberOfEdges();
    for(size_t i= 0;i<numSides; i++)
      {
        Edge *edge= lines[i].getEdge();
        const double l= edge->getLength();
        size_t n= ceil(l/sz);
	if(mustBeEven && (n % 2 != 0))
	  n++;
        edge->setNDiv(n);
      }
  }

//! @brief Creates and inserts the lines from the points identified
//! by the indexes being passed as parameter.
void XC::PolygonalFace::setPoints(const ID &point_indexes)
  {
    const size_t np= point_indexes.Size(); //Number of indexes.
    if(np<3)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; surface definition needs "
                << 3 << " points or more, we got: " << np << ".\n";
    else
      {
        if(getNumberOfEdges()>0)
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; warning redefinition of surface: '"
                    << getName() << "'.\n";

	Face::addPoints(point_indexes);
        close();
      }
    int tagV1= getVertex(1)->getTag();
    if(tagV1!=point_indexes(0))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; surface: " << getTag()
                << "is inverted." << std::endl;
  }

//! @brief Creates surface nodes.
void XC::PolygonalFace::create_nodes(void)
  {
    if(ttzNodes.Null())
      {
        create_line_nodes();

        const size_t n_rows= 1;
	const size_t numSides= getNumberOfEdges();
	size_t n_cols= 0;
	for(size_t i= 0;i<numSides; i++)
	  {
	    Edge *edge= lines[i].getEdge();
	    n_cols+= edge->NDiv();
	  }
        ttzNodes= NodePtrArray3d(1,n_rows,n_cols);

        //Set the pointers of the contour nodes.
	for(size_t j= 0; j<numSides; j++)
	  for(size_t k=1;k<=n_cols;k++)
	    {
	      Side &ll= lines[j];
	      Node *nn= ll.getNode(k);
	      ttzNodes(1,1,k)= nn;
	    }
      }
    else
      if(verbosity>2)
        std::clog << getClassName() << "::" << __FUNCTION__
	          << "; nodes of entity: '" << getName()
		  << "' already exist." << std::endl;      
  }

//! @brief Triggers mesh creation.
void XC::PolygonalFace::genMesh(meshing_dir dm)
  {
  }
