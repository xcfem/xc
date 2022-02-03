// -*-c++-*-
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
//SurfaceMap.h

#ifndef MAPSurfaces_H
#define MAPSurfaces_H

#include "EntityMap.h"
#include "preprocessor/multi_block_topology/entities/2d/Face.h"

namespace XC {
class QuadSurface;
class PolygonalFace;

//! @ingroup MultiBlockTopology
//
//! @brief Model points container.
class SurfaceMap: public EntityMap<Face>
  {
  protected:
    class Graph
      {
      public:
	typedef const Edge *vertex_type;
	typedef std::list<vertex_type> vertex_list;
	typedef std::map<vertex_type, vertex_list > graph_container;
	typedef std::pair<vertex_type, vertex_list> map_pair;
      protected:
	graph_container edges;
      public:
	typedef graph_container::iterator iterator;
	typedef graph_container::const_iterator const_iterator;

	const_iterator find(const vertex_type &v) const
  	  { return edges.find(v); }
	const_iterator begin(void) const
  	  { return edges.begin(); }
	const_iterator end(void) const
  	  { return edges.end(); }
        void add_vertex(const Edge *);
	void add_edge(vertex_type v,vertex_type w)
	  {
	    edges[v].push_back(w);
	    edges[w].push_back(v);
	  }

	std::set<const Edge *> breadth_first_search(vertex_type s) const
	  {
	    std::set<const Edge *> retval;
	    std::map<vertex_type,bool> visited;
	    std::list<vertex_type> q;
	    visited[s]= true;
	    q.push_back(s);
	    while(!q.empty())
	      {
		s = q.front();
		retval.insert(s);
		q.pop_front();
		const_iterator i_edges= find(s);
		for(auto i : i_edges->second)
		  {
		    if(!visited[i])
		      {
			visited[i] = true;
			q.push_back(i);
		      }
		  }
	      }
            return retval;
	  }

      };
  private:
    Graph edgeGraph;
    void add_graph_vertex(const Edge *e);
    void updateGraph(const Face &);
    void updateSets(Face *) const;
  public:
    SurfaceMap(MultiBlockTopology *mbt= nullptr);

    bool conciliaNDivs(void);
    bool checkNDivs(void) const;
    std::set<const XC::Edge *> getHomologousSides(const Edge *) const;
    
    template <class F>
    Face *New(void);
    Face *findOrCreateFace(Pnt *,Pnt *,Pnt *,Pnt *);

    QuadSurface *newQuadSurfacePts(const size_t &, const size_t &,const size_t &,const size_t &);
    QuadSurface *newQuadSurfaceLines(const size_t &, const size_t &,const size_t &,const size_t &);
    QuadSurface *newQuadSurfaceGridPoints(const boost::python::list &);
    PolygonalFace *newPolygonalFacePoints(const ID &);
    PolygonalFace *newPolygonalFacePointsPy(const boost::python::list &);
    double getAverageArea(void) const;
  };


//! @brief Creates a new surface.
template <class F>
Face *SurfaceMap::New(void)
  {
    Face *retval= busca(getTag());
    if(!retval) //Surface is new.
      {
        Preprocessor *preprocessor= getPreprocessor();
        retval= new F(preprocessor);
        if(retval)
          {
            retval->Name()= "f"+boost::lexical_cast<std::string>(getTag());
            (*this)[getTag()]= retval;
            updateSets(retval);
            tag++;
	  }
      }
    return retval;
  }

} //end of XC namespace
#endif
