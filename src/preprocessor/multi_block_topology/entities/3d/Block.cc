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
//Block.cc

#include "Block.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/multi_block_topology/entities/2d/QuadSurface.h"
#include "utility/matrices/m_int.h"
#include "utility/geom/d3/BND3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"
#include "utility/geom/pos_vec/Pos3dArray.h"
#include "utility/geom/pos_vec/Pos3dArray3d.h"
#include "utility/geom/pos_vec/Pos3dList.h"
#include "utility/geom/d3/3d_polyhedrons/Hexahedron3d.h"
#include "vtkCellType.h"
#include "utility/utils/misc_utils/colormod.h"

// Numbering of vertices:
//
//       5        8
//       +--------+
//      /        /|
//     /      7 / |
//  6 +--------+  |
//    |        |  |
//    |        |  + 4
//    |        | /
//    |        |/
//    +--------+
//    2        3
//
// Numbering of faces:
//
//                
//       +--------+         0: Bottom face; vertices 1,4,3,2. (0,3,2,1)
//      /   5    /|         1: Left-side face; vertices 1,2,6,5. (0,1,5,4)
//     /        / |         2: Front face; vertices 2,3,7,6. (1,2,6,5)
//    +--------+  |         3: Right-side face; vertices 3,4,8,7. (2,3,7,6)
//    |        | 3|         4: Back face; vertices 1,5,8,4. (0,4,7,3)
//    |        |  +         5: Top face; vertices 5,6,7,8. (4,5,6,7)
//    |   2    | /
//    |        |/
//    +--------+
//

// Face indexes.
const std::vector< std::tuple<size_t, size_t, size_t, size_t> > face_vertices
  {
    std::tuple<size_t, size_t, size_t, size_t> {0, 3, 2, 1}, // vertices of face 0
    std::tuple<size_t, size_t, size_t, size_t> {0, 1, 5, 4}, // vertices of face 1
    std::tuple<size_t, size_t, size_t, size_t> {1, 2, 6, 5}, // vertices of face 2
    std::tuple<size_t, size_t, size_t, size_t> {2, 3, 7, 6}, // vertices of face 3
    std::tuple<size_t, size_t, size_t, size_t> {0, 4, 7, 3}, // vertices of face 4
    std::tuple<size_t, size_t, size_t, size_t> {4, 5, 6, 7}  // vertices of face 5
  };

//! @brief Constructor.
XC::Block::Block(Preprocessor *m,const std::string &name)
  : Body(m,name) {}

//! @brief Comparison operator.
bool XC::Block::operator==(const Block &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= Body::operator==(other);
        if(retval)
          retval= ((sups[0]==other.sups[0]) && (sups[1]==other.sups[1]) && (sups[2]==other.sups[2]) && (sups[3]==other.sups[3]) && (sups[4]==other.sups[4]) && (sups[5]==other.sups[5]));
       }
    return retval;
  }

//! @brief Virtual constructor.
XC::SetEstruct *XC::Block::getCopy(void) const
  { return new Block(*this); }

//! @brief Interface with VTK.
int XC::Block::getVtkCellType(void) const
  { return VTK_HEXAHEDRON; }

//! @brief Returns a list with the edges that have an incompatible number of divisions.
std::deque<const XC::Edge *> XC::Block::getNDivErrors(void) const
  {
    std::deque<const Edge *> retval;
    for(int i= 0; i<6; i++)
      {
        std::deque<const Edge *> tmp= this->sups[i].getNDivErrors();
        for(std::deque<const Edge *>::const_iterator j= tmp.begin(); j!= tmp.end(); j++)
	      retval.push_back(*j);
      }
    return retval;
  }
//! @brief Check that number of divisions of the lines are compatible.
bool XC::Block::checkNDivs(void) const
  {
    return (sups[0].checkNDivs() && sups[1].checkNDivs() && sups[2].checkNDivs() && sups[3].checkNDivs() && sups[4].checkNDivs() && sups[5].checkNDivs());
  }

//! @brief Return the number of edges.
size_t XC::Block::getNumberOfLines(void) const
  { return 12; }

//! @brief Return the number of vertices.
size_t XC::Block::getNumberOfVertices(void) const
  { return 8; }

//! @brief Return the number of faces.
size_t XC::Block::getNumberOfFaces(void) const
  { return 6; }

//! @brief Return the number of already defined faces.
size_t XC::Block::getNumberOfDefinedFaces(void) const
  {
    size_t retval= 0;
    for(size_t i= 0;i<6;i++)
      if(sups[i].Surface()) retval++;
    return retval;
  }


//! @brief Return the face with the index passed as parameter.
const XC::Block::BodyFace *XC::Block::getFace(const size_t &i) const
  { return &sups[i-1]; }

//! @brief Return the face with the index passed as parameter.
XC::Block::BodyFace *XC::Block::getFace(const size_t &i)
  { return &sups[i-1]; }

//! @brief Creates a new face between the points being passed as parameters
//! and inserts it on the faces set.
//! @param i: index of the face in this block.
//! @param pA: first vertex of the new (or existing) face.
//! @param pB: second vertex of the new (or existing) face.
//! @param pC: third vertex of the new (or existing) face.
//! @param pD: fourth vertex of the new (or existing) face.
XC::Face *XC::Block::newFace(const size_t &i,Pnt *pA,Pnt *pB,Pnt *pC,Pnt *pD)
  {
    Face *retval= nullptr;
    assert(getPreprocessor());
    retval= dynamic_cast<Face *>(getPreprocessor()->getMultiBlockTopology().getSurfaces().findOrCreateFace(pA,pB,pC,pD));
    if(retval)
      { put(i, retval); }
    else
       std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		 << "; surface with vertices: "
                 << pA->getName() << ", " << pB->getName()
                 << pC->getName() << " and " << pB->getName()
		 << " not found in definition of block: '"
                 << getName() << "'"
		 << Color::def << std::endl;
    return retval;
  }

//! @brief Return the i-th edge of the solid.
XC::CmbEdge::Side *XC::Block::getEdge(const size_t &i)
  {
    CmbEdge::Side *retval(nullptr);
    switch(i)
      {
        case 1:
        case 2:
        case 3:
        case 4:
          retval=sups[0].getSide(i);
          break;
        case 5:
        case 6:
        case 7:
        case 8:
          retval= sups[i-4].getSide(4);
          break;
        case 9:
        case 10:
        case 11:
        case 12:
          retval= sups[5].getSide(i-8);
          break;
        default:
          retval= nullptr;
          break;
      }
    return retval;
  }
//! @brief Return the i-th edge of the solid.
const XC::CmbEdge::Side *XC::Block::getEdge(const size_t &i) const
  {
    Block *this_no_const= const_cast<Block *>(this);
    return this_no_const->getEdge(i);
  }

//! @brief Return the i-th vertex of the solid.
const XC::Pnt *XC::Block::getVertex(const size_t &i) const
  {
    Block *this_no_const= const_cast<Block *>(this);
    return this_no_const->getVertex(i);
  }

//       +--------+         0: Bottom face; vertices 1,4,3,2. (0,3,2,1)
//      /   5    /|         1: Left-side face; vertices 1,2,6,5. (0,1,5,4)
//     /        / |         2: Front face; vertices 2,3,7,6. (1,2,6,5)
//    +--------+  |         3: Right-side face; vertices 3,4,8,7. (2,3,7,6)
//    |        | 3|         4: Back face; vertices 1,5,8,4. (0,4,7,3)
//    |        |  +         5: Top face; vertices 5,6,7,8. (4,5,6,7)

//! @brief Return the i-th vertex of the solid.
XC::Pnt *XC::Block::getVertex(const size_t &i)
  {
    Pnt *retval= nullptr;
    if(i<=4)
      retval= sups[0].getVertex(i); //Bottom
    else if(i<=8) //Top
      {
        std::set<const Pnt *> intersection;
        if(i==5)
	  intersection= getCommonVertex(*sups[1].Surface(),*sups[4].Surface(),*sups[5].Surface());
	else if(i==6)
	  intersection= getCommonVertex(*sups[1].Surface(),*sups[2].Surface(),*sups[5].Surface());
	else if(i==7)
	  intersection= getCommonVertex(*sups[2].Surface(),*sups[3].Surface(),*sups[5].Surface());
	else if(i==8)
	  intersection= getCommonVertex(*sups[3].Surface(),*sups[4].Surface(),*sups[5].Surface());
	assert(intersection.size()==1);
	return retval= const_cast<Pnt *>(*intersection.begin()); 
      }
    return retval;
  }


//! @brief Return the surfaces that close the solid.
std::set<const XC::Face *> XC::Block::getSurfaces(void) const
  {
    std::set<const Face *> retval;
    if(!sups[0].Vacia()) retval.insert(sups[0].Surface());
    if(!sups[1].Vacia()) retval.insert(sups[1].Surface());
    if(!sups[2].Vacia()) retval.insert(sups[2].Surface());
    if(!sups[3].Vacia()) retval.insert(sups[3].Surface());
    if(!sups[4].Vacia()) retval.insert(sups[4].Surface());
    if(!sups[5].Vacia()) retval.insert(sups[5].Surface());
    return retval;
  }

//! @brief Return the surfaces that close the solid.
std::set<XC::Face *> XC::Block::getSurfaces(void)
  {
    std::set<Face *> retval;
    if(!sups[0].Vacia()) retval.insert(sups[0].Surface());
    if(!sups[1].Vacia()) retval.insert(sups[1].Surface());
    if(!sups[2].Vacia()) retval.insert(sups[2].Surface());
    if(!sups[3].Vacia()) retval.insert(sups[3].Surface());
    if(!sups[4].Vacia()) retval.insert(sups[4].Surface());
    if(!sups[5].Vacia()) retval.insert(sups[5].Surface());
    return retval;
  }

//! @brief Update topology of the enclosing surface (neighbors).
void XC::Block::update_topology(void)
  {
    if(!sups[0].Vacia()) set_surf(sups[0].Surface());
    if(!sups[1].Vacia()) set_surf(sups[1].Surface());
    if(!sups[2].Vacia()) set_surf(sups[2].Surface());
    if(!sups[3].Vacia()) set_surf(sups[3].Surface());
    if(!sups[4].Vacia()) set_surf(sups[4].Surface());
    if(!sups[5].Vacia()) set_surf(sups[5].Surface());
  }

//! Return the index of the surface passed as parameter
//! as follows:
//! - If the surface is the first one that defines the solid,
//! then that one is the base.
//! - If one or more of the faces are already defined, we search for the index
//!  on the base of the line in common with it.
//! If this line exists, that index corresponds to the surface in the solid.
size_t XC::Block::index(Face *s) const
  {
    size_t retval= 0;
    if(sups[0].Vacia())
      retval= 0; //It's the base.
    else //Base is already set.
      {
        const Face *base= sups[0].Surface();
        size_t first= base->CommonEdge(*s); //Line shared by "s" and the base.
        if(first)
          retval= first; //Is a side face.
        else
          retval= 5; //Is the top face.
      }
    return retval;
  }

//! @brief Put the surface as solid limit.
void XC::Block::put(const size_t &i,Face *s)
  {
    size_t first= 1;
    int sense= 1;
    if((i>0) && (i<5)) //Is a side face.
      {
        const Face *base= sups[0].Surface();
        first= s->CommonEdge(*base); //Index of the line in common with the base.
        const Edge *line= s->getSide(first)->getEdge();
        sense= base->SenseOfEdge(line,*s);
      }
    if(i == 5) //Is the top face
      {
        //Seek for an assigned face.
        size_t iFace= 1;
        Face *face= sups[iFace].Surface();
        if(!face) { iFace=2; face= sups[iFace].Surface(); }
        if(!face) { iFace=3; face= sups[iFace].Surface(); }
        if(!face) { iFace=4; face= sups[iFace].Surface(); }
        if(!face)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; error before introducing face 5 you must introduce"
	            << " either the 1 , 2 , 3 or 4 faces."
		    << Color::def << std::endl;
        else
          {
            first= face->CommonEdge(*s); //Index of the line in common of s with the face.
            if(first) //They have a common edge.
              {
                const Edge *linea= face->getSide(first)->getEdge();
                sense= -face->SenseOfEdge(linea,*s);
              }
            else //They don't share a common edge.
              {
                std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			  << "; error: Block(" << getName() << "); the face "
			  << s->getName() 
                          << " does not share a common edge with face "
			  << face->getName() << '.'
			  << Color::def << std::endl;
              }
          }
      }
    bool forward= true;
    if(sense==1)
      forward= true;
    else
      if(sense==-1)
        forward= false;
      else
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; the surfaces do not share a common edge."
		  << Color::def << std::endl;
    sups[i]= BodyFace(this,s,first,forward);
  }

//! @brief Insert the surface with the identifier passed as parameter
//! (if found).
void XC::Block::insert(const size_t &i)
  {
    Face *s= BuscaFace(i);
    if(s)
      put(index(s),s);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; surface: " << i
		<< " not found."
		<< Color::def << std::endl;
  }

//! @brief Create and insert the faces from the indices passed
//! as parameter.
void XC::Block::append_faces(const std::vector<size_t> &face_indexes)
  {
    const size_t nc= face_indexes.size(); //Number of indices.
    for(size_t i= 0;i<nc;i++)
      insert(face_indexes[i]);
  }

//! @brief Trigger the creation of nodes on faces.
void XC::Block::create_face_nodes(void)
  {
    for(size_t i= 0; i<6; i++)
      sups[i].create_nodes();
  }

Pos3d XC::Block::getCentroid(void) const
  {
    Pos3dList tmp;
    const Pos3d &p1= getVertex(1)->getPos();
    tmp.appendPoint(p1);
    const Pos3d &p2= getVertex(2)->getPos();
    tmp.appendPoint(p2);
    const Pos3d &p3= getVertex(3)->getPos();
    tmp.appendPoint(p3);
    const Pos3d &p4= getVertex(4)->getPos();
    tmp.appendPoint(p4);
    
    const Pos3d &p5= getVertex(5)->getPos();
    tmp.appendPoint(p5);
    const Pos3d &p6= getVertex(6)->getPos();
    tmp.appendPoint(p6);
    const Pos3d &p7= getVertex(7)->getPos();
    tmp.appendPoint(p7);
    const Pos3d &p8= getVertex(8)->getPos();
    tmp.appendPoint(p8);
    return tmp.getCenterOfMass();
  }

//! @brief Return (ndivI+1)*(ndivJ+1)*(ndivK+1) positions for the nodes.
//!
//! The returned 3D-matrix of points is organized as follows:
//! - The point (0,1,1) (layer,row,column) corresponds to vertex  1.
//! - The layer index ranges from 0 for face 1 to numberOfLayers for face 6.
//! - The row index ranges from 1 for face 5 to numberOfRows for face 3.
//! - The column index rangess from 1 for face 2 to ncol for face 4.
//!
//! So:
//! - Edge 1 has indices (0,j=1..nfil,1) 
//! - Edge 2 has indices (0,nfil,k=1..ncol) 
//! - Edge 3 has indices (0,j=1..nfil,ncol)
//! - Edge 4 has indices (0,1,k=1..ncol)

//! - Edge 5 has indices (i=0..numberOfLayers,1,1) 
//! - Edge 6 has indices (i=0..numberOfLayers,nfil,1) 
//! - Edge 7 has indices (i=0..numberOfLayers,nfil,ncol) 
//! - Edge 8 has indices (i=0..numberOfLayers,1,ncol) 

//! - Edge 9 has indices (numberOfLayers,j=1..nfil,1) 
//! - Edge 10 has indices (numberOfLayers,nfil,k=1..ncol) 
//! - Edge 11 has indices (numberOfLayers,j=1..nfil,ncol) 
//! - Edge 12 has indices (numberOfLayers,1,k=1..ncol) 
Pos3dArray3d XC::Block::get_positions(void) const
  {
    const size_t ndiv_12= NDivJ();
    const size_t ndiv_23= NDivI();
    const size_t ndiv_15= NDivK();

    Pos3dList bottomFace;
    const Pos3d &p1= getVertex(1)->getPos();
    bottomFace.appendPoint(p1);
    const Pos3d &p2= getVertex(2)->getPos();
    bottomFace.appendPoint(p2);
    const Pos3d &p3= getVertex(3)->getPos();
    bottomFace.appendPoint(p3);
    const Pos3d &p4= getVertex(4)->getPos();
    bottomFace.appendPoint(p4);
    
    Pos3dList topFace;
    const Pos3d &p5= getVertex(5)->getPos();
    topFace.appendPoint(p5);
    const Pos3d &p6= getVertex(6)->getPos();
    topFace.appendPoint(p6);
    const Pos3d &p7= getVertex(7)->getPos();
    topFace.appendPoint(p7);
    const Pos3d &p8= getVertex(8)->getPos();
    topFace.appendPoint(p8);

    // Compute face orientation.
    const Pos3d centroid= this->getCentroid();
    bool bottomFaceClockWise= bottomFace.clockwise(centroid);
    bool topFaceClockWise= bottomFace.clockwise(centroid);
    const Pos3dArray ptos_l15= Pos3dArray(p1,p5,ndiv_15);
    const Pos3dArray ptos_l37= Pos3dArray(p3,p7,ndiv_15);
    Pos3dArray3d retval;
    if(bottomFaceClockWise && topFaceClockWise)
      {
	const Pos3dArray ptos_l46= Pos3dArray(p4,p6,ndiv_15);
	const Pos3dArray ptos_l28= Pos3dArray(p2,p8,ndiv_15);
        retval= Pos3dArray3d(ptos_l15, ptos_l46, ptos_l37, ptos_l28, ndiv_12, ndiv_23);
      }
    else if(!bottomFaceClockWise && !topFaceClockWise)
      {
	const Pos3dArray ptos_l26= Pos3dArray(p2,p6,ndiv_15);
	const Pos3dArray ptos_l48= Pos3dArray(p4,p8,ndiv_15);
        retval= Pos3dArray3d(ptos_l15, ptos_l26, ptos_l37, ptos_l48, ndiv_12, ndiv_23);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; wrong orientation of vertices for block: "
		<< this->getTag()
		<< Color::def << std::endl;
    // const size_t n_layers= retval.getNumberOfLayers();
    // const size_t n_rows= retval.getNumberOfRows();
    // const size_t n_cols= retval.getNumberOfColumns();
    // for(size_t i= 1; i<=n_layers; i++)
    //   for(size_t j= 1; j<=n_rows; j++)
    // 	for(size_t k= 1; k<=n_cols; k++)
    // 	  {
    // 	    bool interior= (i!=1 && i!=n_layers) && (j!=1 && j!=n_rows) && (k!=1 && k!=n_rows);
    // 	    std::cout << "(" << i << ", " << j << "; " << k << ") pos= "
    // 		      << retval(i,j,k)
    // 		      << " interior: " << interior << std::endl;
    // 	  }
    return retval;
  }

//! @brief Return the positions that will be used for the nodes when meshing.
boost::python::list XC::Block::getPositionsPy(void) const
  {
    boost::python::list retval;
    Pos3dArray3d node_pos= this->get_positions(); //Node positions.
    const size_t n_layers= node_pos.getNumberOfLayers();
    const size_t n_rows= node_pos.getNumberOfRows();
    const size_t n_cols= node_pos.getNumberOfColumns();
    for(size_t k= 1; k<=n_layers; k++)
      {
	boost::python::list row_list;
	for(size_t i= 1; i<=n_rows; i++)
	  {
	    boost::python::list row;
	    for(size_t j= 1; j<=n_cols; j++)
	      {
		const Pos3d &pos= node_pos(k, i, j);
		row.append(pos);
	      }
	    row_list.append(row);
	  }
	retval.append(row_list);
      }
    return retval;
  }

//! @brief Return the number of divisions along the edge 1->2.
size_t XC::Block::NDivI(void) const
  { return getEdge(1)->NDiv(); }

void XC::Block::setNDivI(const size_t &nDiv)
  {
    getEdge(1)->setNDivHomologousEdges(nDiv);
    for(size_t i= 0;i<6;i++)
      sups[i].Surface()->ConciliaNDivIJ();
  }

//! @brief Return the number of divisions along the edge 2->3.
size_t XC::Block::NDivJ(void) const
  { return getEdge(2)->NDiv(); }

void XC::Block::setNDivJ(const size_t &nDiv)
  {
    getEdge(2)->setNDivHomologousEdges(nDiv);
    for(size_t i= 0;i<6;i++)
      sups[i].Surface()->ConciliaNDivIJ();
  }

//! @brief Return the number of divisions along the edge 1->5.
size_t XC::Block::NDivK(void) const
  { return getEdge(5)->NDiv(); }

void XC::Block::setNDivK(const size_t &nDiv)
  {
    getEdge(5)->setNDivHomologousEdges(nDiv);
    for(size_t i= 0;i<6;i++)
      sups[i].Surface()->ConciliaNDivIJ();
  }

typedef std::tuple<size_t, size_t, size_t> ijk_node_key;

struct ijk_node_key_hash : public std::unary_function<ijk_node_key, std::size_t>
  {
    std::size_t operator()(const ijk_node_key& k) const
    {
      return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
    }
  };
struct ijk_node_key_equal : public std::binary_function<ijk_node_key, ijk_node_key, bool>
  {
    bool operator()(const ijk_node_key& v0, const ijk_node_key& v1) const
      {
	return (std::get<0>(v0) == std::get<0>(v1) && std::get<1>(v0) == std::get<1>(v1) && std::get<2>(v0) == std::get<2>(v1));
      }
  };
 
typedef std::unordered_map<const ijk_node_key, Pos3d, ijk_node_key_hash, ijk_node_key_equal> map_pending_node_positions;

//! @brief Create nodes for the block.
void XC::Block::create_nodes(void)
  {
    checkNDivs();
    if(ttzNodes.Null())
      {
        create_face_nodes();

        Pos3dArray3d node_pos= this->get_positions(); //Node positions.
        const size_t n_layers= node_pos.getNumberOfLayers();
        const size_t n_rows= node_pos.getNumberOfRows();
        const size_t n_cols= node_pos.getNumberOfColumns();
        ttzNodes= NodePtrArray3d(n_layers, n_rows, n_cols); //Pointers to node.

	// Populate pending_node_positions with all the position in the faces
	// of the block.
	map_pending_node_positions pending_node_positions;
	const std::vector<size_t> layers({1, n_layers}); // First and last layer.
	for(std::vector<size_t>::const_iterator il= layers.begin(); il!= layers.end(); il++)
	  {
	    for(size_t i= 1; i<=n_rows; i++)
	      for(size_t j= 1; j<=n_cols; j++)
		{
		  const size_t i_layer= *il;
		  const Pos3d &pos= node_pos(i_layer, i, j);
		  pending_node_positions[std::make_tuple(i_layer, i, j)]= pos;
		}
	  }
	const std::vector<size_t> rows({1, n_rows}); // First and last row.
	for(std::vector<size_t>::const_iterator ir= rows.begin(); ir!= rows.end(); ir++)
	  {
	    for(size_t i= 1; i<=n_layers; i++)
	      for(size_t j= 1; j<=n_cols; j++)
		{
		  const size_t i_row= *ir;
		  const Pos3d &pos= node_pos(i, i_row, j);
		  pending_node_positions[std::make_tuple(i, i_row, j)]= pos;
		}
	  }
	const std::vector<size_t> columns({1, n_cols}); // First and last column.
	for(std::vector<size_t>::const_iterator ic= columns.begin(); ic!= columns.end(); ic++)
	  {
	    for(size_t i= 1; i<=n_layers; i++)
	      for(size_t j= 1; j<=n_rows; j++)
		{
		  const size_t i_col= *ic;
		  const Pos3d &pos= node_pos(i, j, i_col);
		  pending_node_positions[std::make_tuple(i, j, i_col)]= pos;
		}
	  }
	// Vertex nodes.
	for(size_t ivertex= 1; ivertex<=8; ivertex++)
	  {
	    const Node *nPtr= getVertex(ivertex)->getNode();
	    const Pos3d pos= nPtr->getInitialPosition3d();
	    std::list<ijk_node_key> keys_to_remove;
	    for(map_pending_node_positions::const_iterator j= pending_node_positions.begin(); j!= pending_node_positions.end(); j++)
	      {
		const Pos3d pB= (*j).second;
		const double d2= dist2(pos,pB);
		if(d2<1e-4) // found.
		  {
		    const ijk_node_key &key= (*j).first;
		    const size_t ii= get<0>(key);
		    const size_t jj= get<1>(key);
		    const size_t kk= get<2>(key);
		    ttzNodes(ii, jj, kk)= const_cast<Node *>(nPtr);
		    keys_to_remove.push_back(key);
		  }
	      }
	    const size_t num_keys_to_remove= keys_to_remove.size();
	    if(num_keys_to_remove==0)
	      {
		std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			  << "; node: " << nPtr->getTag()
			  << " in position: " << pos << " not found."
			  << Color::def << std::endl;
	      }
	    for(std::list<ijk_node_key>::const_iterator k= keys_to_remove.begin(); k!= keys_to_remove.end();k++)
	      { pending_node_positions.erase(*k); }	    
   	  }
	// Face nodes.
	for(size_t iface= 0; iface<6; iface++)
	  {

	    const BodyFace &currentFace= sups[iface];
	    const Face *face= currentFace.Surface();
	    const std::list<const Node *> faceNodes= face->getNodes();
	    for(std::list<const Node *>::const_iterator i= faceNodes.begin(); i!= faceNodes.end(); i++)
	      {
		const Node *n= *i;
		const Pos3d pos= n->getInitialPosition3d();
		std::list<ijk_node_key> keys_to_remove;
		for(map_pending_node_positions::const_iterator j= pending_node_positions.begin(); j!= pending_node_positions.end(); j++)
		  {
		    const Pos3d pB= (*j).second;
		    const double d2= dist2(pos,pB);
		    if(d2<1e-4) // found.
		      {
			const ijk_node_key &key= (*j).first;
			const size_t ii= get<0>(key);
			const size_t jj= get<1>(key);
			const size_t kk= get<2>(key);
			ttzNodes(ii, jj, kk)= const_cast<Node *>(n);
		        keys_to_remove.push_back(key);
		      }
		  }
		for(std::list<ijk_node_key>::const_iterator k= keys_to_remove.begin(); k!= keys_to_remove.end();k++)
		  { pending_node_positions.erase(*k); }
	      }
	  }
	const size_t num_pending_nodes= pending_node_positions.size();
	if(num_pending_nodes!=0)
	  {
	    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		      << "; some face nodes where not detected: " << std::endl;
	    for(map_pending_node_positions::const_iterator j= pending_node_positions.begin(); j!= pending_node_positions.end(); j++)
	      {
		const Pos3d pos= (*j).second;
		const ijk_node_key &key= (*j).first;
		const size_t ii= get<0>(key);
		const size_t jj= get<1>(key);
		const size_t kk= get<2>(key);
		std::cerr << "node: (" << ii << " ," << jj << " ," << kk << ")"
			  << " in position: " << pos << std::endl;
	      }
	    std::cerr << Color::def << std::endl;
	  }
	
        // Interior nodes. 
        for(size_t i= 2;i<n_layers;i++) //interior layers.
          for(size_t j= 2;j<n_rows;j++) //interior rows.
            for(size_t k= 2;k<n_cols;k++) //interior columns.
              create_node(node_pos(i,j,k),i,j,k);
	
	for(size_t i= 1; i<=n_layers; i++)
	  for(size_t j= 1; j<=n_rows; j++)
	    for(size_t k= 1; k<=n_cols; k++)
	      {
		const Node *nPtr= ttzNodes(i,j,k);
		if(!nPtr) // null pointer.
		  {
		    bool interior= (i!=1 && i!=n_layers) && (j!=1 && j!=n_rows) && (k!=1 && k!=n_rows);
		    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			      << "; null pointer at position: ("
			      << i << ", " << j << ", " << k << ") pos= "
			      << node_pos(i,j,k)
			      << " interior: " << interior
			      << Color::def << std::endl;
		  }
	      }
      }
    else
      if(verbosity>2)
        std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	          << "; nodes of entity: '" << getName()
		  << "' already exist."
		  << Color::def << std::endl;      
  }

//! @brief Trigger mesh generation.
void XC::Block::genMesh(meshing_dir dm)
  {
    if(verbosity>3)
      std::clog << Color::yellow << "Meshing Block...(" << getName() << ")...";
    create_nodes();
    if(ttzElements.Null())
      create_elements(dm);
    else
      if(verbosity>2)
        std::clog << getClassName() << "::" << __FUNCTION__
	          << "; nodes of entity: '" << getName()
		  << "' already exist." << std::endl;      
    if(verbosity>3)
      std::clog << "done."
		<< Color::def << std::endl;
  }
//! @brief Creates and inserts the the faces that link the points
//! from the indexes being passed as parameter.
void XC::Block::addPoints(const ID &point_indexes)
  {
    const size_t np= point_indexes.Size(); //Number of indexes.
    if(np==8)
      {
	std::vector<Pnt *> pntPtrs(8,nullptr);
	for(size_t i= 0;i<np;i++)
	  {

	    Pnt *p= BuscaPnt(point_indexes(i));
	    if(!p)
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; point: " << point_indexes(i-1)
			<< " not found in definition of surface: '"
			<< getName() << "'"
			<< Color::def << std::endl;
	    pntPtrs[i]= p;
	  }
	//
	//     5 +---------+ 8     0: Bottom face; vertices 1,4,3,2. (0,3,2,1)
	//      /|        /|       1: Left-side face; vertices 1,2,6,5. (0,1,5,4)
	//     / |       / |       2: Front face; vertices 2,3,7,6. (1,2,6,5)
	//  6 +---------+7 |       3: Right-side face; vertices 3,4,8,7. (2,3,7,6)
	//    |  |      |  |       4: Back face; vertices 1,5,8,4. (0,4,7,3)
	//    |1 +------|--+ 4     5: Top face; vertices 5,6,7,8. (4,5,6,7)
	//    | /       | /
	//    |/        |/
	//  2 +---------+ 3
	//
	for(size_t ii= 0; ii<6; ii++)
	  {
	    const std::tuple<size_t, size_t, size_t, size_t> &tpl= face_vertices[ii];
	    const size_t i= get<0>(tpl); const size_t j= get<1>(tpl);
	    const size_t k= get<2>(tpl); const size_t l= get<3>(tpl);
	    this->newFace(ii, pntPtrs[i],pntPtrs[j],pntPtrs[k],pntPtrs[l]);
	  }
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; exactly eight points are needed, we got: "
		<< np
		<< Color::def << std::endl;
  }

//! @brief Creates and inserts the faces from the points identified
//! by the indexes being passed as parameter.
void XC::Block::setPoints(const ID &point_indexes)
  {
    const size_t np= point_indexes.Size(); //Number of indexes.
    if(np!=8)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; block definition needs "
                << 8 << " points, we got: " << np << "."
	        << Color::def << std::endl;
    else
      {
        if(getNumberOfDefinedFaces()>0)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << "; warning redefinition of block: '"
                    << getName() << "'."
	            << Color::def << std::endl;

	addPoints(point_indexes);
      }
  }

double XC::Block::getVolume(void) const
  {
    const std::deque<const Pnt *> vertices= this->getVertices();
    std::vector<int> order({0, 3, 2, 1, 4, 5, 6, 7});
    const Pos3d p0= vertices[order[0]]->getPos();
    const Pos3d p1= vertices[order[1]]->getPos();
    const Pos3d p2= vertices[order[2]]->getPos();
    const Pos3d p3= vertices[order[3]]->getPos();
    const Pos3d p4= vertices[order[4]]->getPos();
    const Pos3d p5= vertices[order[5]]->getPos();
    const Pos3d p6= vertices[order[6]]->getPos();
    const Pos3d p7= vertices[order[7]]->getPos();
    const Hexahedron3d h(p0, p1, p2, p3, p4, p5, p6, p7);
    const double retval= h.getVolume();
    return retval;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::Block::getPyDict(void) const
  {
    boost::python::dict retval= Body::getPyDict();
    boost::python::list faces;
    for(size_t i= 0; i<6; i++)
      {
	const BodyFace &bodyFace= sups[i];
	faces.append(bodyFace.getPyDict());
      }
    retval["faces"]= faces;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::Block::setPyDict(const boost::python::dict &d)
  {
    Body::setPyDict(d);
    boost::python::list faceList= boost::python::extract<boost::python::list>(d["faces"]);
    const size_t numFaces= boost::python::len(faceList);
    if(numFaces!=6)
      std::cerr << Color::red << getClassName() << __FUNCTION__
	        << "; six faces expected, got: "
		<< numFaces
		<< Color::def << std::endl;
    Preprocessor *prep= getPreprocessor();
    if(prep)
      {
	MultiBlockTopology &mbt= prep->getMultiBlockTopology();
	SurfaceMap &surfaces= mbt.getSurfaces();
	for(size_t i= 0; i<numFaces; i++)
	  {
	    const boost::python::dict faceDict= boost::python::extract<boost::python::dict>(faceList[i]);
	    sups[i].setPyDict(surfaces, faceDict);
	  }
	this->update_topology();
      }
    else
      std::cerr << Color::red << getClassName() << __FUNCTION__
	        << "; preprocessor needed."
		<< Color::def << std::endl;
  }
