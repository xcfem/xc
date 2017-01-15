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
    if(malla)
      {
        delete malla;
        malla= nullptr;
      }
  }

//! @brief Reserva memoria para la malla MED.
void XC::MEDMesh::alloc(const std::string &fName) const
  {
    libera();
    malla= new MEDMEM::MESH(MEDMEM::MED_DRIVER,fName,meshName);
    if(!malla)
      std::cerr << "MEDMesh::alloc; problema al leer la malla: "
                << meshName << " en el archivo: " << fName << std::endl;
  }

void XC::MEDMesh::alloc(const MEDMesh &otro) const
  {
    libera();
    if(otro.malla)
      malla= new MEDMEM::MESH(*otro.malla);
  }

//! Constructor.
XC::MEDMesh::MEDMesh(void)
  : malla(nullptr) {}

//! Constructor.
XC::MEDMesh::MEDMesh(const MEDMesh &otro)
  : MEDObject(otro), malla(nullptr)
  {
    meshName= otro.meshName;
    alloc(otro);
  }

//! Operador asignación.
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
    const std::string nmb= malla->getName();
    if(nmb!=meshName)
      {
        std::cerr << "Error al leer el nombre de la malla: se preguntó por #"
                  << meshName <<"# y se obtuvo #"
                  << nmb <<"#" << std::endl ;
      }
  }

size_t XC::MEDMesh::getSpaceDimension(void)
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getSpaceDimension();
    else
      std::cerr << "MEDMesh::getSpaceDimension(); no se ha leído la malla." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getMeshDimension(void)
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getMeshDimension();
    else
      std::cerr << "MEDMesh::getMeshDimension(); no se ha leído la malla." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfNodes(void)
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getNumberOfNodes();
    else
      std::cerr << "MEDMesh::getNumberOfNodes(); no se ha leído la malla." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCoordinatesNames(void)
  {
    boost::python::list retval;
    if(malla)
      {
        const std::string *coordinatesNames= malla->getCoordinatesNames();
        const size_t spaceDim= malla->getSpaceDimension();
        for(size_t i=0;i<spaceDim;i++)
          retval.append(std::string(coordinatesNames[i]));
      }
    else
      std::cerr << "MEDMesh::getCoordinatesNames(); no se ha leído la malla." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCoordinatesUnits(void)
  {
    boost::python::list retval;
    if(malla)
      {
        const std::string *unitsNames= malla->getCoordinatesUnits();
        const size_t spaceDim= malla->getSpaceDimension();
        for(size_t i=0;i<spaceDim;i++)
          retval.append(std::string(unitsNames[i]));
      }
    else
      std::cerr << "MEDMesh::getCoordinatesUnits(); no se ha leído la malla." << std::endl;
    return retval;
  }

XC::Matrix XC::MEDMesh::getCoordinates(void)
  {
    Matrix retval;
    if(malla)
      {
        const double *coordenadas= malla->getCoordinates(::MED_FULL_INTERLACE);
        const size_t numNodos= malla->getNumberOfNodes();
        const size_t spaceDim= malla->getSpaceDimension();
        retval= Matrix(numNodos,spaceDim);
        for(size_t i=0;i<numNodos;i++)
          for(size_t j=0;j<spaceDim;j++)
            retval(i,j)= coordenadas[i*spaceDim+j];
      }
    else
      std::cerr << "MEDMesh::getCoordinates(); no se ha leído la malla." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfCellTypes()
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getNumberOfTypes(MED_EN::MED_CELL);
    else
      std::cerr << "MEDMesh::getCoordinates(); no se ha leído la malla." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCellTypes(void)
  {
    boost::python::list retval;
    if(malla)
      {
        const MED_EN::medGeometryElement *tipos= malla->getTypes(MED_EN::MED_CELL);
        const size_t numTipos= malla->getNumberOfTypes(MED_EN::MED_CELL);
        for(size_t i=0;i<numTipos;i++)
          retval.append(tipos[i]);
      }
    else
      std::cerr << "MEDMesh::getCoordinates(); no se ha leído la malla." << std::endl;
    return retval;
  }

boost::python::list XC::MEDMesh::getCellTypeNames(void)
  {
    boost::python::list retval;
    if(malla)
      {
        const std::string *tipos= malla->getCellTypeNames(MED_EN::MED_CELL);
        const size_t numTipos= malla->getNumberOfTypes(MED_EN::MED_CELL);
        for(size_t i=0;i<numTipos;i++)
          retval.append(std::string(tipos[i]));
      }
    else
      std::cerr << "MEDMesh::getCellTypeNames(); no se ha leído la malla." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumCellsOfType(int tipo)
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getNumberOfElements(MED_EN::MED_CELL,tipo);
    else
      std::cerr << "MEDMesh::getNumCellsOfType(); no se ha leído la malla." << std::endl;
    return retval;
  }

XC::Matrix XC::MEDMesh::getConnectivityCellsOfType(int tipo)
  {
    Matrix retval;
    size_t numNodosCeldaTipo= tipo%100;
    size_t numCeldasTipo= 0;
    if(malla)
      {
        numCeldasTipo= malla->getNumberOfElements(MED_EN::MED_CELL,tipo);
        retval= Matrix(numCeldasTipo,numNodosCeldaTipo);
        const int *conectividad= malla->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL,tipo);
        for(size_t i= 0;i<numCeldasTipo;i++)
          for(size_t j= 0;j<numNodosCeldaTipo;j++)
            retval(i,j)= conectividad[i*numNodosCeldaTipo+j];
      }
    else
      std::cerr << "MEDMesh::getConnectivityCellsOfType(); no se ha leído la malla." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfGroups(int tipo_entidad)
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getNumberOfGroups(tipo_entidad);
    else
      std::cerr << "MEDMesh::getNumberOfGroups(); no se ha leído la malla." << std::endl;
    return retval;
  }

size_t XC::MEDMesh::getNumberOfFamilies(int tipo_entidad)
  {
    size_t retval= 0;
    if(malla)
      retval= malla->getNumberOfFamilies(tipo_entidad);
    else
      std::cerr << "MEDMesh::getNumberOfFamilies(); no se ha leído la malla." << std::endl;
    return retval;
  }

