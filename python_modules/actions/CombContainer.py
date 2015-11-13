# -*- coding: utf-8 -*-

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

class SLSCombinations(object):
  def __init__(self):
    self.name= "Service limit states."
    self.rare= SituationCombs('Rare situations.')
    self.freq= SituationCombs('Frequent situations.')
    self.qp= SituationCombs('Quasi-permanent situations.')
    self.earthquake= SituationCombs('Earthquake situations for SLS.')

class ULSCombinations(object):
  def __init__(self):
    self.name= "Ultimate limit states."
    self.perm= SituationCombs('Permanent and transitory situations.')
    self.acc= SituationCombs('Exceptional (accidental) situations.')
    self.fatigue= SituationCombs('Fatigue situations.')
    self.earthquake= SituationCombs('Earthquake situations for ULS.')

class CombContainer(object):
  def __init__(self):
    self.SLS= SLSCombinations()
    self.ULS= ULSCombinations()
