# -*- coding: utf-8 -*-
# Rutinas para facilitar el tesado de elementos

def preTensaElementosLinea(preprocessor,numLinea, tension):
  line= preprocessor.getCad.getLine(numLinea)
  elems= linea.getElems()
  for e in elems:
    e.getMaterial().prestress(tension)

def posTensaElementosLinea(preprocessor,numLinea, tension):
  line= preprocessor.getCad.getLine(numLinea)
  elems= linea.getElems()
  for e in elems:
    pret= e.getPrestrass
    e.getMaterial().prestress(tension+pret)

def retensaElementos(nmbSetLineas, umbral, tension):
  setsLines= preprocessor.getSets.getSet(nmbSetLineas)
  for s in setsLines:
    elems= linea.getElems()
    for e in elems:
      pret= e.getPrestress()
      if(pret<umbral):
        e.prestress(tension+pret)
