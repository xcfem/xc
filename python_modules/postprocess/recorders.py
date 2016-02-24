# -*- coding: utf-8 -*-
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

