# -*- coding: utf-8 -*-

import geom
import xc
from miscUtils import LogMessages as lmsg


class LoadCaseManager(object):
  '''Manager for XC load cases.'''
  def __init__(self,preprocessor):
    self.prep= preprocessor
    self.loadLoader= preprocessor.getLoadLoader
    self.loadPatterns= self.loadLoader.getLoadPatterns
    self.timeSeries= dict()
    self.loadCases= dict()

  def defineSimpleLoadCases(self,names):
    '''Define load patterns with constant time series.'''
    tsName= 'ts'
    ts= self.loadPatterns.newTimeSeries('constant_ts',tsName)
    self.timeSeries[tsName]= ts
    self.loadPatterns.currentTimeSeries= tsName
    for name in names:
      self.loadCases[name]= self.loadPatterns.newLoadPattern('default',name)

  def setCurrentLoadCase(self,name):
    self.loadPatterns.currentLoadPattern= name
    if(name not in self.loadCases):
      lmsg.warning('Load case: \''+ name+ '\' doesn\'t exists.')
