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
#include "vtkCellType.h"

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
XC::Face *XC::Block::newFace(const size_t &i,Pnt *pA,Pnt *pB,Pnt *pC,Pnt *pD)
  {
    Face *retval= nullptr;
    assert(getPreprocessor());
    retval= dynamic_cast<Face *>(getPreprocessor()->getMultiBlockTopology().getSurfaces().findOrCreateFace(pA,pB,pC,pD));
    if(retval)
      { put(i,retval); }
    else
       std::cerr << getClassName() << "::" << __FUNCTION__
		 << "; surface with vertices: "
                 << pA->getName() << ", " << pB->getName()
                 << pC->getName() << " and " << pB->getName()
		 << " not found in definition of block: '"
                 << getName() << "'" << std::endl;
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
std::set<const XC::Face *> XC::Block::getSurfaces(void)
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
    if( (i>0) && (i<5)) //Is a side face.
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
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; error before introducing face 5 you must introduce"
	            << " either the 1 , 2 , 3 or 4 faces." << std::endl;
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
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; error: Block(" << getName() << "); the face "
			  << s->getName() 
                          << " does not share a common edge with face "
			  << face->getName() << '.' << std::endl;
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
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the surfaces do not share a common edge."
		  << std::endl;
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
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; surface: " << i
		<< " not found." << std::endl;
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
    sups[0].create_nodes();
    sups[1].create_nodes();
    sups[2].create_nodes();
    sups[3].create_nodes();
    sups[4].create_nodes();
    sups[5].create_nodes();
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
    const size_t ndiv_12= NDivI();
    const size_t ndiv_23= NDivJ();
    const size_t ndiv_15= NDivK();
    const Pos3d p1= getVertex(1)->getPos();
    const Pos3d p2= getVertex(2)->getPos();
    const Pos3d p3= getVertex(3)->getPos();
    const Pos3d p4= getVertex(4)->getPos();
    const Pos3d p5= getVertex(5)->getPos();
    const Pos3d p6= getVertex(6)->getPos();
    const Pos3d p7= getVertex(7)->getPos();
    const Pos3d p8= getVertex(8)->getPos();
    const Pos3dArray ptos_l5= Pos3dArray(p1,p5,ndiv_15);
    const Pos3dArray ptos_l6= Pos3dArray(p2,p6,ndiv_15);
    const Pos3dArray ptos_l7= Pos3dArray(p3,p7,ndiv_15);
    const Pos3dArray ptos_l8= Pos3dArray(p4,p8,ndiv_15);
    Pos3dArray3d retval(ptos_l5,ptos_l6,ptos_l7,ptos_l8,ndiv_12,ndiv_23);
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

//! @brief Create nodes for the block.
void XC::Block::create_nodes(void)
  {
    std::cout << "Entra Block::create_nodes" << std::endl;
    checkNDivs();
    if(ttzNodes.Null())
      {
        create_face_nodes();
        BodyFace &bottom= sups[0];
        BodyFace &top= sups[5];
        BodyFace &leftFace= sups[1];
        BodyFace &rightFace= sups[3];
        BodyFace &frontFace= sups[2];
        BodyFace &backFace= sups[4];

        const size_t n_layers= NDivK()+1;
        const size_t n_rows= NDivJ()+1;
        const size_t n_cols= NDivI()+1;
        ttzNodes = NodePtrArray3d(n_layers,n_rows,n_cols); //Pointers to node.
        Pos3dArray3d node_pos= get_positions(); //Node positions.

        //Vertices.
	ttzNodes(1,1,1)= getVertex(1)->getNode();
        std::cout << "Nd1= " << ttzNodes(1,1,1)->getInitialPosition3d() << std::endl;
        ttzNodes(1,n_rows,1)= getVertex(2)->getNode();
        std::cout << "Nd2= " << ttzNodes(1,n_rows,1)->getInitialPosition3d() << std::endl;
	ttzNodes(1,n_rows,n_cols)= getVertex(3)->getNode();
        ttzNodes(1,1,n_cols)= getVertex(4)->getNode();
	ttzNodes(n_layers,1,1)= getVertex(5)->getNode();
        std::cout << "Nd5= " << ttzNodes(n_layers,1,1)->getInitialPosition3d() << std::endl;
        ttzNodes(n_layers,n_rows,1)= getVertex(6)->getNode();
        std::cout << "Nd6= " << ttzNodes(n_layers,n_rows,1)->getInitialPosition3d() << std::endl;
	ttzNodes(n_layers,n_rows,n_cols)= getVertex(7)->getNode();
        ttzNodes(n_layers,1,n_cols)= getVertex(8)->getNode();

        const Node *n1= ttzNodes(1,1,1);
        const Node *n2= ttzNodes(1,n_rows,1);
        const Node *n3= ttzNodes(1,n_rows,n_cols);
        const Node *n4= ttzNodes(1,1,n_cols);
        const Node *n5= ttzNodes(n_layers,1,1);
        const Node *n6= ttzNodes(n_layers,n_rows,1);
        const Node *n7= ttzNodes(n_layers,n_rows,n_cols);
        const Node *n8= ttzNodes(n_layers,1,n_cols);

    std::cout << "n_layers= " << n_layers << std::endl;
    std::cout << "n_rows= " << n_rows << std::endl;
    std::cout << "n_cols= " << n_cols << std::endl;

    std::cout << "bottom" << std::endl;
        //Linking with the nodes of the bottom i=1
        ID IJK1= bottom.Surface()->getNodeIndices(n1);
        ID IJK2= bottom.Surface()->getNodeIndices(n2);
        ID IJK4= bottom.Surface()->getNodeIndices(n4);
	std::cout << "IJK1= " << IJK1 << std::endl;
	std::cout << "IJK2= " << IJK2 << std::endl;
	std::cout << "IJK4= " << IJK4 << std::endl;
        size_t ind_i= 0, ind_j= 0;
        if((IJK2[1]-IJK1[1])>0)
          { ind_i= 1; ind_j= 2; }
        else if((IJK2[2]-IJK1[2])>0)
          { ind_j= 1; ind_i= 2; }
        const size_t nf= abs(IJK2[ind_i]-IJK1[ind_i])+1;
        const size_t nc= abs(IJK4[ind_j]-IJK1[ind_j])+1;
	std::cout << "ind_i= " << ind_i << " nf= " << nf << std::endl;
	std::cout << "ind_j= " << ind_j << " nc= " << nc << std::endl;
        double d2= 0;
        for(size_t i=1;i<=nf;i++)
          for(size_t j=1;j<=nc;j++)
            {
              size_t J= (IJK2[ind_i]-IJK1[ind_i])/(nf-1)*(i-1)+IJK1[ind_i];
              size_t K= (IJK4[ind_j]-IJK1[ind_j])/(nc-1)*(j-1)+IJK1[ind_j];
              if(ind_i<ind_j)
                ttzNodes(1,J,K)= bottom.getNode(i,j);
              else
                ttzNodes(1,J,K)= bottom.getNode(j,i);
              d2= dist2(ttzNodes(1,J,K)->getInitialPosition3d(),node_pos(1,J,K));
              if(d2>1e-4)
		std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; error while linking node: ("
                          << i << "," << j << ") in face." << std::endl;
		std::cout << "i= " << i << " j= " << j
			  << " J= " << J << " K= " << K 
			  << " dist2= " << d2 << std::endl;
            }
	/*
    std::cout << "top" << std::endl;
        //Top i=n_layers
        IJK1= top.Surface()->getNodeIndices(n5);
        IJK2= top.Surface()->getNodeIndices(n7);
        for(size_t i=1;i<=n_rows;i++)
          for(size_t j=1;j<=n_cols;j++)
            {
              size_t J= (IJK2[1]-IJK1[1])/(n_rows-1)*(i-1)+IJK1[1];
              size_t K= (IJK2[2]-IJK1[2])/(n_cols-1)*(j-1)+IJK1[2];
              ttzNodes(n_layers,J,K)= top.getNode(i,j);
            }

        //Lateral izquierdo j=1.
        IJK1= leftFace.Surface()->getNodeIndices(n1);
        IJK2= leftFace.Surface()->getNodeIndices(n6);
        for(size_t i=1;i<=n_rows;i++)
          for(size_t j=1;j<=n_cols;j++)
            {
              size_t J= (IJK2[1]-IJK1[1])/(n_rows-1)*(j-1)+IJK1[1];
              size_t K= (IJK2[2]-IJK1[2])/(n_rows-1)*(j-1)+IJK1[2];
              ttzNodes(n_layers,J,K)= top.getNode(i,j);
            }
	*/

        for(size_t k= 2;k<n_layers;k++) //interior layers.
          for(size_t j= 2;j<n_rows;j++) //interior rows.
            for(size_t i= 2;i<n_cols;i++) //interior columns.
              create_node(node_pos(i,j,k),i,j,k);
      }
    else
      if(verbosity>2)
        std::clog << getClassName() << "::" << __FUNCTION__
	          << "; nodes of entity: '" << getName()
		  << "' already exist." << std::endl;      
  }

//! @brief Trigger mesh generation.
void XC::Block::genMesh(meshing_dir dm)
  {
    if(verbosity>3)
      std::clog << "Meshing Block...(" << getName() << ")...";
    create_nodes();
    if(ttzElements.Null())
      create_elements(dm);
    else
      if(verbosity>2)
        std::clog << getClassName() << "::" << __FUNCTION__
	          << "; nodes of entity: '" << getName()
		  << "' already exist." << std::endl;      
    if(verbosity>3)
      std::clog << "done." << std::endl;
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
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; point: " << point_indexes(i-1)
			<< " not found in definition of surface: '"
			<< getName() << "'" << std::endl;
	    pntPtrs[i]= p;
	  }
	newFace(0,pntPtrs[0],pntPtrs[3],pntPtrs[2],pntPtrs[1]); //Bottom
	newFace(1,pntPtrs[0],pntPtrs[1],pntPtrs[5],pntPtrs[4]); //Left-side face
	newFace(2,pntPtrs[1],pntPtrs[2],pntPtrs[6],pntPtrs[5]); //Front face
	newFace(3,pntPtrs[2],pntPtrs[3],pntPtrs[7],pntPtrs[6]); //Right-side face
	newFace(4,pntPtrs[0],pntPtrs[4],pntPtrs[7],pntPtrs[3]); //Back face
	newFace(5,pntPtrs[4],pntPtrs[5],pntPtrs[6],pntPtrs[7]); //Top
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; exactly eight points are needed, we got: "
		<< np << std::endl;
  }

//! @brief Creates and inserts the faces from the points identified
//! by the indexes being passed as parameter.
void XC::Block::setPoints(const ID &point_indexes)
  {
    const size_t np= point_indexes.Size(); //Number of indexes.
    if(np!=8)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; block definition needs "
                << 8 << " points, we got: " << np << ".\n";
    else
      {
        if(getNumberOfDefinedFaces()>0)
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; warning redefinition of block: '"
                    << getName() << "'.\n";

	addPoints(point_indexes);
      }
  }

