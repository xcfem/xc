# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from postprocess import callback_controls
from postprocess import def_vars_control

def installNodeDisplacementRecorder(recorderName, nodeSet):
  def_vars_control.defVarsControlMovModulus(nodeSet)

  preprocessor= nodeSet.owner.getPreprocessor
  nodes= preprocessor.getNodeLoader
  domain= preprocessor.getDomain
  recorder= domain.newRecorder(recorderName,None);
  recorder.setNodes(nodeSet.getTags())
  if(nodes.numGdls==3):
    recorder.callbackRecord= callback_controls.controlMovModulusUV()
  else:
    recorder.callbackRecord= callback_controls.controlMovModulusUVW()

  recorder.callbackRestart= "print \"Restart method called.\""
  return recorder

