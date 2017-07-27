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
//Set.cc

#include "Set.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
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
XC::Set::Set(const std::string &nmb,Preprocessor *md)
  : SetMeshComp(nmb,md), puntos(this), lineas(this), surfaces(this),
    cuerpos(this), uniform_grids(this) {}

//! @brief Copy constructor.
XC::Set::Set(const Set &otro)
  : SetMeshComp(otro)
  {
    copia_listas(otro);
  }

//! @brief Assignment operator.
XC::Set &XC::Set::operator=(const Set &otro)
  {
    SetMeshComp::operator=(otro);
    copia_listas(otro);
    return *this;
  }

//! @brief Copia las listas de objetos of the set s.
//!
//! Copia en ESTE objeto las listas de objetos of the set
//! being passed as parameter.
void XC::Set::copia_listas(const Set &otro)
  {
    SetMeshComp::copia_listas(otro);
    puntos= otro.puntos;
    puntos.set_owner(this);
    lineas= otro.lineas;
    lineas.set_owner(this);
    surfaces= otro.surfaces;
    surfaces.set_owner(this);
    cuerpos= otro.cuerpos;
    cuerpos.set_owner(this);
    uniform_grids= otro.uniform_grids;
    uniform_grids.set_owner(this);
  }

//! @brief Extend this set with the objects of the set
//! being passed as parameter.
void XC::Set::extend_lists(const Set &otro)
  {
    SetMeshComp::extend_lists(otro);
    puntos.extend(otro.puntos);
    lineas.extend(otro.lineas);
    surfaces.extend(otro.surfaces);
    cuerpos.extend(otro.cuerpos);
    uniform_grids.extend(otro.uniform_grids);
  }

// //! @brief Extend this set with the objects from the set
// //! being passed as parameter that fulfill the condition.
// void XC::Set::extend_lists_cond(const Set &otro,const std::string &cond)
//   {
//     SetMeshComp::extend_lists_cond(otro,cond);
//     puntos.extend_cond(otro.puntos,cond);
//     lineas.extend_cond(otro.lineas,cond);
//     surfaces.extend_cond(otro.surfaces,cond);
//     cuerpos.extend_cond(otro.cuerpos,cond);
//     uniform_grids.extend_cond(otro.uniform_grids,cond);
//   }

//! @brief Addition assignment operator.
//!
//! Concatenates the name and description of the argument and
//! extend the lists with the objects from the argument.
XC::Set &XC::Set::operator+=(const Set &otro)
  {
    setName(getName()+"+"+otro.getName()); //Concatenate names.
    description+= "+" + otro.description; //Concatenate descriptions.
    extend_lists(otro); //Extend lists.
    return *this;
  }

//! @brief Addition operator.
//!
//! Concatenates the name and description of the arguments and
//! extend the lists of the first set with the objects from the argument.
XC::Set XC::Set::operator+(const XC::Set &b) const
  {
    Set result(*this);
    result+= b;
    return result;
  }

//! @brief Clears the set.
void XC::Set::clear(void)
  {
    SetMeshComp::clear();
    puntos.clear();
    lineas.clear();
    surfaces.clear();
    cuerpos.clear();
    uniform_grids.clear();
  }

//!  @brief Clears all the objects of the set.
void XC::Set::clearAll(void)
  {
    SetMeshComp::clearAll();
    puntos.clearAll();
    lineas.clearAll();
    surfaces.clearAll();
    cuerpos.clearAll();
    uniform_grids.clearAll();
  }

//! @brief Set indices for the set objects (nodes,elements,points...) to its use in VTK.
void XC::Set::numera(void)
  {
    SetMeshComp::numera();
    SetMeshComp::numera_lista(puntos);
    SetMeshComp::numera_lista(lineas);
    SetMeshComp::numera_lista(surfaces);
//     numera_lista(cuerpos);
  }

//! @brief Moves the objects of the set.
void XC::Set::mueve(const Vector3d &desplaz)
  {
    for(lst_ptr_points::iterator i= puntos.begin();i!=puntos.end();i++)
      (*i)->Mueve(desplaz);
    SetMeshComp::mueve(desplaz);
  }

//! @brief Applies the transformation to the elements of the set.
void XC::Set::Transforma(const TrfGeom &trf)
  {
    for(lst_ptr_points::iterator i= puntos.begin();i!=puntos.end();i++)
      (*i)->Transforma(trf);
    SetMeshComp::Transforma(trf);
  }

//! @brief Aplica to the set the transformation with the index being passed as parameter.
void XC::Set::Transforma(const size_t &indice_trf)
  {
    TrfGeom *trf= getPreprocessor()->getCad().getTransformacionesGeometricas().busca(indice_trf);
    if(trf)
      Transforma(*trf);
  }

//! @brief Creates a copy of the elements of the set and put them in another
//! set with the name is being passed as parameter. The coordinates of the new
//! points will be obtained by adding to the original coordinates those
//! of the vector being passed as parameter.
void XC::Set::create_copy(const std::string &nombre,const Vector3d &v= Vector3d())
  {
    Preprocessor *preprocessor= getPreprocessor();
    if(!preprocessor)
      {
        std::cerr << nombre_clase() << "::" << __FUNCTION__
	          << "; preprocessor not assigned." << std::endl;
	return;
      }
    Set *new_set= getPreprocessor()->get_sets().create_set(nombre);
    std::clog << nombre_clase() << "::" << __FUNCTION__
              << "; warning! copy of nodes and elements"
              << " not implemented." << std::endl;
    //Copiamos los puntos.
    std::map<std::string,std::string> new_points_names;
    for(lst_ptr_points::iterator i= puntos.begin();i!=puntos.end();i++)
      {
        const std::string nombre_viejo= (*i)->getName();
        const std::string new_name= nombre+nombre_viejo;
        Pnt *new_point= getPreprocessor()->getCad().getPuntos().Copia(*i,v);
        new_point->BorraPtrNodElem();
        new_set->puntos.push_back(new_point);
        new_points_names[nombre_viejo]= new_name;
      }
    //Copy lines.
    std::map<std::string,std::string> new_lines_names;
    for(lst_line_pointers::iterator i= lineas.begin();i!=lineas.end();i++)
      {
        const std::string nombre_viejo= (*i)->getName();
        const std::string new_name= nombre+nombre_viejo;
        Edge *new_edge= getPreprocessor()->getCad().getLineas().createCopy(*i);
        new_edge->BorraPtrNodElem();
        new_set->lineas.push_back(new_edge);
        new_lines_names[nombre_viejo]= new_name;
        const size_t nv= new_edge->NumVertices();
        for(size_t i= 0;i<nv;i++)
          {
            const Pnt *vertice_viejo= new_edge->GetVertice(i);
            const std::string nombre_viejo= vertice_viejo->getName();
            const std::string new_name= new_lines_names[nombre_viejo];
            Pnt *new_point= new_set->puntos.BuscaNmb(new_name);
            new_edge->SetVertice(i,new_point);
	  }
      }
  }

//! @brief Create nodes and, where appropriate, elements on set points.
void XC::Set::point_meshing(meshing_dir dm)
  {
    if(verbosity>2)
      std::clog << "Meshing points...";
    for(lst_ptr_points::iterator i= puntos.begin();i!=puntos.end();i++)
      (*i)->genMesh(dm);
    if(verbosity>2)
      std::clog << "done." << std::endl;
  }

//! @brief Create nodes and, where appropriate, elements on set lines.
void XC::Set::line_meshing(meshing_dir dm)
  {
    if(verbosity>2)
      std::clog << "Meshing lines...";
    for(lst_line_pointers::iterator i= lineas.begin();i!=lineas.end();i++)
      (*i)->genMesh(dm);
    if(verbosity>2)
      std::clog << "done." << std::endl;
  }

//! @brief Create nodes and, where appropriate, elements on surfaces.
void XC::Set::surface_meshing(meshing_dir dm)
  {
    if(verbosity>2)
      std::clog << "Meshing surfaces...";
    for(lst_surface_ptrs::iterator i= surfaces.begin();i!=surfaces.end();i++)
      (*i)->genMesh(dm);
    if(verbosity>2)
      std::clog << "done." << std::endl;
  }

//! @brief Create nodes and, where appropriate, elements on set bodies.
void XC::Set::body_meshing(meshing_dir dm)
  {
    if(verbosity>2)
      std::clog << "Meshing bodies...";
    for(lst_ptr_cuerpos::iterator i= cuerpos.begin();i!=cuerpos.end();i++)
      (*i)->genMesh(dm);
    if(verbosity>2)
      std::clog << "done." << std::endl;
  }

//! @brief Creates nodes and, eventually, elements on the points of the set.
void XC::Set::uniform_grid_meshing(meshing_dir dm)
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
void XC::Set::genMesh(meshing_dir dm)
  {
    Preprocessor *mdl= getPreprocessor();
    assert(mdl);
    mdl->get_sets().abre_set(getName()); //To let nodes and elements enter this set.

    if(verbosity>1)
      std::clog << "Meshing set: " << getName() << " ...";

    //body_meshing(dm);
    surface_meshing(dm);
    line_meshing(dm);
    point_meshing(dm);
    uniform_grid_meshing(dm);

    mdl->get_sets().cierra_set(getName()); //Cerramos.

    if(verbosity>1)
      std::clog << "done." << std::endl;
  }

//! @brief Returns true if the point belongs to the set.
bool XC::Set::In(const Pnt *p) const
  { return puntos.in(p); }

//! @brief Returns true if the edge belongs to the set.
bool XC::Set::In(const Edge *e) const
  { return lineas.in(e); }

//! @brief Returns true if the surface belongs to the set.
bool XC::Set::In(const Face *f) const
  { return surfaces.in(f); }

//! @brief Returns true if the body belongs to the set.
bool XC::Set::In(const Body *b) const
  { return cuerpos.in(b); }

//! @brief Returns true if the «uniform grid» belongs to the set.
bool XC::Set::In(const UniformGrid *ug) const
  { return uniform_grids.in(ug); }

//! @brief Appends to the set being passed as parameter
//! the elements that intervene on the definition
//! of those entities that are already in the set.
void XC::Set::CompletaHaciaAbajo(void)
  {
//     for(lst_cuerpos::iterator i=cuerpos.begin();i!=cuerpos.end();i++)
//       {
//         lst_surfaces ss= (*i)->getSurfaces();
//         surfaces.insert(surfaces.end(),ss.begin(),ss.end());
//       }
    for(sup_iterator i=surfaces.begin();i!=surfaces.end();i++)
      {
        //Lineas.
        lst_line_pointers ll((*i)->GetEdges());
        lineas.insert(lineas.end(),ll.begin(),ll.end());

        //Elements.
        TritrizPtrElem &ttz_elements= (*i)->getTtzElements();
        const size_t ncapas= ttz_elements.GetCapas();
        const size_t nfilas= ttz_elements.getNumFilas();
        const size_t ncols= ttz_elements.getNumCols();
        for(size_t i=1;i<=ncapas;i++)
          for(size_t j=1;j<=nfilas;j++)
            for(size_t k=1;k<=ncols;k++)
              addElement(ttz_elements(i,j,k));

      }
    for(lin_iterator i=lineas.begin();i!=lineas.end();i++)
      {
        //Puntos.
        const size_t nv= (*i)->NumVertices();
        for(register size_t j=1;j<=nv;j++)
          puntos.push_back(const_cast<Pnt *>((*i)->GetVertice(j)));

        //Elements.
        TritrizPtrElem &ttz_elements= (*i)->getTtzElements();
        const size_t ncapas= ttz_elements.GetCapas();
        const size_t nfilas= ttz_elements.getNumFilas();
        const size_t ncols= ttz_elements.getNumCols();
        for(size_t i=1;i<=ncapas;i++)
          for(size_t j=1;j<=nfilas;j++)
            for(size_t k=1;k<=ncols;k++)
              addElement(ttz_elements(i,j,k));
      }
    SetMeshComp::CompletaHaciaAbajo();
  }

//! @brief  Appends to this set the objects that make reference
//! to one or more of the objects that already make part of the set.
void XC::Set::CompletaHaciaArriba(void)
  {
    SetMeshComp::CompletaHaciaArriba();
    std::cerr << "Set::CompletaHaciaArriba() work in progress." << std::endl;
    for(pnt_iterator i=puntos.begin();i!=puntos.end();i++)
      {
        std::set<const Edge *> ll= GetLineasTocan(**i);
        for(std::set<const Edge *>::const_iterator j= ll.begin();j!=ll.end();j++)
          lineas.push_back(const_cast<Edge *>(*j));
      }
    for(lin_iterator i=lineas.begin();i!=lineas.end();i++)
      {
        lst_surface_ptrs ss(GetSupsTocan(**i));
        surfaces.insert(surfaces.end(),ss.begin(),ss.end());
      }
//     for(lst_surfaces::iterator i=surfaces.begin();i!=surfaces.end();i++)
//       {
//         lst_cuerpos bb= GetCuerposTocan(**i);
//         cuerpos.insert(cuerpos.end(),bb.begin(),bb.end());
//       }
  }

//! @brief Select the points identified by the tags in the parameter.
//!
//! @param tags: identifiers of the points to select.
void XC::Set::sel_points_lista(const ID &tags)
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
                  puntos.push_back(ipt);
                else
		  std::cerr << "Point identified by tag: "
			    << tags(i) << " not found." << std::endl;
              }
          }
        else
          std::cerr << nombre_clase() << __FUNCTION__
	            << "; preprocessor needed." << std::endl;
      }
  }

//! @brief Select the lines identified by the tags in the parameter.
//!
//! @param tags: identifiers of the points to select.
void XC::Set::sel_lineas_lista(const ID &tags)
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
                  lineas.push_back(iedge);
                else
		  std::cerr << "Line identified by tag: "
			    << tags(i) << " not found." << std::endl;
              }
          }
        else
          std::cerr << nombre_clase() << __FUNCTION__
	            << "; preprocessor needed." << std::endl;
      }
  }

//! @brief Selects the surfaces with the identifiers being passed as parameter.
void XC::Set::sel_surfaces_lst(const ID &tags)
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
          std::cerr << nombre_clase() << __FUNCTION__
	            << "; preprocessor needed." << std::endl;
      }
  }

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::Set::getDbTagData(void) const
  {
    static DbTagData retval(19);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::Set::sendData(CommParameters &cp)
  {
    int res= SetMeshComp::sendData(cp);
//     res+= puntos.sendTags(9,10,getDbTagData(),cp);
//     res+= lineas.sendTags(11,12,getDbTagData(),cp);
//     res+= surfaces.sendTags(13,14,getDbTagData(),cp);
//     res+= cuerpos.sendTags(15,16,getDbTagData(),cp);
//     res+= uniform_grids.sendTags(17,18,getDbTagData(),cp);
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::Set::recvData(const CommParameters &cp)
  {
    ID tmp;
    int res= SetMeshComp::recvData(cp);
//     tmp= puntos.receiveTags(9,10,getDbTagData(),cp);
//     sel_points_lista(tmp);
//     tmp= lineas.receiveTags(11,12,getDbTagData(),cp);
//     sel_lineas_lista(tmp);
//     tmp= surfaces.receiveTags(13,14,getDbTagData(),cp);
//     sel_surfaces_lst(tmp);
//     tmp= cuerpos.receiveTags(15,16,getDbTagData(),cp);
//     sel_cuerpos_lista(tmp);
//     tmp= uniform_grids.receiveTags(17,18,getDbTagData(),cp);
    return res;
  }

//!  @brief Destructor.
XC::Set::~Set(void)
  { clearAll(); }

//! @brief Sends object through the channel being passed as parameter.
int XC::Set::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(19);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Set::recvSelf(const CommParameters &cp)
  {
    inicComm(19);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
