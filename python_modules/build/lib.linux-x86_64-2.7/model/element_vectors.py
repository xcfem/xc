# -*- coding: utf-8 -*-
import xc_base
import geom
import xc

# Vectores de elementos unidimensionales.
def getVectorIElem(mdlr,idElem):
  elem= mdlr.getElementLoader.getElement(idElem)
  return elem.getCoordTransf.getIVector


def getVectorJElem(mdlr,idElem):
  elem= mdlr.getElementLoader.getElement(idElem)
  return elem.getCoordTransf.getJVector

def getVectorKElem(mdlr,idElem):
  elem= mdlr.getElementLoader.getElement(idElem)
  return elem.getCoordTransf.getKVector

