# -*- coding: utf-8 -*-

import pickle
import sys


class ShellElementSectionMap(dict):
  mapSectionsFileName= './mapSectionsReinforcement.pkl'
  '''Map with asssings to each shell element two
     reinforced concrete section names. This map that will be readed to
     create fiber section models during reinforcement verification.'''
  def assign(self,elemSet,sectionName):
    '''Assigns the sections sectionName+'1' and sectionName+'2'
       to the elements of the set.'''
    propName= 'sectionName'
    s2= sectionName+"2"
    s1= sectionName+"1"
    for e in elemSet:
      if(not e.hasProp(propName)):
        self[e.tag]= [s1,s2]
        e.setProp(propName,sectionName)
      else:
        sys.stderr.write("element: "+ str(e.tag) + " has already a section ("+e.getProp(propName)+")\n")
  def dump(self):
    with open(self.mapSectionsFileName, 'wb') as f:
      pickle.dump(self, f, pickle.HIGHEST_PROTOCOL)

def loadShellElementSectionMap():
  with open(ShellElementSectionMap.mapSectionsFileName, 'r') as f:
    return pickle.load(f)
