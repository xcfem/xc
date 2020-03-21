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
//SingleDomTIter.h

#ifndef SingleDomTIter_h
#define SingleDomTIter_h
#include <utility/tagged/storage/TaggedObjectIter.h>
#include <utility/tagged/storage/TaggedObjectStorage.h>

namespace XC {

template <class TIter>
class SingleDomTIter: public  TIter
  {
  private:
    TaggedObjectIter &myIter;
  public:
    SingleDomTIter(TaggedObjectStorage *theStorage);

    inline TaggedObject *getIterPtr(void)
      { return myIter(); }
    virtual void reset(void);
  };

template <class TIter>
SingleDomTIter<TIter>::SingleDomTIter(TaggedObjectStorage *theStorage)
  :myIter(theStorage->getComponents())
  {}

template <class TIter>
void SingleDomTIter<TIter>::reset(void)
  { myIter.reset(); }

template <class T,class TIter>
T *getIterPtr(TIter &titer)
  {
    // check if we still have Nodes in the model
    // if not return 0, indicating we are done
    TaggedObject *theComponent= titer.getIterPtr();
    if(!theComponent)
      return nullptr;
    else
      return dynamic_cast<T *>(theComponent);
  }

} // end of XC namespace

#endif

