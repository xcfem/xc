# -*- coding: utf-8 -*-
'''
Define esquema2d a partir de dos vectores: uno de abcisas y otro 
   de ordenadas. Las abcisas se colocan por columnas
'''
def setupEsquema2dXZ(preprocessor, idEsquema, abcisas, ordenadas):
  ncol= size(abcisas)
  nfil= size(ordenadas)
  esq2d= preprocessor.getMultiBlockTopology.new2dScheme(idEsquema,nfil,ncol)
  for i in range(1,nfil+1):
    for j in range(1,ncol+1):
      pnt= preprocessor.getMultiBlockTopology.newPnt(abcisas[j-1],0.0,ordenadas[i-1])
      esq2d.setPnt(i,j,pnt)
  return esq2d

'''
Define una línea en un esquema bidimensional:
idEsquema: Identificador del esquema a emplear
i1,j1: Índices i y j del punto origen.
i2,j2: Índices i y j del punto destino.
nDivL: Número de divisiones para las líneas.
setName: Set en el que incluiremos la línea.
'''
def defLineaEsquema2d(preprocessor, idEsquema, i1, j1, i2, j2, nDivL, setName):
  esq2d= preprocessor.getMultiBlockTopology.get2DScheme(idEsquema)
  tagP1= esq2d.getPnt(i1,j1)
  tagP2= esq2d.getPnt(i2,j2)
  ln= preprocessor.getMultiBlockTopology.newLine(tagP1,tagP2)
  ln.nDiv= nDivL
  tagL= ln.tag
  st= preprocessor.getSets.getSet(setName)
  st.selEdge(tagL)

'''
Fija los grados de libertad de un node
idEsquema: Identificador del esquema a emplear.
i,j: Índices i y j del punto al que pertenece el node.
gdls: Grados de libertad a eliminar.
'''
def fixNodeEsquema2d(preprocessor, idEsquema, i, j, gdls):
  esq2d= preprocessor.getMultiBlockTopology.get2DScheme(idEsquema)
  n= esq2d.getPnt(i,j).getNode()
  n.fix(gdls)

'''
Carga sobre un node
idEsquema: Identificador del esquema a emplear.
i,j: Índices i y j del punto al que pertenece el node.
carga: Carga a aplicar.
'''
def loadNodeEsquema2d(preprocessor,idEsquema, i, j, carga):
  esq2d= preprocessor.getMultiBlockTopology.get2DScheme(idEsquema)
  n= esq2d.getPnt(i,j).getNode()
  n.load(carga)

'''
Fija los grados de libertad de un node
idEsquema: Identificador del esquema a emplear.
i,j: Índices i y j del punto al que pertenece el node.
gdls: Grados de libertad a eliminar.
'''
def nodeDisplacementVectorEsquema2d(idEsquema, i, j):
  esq2d= preprocessor.getMultiBlockTopology.get2DScheme(idEsquema)
  n= esq2d.getPnt(i,j).getNode()
  return n.getDisp()

