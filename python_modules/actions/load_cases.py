# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O) Luis C. Pérez Tato"
__copyright__= "Copyright 2017, AO_O LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es l.pereztato@ciccp.es"

from miscUtils import LogMessages as lmsg

class LoadCase(object):
    '''Definition of a load case.

    :ivar name: name given to the load case
    :ivar loadPType: type of load pattern. Available types:
          "default", "uniform_excitation", "multi_support_pattern", 
          "pbowl_loading". Defaults to "default"
    :ivar timeSType: type of Time Series. Available time series:
          "constant_ts","linear_ts","path_ts","pulse_ts","rectangular_ts","triangular_ts","trig_ts". Defaults to "constant_ts"
    :ivar timeSName: name of the Time Serie
    '''
    def __init__(self,preprocessor,name,loadPType="default",timeSType="constant_ts"):
        self.preprocessor=preprocessor
        self.name= name
        self.loadPType=loadPType
        self.timeSType=timeSType
        self.timeSName=self.name+'TS'

    def create(self):
        '''Create the load case and set it as current
        '''
        lPatterns=self.preprocessor.getLoadLoader.getLoadPatterns
        lPatterns.newTimeSeries(self.timeSType,self.timeSName)
        lPatterns.currentTimeSeries=self.timeSName
        lPatterns.newLoadPattern(self.loadPType,self.name)
        lPatterns.addToDomain(self.name)
        lPatterns.currentLoadPattern=self.name
            
    def addLstLoads(self,lstLoads):
        '''list of loads to be added to the load case.
        '''
        for ld in lstLoads:
            ld.appendLoadToCurrentLoadPattern()

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
       '''Sets current load case.'''
       self.loadPatterns.currentLoadPattern= name
       if(name not in self.loadCases):
          lmsg.warning('Load case: \''+ name+ '\' doesn\'t exists.')
       return self.getLoadCase(name)

    def getCurrentLoadCase(self):
       '''Returns current load case.'''
       name= self.loadPatterns.currentLoadPattern
       return self.getLoadCase(name)

    def getLoadCase(self,name):
       return self.loadCases[name]


def resetAccionesConstantTS(preprocessor,tipoTimeSeries, nmbTimeSeries, fct):
    '''Clear all load patterns in the model and create a new TimeSeries

    :param tipoTimeSeries: time series type
    :param  nmbTimeSeries: time series name.
    :param  fct: Factor
    '''
    constraints= preprocessor.getConstraintLoader
    lps= constraints.getActiveLoadPatterns
    lps.clearrAll()
    loads= preprocessor.getLoadLoader
    casos= loads.getLoadPatterns
    loads.clearAll()
    ts= casos.newTimeSeries(tipoTimeSeries,nmbTimeSeries)
    casos.currentTimeSeries= nmbTimeSeries
