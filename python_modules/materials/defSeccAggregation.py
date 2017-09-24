# -*- coding: utf-8 -*-

import xc_base
from materials import typical_materials

def defSeccAggregation3d(preprocessor,defSecc,defMat):
  ''' Definition of a elastic material-section for 3D elements

  :param preprocessor: preprocessor name
  :param defSecc:  object with the mechanical properties of the section (A, Iy, Iz, ...)
  :param defMat:   object with the properties of the material (E, G)  
  '''
  nmbRigF= defSecc.sectionName+ "_rigF" # Bending stiffness.
  typical_materials.defElasticSection3d(preprocessor,nmbRigF,defSecc.A(),defMat.E,defMat.G(),defSecc.Iz(),defSecc.Iy(),defSecc.J())
  nmbRigVy= defSecc.sectionName+"_rigVy" # Y shear stiffness.
  typical_materials.defElasticMaterial(preprocessor,nmbRigVy,defSecc.alphaY()*defMat.G()*defSecc.A())
  nmbRigVz= defSecc.sectionName+"_rigVz" # Z shear stiffness.
  typical_materials.defElasticMaterial(preprocessor,nmbRigVz,defSecc.alphaY()*defMat.G()*defSecc.A())
  nmbRigT= defSecc.sectionName+"_rigT" # Torsional stiffness.
  typical_materials.defElasticMaterial(preprocessor,nmbRigT,defMat.G()*defSecc.J())
  materiales= preprocessor.getMaterialLoader
  agg= materiales.newMaterial("section_aggregator",defSecc.sectionName)
  agg.setSection(nmbRigF)
  agg.setAdditions(["T","Vy","Vz"],[nmbRigT,nmbRigVy,nmbRigVz])
  #agg.setAddition("Vy",nmbRigVy)
  #agg.setAddition("Vz",nmbRigVz)

def defSeccAggregation2d(preprocessor,defSecc,defMat):
  ''' Definition of a elastic material-section for 2D elements

  :param preprocessor: preprocessor name
  :param defSecc:  object with the mechanical properties of the section (A, Iy, Iz, ...)
  :param defMat:   object with the properties of the material (E, G)  
  '''
  nmbRigF= defSecc.sectionName+ "_rigF" # Bending stiffness.
  typical_materials.defElasticSection2d(preprocessor,nmbRigF,defSecc.A(),defMat.E,defSecc.Iz())
  nmbRigVy= defSecc.sectionName+"_rigVy" # Y shear stiffness.
  typical_materials.defElasticMaterial(preprocessor,nmbRigVy,defSecc.alphaY()*defMat.G()*defSecc.A())
  materiales= preprocessor.getMaterialLoader
  agg= materiales.newMaterial("section_aggregator",defSecc.sectionName)
  agg.setSection(nmbRigF)
  agg.setAdditions(["Vy"],[nmbRigVy])
  #agg.setAddition("Vy",nmbRigVy)

