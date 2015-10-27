# -*- coding: utf-8 -*-
import xc_base
import geom
import xc

# Vectores de elementos unidimensionales.
def getVectorIElem(preprocessor,idElem):
  elem= preprocessor.getElementLoader.getElement(idElem)
  return elem.getCoordTransf.getIVector


def getVectorJElem(preprocessor,idElem):
  elem= preprocessor.getElementLoader.getElement(idElem)
  return elem.getCoordTransf.getJVector

def getVectorKElem(preprocessor,idElem):
  elem= preprocessor.getElementLoader.getElement(idElem)
  return elem.getCoordTransf.getKVector

