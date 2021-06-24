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
#include "preprocessor/multi_block_topology/entities/1d/LineBase.h"
#include "preprocessor/multi_block_topology/MultiBlockTopology.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/pos_vec/Pos3dArray.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "preprocessor/Preprocessor.h"
#include "utility/paving/Paver.h"
#include <gmsh.h>

//! @brief Constructor.
XC::PolygonalFace::PolygonalFace(Preprocessor *m)
  : Face(m,0,0) {}

//! @brief Virtual constructor.
XC::SetEstruct *XC::PolygonalFace::getCopy(void) const
  { return new PolygonalFace(*this); }


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
    ref= getPolygon().getRef();
  }

//! @brief Returns a vector in the direction of the local
//! X axis.
Vector3d XC::PolygonalFace::getIVector(void) const
  { return ref.getIVector(); }

//! @brief Returns a vector in the direction of the local
//! Y axis.
Vector3d XC::PolygonalFace::getJVector(void) const
  { return ref.getJVector(); }

//! @brief Creates surface nodes.
void XC::PolygonalFace::create_nodes_from_paving(Paver &paver)
  {
    if(ttzNodes.Null())
      {
	double maxElemSize= -1e9;
	double minElemSize= 1e9;
	
	// Create perimeter nodes.
        create_line_nodes();

	// Prepare perimeters node positions.
	std::deque<Node *> perimeterNodes;
	GeomObj::list_Pos3d contourPositions;

	//// Exterior contour.
	const size_t numSides= getNumberOfEdges();
	for(size_t i= 0;i<numSides; i++)
	  {
	    Side &side= lines[i];
	    Edge *edge= side.getEdge();
	    const size_t nNodesEdge= edge->getNumberOfNodes();
	    const double elemSize= side.getLength()/nNodesEdge;
	    maxElemSize= std::max(elemSize, maxElemSize);
	    minElemSize= std::min(elemSize, minElemSize);
	    for(size_t j= 0;j<nNodesEdge-1;j++)
	      {
		Node *nn= side.getNode(j+1);
	        perimeterNodes.push_back(nn);
  	        contourPositions.push_back(nn->getInitialPosition3d());
	      }
	  }

	//// Holes.
	size_t nHolePositions= 0;
        std::deque<Polygon3d> holePolygons;
	for(std::deque<PolygonalFace *>::iterator i= holes.begin(); i!= holes.end(); i++)
	  {
	    PolygonalFace *hole= *i;
  	    GeomObj::list_Pos3d holePositions;
	    const size_t numSides= hole->getNumberOfEdges();
	    for(size_t j= 0;j<numSides; j++)
	      {
		Side &side= hole->lines[j];
		Edge *edge= side.getEdge();
		const size_t nNodesEdge= edge->getNumberOfNodes();
		const double elemSize= side.getLength()/nNodesEdge;
		maxElemSize= std::max(elemSize, maxElemSize);
		minElemSize= std::min(elemSize, minElemSize);
		for(size_t k= 0;k<nNodesEdge-1;k++)
		  {
		    Node *nn= side.getNode(k+1);
		    perimeterNodes.push_back(nn);
  	            holePositions.push_back(nn->getInitialPosition3d());
		  }
	      }
	    Polygon3d holePlg= Polygon3d(holePositions);
	    nHolePositions+= holePlg.getNumVertices();
	    holePolygons.push_back(holePlg);
	  }
	const double elemSizeRatio= minElemSize/maxElemSize;
	if(elemSizeRatio<0.15)
	  {
	    std::clog << getClassName() << "::" << __FUNCTION__
		      << "; max. element size: " << maxElemSize
	              << " and min. element size: " << minElemSize
	              << " too different; ratio: " << minElemSize/maxElemSize
	              << std::endl;
	  }

	// Call paving routines.
        paver.mesh(Polygon3d(contourPositions), holePolygons);

	// Populate node array.
	const std::vector<Pos3d> &nodePositions= paver.getNodePositions();
	const size_t nNodes= nodePositions.size();
	if(nNodes>0)
	  {
	    ttzNodes= NodePtrArray3d(1,1,nNodes);

	    //// Put perimeter nodes (they exist already)
	    size_t count= 0;
	    const size_t numPerimeterNodes= perimeterNodes.size();
	    for(size_t i= 0;i<numPerimeterNodes;i++, count++)
	      {
		Node *nn= perimeterNodes[i];
		size_t index= i;
		const Pos3d nodePos= nodePositions[index];
		const Pos3d nP= nn->getInitialPosition3d();
		const double d2= dist2(nodePos, nP);
		if(d2>1e-8)
		  {
		    index= paver.getIndexNearestPosition(nP,numPerimeterNodes);
		  }
		ttzNodes(1,1,index+1)= nn;
	      }
	    //// Create new nodes.
	    for(size_t k= count;k<nNodes;k++, count++)
	      {
		const Pos3d nodePos= nodePositions[count];
		create_node(nodePos,1,1,k+1);
	      }
	  }
	else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; paving routine returned: " << nNodes
		    << " nodes. Exiting." << std::endl; 
      }
    else
      if(verbosity>2)
	std::clog << getClassName() << "::" << __FUNCTION__
		  << "; nodes of entity: '" << getName()
		  << "' already exist." << std::endl; 
  }

//! @brief Create nodes from quad tags (i.e. [tagI, tagJ, tagK, tagL].
int XC::PolygonalFace::create_elements_from_quads(const std::deque<std::vector<int> > &quads)
  {
    int retval= 0;
    const Element *seed= getPreprocessor()->getElementHandler().get_seed_element();
    if(seed)
      {
	const size_t numElements= quads.size();
	ttzElements= ElemPtrArray3d(1,1,numElements);
	for(size_t i= 0;i<numElements;i++)
	  {
	    std::vector<int> quad= quads[i];
	    const size_t nNodes= quad.size();
	    if(nNodes>0)
	      {
		ID nTags(nNodes);
		for(size_t j= 0; j<nNodes; j++)
		  { nTags[j]= quad[j]; }
		Element *tmp= seed->getCopy();
		tmp->setIdNodes(nTags);
		ttzElements(1,1,i+1)= tmp;
	      }
	    else
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; empty quad at position: " << i
			<< std::endl;
	  }
	add_elements_to_handler(ttzElements);
	retval= numElements;
      }
    else if(verbosity>0)
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; seed element not set." << std::endl;
    return retval;
  }

//! @brief Creates elements on the nodes created
//! in create_nodes_from_paving.
int XC::PolygonalFace::create_elements_from_paving(const Paver &paver)
  {
    bool retval= 0;
    if(!ttzNodes.empty())
      {
        if(ttzNodes.HasNull())
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; there are null pointers."
                    << " Elements were not created." << std::endl;
        else
          if(ttzElements.Null())
            {
              if(getPreprocessor())
                {
                  if(verbosity>4)
                    std::clog << "Creating elements of entity: '"
			      << getName() << "'...";   
                  const std::deque<std::vector<int> > &paverQuads= paver.getQuads();
		  const size_t numQuads= paverQuads.size();
		  std::deque<std::vector<int> > xcQuads(numQuads);
		  // Get XC tags from paver tags.
		  for(size_t i= 0;i<numQuads;i++)
		    {
		      const std::vector<int> &paverQuad= paverQuads[i];
		      const size_t nNodes= paverQuad.size();
		      std::vector<int> xcQuad(nNodes);
		      if(nNodes>0)
			{
			  for(size_t j= 0; j<nNodes; j++)
			    {
			      const Node *n= ttzNodes(1,1,paverQuad[j]+1);
			      const int nTag= n->getTag();
			      xcQuad[j]= nTag;
			    }
			}
		      xcQuads[i]= xcQuad;
		    }
		  retval= create_elements_from_quads(xcQuads);
                  if(verbosity>4)
                    std::clog << "created." << std::endl;
                }
              else
                std::cerr << getClassName() << "::" << __FUNCTION__
		          << "; preprocessor undefined." << std::endl;
            }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; there is no nodes for the elements." << std::endl;
    retval= ttzElements.NumPtrs();
    if(retval==0 && verbosity>0)
      std::clog << getClassName() << "::" << __FUNCTION__
	        << "; warning 0 elements created for entity: " << getName()
	        << std::endl;
    return retval;
  }

//! @brief Creates mesh using paving algorithm.
void XC::PolygonalFace::gen_mesh_paving(meshing_dir dm)
  {
    Paver paver;
    if(verbosity>3)
      std::clog << "Meshing polygonal surface...("
		<< getName() << ")...";

    create_nodes_from_paving(paver);
    if(ttzElements.Null())
      { create_elements_from_paving(paver); }
    else
      if(verbosity>2)
        std::clog << getClassName() << "::" << __FUNCTION__
	          << "; elements for surface: '" << getName()
		  << "' already exist." << std::endl;      
    if(verbosity>3)
      std::clog << "done." << std::endl;
  }

//! @brief Create Gmsh points from its vertices.
//!
//! @param elemSize: target mesh size (the "characteristic  length")
//! close to the point
void XC::PolygonalFace::create_gmsh_points(const double &elemSize) const
  {
    const std::deque<const Pnt *> vertices= getVertices();
    for(std::deque<const Pnt *>::const_iterator i= vertices.begin();i!=vertices.end();i++)
      {
	const Pnt *pnt= *i;
	if(pnt)
	  {
	    const int gmshTag= pnt->getTag()+1; // Gmsh tags must be strictly positive.
	    const Pos3d pos= pnt->getPos();
            gmsh::model::geo::addPoint(pos.x(), pos.y(), pos.z(), elemSize, gmshTag);
	  }
	else
	  std::cerr << getClassName() << "::" << __FUNCTION__
                    << "; null vertex pointer." << std::endl;
      }
  }

//! @brief Create a Gmsh curve loop from its sides.
void XC::PolygonalFace::create_gmsh_lines(void) const
  {
    const size_t numSides= getNumberOfEdges();
    for(size_t i= 0;i<numSides; i++)
      {
	const Side &side= lines[i];
	const int gmshLineTag= side.getTag()+1; // Gmsh tags must be strictly positive.
	const int p1GmshTag= side.P1()->getTag()+1;
	const int p2GmshTag= side.P2()->getTag()+1;
	gmsh::model::geo::addLine(p1GmshTag, p2GmshTag, gmshLineTag);
      }
    for(hole_const_iterator i= holes.begin(); i!= holes.end(); i++)
      { (*i)->create_gmsh_lines(); }
  }

//! @brief Create the nodes on this surface from the positions
//! computed by Gmsh.
std::map<int, const XC::Node *> XC::PolygonalFace::create_nodes_from_gmsh(void)
  {
    std::map<int, const Node *> mapNodeTags;
    size_t nNodes;
    if(ttzNodes.Null())
      {
	std::map<int, int> mapVertexNodes;
	typedef std::map<int, std::deque< std::pair<int, Pos3d> > > MapEdgeNodes;
	MapEdgeNodes mapEdgeNodes;
	typedef std::deque< std::pair<int, Pos3d> > DqInteriorNodes;
	DqInteriorNodes dqInteriorNodes;
        std::deque<Pnt *> vertices= getVertices();
	// Nodes
	std::vector<std::size_t> nodeTags;
	std::vector<double> nodeCoord;
	std::vector<double> nodeParametricCoord;

	gmsh::model::mesh::getNodes(nodeTags, nodeCoord, nodeParametricCoord);
	nNodes= nodeTags.size();
	for(size_t i= 0;i<nNodes; i++)
	  {
	    const size_t gmshTag= nodeTags[i];
	    const size_t j= i*3;
	    const double x= nodeCoord[j];
	    const double y= nodeCoord[j+1];
	    const double z= nodeCoord[j+2];
	    const Pos3d gmshPos(x,y,z);
	    //Check if it's a vertex node:
  	    Pnt *p= findVertex(gmshPos);
	    if(p)
	      {	mapVertexNodes[p->getTag()]= gmshTag; }
	    std::deque<Side *> posSides= findSides(gmshPos);
	    const size_t nSides= posSides.size();
	    if(nSides>0)
	      {
	        for(std::deque<Side *>::const_iterator i= posSides.begin();i!=posSides.end();i++)
		  {
		    Side *s= (*i);
		    if(s->getEdge()->hasNodes())
		      std::cerr << getClassName() << "::" << __FUNCTION__
				<< "; edge: " << s->getName()
				<< " already meshed. Side node ignored."
				<< std::endl;
		    else
		      {	mapEdgeNodes[s->getTag()].push_back(std::pair(gmshTag, gmshPos)); }
		  }
	      }
	    else // not a side node.
	      if(!p)
	        { dqInteriorNodes.push_back(std::pair(gmshTag, gmshPos)); }
	  } // End Gmsh positions.

	// Create nodes.
        ttzNodes= NodePtrArray3d(1,1,nNodes);
	size_t nCount= 1;
        // Put nodes in the vertices.
        for(std::map<int, int >::const_iterator i= mapVertexNodes.begin();i!=mapVertexNodes.end();i++)
	  {
	    const int xcTag= (*i).first;
	    const int gmshTag= (*i).second;
	    Pnt *p= BuscaPnt(xcTag);
	    p->genMesh();
	    Node *nodePtr= p->getNode();
	    ttzNodes(1,1,nCount++)= nodePtr;
	    mapNodeTags[gmshTag]= nodePtr;
	  }
        const double elemSize= getAvgElemSize();
	// Put nodes in the Edges.
        for(MapEdgeNodes::const_iterator i= mapEdgeNodes.begin();i!=mapEdgeNodes.end();i++)
	  {
	    const int xcTag= (*i).first;
	    std::deque< std::pair<int, Pos3d> > positions= (*i).second;
	    Edge *e= BuscaEdge(xcTag);
	    LineBase *l= dynamic_cast<LineBase *>(e);
	    if(l)
	      {
		const size_t sz= positions.size();
		std::deque<Pos3d> tmp(sz);
		for(size_t i= 0;i<sz;i++)
		  { tmp[i]= positions[i].second; }
		l->create_nodes(tmp);
		for(size_t i= 0;i<sz;i++)
		  {
	            const int gmshTag= positions[i].first;
		    const Pos3d gmshPos= positions[i].second;
		    bool interior= true;
		    const LineBase *cl= l;
		    const Pnt *p1= cl->P1();
		    interior= (interior && (dist2(gmshPos,p1->getPos())>elemSize/1e6));
		    const Pnt *p2= cl->P2();
		    interior= (interior && (dist2(gmshPos,p2->getPos())>elemSize/1e6));
		    if(interior) // is an interior (to the edge) node.
		      {		    
			 Node *nodePtr= l->getNearestNode(gmshPos);
			 const Pos3d nodePos= nodePtr->getInitialPosition3d();
			 //Node *nodePtr= l->getNode(i+1);
			 mapNodeTags[gmshTag]= nodePtr;
			 ttzNodes(1,1,nCount++)= nodePtr;
			 const double d= dist2(tmp[i],nodePos);
			 if(d>1e-6)
			   std::cerr << getClassName() << "::" << __FUNCTION__
				     << "; error node i= " << i
				     << " gmshTag= " << gmshTag
				     << "d= " << d << std::endl;
		      }
		  }		
	      }
	    else
	      std::cerr << getClassName() << "::" << __FUNCTION__
		        << "; line with tag: " << xcTag
		        << " not found." << std::endl;
	  }
	// Put nodes in the interior.
	for(DqInteriorNodes::const_iterator i= dqInteriorNodes.begin();i!= dqInteriorNodes.end();i++)
	  {
	    const std::pair<const int, Pos3d> pair= (*i);
	    const int gmshTag= pair.first;
	    const Pos3d nodePos= pair.second;

	    Node *nodePtr= create_node(nodePos,1,1,nCount++);
	    mapNodeTags[gmshTag]= nodePtr;
	  }
      }
    return mapNodeTags;
  }

//! @brief Create the elements on this surface from the mesh
//! computed by Gmsh.
int XC::PolygonalFace::create_elements_from_gmsh(const std::map<int, const XC::Node *> &nodeMap)
  {
    int retval= 0;
    if(!nodeMap.empty())
      {
	// Elements
	std::vector<int> elementTypes;
	std::vector<std::vector<std::size_t> > elementTags;
	std::vector<std::vector<std::size_t> > elementNodeTags;
	gmsh::model::mesh::getElements(elementTypes, elementTags, elementNodeTags);
	const size_t numTypes= elementTypes.size();
	std::deque<std::vector<int> > quads;
	for(size_t i= 0;i<numTypes;i++)
	  {
	    std::vector<std::size_t> eTags= elementTags[i];
	    const size_t numElemOfType= eTags.size();
	    std::string elementName= "";
	    int dim= 0;
	    int order= 0;
	    int numNodes= 0;
	    std::vector<double> localNodeCoord;
#if GMSH_API_VERSION_MINOR < 5	    
	    gmsh::model::mesh::getElementProperties(elementTypes[i], elementName, dim, order, numNodes, localNodeCoord);
#else
	    int numPrimaryNodes= 0;
	    gmsh::model::mesh::getElementProperties(elementTypes[i], elementName, dim, order, numNodes, localNodeCoord, numPrimaryNodes);
#endif
	    
	    if(numNodes>2) // Quads and triangles.
	      {
		if(verbosity>4)
		  std::clog << "Creating elements of entity: '"
			    << getName() << "'...";   
		for(size_t j= 0;j<numElemOfType;j++)
		  {
		    std::vector<std::size_t> eNodeTags= elementNodeTags[i];
		    size_t k= j*numNodes;
		    std::vector<int> quad(4);
		    for(int l= 0;l<numNodes;l++)
		      {
			const int gmshTag= eNodeTags[k+l];
			const Node *nPtr= nodeMap.at(gmshTag);
			quad[l]= nPtr->getTag();
		      }
		    if(numNodes==3)
		      quad[3]= quad[2];
		    quads.push_back(quad);
		  }
	      }
	  }
	retval= create_elements_from_quads(quads);
	if(verbosity>4)
	  std::clog << "created." << std::endl;
      }
    return retval;
  }

//! @brief Creates mesh using paving algorithm.
void XC::PolygonalFace::gen_mesh_gmsh(meshing_dir dm)
  {
    // Before using any functions in the C++ API, Gmsh must be initialized:
    gmsh::initialize();
    // By default Gmsh will not print out any messages: in order to output
    // messages on the terminal, just set the "General.Terminal" option to 1:
    if(verbosity>1)
      gmsh::option::setNumber("General.Terminal", 1);
    else
      gmsh::option::setNumber("General.Terminal", 0);
    
    // We now add a new model, named "gmsh_<surfaceName>". If gmsh::model::add() is not called, a
    // new default (unnamed) model will be created on the fly, if necessary.
    gmsh::model::add("gmsh_"+getName());

    // Element size
    const double lc= getAvgElemSize();

    // Create gmsh points.
    //// Contour points.
    create_gmsh_points(lc);
    //// Hole points.
    for(std::deque<PolygonalFace *>::iterator i= holes.begin(); i!= holes.end(); i++)
      { (*i)->create_gmsh_points(lc); }

    // Create gmsh lines.
    create_gmsh_lines();
    
    // Create gmsh surface.
    const int pl= create_gmsh_surface();
    
    gmsh::model::geo::synchronize();

    // To generate quadrangles instead of triangles, we can simply add
    gmsh::model::mesh::setRecombine(2, pl);

    // We can then generate a 2D mesh...
    gmsh::model::mesh::generate(2);
    if(verbosity>1)
      gmsh::fltk::run(); // Display mesh for debugging.

    // Extract mesh data.
    const std::map<int, const Node *> mapNodes= create_nodes_from_gmsh();
    const int numElements= create_elements_from_gmsh(mapNodes);
    
    // This should be called when you are done using the Gmsh C++ API:
    gmsh::finalize();
  }

//! @brief Triggers mesh creation.
void XC::PolygonalFace::genMesh(meshing_dir dm, bool paving)
  {
    if(paving)
      gen_mesh_paving(dm);
    else
      gen_mesh_gmsh(dm);
  }
