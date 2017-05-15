# -*- coding: utf-8 -*-

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

