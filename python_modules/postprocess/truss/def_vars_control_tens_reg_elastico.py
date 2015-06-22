# -*- coding: utf-8 -*-

def defVarsControlTensRegElastico(elems):
  for e in elems:
    e.setProp(Sg,0)
    e.setProp(SgMax,0)
    e.setProp(SgMin,0)
    e.setProp(NCP,0)
    e.setProp(FCTN,0)
    e.setProp(FCTNCP,0)
    e.setProp(HIPCPTN,"")
