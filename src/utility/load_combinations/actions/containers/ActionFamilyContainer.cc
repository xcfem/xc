//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//ActionFamilyContainer.cxx

#include "ActionFamilyContainer.h"
#include "utility/load_combinations/comb_analysis/LoadCombinationVector.h"
#include "utility/load_combinations/actions/LeadingActionInfo.h"


//! @brief Default constructor.
cmb_acc::ActionFamilyContainer::ActionFamilyContainer(void)
  : G("Permanentes"), 
    G_aster("Permanentes val. no cte."), 
    Q("Variables"),
    A("Accidentales"),
    AS("Sísmicas")
  {
    G.set_owner(this);
    G_aster.set_owner(this);
    Q.set_owner(this);
    A.set_owner(this);
    AS.set_owner(this);
  }

//!@brief Return the family corresponding to the familyName argument. 
const cmb_acc::ActionsFamily &cmb_acc::ActionFamilyContainer::getFamily(const std::string &familyName) const
  {
    ActionFamilyContainer *this_no_const= const_cast<ActionFamilyContainer *>(this);
    return this_no_const->getFamily(familyName);
  }

//!@brief Return the family corresponding to the familyName argument. 
cmb_acc::ActionsFamily &cmb_acc::ActionFamilyContainer::getFamily(const std::string &familyName)
  {
    if(familyName=="permanentes" or familyName=="permanent")
      return G;
    else if(familyName=="permanentes_nc")
      return G_aster;
    else if(familyName=="variables" or familyName=="variable")
      return Q;
    else if(familyName=="accidentales" or familyName=="accidental")
      return A;
    else if(familyName=="sismicas" or familyName=="seismic")
      return AS;
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; load family: '"
                  << familyName << "' not found."
	          << " Candidates are: "
	          << " permanentes, permanentes_nc, variables,"
	          << " accidentales and sismicas."
	          << " Return variable loads family.\n";
        return Q;
      }
    }

//! @brief Insert the action into the family identified by the string.
//! @param familyName: name of the family to which the action belongs.
//! @param acc: Action object to insert.
//! @param combination_factors_name: name of the combination factors that
//!                                  correspond to the action.
//! @param partial_safety_factors_name: name of the partial safety factors
//!                                     that correspond to the action.
cmb_acc::ActionWrapper &cmb_acc::ActionFamilyContainer::insert(const std::string &familyName,const Action &acc,const std::string &combination_factors_name,const std::string &partial_safety_factors_name)
  {
    ActionsFamily &family= getFamily(familyName);
    return family.insert(acc,combination_factors_name,partial_safety_factors_name);
  }

//! @brief Insert the group of actions being passed as parameter and sets
//! its combination and partial safety factors.
//! @param familyName: name of the family to which the action belongs.
//! @param actions: Vector of action objects to insert.
//! @param combination_factors_names: vector of names of the combination factors //!                                  that correspond to the action.
//! @param partial_safety_factors_name: name (unique) of the partial safety
//!                                     factors that correspond to the actions
//!                                     of the group. The uniqueness of the
//!                                     applicable partial safety factors is
//!                                     the essence of a group of actions.
cmb_acc::ActionWrapper &cmb_acc::ActionFamilyContainer::insertGroup(const std::string &familyName, const std::vector<Action> &actions, const std::vector<std::string> &combination_factors_names, const std::string &partial_safety_factors_name)
  {
    ActionsFamily &family= getFamily(familyName);
    return family.insertGroup(actions,combination_factors_names,partial_safety_factors_name);
  }

//! @brief Insert the group of actions being passed as parameter and sets
//! its combination and partial safety factors.
//! @param familyName: name of the family to which the action belongs.
//! @param actionTuples: list of (action, combination_factors_name) tuples.
//! @param partial_safety_factors_name: name (unique) of the partial safety
//!                                     factors that correspond to the actions
//!                                     of the group. The uniqueness of the
//!                                     applicable partial safety factors is
//!                                     the essence of a group of actions.
cmb_acc::ActionWrapper &cmb_acc::ActionFamilyContainer::insertGroupPy(const std::string &familyName, const boost::python::list &actionTuples, const std::string &partial_safety_factors_name)
  {
    ActionsFamily &family= getFamily(familyName);
    return family.insertGroupPy(actionTuples, partial_safety_factors_name);
  }

//! @brief Return el conjunto de acciones permanentes.
const cmb_acc::ActionsFamily &cmb_acc::ActionFamilyContainer::getPermanentActions(void) const
  { return G; }

//! @brief Asigna el conjunto de acciones permanentes.
void cmb_acc::ActionFamilyContainer::setPermanentActions(const ActionsFamily &g)
  { G= g; }

//! @brief Return the non-constant permanent actions.
const cmb_acc::ActionsFamily &cmb_acc::ActionFamilyContainer::getPermanentActionsNC(void) const
  { return G_aster; }

//! @brief Set the non-constant permanent actions.
void cmb_acc::ActionFamilyContainer::setPermanentActionsNC(const ActionsFamily &mfa)
  { G_aster= mfa; }

//! @brief Return el conjunto de acciones variables.
const cmb_acc::ActionsFamily &cmb_acc::ActionFamilyContainer::getVariableActions(void) const
  { return Q; }

//! @brief Return el conjunto de acciones variables.
void cmb_acc::ActionFamilyContainer::setVariableActions(const ActionsFamily &fa)
  { Q= fa; }

//! @brief Return el conjunto de acciones accidentales.
const cmb_acc::ActionsFamily &cmb_acc::ActionFamilyContainer::getAccidentalActions(void) const
  { return A; }

//! @brief Asigna el conjunto de acciones accidentales.
void cmb_acc::ActionFamilyContainer::setAccidentalActions(const ActionsFamily &fa)
  { A= fa; }

//! @brief Return el conjunto de acciones sísmicas.
const cmb_acc::ActionsFamily &cmb_acc::ActionFamilyContainer::getSeismicActions(void) const
  { return AS; }

//! @brief Asigna el conjunto de acciones sísmicas.
void cmb_acc::ActionFamilyContainer::setSeismicActions(const ActionsFamily &fa)
  { AS= fa; }

//! \fn cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetPermanentes(const bool &uls,const bool &sit_accidental) const
//! @brief Return the combinations obtained from permanent and non-constant permanent actions.
//! @param uls: True if the combinations correspond to ultimate limit states.
//! @param sit_accidental: True if the combinations correspond to accidental situations.
cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetPermanentes(const bool &uls,const bool &sit_accidental) const
  {
    LoadCombinationVector retval;
    if(!G.empty()) //Hay acciones permanentes.
      {
        retval= G.getActions().getCombinations(uls,sit_accidental,-1);//Las permanentes siempre con characteristic value.
      }
    if(!G_aster.empty()) //There are non-constant permanent actions.
      {
        const LoadCombinationVector SG_aster= G_aster.getActions().getCombinations(uls,sit_accidental,-1);//Las permanentes siempre con characteristic value.
        retval= LoadCombinationVector::ProdCartesiano(retval,SG_aster,Action::zero);
      }
    retval= get_compatibles(retval); //Filtramos las que contienen acciones incompatibles
                                                                   // o esclavas huérfanas.
    return retval;
  }

//! \fn cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetVariables(const LoadCombinationVector &permanentes,const bool &uls,const bool &sit_accidental,const short int &v) const
//! @brief Return the combinations that correspond to permanent (constant or not) and variable actions.
//! @param permanentes: LoadCombinations de las acciones permanentes obtenidas mediante cmb_acc::ActionFamilyContainer::GetPermanentes.
//! @param uls: True if the combinations correspond to ultimate limit states.
//! @param sit_accidental: True if the combinations correspond to accidental situations.
//! @param v: representative value to consider for the leading action.
cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetVariables(const LoadCombinationVector &permanentes,const bool &uls,const bool &sit_accidental,const bool &sit_sismica,const short int &v) const
  {
    LoadCombinationVector retval; //Inicializa con acciones permanentes.
    if(!Q.empty()) //Hay acciones variables.
      {
        LoadCombinationVector SQ= Q.getActions().getCombinationsWhenLeading(uls,sit_accidental, sit_sismica,v);
        retval= LoadCombinationVector::ProdCartesiano(permanentes,SQ,Action::zero);
        if(uls)
	  retval= LoadCombinationVector::Concat(permanentes,retval,Action::zero); //Si ULS consideramos también las cargas permanentes SOLAS.
      }
    else //No hay acciones variables.
      retval= permanentes;
    retval= get_compatibles(retval); //Filter the combinations that contain
                                     //incombpatible actions or slave actions
                                     //without its master.
    return retval;
  }

//! @brief Return the combinations that correspond to accidental or seismic situations.
//! @param previas: LoadCombinations de las acciones permanentes y variables obtenidas mediante cmb_acc::ActionFamilyContainer::GetVariables.
//! @param Acc: Familia de acciones accidentales o sísmicas.
cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetAccSis(const LoadCombinationVector &previas,const ActionsFamily &Acc) const
  {
    LoadCombinationVector retval(previas);
    if(!Acc.empty()) //Existen acciones accidentales o sísmicas.
      {
        const size_t na= Acc.getActions().size();
        LoadCombinationVector SA;
        for(size_t i=0;i<na;i++) //i: Índice de la acción accidental o sísmica.
          {
	    LeadingActionInfo lci(i,-1,0); //Leading action with characteristic value (-1)
                                           //rest of actions with combination value 0 (WHICH MUST BE ZERO).
            LoadCombinationVector temp= Acc.getActions().getCombinations(true,true,lci);
            SA= LoadCombinationVector::Concat(SA,temp,Action::zero);
          }
        retval= LoadCombinationVector::ProdCartesiano(retval,SA,Action::zero);
      }
    retval= get_compatibles(retval); //Filtramos las que contienen acciones incompatibles.
    retval= must_contain(retval, Acc); // Remove those that don't contain any accidental action.
    return retval;
  }



//! \fn cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetPersistentesOTransit(void) const
//! @brief Return the combinations that correspond to ultimate limit states in persisten or transient situations.
cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetPersistentesOTransit(void) const
  {
    if(verbosity>1) std::clog << "Computing combinations for ULS in persistent or transient situations..." << std::endl; 
    if(verbosity>1) std::clog << "  Computing combinatios of permanent actions...";
    LoadCombinationVector retval= GetPermanentes(true,false); //ULS, situación no accidental.
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "  Computing combinations of variable actions...";
    retval= GetVariables(retval,true,false,false,0);
    if(verbosity>1) std::clog << "  Filtering combinations of incompatible actions...";
    retval= filtraCombsEsclavasHuerfanas(get_compatibles(retval)); //Filter incompatible actions.
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "done." << std::endl;
    return retval;
  }

//! \fn cmb_acc::ActionFamilyContainer::GetAccidentales(void) const
//! @brief Return the combinations that correspond to ultimate limit states in accidental situations.
cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetAccidentales(void) const
  {
    LoadCombinationVector retval;
    if(A.empty()) return retval; //No hay acciones accidentales.
    if(verbosity>1) std::clog << "Computing combinations for ULS in accidental situations..." << std::endl;
    if(verbosity>1) std::clog << "  Computing combinations of permanent actions...";
    retval= GetPermanentes(true,true); //ULS, accidental situations.
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "  Computing combinations of variable actions...";
    retval= GetVariables(retval,true,true,false,2); //ULS, leading action with characteristic value
                                              //rest of actions with cuasi-permanent value.
    if(verbosity>1) std::clog << "  done." << std::endl;
    if(verbosity>1) std::clog << "  Computing combinations of accidental actions...";
    retval= GetAccSis(retval,A);
    if(verbosity>1) std::clog << "  Filtering combinations of incompatible actions...";
    retval= filtraCombsEsclavasHuerfanas(get_compatibles(retval)); //Filter incompatible actions.
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "done." << std::endl;
    return retval;
  }


//! \fn cmb_acc::ActionFamilyContainer::GetSismicas(void) const
//! @brief Return the combinations that correspond to ultimate limit states in seismic situations.
cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetSismicas(void) const
  {
    LoadCombinationVector retval;
    if(AS.empty()) return retval; //No hay acciones sismicas.
    if(verbosity>1) std::clog << "Computing combinations for ULS in seismic situations..." << std::endl;
    if(verbosity>1) std::clog << "  Computing combinations of permanent actions...";
    retval= GetPermanentes(true,true); //ULS, situación sísmica.
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "  Computing combinations of variable actions...";
    retval= GetVariables(retval,true,true,true,2); //ULS, ll actions with quasi-permanent value.
    if(verbosity>1) std::clog << "  done." << std::endl;
    if(verbosity>1) std::clog << "  Computing combinations of seismic actions...";
    retval= GetAccSis(retval,AS);
    if(verbosity>1) std::clog << "  Filtering combinations of incompatible actions...";
    retval= filtraCombsEsclavasHuerfanas(get_compatibles(retval)); //Filter incompatible actions.
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "done." << std::endl;
    return retval;
  }

//! \fn cmb_acc::ActionFamilyContainer::GetCombULS(void) const
//! @brief Return the combinations that correspond to all ultimate limit states.
cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetCombULS(void) const
  {
    LoadCombinationVector retval= GetPersistentesOTransit(); //Situaciones persistentes o transitorias.
    LoadCombinationVector accidentales= GetAccidentales();
    retval= LoadCombinationVector::Concat(retval,accidentales,0.0);
    LoadCombinationVector sismicas= GetSismicas();
    retval= LoadCombinationVector::Concat(retval,sismicas,0.0);
    retval= filtraCombsEsclavasHuerfanas(get_compatibles(retval)); //Filter incompatible actions.
    if(verbosity>1) std::clog << "  Calculadas " << retval.size() << " ULS combinations." << std::endl;
    retval.Numera("H-ULS-");
    return retval;
  }

//serviceability limit states.


//! \fn cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetPocoFrecuentes(void) const
//! @brief Return the combinations that correspond to rare situations.
cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetPocoFrecuentes(void) const
  {
    if(verbosity>1) std::clog << "Computing combinations for SLS in rare situations..." << std::endl; 
    if(verbosity>1) std::clog << "  Computing combinations of permanent actions...";
    LoadCombinationVector retval= GetPermanentes(false,false); //SLS, situación no accidental (obviamente).
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "  Computing combinations of variable actions...";
    retval= GetVariables(retval,false,false,false,0);//SLS, leading action with characteristic value
                                              //rest of actions with combination value.
    if(verbosity>1) std::clog << "  Filtering combinations of incompatible actions...";
    retval= filtraCombsEsclavasHuerfanas(get_compatibles(retval)); //Filter incompatible actions.
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "done." << std::endl;
    return retval;
  }

//! \fn cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetFrecuentes(void) const
//! @brief Return the combinations that correspond to frequent situations.
cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetFrecuentes(void) const
  {
    if(verbosity>1) std::clog << "Computing combinations for SLS in frequent situations..." << std::endl; 
    if(verbosity>1) std::clog << "  Computing combinations of permanent actions...";
    LoadCombinationVector retval= GetPermanentes(false,false); //SLS, situación no accidental (obviamente).
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "  Computing combinations of variable actions...";
    retval= GetVariables(retval,false,false,false,1);//SLS, leading action with characteristic value
                                              //rest of actions with quasi-permanent value.
    if(verbosity>1) std::clog << "  Filtering combinations of incompatible actions...";
    retval= filtraCombsEsclavasHuerfanas(get_compatibles(retval)); //Filter incompatible actions.
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "done." << std::endl;
    return retval;
  }

//! \fn cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetCuasiPermanentes(void) const
//! @brief Return the combinations that correspond to quasi-permanent situations.
cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetCuasiPermanentes(void) const
  {
    if(verbosity>1) std::clog << "Computing combinations for SLS in quasi-permanent situations..." << std::endl; 
    if(verbosity>1) std::clog << "  Computing combinations of permanent actions...";
    LoadCombinationVector retval= GetPermanentes(false,false); //SLS, situación no accidental.
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "  Computing combinations of variable actions...";
    // We use sit_sismica= true to avoid leading loads.
    retval= GetVariables(retval,false,false,true,2);//SLS, all actions with quasi-permanent value.
    if(verbosity>1) std::clog << "  Filtering combinations of incompatible actions...";
    retval= filtraCombsEsclavasHuerfanas(get_compatibles(retval)); //Filter incompatible actions.
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "done." << std::endl;
    if(verbosity>1) std::clog << "done." << std::endl;
    return retval;
  }

//! \fn cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetCombSLS(void) const
//! @brief Return the combinations that correspond to all serviceability limit states.
cmb_acc::LoadCombinationVector cmb_acc::ActionFamilyContainer::GetCombSLS(void) const
  {
    LoadCombinationVector retval= GetPocoFrecuentes(); //LoadCombinations poco frecuentes.
    LoadCombinationVector frecuente= GetFrecuentes();
    retval= LoadCombinationVector::Concat(retval,frecuente,0.0);
    LoadCombinationVector cp= GetCuasiPermanentes();
    retval= LoadCombinationVector::Concat(retval,cp,0.0);
    retval= filtraCombsEsclavasHuerfanas(get_compatibles(retval)); //Filter incompatible actions.
    if(verbosity>1) std::clog << "  Calculadas " << retval.size() << " SLS combinations." << std::endl;
    retval.Numera("H-SLS-");
    return retval;
  }
