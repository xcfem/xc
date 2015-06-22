# -*- coding: utf-8 -*-
# Rutinas para facilitar el tesado de elementos

def preTensaElementosLinea(mdlr,numLinea, tension):
  line= mdlr.getCad.getLine(numLinea)
  elems= linea.getElems()
  for e in elems:
    e.getMaterial().prestress(tension)

def posTensaElementosLinea(mdlr,numLinea, tension):
  line= mdlr.getCad.getLine(numLinea)
  elems= linea.getElems()
  for e in elems:
    pret= e.getPrestrass
    e.getMaterial().prestress(tension+pret)

def retensaElementos(nmbSetLineas, umbral, tension):
  setsLines= mdlr.getSets.getSet(nmbSetLineas)
  for s in setsLines:
    elems= linea.getElems()
    for e in elems:
      pret= e.getPrestress()
      if(pret<umbral):
        e.prestress(tension+pret)
