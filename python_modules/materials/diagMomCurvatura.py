# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof

def getDiagMomentoCurvatura3d(preprocessor, nmbSecc, esfAxil, maxK, numIncr):
  ''' Función que devuelve los puntos del diagrama momento curvatura de una sección.

  :param nmbSecc: Nombre de la sección a analizar.
  :param esfAxil: Esfuerzo axil que actúa sobre la sección.
  :param maxK: Curvatura máxima alcanzada durante el análisis.
  :param numIncr: Número de incrementos en que se divide el intervalo 0->maxK.
  '''
  nodos= preprocessor.getNodeLoader

  predefined_spaces.gdls_resist_materiales3D(nodos)
  nodos.newNodeIDXYZ(1001,1,0,0)

  nodos.newNodeIDXYZ(1002,1,0,0)
  elementos= preprocessor.getElementLoader
  elementos.defaultMaterial= nmbSecc
  elementos.defaultTag= 2001 #Tag for the next element.
  zls= elementos.newElement("zero_length_section",xc.ID([1001,1002]));

  coacciones= preprocessor.getConstraintLoader
  fix_node_6dof.fixNode6DOF(coacciones,1001)
  coacciones.newSPConstraint(1002,1,0.0)
  coacciones.newSPConstraint(1002,2,0.0)

  cargas= preprocessor.getLoadLoader
  casos= cargas.getLoadPatterns
  #Load modulation.
  ts= casos.newTimeSeries("constant_ts","ts")
  casos.currentTimeSeries= "ts"
  lp0= casos.newLoadPattern("default","0")
  lp0.newNodalLoad(1002,xc.Vector([esfAxil,0,0,0,0,0]))
  #We add the load case to domain.
  casos.addToDomain("0")

  analisis= predefined_solutions.simple_newton_raphson(prueba)
  analOk= analisis.analyze(1)

  lp1= casos.newLoadPattern("default","1")
  lp1.newNodalLoad(1002,xc.Vector([0,0,0,0,0,0,1]))

  # Calculamos el incremento de curvatura
  dK= maxK/numIncr
  soluMethods= prb.getSolProc.getSoluMethodContainer
  smt= getSoluMethod("smt")
  integ= smt.newIntegrator("displacement_control_integrator")
