# -*- coding: utf-8 -*-
import xc_base
import geom
import xc
import nodalReactions

print "reacs_nodo_6gdl deprecated; use nodalReactions."

def vectorReacUVW(mdlr,idNodo):
  return nodalReactions.vectorReacUVW(mdlr,idNodo)


