# -*- coding: utf-8 -*-
# Rutinas para facilitar la carga de elementos

def vector3dUniformLoadGlobalSetLineas(preprocessor,setNameLineas, cargaUnif):
  st= preprocessor.getSets.getSet(setNameLineas)
  lines= st.getLines()
  for l in lines:
    elems= l.getElems()
    for e in elems:
      vector3d_uniform_load_global(e.tag,cargaUnif)

def vector3dUniformLoadGlobalSetElementos(preprocessor,setNameElem, cargaUnif):
  st= preprocessor.getSets.getSet(setNameElem)
  elems= st.getElems()
  for e in elems:
    vector3d_uniform_load_global(e.tag,cargaUnif)

def vector3dUniformLoadLocalSetElementos(preprocessor,setNameElem, cargaUnif):
  st= preprocessor.getSets.getSet(setNameElem)
  elems= st.getElems()
  for e in elems:
    vector3d_uniform_load_local(e.tag,cargaUnif)

def beamStrainLoadSetLineas(preprocessor,setNameLineas, strain):
  pDef= xc.PlanoDeformacion(strain)
  st= preprocessor.getSets.getSet(setNameLineas)
  lines= st.getLines()
  for l in lines:
    elems= l.getElems()
    for e in elems:
      eleLoad= lp0.newElementalLoad("beam_strain_load")
      eleLoad.elementTags= xc.ID([e.tag])
      eleLoad.planoDeformacionDorsal= pDef
      eleLoad.planoDeformacionFrontal= pDef
