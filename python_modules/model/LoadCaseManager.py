# -*- coding: utf-8 -*-

import geom
import xc
import logging

logging.addLevelName( logging.WARNING, "\033[1;31m%s\033[1;0m" % logging.getLevelName(logging.WARNING))
logging.addLevelName( logging.ERROR, "\033[1;41m%s\033[1;0m" % logging.getLevelName(logging.ERROR))

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
      logging.warning('Load case: \''+ name+ '\' doesn\'t exists.')
