# -*- coding: utf-8 -*-

def VtkCargaNodo(nod, renderer, color, carga, momento, fEscala):
  actorName= actorName+format(nod.tag,"%04d") # Tag nodo.

  pos= nod.coord
  if(abs(carga)>1e-6):
    dibujaFlecha(actorName+"P",renderer,color,pos,carga,fEscala*abs(carga))

  if(abs(momento)>1e-6):
    dibujaFlechaDoble(actorName+"M",renderer,color,pos,momento,fEscala*abs(momento))

def VtkCargasNodos(loadPattern, renderer, color, fEscala):
  mdlr.getLoadLoader.getLoadPatterns.addToDomain(loadPattern)
  lPattern= mdlr.getLoadLoader.getLoadPatterns.getLoadPattern(loadPattern)
  lIter= lPattern.getNodalLoadIter()
  load= lIter.next()
  while not(load is None):
    actorName= "flecha"+loadPattern+format(tag,"%04d") # Tag carga.
    carga= load.getForce
    momento= load.getMoment
    VtkCargaNodo(loadPattern,renderer,color,carga,momento,fEscala)    
    load= lIter.next()
  mdlr.getLoadLoader.getLoadPatterns.removeFromDomain(loadPattern)
