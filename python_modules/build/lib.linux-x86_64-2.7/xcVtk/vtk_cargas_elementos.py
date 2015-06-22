def VtkCargaUniforme(mdlr,unifLoad,loadPattern, renderer, color, carga, fEscala):
  actorName= "flechaU"+loadPattern+"%04d".format(tag) # Tag carga.
  eleTags= unifLoad.getElementTags()
  for tag in eleTags:
    ele= mdlr.getElementLoader.getElement(tag)
    actorName= actorName+format(tag,"%04d") # Tag elemento.
    puntos= ele.getPuntos(3)
    for i in range(0,2):
      pos= getRow(i)
      dibujaFlecha(actorName+"%02d".format(i),renderer,color,pos,carga,fEscala*abs(carga))

def VtkCargaPuntual(mdlr,pLoad,loadPattern, renderer, color, carga, fEscala):
  xCarga= pLoad.getElems()
  eleTags= unifLoad.getElementTags()
  actorName= "flechaP"+loadPattern+format(tag,"%04d") # Tag carga.
  for tag in eleTags:
    ele= mdlr.getElementLoader.getElement(tag)
    actorName= actorName+format(tag,"%04d") # Tag elemento.
    pos= ele.punto(xCarga)
    dibujaFlecha(actorName,renderer,color,pos,carga,fEscala)()

def VtkCargasElementos(mdlr,loadPattern, renderer, color, fEscala):
  #mdlr.getLoadLoader.getLoadPatterns.addToDomain(loadPattern)
  loadPattern.addToDomain()
  eleLoadIter= loadPattern.getElementalLoadIter
  eleLoad= eleLoadIter.next()
  while not(eleLoad is None):
    carga= eleLoad.getGlobalForces
    categoria= eleLoad.category
    if(categoria=="uniforme"):
      VtkCargaUniforme(mdlr,loadPattern,renderer,color,carga,fEscala)
    else:
      VtkCargaPuntual(mdlr,loadPattern,renderer,color,carga,fEscala)
  loadPattern.removeFromDomain()


