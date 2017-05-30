# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"


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



