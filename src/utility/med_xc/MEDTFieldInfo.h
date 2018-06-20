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
//MEDTFieldInfo.h

#ifndef MEDTFIELDINFO_H
#define MEDTFIELDINFO_H

#include "MEDFieldInfo.h"
#include "xc_basic/src/med_xc/MEDMEM_ArrayInterface.hxx"
#include "xc_basic/src/med_xc/MEDMEM_Field.hxx"
#include "MEDGaussModel.h"
#include "preprocessor/set_mgmt/Set.h"
#include "domain/mesh/element/Element.h"

namespace XC {
class Set;
class MEDGroupInfo;

//! @ingroup MED
//
//! @brief Iformation about a field defined over a mesh subset.
template <class T>
class MEDTFieldInfo: public MEDFieldInfo
  {
  public:
    typedef typename MEDMEM::MEDMEM_ArrayInterface<T,MEDMEM::FullInterlace,MEDMEM::Gauss>::Array ArrayGauss;
    typedef typename MEDMEM::FIELD<T> fieldType;
  protected:
    mutable fieldType *field; //!< Field definition in MEDMEM.
    bool has_gauss_points;
    void free_mem(void) const;
    void alloc(void) const;
    ArrayGauss *getArrayGauss(void) const;
    static void checkVectorDimension(const std::string &,const std::vector<T> &, const size_t &);

  private:
    MEDTFieldInfo(const MEDTFieldInfo &);
    MEDTFieldInfo &operator=(const MEDTFieldInfo &);
    void setGaussModel(const MEDGaussModel &);
  public:
    MEDTFieldInfo(const FieldInfo &,MEDGroupInfo *);
    ~MEDTFieldInfo(void);
    void setValueIJ(int i, int j,const T &);
    void defineGaussModels(const Set &);
    void setValueIJK(int i, int j,int k,int t,const T &);
    void to_med(void) const;
    void write(const std::string &) const;


  };

//! @brief Constructor.
template <class T>
MEDTFieldInfo<T>::MEDTFieldInfo(const FieldInfo &fi,MEDGroupInfo *grp)
  : MEDFieldInfo(fi,grp), field(nullptr), has_gauss_points(false) {}

//! @brief Destructor.
template <class T>
MEDTFieldInfo<T>::~MEDTFieldInfo(void)
  { free_mem(); }

//! @brief Frees memory (and erases the field definition).
template <class T>
void MEDTFieldInfo<T>::free_mem(void) const
  {
    if(field)
      {
        delete field;
        field= nullptr;
      }
  }

//! @brief Creates MEDMEM field definition.
template <class T>
void MEDTFieldInfo<T>::alloc(void) const
  {
    free_mem();
    const int nc= getXCFieldInfo().getNumberOfComponents();
    if(nc>0)
      {
        MEDMEM::GROUP *grupo= getGrupoMED();
        assert(grupo);
        field= new MEDMEM::FIELD<T>(grupo,nc);
      }
    else
      std::cerr << "El field: '" << getXCFieldInfo().getName() 
                << "' tiene 0 componentes; se ignora." << std::endl;
  }

//! @brief Create the matrix to store the field values at the Gauss points.
template <class T>
typename MEDTFieldInfo<T>::ArrayGauss *MEDTFieldInfo<T>::getArrayGauss(void) const
  {
    ArrayGauss *retval= nullptr;
    const int numberOfComponents= getXCFieldInfo().getNumberOfComponents();
    const MEDMapNumCellsByType &cell_types= getGrupo().getMapCellTypes();
    const size_t numberOfTypes= cell_types.getNumberOfTypes();
    std::vector<int> numberOfElementsOfTypeC(numberOfTypes+1,0);
    std::vector<int> numberOfGaussPoint(numberOfTypes+1,1);
    numberOfElementsOfTypeC[0]= 1;
    numberOfGaussPoint[0]= 1;
    size_t conta= 1;
    for(MEDMapNumCellsByType::const_iterator i= cell_types.begin();i!=cell_types.end();i++,conta++)
      {
        const MED_EN::medGeometryElement type= i->first;
        numberOfElementsOfTypeC[conta]= i->second+numberOfElementsOfTypeC[conta-1]; //Acumulados.
        numberOfGaussPoint[conta]= field->getGaussLocalization(type).getNbGauss();
      }
    retval= new ArrayGauss(numberOfComponents,numberOfElementsOfTypeC[numberOfTypes]-1,numberOfTypes, &numberOfElementsOfTypeC[0], &numberOfGaussPoint[0]);
    return retval;
  }

//! @brief Checks vector dimension.
template <class T>
  void XC::MEDTFieldInfo<T>::checkVectorDimension(const std::string &nmb_prop,const std::vector<T> &value, const size_t &dim)
  {
     if(value.size()!=dim)
       std::cerr << "The property: '" << nmb_prop
                 << "' returns a vector with dimension: "
                 << value.size() << "; a vector of dimension "
                 << dim << " was expected" << std::endl;
  }
 
//! @brief Sets the Gauss integration model.
template <class T>
void MEDTFieldInfo<T>::setGaussModel(const MEDGaussModel &gm)
  {
    assert(field);
    const MEDGaussModel::med_gauss_model *model= gm.getGaussModelMED();
    assert(model);
    field->setGaussLocalization(gm.getType(),*model);
    has_gauss_points= true;
  }

//! @brief Sets the Gauss integration model for the elements in the set.
template <class T>
void MEDTFieldInfo<T>::defineGaussModels(const Set &set)
  {
     const DqPtrsElem &elements= set.getElements();
     std::set<std::string> nombres;
     for(DqPtrsElem::const_iterator j= elements.begin();j!=elements.end();j++)
       {
          MEDGaussModel gm= (*j)->getMEDGaussModel();
          const std::string nmb= gm.getName();
          if(nombres.find(nmb)==nombres.end()) //Nuevo
            {
              nombres.insert(nmb);
              setGaussModel(gm);
            }
       }
    if(has_gauss_points)
      field->setArray(getArrayGauss());
  }

//! @brief Value of the field at indexes i,j.
template <class T>
void MEDTFieldInfo<T>::setValueIJ(int i, int j,const T &value)
  {
    const int nc= getXCFieldInfo().getNumberOfComponents();
    if(j>nc)
      std::cerr << "j index: " << j
                << " out of range (1," << nc
                << ").\n" << std::endl;
    const int ne= getGrupo().getNumberOfElements();
    if(i>ne)
      std::cerr << "i index: " << i
                << " out of range (1," << ne
                << ").\n" << std::endl;
    assert(field);
    field->setValueIJ(i,j,value);
  }

//! @brief Value of the field at indexes i,j y k (Gauss point).
template <class T>
void MEDTFieldInfo<T>::setValueIJK(int i, int j,int k,int t,const T &value)
  {
    const int nc= getXCFieldInfo().getNumberOfComponents();
    if(j>nc)
      std::cerr << "j index: " << j
                << " out of range (1," << nc
                << ").\n" << std::endl;
    const int ne= getGrupo().getNumberOfElements();
    if(i>ne)
      std::cerr << "i index: " << i
                << " out of range (1," << ne
                << ").\n" << std::endl;
    assert(field);
    const int num_ptos_gauss= field->getNumberOfGaussPoints(t);
    if(k>num_ptos_gauss)
      std::cerr << "k index: " << k
                << " out of range (1," << num_ptos_gauss
                << ").\n" << std::endl;
    field->setValueIJK(i,j,k,value);
  }

//! @brief Dumps the field definition into MEDMEM.
template <class T>
void MEDTFieldInfo<T>::to_med(void) const
  {
    alloc();
    field->setName(getXCFieldInfo().getName());
    //Information about components.
    field->setComponentsNames(getComponentNamesPtr());
    field->setComponentsDescriptions(getComponentDescriptionsPtr());
    field->setMEDComponentsUnits(getComponentUnitsPtr());
    //Information about iteration.
    field->setIterationNumber(getXCFieldInfo().getIterationNumber());
    field->setOrderNumber(getXCFieldInfo().getOrderNumber());
    field->setTime(getXCFieldInfo().getTime());
  }

//! @brief Escribe el field en el archivo which name being passed as parameter.
template <class T>
void MEDTFieldInfo<T>::write(const std::string &filename) const
  {
    if(field)
      field->write(MEDMEM::MED_DRIVER,filename.c_str());
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< ";field not set." << std::endl;
  }

} // end of XC namespace
#endif
