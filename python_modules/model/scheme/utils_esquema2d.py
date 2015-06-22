# -*- coding: utf-8 -*-
'''
Define esquema2d a partir de dos vectores: uno de abcisas y otro 
   de ordenadas. Las abcisas se colocan por columnas
'''
def setupEsquema2dXZ(mdlr, idEsquema, abcisas, ordenadas):
  ncol= size(abcisas)
  nfil= size(ordenadas)
  esq2d= mdlr.getCad.new2dScheme(idEsquema,nfil,ncol)
  for i in range(1,nfil+1):
    for j in range(1,ncol+1):
      pnt= mdlr.getCad.newPnt(abcisas[j-1],0.0,ordenadas[i-1])
      esq2d.setPnt(i,j,pnt)
  return esq2d

'''
Define una línea en un esquema bidimensional:
idEsquema: Identificador del esquema a emplear
i1,j1: Índices i y j del punto origen.
i2,j2: Índices i y j del punto destino.
nDivL: Número de divisiones para las líneas.
nmbSet: Set en el que incluiremos la línea.
'''
def defLineaEsquema2d(mdlr, idEsquema, i1, j1, i2, j2, nDivL, nmbSet):
  esq2d= mdlr.getCad.get2DScheme(idEsquema)
  tagP1= esq2d.getPnt(i1,j1)
  tagP2= esq2d.getPnt(i2,j2)
  ln= mdlr.getCad.newLine(tagP1,tagP2)
  ln.nDiv= nDivL
  tagL= ln.tag
  st= mdlr.getSets.getSet(nmbSet)
  st.selEdge(tagL)

'''
Fija los grados de libertad de un nodo
idEsquema: Identificador del esquema a emplear.
i,j: Índices i y j del punto al que pertenece el nodo.
gdls: Grados de libertad a eliminar.
'''
def fijaNodoEsquema2d(mdlr, idEsquema, i, j, gdls):
  esq2d= mdlr.getCad.get2DScheme(idEsquema)
  n= esq2d.getPnt(i,j).getNode()
  n.fix(gdls)

'''
Carga sobre un nodo
idEsquema: Identificador del esquema a emplear.
i,j: Índices i y j del punto al que pertenece el nodo.
carga: Carga a aplicar.
'''
def cargaNodoEsquema2d(mdlr,idEsquema, i, j, carga):
  esq2d= mdlr.getCad.get2DScheme(idEsquema)
  n= esq2d.getPnt(i,j).getNode()
  n.load(carga)

'''
Fija los grados de libertad de un nodo
idEsquema: Identificador del esquema a emplear.
i,j: Índices i y j del punto al que pertenece el nodo.
gdls: Grados de libertad a eliminar.
'''
def vectorDesplazNodoEsquema2d(idEsquema, i, j):
  esq2d= mdlr.getCad.get2DScheme(idEsquema)
  n= esq2d.getPnt(i,j).getNode()
  return n.getDisp()

