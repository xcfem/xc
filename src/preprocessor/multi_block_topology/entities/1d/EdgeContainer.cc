//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
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
//EdgeContainer.cpp

#include "EdgeContainer.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include <set>

//! @brief Constructor.
XC::EdgeContainer::EdgeContainer(void)
  : CommandEntity() {}

//! @brief returns the loop that starts in vertex passed as parameter.
std::deque<const XC::Edge *> XC::EdgeContainer::getLoop(const Edge *first) const
  {
    const Pnt *firstPnt= first->P1();
    const Pnt *lastPnt= first->P2();
    std::set<const Edge *> visited;
    visited.insert(first);
    std::deque<const Edge *> retval;
    retval.push_back(first);
    const size_t maxSizeLoop= 2*size()-1; //Max. loop size.
    size_t count= 1;
    do
      {
        const Edge *next= lastPnt->next(*this,visited);
        if(next)
          {
            retval.push_back(next);
            visited.insert(next);
            first= next;
            lastPnt= first->P2();
          }
        else
          {
	    std::cerr << "EdgeContainer::getLoop error; next edge not found." 
                      << std::endl;
            break;
          }
        count++;
        if(count>maxSizeLoop) //Something goes wrong.
          {
	    std::cerr << "EdgeContainer::getLoop error; error in loop finding." 
                      << std::endl;
            break;
          }
      }
    while(firstPnt!=lastPnt);
    return retval;
  }

XC::EdgeContainer::point_sequence getPointSequenceFromLoop(const std::deque<const XC::Edge *> &loop)
  {
    XC::EdgeContainer::point_sequence retval;
    const XC::Pnt *pt(nullptr);
    for(std::deque<const XC::Edge *>::const_iterator i= loop.begin();i!=loop.end();i++)
      {
        pt= (*i)->P1();
        retval.push_back(pt);
      }
    return retval;
  } 


//! @brief returns closed contours from de edge set.
std::deque<XC::EdgeContainer::point_sequence> XC::EdgeContainer::getContoursPointSequences(void) const
  {
    std::deque<XC::EdgeContainer::point_sequence> retval;
    if(!this->empty())
      retval= XC::getContoursPointSequences(*this);
    return retval;
  }

//! @brief returns closed contours from de edge set.
std::deque<XC::EdgeContainer::point_sequence> XC::getContoursPointSequences(EdgeContainer edges)
  {
    const size_t max_iter= 100;
    size_t iter= 0;
    std::deque<XC::EdgeContainer::point_sequence> retval;
    do
      {
	EdgeContainer::const_iterator i= edges.begin();
        const Edge *first= *i;
        std::deque<const Edge *> loop= edges.getLoop(first);
        retval.push_back(getPointSequenceFromLoop(loop));
	edges= edges.getEdgesNotInLoop(loop);
	iter++;
	if(iter>max_iter)
	  {
	    std::cerr << "getContours error; " << edges.size()
	              << " left, after " << iter << " iterations." << std::endl;
	    break;
	  }
      }
    while(!edges.empty());
    return retval;
  }

//! @brief Returns the edges that result from removing the edges from the loop passed as parameter.
XC::EdgeContainer XC::EdgeContainer::getEdgesNotInLoop(const std::deque<const Edge *> &loop) const
  {
    EdgeContainer retval;
    for(EdgeContainer::const_iterator i= begin();i!=end();i++)
      {
	const Edge *edgePtr= *i;
        std::deque<const Edge *>::const_iterator j= find(loop.begin(),loop.end(),edgePtr);
	if(j==loop.end()) //Not found.
	  retval.push_back(edgePtr);
      }
    return retval;
  }

//! @brief Prints edges information.
void XC::EdgeContainer::print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      os << *i << ' '; 
  }
