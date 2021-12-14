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
//MultiBlockTopology.cc

#include "MultiBlockTopology.h"
#include "aux_meshing.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/multi_block_topology/entities/1d/Line.h"
#include "preprocessor/multi_block_topology/entities/1d/DividedLine.h"
#include "preprocessor/multi_block_topology/entities/1d/CmbEdge.h"
#include "preprocessor/multi_block_topology/entities/1d/CircularArc.h"
#include "preprocessor/multi_block_topology/entities/2d/QuadSurface.h"
#include "preprocessor/multi_block_topology/entities/3d/Block.h"
#include "preprocessor/multi_block_topology/entities/3d/UniformGrid.h"
#include "preprocessor/multi_block_topology/matrices/PntPtrArray.h"
#include "preprocessor/multi_block_topology/matrices/PntPtrArray3d.h"
#include "trf/Rotation.h"
#include "trf/Reflection.h"
#include "trf/Scaling.h"
#include "trf/Translation.h"

#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"

#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/geom/d1/Line3d.h"

#include "boost/lexical_cast.hpp"


#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::MultiBlockTopology::MultiBlockTopology(Preprocessor *prep)
  : PreprocessorContainer(prep), MovableObject(0), reference_systems(this),
    points(this), edges(this), faces(this), bodies(this), unif_grid(this),
    framework2d(this), framework3d(this) {}

//! @brief Assign indexes to the objects (nodes,elements,points,...)
//! to be used in VTK arrays.
void XC::MultiBlockTopology::numera(void)
  {
    points.numera();
    edges.numera();
    faces.numera();
    bodies.numera();
  }

//! @brief Return the «edge» that has as end points those
//! whose indices are passed as parameters.
XC::Edge *XC::MultiBlockTopology::find_edge_by_endpoints(const PntMap::Indice &pA,const PntMap::Indice &pB)
  {
    Edge *retval= nullptr;
    const Pnt *p1= points.busca(pA);
    const Pnt *p2= points.busca(pB);
    if(p1 && p2)
      { retval= find_edge_ptr_by_endpoints(*p1,*p2); }
    else
      {
        if(!p1)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; point identified by : "
                    << pA << " not found." << std::endl;
        if(!p2)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; point identified by : "
                    << pB << " not found." << std::endl;
      }
    return retval;
  }

//! @brief Returns the «edge» that has as end points those
//! whose indices are passed as parameter.
const XC::Edge *XC::MultiBlockTopology::find_edge_by_endpoints(const PntMap::Indice &pA,const PntMap::Indice &pB) const
  {
    const Edge *retval= nullptr;
    const Pnt *p1= points.busca(pA);
    const Pnt *p2= points.busca(pB);
    if(p1 && p2)
      { retval= find_edge_ptr_by_endpoints(*p1,*p2); }
    else
      {
        if(!p1)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; point identified by : "
                    << pA << " not found." << std::endl;
        if(!p2)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; point identified by : "
                    << pB << " not found." << std::endl;
      }
    return retval;
  }

//! @brief Conciliate number of divisions of the lines.
void XC::MultiBlockTopology::conciliaNDivs(void)
  {
    std::vector<Face *> face_ptrs(faces.size());
    int j= 0;
    for(SurfaceMap::iterator i=faces.begin();i!= faces.end();i++, j++)
      face_ptrs[j]= (*i).second;
    std::vector<Edge *> edge_ptrs(edges.size());
    j= 0;
    for(LineMap::iterator i=edges.begin();i!=edges.end();i++, j++)
      edge_ptrs[j]= (*i).second;
    conciliate_divisions(face_ptrs, edge_ptrs);
    // if(!faces.empty())
    //   {
    //     std::set<const Edge *> tmp_edges;
    //     for(LineMap::iterator i=edges.begin();i!=edges.end();i++)
    //       tmp_edges.insert((*i).second);
    //     const size_t max_num_iter= 1000; 
    //     size_t conta= 0;
    //     while(!tmp_edges.empty())
    //       {
    //         const Edge *lado= *tmp_edges.begin();
    // 	    std::set<const Edge *> homologous= lado->getHomologousSides();
    // 	    if(homologous.empty()) // nothing to do
    // 	      {
    // 		Edge *tmp= const_cast<Edge *>(lado);
    // 	        tmp_edges.erase(tmp);
    // 	      }
    // 	    else
    // 	      {
    // 		homologous.insert(lado);
    // 		const size_t nd= calcula_ndiv_lados(homologous);
    // 		for(std::set<const Edge *>::const_iterator i= homologous.begin();i!=homologous.end();i++)
    // 		  {
    // 		    Edge *tmp= const_cast<Edge *>(*i);
    // 		    tmp->setNDiv(nd);
    // 		    tmp_edges.erase(tmp);
    // 		  }
    // 		conta++;
    // 		if(conta>max_num_iter)
    // 		  {
    // 		    std::cerr << getClassName() << "::" << __FUNCTION__
    // 			      << "; too much iterations." << std::endl;
    // 		    break;
    // 		  }
    // 	      }
    //       }
    //     for(SurfaceMap::iterator i=faces.begin();i!= faces.end();i++)
    //       (*i).second->ConciliaNDivIJ();        
    //   }
  }

//! @brief Search for the entity whose name is passed as a parameter.
XC::SetEstruct *XC::MultiBlockTopology::find_struct_set(const UniformGridMap::Indice &id)
  {
    SetEstruct *retval= points.busca(id);
    if(!retval)
      retval= unif_grid.busca(id);
    return retval;
  }

//! @brief Erase all the geometry entities.
void XC::MultiBlockTopology::clearAll(void)
  {
    reference_systems.clearAll();

    framework2d.clearAll();
    framework3d.clearAll();

    unif_grid.clearAll();
    bodies.clearAll();
    faces.clearAll();
    edges.clearAll();
    points.clearAll();
  }

double XC::MultiBlockTopology::getAverageSize(void) const
  {
    double retval= edges.getAverageLength();
    const size_t numFaces= faces.size();
    if(numFaces>0)
      {
        const double area= faces.getAverageArea();
        retval+= sqrt(area);
      }
    const size_t numBodies= bodies.size();
    if(numBodies>0)
      {
        const double volume= bodies.getAverageVolume();
        retval+= pow(volume,1.0/3.0);
      }
    retval/=3.0;
    return retval;
  }

XC::MultiBlockTopology::~MultiBlockTopology(void)
  { clearAll(); }

//! @brief Return a vector to store the dbTags
//! of the class members
XC::DbTagData &XC::MultiBlockTopology::getDbTagData(void) const
  {
    static DbTagData retval(8);
    return retval;
  }

//! @brief Send data through the communicator argument.
int XC::MultiBlockTopology::sendData(Communicator &comm)
  {
    int res= 0;
    //int res= comm.sendMovable(reference_systems,getDbTagData(),CommMetaData(0));
    //res+= comm.sendMovable(framework2d,getDbTagData(),CommMetaData(1));
    //res+= comm.sendMovable(framework3d,getDbTagData(),CommMetaData(2));

    //res+= comm.sendMovable(unif_grid,getDbTagData(),CommMetaData(3));
    //res+= comm.sendMovable(bodies,getDbTagData(),CommMetaData(4));
    //res+= comm.sendMovable(faces,getDbTagData(),CommMetaData(5));
    //res+= comm.sendMovable(edges,getDbTagData(),CommMetaData(6));
    //res+= comm.sendMovable(points,getDbTagData(),CommMetaData(7));

    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::MultiBlockTopology::recvData(const Communicator &comm)
  {
    int res= 0;
    // int res= comm.receIvemovable(reference_systems,getDbTagData(),CommMetaData(0));
    // res+= comm.receiveMovable(framework2d,getDbTagData(),CommMetaData(1));
    // res+= comm.receiveMovable(framework3d,getDbTagData(),CommMetaData(2));

    //res+= comm.receiveMovable(points,getDbTagData(),CommMetaData(7));
    //res+= comm.receiveMovable(edges,getDbTagData(),CommMetaData(6));
    //res+= comm.receiveMovable(faces,getDbTagData(),CommMetaData(5));
    //res+= comm.receiveMovable(bodies,getDbTagData(),CommMetaData(4));
    //res+= comm.receiveMovable(unif_grid,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Send object through the communicator argument.
int XC::MultiBlockTopology::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(getDbTagData().Size());
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

//! @brief Receive object through the communicator argument.
int XC::MultiBlockTopology::recvSelf(const Communicator &comm)
  {
    inicComm(getDbTagData().Size());
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

