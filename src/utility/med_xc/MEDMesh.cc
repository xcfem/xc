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

//! @brief Libera la memoria ocupada.
void XC::MEDMesh::libera(void) const
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
    libera();
    mesh= new MEDMEM::MESH(MEDMEM::MED_DRIVER,fName,meshName);
    if(!mesh)
      std::cerr << "MEDMesh::alloc; error in mesh reading: "
                << meshName << " on file: " << fName << std::endl;
  }

void XC::MEDMesh::alloc(const MEDMesh &otro) const
  {
    libera();
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
  { libera(); }

void XC::MEDMesh::read(const std::string &fileName)
  {
    alloc(fileName);
    const std::string nmb= mesh->getName();
    if(nmb!=meshName)
      {
        std::cerr << "Error in reading mesh name: Is asked for #"
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
      std::cerr << "MEDMesh::getSpaceDimension(); mesh not readed." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getMeshDimension(void)
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getMeshDimension();
    else
      std::cerr << "MEDMesh::getMeshDimension(); mesh not readed." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfNodes(void)
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getNumberOfNodes();
    else
      std::cerr << "MEDMesh::getNumberOfNodes(); mesh not readed." << std::endl;
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
      std::cerr << "MEDMesh::getCoordinatesNames(); mesh not readed." << std::endl;
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
      std::cerr << "MEDMesh::getCoordinatesUnits(); mesh not readed." << std::endl;
    return retval;
  }

XC::Matrix XC::MEDMesh::getCoordinates(void)
  {
    Matrix retval;
    if(mesh)
      {
        const double *coordenadas= mesh->getCoordinates(::MED_FULL_INTERLACE);
        const size_t numNodos= mesh->getNumberOfNodes();
        const size_t spaceDim= mesh->getSpaceDimension();
        retval= Matrix(numNodos,spaceDim);
        for(size_t i=0;i<numNodos;i++)
          for(size_t j=0;j<spaceDim;j++)
            retval(i,j)= coordenadas[i*spaceDim+j];
      }
    else
      std::cerr << "MEDMesh::getCoordinates(); mesh not readed." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfCellTypes()
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getNumberOfTypes(MED_EN::MED_CELL);
    else
      std::cerr << "MEDMesh::getCoordinates(); mesh not readed." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCellTypes(void)
  {
    boost::python::list retval;
    if(mesh)
      {
        const MED_EN::medGeometryElement *tipos= mesh->getTypes(MED_EN::MED_CELL);
        const size_t numTipos= mesh->getNumberOfTypes(MED_EN::MED_CELL);
        for(size_t i=0;i<numTipos;i++)
          retval.append(tipos[i]);
      }
    else
      std::cerr << "MEDMesh::getCoordinates(); mesh not readed." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCellTypeNames(void)
  {
    boost::python::list retval;
    if(mesh)
      {
        const std::string *tipos= mesh->getCellTypeNames(MED_EN::MED_CELL);
        const size_t numTipos= mesh->getNumberOfTypes(MED_EN::MED_CELL);
        for(size_t i=0;i<numTipos;i++)
          retval.append(std::string(tipos[i]));
      }
    else
      std::cerr << "MEDMesh::getCellTypeNames(); mesh not readed." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumCellsOfType(int tipo)
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getNumberOfElements(MED_EN::MED_CELL,tipo);
    else
      std::cerr << "MEDMesh::getNumCellsOfType(); mesh not readed." << std::endl;
    return retval;
  }

XC::Matrix XC::MEDMesh::getConnectivityCellsOfType(int tipo)
  {
    Matrix retval;
    size_t numNodosCeldaTipo= tipo%100;
    size_t numCeldasTipo= 0;
    if(mesh)
      {
        numCeldasTipo= mesh->getNumberOfElements(MED_EN::MED_CELL,tipo);
        retval= Matrix(numCeldasTipo,numNodosCeldaTipo);
        const int *conectividad= mesh->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL,tipo);
        for(size_t i= 0;i<numCeldasTipo;i++)
          for(size_t j= 0;j<numNodosCeldaTipo;j++)
            retval(i,j)= conectividad[i*numNodosCeldaTipo+j];
      }
    else
      std::cerr << "MEDMesh::getConnectivityCellsOfType(); mesh not readed." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfGroups(int tipo_entidad)
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getNumberOfGroups(tipo_entidad);
    else
      std::cerr << "MEDMesh::getNumberOfGroups(); mesh not readed." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfFamilies(int tipo_entidad)
  {
    size_t retval= 0;
    if(mesh)
      retval= mesh->getNumberOfFamilies(tipo_entidad);
    else
      std::cerr << "MEDMesh::getNumberOfFamilies(); mesh not readed." << std::endl;
    return retval;
  }

