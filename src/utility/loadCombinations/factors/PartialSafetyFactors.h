// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//PartialSafetyFactors.h
//Partial safety factors for actions.


#ifndef PARTIALSAFETYFACTORS_HXX
#define PARTIALSAFETYFACTORS_HXX

#include "utility/kernel/CommandEntity.h"


namespace cmb_acc{
class Variation;
class Variations;
class ActionRValueList;

//! @ingroup CMBACC
//
//! @brief Serviceability limit states partial safety factors.
class SLSPartialSafetyFactors: public CommandEntity
  {
  protected:
    //Partial safety factors
    float partial_safety_factors_fav; //!< favourable effect.
    float partial_safety_factors_unfav; //!< unfavourable effect.

    //! @brief Lee el objeto desde archivo.

    friend class PartialSafetyFactors;
    Variation Coefs(void) const;
  public:
    //! @brief Default constructor. Supone control normal.
    SLSPartialSafetyFactors(const float &fav=0.0,const float &desfav=1.0);
    inline float getFavorable(void) const
      { return partial_safety_factors_fav; }
    inline void setFavorable(const float &f)
      { partial_safety_factors_fav= f; }
    inline float getDesfavorable(void) const
      { return partial_safety_factors_unfav; }
    inline void setDesfavorable(const float &f)
      { partial_safety_factors_unfav= f; }
    virtual void Print(std::ostream &os) const;
  };
 
inline std::ostream &operator<<(std::ostream &os, const SLSPartialSafetyFactors &g)
  {
    g.Print(os);
    return os;
  }
 
//! @brief Partial safety factors de una familia de acciones (permanentes, variables,...), en estados límite últimos .
class ULSPartialSafetyFactors: public SLSPartialSafetyFactors
  {
    //Partial safety factors
    float partial_safety_factors_fav_acc; //!< para favourable effect en situacion accidental o sísmica.
    float partial_safety_factors_unfav_acc; //!< para unfavourable effect en situacion accidental o sísmica.
  protected:
    //! @brief Lee el objeto desde archivo.

    friend class PartialSafetyFactors;
    //! @brief Return the partial safety factors correspondientes a situación persistente o transitoria.
    Variation CoefsPT(void) const;
    //! @brief Return the partial safety factors correspondientes a situación accidental o sísmica.
    Variation CoefsAcc(void) const;
  public:
    //! @brief Default constructor. Supone control normal.
    ULSPartialSafetyFactors(const float &fav=0.0,const float &desfav=1.8,const float &fav_acc=0.0,const float &desfav_acc=1.0);
    inline float getFavorableAccidental(void) const
      { return partial_safety_factors_fav_acc; }
    inline void setFavorableAccidental(const float &f)
      { partial_safety_factors_fav_acc= f; }
    inline float getDesfavorableAccidental(void) const
      { return partial_safety_factors_unfav_acc; }
    inline void setDesfavorableAccidental(const float &f)
      { partial_safety_factors_unfav_acc= f; }
    virtual void Print(std::ostream &os) const;
  };

//! @brief Partial safety factors de una familia de acciones en SLS y ULS.
class PartialSafetyFactors: public CommandEntity
  {
    ULSPartialSafetyFactors uls_partial_safety_factors; //!< Partial safety factors en estados límite últimos.
    SLSPartialSafetyFactors sls_partial_safety_factors; //!< Partial safety factors en serviceability limit states.
  protected:
    Variation CoefsEls(void) const;
  public:
    PartialSafetyFactors(const ULSPartialSafetyFactors &gf_uls=ULSPartialSafetyFactors(), const SLSPartialSafetyFactors &gf_els=SLSPartialSafetyFactors());

    //! @brief Return the partial safety factors for ultimate limit states.
    inline const ULSPartialSafetyFactors &getULSPartialSafetyFactors(void) const
      { return uls_partial_safety_factors; }
    //! @brief Return the partial safety factors for serviceability limit states.
    inline const SLSPartialSafetyFactors &getSLSPartialSafetyFactors(void) const
      { return sls_partial_safety_factors; }
    Variation getVariation(const bool &uls,const bool &sit_accidental) const;
    virtual void Print(std::ostream &os) const;
  };

inline std::ostream &operator<<(std::ostream &os, const PartialSafetyFactors &g)
  {
    g.Print(os);
    return os;
  }

} //fin namespace nmb_acc.

#endif
