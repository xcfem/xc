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
//EntMdlr.h
//Multiblock topology entity.

#ifndef ENTMDLR_H
#define ENTMDLR_H

#include "preprocessor/set_mgmt/SetEstruct.h"
#include "preprocessor/multi_block_topology/matrices/TritrizPtrNod.h"
#include "preprocessor/multi_block_topology/matrices/TritrizPtrElem.h"
#include "preprocessor/MeshingParams.h"

class BND3d;
class Pos3d;
class MatrizPos3d;
class TritrizPos3d;
class RangoIndice;
class RangoTritriz;
class GeomObj3d;

namespace XC {
class IRowSet;
class JRowSet;
class KRowSet;

//!  @ingroup MultiBlockTopology
//! 
//!  \brief Multiblock topology object (point, line, face, block,...).
class EntMdlr: public SetEstruct
  {
  private:
    size_t idx; //!< @brief Object index (to be used as index for VTK arrays).
    bool doGenMesh; //!< True if the point must be meshed (node will be created). For exemple is false when it's the middle point of a line.
  protected:
    TritrizPtrNod ttzNodes;
    TritrizPtrElem ttzElements;
    friend class Set;
    friend class SetMeshComp;
    friend class MultiBlockTopology;
    virtual void update_topology(void)= 0;
    void create_nodes(const TritrizPos3d &);
    Node *create_node(const Pos3d &pos,size_t i=1,size_t j=1, size_t k=1);
    bool create_elements(meshing_dir dm);
    Pnt *create_point(const Pos3d &);
    void create_points(const MatrizPos3d &);
    SetEstruct *create_row_set(const RangoTritriz &,const std::string &);

    void clearAll(void);
  public:
    EntMdlr(Preprocessor *m,const size_t &i= 0);
    EntMdlr(const std::string &nombre= "",const size_t &i= 0,Preprocessor *m= nullptr);
    EntMdlr(const EntMdlr &otro);
    EntMdlr &operator=(const EntMdlr &otro);

    virtual void set_index(const size_t &i);
    //! @brief Returns the index of the object for it use in VTK arrays.
    inline size_t getIdx(void) const
      { return idx; }

    virtual bool In(const GeomObj3d &, const double &tol= 0.0) const;
    virtual bool Out(const GeomObj3d &, const double &tol= 0.0) const;

    inline bool hasNodes(void) const
      { return !ttzNodes.empty(); }
    virtual size_t getNumNodeLayers(void) const
      { return ttzNodes.getNumberOfLayers(); }
    virtual size_t getNumNodeRows(void) const
      { return ttzNodes.getNumberOfRows(); }
    virtual size_t getNumNodeColumns(void) const
      { return ttzNodes.getNumberOfColumns(); }
    virtual size_t getNumElementLayers(void) const
      { return ttzElements.getNumberOfLayers(); }
    virtual size_t getNumElementRows(void) const
      { return ttzElements.getNumberOfRows(); }
    virtual size_t getNumElementColumns(void) const
      { return ttzElements.getNumberOfColumns(); }

    virtual Node *getNode(const size_t &i=1,const size_t &j=1,const size_t &k=1);
    virtual const Node *getNode(const size_t &i=1,const size_t &j=1,const size_t &k=1) const;
    Node *getNearestNode(const Pos3d &p);
    const Node *getNearestNode(const Pos3d &p) const;
    ID getNodeIndices(const Node *) const;
    virtual Element *getElement(const size_t &i=1,const size_t &j=1,const size_t &k=1);
    virtual const Element *getElement(const size_t &i=1,const size_t &j=1,const size_t &k=1) const;
    Node *findNode(const int &tag);
    const Node *findNode(const int &tag) const;

    Element *findElement(const int &);
    const Element *findElement(const int &) const;
    Element *getNearestElement(const Pos3d &p);
    const Element *getNearestElement(const Pos3d &p) const;

    TritrizPtrNod &getTtzNodes(void)
      { return ttzNodes; }
    const TritrizPtrNod &getTtzNodes(void) const
      { return ttzNodes; }
    TritrizPtrElem &getTtzElements(void)
      { return ttzElements; }
    const TritrizPtrElem &getTtzElements(void) const
      { return ttzElements; }
    //! @brief Return the object dimension (0, 1, 2 or 3).
    virtual unsigned short int GetDimension(void) const= 0;
    virtual BND3d Bnd(void) const= 0;

    IRowSet getVarRefIRow(size_t f=1,size_t c=1,const std::string &nmb="tmp");
    IRowSet getVarRefIRow(const RangoIndice &layer_range,size_t f,size_t c,const std::string &nmb="tmp");
    IRowSet getVarRefIRow(const RangoTritriz &rango,const std::string &nmb="tmp");

    JRowSet getVarRefJRow(size_t layer=1,size_t c=1,const std::string &nmb="tmp");
    JRowSet getVarRefJRow(size_t layer,const RangoIndice &row_range,size_t c,const std::string &nmb="tmp");
    JRowSet getVarRefJRow(const RangoTritriz &rango,const std::string &nmb="tmp");

    KRowSet getVarRefKRow(size_t layer=1,size_t f=1,const std::string &nmb="tmp");
    KRowSet getVarRefKRow(size_t layer,size_t f,const RangoIndice &column_range,const std::string &nmb="tmp");
    KRowSet getVarRefKRow(const RangoTritriz &rango,const std::string &nmb="tmp");

    void fix(const SFreedom_Constraint &);

    virtual int getMEDCellType(void) const;
    virtual int getVtkCellType(void) const;

    void setGenMesh(bool m);
    const bool &getGenMesh(void) const;

    virtual std::set<SetBase *> get_sets(void) const= 0;

    virtual double getSquaredDistanceTo(const Pos3d &pt) const;
    virtual double getDistanceTo(const Pos3d &pt) const;

    Vector getSimpsonWeights(const std::string &,const std::string &,const size_t &f=1,const size_t &c=1, const size_t &n= 10) const;

    void BorraPtrNodElem(void);


    virtual ~EntMdlr(void);
  };

} //end of XC namespace

#endif
