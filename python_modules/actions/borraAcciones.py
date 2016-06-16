# -*- coding: utf-8 -*-
'''Borra todas las acciones del modelo y crea un nuevo TimeSeries
    tipoTimeSeries: Tipo del time series.
    nmbTimeSeries: Nombre del time series.
    fct: Factor.
'''
def resetAccionesConstantTS(preprocessor,tipoTimeSeries, nmbTimeSeries, fct):
  '''Clears all load patterns in the model and creates a new TimeSeries

  :param tipoTimeSeries: time series type
  :param  nmbTimeSeries: time series name.
  :param  fct: Factor

  '''
  coacciones= preprocessor.getConstraintLoader
  lps= coacciones.getActiveLoadPatterns
  lps.clearrAll()
  loads= preprocessor.getLoadLoader
  casos= loads.getLoadPatterns
  loads.clearAll()
  ts= casos.newTimeSeries(tipoTimeSeries,nmbTimeSeries)
  casos.currentTimeSeries= nmbTimeSeries

