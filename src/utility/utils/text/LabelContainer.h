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
//LabelContainer.h

#ifndef LABELCONTAINER_H
#define LABELCONTAINER_H

#include <string>
#include <set>
#include "bimap.h"
#include <iostream>

//! @brief Label dictionary
class LabelDictionary
  {
    friend class LabelContainer;
  private:
    typedef bidirectional_map<int,std::string>::type bm_type;
    bm_type bm;

    int get_indice_etiqueta(const std::string &e) const;
    const std::string &get_etiqueta_indice(const int &i) const;
    int insert(const std::string &e);
  public:
    size_t getNumLabels(void) const;

    const std::string &operator()(const int &) const;
    int operator()(const std::string &) const;
  
    void Print(std::ostream &os) const;
  };

std::ostream &operator<<(std::ostream &os,const LabelDictionary &lc);

//! @brief Label container.
class LabelContainer
  {
    static LabelDictionary dic;
    std::set<int> labels;
    bool hasLabel(const int &i) const;

  public:
    LabelContainer(void);
    LabelContainer &operator+=(const LabelContainer &);
    LabelContainer &operator-=(const LabelContainer &);
    LabelContainer &operator*=(const LabelContainer &);
    bool operator==(const LabelContainer &) const;

    void extend(const LabelContainer &);

    const std::set<int> &getIdsLabels(void) const;

    static const LabelDictionary &getDictionary(void);
    size_t getNumLabels(void) const;
    int addLabel(const std::string &e);
    int removeLabel(const std::string &e);
    bool hasLabel(const std::string &e) const;
    void clear(void);

    void Print(std::ostream &os) const;
    friend LabelContainer operator+(const LabelContainer &,const LabelContainer &);
    friend LabelContainer operator-(const LabelContainer &,const LabelContainer &);
    friend LabelContainer operator*(const LabelContainer &,const LabelContainer &);

  };

std::ostream &operator<<(std::ostream &os,const LabelContainer &lc);
LabelContainer operator+(const LabelContainer &,const LabelContainer &);
LabelContainer operator-(const LabelContainer &,const LabelContainer &);
LabelContainer operator*(const LabelContainer &,const LabelContainer &);

#endif
