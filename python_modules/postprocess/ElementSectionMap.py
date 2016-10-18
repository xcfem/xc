# -*- coding: utf-8 -*-

import pickle
import sys


class ShellElementSectionMap(dict):
  mapSectionsFileName= './mapSectionsReinforcement.pkl'
  '''Map with asssings to each shell element two
     reinforced concrete section names. This map that will be readed to
     create fiber section models during reinforcement verification.'''
  def assign(self,elemSet,setRCSEctsName):
    '''Assigns the sections setRCSEctsName+'1' and setRCSEctsName+'2'
       to the elements of the set.'''
    propName= 'sectionName'
    s2= setRCSEctsName+"2" #Section name for directions 1 and 2.
    s1= setRCSEctsName+"1"
    for e in elemSet:
      if(not e.hasProp(propName)):
        self[e.tag]= [s1,s2]
        e.setProp(propName,setRCSEctsName)
      else:
        sys.stderr.write("element: "+ str(e.tag) + " has already a section ("+e.getProp(propName)+")\n")
  def dump(self):
    with open(self.mapSectionsFileName, 'wb') as f:
      pickle.dump(self, f, pickle.HIGHEST_PROTOCOL)

def loadShellElementSectionMap():
  with open(ShellElementSectionMap.mapSectionsFileName, 'r') as f:
    return pickle.load(f)
