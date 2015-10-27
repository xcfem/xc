# -*- coding: utf-8 -*-
'''Borra todas las acciones del modelo y crea un nuevo TimeSeries
    tipoTimeSeries: Tipo del time series.
    nmbTimeSeries: Nombre del time series.
    fct: Factor.
'''
def resetAccionesConstantTS(preprocessor,tipoTimeSeries, nmbTimeSeries, fct):
  coacciones= preprocessor.getConstraintLoader
  lps= coacciones.getActiveLoadPatterns
  lps.clearrAll()
  loads= preprocessor.getLoadLoader
  casos= loads.getLoadPatterns
  loads.clearAll()
  ts= casos.newTimeSeries(tipoTimeSeries,nmbTimeSeries)
  casos.currentTimeSeries= nmbTimeSeries

