# -*- coding: utf-8 -*-

import pickle
import sys


class ElementSectionMap(dict):
  mapSectionsFileName= './mapSectionsReinforcement.pkl'
  '''Map with asssings to each shell element two
     reinforced concrete section names. This map that will be readed to
     create fiber section models during reinforcement verification.'''
  def assign(self,elemSet,setRCSects):
    '''Assigns the sections setRCSEctsName+'1', setRCSEctsName+'2', ...
       to the elements of the set.'''
    propName= 'sectionName'
    for e in elemSet:
      if(not e.hasProp(propName)):
        self[e.tag]=[]
        for i in range(len(setRCSects.lstRCSects)):
          sname=setRCSects.name+str(i+1)
          self[e.tag].append(sname)
        e.setProp(propName,setRCSects.name)
      else:
        sys.stderr.write("element: "+ str(e.tag) + " has already a section ("+e.getProp(propName)+")\n")
  def dump(self):
    with open(self.mapSectionsFileName, 'wb') as f:
      pickle.dump(self, f, pickle.HIGHEST_PROTOCOL)

def loadElementSectionMap():
  with open(ElementSectionMap.mapSectionsFileName, 'r') as f:
    return pickle.load(f)
