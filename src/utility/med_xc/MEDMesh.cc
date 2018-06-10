//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//MEDMesh

#include "MEDMesh.h"




#include "utility/matrix/Matrix.h"

//! @brief Free memory.
void XC::MEDMesh::free_mem(void) const
  {
    if(mesh)
      {
        delete mesh;
        mesh= nullptr;
      }
  }

//! @brief Allocs memory for the MED mesh.
void XC::MEDMesh::alloc(const std::string &fName) const
  {
    free_mem();
    mesh= new MEDMEM::MESH(MEDMEM::MED_DRIVER,fName,meshName);
    if(!mesh)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; error in mesh reading: "
                << meshName << " on file: " << fName << std::endl;
  }

void XC::MEDMesh::alloc(const MEDMesh &otro) const
  {
    free_mem();
    if(otro.mesh)
      mesh= new MEDMEM::MESH(*otro.mesh);
  }

//! Constructor.
XC::MEDMesh::MEDMesh(void)
  : mesh(nullptr) {}

//! Constructor.
XC::MEDMesh::MEDMesh(const MEDMesh &otro)
  : MEDObject(otro), mesh(nullptr)
  {
    meshName= otro.meshName;
    alloc(otro);
  }

//! Assignment operator.
XC::MEDMesh &XC::MEDMesh::operator=(const MEDMesh &otro)
  {
    MEDObject::operator=(otro);
    meshName= otro.meshName;
    alloc(otro);
    return *this;
  }
//! Destructor.
XC::MEDMesh::~MEDMesh(void)
  { free_mem(); }

void XC::MEDMesh::read(const std::string &fileName)
  {
    alloc(fileName);
    const std::string nmb= mesh->getName();
    if(nmb!=meshName)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error in reading mesh name: Is asked for #"
                  << meshName <<"# and obtained #"
                  << nmb <<"#" << std::endl ;
      }
  }

size_t XC::MEDMesh::getSpaceDimension(void)
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getSpaceDimension();
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh not read." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getMeshDimension(void)
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getMeshDimension();
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh not read." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfNodes(void)
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getNumberOfNodes();
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh not read." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCoordinatesNames(void)
  {
    boost::python::list retval;
    if(mesh)
      {
        const std::string *coordinatesNames= mesh->getCoordinatesNames();
        const size_t spaceDim= mesh->getSpaceDimension();
        for(size_t i=0;i<spaceDim;i++)
          retval.append(std::string(coordinatesNames[i]));
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh not read." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCoordinatesUnits(void)
  {
    boost::python::list retval;
    if(mesh)
      {
        const std::string *unitsNames= mesh->getCoordinatesUnits();
        const size_t spaceDim= mesh->getSpaceDimension();
        for(size_t i=0;i<spaceDim;i++)
          retval.append(std::string(unitsNames[i]));
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh not read." << std::endl;
    return retval;
  }

XC::Matrix XC::MEDMesh::getCoordinates(void)
  {
    Matrix retval;
    if(mesh)
      {
        const double *coordenadas= mesh->getCoordinates(::MED_FULL_INTERLACE);
        const size_t numNodes= mesh->getNumberOfNodes();
        const size_t spaceDim= mesh->getSpaceDimension();
        retval= Matrix(numNodes,spaceDim);
        for(size_t i=0;i<numNodes;i++)
          for(size_t j=0;j<spaceDim;j++)
            retval(i,j)= coordenadas[i*spaceDim+j];
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh not read." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfCellTypes()
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getNumberOfTypes(MED_EN::MED_CELL);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh not read." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCellTypes(void)
  {
    boost::python::list retval;
    if(mesh)
      {
        const MED_EN::medGeometryElement *types= mesh->getTypes(MED_EN::MED_CELL);
        const size_t numTypes= mesh->getNumberOfTypes(MED_EN::MED_CELL);
        for(size_t i=0;i<numTypes;i++)
          retval.append(types[i]);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; mesh not read." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCellTypeNames(void)
  {
    boost::python::list retval;
    if(mesh)
      {
        const std::string *types= mesh->getCellTypeNames(MED_EN::MED_CELL);
        const size_t numTypes= mesh->getNumberOfTypes(MED_EN::MED_CELL);
        for(size_t i=0;i<numTypes;i++)
          retval.append(std::string(types[i]));
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh not read." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumCellsOfType(int type)
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getNumberOfElements(MED_EN::MED_CELL,type);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh not read." << std::endl;
    return retval;
  }

XC::Matrix XC::MEDMesh::getConnectivityCellsOfType(int type)
  {
    Matrix retval;
    size_t numNodesCellType= type%100;
    size_t numCeldasType= 0;
    if(mesh)
      {
        numCeldasType= mesh->getNumberOfElements(MED_EN::MED_CELL,type);
        retval= Matrix(numCeldasType,numNodesCellType);
        const int *conectividad= mesh->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL,type);
        for(size_t i= 0;i<numCeldasType;i++)
          for(size_t j= 0;j<numNodesCellType;j++)
            retval(i,j)= conectividad[i*numNodesCellType+j];
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh not read." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfGroups(int entity_type)
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getNumberOfGroups(entity_type);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh not read." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfFamilies(int entity_type)
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getNumberOfFamilies(entity_type);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; mesh not read." << std::endl;
    return retval;
  }

