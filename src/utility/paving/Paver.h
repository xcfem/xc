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
//Paver.h

#ifndef PAVER_H
#define PAVER_H

#include "utility/kernel/CommandEntity.h"
#include "utility/geom/d2/Polygon3d.h"

extern "C" {
#include "paving.h"
}

namespace XC {

class Paver: public CommandEntity
  {
  private:
    integer nbnode; //!< number of nodes on the initial boundary
    integer nprm; //!< number of separate perimeters in the boundary
              //  (there is one outside perimeter and one perimeter for
              // each hole in the boundary)
    integer mln= 8; //!< number of attributes needed in lnodes array.  this
                      // number should be passed in as eight (8) currently.
    std::vector<integer> iptper; //!< integer array of pointers into the
                                 // bnode array. Each pointer indicates the
                                 // beginning node for that perimeter in lperim.
    std::vector<integer> numper; //!< integer array containing the number of
                                 // nodes in each of the perimeters.
    std::vector<integer> lperim; //!< list of perimeter nodes
    std::vector<xc_float> x; //!< xc_float array of x values of nodes dimensioned
                         // to mxnd.
    std::vector<xc_float> y; //!< xc_float array of y values of nodes dimensioned
                         // to mxnd.
    std::vector<xc_float> z; //!< xc_float array of z values of nodes dimensioned
                         // to mxnd.
    std::vector<integer> iexk; //!< integer array of edges attached to each
                               // element dimensioned as (4, mxnd)
    std::vector<integer> inxe; //!< integer array of nodes attached to each
                               // edge dimensioned as (2, mxnd)
    integer nnn= 0; //!< number of nodes in the final mesh
    integer lll= 0; //!< number of edges in the final mesh
    integer kkk= 0; //!< number of elements in the final mesh
    integer mxnd= 0; //!< maximum number of nodes expected in the mesh
                   // (if this is exceeded, noroom is returned as .true.)
    std::vector<xc_float> angle; //!< xc_float array for working space in
                              // paving - dimensioned to (mxnd) - this becomes
                              // the angle array.
    std::vector<xc_float> bnsize; //!< xc_float array for working space in
                              // paving - dimensioned to (mxnd * 2) - this
                              // becomes the bnsize array
    std::vector<integer> lnodes; //!< integer array for working space in
                                 // paving - dimensioned to (mxnd * 8) -
                                 // this becomes the lnodes array
    std::vector<integer> linkpr; //!< integer array for working space in
                                 // paving - dimensioned to (nprm * 3 -
                                 // this becomes the linkpr array
    std::vector<integer> nperim; //!< integer array for working space in
                                 // paving - dimensioned to (nprm -
                                 // this becomes the nperim array
    std::vector<integer> lxk; //!< integer array for working space in
                                 // paving - dimensioned to (mxnd * 4 -
                                 // this becomes the lxk array
    std::vector<integer> kxl; //!< integer array for working space in
                                 // paving - dimensioned to (mxnd * 6 -
                                 // this becomes the kxl array
    std::vector<integer> nxl; //!< integer array for working space in
                                 // paving - dimensioned to (mxnd * 6 -
                                 // this becomes the nxl array
    std::vector<integer> lxn; //!< integer array for working space in
                                 // paving - dimensioned to (mxnd * 4 -
                                 // this becomes the lxn array
    std::vector<integer> nuid; //!< integer array for working space in
                                 // paving - dimensioned to (mxnd -
                                 // this becomes the nuid array
    integer iavail= 0; //!< a dummy integer parameter needed by
                   //! fastq - this becomes iavail.
    integer navail= 0; //!< a dummy integer parameter needed by fastq -  this
                   // becomes navail.
    logical graph= 0; //!< .true. if plotting at each stage is desired
    logical timer= 0; //!< .true. if a timing report is desired
    logical video= 0; //!< .true. if a video animation sequence plot is desired
    xc_float defsiz= 0.0; //!< the default size of the elements in this region
                      // (set it to zero if you don't know what else to do.)
    logical sizeit= 0; //!< .true. if a sizing function is to be used
                       // with paving.
    char dev1[3]= {' ',' ',' '}; //!< a character variable of length 3
                                 // describing the plotting device being used.
    integer kreg= 0; //!< the region number being processed (for plotting id)
    logical batch= 1; //!< .true. if the program is being run without
                      // graphics capabilities.
    logical noroom= 0; //!< .true. if array sizes built according to mxnd are
                    // exceeded (more space is needed)
    logical err= 0; //!< .true. if an error occurs during paving or if noroom
                 // is .true.
    std::vector<xc_float> amesur; //!< the nodal error measure variable
                                   // (used in adaptive meshing).
    std::vector<xc_float> bmesur; //!< ??
                                   // (used in adaptive meshing).
    std::vector<xc_float> xnold; //!< the old xn array for the old mesh
                             //! (used in adaptive meshing)
    std::vector<xc_float> ynold; //!< the old yn array for the old mesh
                             //! (used in adaptive meshing)
    std::vector<integer> nxkold; //!< the old connectivity array of the old mesh
                                 // (used in adaptive meshing).
    std::vector<integer> mmpold; //!< the old material map array (used in adaptive meshing)
    std::vector<integer> linkeg; //!< the linking array mapping elements to a search grid
    std::vector<integer> listeg; //!< the list of element that the link points to
    integer mlink= 0; //!< the maximum space needed for the search grid link (linkeg)
    integer nprold= 0; //!< the number of processed regions in the old mesh
    integer npnold= 0; //!< the number of processed nodes in the old mesh
    integer npeold= 0; //!< the number of processed elements in the old mesh
    integer nnxk= 0; //!< the number of nodes per element in the old mesh
    logical remesh= 0; //!< .true. if an adaptive meshing is requested
    xc_float rexmin= 0.0; //!< min x for the old mesh
    xc_float rexmax= 0.0; //!< max x for the old mesh
    xc_float reymin= 0.0; //!< min y for the old mesh
    xc_float reymax= 0.0; //!< max y for the old mesh
    int idivis= 0; //!< number of divisions in the search grid link */
    int call_paving(const Ref2d3d &, const Polygon3d &, const std::deque<Polygon3d> &);
    int extract_mesh(const Ref2d3d &);
    std::vector<int> get_elem_nodes(const std::vector<int> &);

    // Resulting mesh.
    std::vector<Pos3d> nodePos; //Positions of the nodes.
    std::vector<std::vector<int> > elemEdges; //Element edges.
    std::deque<std::vector<int> > elemNodes; //Element nodes.
  public:
    Paver(void);
    void report(std::ostream &);
    int mesh(const Polygon3d &, const std::deque<Polygon3d> &);
    int meshPy(const Polygon3d &, const boost::python::list &);
    boost::python::list getNodePositionsPy(void) const;
    boost::python::list getQuadsPy(void) const;
    const std::vector<Pos3d> &getNodePositions(void) const;
    size_t getIndexNearestPosition(const Pos3d &, const size_t &limit) const;
    const std::deque<std::vector<int> > &getQuads(void) const;
  };

} // end of XC namespace

#endif
