# -*- coding: utf-8 -*-

def defVarControlMov(obj, code):
  obj.setProp(code+'Max',0.0)
  obj.setProp('Comb'+code+'Max',"")
  obj.setProp(code+'Min',0.0)
  obj.setProp('Comb'+code+'Min',"")

def defVarsControlMovs(nodes,flags):
  tags= []
  for n in nodes:
    tags.append(n.tag)
    for f in flags:
      defVarControlMov(n,f)
  return tags

# Deprecated.
# def defVarsControlMovU(nodes):
#   return defVarsControlMovs(nodes,{'U'})

# def defVarsControlMovV(nodes):
#   return defVarsControlMovs(nodes,{'V'})

# def defVarsControlMovW(nodes):
#   return defVarsControlMovs(nodes,{'W'})

# def defVarsControlMovUV(nodes):
#   return defVarsControlMovs(nodes,{'U','V'})
# def defVarsControlMovUVW(nodes):
#   return defVarsControlMovs(nodes,{'U','V','W'})

# def defVarsControlMovUVWRXRYRZ(nodes):
#   return defVarsControlMovs(nodes,{'U','V','W','RX','RY','RZ'})

def defVarsControlMovModulus(nodes):
  tags= []
  for n in nodes:
    tags.append(n.tag)
    n.setProp("dispMax",0.0)
    n.setProp("CombdispMax","")
  return tags


def defVarsControlTensRegElastico2d(elems):
  for e in elems:
    e.setProp("Sg",0)
    e.setProp("SgMax",0)
    e.setProp("SgMin",0)
    e.setProp("NCP",0)
    e.setProp("MzCP",0)
    e.setProp("FCTN",0)
    e.setProp("FCTNCP",0)
    e.setProp("HIPCPTN","")
    e.setProp("Tau",0)
    e.setProp("TauMax",0)
    e.setProp("VyCP",0)
    e.setProp("FCV",0)
    e.setProp("FCVCP",0)
    e.setProp("HIPCPV","")

def defVarsControlTensRegElastico3d(elems):
  for e in elems:
    e.setProp("Sg",0)
    e.setProp("SgMax",0)
    e.setProp("SgMin",0)
    e.setProp("NCP",0)
    e.setProp("MyCP",0)
    e.setProp("MzCP",0)
    e.setProp("FCTN",0)
    e.setProp("FCTNCP",0)
    e.setProp("HIPCPTN","")
    e.setProp("Tau",0)
    e.setProp("TauMax",0)
    e.setProp("VyCP",0)
    e.setProp("VzCP",0)
    e.setProp("FCV",0)
    e.setProp("FCVCP",0)
    e.setProp("HIPCPV","")


