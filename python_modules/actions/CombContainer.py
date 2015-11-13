# -*- coding: utf-8 -*-

from import_export import NeutralLoadDescription as nld

'''CombContainer.py: container for load combinations.'''

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

class CombinationRecord(object):
  '''Combination name and expression (i.e. ELS01= 1.0*G+1.0*Q)'''
  def __init__(self,name,expr):
    self.name= name
    self.expr= expr

class SituationCombs(dict):
  '''Combinations for a situation (frequent, rare, persistent,...).'''
  def __init__(self, desc):
    self.description= desc
  def add(self,name,expr):
    self[name]= CombinationRecord(name,expr)
  def getNeutralFormat(self, counter, typ, mapLoadCases):
    retval= dict()
    for key in self:
      comb= self[key]
      retval[comb.name]= nld.LoadComb(counter,comb.name,self.description,typ,nld.getComponentsFromStr(comb.expr,mapLoadCases))
      counter+=1
    return retval



class SLSCombinations(object):
  def __init__(self):
    self.name= "Service limit states."
    self.rare= SituationCombs('Rare situations.')
    self.freq= SituationCombs('Frequent situations.')
    self.qp= SituationCombs('Quasi-permanent situations.')
    self.earthquake= SituationCombs('Earthquake situations for SLS.')
  def getNeutralFormat(self, counter, mapLoadCases):
    retval= self.rare.getNeutralFormat(counter,'SLSR', mapLoadCases)
    retval.update(self.freq.getNeutralFormat(counter+len(retval),'SLSF', mapLoadCases))
    retval.update(self.qp.getNeutralFormat(counter+len(retval),'SLSQP', mapLoadCases))
    retval.update(self.earthquake.getNeutralFormat(counter+len(retval),'SLSS', mapLoadCases))
    return retval
    

class ULSCombinations(object):
  def __init__(self):
    self.name= "Ultimate limit states."
    self.perm= SituationCombs('Permanent and transitory situations.')
    self.acc= SituationCombs('Exceptional (accidental) situations.')
    self.fatigue= SituationCombs('Fatigue situations.')
    self.earthquake= SituationCombs('Earthquake situations for ULS.')
  def getNeutralFormat(self, counter, mapLoadCases):
    retval= self.perm.getNeutralFormat(counter,'ULST2', mapLoadCases)
    retval.update(self.acc.getNeutralFormat(counter+len(retval),'ULST2A', mapLoadCases))
    retval.update(self.fatigue.getNeutralFormat(counter+len(retval),'ULSF', mapLoadCases))
    retval.update(self.earthquake.getNeutralFormat(counter+len(retval),'ULSS', mapLoadCases))
    return retval

class CombContainer(object):
  def __init__(self):
    self.SLS= SLSCombinations()
    self.ULS= ULSCombinations()
  def getNeutralFormat(self, mapLoadCases):
    counter= 1
    retval= self.SLS.getNeutralFormat(counter, mapLoadCases)
    retval.update(self.ULS.getNeutralFormat(counter+len(retval), mapLoadCases))
    return retval
    
