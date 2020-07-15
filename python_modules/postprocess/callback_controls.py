def controlMovU():
  return """U= self.getDisp[0]
nmbComb= recorder.getCurrentCombinationName
if(U>self.getProp("UMax")):
   self.setProp("UMax",U)
   self.setProp("CombUMax",nmbComb)
if(U<self.getProp("UMin")):
   self.setProp("UMin",U)
   self.setProp("CombUMin",nmbComb)"""

def controlMovV():
  return """V= self.getDisp[1]
nmbComb= recorder.getCurrentCombinationName
if(V>self.getProp("VMax")):
   self.setProp("VMax",V)
   self.setProp("CombVMax",nmbComb)
if(V<self.getProp("VMin")):
   self.setProp("VMin",V)
   self.setProp("CombVMin",nmbComb)"""

def controlMovW():
  return """W= self.getDisp[2]
nmbComb= recorder.getCurrentCombinationName
if(W>self.getProp("WMax")):
   self.setProp("WMax",W)
   self.setProp("CombWMax",nmbComb)
if(W<self.getProp("WMin")):
   self.setProp("WMin",W)
   self.setProp("CombWMin",nmbComb)"""

def controlMovUV():
  return """U= self.getDisp[0]
nmbComb= recorder.getCurrentCombinationName
if(U>self.getProp("UMax")):
   self.setProp("UMax",U)
   self.setProp("CombUMax",nmbComb)
if(U<self.getProp("UMin")):
   self.setProp("UMin",U)
   self.setProp("CombUMin",nmbComb)
V= self.getDisp[1]
if(V>self.getProp("VMax")):
   self.setProp("VMax",V)
   self.setProp("CombVMax",nmbComb)
if(V<self.getProp("VMin")):
   self.setProp("VMin",V)
   self.setProp("CombVMin",nmbComb)"""

def controlMovModulusUV():
  return """modMov= math.sqrt(self.getDisp[0]**2+self.getDisp[1]**2)
nmbComb= recorder.getCurrentCombinationName
if(modMov>self.getProp("dispMax")):
   self.setProp("dispMax",modMov)
   self.setProp("CombDispMax",nmbComb)"""

def fnControlMovComponent(recorder, obj, codeComponent, value):
  propVMax= codeComponent+'Max'
  propCombVMax= 'Comb'+propVMax
  propVMin= codeComponent+'Min'
  propCombVMin= 'Comb'+propVMin

  nmbComb= recorder.getCurrentCombinationName
  if(value>obj.getProp(propVMax)):
     obj.setProp(propVMax,value)
     obj.setProp(propCombVMax,nmbComb)
  if(value<obj.getProp(propVMin)):
     obj.setProp(propVMin,value)
     obj.setProp(propCombVMin,nmbComb)

def fnControlMovs(recorder, obj):
  componentsToControl= recorder.getProp("flags")
  for key in componentsToControl:
    d= obj.getDisp[componentsToControl[key]]
    fnControlMovComponent(recorder,obj,key,d)

def fnControlMovUVW(recorder, obj):
  U= obj.getDisp[0]
  fnControlMovComponent(recorder,obj,'U',U)
  V= obj.getDisp[1]
  fnControlMovComponent(recorder,obj,'V',V)
  W= obj.getDisp[2]
  fnControlMovComponent(recorder,obj,'W',W)

def controlMovUVW():
  return """callback_controls.fnControlMovUVW(recorder,self)"""

def controlMovs():
  return """callback_controls.fnControlMovs(recorder,self)"""

def controlMovModulusUVW():
  return """modMov= math.sqrt(self.getDisp[0]**2+self.getDisp[1]**2+self.getDisp[2]**2)
nmbComb= recorder.getCurrentCombinationName
if(modMov>self.getProp("dispMax")):
   self.setProp("dispMax",modMov)
   self.setProp("CombDispMax",nmbComb)"""

def controTensRecElastico3d():
  '''Code to execute in every commit to check stress criterion (bars in 3D problems).'''
  return """nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
Area= self.sectionProperties.A
Wyel= self.getProp("Wyel")
Wzel= self.getProp("Wzel")
AreaQy= self.getProp("AreaQy")
AreaQz= self.getProp("AreaQz")
fyd= self.getProp("fyd")
fydV= self.getProp("fydV")
Sg= self.getN1/self.sectionProperties.A+self.getMy1/Wyel+self.getMz1/Wzel
if(Sg >self.getProp("SgMax")):
   self.setProp("SgMax",Sg)
   self.setProp("FCTN",abs(self.getProp("SgMax"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN1)
   self.setProp("MyCP",self.getMy1)
   self.setProp("MzCP",self.getMz1)
   self.setProp("HIPCPTN",nmbComb)

if(Sg < self.getProp("SgMin")):
   self.setProp("SgMin",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMin"))/fyd)
if(self.getProp("FCTN")>self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN1)
   self.setProp("MyCP",self.getMy1)
   self.setProp("MzCP",self.getMz1)
   self.setProp("HIPCPTN",nmbComb)

Sg= self.getN1/Area+self.getMy1/Wyel-self.getMz1/Wzel

if(Sg > self.getProp("SgMax")):
   self.setProp("SgMax",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMax"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN1)
   self.setProp("MyCP",self.getMy1)
   self.setProp("MzCP",self.getMz1)
   self.setProp("HIPCPTN",nmbComb)

if(Sg < self.getProp("SgMin")):
   self.setProp("SgMin",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMin"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN1)
   self.setProp("MyCP",self.getMy1)
   self.setProp("MzCP",self.getMz1)
   self.setProp("HIPCPTN",nmbComb)

Sg= self.getN1/Area-self.getMy1/Wyel+self.getMz1/Wzel

if(Sg > self.getProp("SgMax")):
   self.setProp("SgMax",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMax"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN1)
   self.setProp("MyCP",self.getMy1)
   self.setProp("MzCP",self.getMz1)
   self.setProp("HIPCPTN",nmbComb)

if(Sg < self.getProp("SgMin")):
   self.setProp("SgMin",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMin"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN1)
   self.setProp("MyCP",self.getMy1)
   self.setProp("MzCP",self.getMz1)
   self.setProp("HIPCPTN",nmbComb)

Sg= self.getN1/Area-self.getMy1/Wyel-self.getMz1/Wzel

if(Sg > self.getProp("SgMax")):
   self.setProp("SgMax",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMax"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN1)
   self.setProp("MyCP",self.getMy1)
   self.setProp("MzCP",self.getMz1)
   self.setProp("HIPCPTN",nmbComb)

if(Sg < self.getProp("SgMin")):
   self.setProp("SgMin",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMin"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN1)
   self.setProp("MyCP",self.getMy1)
   self.setProp("MzCP",self.getMz1)
   self.setProp("HIPCPTN",nmbComb)


Sg= self.getN2/Area+self.getMy2/Wyel+self.getMz2/Wzel

if(Sg > self.getProp("SgMax")):
   self.setProp("SgMax",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMax"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN2)
   self.setProp("MyCP",self.getMy2)
   self.setProp("MzCP",self.getMz2)
   self.setProp("HIPCPTN",nmbComb)

if(Sg < self.getProp("SgMin")):
   self.setProp("SgMin",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMin"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN2)
   self.setProp("MyCP",self.getMy2)
   self.setProp("MzCP",self.getMz2)
   self.setProp("HIPCPTN",nmbComb)

Sg= self.getN2/Area+self.getMy2/Wyel-self.getMz2/Wzel

if(Sg > self.getProp("SgMax")):
   self.setProp("SgMax",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMax"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN2)
   self.setProp("MyCP",self.getMy2)
   self.setProp("MzCP",self.getMz2)
   self.setProp("HIPCPTN",nmbComb)

if(Sg < self.getProp("SgMin")):
   self.setProp("SgMin",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMin"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN2)
   self.setProp("MyCP",self.getMy2)
   self.setProp("MzCP",self.getMz2)
   self.setProp("HIPCPTN",nmbComb)

Sg= self.getN2/Area-self.getMy2/Wyel+self.getMz2/Wzel

if(Sg > self.getProp("SgMax")):
   self.setProp("SgMax",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMax"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN2)
   self.setProp("MyCP",self.getMy2)
   self.setProp("MzCP",self.getMz2)
   self.setProp("HIPCPTN",nmbComb)

if(Sg < self.getProp("SgMin")):
   self.setProp("SgMin",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMin"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN2)
   self.setProp("MyCP",self.getMy2)
   self.setProp("MzCP",self.getMz2)
   self.setProp("HIPCPTN",nmbComb)

Sg= self.getN2/Area-self.getMy2/Wyel-self.getMz2/Wzel

if(Sg > self.getProp("SgMax")):
   self.setProp("SgMax",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMax"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN2)
   self.setProp("MyCP",self.getMy2)
   self.setProp("MzCP",self.getMz2)
   self.setProp("HIPCPTN",nmbComb)

if(Sg < self.getProp("SgMin")):
   self.setProp("SgMin",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMin"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN2)
   self.setProp("MyCP",self.getMy2)
   self.setProp("MzCP",self.getMz2)
   self.setProp("HIPCPTN",nmbComb)

Tau= max(abs(self.getVy())/AreaQy,abs(self.getVz())/AreaQz)
if(Tau > self.getProp("TauMax")):
   self.setProp("TauMax",Tau )
   self.setProp("FCV",self.getProp("TauMax")/fydV)
if(self.getProp("FCV") > self.getProp("FCVCP")):
   self.setProp("FCVCP",self.getProp("FCV"))
   self.setProp("VyCP",self.getVy())
   self.setProp("VzCP",self.getVz())
   self.setProp("HIPCPV",nmbComb)"""


def controTensRecElastico2d():
  '''Code to execute in every commit to check stress criterion (bars in plane problems).'''
  return """nmbComb= recorder.getCurrentCombinationName
self.getResistingForce()
Area= self.sectionProperties.A
Wel= self.getProp("Wel")
Sg= self.getN1/Area+self.getM1/Wel
if(Sg >self.getProp("SgMax")):
   self.setProp("SgMax",Sg)
   self.setProp("FCTN",abs(self.getProp("SgMax"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN1)
   self.setProp("MzCP",self.getM1)
   self.setProp("HIPCPTN",nmbComb)

if(Sg < self.getProp("SgMin")):
   self.setProp("SgMin",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMin"))/fyd)
if(self.getProp("FCTN")>self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN1)
   self.setProp("MzCP",self.getM1)
   self.setProp("HIPCPTN",nmbComb)

Sg= self.getN1/Area-self.getM1/Wel

if(Sg > self.getProp("SgMax")):
   self.setProp("SgMax",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMax"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN1)
   self.setProp("MzCP",self.getM1)
   self.setProp("HIPCPTN",nmbComb)

if(Sg < self.getProp("SgMin")):
   self.setProp("SgMin",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMin"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN1)
   self.setProp("MzCP",self.getM1)
   self.setProp("HIPCPTN",nmbComb)

Sg= self.getN2/Area+self.getM2/Wel

if(Sg > self.getProp("SgMax")):
   self.setProp("SgMax",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMax"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN2)
   self.setProp("MzCP",self.getM2)
   self.setProp("HIPCPTN",nmbComb)

if(Sg < self.getProp("SgMin")):
   self.setProp("SgMin",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMin"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN2)
   self.setProp("MzCP",self.getM2)
   self.setProp("HIPCPTN",nmbComb)

Sg= self.getN2/Area-self.getM2/Wel

if(Sg > self.getProp("SgMax")):
   self.setProp("SgMax",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMax"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN2)
   self.setProp("MzCP",self.getM2)
   self.setProp("HIPCPTN",nmbComb)

if(Sg < self.getProp("SgMin")):
   self.setProp("SgMin",Sg )
   self.setProp("FCTN",abs(self.getProp("SgMin"))/fyd)
if(self.getProp("FCTN") > self.getProp("FCTNCP")):
   self.setProp("FCTNCP",self.getProp("FCTN"))
   self.setProp("NCP",self.getN2)
   self.setProp("MzCP",self.getM2)
   self.setProp("HIPCPTN",nmbComb)

Tau= abs(self.getV())/Area
if(Tau > self.getProp("TauMax")):
   self.setProp("TauMax",Tau )
   self.setProp("FCV",self.getProp("TauMax")/fydV)
if(self.getProp("FCV") > self.getProp("FCVCP")):
   self.setProp("FCVCP",self.getProp("FCV"))
   self.setProp("VyCP",self.getV())
   self.setProp("HIPCPV",nmbComb)"""
