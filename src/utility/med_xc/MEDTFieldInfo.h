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
    typedef typename MEDMEM::FIELD<T> TipoCampo;
  protected:
    mutable TipoCampo *campo; //!< Definición del campo en MEDMEM.
    bool has_gauss_points;
    void libera(void) const;
    void alloc(void) const;
    ArrayGauss *getArrayGauss(void) const;

  private:
    MEDTFieldInfo(const MEDTFieldInfo &);
    MEDTFieldInfo &operator=(const MEDTFieldInfo &);
    void setGaussModel(const MEDGaussModel &);
  public:
    MEDTFieldInfo(const FieldInfo &,MEDGroupInfo *);
    ~MEDTFieldInfo(void);
    void setValueIJ(int i, int j,const T &valor);
    void defineGaussModels(const Set &);
    void setValueIJK(int i, int j,int k,int t,const T &valor);
    void to_med(void) const;
    void write(const std::string &) const;


  };

//! @brief Constructor.
template <class T>
MEDTFieldInfo<T>::MEDTFieldInfo(const FieldInfo &fi,MEDGroupInfo *grp)
  : MEDFieldInfo(fi,grp), campo(NULL), has_gauss_points(false) {}

//! @brief Destructor.
template <class T>
MEDTFieldInfo<T>::~MEDTFieldInfo(void)
  { libera(); }

//! @brief Borra la definición del campo en MEDMEM;
template <class T>
void MEDTFieldInfo<T>::libera(void) const
  {
    if(campo)
      {
        delete campo;
        campo= NULL;
      }
  }

//! @brief Crea la definición del campo en MEDMEM;
template <class T>
void MEDTFieldInfo<T>::alloc(void) const
  {
    libera();
    const int nc= getXCFieldInfo().getNumberOfComponents();
    if(nc>0)
      {
        MEDMEM::GROUP *grupo= getGrupoMED();
        assert(grupo);
        campo= new MEDMEM::FIELD<T>(grupo,nc);
      }
    else
      std::cerr << "El campo: '" << getXCFieldInfo().GetNombre() 
                << "' tiene 0 componentes; se ignora." << std::endl;
  }

//! @brief Crea la matriz para los valores del campo en los puntos de Gauss.
template <class T>
typename MEDTFieldInfo<T>::ArrayGauss *MEDTFieldInfo<T>::getArrayGauss(void) const
  {
    ArrayGauss *retval= NULL;
    const int numberOfComponents= getXCFieldInfo().getNumberOfComponents();
    const MEDMapNumCeldasPorTipo &cell_types= getGrupo().getMapCellTypes();
    const size_t numberOfTypes= cell_types.getNumberOfTypes();
    std::vector<int> numberOfElementsOfTypeC(numberOfTypes+1,0);
    std::vector<int> numberOfGaussPoint(numberOfTypes+1,1);
    numberOfElementsOfTypeC[0]= 1;
    numberOfGaussPoint[0]= 1;
    size_t conta= 1;
    for(MEDMapNumCeldasPorTipo::const_iterator i= cell_types.begin();i!=cell_types.end();i++,conta++)
      {
        const MED_EN::medGeometryElement tipo= i->first;
        numberOfElementsOfTypeC[conta]= i->second+numberOfElementsOfTypeC[conta-1]; //Acumulados.
        numberOfGaussPoint[conta]= campo->getGaussLocalization(tipo).getNbGauss();
      }
    retval= new ArrayGauss(numberOfComponents,numberOfElementsOfTypeC[numberOfTypes]-1,numberOfTypes, &numberOfElementsOfTypeC[0], &numberOfGaussPoint[0]);
    return retval;
  }

//! @brief Sets the Gauss integration model.
template <class T>
void MEDTFieldInfo<T>::setGaussModel(const MEDGaussModel &gm)
  {
    assert(campo);
    const MEDGaussModel::med_gauss_model *model= gm.getGaussModelMED();
    assert(model);
    campo->setGaussLocalization(gm.getTipo(),*model);
    has_gauss_points= true;
  }

//! @brief Sets the Gauss integration model for the elements in the set.
template <class T>
void MEDTFieldInfo<T>::defineGaussModels(const Set &set)
  {
     const DqPtrsElem &elementos= set.GetElementos();
     std::set<std::string> nombres;
     for(DqPtrsElem::const_iterator j= elementos.begin();j!=elementos.end();j++)
       {
          MEDGaussModel gm= (*j)->getMEDGaussModel();
          const std::string nmb= gm.getNombre();
          if(nombres.find(nmb)==nombres.end()) //Nuevo
            {
              nombres.insert(nmb);
              setGaussModel(gm);
            }
       }
    if(has_gauss_points)
      campo->setArray(getArrayGauss());
  }

//! @brief Valor del campo de índices i,j.
template <class T>
void MEDTFieldInfo<T>::setValueIJ(int i, int j,const T &valor)
  {
    const int nc= getXCFieldInfo().getNumberOfComponents();
    if(j>nc)
      std::cerr << "índice j= " << j
                << " fuera de rango (1," << nc
                << ").\n" << std::endl;
    const int ne= getGrupo().getNumeroElementos();
    if(i>ne)
      std::cerr << "índice i= " << i
                << " fuera de rango (1," << ne
                << ").\n" << std::endl;
    assert(campo);
    campo->setValueIJ(i,j,valor);
  }

//! @brief Valor del campo de índices i,j y k (punto de Gauss).
template <class T>
void MEDTFieldInfo<T>::setValueIJK(int i, int j,int k,int t,const T &valor)
  {
    const int nc= getXCFieldInfo().getNumberOfComponents();
    if(j>nc)
      std::cerr << "índice j= " << j
                << " fuera de rango (1," << nc
                << ").\n" << std::endl;
    const int ne= getGrupo().getNumeroElementos();
    if(i>ne)
      std::cerr << "índice i= " << i
                << " fuera de rango (1," << ne
                << ").\n" << std::endl;
    assert(campo);
    const int num_ptos_gauss= campo->getNumberOfGaussPoints(t);
    if(k>num_ptos_gauss)
      std::cerr << "índice k= " << k
                << " fuera de rango (1," << num_ptos_gauss
                << ").\n" << std::endl;
    campo->setValueIJK(i,j,k,valor);
  }

//! @brief Vuelca la definición del campo en MEDMEM
template <class T>
void MEDTFieldInfo<T>::to_med(void) const
  {
    alloc();
    campo->setName(getXCFieldInfo().GetNombre());
    //Información sobre las componentes.
    campo->setComponentsNames(getComponentNamesPtr());
    campo->setComponentsDescriptions(getComponentDescriptionsPtr());
    campo->setMEDComponentsUnits(getComponentUnitsPtr());
    //Información sobre la iteración.
    campo->setIterationNumber(getXCFieldInfo().getIterationNumber());
    campo->setOrderNumber(getXCFieldInfo().getOrderNumber());
    campo->setTime(getXCFieldInfo().getTime());
  }

//! @brief Escribe el campo en el archivo cuyo nombre being passed as parameter.
template <class T>
void MEDTFieldInfo<T>::write(const std::string &filename) const
  {
    if(campo)
      campo->write(MEDMEM::MED_DRIVER,filename.c_str());
    else
      std::cerr << "the pointer al campo es nulo." << std::endl;
  }

} // end of XC namespace
#endif
