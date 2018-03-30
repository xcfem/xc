# -*- coding: utf-8 -*-

'''combinations.py: manages the container for load combinations.'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO) "
__copyright__= "Copyright 2015,  LCPT A_OO "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

from import_export import NeutralLoadDescription as nld
from postprocess.reports import graphical_reports


class CombinationRecord(object):
  '''Combination name and expression (i.e. ELS01= 1.0*G+1.0*Q)'''
  def __init__(self,name,expr):
    self.name= name
    self.expr= expr
  def createCombination(self,xcCombHandler):
    '''Create combination and insert it into the XC combination handler.'''
    xcCombHandler.newLoadCombination(self.name,self.expr)
  def getRecordLoadCaseDisp(self,setsToDispLoads,setsToDispDspRot,setsToDispIntForc):
    '''Return a suitable RecordLoadCaseDisp for the combination.

    :ivar setsToDispLoads: ordered list of sets of elements to display loads.
    :ivar setsToDispDspRot: ordered list of sets of elements to display 
                            displacements. 
    :ivar setsToDispIntForc: ordered list of sets of elements to display 
                             internal forces.
    '''
    return graphical_reports.RecordLoadCaseDisp(self.name,self.expr,self.expr,setsToDispLoads,setsToDispDspRot,setsToDispIntForc)

class SituationCombs(dict):
  '''Combinations for a situation (frequent, rare, persistent,...).'''
  def __init__(self, desc):
    self.description= desc
  def add(self,name,expr):
    self[name]= CombinationRecord(name,expr)
  def getNames(self):
    '''returns a list of the combination names.'''
    return self.keys()
  def getNeutralFormat(self, counter, typ, mapLoadCases):
    retval= dict()
    for key in self:
      comb= self[key]
      retval[comb.name]= nld.LoadComb(counter,comb.name,self.description,typ,nld.getComponentsFromStr(comb.expr,mapLoadCases))
      counter+=1
    return retval
  def dumpCombinations(self,xcCombHandler):
    '''Introduces the combinations into the XC combination handler.'''
    for key in self:
      self[key].createCombination(xcCombHandler)
  def getRecordLoadCaseDisp(self,combName,setsToDispLoads,setsToDispDspRot,setsToDispIntForc):
    '''Returns a suitable RecordLoadCaseDisp for the combination.

    :ivar setsToDispLoads: ordered list of sets of elements to display loads.
    :ivar setsToDispDspRot: ordered list of sets of elements to display 
                            displacements. 
    :ivar setsToDispIntForc: ordered list of sets of elements to display 
                             internal forces.
    '''
    comb= self[combName]
    return comb.getRecordLoadCaseDisp(setsToDispLoads,setsToDispDspRot,setsToDispIntForc)

class SituationsSet(object):
  '''Set of situations as used in limit states
  
  :ivar name:        name to identify the situation set
  :ivar situations:  set of situations
  '''
  def __init__(self,name):
    self.name= name
    self.situations= None
  def getNames(self):
    '''returns a list of the combination names.'''
    retval= list()
    for s in self.situations:
      retval.extend(s.getNames())
    return retval
  def dumpCombinations(self,xcCombHandler):
    '''Introduces the combinations into the XC combination handler.'''
    for s in self.situations:
      s.dumpCombinations(xcCombHandler)
  def getRecordLoadCaseDisp(self,combName,setsToDispLoads,setsToDispDspRot,setsToDispIntForc):
    '''Returns a suitable RecordLoadCaseDisp for the combination.

    :ivar setsToDispLoads: ordered list of sets of elements to display loads.
    :ivar setsToDispDspRot: ordered list of sets of elements to display 
                            displacements. 
    :ivar setsToDispIntForc: ordered list of sets of elements to display 
                             internal forces.
    '''
    for s in self.situations:
      if combName in s:
        comb= s[combName]
        return comb.getRecordLoadCaseDisp(setsToDispLoads,setsToDispDspRot,setsToDispIntForc)
  

class SLSCombinations(SituationsSet):
  '''Combinations of actions for serviceability limit states
  
  :ivar name:        name to identify the limit state (serviceability, ultimate,...)
  :ivar rare:        combination for a rare design situation
  :ivar freq:        combination for a frequent design situation
  :ivar qp:          combination for a quasi-permanent design situation
  :ivar earthquake:  combination for a earthquake design situation
  '''
  def __init__(self):
    super(SLSCombinations,self).__init__("Serviceability limit states.")
    self.rare= SituationCombs('Rare situations.')
    self.freq= SituationCombs('Frequent situations.')
    self.qp= SituationCombs('Quasi-permanent situations.')
    self.earthquake= SituationCombs('Earthquake situations for SLS.')
    self.situations= [self.rare,self.freq,self.qp,self.earthquake]
  def getNeutralFormat(self, counter, mapLoadCases):
    retval= self.rare.getNeutralFormat(counter,'SLSR', mapLoadCases)
    retval.update(self.freq.getNeutralFormat(counter+len(retval),'SLSF', mapLoadCases))
    retval.update(self.qp.getNeutralFormat(counter+len(retval),'SLSQP', mapLoadCases))
    retval.update(self.earthquake.getNeutralFormat(counter+len(retval),'SLSS', mapLoadCases))
    return retval

class ULSCombinations(SituationsSet):
  '''Combinations of actions for ultimate limit states
  
  :ivar name:        name to identify the limit state (ultimate,...)
  :ivar perm:        combination for a persistent or transient design situation
  :ivar acc:         combination for a accidental design situation
  :ivar fatigue:     combination for a fatigue design situation
  :ivar earthquake:  combination for a seismic design situation
  '''
  def __init__(self):
    super(ULSCombinations,self).__init__("Ultimate limit states.")
    self.perm= SituationCombs('Persistent or transient situations.')
    self.acc= SituationCombs('Exceptional (accidental) situations.')
    self.fatigue= SituationCombs('Fatigue situations.')
    self.earthquake= SituationCombs('Earthquake situations for ULS.')
    self.situations= [self.perm,self.acc,self.fatigue,self.earthquake]
  def getNeutralFormat(self, counter, mapLoadCases):
    retval= self.perm.getNeutralFormat(counter,'ULST2', mapLoadCases)
    retval.update(self.acc.getNeutralFormat(counter+len(retval),'ULST2A', mapLoadCases))
    retval.update(self.fatigue.getNeutralFormat(counter+len(retval),'ULSF', mapLoadCases))
    retval.update(self.earthquake.getNeutralFormat(counter+len(retval),'ULSS', mapLoadCases))
    return retval

class CombContainer(object):
  '''Container of load combinations
  :ivar SLS: serviceability limit state combination
  :ivar ULS: ultimate limit state combination
  '''
  def __init__(self):
    self.SLS= SLSCombinations()
    self.ULS= ULSCombinations()
    self.limitStates= [self.SLS, self.ULS]
  def getNames(self):
    '''returns a list of the combination names.'''
    retval= list()
    for ls in self.limitStates:
      retval.extend(ls.getNames())
    return retval
  def getNeutralFormat(self, mapLoadCases):
    counter= 1
    retval= self.SLS.getNeutralFormat(counter, mapLoadCases)
    retval.update(self.ULS.getNeutralFormat(counter+len(retval), mapLoadCases))
    return retval
  def dumpCombinations(self,preprocessor):
    '''Introduces the combinations into the XC combination handler.'''
    xcCombHandler= preprocessor.getLoadHandler.getLoadCombinations
    for ls in self.limitStates:
      ls.dumpCombinations(xcCombHandler)
  def getRecordLoadCaseDisp(self,combName,setsToDispLoads,setsToDispDspRot,setsToDispIntForc):
    '''Returns a suitable RecordLoadCaseDisp for the combination.

    :ivar setsToDispLoads: ordered list of sets of elements to display loads.
    :ivar setsToDispDspRot: ordered list of sets of elements to display 
                            displacements. 
    :ivar setsToDispIntForc: ordered list of sets of elements to display 
                             internal forces.
    '''
    retval= self.SLS.getRecordLoadCaseDisp(combName,setsToDispLoads,setsToDispDspRot,setsToDispIntForc)
    if not retval:
      retval= self.ULS.getRecordLoadCaseDisp(combName,setsToDispLoads,setsToDispDspRot,setsToDispIntForc)
    return retval
