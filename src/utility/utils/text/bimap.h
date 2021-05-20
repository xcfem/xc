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
//bimap.h

#ifndef BIMAP_H
#define BIMAP_H

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <string>

/* tags for accessing both sides of a bidirectional map */

struct from{};
struct to{};

/* The class template bidirectional_map wraps the specification
 * of a bidirectional map based on multi_index_container.
 */

template<typename FromType,typename ToType>
struct bidirectional_map
  {
    typedef std::pair<FromType,ToType> value_type;


    /* A bidirectional map can be simulated as a multi_index_container
     * of pairs of (FromType,ToType) with two unique indices, one
     * for each member of the pair.
     */

    typedef boost::multi_index_container< value_type,
      boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique<
        boost::multi_index::tag<from>,boost::multi_index::member<value_type,FromType,&value_type::first> >,
      boost::multi_index::ordered_unique<
        boost::multi_index::tag<to>,  boost::multi_index::member<value_type,ToType,&value_type::second> >
    >
    > type;
  };

#endif


