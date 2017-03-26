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
//  bajo los términos de l Licencia Pública General GNU publicada 
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
//MaterialVector.h

#ifndef MaterialVector_h
#define MaterialVector_h

#include <vector>
#include "xc_utils/src/nucleo/EntCmd.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/matrix/Vector.h"


namespace XC {

//! @ingroup Mat
//
//! @brief Material pointer container. It's used by
//! elements to store materials for each integration point.
template <class MAT>
class MaterialVector: public std::vector<MAT *>, public EntCmd, public MovableObject
  {
  protected:
    void clear_materials(void);
    void clearAll(void);
    void alloc(const std::vector<MAT *> &mats);


    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);  
    int recvData(const CommParameters &);
  public:
    typedef typename std::vector<MAT *> mat_vector;
    typedef typename mat_vector::iterator iterator;
    typedef typename mat_vector::reference reference;
    typedef typename mat_vector::const_reference const_reference;

    MaterialVector(const size_t &nMat,const MAT *matModel= nullptr);
    MaterialVector(const MaterialVector<MAT> &);
    MaterialVector<MAT> &operator=(const MaterialVector<MAT> &);
    ~MaterialVector(void)
      { clearAll(); }

    void setMaterial(const MAT *nuevo_mat);
    void setMaterial(size_t i,MAT *nuevo_mat);
    void setMaterial(const MAT *nuevo_mat,const std::string &tipo);
    bool empty(void) const;
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    void setInitialGeneralizedStrains(const std::vector<Vector> &);
    void addInitialGeneralizedStrains(const std::vector<Vector> &);
    void zeroInitialGeneralizedStrains(void);

    size_t getGeneralizedStressSize(void) const;
    size_t getGeneralizedStrainSize(void) const;
    m_double getGeneralizedStresses(void) const;
    m_double getGeneralizedStrains(void) const;
    const Vector &getMeanGeneralizedStress(void) const;
    const Vector &getMeanGeneralizedStrain(void) const;
    double getMeanGeneralizedStrain(const int &defID) const;
    double getMeanGeneralizedStress(const int &defID) const;
    double getMeanGeneralizedStrainByName(const std::string &) const;
    double getMeanGeneralizedStressByName(const std::string &) const;
    m_double getGeneralizedStrain(const int &defID) const;
    m_double getGeneralizedStress(const int &defID) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

//! @brief Default constructor.
template <class MAT>
MaterialVector<MAT>::MaterialVector(const size_t &nMat,const MAT *matModel)
  : std::vector<MAT *>(nMat,nullptr), MovableObject(MAT_VECTOR_TAG)
  {
    if(matModel)
      {
        for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
          {
            (*i)= matModel->getCopy();
            if(!(*i))
              std::cerr<<" MaterialVector -- failed allocate material model pointer\n";
          }
      }
  }

//! @brief Copy materials from another vector.
template <class MAT>
void MaterialVector<MAT>::alloc(const std::vector<MAT *> &mats)
  {
    clearAll();
    const size_t nMat= mats.size();
    this->resize(nMat);
    for(size_t i= 0;i<nMat;i++)
      {
        if(mats[i])
          {
            (*this)[i]= mats[i]->getCopy();
            if(!(*this)[i])
              std::cerr<<" MaterialVector -- failed allocate material model pointer\n";
          }
      }
  }

//! @brief Copy constructor.
template <class MAT>
MaterialVector<MAT>::MaterialVector(const MaterialVector<MAT> &otro)
  : std::vector<MAT *>(otro.size(),nullptr), MovableObject(MAT_VECTOR_TAG)
  { alloc(otro); }

//! @brief Assignment operator.
template <class MAT>
MaterialVector<MAT> &MaterialVector<MAT>::operator=(const MaterialVector<MAT> &otro)
  { 
    alloc(otro);
    return *this;
  }

template <class MAT>
void MaterialVector<MAT>::setMaterial(const MAT *nuevo_mat)
  {
    clear_materials();
    if(nuevo_mat)
      {
        for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
          {
            (*i)= nuevo_mat->getCopy();
            if(!(*i))
              std::cerr<<" MaterialVector -- failed allocate material model pointer\n";
          }
      }
  }

template <class MAT>
void MaterialVector<MAT>::setMaterial(const MAT *nuevo_mat, const std::string &tipo)
  {
    clear_materials();
    if(nuevo_mat)
      {
        for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
          {
            (*i)= nuevo_mat->getCopy(tipo.c_str());
            if(!(*i))
              std::cerr<<" MaterialVector::setMaterial -- failed allocate material model pointer\n";
          }
      }
  }

template <class MAT>
void MaterialVector<MAT>::setMaterial(size_t i,MAT *nuevo_mat)
  {
    if((*this)[i])
      delete (*this)[i];
    (*this)[i]= nuevo_mat;
  }

template <class MAT>
void MaterialVector<MAT>::clear_materials(void)
  {
    for(iterator i= mat_vector::begin();i!=mat_vector::end();i++)
      {
        if(*i) delete (*i);
          (*i)= nullptr;
      }
  }

//! @brief Returns true ifno se ha asignado material.
template <class MAT>
bool MaterialVector<MAT>::empty(void) const
  {
    if(mat_vector::empty())
      return true;
    else
      return ((*this)[0]==nullptr);
  }

template <class MAT>
void MaterialVector<MAT>::clearAll(void)
  {
    clear_materials();
    std::vector<MAT *>::clear();
  }


//! @brief Commits materials state (normally after convergence).
template <class MAT>
int MaterialVector<MAT>::commitState(void)
  {
    int retVal= 0;

    for(iterator i=mat_vector::begin();i!=mat_vector::end();i++)
      retVal+= (*i)->commitState();
    return retVal;
  }

//! @brief Returns materials to its last commited state.
template <class MAT>
int MaterialVector<MAT>::revertToLastCommit(void)
  {
    int retVal= 0;

    for(iterator i=mat_vector::begin();i!=mat_vector::end();i++)
      retVal+= (*i)->revertToLastCommit() ;
    return retVal;
  }


//! @brief Return materials to its initial state.
template <class MAT>
int MaterialVector<MAT>::revertToStart(void)
  {
    int retVal = 0;

    for(iterator i=mat_vector::begin();i!=mat_vector::end();i++)
      retVal+= (*i)->revertToStart() ;
    return retVal;
  }

//! @brief Returns the tamaño del vector de tensiones.
template <class MAT>
size_t MaterialVector<MAT>::getGeneralizedStressSize(void) const
  { return (*this)[0]->getGeneralizedStress().Size(); }

//! @brief Returns the tamaño del generalized strains vector.
template <class MAT>
size_t MaterialVector<MAT>::getGeneralizedStrainSize(void) const
  { return (*this)[0]->getGeneralizedStrain().Size(); }

//! @brief Returns generalized stress values on each integration point.
template <class MAT>
m_double MaterialVector<MAT>::getGeneralizedStresses(void) const
  {
    const size_t ncol= getGeneralizedStressSize();
    const size_t nMat= this->size();
    m_double retval(nMat,ncol,0.0);
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStress();
        for(size_t j= 0;j<ncol;j++)
          retval(i,j)= s(j);
      }
    return retval;
  }

//! @brief Returns generalized strain values on each integration point.
template <class MAT>
m_double MaterialVector<MAT>::getGeneralizedStrains(void) const
  {
    const size_t ncol= getGeneralizedStrainSize();
    const size_t nMat= this->size();
    m_double retval(nMat,ncol,0.0);
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStrain();
        for(size_t j= 0;j<ncol;j++)
          retval(i,j)= s(j);
      }
    return retval;
  }

//! @brief Returns average generalized stress values on element. In a future we can enhance this by providing an extrapolation of results in gauss points as described in 28.4 <a href="http://www.colorado.edu/engineering/cas/courses.d/IFEM.d/IFEM.Ch28.d/IFEM.Ch28.pdf">IFEM chapter 28.</a> 
template <class MAT>
const Vector &MaterialVector<MAT>::getMeanGeneralizedStress(void) const
  {
    static Vector retval;
    retval= (*this)[0]->getGeneralizedStress();
    const size_t nMat= this->size();
    for(size_t i= 1;i<nMat;i++)
      retval+= (*this)[i]->getGeneralizedStress();
    retval/=nMat;
    return retval;
  }

//! @brief Returns average generalized strain values on element.In a future we can enhance this by providing an extrapolation of results in gauss points as described in 28.4 <a href="http://www.colorado.edu/engineering/cas/courses.d/IFEM.d/IFEM.Ch28.d/IFEM.Ch28.pdf">IFEM chapter 28.</a> 
template <class MAT>
const Vector &MaterialVector<MAT>::getMeanGeneralizedStrain(void) const
  {
    static Vector retval;
    retval= (*this)[0]->getGeneralizedStrain();
    const size_t nMat= this->size();
    for(size_t i= 0;i<nMat;i++)
      retval+= (*this)[i]->getGeneralizedStrain();
    retval/=nMat;
    return retval;
  }

//! @brief Returns the defID component of the average strain vector.
//! @param defID component index.
template <class MAT>
double MaterialVector<MAT>::getMeanGeneralizedStrain(const int &defID) const
  {
    double retval= 0.0;
    const Vector &e= getMeanGeneralizedStrain(); //generalized strains vector.
    const ResponseId &code= (*this)[0]->getType();
    const int order= code.Size();
    for(register int i= 0;i<order;i++)
      if(code(i) == defID)
        retval+= e(i);
    return retval;
  }

//! @brief Returns the component of the average strain vector which has the code being passed as parameter.
//! @param cod component code.
template <class MAT>
double MaterialVector<MAT>::getMeanGeneralizedStrainByName(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "n1")
      retval= this->getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2")
      retval= this->getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n2);
    else if(cod == "m1") //Flector en torno al eje 1.
      retval= this->getMeanGeneralizedStrain(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Flector en torno al eje 2.
      retval= this->getMeanGeneralizedStrain(PLATE_RESPONSE_m2);
    else if(cod == "q13")
      retval= this->getMeanGeneralizedStrain(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= this->getMeanGeneralizedStrain(PLATE_RESPONSE_q23);
    else if(cod == "m12")
      retval= this->getMeanGeneralizedStrain(PLATE_RESPONSE_m12);
    else if(cod == "n12")
      retval= this->getMeanGeneralizedStrain(MEMBRANE_RESPONSE_n12);
    else
      std::cerr << "stress code: '" << cod << " unknown." << std::endl;
    return retval;
  }

//! @brief Returns the defID component of the average generalized stress vector.
//! @param defID component index.
template <class MAT>
double MaterialVector<MAT>::getMeanGeneralizedStress(const int &defID) const
  {
    double retval= 0.0;
    const Vector &f= getMeanGeneralizedStress(); //Vector de esfuerzos.
    const ResponseId &code= (*this)[0]->getType();
    const int order= code.Size();
    for(register int i= 0;i<order;i++)
      if(code(i) == defID)
        retval+= f(i);
    return retval;
  }

//! @brief Returns the component of the average generalized stress vector which corresponds to the code being passed as parameter.
//! @param cod component code (n1,n2,n12,m1,m2,m12,q13,q23)
template <class MAT>
double MaterialVector<MAT>::getMeanGeneralizedStressByName(const std::string &cod) const
  {
    double retval= 0.0;
    if(cod == "n1") //Esfuerzo axil medio por unidad de longitud, paralelo al eje 1.
      retval= this->getMeanGeneralizedStress(MEMBRANE_RESPONSE_n1);
    else if(cod == "n2") //Esfuerzo axil medio por unidad de longitud, paralelo al eje 2.
      retval= this->getMeanGeneralizedStress(MEMBRANE_RESPONSE_n2);
    else if(cod == "n12")
      retval= this->getMeanGeneralizedStress(MEMBRANE_RESPONSE_n12);
    else if(cod == "m1") //Flector medio por unidad de longitud, en torno al eje 1.
      retval= this->getMeanGeneralizedStress(PLATE_RESPONSE_m1);
    else if(cod == "m2") //Flector medio por unidad de longitud, en torno al eje 2.
      retval= this->getMeanGeneralizedStress(PLATE_RESPONSE_m2);
    else if(cod == "m12")
      retval= this->getMeanGeneralizedStress(PLATE_RESPONSE_m12);
    else if(cod == "q13")
      retval= this->getMeanGeneralizedStress(PLATE_RESPONSE_q13);
    else if(cod == "q23")
      retval= this->getMeanGeneralizedStress(PLATE_RESPONSE_q23);
    else
      std::cerr << "stress code: '" << cod << " unknown." << std::endl;
    return retval;
  }

//! @brief Returns the defID component of generalized stress vector on each integration point.
//! @param defID component index.
template <class MAT>
m_double MaterialVector<MAT>::getGeneralizedStress(const int &defID) const
  {
    const size_t nMat= this->size();
    m_double retval(nMat,1,0.0);
    const ResponseId &code= (*this)[0]->getType();
    const int order= code.Size();
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStress();
        for(register int j= 0;j<order;j++)
          if(code(j) == defID)
            retval(i,1)+= s(i);
      }
    return retval;
  }

//! @brief Returns the defID component of generalized strain vector on each integration point.
//! @param defID component index.
template <class MAT>
m_double MaterialVector<MAT>::getGeneralizedStrain(const int &defID) const
  {
    const size_t nMat= this->size();
    m_double retval(nMat,1,0.0);
    const ResponseId &code= (*this)[0]->getType();
    const int order= code.Size();
    for(size_t i= 0;i<nMat;i++)
      {
        const Vector &s= (*this)[i]->getGeneralizedStrain();
        for(register int j= 0;j<order;j++)
          if(code(j) == defID)
            retval(i,1)+= s(i);
      }
    return retval;
  }

//! @brief Assigns initial values to materials initial strains.
template <class MAT>
void MaterialVector<MAT>::setInitialGeneralizedStrains(const std::vector<Vector> &iS)
  {
    const size_t nMat= this->size();
    const size_t sz= std::min(nMat,iS.size());
    if(iS.size()<nMat)
      std::cerr << "MaterialVector::setInitialGeneralizedStrains; received: "
                << iS.size() << " generalized strain vectors, expected: "
                << nMat << ".\n";
    for(size_t i= 0;i<sz;i++)
      (*this)[i]->setInitialGeneralizedStrain(iS[i]);
  }

//! \brief Adds to the materials initial strains the values being passed as parameters.
template <class MAT>
void MaterialVector<MAT>::addInitialGeneralizedStrains(const std::vector<Vector> &iS)
  {
    const size_t nMat= this->size();
    const size_t sz= std::min(nMat,iS.size());
    if(iS.size()<nMat)
      std::cerr << "MaterialVector::setInitialGeneralizedStrains; se recibieron: "
                << iS.size() << " generalized strain vectors, se esperaban: "
                << nMat << ".\n";
    for(size_t i= 0;i<sz;i++)
      (*this)[i]->addInitialGeneralizedStrain(iS[i]);
  }

//! \brief Initialize initial strains.
template <class MAT>
void MaterialVector<MAT>::zeroInitialGeneralizedStrains(void)
  {
    const size_t nMat= this->size();
    for(size_t i= 0;i<nMat;i++)
      (*this)[i]->zeroInitialGeneralizedStrain();
  }

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
template <class MAT>
XC::DbTagData &MaterialVector<MAT>::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Send object members through the channel being passed as parameter.
template <class MAT>
int MaterialVector<MAT>::sendData(CommParameters &cp)
  {
    int res= 0;
    if(this->empty())
      setDbTagDataPos(0,0);
    else
      {
        setDbTagDataPos(0,1);
        const size_t nMat= this->size();
        DbTagData cpMat(nMat*3);

        for(size_t i= 0;i<nMat;i++)
          res+= cp.sendBrokedPtr((*this)[i],cpMat,BrokedPtrCommMetaData(i,i+nMat,i+2*nMat));
        res+= cpMat.send(getDbTagData(),cp,CommMetaData(1));
      }
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
template <class MAT>
int MaterialVector<MAT>::recvData(const CommParameters &cp)
  {
    const int flag = getDbTagDataPos(0);
    int res= 0;
    if(flag!=0)
      {
        const size_t nMat= this->size();
        DbTagData cpMat(nMat*3);
        res+= cpMat.receive(getDbTagData(),cp,CommMetaData(1));

        for(size_t i= 0;i<nMat;i++)
          {
            const BrokedPtrCommMetaData meta(i,i+nMat,i+2*nMat);
            // Receive the material
            (*this)[i]= cp.getBrokedMaterial((*this)[i],cpMat,meta);
          }
      }
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
template <class MAT>
int MaterialVector<MAT>::sendSelf(CommParameters &cp)
  {
    inicComm(2);
    int res= sendData(cp);
    const int dataTag=getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "WARNING MaterialVector::sendSelf() - " 
                << dataTag << " failed to send ID";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
template <class MAT>
int MaterialVector<MAT>::recvSelf(const CommParameters &cp)
  {
    const int dataTag= this->getDbTag();
    inicComm(2);
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "WARNING MaterialVector::recvSelf() - "
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(cp);
    return res;
  }

} // end of XC namespace

#endif
