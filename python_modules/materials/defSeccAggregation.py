# -*- coding: utf-8 -*-

import xc_base
from materials import typical_materials

# Define una sección elástica para elementos 3d a partir de los datos del registro.
def defSeccAggregation3d(preprocessor,defSecc):
  nmbRigF= defSecc.nmb+ "_rigF" # Rigidez a flexión.
  typical_materials.defElasticSection3d(preprocessor,nmbRigF,defSecc.A(),defSecc.E,defSecc.G(),defSecc.Iz(),defSecc.Iy(),defSecc.J())
  nmbRigVy= defSecc.nmb+"_rigVy" # Rigidez a cortante y.
  typical_materials.defElasticMaterial(preprocessor,nmbRigVy,defSecc.alphaY()*defSecc.G()*defSecc.A())
  nmbRigVz= defSecc.nmb+"_rigVz" # Rigidez a cortante z.
  typical_materials.defElasticMaterial(preprocessor,nmbRigVz,defSecc.alphaY()*defSecc.G()*defSecc.A())
  nmbRigT= defSecc.nmb+"_rigT" # Rigidez a torsión.
  typical_materials.defElasticMaterial(preprocessor,nmbRigT,defSecc.G()*defSecc.J())
  materiales= preprocessor.getMaterialLoader
  agg= materiales.newMaterial("section_aggregator",defSecc.nmb)
  agg.setSection(nmbRigF)
  agg.setAdditions(["T","Vy","Vz"],[nmbRigT,nmbRigVy,nmbRigVz])
  #agg.setAddition("Vy",nmbRigVy)
  #agg.setAddition("Vz",nmbRigVz)

# Define una sección elástica para elementos 2d a partir de los datos del registro.
def defSeccAggregation2d(preprocessor,defSecc):
  nmbRigF= defSecc.nmb+ "_rigF" # Rigidez a flexión.
  typical_materials.defElasticSection2d(preprocessor,nmbRigF,defSecc.A(),defSecc.E,defSecc.Iz())
  nmbRigVy= defSecc.nmb+"_rigVy" # Rigidez a cortante y.
  typical_materials.defElasticMaterial(preprocessor,nmbRigVy,defSecc.alphaY()*defSecc.G()*defSecc.A())
  materiales= preprocessor.getMaterialLoader
  agg= materiales.newMaterial("section_aggregator",defSecc.nmb)
  agg.setSection(nmbRigF)
  agg.setAdditions(["Vy"],[nmbRigVy])
  #agg.setAddition("Vy",nmbRigVy)

