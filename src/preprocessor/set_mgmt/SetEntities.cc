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
//SetEntities.cc

#include "SetEntities.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/multi_block_topology/MultiBlockTopology.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/multi_block_topology/entities/1d/Edge.h"
#include "preprocessor/multi_block_topology/entities/2d/Face.h"
#include "preprocessor/multi_block_topology/entities/2d/PolygonalFace.h"
#include "preprocessor/multi_block_topology/entities/3d/Body.h"
#include "preprocessor/multi_block_topology/entities/3d/UniformGrid.h"
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "preprocessor/multi_block_topology/trf/TrfGeom.h"
#include "utility/matrix/ID.h"

#include "utility/geom/pos_vec/SlidingVectorsSystem3d.h"
#include "utility/geom/d1/Segment3d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/geom/d3/HalfSpace3d.h"

#include <gmsh.h>

//! @brief Constructor.
XC::SetEntities::SetEntities(Preprocessor *md)
  : PreprocessorContainer(md), MovableObject(0), points(this),
    lines(this), surfaces(this), bodies(this), uniform_grids(this),
    useGmsh(false) {}

//! @brief Copy constructor.
XC::SetEntities::SetEntities(const SetEntities &other)
  : PreprocessorContainer(other), MovableObject(other), useGmsh(other.useGmsh)
  {
    copy_lists(other);
  }

//! @brief Assignment operator.
XC::SetEntities &XC::SetEntities::operator=(const SetEntities &other)
  {
    PreprocessorContainer::operator=(other);
    MovableObject::operator=(other);
    copy_lists(other);
    useGmsh= other.useGmsh;
    return *this;
  }

//! @brief Copy the object lists of the set s.
//!
//! Append to this object the lists of objects from the argument.
void XC::SetEntities::copy_lists(const SetEntities &other)
  {
    points= other.points;
    points.set_owner(this);
    lines= other.lines;
    lines.set_owner(this);
    surfaces= other.surfaces;
    surfaces.set_owner(this);
    bodies= other.bodies;
    bodies.set_owner(this);
    uniform_grids= other.uniform_grids;
    uniform_grids.set_owner(this);
  }

//! @brief Extend this set with the points of the argument.
void XC::SetEntities::extend(const lst_ptr_points &pts)
  { points.extend(pts);  }
  
//! @brief Extend this set with the lines of the argument.
void XC::SetEntities::extend(const lst_line_pointers &lns)
  { lines.extend(lns); }
  
//! @brief Extend this set with the surfaces of the argument.
void XC::SetEntities::extend(const lst_surface_ptrs &sfs)
  { surfaces.extend(sfs);  }

//! @brief Extend this set with the bodies of the argument.
void XC::SetEntities::extend(const lst_body_pointers &bds)
  { bodies.extend(bds); }
  
//! @brief Extend this set with the uniform grids of the argument.
void XC::SetEntities::extend(const lst_ptr_uniform_grids &ugrids)
  { uniform_grids.extend(ugrids); }

//! @brief Extend this set with the objects of the set
//! being passed as parameter.
void XC::SetEntities::extend_lists(const SetEntities &other)
  {
    points.extend(other.points);
    lines.extend(other.lines);
    surfaces.extend(other.surfaces);
    bodies.extend(other.bodies);
    uniform_grids.extend(other.uniform_grids);
  }

//! @brief Removes from this set the objects that belongs
//! also to the argument.
void XC::SetEntities::substract_lists(const SetEntities &other)
  {
    points-= other.points;
    lines-= other.lines;
    surfaces-= other.surfaces;
    bodies-= other.bodies;
    uniform_grids-= other.uniform_grids;
  }

//! @brief Removes from this set the objects that belongs
//! also to the argument.
void XC::SetEntities::intersect_lists(const SetEntities &other)
  {
    points-= other.points;
    lines-= other.lines;
    surfaces-= other.surfaces;
    bodies-= other.bodies;
    uniform_grids-= other.uniform_grids;
  }

//! @brief Addition assignment operator.
//!
//! Extend the lists with the objects from the argument.
XC::SetEntities &XC::SetEntities::operator+=(const SetEntities &other)
  {
    extend_lists(other); //Extend lists.
    return *this;
  }

//! @brief -= operator.
XC::SetEntities &XC::SetEntities::operator-=(const SetEntities &other)
  {
    substract_lists(other);
    return *this;
  }

//! @brief *= operator (intersection).
XC::SetEntities &XC::SetEntities::operator*=(const SetEntities &other)
  {
    intersect_lists(other);
    return *this;
  }

//! @brief Addition (union) operator.
//!
//! Extend the lists of the first set with the objects from the argument.
XC::SetEntities XC::SetEntities::operator+(const SetEntities &b) const
  {
    SetEntities result(*this);
    result+= b;
    return result;
  }

//! @brief Difference operator.
//!
//! Removes the objects that belong also to the argument.
XC::SetEntities XC::SetEntities::operator-(const SetEntities &b) const
  {
    SetEntities result(*this);
    result-= b;
    return result;
  }

//! @brief Intersection operator.
//!
//! Return the set intersection.
XC::SetEntities XC::SetEntities::operator*(const SetEntities &b) const
  {
    SetEntities result(*this);
    result*= b;
    return result;
  }

//! @brief Clears the set.
void XC::SetEntities::clear(void)
  {
    points.clear();
    lines.clear();
    surfaces.clear();
    bodies.clear();
    uniform_grids.clear();
  }

//! @brief Return true if the set is empty.
bool XC::SetEntities::empty(void) const
  {
    return ( points.empty() && lines.empty() && surfaces.empty() && bodies.empty() && uniform_grids.empty() );
  }
//!  @brief Clears all the objects of the set.
void XC::SetEntities::clearAll(void)
  {
    points.clearAll();
    lines.clearAll();
    surfaces.clearAll();
    bodies.clearAll();
    uniform_grids.clearAll();
  }

//! @brief Appends to the set being passed as parameter
//! the elements that intervene on the definition
//! of those entities that are already in the set.
void XC::SetEntities::fillDownwards(void)
  {
    // Bodies
//     for(lst_bodies::iterator i=bodies.begin();i!=bodies.end();i++)
//       {
//         lst_surfaces ss= (*i)->getSurfaces();
//         surfaces.insert_unique(surfaces.end(),ss.begin(),ss.end());
//       }
    
    // Surface holes.
    for(sup_iterator i=surfaces.begin();i!=surfaces.end();i++)
      {
	std::deque<PolygonalFace *> holes= (*i)->getHoles();
	surfaces.insert_unique(surfaces.end(), holes.begin(), holes.end());
      }
    
    // Surfaces
    for(sup_iterator i=surfaces.begin();i!=surfaces.end();i++)
      {
        // append surface lines.
        lst_line_pointers ll((*i)->getEdges());
        lines.insert_unique(lines.end(),ll.begin(),ll.end());
      }
    // Lines
    for(lin_iterator i=lines.begin();i!=lines.end();i++)
      {
        // append line points.
        const size_t nv= (*i)->getNumberOfVertices();
        for( size_t j=1;j<=nv;j++)
          points.push_back(const_cast<Pnt *>((*i)->getVertex(j)));
      }
  }

//! @brief Appends to the set being passed as parameter
//! the elements that intervene on the definition
//! of those entities that are already in the set.
void XC::SetEntities::fillDownwards(SetMeshComp &mc)
  {
    fillDownwards(); // append entities.
    // Bodies
//     for(lst_bodies::iterator i=bodies.begin();i!=bodies.end();i++)
//       {
//         lst_surfaces ss= (*i)->getSurfaces();
//         surfaces.insert_unique(surfaces.end(),ss.begin(),ss.end());
//       }
    // Surfaces
    for(sup_iterator i=surfaces.begin();i!=surfaces.end();i++)
      {
        // append elements.
        ElemPtrArray3d &ttz_elements= (*i)->getTtzElements();
        const size_t numberOfLayers= ttz_elements.getNumberOfLayers();
        const size_t numberOfRows= ttz_elements.getNumberOfRows();
        const size_t numberOfColumns= ttz_elements.getNumberOfColumns();
        for(size_t i=1;i<=numberOfLayers;i++)
          for(size_t j=1;j<=numberOfRows;j++)
            for(size_t k=1;k<=numberOfColumns;k++)
              mc.addElement(ttz_elements(i,j,k));

      }
    // Lines
    for(lin_iterator i=lines.begin();i!=lines.end();i++)
      {
        // append elements.
        ElemPtrArray3d &ttz_elements= (*i)->getTtzElements();
        const size_t numberOfLayers= ttz_elements.getNumberOfLayers();
        const size_t numberOfRows= ttz_elements.getNumberOfRows();
        const size_t numberOfColumns= ttz_elements.getNumberOfColumns();
        for(size_t i=1;i<=numberOfLayers;i++)
          for(size_t j=1;j<=numberOfRows;j++)
            for(size_t k=1;k<=numberOfColumns;k++)
              mc.addElement(ttz_elements(i,j,k));
      }
    // Points
    for(pnt_iterator i= points.begin();i!=points.end();i++)
      {
        // append nodes.
	Pnt &p= *(*i);
	if(p.hasNode())
	  {
            Node *n= (*i)->getNode();
	    if(n)
	      mc.addNode(n);
	  }
      }
  }

//! @brief  Appends to this set the objects that make reference
//! to one or more of the objects that already make part of the set.
void XC::SetEntities::fillUpwards(const SetMeshComp &mc)
  {
    for(pnt_iterator i=points.begin();i!=points.end();i++)
      {
        std::set<const Edge *> ll= getConnectedLines(**i);
        for(std::set<const Edge *>::const_iterator j= ll.begin();j!=ll.end();j++)
          lines.push_back(const_cast<Edge *>(*j));
      }
    for(lin_iterator i=lines.begin();i!=lines.end();i++)
      {
        lst_surface_ptrs ss(getConnectedSurfaces(**i));
        surfaces.insert_unique(surfaces.end(),ss.begin(),ss.end());
      }
    for(lst_surface_ptrs::const_iterator i= surfaces.begin();i!=surfaces.end();i++)
      {
        const std::set<const Body *> &cb= (*i)->getConnectedBodies();
        for(std::set<const Body *>::const_iterator j= cb.begin();j!=cb.end();j++)
          bodies.push_back(const_cast<Body *>(*j));
      }
  }

//! @brief Moves the objects of the set.
void XC::SetEntities::move(const Vector3d &desplaz)
  {
    for(lst_ptr_points::iterator i= points.begin();i!=points.end();i++)
      (*i)->Move(desplaz);
  }

//! @brief Applies the transformation to the elements of the set.
void XC::SetEntities::Transform(const TrfGeom &trf)
  {
    for(lst_ptr_points::iterator i= points.begin();i!=points.end();i++)
      (*i)->Transform(trf);
  }

//! @brief Creates a copy of the entities of the set and put them in another
//! set with the name is being passed as parameter. The coordinates of the new
//! points will be obtained by adding to the original coordinates those
//! of the vector being passed as parameter.
XC::SetEntities XC::SetEntities::create_copy(const std::string &name,const Vector3d &v= Vector3d())
  {
    Preprocessor *preprocessor= getPreprocessor();
    SetEntities retval(preprocessor);
    if(!preprocessor)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; preprocessor not assigned." << std::endl;
    else
      {
	//Copying points.
	std::map<std::string,std::string> new_points_names;
	for(lst_ptr_points::iterator i= points.begin();i!=points.end();i++)
	  {
	    const std::string oldName= (*i)->getName();
	    const std::string new_name= name+oldName;
	    Pnt *new_point= preprocessor->getMultiBlockTopology().getPoints().Copy(*i,v);
	    new_point->BorraPtrNodElem();
	    retval.points.push_back(new_point);
	    new_points_names[oldName]= new_name;
	  }
	//Copy lines.
	std::map<std::string,std::string> new_lines_names;
	for(lst_line_pointers::iterator i= lines.begin();i!=lines.end();i++)
	  {
	    const std::string oldName= (*i)->getName();
	    const std::string new_name= name+oldName;
	    Edge *new_edge= preprocessor->getMultiBlockTopology().getLines().createCopy(*i);
	    new_edge->BorraPtrNodElem();
	    retval.lines.push_back(new_edge);
	    new_lines_names[oldName]= new_name;
	    const size_t nv= new_edge->getNumberOfVertices();
	    for(size_t i= 0;i<nv;i++)
	      {
		const Pnt *vertice_viejo= new_edge->getVertex(i);
		const std::string oldName= vertice_viejo->getName();
		const std::string new_name= new_lines_names[oldName];
		Pnt *new_point= retval.points.searchName(new_name);
		new_edge->SetVertice(i,new_point);
	      }
	  }
      }
    return retval;
  }

//! @brief Conciliate number of divisions of the lines shared by
//! faces.
void XC::SetEntities::conciliaNDivs(void)
  {
    fillDownwards();
    conciliate_divisions(surfaces, lines);
  }

//! @brief Set the value of the useGmsh member.
void XC::SetEntities::setUseGmsh(const bool &b)
  { useGmsh= b; }

//! @brief Get the value of the useGmsh member.
bool XC::SetEntities::getUseGmsh(void) const
  { return useGmsh; }

//! @brief Create Gmsh points from the points of this object.
void XC::SetEntities::create_gmsh_points(void) const
  {
    for(lst_ptr_points::const_iterator i= points.begin();i!=points.end();i++)
      {
	const Pnt *pnt= *i;
	const double elemSize= pnt->getAverageElementSize();
	const int gmshTag= pnt->getTag()+1; // Gmsh tags must be strictly positive.
	const Pos3d pos= pnt->getPos();
	gmsh::model::geo::addPoint(pos.x(), pos.y(), pos.z(), elemSize, gmshTag);
      }
  }

//! @brief Create nodes and, where appropriate, elements on set points.
void XC::SetEntities::point_meshing(meshing_dir dm)
  {
    if(verbosity>2)
      std::clog << "Meshing points...";
    for(lst_ptr_points::iterator i= points.begin();i!=points.end();i++)
      (*i)->genMesh(dm);
    if(verbosity>2)
      std::clog << "done." << std::endl;
  }

//! @brief Create a gmsh lines from the edges.
void XC::SetEntities::create_gmsh_lines(void) const
  {
    for(lst_line_pointers::const_iterator i= lines.begin();i!=lines.end();i++)
      {
	const Edge *edge= *i;
	edge->create_gmsh_line();
      }
  }

//! @brief Create nodes and, where appropriate, elements on set lines.
void XC::SetEntities::line_meshing(meshing_dir dm)
  {
    if(verbosity>2)
      std::clog << "Meshing lines...";
    for(lst_line_pointers::iterator i= lines.begin();i!=lines.end();i++)
      (*i)->genMesh(dm);
    if(verbosity>2)
      std::clog << "done." << std::endl;
  }

//! @brief Create a gmsh lines from the edges.
void XC::SetEntities::create_gmsh_loops(void) const
  {
    for(lst_surface_ptrs::const_iterator i= surfaces.begin();i!=surfaces.end();i++)
      {
	const Face *face= *i;
	face->create_gmsh_loops();
      }
  }

//! @brief Create a gmsh lines from the edges.
std::vector<int> XC::SetEntities::create_gmsh_surfaces(void) const
  {
    const size_t numSurfaces= surfaces.size();
    std::vector<int> retval(numSurfaces);
    size_t count= 0;
    for(lst_surface_ptrs::const_iterator i= surfaces.begin();i!=surfaces.end();i++, count++)
      {
	const Face *face= *i;
        retval[count]= face->create_gmsh_surface();
      }
    return retval;
  }

//! @brief Create nodes and, where appropriate, elements on surfaces.
void XC::SetEntities::surface_meshing(meshing_dir dm)
  {
    if(verbosity>2)
      std::clog << "Meshing surfaces...";
    for(lst_surface_ptrs::iterator i= surfaces.begin();i!=surfaces.end();i++)
      (*i)->genMesh(dm);
    if(verbosity>2)
      std::clog << "done." << std::endl;
  }

//! @brief Create nodes and, where appropriate, elements on set bodies.
void XC::SetEntities::body_meshing(meshing_dir dm)
  {
    if(verbosity>2)
      std::clog << "Meshing bodies...";
    for(lst_body_pointers::iterator i= bodies.begin();i!=bodies.end();i++)
      (*i)->genMesh(dm);
    if(verbosity>2)
      std::clog << "done." << std::endl;
  }

//! @brief Creates nodes and, eventually, elements on the points of the set.
void XC::SetEntities::uniform_grid_meshing(meshing_dir dm)
  {
    if(verbosity>2)
      std::clog << "Meshing uniform grids...";
    for(lst_ptr_uniform_grids::iterator i= uniform_grids.begin();i!=uniform_grids.end();i++)
      (*i)->genMesh(dm);
    if(verbosity>2)
      std::clog << "done." << std::endl;
  }

//! @brief Create the nodes from the positions computed by Gmsh.
std::map<int, const XC::Node *> XC::SetEntities::create_nodes_from_gmsh(void)
  {
    std::map<int, const Node *> mapNodeTags;
    std::vector<std::size_t> nodeTags;
    std::vector<double> nodeCoord;
    std::vector<double> nodeParametricCoord;
    // Nodes on points.
    for(lst_ptr_points::iterator i= points.begin();i!=points.end();i++)
      {
	Pnt *pnt= *i;
	const int gmshTag= pnt->getTag()+1; // Gmsh tag for the point.
	//nodeTags.clear(); nodeCoord.clear(); nodeParametricCoord.clear();
	gmsh::model::mesh::getNodes(nodeTags, nodeCoord, nodeParametricCoord, 0, gmshTag);
        const size_t gmshNodeTag= nodeTags[0];
	const double x= nodeCoord[0];
	const double y= nodeCoord[1];
	const double z= nodeCoord[2];
	const Pos3d gmshPos(x,y,z);
	if(gmshPos.dist2(pnt->getPos())>1e-6)
	  std::cerr << getClassName() << "::" << __FUNCTION__
                    << "; error reading node for point: " << gmshTag-1
		    << std::endl;
	pnt->genMesh(); // Create the node.
	mapNodeTags[gmshNodeTag]= pnt->getNode();
      }
    // Nodes on edges.
    for(lst_line_pointers::iterator i= lines.begin();i!=lines.end();i++)
      {
	Edge *edge= *i;
	const int gmshTag= edge->getTag()+1; // Gmsh tag for the line.
	//nodeTags.clear(); nodeCoord.clear(); nodeParametricCoord.clear();
	gmsh::model::mesh::getNodes(nodeTags, nodeCoord, nodeParametricCoord, 1, gmshTag);
	const size_t nNodes= nodeTags.size();
	std::vector<Pos3d> positions(nNodes);
	for(size_t i= 0;i<nNodes; i++)
	  {
	    const size_t j= i*3;
	    const double x= nodeCoord[j];
	    const double y= nodeCoord[j+1];
	    const double z= nodeCoord[j+2];
	    positions[i]= Pos3d(x,y,z);
	    //const double u= nodeParametricCoord[i];
	  }
	std::vector<Node *> nodePtrs= edge->create_interior_nodes(positions); // create the edge nodes.
	for(size_t i= 0;i<nNodes; i++) // populate node map.
	  {
            const size_t gmshNodeTag= nodeTags[i];
	    mapNodeTags[gmshNodeTag]= nodePtrs[i];
	  }
      }
    // Nodes on surfaces.
    for(lst_surface_ptrs::iterator i= surfaces.begin();i!=surfaces.end();i++)
      {
	Face *face= *i;
	if(!face->isHole())
	  {
	    const int gmshTag= face->getTag()+1; // Gmsh tag for the face.
	    gmsh::model::mesh::getNodes(nodeTags, nodeCoord, nodeParametricCoord, 2, gmshTag);
	    const size_t nNodes= nodeTags.size();
	    std::vector<Pos3d> positions(nNodes);
	    for(size_t i= 0;i<nNodes; i++)
	      {
		const size_t j= i*3;
		const double x= nodeCoord[j];
		const double y= nodeCoord[j+1];
		const double z= nodeCoord[j+2];
		positions[i]= Pos3d(x,y,z);
		//const size_t k= i*2;
		//const double u= nodeParametricCoord[k];
		//const double v= nodeParametricCoord[k+1];
	      }
	    std::vector<Node *> nodePtrs= face->create_interior_nodes(positions);
	    for(size_t i= 0;i<nNodes; i++) // populate node map.
	      {
		const size_t gmshNodeTag= nodeTags[i];
		mapNodeTags[gmshNodeTag]= nodePtrs[i];
	      }
	  }
      }
    return mapNodeTags;
  }

//! @brief Create the elements from the mesh computed by Gmsh.
int XC::SetEntities::create_elements_from_gmsh(const std::map<int, const XC::Node *> &nodeMap)
  {
    int retval= 0;
    if(!nodeMap.empty())
      {
	std::vector<int> elementTypes;
	std::vector<std::vector<std::size_t> > elementTags;
	std::vector<std::vector<std::size_t> > elementNodeTags;
	// Elements on surfaces.
	for(lst_surface_ptrs::iterator i= surfaces.begin();i!=surfaces.end();i++)
	  {
	    Face *face= *i;
	    if(!face->isHole())
	      {
	        const int gmshTag= face->getTag()+1; // Gmsh tag for the face.
		gmsh::model::mesh::getElements(elementTypes, elementTags, elementNodeTags, 2, gmshTag);
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
				    << face->getName() << "'...";   
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
		retval+= face->create_elements_from_quads(quads);
	      }
	  }
	if(verbosity>4)
	  std::clog << "created." << std::endl;
      }
    return retval;
  }

//! @brief Asks Gmsh to create the mesh.
//!
//! @param modelName: string identifier for the gmsh model.
void XC::SetEntities::gen_mesh_gmsh(const std::string &modelName)
  {
    // Before using any functions in the C++ API, Gmsh must be initialized:
    gmsh::initialize();
    // By default Gmsh will not print out any messages: in order to output
    // messages on the terminal, just set the "General.Terminal" option to 1:
    if(verbosity>1)
      gmsh::option::setNumber("General.Terminal", 1);
    else
      gmsh::option::setNumber("General.Terminal", 0);
    
    // We now add a new model, named "gmsh_<surfaceName>". If
    // gmsh::model::add() is not called, a new default (unnamed) model
    // will be created on the fly, if necessary.
    gmsh::model::add(modelName);

    // Make the lines and points part of the set.
    fillDownwards();
    // Create gmsh points.
    create_gmsh_points();
    // Create gmsh lines.
    create_gmsh_lines();
  
    // Create gmsh loops.
    const std::vector<int> surfaceTags= create_gmsh_surfaces();
    
    gmsh::model::geo::synchronize();

    // To generate quadrangles instead of triangles, we can simply add
    for(std::vector<int>::const_iterator i= surfaceTags.begin(); i!= surfaceTags.end(); i++)
      {
    	const int sTag= *i;
    	if(sTag>0) // surface is not a hole.
          gmsh::model::mesh::setRecombine(2, *i);
      }

    // Transfinite automatic.
    gmsh::option::setNumber("Mesh.MeshSizeMin", getMinElementSize()) ;
    gmsh::option::setNumber("Mesh.MeshSizeMax", getMaxElementSize()); 
    gmsh::model::mesh::setTransfiniteAutomatic();
    
    // We can then generate a 2D mesh...
    gmsh::model::mesh::generate(2);
    //gmsh::model::mesh::refine();

    if(verbosity>1)
      gmsh::fltk::run(); // Display mesh for debugging.

    // Extract mesh data.
    const std::map<int, const Node *> mapNodes= create_nodes_from_gmsh();
    const int numElements= create_elements_from_gmsh(mapNodes);

    // This should be called when you are done using the Gmsh C++ API:
    gmsh::finalize();
  }

//!  @brief Triggers mesh generation from set components.
//!
//! @param dm: Meshing direction.
void XC::SetEntities::genMesh(const std::string &setName, meshing_dir dm)
  {
    Preprocessor *prep= getPreprocessor();
    assert(prep);
    prep->get_sets().abre_set(setName); //To let nodes and elements enter this set.

    if(verbosity>1)
      std::clog << "Meshing set: " << setName << " ...";

    if(useGmsh)
      gen_mesh_gmsh("gmsh_"+setName);
    else
      {
	body_meshing(dm);
	surface_meshing(dm);
	line_meshing(dm);
	point_meshing(dm);
	uniform_grid_meshing(dm);
      }
    prep->get_sets().cierra_set(setName); //Cerramos.

    if(verbosity>1)
      std::clog << "done." << std::endl;
  }

//! @brief Returns true if the point belongs to the set.
bool XC::SetEntities::In(const Pnt *p) const
  { return points.in(p); }

//! @brief Return a new set that contains the points that lie insiof the
//! geometric object.
//!
//! @param geomObj: geometric object that must contain the nodes.
//! @param tol: tolerance for "In" function.
XC::SetEntities XC::SetEntities::pickPointsInside(const GeomObj3d &geomObj, const double &tol) const
  {
    Preprocessor *prep= const_cast<Preprocessor *>(getPreprocessor());
    SetEntities retval(prep);
    retval.points= points.pickEntitiesInside(geomObj,tol);
    return retval;
  }


//! @brief Return the point closest to the position argument.
XC::Pnt *XC::SetEntities::getNearestPoint(const Pos3d &p)
  { return points.getNearest(p); }

//! @brief Return the point closest to the position argument.
const XC::Pnt *XC::SetEntities::getNearestPoint(const Pos3d &p) const
  { return points.getNearest(p); }


//! @brief Returns true if the edge belongs to the set.
bool XC::SetEntities::In(const Edge *e) const
  { return lines.in(e); }

//! @brief Return the intersections between the lines of the model.
XC::edge_intersection_pairs XC::SetEntities::getLineIntersections(const double &tol) const
  {
    edge_intersection_pairs retval;
    std::deque<Segment3d> segments;
    for(lst_line_pointers::const_iterator i= lines.begin();i!=lines.end();i++)
      {
	const Edge &line= **i;
	const std::deque<Segment3d> edgeSegments= line.getSegments();
	segments.insert(segments.end(), edgeSegments.begin(),edgeSegments.end());
      }
    int_pair_deque intersections= getIntersections(segments);
    for(int_pair_deque::const_iterator i= intersections.begin();i!=intersections.end();i++)
      {
	int_pair intPair= *i;
	const int first= intPair.first;
	const int second= intPair.second;
	Segment3d s1= segments[first];
	Segment3d s2= segments[second];
	Edge *a= dynamic_cast<Edge *>(s1.Owner());
	Edge *b= dynamic_cast<Edge *>(s2.Owner());
	GeomObj3d::list_Pos3d points= s1.getIntersection(s2);
        if(!points.empty()) // Intersection exists.
	  {
            const Pos3d &p= *points.begin();
 	    retval.push_back(EdgeIntersectionRef(a,b,p));
	  }
	else
	  {
	    // Test if the problem is that one segment
	    // almost touch the other:
	    const double tol2= tol*tol;
	    const Pos3d s1p1= s1.getFromPoint();
	    const double d1= s2.dist2(s1p1);
	    if(d1<tol2)
	      { retval.push_back(EdgeIntersectionRef(a,b,s1p1)); }
	    else
	      {
	        const Pos3d s1p2= s1.getToPoint();
	        const double d2= s2.dist2(s1p2);
	        if(d2<tol2)
		  { retval.push_back(EdgeIntersectionRef(a,b,s1p2)); }
		else
		  {
	            const Pos3d s2p1= s2.getFromPoint();
	            const double d3= s1.dist2(s2p1);
		    if(d3<tol2)
		      { retval.push_back(EdgeIntersectionRef(a,b,s2p1)); }
		    else
		      {
	                const Pos3d s2p2= s2.getToPoint();
	                const double d4= s1.dist2(s2p2);
		        if(d4<tol2)
		          { retval.push_back(EdgeIntersectionRef(a,b,s2p2)); }
			else
			  std::cerr << getClassName() << "::" << __FUNCTION__
	                            << "; s1= " << s1 << " s2= " << s2
	                            << " intersection not found."
			            << " d1= " << d1 << " d2= " << d2
			            << " d3= " << d3 << " d4= " << d4
				    << std::endl;
		      }
		  }
	      }
	  }
      }
    return retval;
  }

//! @brief Split the lines of the set at its intersection points.
void XC::SetEntities::splitLinesAtIntersections(const double &tol)
  {
    std::map<Edge *, std::list<Edge *> > aliases;
    edge_intersection_pairs intersections= getLineIntersections(tol);
    for(edge_intersection_pairs::iterator i= intersections.begin();
	i!=intersections.end();i++)
      {
	EdgeIntersectionRef ir= *i;
	const Pos3d p= ir.intersectionPos;
	if(aliases.find(ir.first)==aliases.end()) // no aliases
	  {
	    Edge *newLine= ir.first->splitAtPos3d(p);
	    if(newLine)
	      aliases[ir.first].push_back(newLine);
	  }
        else // already splitted: aliases exist.
	  {
	    Edge *nearestAlias= ir.first;
	    double dist2= nearestAlias->getSquaredDistanceTo(p);
	    std::list<Edge *> edgeAliases= aliases[ir.first];
	    for(std::list<Edge *>::iterator i= edgeAliases.begin(); i!= edgeAliases.end();i++)
	      {
		const double d2= (*i)->getSquaredDistanceTo(p);
		if(d2<dist2)
		  {
		    nearestAlias= (*i);
		    dist2= d2;
		  }
	      }
	    Edge *newLine= nearestAlias->splitAtPos3d(p);
	    if(newLine)
	      aliases[ir.first].push_back(newLine);
	  }
	if(aliases.find(ir.second)==aliases.end()) // no aliases
	  {
  	    Edge *newLine= ir.second->splitAtPos3d(p);
	    if(newLine)
	      aliases[ir.second].push_back(newLine);
	  }
	else
	  {
	    Edge *nearestAlias= ir.second;
	    double dist2= nearestAlias->getSquaredDistanceTo(p);
	    std::list<Edge *> edgeAliases= aliases[ir.second];
	    for(std::list<Edge *>::iterator i= edgeAliases.begin(); i!= edgeAliases.end();i++)
	      {
		const double d2= (*i)->getSquaredDistanceTo(p);
		if(d2<dist2)
		  {
		    nearestAlias= (*i);
		    dist2= d2;
		  }
	      }
	    Edge *newLine= nearestAlias->splitAtPos3d(p);
	    if(newLine)
	      aliases[ir.second].push_back(newLine);
	  }
      }
    // And for the remaining lines we use a "brute-force"
    // algorithm. Some form of neares neighbor algorithm
    // will be very useful here. The double for loop is
    // expensive.
    for(pnt_iterator i=points.begin();i!=points.end();i++)
      {
	Pnt *pnt= *i;
	const Pos3d pos= pnt->getPos();
        std::set<const Edge *> conn= getConnectedLines(*pnt);
        const double tol2= tol*tol;
        for(lin_iterator j= lines.begin();j!=lines.end();j++)
	  {
	    Edge *l= *j;
	    if(conn.find(l)==conn.end()) // not already connected
	      {
	        const double dist2= (*j)->getSquaredDistanceTo(pos);
		if(dist2<tol2)
		  { l->splitAtPoint(pnt); }
	      }
	  }
      }
  }

//! @brief Return a new set that contains the lines that lie inside
//! of the geometric object.
//!
//! @param geomObj: geometric object that must contain the nodes.
//! @param tol: tolerance for "In" function.
XC::SetEntities XC::SetEntities::pickLinesInside(const GeomObj3d &geomObj, const double &tol) const
  {
    Preprocessor *prep= const_cast<Preprocessor *>(getPreprocessor());
    SetEntities retval(prep);
    retval.lines= lines.pickEntitiesInside(geomObj,tol);
    return retval;
  }

//! @brief Returns true if the surface belongs to the set.
bool XC::SetEntities::In(const Face *f) const
  { return surfaces.in(f); }

//! @brief Return a new set that contains the surfaces that lie insiof the
//! geometric object.
//!
//! @param geomObj: geometric object that must contain the nodes.
//! @param tol: tolerance for "In" function.
XC::SetEntities XC::SetEntities::pickSurfacesInside(const GeomObj3d &geomObj, const double &tol) const
  {
    Preprocessor *prep= const_cast<Preprocessor *>(getPreprocessor());
    SetEntities retval(prep);
    retval.surfaces= surfaces.pickEntitiesInside(geomObj,tol);
    return retval;
  }

//! @brief Returns true if the body belongs to the set.
bool XC::SetEntities::In(const Body *b) const
  { return bodies.in(b); }

//! @brief Return a new set that contains the bodies that lie insiof the
//! geometric object.
//!
//! @param geomObj: geometric object that must contain the nodes.
//! @param tol: tolerance for "In" function.
XC::SetEntities XC::SetEntities::pickBodiesInside(const GeomObj3d &geomObj, const double &tol) const
  {
    Preprocessor *prep= const_cast<Preprocessor *>(getPreprocessor());
    SetEntities retval(prep);
    retval.bodies= bodies.pickEntitiesInside(geomObj,tol);
    return retval;
  }

//! @brief Returns true if the «uniform grid» belongs to the set.
bool XC::SetEntities::In(const UniformGrid *ug) const
  { return uniform_grids.in(ug); }

//! @brief Return the entities current position boundary.
BND3d XC::SetEntities::Bnd(void) const
  {
    BND3d retval= points.Bnd();
    retval+= lines.Bnd();
    retval+= surfaces.Bnd();
    retval+= bodies.Bnd();
    return retval;
  }

//! @brief Return the average size of the geometric entities.
double XC::SetEntities::getAverageSize(void) const
  {
    double retval= 0.0;
    const size_t numEdges= lines.size();
    if(numEdges>0)
      {
	for(lst_line_pointers::const_iterator i= lines.begin();i!=lines.end();i++)
	  {
	    const Edge &line= **i;
	    retval+= line.getLength();
	  }
      }
    const size_t numFaces= surfaces.size();
    if(numFaces>0)
      {
	for(lst_surface_ptrs::const_iterator i= surfaces.begin();i!=surfaces.end();i++)
	  {
	    const double area= (*i)->getArea();
            retval+= sqrt(area);
	  }
      }
    const size_t numBodies= bodies.size();
    if(numBodies>0)
      {
	for(lst_body_pointers::const_iterator i= bodies.begin();i!=bodies.end();i++)
	  {
	    const double volume= (*i)->getVolume();
            retval+= pow(volume,1.0/3.0);
	  }
      }
    retval/=(numEdges+numFaces+numBodies);
    return retval;
  }

//! @brief Return the maximum element size according to the
//! number of divisions of the lines.
double XC::SetEntities::getMaxElementSize(void) const
  {
    double retval= 0.0;
    if(lines.empty())
      std::clog << getClassName() << "::" << __FUNCTION__
	        << "; WARNING, set has no lines." << std::endl;
    else
	for(lst_line_pointers::const_iterator i= lines.begin();i!=lines.end();i++)
	  {
	    const Edge &line= **i;
	    retval= std::max(retval, line.getElemSize());
	  }
    return retval;
  }

//! @brief Return the minimum element size according to the
//! number of divisions of the lines.
double XC::SetEntities::getMinElementSize(void) const
  {
    double retval= 6.023e23;
    if(lines.empty())
      std::clog << getClassName() << "::" << __FUNCTION__
	        << "; WARNING, set has no lines." << std::endl;
    else
      for(lst_line_pointers::const_iterator i= lines.begin();i!=lines.end();i++)
	{
	  const Edge &line= **i;
	  retval= std::min(retval, line.getElemSize());
	}
    return retval;
  }
  
//! @brief Return the minimum element size according to the
//! number of divisions of the lines.
double XC::SetEntities::getAverageElementSize(void) const
  {
    double totalLength= 0.0;
    double totalNDiv= 0.0;
    if(lines.empty())
      std::clog << getClassName() << "::" << __FUNCTION__
	        << "; WARNING, set has no lines." << std::endl;
    else
      for(lst_line_pointers::const_iterator i= lines.begin();i!=lines.end();i++)
	{
	  const Edge &line= **i;
	  totalLength+= line.getLength();
	  totalNDiv+= line.NDiv();
	}
    return totalLength/totalNDiv;
  }

//! @brief Return a new set that contains the bodies that lie insiof the
//! geometric object.
//!
//! @param geomObj: geometric object that must contain the entities.
//! @param tol: tolerance for "In" function.
XC::SetEntities XC::SetEntities::pickEntitiesInside(const GeomObj3d &geomObj, const double &tol) const
  {
    Preprocessor *prep= const_cast<Preprocessor *>(getPreprocessor());
    SetEntities retval(prep);
    retval.points= points.pickEntitiesInside(geomObj,tol);
    retval.lines= lines.pickEntitiesInside(geomObj,tol);
    retval.surfaces= surfaces.pickEntitiesInside(geomObj,tol);
    retval.bodies= bodies.pickEntitiesInside(geomObj,tol);
    return retval;
  }

//! @brief Select the points identified by the tags in the parameter.
//!
//! @param tags: identifiers of the points to select.
void XC::SetEntities::sel_points_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= getPreprocessor();
        if(preprocessor)
          {
            MultiBlockTopology &mbt= getPreprocessor()->getMultiBlockTopology();
            for(size_t i= 0;i<sz;i++)
              {
	        Pnt *ipt= mbt.getPoints().busca(tags(i)); 
                if(ipt)
                  points.push_back(ipt);
                else
		  std::cerr << "Point identified by tag: "
			    << tags(i) << " not found." << std::endl;
              }
          }
        else
          std::cerr << getClassName() << __FUNCTION__
	            << "; preprocessor needed." << std::endl;
      }
  }

//! @brief Select the lines identified by the tags in the parameter.
//!
//! @param tags: identifiers of the points to select.
void XC::SetEntities::sel_lines_list(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= getPreprocessor();
        if(preprocessor)
          {
            MultiBlockTopology &mbt= getPreprocessor()->getMultiBlockTopology();
            for(size_t i= 0;i<sz;i++)
              {
	        Edge *iedge= mbt.getLines().busca(tags(i)); 
                if(iedge)
                  lines.push_back(iedge);
                else
		  std::cerr << "Line identified by tag: "
			    << tags(i) << " not found." << std::endl;
              }
          }
        else
          std::cerr << getClassName() << __FUNCTION__
	            << "; preprocessor needed." << std::endl;
      }
  }

//! @brief Selects the surfaces with the identifiers being passed as parameter.
void XC::SetEntities::sel_surfaces_lst(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= getPreprocessor();
        if(preprocessor)
          {
            MultiBlockTopology &mbt= getPreprocessor()->getMultiBlockTopology();
            for(size_t i= 0;i<sz;i++)
              {
	        Face *iface= mbt.getSurfaces().busca(tags(i)); 
                if(iface)
                  surfaces.push_back(iface);
                else
		  std::cerr << "Surface with tag: " << tags(i) << " not found." << std::endl;
              }
          }
        else
          std::cerr << getClassName() << __FUNCTION__
	            << "; preprocessor needed." << std::endl;
      }
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::SetEntities::getDbTagData(void) const
  {
    static DbTagData retval(19);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::SetEntities::sendData(Communicator &comm)
  {
    int res= 0;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
//     res+= points.sendTags(9,10,getDbTagData(),comm);
//     res+= lines.sendTags(11,12,getDbTagData(),comm);
//     res+= surfaces.sendTags(13,14,getDbTagData(),comm);
//     res+= bodies.sendTags(15,16,getDbTagData(),comm);
//     res+= uniform_grids.sendTags(17,18,getDbTagData(),comm);
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::SetEntities::recvData(const Communicator &comm)
  {
    ID tmp;
    int res= 0;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
//     tmp= points.receiveTags(9,10,getDbTagData(),comm);
//     sel_points_lista(tmp);
//     tmp= lines.receiveTags(11,12,getDbTagData(),comm);
//     sel_lines_list(tmp);
//     tmp= surfaces.receiveTags(13,14,getDbTagData(),comm);
//     sel_surfaces_list(tmp);
//     tmp= bodies.receiveTags(15,16,getDbTagData(),comm);
//     sel_bodies_list(tmp);
//     tmp= uniform_grids.receiveTags(17,18,getDbTagData(),comm);
    return res;
  }

//!  @brief Destructor.
XC::SetEntities::~SetEntities(void)
  { clearAll(); }

//! @brief Sends object through the communicator argument.
int XC::SetEntities::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(19);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::SetEntities::recvSelf(const Communicator &comm)
  {
    inicComm(19);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }
