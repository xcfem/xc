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
//PntMap.cc

#include "PntMap.h"
#include "preprocessor/Preprocessor.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/d2/Plane.h"
#include "utility/geom/d1/Line3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/set_mgmt/Set.h"



//! @brief Constructor.
XC::PntMap::PntMap(MultiBlockTopology *mbt)
  : EntityMap<Pnt>(mbt) {}


//! @brief Insert the new point in the total and the opened sets.
void XC::PntMap::updateSets(Pnt *new_point) const
  {
    MultiBlockTopology *mbt= const_cast<MultiBlockTopology *>(dynamic_cast<const MultiBlockTopology *>(Owner()));
    Preprocessor *preprocessor= mbt->getPreprocessor();
    preprocessor->get_sets().get_set_total()->getPoints().push_back(new_point);
    preprocessor->get_sets().insert_ent_mdlr(new_point);
    MapSet::map_sets &open_sets= preprocessor->get_sets().get_open_sets();
    for(MapSet::map_sets::iterator i= open_sets.begin();i!= open_sets.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->getPoints().push_back(new_point);
      }
  }

//! @brief Return the centroid of the points.
Pos3d XC::PntMap::getCentroid(void) const
  {
    Pos3d retval;
    if(!this->empty())
      {
	const size_t sz= this->size();
	const_iterator bg= this->begin();
	if(sz<2)
	  {
	    const Pos3d pos= (*bg).second->getPos();
	    retval= pos;
	  }
        else
	  {
	    const_iterator i= bg;
	    const Pos3d pos= (*i).second->getPos();
            Vector3d vpos_center_of_mass(pos.VectorPos());
            i++;
            for(; i != this->end(); i++)
	      {
   	        const Pos3d pos= (*i).second->getPos();
                vpos_center_of_mass= vpos_center_of_mass + pos.VectorPos();
	      }
            vpos_center_of_mass= vpos_center_of_mass * (1.0/sz);
            retval+= vpos_center_of_mass;
	  }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; point set is empty." << std::endl;
    return retval;
  }

//! @brief Return the vector defined by the points which indices are being passed as parameters.
Vector3d XC::PntMap::getVector(const Indice &i,const Indice &j) const
  {
    Vector3d retval;
    const Pnt *pA= busca(i);
    const Pnt *pB= busca(j);
    if(!pA)
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; point: " << i
		<< " not found." << std::endl;
    else if(!pB)
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; point: " << j 
		<< " not found." << std::endl;
    else
      retval= Vector3d(pA->getPos(),pB->getPos());
    return retval;
  }

//! @brief Return the line defined by the points which indices are being passed as parameters.
Line3d XC::PntMap::getRecta(const Indice &i,const Indice &j) const
  {
    Line3d retval;
    const Pnt *pA= busca(i);
    const Pnt *pB= busca(j);
    if(!pA)
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; point: " << i
		<< " not found." << std::endl;
    else if(!pB)
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; point: " << j 
		<< " not found." << std::endl;
    else
      retval= Line3d(pA->getPos(),pB->getPos());
    return retval;
  }

//! @brief Return the plane defined by the points which indices are being passed as parameters.
Plane XC::PntMap::getPlane(const Indice &i,const Indice &j,const Indice &k) const
  {
    Plane retval;
    const Pnt *pA= busca(i);
    const Pnt *pB= busca(j);
    const Pnt *pC= busca(k);
    if(!pA)
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; point: " << i
		<< " not found." << std::endl;
    else if(!pB)
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; point: " << j 
		<< " not found." << std::endl;
    else if(!pC)
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; point: " << k 
		<< " not found." << std::endl;
    else
      retval= Plane(pA->getPos(),pB->getPos(),pC->getPos());
    return retval;
  }


//! @brief Creates a new point.
XC::Pnt *XC::PntMap::Crea(void)
  {
    Preprocessor *preprocessor= getPreprocessor();
    assert(preprocessor);
    Pnt *retval= new Pnt("p"+boost::lexical_cast<std::string>(getTag()),preprocessor);
    (*this)[getTag()]= retval;
    updateSets(retval);
    tag++;
    return retval;
  }

//! @brief Check if there is a point too close to the new one.
bool XC::PntMap::checkPosition(const Pos3d &newPos)
  {
    bool retval= true;
    if(verbosity>1)
      {
	const Pnt *tmp= getNearest(newPos);
	if(tmp)
	  {
	    const Pos3d pos= tmp->getPos();
	    const double d2= dist2(pos,newPos);
	    if(d2<(tol*tol))
	      {
		std::clog << getClassName() << "::" << __FUNCTION__
			  << "; new point at position: " << newPos
			  << " is very close to point: " << tmp->getTag()
			  << " at position: " << pos
			  << " distance: " << sqrt(d2)
			  << std::endl;
	      }
	  }
      }
    return retval;
  }

//! @brief Creates a new point.
XC::Pnt *XC::PntMap::New(void)
  {
    Pnt *retval= busca(getTag());
    if(!retval) //The point is new.
      retval= Crea();
    return retval;
  }


//! @brief Creates a new point at the position being passed as parameter.
XC::Pnt *XC::PntMap::New(const Pos3d &pos)
  { 
    Pnt *retval= busca(getTag());
    if(retval)
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; point with tag: " 
                << getTag() << " already exists, doing nothing." << std::endl;
    else //The point is new.
      {
	checkPosition(pos);
        retval= Crea();
        retval->Pos()= pos;
      }
    return retval;
  }

//! @brief Creates a new point with the tag being passed as parameter.
XC::Pnt *XC::PntMap::New(const size_t &tag,const Pos3d &pos)
  {
    bool isNew= true;
    size_t old_tag= getTag();
    Pnt *retval= nullptr;
    setTag(tag); //Point identifier.
    retval= busca(getTag());
    if(retval)
      isNew= false;
    else
      {
        checkPosition(pos);
        retval= Crea();
      }
    if(!isNew)
      setTag(old_tag);
    retval->setPos(pos); //Sets the position.
    return retval;
  }

//! @brief Creates a new point at the position being passed as parameter.
XC::Pnt *XC::PntMap::New(const Pos2d &pos)
  {
    const Pos3d p3d= To3dXY2d(pos);
    return New(p3d);
  }

//! @brief Creates a new point with the tag being passed as parameter.
XC::Pnt *XC::PntMap::New(const size_t &tag,const Pos2d &pos)
  {
    const Pos3d p3d= To3dXY2d(pos);
    return New(tag, p3d);
  }

//! @brief Creates a new point, copy from the point argument with
//! the corresponding name according to the value of the tag. The coordinates
//! of the new point will be those obtained from the addition to the point
//! the vector being passed as parameter.
XC::Pnt *XC::PntMap::Copy(const Pnt *p,const Vector3d &v= Vector3d())
  {
    Pnt *retval= busca(getTag());
    if(!p)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the pointer to original point is null." << std::endl;
        return retval;
      }
    if(retval)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; the point identified by: " 
                << getTag() << " already exists, no changes made." << std::endl;
    else //The point is new.
      {
	checkPosition(p->getPos()+v);
        retval= new Pnt(*p);
        if(retval)
          {
            if(!v.Nulo())
              retval->Pos()+= v;
            retval->Name()= "p"+boost::lexical_cast<std::string>(getTag());
            (*this)[getTag()]= retval;
            updateSets(retval);
            tag++;
	  }
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; memoria agotada." << std::endl; 
      }
    return retval;
  }

//! @brief Create copies of the points which identifiers are being passed as
//! parameters.
void XC::PntMap::Copy(const std::vector<Indice> &indices)
  {
    for(std::vector<Indice>::const_iterator i= indices.begin();i!=indices.end();i++)
      {
        Pnt *original= busca(*i);
        if(original)
          Copy(original);
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; point: " << *i << " not found.\n";
      }
  }

//! @brief Applies the transformation to the points
//! identified by the indices being passed as parameter.
void XC::PntMap::Transform(const TrfGeom &trf,const std::vector<Indice> &indices)
  {
    for(std::vector<Indice>::const_iterator i= indices.begin();i!=indices.end();i++)
      {
        Pnt *p= busca(*i);
        if(p)
	  {
	    // position check?
            p->Transform(trf);
	  }
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; point: " << *i << " not found.\n";
      }
  }

//! @brief Return the distance between the points which identifiers are being
//! passed as parameters.
double XC::PntMap::Dist(const Indice &i,const Indice &j) const
  {
    double retval(-1.0);
    const Pnt *pA= busca(i);
    const Pnt *pB= busca(j);
    if(!pA)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; point identified by: " 
                << i << " not found." << std::endl;
    else if(!pB)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; point identified by: " 
                << j << " not found." << std::endl;
    else
      retval= pA->getDistanceTo(pB->getPos());
    return retval;
  }
