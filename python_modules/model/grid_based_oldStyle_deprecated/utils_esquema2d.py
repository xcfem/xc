# -*- coding: utf-8 -*-

def setupEsquema2dXZ(preprocessor, idEsquema, abcisas, ordenadas):
  '''
  Define esquema2d a partir de dos vectores: uno de abcisas y otro 
     de ordenadas. Las abcisas se colocan por columnas
  '''
  ncol= size(abcisas)
  nfil= size(ordenadas)
  esq2d= preprocessor.getMultiBlockTopology.new2dScheme(idEsquema,nfil,ncol)
  for i in range(1,nfil+1):
    for j in range(1,ncol+1):
      pnt= preprocessor.getMultiBlockTopology.newPnt(abcisas[j-1],0.0,ordenadas[i-1])
      esq2d.setPnt(i,j,pnt)
  return esq2d

def defLineaEsquema2d(preprocessor, idEsquema, i1, j1, i2, j2, nDivL, setName):
  '''
  Define una línea en un esquema bidimensional:

  idEsquema: Identificador del esquema a emplear
  i1,j1: indices i and j of the origin point.
  i2,j2: indices i and j of the target point.
  nDivL: Número de divisiones para las líneas.
  setName: Set en el que incluiremos la línea.
  '''
  esq2d= preprocessor.getMultiBlockTopology.get2DScheme(idEsquema)
  tagP1= esq2d.getPnt(i1,j1)
  tagP2= esq2d.getPnt(i2,j2)
  ln= preprocessor.getMultiBlockTopology.newLine(tagP1,tagP2)
  ln.nDiv= nDivL
  tagL= ln.tag
  st= preprocessor.getSets.getSet(setName)
  st.selEdge(tagL)

def fixNodeEsquema2d(preprocessor, idEsquema, i, j, gdls):
  '''
  Fija los grados de libertad de un node

  idEsquema: Identificador del esquema a emplear.
  i,j: indices i and j of the point to which the node belongs.
  gdls: Grados de libertad a eliminar.
  '''
  esq2d= preprocessor.getMultiBlockTopology.get2DScheme(idEsquema)
  n= esq2d.getPnt(i,j).getNode()
  n.fix(gdls)

def loadNodeEsquema2d(preprocessor,idEsquema, i, j, carga):
  '''
  Carga sobre un node

  idEsquema: Identificador del esquema a emplear.
  i,j: indices i and j of the point to which the node belongs.
  carga: Carga a aplicar.
  '''
  esq2d= preprocessor.getMultiBlockTopology.get2DScheme(idEsquema)
  n= esq2d.getPnt(i,j).getNode()
  n.load(carga)

def nodeDisplacementVectorEsquema2d(idEsquema, i, j):
  '''
  Fija los grados de libertad de un node

  idEsquema: Identificador del esquema a emplear.
  i,j: indices i and j of the point to which the node belongs.
  gdls: Grados de libertad a eliminar.
  '''
  esq2d= preprocessor.getMultiBlockTopology.get2DScheme(idEsquema)
  n= esq2d.getPnt(i,j).getNode()
  return n.getDisp()

