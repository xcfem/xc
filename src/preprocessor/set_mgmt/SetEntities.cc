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
//SetEntities.cc

#include "SetEntities.h"
#include "domain/domain/Domain.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "preprocessor/cad/entidades/Edge.h"
#include "preprocessor/cad/entidades/Face.h"
#include "preprocessor/cad/entidades/Body.h"
#include "preprocessor/cad/entidades/UniformGrid.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "preprocessor/cad/trf/TrfGeom.h"
#include "utility/matrix/ID.h"

#include "xc_utils/src/geom/pos_vec/SVD3d.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d3/SemiEspacio3d.h"


//! @brief Constructor.
XC::SetEntities::SetEntities(Preprocessor *md)
  : PreprocessorContainer(md), MovableObject(0), points(this), lines(this), surfaces(this),
    bodies(this), uniform_grids(this) {}

//! @brief Copy constructor.
XC::SetEntities::SetEntities(const SetEntities &other)
  : PreprocessorContainer(other), MovableObject(other)
  {
    copia_listas(other);
  }

//! @brief Assignment operator.
XC::SetEntities &XC::SetEntities::operator=(const SetEntities &other)
  {
    PreprocessorContainer::operator=(other);
    MovableObject::operator=(other);
    copia_listas(other);
    return *this;
  }

//! @brief Copia las listas de objetos of the set s.
//!
//! Copia en ESTE objeto las listas de objetos of the set
//! being passed as parameter.
void XC::SetEntities::copia_listas(const SetEntities &other)
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
void XC::SetEntities::fillDownwards(SetMeshComp &mc)
  {
//     for(lst_cuerpos::iterator i=bodies.begin();i!=bodies.end();i++)
//       {
//         lst_surfaces ss= (*i)->getSurfaces();
//         surfaces.insert(surfaces.end(),ss.begin(),ss.end());
//       }
    for(sup_iterator i=surfaces.begin();i!=surfaces.end();i++)
      {
        //Lines.
        lst_line_pointers ll((*i)->GetEdges());
        lines.insert(lines.end(),ll.begin(),ll.end());

        //Elements.
        TritrizPtrElem &ttz_elements= (*i)->getTtzElements();
        const size_t ncapas= ttz_elements.GetCapas();
        const size_t nfilas= ttz_elements.getNumFilas();
        const size_t ncols= ttz_elements.getNumCols();
        for(size_t i=1;i<=ncapas;i++)
          for(size_t j=1;j<=nfilas;j++)
            for(size_t k=1;k<=ncols;k++)
              mc.addElement(ttz_elements(i,j,k));

      }
    for(lin_iterator i=lines.begin();i!=lines.end();i++)
      {
        //Points.
        const size_t nv= (*i)->NumVertices();
        for(register size_t j=1;j<=nv;j++)
          points.push_back(const_cast<Pnt *>((*i)->GetVertice(j)));

        //Elements.
        TritrizPtrElem &ttz_elements= (*i)->getTtzElements();
        const size_t ncapas= ttz_elements.GetCapas();
        const size_t nfilas= ttz_elements.getNumFilas();
        const size_t ncols= ttz_elements.getNumCols();
        for(size_t i=1;i<=ncapas;i++)
          for(size_t j=1;j<=nfilas;j++)
            for(size_t k=1;k<=ncols;k++)
              mc.addElement(ttz_elements(i,j,k));
      }
  }

//! @brief  Appends to this set the objects that make reference
//! to one or more of the objects that already make part of the set.
void XC::SetEntities::fillUpwards(const SetMeshComp &mc)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; work in progress." << std::endl;
    for(pnt_iterator i=points.begin();i!=points.end();i++)
      {
        std::set<const Edge *> ll= GetLineasTocan(**i);
        for(std::set<const Edge *>::const_iterator j= ll.begin();j!=ll.end();j++)
          lines.push_back(const_cast<Edge *>(*j));
      }
    for(lin_iterator i=lines.begin();i!=lines.end();i++)
      {
        lst_surface_ptrs ss(GetSupsTocan(**i));
        surfaces.insert(surfaces.end(),ss.begin(),ss.end());
      }
//     for(lst_surfaces::iterator i=surfaces.begin();i!=surfaces.end();i++)
//       {
//         lst_cuerpos bb= GetCuerposTocan(**i);
//         bodies.insert(bodies.end(),bb.begin(),bb.end());
//       }
  }

//! @brief Moves the objects of the set.
void XC::SetEntities::mueve(const Vector3d &desplaz)
  {
    for(lst_ptr_points::iterator i= points.begin();i!=points.end();i++)
      (*i)->Mueve(desplaz);
  }

//! @brief Applies the transformation to the elements of the set.
void XC::SetEntities::Transforma(const TrfGeom &trf)
  {
    for(lst_ptr_points::iterator i= points.begin();i!=points.end();i++)
      (*i)->Transforma(trf);
  }

//! @brief Aplica to the set the transformation with the index being passed as parameter.
void XC::SetEntities::Transforma(const size_t &indice_trf)
  {
    TrfGeom *trf= getPreprocessor()->getCad().getTransformacionesGeometricas().busca(indice_trf);
    if(trf)
      Transforma(*trf);
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
	    Pnt *new_point= preprocessor->getCad().getPuntos().Copia(*i,v);
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
	    Edge *new_edge= preprocessor->getCad().getLineas().createCopy(*i);
	    new_edge->BorraPtrNodElem();
	    retval.lines.push_back(new_edge);
	    new_lines_names[oldName]= new_name;
	    const size_t nv= new_edge->NumVertices();
	    for(size_t i= 0;i<nv;i++)
	      {
		const Pnt *vertice_viejo= new_edge->GetVertice(i);
		const std::string oldName= vertice_viejo->getName();
		const std::string new_name= new_lines_names[oldName];
		Pnt *new_point= retval.points.searchName(new_name);
		new_edge->SetVertice(i,new_point);
	      }
	  }
      }
    return retval;
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
    for(lst_ptr_cuerpos::iterator i= bodies.begin();i!=bodies.end();i++)
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

    //body_meshing(dm);
    surface_meshing(dm);
    line_meshing(dm);
    point_meshing(dm);
    uniform_grid_meshing(dm);

    prep->get_sets().cierra_set(setName); //Cerramos.

    if(verbosity>1)
      std::clog << "done." << std::endl;
  }

//! @brief Returns true if the point belongs to the set.
bool XC::SetEntities::In(const Pnt *p) const
  { return points.in(p); }

//! @brief Return a new set that contains the points that lie inside the
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

//! @brief Returns true if the edge belongs to the set.
bool XC::SetEntities::In(const Edge *e) const
  { return lines.in(e); }

//! @brief Return a new set that contains the lines that lie inside the
//! geometric object.
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

//! @brief Return a new set that contains the surfaces that lie inside the
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

//! @brief Return a new set that contains the bodies that lie inside the
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
            Cad &cad= getPreprocessor()->getCad();
            for(size_t i= 0;i<sz;i++)
              {
	        Pnt *ipt= cad.getPuntos().busca(tags(i)); 
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
void XC::SetEntities::sel_lineas_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= getPreprocessor();
        if(preprocessor)
          {
            Cad &cad= getPreprocessor()->getCad();
            for(size_t i= 0;i<sz;i++)
              {
	        Edge *iedge= cad.getLineas().busca(tags(i)); 
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
            Cad &cad= getPreprocessor()->getCad();
            for(size_t i= 0;i<sz;i++)
              {
	        Face *iface= cad.getSurfaces().busca(tags(i)); 
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

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::SetEntities::getDbTagData(void) const
  {
    static DbTagData retval(19);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::SetEntities::sendData(CommParameters &cp)
  {
    int res= 0;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
//     res+= points.sendTags(9,10,getDbTagData(),cp);
//     res+= lines.sendTags(11,12,getDbTagData(),cp);
//     res+= surfaces.sendTags(13,14,getDbTagData(),cp);
//     res+= bodies.sendTags(15,16,getDbTagData(),cp);
//     res+= uniform_grids.sendTags(17,18,getDbTagData(),cp);
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::SetEntities::recvData(const CommParameters &cp)
  {
    ID tmp;
    int res= 0;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
//     tmp= points.receiveTags(9,10,getDbTagData(),cp);
//     sel_points_lista(tmp);
//     tmp= lines.receiveTags(11,12,getDbTagData(),cp);
//     sel_lineas_lista(tmp);
//     tmp= surfaces.receiveTags(13,14,getDbTagData(),cp);
//     sel_surfaces_lst(tmp);
//     tmp= bodies.receiveTags(15,16,getDbTagData(),cp);
//     sel_cuerpos_lista(tmp);
//     tmp= uniform_grids.receiveTags(17,18,getDbTagData(),cp);
    return res;
  }

//!  @brief Destructor.
XC::SetEntities::~SetEntities(void)
  { clearAll(); }

//! @brief Sends object through the channel being passed as parameter.
int XC::SetEntities::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(19);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::SetEntities::recvSelf(const CommParameters &cp)
  {
    inicComm(19);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }
