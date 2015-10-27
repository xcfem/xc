# -*- coding: utf-8 -*-

def defMaterialesK(preprocessor,path, nmbScc1, nmbScc2):
  '''
   Define los materiales (hormigón, acero y secciones)
    que se emplearán para la comprobación a fisuración
    empleando sus valores característicos.
  '''
  preprocessor.clearAll
  materiales= preprocessor.getMaterialLoader
  tagDiagHormigon= materiales.defDiagKHormigon(codHormigon)
  tagDiagAceroArmar= materiales.defDiagKAcero(codArmadura)
  mat= mat_with_tag(tagDiagHormigon)
  mat.Ec= getTangent 
  mat= mat_with_tag(tagDiagAceroArmar)
  mat.Es= getTangent 
  defSeccionHASimple(nmbScc1,"k","defSectionGeometry")
  archTex= open(path+nmbScc1+".tex","w")
  informeGeomSeccion(nmbScc1,archTex,path+nmbScc1+".eps")
  archTex.close()
  defSeccionHASimple(nmbScc2,"k","defSectionGeometry")
  archTex= open(path+nmbScc2+".tex","w")
  informeGeomSeccion(nmbScc2,archTex,path+nmbScc2+".eps")
  archTex.close()

def defMaterialesD(path, nmbScc1, nmbScc2):
  '''
   Define los materiales (hormigón, acero y secciones)
    que se emplearán para la comprobación a flexion
    empleando sus valores de cálculo.
  '''
  preprocessor.clearAll
  materiales= preprocessor.getMaterialLoader
  tagDiagHormigon= materiales.defDiagDHormigon(codHormigon)
  tagDiagAceroArmar= materiales.defDiagDAcero(codArmadura)
  mat= mat_with_tag(tagDiagHormigon)
  mat.Ec= getTangent 
  mat= mat_with_tag(tagDiagAceroArmar)
  mat.Es= getTangent 

  defSeccionHASimple(nmbScc1,"d","defSectionGeometry")
  archTex= open(path+nmbScc1+".tex","w")
  informeGeomSeccion(nmbScc1,archTex,path+nmbScc1+".eps")
  archTex.close()
  defSeccionHASimple(nmbScc2,"d","defSectionGeometry")
  archTex= open(path+nmbScc2+".tex","w")
  informeGeomSeccion(nmbScc2,archTex,path+nmbScc2+".eps")
  archTex.close()


def defMaterialesKBarra(path, nmbScc, nmbRutinaDefGeom):
  '''
   Define los materiales (hormigón, acero y secciones)
    que se emplearán para la comprobación a fisuración
    empleando sus valores característicos.
  '''
  preprocessor.clearAll
  materiales= preprocessor.getMaterialLoader
  tagDiagHormigon= materiales.defDiagKHormigon(codHormigon)
  tagDiagAceroArmar= materiales.defDiagKAcero(codArmadura)
  mat= mat_with_tag(tagDiagHormigon)
  mat.Ec= getTangent 
  mat= mat_with_tag(tagDiagAceroArmar)
  mat.Es= getTangent 
  defSeccionHASimple(nmbScc,"k",nmbRutinaDefGeom)
  archTex= open(path+nmbScc+".tex","w")
  informeGeomSeccion(nmbScc,archTex,path+nmbScc+".eps")
  archTex.close()


def defMaterialesDBarra(path, nmbScc, nmbRutinaDefGeom):
  '''
   Define los materiales (hormigón, acero y secciones)
    que se emplearán para la comprobación a flexión
    empleando sus valores de cálculo.
  '''
  preprocessor.clearAll
  materiales= preprocessor.getMaterialLoader
  tagDiagHormigon= materiales.defDiagDHormigon(codHormigon)
  tagDiagAceroArmar= materiales.defDiagDAcero(codArmadura)
  mat= mat_with_tag(tagDiagHormigon)
  mat.Ec= getTangent 
  mat= mat_with_tag(tagDiagAceroArmar)
  mat.Es= getTangent 

  defSeccionHASimple(nmbScc,"d",nmbRutinaDefGeom)
  archTex= open(path+nmbScc+".tex","w")
  informeGeomSeccion(nmbScc,archTex,path+nmbScc+".eps")
  archTex.close()
