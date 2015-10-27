# -*- coding: utf-8 -*-

# Crea líneas entre los puntos de la lista que se pasa como parámetro
def creaLineasLista(preprocessor, nmbSet, lstTagsPuntos, numDiv):
  preprocessor.getSets().abreSet(nmbSet)
  lineas= preprocessor.getCad.getLines
  nl= lstPuntos.size-1
  i= 0.0
  for i in range(0,nl):
    l= lineas.newLine(lstPuntos[i],lstPuntos[i+1])
    l.nDiv= numDiv
  preprocessor.getSets().cierraSet(nmbSet)


# Crea líneas entre los puntos de las listas que se pasa como parámetro
def creaLineasListas(preprocessor,lstPuntosA,lstPuntosB, numDiv, nmbSet):
  nl= min(lstPuntosA.size,lstPuntosB.size)
  i= 0.0
  preprocessor.getSets().abreSet(nmbSet)
  for i in range(0,nl):
    l= lineas.newLine(lstPuntosA[i],lstPuntosB[i])
    l.nDiv= numDiv
  preprocessor.getSets().cierraSet(nmbSet)


# Crea líneas entre puntos consecutivos
def CreaLineasPtosConsec(primero, ultimo, numDiv, nmbSet):
  i= 0.0
  preprocessor.getSets().abreSet(nmbSet)
  for i in range(primero,ultimo+1):
    l= lineas.newLine(i,i+1)
    l.nDiv= numDiv
  preprocessor.getSets().cierraSet(nmbSet)

