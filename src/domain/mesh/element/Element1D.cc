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
//Element1D.cc

#include "Element1D.h"
#include "preprocessor/multi_block_topology/matrices/NodePtrArray3d.h"
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/prep_handlers/LoadHandler.h"
#include "preprocessor/set_mgmt/SetEstruct.h"
#include "domain/mesh/node/Node.h"
#include "domain/load/beam_loads/Beam3dUniformLoad.h"
#include "domain/load/beam_loads/Beam3dPointLoad.h"
#include "domain/load/beam_loads/Beam2dUniformLoad.h"
#include "domain/load/beam_loads/Beam2dPartialUniformLoad.h"
#include "domain/load/beam_loads/Beam2dPointLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"

#include "utility/matrix/Matrix.h"
#include "utility/matrices/m_int.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf2d.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf3d.h"
#include "domain/mesh/element/utils/ParticlePos3d.h"
#include "utility/geom/pos_vec/Pos3dArray.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/coo_sys/Rect3d3dCooSys.h"
#include "utility/geom/d1/Segment3d.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "vtkCellType.h"

//! @brief Constructor.
XC::Element1D::Element1D(int tag, int classTag)
  : ElementBase<2>(tag,classTag), tributaryLengths(2,0.0) {}

//! @brief Constructor.
XC::Element1D::Element1D(int tag, int classTag,int Nd1,int Nd2)
  : ElementBase<2>(tag,classTag), tributaryLengths(2,0.0)
  { theNodes.set_id_nodes(Nd1,Nd2); }


void XC::Element1D::vector2dUniformLoadGlobal(const Vector &v)
  {
    const CrdTransf *crd_trf= getCoordTransf();
    // Some 1D elements like Trusses have no coordinate
    // transformation (and don't accept body loads neither).
    if(crd_trf)
      {
	const Vector vTrf= crd_trf->getVectorLocalCoordFromGlobal(v);
	vector2dUniformLoadLocal(vTrf);
      }
    else
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; WARNING element has no coordinate transformation"
		<< " load ignored." << std::endl;
  }

void XC::Element1D::vector2dUniformLoadLocal(const Vector &v)
  {
    Preprocessor *preprocessor= getPreprocessor();
    MapLoadPatterns &lPatterns= preprocessor->getLoadHandler().getLoadPatterns();
    static ID eTags(1);
    eTags[0]= getTag(); //Load for this element.
    const int &loadTag= lPatterns.getCurrentElementLoadTag(); //Load identifier.

    const size_t sz= v.Size();
    if(sz>1)
      {
        if(sz>2)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; WARNING a vector of dimension 2"
	            << " was expected instead of: " << v << std::endl;
        Beam2dUniformLoad *tmp= new Beam2dUniformLoad(loadTag,v[1],v[0],eTags);
        LoadPattern *lp= lPatterns.getCurrentLoadPatternPtr();
        if(lp)
          {
            lp->addElementalLoad(tmp);
            lPatterns.setCurrentElementLoadTag(loadTag+1);
          }
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; there is no current load pattern. Load ignored."
                    << std::endl; 
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ERROR a vector of dimension 2"
	        << " was expected instead of: " << v << std::endl;
  }

void XC::Element1D::vector2dPartialUniformLoadGlobal(const double &aOverL, const double &bOverL, const Vector &v)
  {
    const CrdTransf *crd_trf= getCoordTransf();
    // Some 1D elements like Trusses have no coordinate
    // transformation (and don't accept body loads neither).
    if(crd_trf)
      {
        const Vector vTrf= crd_trf->getVectorLocalCoordFromGlobal(v);
        vector2dPartialUniformLoadLocal(aOverL, bOverL, vTrf);
      }
    else
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; WARNING element has no coordinate transformation"
		<< " load ignored." << std::endl;
  }

void XC::Element1D::vector2dPartialUniformLoadLocal(const double &aOverL, const double &bOverL, const Vector &v)
  {
    Preprocessor *preprocessor= getPreprocessor();
    MapLoadPatterns &lPatterns= preprocessor->getLoadHandler().getLoadPatterns();
    static ID eTags(1);
    eTags[0]= getTag(); //Load for this element.
    const int &loadTag= lPatterns.getCurrentElementLoadTag(); //Load identifier.

    const size_t sz= v.Size();
    if(sz>1)
      {
        if(sz>2)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; WARNING a vector of dimension 2"
	            << " was expected instead of: " << v << std::endl;
        Beam2dPartialUniformLoad *tmp= new Beam2dPartialUniformLoad(loadTag,v[1],v[0],aOverL,bOverL,eTags);
        LoadPattern *lp= lPatterns.getCurrentLoadPatternPtr();
        if(lp)
          {
            lp->addElementalLoad(tmp);
            lPatterns.setCurrentElementLoadTag(loadTag+1);
          }
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; there is no current load pattern. Load ignored."
                    << std::endl; 
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ERROR a vector of dimension 2"
	        << " was expected instead of: " << v << std::endl;
  }

void XC::Element1D::vector2dPointByRelDistLoadGlobal(const double &x,const Vector &v)
  {
    const size_t sz= v.Size();
    if(sz>1)
      {
        const CrdTransf *crd_trf= getCoordTransf();
	// Some 1D elements like Trusses have no coordinate
	// transformation (and don't accept loads neither).
	if(crd_trf)
	  {
	    const Vector vTrf= crd_trf->getVectorLocalCoordFromGlobal(v);
	    vector2dPointByRelDistLoadLocal(x,vTrf);
	  }
	else
	  std::clog << getClassName() << "::" << __FUNCTION__
		    << "; WARNING element has no coordinate transformation"
		    << " load ignored." << std::endl;	
      }
    else
      std::cerr << getClassName() << __FUNCTION__
                << "; ERROR a vector of dimension 2"
	        << " was expected instead of: " << v << std::endl;
  }

void XC::Element1D::vector2dPointByRelDistLoadLocal(const double &x,const Vector &v)
  {
    const size_t sz= v.Size();
    if(sz>1)
      {
        Preprocessor *preprocessor= getPreprocessor();
        MapLoadPatterns &lPatterns= preprocessor->getLoadHandler().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Load for this element.
        const int &loadTag= lPatterns.getCurrentElementLoadTag(); //Load identifier.

	if(sz>2)
	  std::cerr << getClassName() << "::" << __FUNCTION__
                    << "; ERROR a vector of dimension 2"
	            << " was expected instead of: " << v << std::endl;
        Beam2dPointLoad *tmp= new Beam2dPointLoad(loadTag,v[1],x,eTags,v[0]);
        LoadPattern *lp= lPatterns.getCurrentLoadPatternPtr();
        if(lp)
          {
            lp->addElementalLoad(tmp);
            lPatterns.setCurrentElementLoadTag(loadTag+1);
          }
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; there is no current load pattern. Load ignored."
                    << std::endl; 
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ERROR a vector of dimension 2"
                << " was expected instead of: " << v << std::endl;
  }

void XC::Element1D::vector2dPointLoadGlobal(const Vector &p,const Vector &v)
  {
    const CrdTransf *crd_trf= getCoordTransf();
    // Some 1D elements like Trusses have no coordinate
    // transformation (and don't accept loads neither).
    if(crd_trf)
      {    
	const double x= crd_trf->getPointBasicCoordFromGlobal(p);
	if(x<0.0 || x>1.0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; the relative distance x= "
		    << x << " obtained from point: " << p
		    << " must be between 0 and 1." << std::endl;
	vector2dPointByRelDistLoadGlobal(x,v);
      }
    else
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; WARNING element has no coordinate transformation"
		<< " load ignored." << std::endl;    
  }

void XC::Element1D::vector2dPointLoadLocal(const Vector &p,const Vector &v)
  {
    const CrdTransf *crd_trf= getCoordTransf();
    // Some 1D elements like Trusses have no coordinate
    // transformation (and don't accept loads neither).
    if(crd_trf)
      {        
	const double x= crd_trf->getPointBasicCoordFromGlobal(p);
	if(x<0.0 || x>1.0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; the relative distance x= "
		    << x << " obtained from point: " << p
		    << " must be between 0 and 1." << std::endl;
	vector2dPointByRelDistLoadLocal(x,v);
      }
    else
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; WARNING element has no coordinate transformation"
		<< " load ignored." << std::endl;    
  }

//! @bried Defines a uniform load on the vector from a
//! vector in global coordinates.
void XC::Element1D::vector3dUniformLoadGlobal(const Vector &v)
  {
    const size_t sz= v.Size();
    if(sz>2)
      {
        const CrdTransf *crd_trf= getCoordTransf();
	// Some 1D elements like Trusses have no coordinate
	// transformation (and don't accept loads neither).
	if(crd_trf)
	  {
            const Vector vTrf= crd_trf->getVectorLocalCoordFromGlobal(v);
            vector3dUniformLoadLocal(vTrf);
	  }
	else
	  std::clog << getClassName() << "::" << __FUNCTION__
                    << "; WARNING element has no coordinate transformation"
	            << " load ignored." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ERROR a vector of dimension 3"
	        << " was expected instead of: " << v << std::endl;
  }

void XC::Element1D::vector3dUniformLoadLocal(const Vector &v)
  {
    const size_t sz= v.Size();
    if(sz>2)
      {
        Preprocessor *preprocessor= getPreprocessor();
        MapLoadPatterns &lPatterns= preprocessor->getLoadHandler().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Load for this element.
        const int &loadTag= lPatterns.getCurrentElementLoadTag(); //Load identifier.

        LoadPattern *lp= lPatterns.getCurrentLoadPatternPtr();
        if(lp)
          {
            Beam3dUniformLoad *tmp= new Beam3dUniformLoad(loadTag,v,0.0,eTags);
            lp->addElementalLoad(tmp);
            lPatterns.setCurrentElementLoadTag(loadTag+1);
          }
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; there is no current load pattern. Load ignored."
                    << std::endl; 
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ERROR a vector of dimension 3"
	        << " was expected instead of: " << v << std::endl;
  }

void XC::Element1D::vector3dPointByRelDistLoadGlobal(const double &x,const Vector &v)
  {
    const size_t sz= v.Size();
    if(sz>2)
      {
        const CrdTransf *crd_trf= getCoordTransf();
	// Some 1D elements like Trusses have no coordinate
	// transformation (and don't accept loads neither).
	if(crd_trf)
	  {
            const Vector vTrf= crd_trf->getVectorLocalCoordFromGlobal(v);
            vector3dPointByRelDistLoadLocal(x,vTrf);
	  }
	else
	  std::clog << getClassName() << "::" << __FUNCTION__
                    << "; WARNING element has no coordinate transformation"
	            << " load ignored." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ERROR a vector of dimension 3"
	        << " was expected instead of: " << v << std::endl;
  }

void XC::Element1D::vector3dPointByRelDistLoadLocal(const double &x,const Vector &v)
  {
    const size_t sz= v.Size();
    if(sz>2)
      {
        Preprocessor *preprocessor= getPreprocessor();
        MapLoadPatterns &lPatterns= preprocessor->getLoadHandler().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Load for this element.
        const int &loadTag= lPatterns.getCurrentElementLoadTag(); //Load identifier.

        Beam3dPointLoad *tmp= new Beam3dPointLoad(loadTag,v[1],v[2],x,eTags,v[0]);
        LoadPattern *lp= lPatterns.getCurrentLoadPatternPtr();
        if(lp)
          {
            lp->addElementalLoad(tmp);
            lPatterns.setCurrentElementLoadTag(loadTag+1);
          }
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; there is no current load pattern. Load ignored."
                    << std::endl; 

      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ERROR a vector of dimension 3"
	        << " was expected instead of: " << v << std::endl;
  }

void XC::Element1D::vector3dPointLoadGlobal(const Vector &p,const Vector &v)
  {
    const CrdTransf *crd_trf= getCoordTransf();
    // Some 1D elements like Trusses have no coordinate
    // transformation (and don't accept loads neither).
    if(crd_trf)
      {
	const double x= crd_trf->getPointBasicCoordFromGlobal(p);
	if(x<0.0 || x>1.0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; the relative distance x= "
		    << x << " obtained from point: " << p
		    << " must be between 0 and 1." << std::endl;
	vector3dPointByRelDistLoadGlobal(x,v);
      }
    else
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; WARNING element has no coordinate transformation"
		<< " load ignored." << std::endl;
  }

void XC::Element1D::vector3dPointLoadLocal(const Vector &p,const Vector &v)
  {
    const CrdTransf *crd_trf= getCoordTransf();
    // Some 1D elements like Trusses have no coordinate
    // transformation (and don't accept loads neither).
    if(crd_trf)
      {
	const double x= crd_trf->getPointBasicCoordFromGlobal(p);
	if(x<0.0 || x>1.0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; the relative distance x= "
		    << x << " obtained from point: " << p
		    << " must be between 0 and 1." << std::endl;
	vector3dPointByRelDistLoadLocal(x,v);
      }
    else
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; WARNING element has no coordinate transformation"
		<< " load ignored." << std::endl;
  }

void XC::Element1D::strainLoad(const DeformationPlane &p1,const DeformationPlane &p2)
  {
    Preprocessor *preprocessor= getPreprocessor();
    MapLoadPatterns &lPatterns= preprocessor->getLoadHandler().getLoadPatterns();
    static ID eTags(1);
    eTags[0]= getTag(); //Load for this element.
    const int &loadTag= lPatterns.getCurrentElementLoadTag(); //Load identifier.

    BeamStrainLoad *tmp= new BeamStrainLoad(loadTag,eTags);
    tmp->setDeformationPlane1(p1);
    tmp->setDeformationPlane2(p2);
    LoadPattern *lp= lPatterns.getCurrentLoadPatternPtr();
    if(lp)
      {
        lp->addElementalLoad(tmp);
        lPatterns.setCurrentElementLoadTag(loadTag+1);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; there is no current load pattern. Load ignored."
                << std::endl; 
  }

//! @brief Return the element dimension (0, 1, 2 or 3).
size_t XC::Element1D::getDimension(void) const
  { return 1; }

void meshing_on_i(const XC::Element1D &e,const XC::NodePtrArray3d::const_ref_i_row &nodes,XC::ElemPtrArray3d::var_ref_i_row &elements)
  {
    XC::Element *tmp= nullptr;
    const size_t numberOfLayers= nodes.getNumberOfLayers();
    for(size_t i=1;i<numberOfLayers;i++)
      {
	tmp= e.getCopy();
        const int Nd1= nodes(i)->getTag();
        const int Nd2= nodes(i+1)->getTag();
        bool changed= tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
	if(changed)
	  std::cerr << __FUNCTION__
	            << "; nodes were already assigned."
	            << std::endl;
        elements(i)= tmp;
      }
  }

void meshing_on_j(const XC::Element1D &e,const XC::NodePtrArray3d::const_ref_j_row &nodes,XC::ElemPtrArray3d::var_ref_j_row &elements)
  {
    XC::Element *tmp= nullptr;
    const size_t numberOfRows= nodes.getNumberOfRows();
    for(size_t j=1;j<numberOfRows;j++)
      {
	tmp= e.getCopy();
        const int Nd1= nodes(j)->getTag();
        const int Nd2= nodes(j+1)->getTag();
        bool changed= tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
	if(changed)
	  std::cerr << __FUNCTION__
	            << "; nodes were already assigned."
	            << std::endl;
        elements(j)= tmp;
      }
  }

void meshing_on_k(const XC::Element1D &e,const XC::NodePtrArray3d::const_ref_k_row &nodes,XC::ElemPtrArray3d::var_ref_k_row &elements)
  {
    XC::Element *tmp= nullptr;
    const size_t numberOfColumns= nodes.getNumberOfColumns();
    for(size_t k=1;k<numberOfColumns;k++)
      {
	tmp= e.getCopy();
        const int Nd1= nodes(k)->getTag();
        const int Nd2= nodes(k+1)->getTag();
        bool changed= tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
	if(changed)
	  std::cerr << __FUNCTION__
	            << "; nodes were already assigned."
	            << std::endl;
        elements(k)= tmp;
      }
  }

void XC::Element1D::unidimensional_meshing(const XC::NodePtrArray3d &nodes,XC::ElemPtrArray3d &elements) const
  {
    const size_t numberOfLayers= nodes.getNumberOfLayers();
    const size_t numberOfRows= nodes.getNumberOfRows();
    const size_t numberOfColumns= nodes.getNumberOfColumns();
    if(nodes.isIRow())
      {
        elements= ElemPtrArray3d(numberOfLayers-1,ElemPtrArray(1,1));
        ElemPtrArray3d::var_ref_i_row element_row= elements.getVarRefIRow(1,1);
        meshing_on_i(*this,nodes.getIRowConstRef(1,1),element_row);
      }
    else if(nodes.isJRow())
      {
        elements= ElemPtrArray3d(numberOfLayers,ElemPtrArray(numberOfRows-1,numberOfColumns));
        ElemPtrArray3d::var_ref_j_row element_row= elements.getVarRefJRow(1,1);
        meshing_on_j(*this,nodes.getJRowConstRef(1,1),element_row);
      }
    else if(nodes.isKRow())
      {
        elements= ElemPtrArray3d(numberOfLayers,ElemPtrArray(numberOfRows,numberOfColumns-1));
        ElemPtrArray3d::var_ref_k_row element_row= elements.getVarRefKRow(1,1);
        meshing_on_k(*this,nodes.getKRowConstRef(1,1),element_row);
      }
   }

//! @brief Return a grid of booleans, one for each of the
//! element nodes. If there is a node that doesn't exist
//! for a position the correspondin value will be false.
BoolArray3d XC::Element1D::getNodePattern(void) const
  {
    BoolArray3d retval(1,1,2,true); //One layer, one row, two columns.
    return retval;
  }

//! @brief shape function routine for one-dimesional two node elements.
//! @param r "r" natural coordinate of the point.
//! @param shp[4] shape function values at the (r) point.
void XC::Element1D::shape1d(const double &r, double shp[2])
  {
    shp[0]= (1.0-r)/2.0;
    shp[1]= (1.0+r)/2.0;
  }

//! @brief Put the elements on the nodes being passed as parameter.
XC::ElemPtrArray3d XC::Element1D::put_on_mesh(const NodePtrArray3d &nodes,meshing_dir dm) const
  {
    const size_t numberOfLayers= nodes.getNumberOfLayers();
    const size_t numberOfRows= nodes.getNumberOfRows();
    const size_t numberOfColumns= nodes.getNumberOfColumns();
    const size_t mesh_dim= nodes.GetDim();
    ElemPtrArray3d retval;
    if(mesh_dim<1)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "There is only one node, can't create elements."
		<< std::endl;
    else
      {
        if(mesh_dim<2) //Bidimensional mesh
          unidimensional_meshing(nodes,retval);
        else
          {
            switch(dm)
              {
              case dirm_i:
                if(numberOfLayers<2)
		  std::cerr << getClassName() << "::" << __FUNCTION__
		            << " insufficient number of nodes on i direction."
		            << " Can't create elements." << std::endl;
                else
                  {
                    retval= ElemPtrArray3d(numberOfLayers-1,ElemPtrArray(numberOfRows,numberOfColumns));
                    for(size_t j=1;j<=numberOfRows;j++)
                      for(size_t k=1;k<=numberOfColumns;k++)
                        {
                          ElemPtrArray3d::var_ref_i_row element_row= retval.getVarRefIRow(j,k);
                          meshing_on_i(*this,nodes.getIRowConstRef(j,k),element_row);
                        }
                  }
                break;
              case dirm_j:
                if(numberOfRows<2)
		  std::cerr << getClassName() << "::" << __FUNCTION__
		            << " insufficient number of nodes on j direction."
		            << " Can't create elements." << std::endl;
                else
                  {
                    retval= ElemPtrArray3d(numberOfLayers,ElemPtrArray(numberOfRows-1,numberOfColumns));
                    for(size_t i=1;i<=numberOfLayers;i++)
                      for(size_t k=1;k<=numberOfColumns;k++)
                        {
                          XC::ElemPtrArray3d::var_ref_j_row element_row= retval.getVarRefJRow(i,k);
                          meshing_on_j(*this,nodes.getJRowConstRef(i,k),element_row);
                        }
                  }
                break;
              case dirm_k:
                if(numberOfColumns<2)
		  std::cerr << getClassName() << "::" << __FUNCTION__
		            << " insufficient number of nodes on k direction."
		            << " Can't create elements." << std::endl;
                else
                  {
                    retval= ElemPtrArray3d(numberOfLayers,ElemPtrArray(numberOfRows,numberOfColumns-1));
                    for(size_t i=1;i<=numberOfLayers;i++)
	              for(size_t j=1;j<=numberOfRows;j++)
                        {
                          XC::ElemPtrArray3d::var_ref_k_row element_row= retval.getVarRefKRow(i,j);
                          meshing_on_k(*this,nodes.getKRowConstRef(i,j),element_row);
                        }
                  }
                break;
              default:
                break;
              }
          }  
      }
    return retval;
  }

XC::ElemPtrArray3d XC::Element1D::sew(const SetEstruct &f1,const SetEstruct &f2) const
  {
    const size_t nelem= f1.getNumberOfNodes();
    ElemPtrArray3d retval(nelem,1,1);
    const size_t numberOfLayers= std::min(f1.getNumNodeLayers(),f2.getNumNodeLayers());
    const size_t numberOfRows= std::min(f1.getNumNodeRows(),f2.getNumNodeRows());
    const size_t numberOfColumns= std::min(f1.getNumNodeColumns(),f2.getNumNodeColumns());
    for(size_t i=1;i<=numberOfLayers;i++)
      for(size_t j=1;j<=numberOfRows;j++)
        for(size_t k=1;k<=numberOfColumns;k++)
          {
            Element *tmp= getCopy();
            const int Nd1= f1.getNode(i,j,k)->getTag();
            const int Nd2= f2.getNode(i,j,k)->getTag();
            bool changed= tmp->getNodePtrs().set_id_nodes(Nd1,Nd2);
  	    if(changed)
	      std::cerr << __FUNCTION__
	                << "; nodes were already assigned."
	                << std::endl;
            retval(i,1,1)= tmp;
          }
    return retval;
  }

void XC::Element1D::setDomain(Domain *theDomain)
  {
    ElementBase<2>::setDomain(theDomain);
  }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
XC::CrdTransf *XC::Element1D::getCoordTransf(void)
  { return nullptr; }

//! @brief Returns (if possible) a const pointer to the coordinate transformation.
const XC::CrdTransf *XC::Element1D::getCoordTransf(void) const
  { return nullptr; }

//! @brief Returns (and checks that it exists) a const pointer to the coordinate transformation.
XC::CrdTransf *XC::Element1D::checkCoordTransf(void)
  {
    CrdTransf *ct= getCoordTransf();
    if(!ct)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; coordinate transformation not defined."
                << std::endl;
    return ct;
  }

//! @brief Returns (and checks that it exists) a const pointer to the coordinate transformation.
const XC::CrdTransf *XC::Element1D::checkCoordTransf(void) const
  {
    const CrdTransf *ct= getCoordTransf();
    if(!ct)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; coordinate transformation not defined."
                << std::endl;
    return ct;
  }


//! @brief Returns a matrix with the axes of the element as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
XC::Matrix XC::Element1D::getLocalAxes(bool initialGeometry) const
  {
    static Matrix retval;
    const CrdTransf *crdTransf= getCoordTransf();
    if(crdTransf)
      retval= crdTransf->getLocalAxes(initialGeometry);
    else
      {
	double factor= 0.0;
	if(!initialGeometry)
	  factor= 1.0;
        const Pos3d p0= theNodes[0]->getCurrentPosition3d(factor);
        const Pos3d p1= theNodes[1]->getCurrentPosition3d(factor);
	const Rect3d3dCooSys sc(p0,p1);
	const Vector3d i= sc.getIVector();
	retval= Matrix(3,3);
	retval(0,0)= i.x(); retval(0,1)= i.y(); retval(0,2)= i.z();
	const Vector3d j= sc.getJVector();
	retval(1,0)= j.x(); retval(1,1)= j.y(); retval(1,2)= j.z();
	const Vector3d k= sc.getKVector();
	retval(2,0)= k.x(); retval(2,1)= k.y(); retval(2,2)= k.z();
      }
    return retval;
  }

//! @brief Return points distributed between the nodes as a matrix with the coordinates as rows.
const XC::Matrix &XC::Element1D::getCooPoints(const size_t &ndiv) const
  {
    static Matrix retval;
    const CrdTransf *tmp= getCoordTransf();
    if(tmp)
      retval= tmp->getCooPoints(ndiv);
    else
      {
        const Pos3d p0= theNodes[0]->getInitialPosition3d();
        const Pos3d p1= theNodes[1]->getInitialPosition3d();
        Pos3dArray linea(p0,p1,ndiv);
        retval= Matrix(ndiv+1,3);
        Pos3d tmp;
        for(size_t i= 0;i<ndiv+1;i++)
          {
            tmp= linea(i+1,1);
            retval(i,0)= tmp.x();
            retval(i,1)= tmp.y();
            retval(i,2)= tmp.z();
          }
      }
    return retval;
  }

//! @brief Return the point that correspond to the relative coordinate 0<=xrel<=1.
const XC::Vector &XC::Element1D::getCooPoint(const double &xrel) const
  {
    static Vector retval;
    const CrdTransf *tmp= getCoordTransf();
    if(tmp)
      retval= tmp->getCooPoint(xrel);
    else
      {
        const Pos3d p0= theNodes[0]->getInitialPosition3d();
        const Pos3d p1= theNodes[1]->getInitialPosition3d();
        const Vector3d v= p1-p0;
        retval= Vector(3);
        const Pos3d tmp= p0+xrel*v;
        retval(0)= tmp.x();
        retval(1)= tmp.y();
        retval(2)= tmp.z();
      }
    return retval;
  }

//! @brief Return the segment defined by the element.
Segment3d XC::Element1D::getLineSegment(bool initialGeometry) const
  {
    if(initialGeometry)
      return Segment3d(theNodes[0]->getInitialPosition3d(),theNodes[1]->getInitialPosition3d());
    else
      return Segment3d(theNodes[0]->getCurrentPosition3d(),theNodes[1]->getCurrentPosition3d());
  }

//! @brief Return the projection of the argument on the element.
//! @param p: position to project.
//! @param initialGeometry: if true use undeformed element geometry.
Pos2d XC::Element1D::getProjection(const Pos2d &p,bool initialGeometry) const
  {
    const Pos3d tmp(getProjection(Pos3d(p.x(),p.y(),0.0)));
    return tmp.XY2DProjection();
  }
  
//! @brief Return the projection of the argument on the element.
//! @param p: position to project.
//! @param initialGeometry: if true use undeformed element geometry.
Pos3d XC::Element1D::getProjection(const Pos3d &p,bool initialGeometry) const
  {
    const Segment3d sg= getLineSegment(initialGeometry);
    return sg.Projection(p);
  }

//! @brief Return the segment defined by the element.
double XC::Element1D::getLength(bool initialGeometry) const
  {
    if(initialGeometry)
      return dist(theNodes[0]->getInitialPosition3d(),theNodes[1]->getInitialPosition3d());
    else
      return dist(theNodes[0]->getCurrentPosition3d(),theNodes[1]->getCurrentPosition3d());
  }

//! @brief Return the squared distance from the element to the point
//! being passed as parameter.
double XC::Element1D::getDist2(const Pos2d &p,bool initialGeometry) const
  { return getDist2(To3dXY2d(p),initialGeometry); }

//! @brief Return the the distance from the element to the point
//! being passed as parameter.
double XC::Element1D::getDist(const Pos2d &p,bool initialGeometry) const
  { return getDist(To3dXY2d(p),initialGeometry); }

//! @brief Return the squared distance from the element to the point
//! being passed as parameter.
double XC::Element1D::getDist2(const Pos3d &p,bool initialGeometry) const
  {
    const Segment3d sg(getLineSegment(initialGeometry));
    return sg.dist2(p);
  }

//! @brief Return the the distance from the element to the point
//! being passed as parameter.
double XC::Element1D::getDist(const Pos3d &p,bool initialGeometry) const
  {
    const Segment3d sg(getLineSegment(initialGeometry));
    return sg.dist(p);
  }

//! @brief Send the coordinate transformation through the communicator argument.
int XC::Element1D::sendCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,Communicator &comm)
  {
    int retval= 0;
    CrdTransf *ptr= getCoordTransf();
    DbTagData &dt= getDbTagData();
    if(!ptr)
      setDbTagDataPos(posFlag,1);
    else
      {
        setDbTagDataPos(posFlag,0);
        setDbTagDataPos(posClassTag,ptr->getClassTag());
        comm.sendMovable(*ptr,dt,CommMetaData(posDbTag));
      }
    return retval;
  }

//! @brief Receives the coordinate transformation through the communicator argument.
XC::CrdTransf *XC::Element1D::recvCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,const Communicator &comm)
  {
    CrdTransf *ptr= getCoordTransf();
    DbTagData &dt= getDbTagData();
    if(getDbTagDataPos(posFlag) == 0)
      {
        // make some room and read in the vector
        if(!ptr)
          {
            ptr= comm.getNewCrdTransf(getDbTagDataPos(posClassTag));
            if(!ptr)
              std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; ran out of memory\n";
          }
        if(ptr)
          {
            int res= comm.receiveMovable(*ptr,dt,CommMetaData(posDbTag));
            ptr->revertToLastCommit();// Revert the crdtrasf to its last committed state
            if(res<0)
              std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; failed to receive vector data\n";
          }
      }
    return ptr;
  }

//! @brief Receive a 2D coordinate transformation through the communicator argument.
XC::CrdTransf2d *XC::Element1D::recvCoordTransf2d(int posFlag,const int &posClassTag,const int &posDbTag,const Communicator &comm)
  {
    CrdTransf2d *retval= nullptr;
    CrdTransf *tmp= recvCoordTransf(posFlag,posClassTag,posDbTag,comm);
    if(tmp)
      {
        retval= dynamic_cast<CrdTransf2d *>(tmp);
        if(!retval)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "Incompatible transformation type."
                      << std::endl;
            delete tmp;
          }
      }
    return retval;
  }

//! @brief Receive a 3D coordinate transformation through the communicator argument.
XC::CrdTransf3d *XC::Element1D::recvCoordTransf3d(int posFlag,const int &posClassTag,const int &posDbTag,const Communicator &comm)
  {
    CrdTransf3d *retval= nullptr;
    CrdTransf *tmp= recvCoordTransf(posFlag,posClassTag,posDbTag,comm);
    if(tmp)
      {
        retval= dynamic_cast<CrdTransf3d *>(tmp);
        if(!retval)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "Incompatible transformation type."
                      << std::endl;
            delete tmp;
          }
      }
    return retval;
  }


//! @brief VTK interface.
int XC::Element1D::getVtkCellType(void) const
  { return VTK_LINE; }

//! @brief Return the local coordinates of the point
double XC::Element1D::getLocalCoordinates(const Pos3d &pt,bool initialGeometry) const
  {
    const Pos3d proj= getProjection(pt, initialGeometry);
    Pos3d node0Pos;
    if(initialGeometry)
      node0Pos= theNodes[0]->getInitialPosition3d();
    else
      node0Pos= theNodes[0]->getCurrentPosition3d();
    return dist(node0Pos, proj);
  }

//! @brief Return the local coordinates of the point
XC::ParticlePos3d XC::Element1D::getNaturalCoordinates(const Pos3d &pt, bool initialGeometry) const
  {
    const double localCoord= getLocalCoordinates(pt, initialGeometry);
    const double L= getLength(initialGeometry);
    
    return ParticlePos3d(2*(localCoord/L)-1.0,0.0,0.0);
  }
      
//! @brief Returns interpolation factors for a material point.
//! @param pos: natural coordinates of the material point.
XC::Vector XC::Element1D::getInterpolationFactors(const ParticlePos3d &pos) const
  {
    static const int numberOfNodes= 2;
    static double shp[numberOfNodes]; // storage for shape functions values

    shape1d(pos.r_coordinate(), shp);

    Vector retval(2);
    retval[0]= shp[0];
    retval[1]= shp[1];
    
    return retval;
  }

//! @brief Returns interpolation factors for a material point.
XC::Vector XC::Element1D::getInterpolationFactors(const Pos3d &pos) const
  { return getInterpolationFactors(getNaturalCoordinates(pos)); }


//! @brief Compute the tributary lengths that corresponds to each
//! node of the element
void XC::Element1D::computeTributaryLengths(bool initialGeometry) const
  {
    const double lt= getLineSegment(initialGeometry).getLength()/2.0;
    tributaryLengths[0]= lt;
    tributaryLengths[1]= lt;
    dumpTributaries(tributaryLengths);
  }

//! @brief Return the tributary length corresponding to the node being passed
//! as parameter.
double XC::Element1D::getTributaryLength(const Node *nod) const
  {
    double retval= 0;
    const int i= theNodes.find(nod);
    if(i>=0) //Node is in this element.
      retval= tributaryLengths[i];
    return retval;
  }
