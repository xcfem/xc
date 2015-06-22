# -*- coding: utf-8 -*-
# Rutinas para facilitar la carga de elementos

def vector3dUniformLoadGlobalSetLineas(mdlr,nmbSetLineas, cargaUnif):
  st= mdlr.getSets.getSet(nmbSetLineas)
  lines= st.getLines()
  for l in lines:
    elems= l.getElems()
    for e in elems:
      vector3d_uniform_load_global(e.tag,cargaUnif)

def vector3dUniformLoadGlobalSetElementos(mdlr,nmbSetElem, cargaUnif):
  st= mdlr.getSets.getSet(nmbSetElem)
  elems= st.getElems()
  for e in elems:
    vector3d_uniform_load_global(e.tag,cargaUnif)

def vector3dUniformLoadLocalSetElementos(mdlr,nmbSetElem, cargaUnif):
  st= mdlr.getSets.getSet(nmbSetElem)
  elems= st.getElems()
  for e in elems:
    vector3d_uniform_load_local(e.tag,cargaUnif)

def beamStrainLoadSetLineas(mdlr,nmbSetLineas, strain):
  pDef= xc.PlanoDeformacion(strain)
  st= mdlr.getSets.getSet(nmbSetLineas)
  lines= st.getLines()
  for l in lines:
    elems= l.getElems()
    for e in elems:
      eleLoad= lp0.newElementalLoad("beam_strain_load")
      eleLoad.elementTags= xc.ID([e.tag])
      eleLoad.planoDeformacionDorsal= pDef
      eleLoad.planoDeformacionFrontal= pDef
