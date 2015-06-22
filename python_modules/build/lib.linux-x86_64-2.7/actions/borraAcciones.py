# -*- coding: utf-8 -*-
'''Borra todas las acciones del modelo y crea un nuevo TimeSeries
    tipoTimeSeries: Tipo del time series.
    nmbTimeSeries: Nombre del time series.
    fct: Factor.
'''
def resetAccionesConstantTS(mdlr,tipoTimeSeries, nmbTimeSeries, fct):
  coacciones= mdlr.getConstraintLoader
  lps= coacciones.getActiveLoadPatterns
  lps.clearrAll()
  loads= mdlr.getLoadLoader
  casos= loads.getLoadPatterns
  loads.clearAll()
  ts= casos.newTimeSeries(tipoTimeSeries,nmbTimeSeries)
  casos.currentTimeSeries= nmbTimeSeries

