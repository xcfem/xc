# -*- coding: utf-8 -*-
'''
Genera elementos de tipo «ElasticBeam3d» para las líneas
   del conjunto que se pasa como parámetro. El conjunto ha de tener
   definidos las siguientes variables:
   - nmbPerfil: Nombre del perfil a emplear (por ejemplo L50x50x5)
   - nmbGeomTransf: Nombre de la transformación geométrica a emplear.
   - numDivisiones: Número de elementos a generar en cada línea.
'''
def mallaSetConPerfilMetalico(preprocessor,nmbSet):
  nmbPerfilMet= "" 
  nmbTrf= ""
  nmbTipoAcero= ""
  nmbSeccion= ""
  setElems= preprocessor.getSetLoader.getSet(nmbSet)
  nmbPerfilMet= setElems.getProp(nmbPerfil)
  nmbTipoAcero= setElems.getProp(nmbAcero)
  nmbTrf= setElems.getProp(nmbGeomTransf)

  nmbSeccion= nmbPerfilMet+"_"+nmbGeomTransf
  defSeccPerfilMetalicoShElastica3d(nmbPerfilMet,nmbSeccion)
  seedElemLoader= preprocessor.getElementLoader.seedElemLoader
  seedElemLoader.defaultMaterial= nmbSeccion
  seedElemLoader.defaultTransformation= nmbTrf
  elem= seedElemLoader.newElement("elastic_beam_3d",xc.ID([0,0]))
  elem.rho= nmbPerfilMet.P
  defParamsPerfilMetalicoRegElasticoSet(elem,nmbPerfilMet)
  lines= setElems.getLines
  for l in lines:
    l.nDiv= numDivisiones
  setElems.genMesh(xc.meshDir,1)
  

'''
Genera elementos de tipo «Truss» para las líneas
   del conjunto que se pasa como parámetro. El conjunto ha de tener
   definidos las siguientes variables:
   - nmbPerfil: Nombre del perfil a emplear (por ejemplo L50x50x5)
   - numDivisiones: Número de elementos a generar en cada línea.
'''
def mallaSetConTruss(nmbSet):
  nmbPerfilMet= "" 
  nmbTipoAcero= ""
  nmbMat= ""
  fydMat= 0.0 
  EMat= 0.0 
  
  setElems= preprocessor.getSetLoader.getSet(nmbSet)
  nmbPerfilMet= setElems.getProp("nmbPerfil")
  nmbTipoAcero= setElems.getProp("nmbAcero")
  gammaMat= setElems.getProp("gammaM")
  areaBrr= nmbPerfilMet.A

  nmbMat= nmbSet+"_"+nmbPerfilMet.nmbTipoAcero
  fydMat= nmbPerfilMet.nmbTipoAcero.fy/gammaMat
  EMat= nmbePerfilMet.nmbTipoAcero.E

  mat= typical_materials.defElasticMaterial(nmbMat,EMat)
  seedElemLoader= preprocessor.getElementLoader.seedElemLoader
  seedElemLoader.defaultMaterial= nmbMat
  seedElemLoader.dimElem= 3
  truss= seedElemLoader.newElement("truss",xc.ID([0,0]))
  truss.A= areaBrr
  truss.setProp("fyd",fydMat)
  lines= setElems.getLines
  for l in lines:
    l.nDiv= numDivisiones
  setElems.genMesh(xc.meshDir,1)

