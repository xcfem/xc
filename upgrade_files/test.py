***23/03/2015 Change of reinforcement'  names:
* in files:
  -tests/materials/xLamina/test_xlamina_eluTN.py
  -materials/fiber_section/defSeccionHASimple.py
  -sectionsDef.py (specific of each project)
*substitution of: 
  -setMainReinf2pos by setMainReinf2pos
  -setMainReinf2neg by setMainReinf2neg
  -setMainReinf1pos by setMainReinf1pos
  -setMainReinf1neg by setMainReinf1neg

*** 24/03/2015 Revision of shear calculations according to SIA262
* files modified:
  -materials/sia262/shearSIA262.py
  -rough_calculations/ng_rebar_def.py
*Cause of revision:
Shear calculations failed in situations of simple tensile or compression stress 
(M=0, Q=0)
* modifications:
  -Shear capacity of sections without shear reinforcement is calculated considering the axial force. To that end, it's applied the simplified method proposed in the document «Dalles sans étriers soumises à l'effort tranchant» by Aurelio Muttoni.
  -In function trataResultsCombV FCtmp>e.getProp("FCCP") is substituted by FCtmp>=e.getProp("FCCP") for taking that value when FCCP is constant
