<map version="freeplane 1.3.0">
<!--To view this file, download free mind mapping software Freeplane from http://freeplane.sourceforge.net -->
<node TEXT="XC FEA" LOCALIZED_STYLE_REF="AutomaticLayout.level.root" ID="ID_1723255651" CREATED="1283093380553" MODIFIED="1470578034694"><hook NAME="MapStyle">
    <properties show_icon_for_attributes="true" show_note_icons="true"/>

<map_styles>
<stylenode LOCALIZED_TEXT="styles.root_node">
<stylenode LOCALIZED_TEXT="styles.predefined" POSITION="right">
<stylenode LOCALIZED_TEXT="default" MAX_WIDTH="600" COLOR="#000000" STYLE="as_parent">
<font NAME="SansSerif" SIZE="10" BOLD="false" ITALIC="false"/>
</stylenode>
<stylenode LOCALIZED_TEXT="defaultstyle.details"/>
<stylenode LOCALIZED_TEXT="defaultstyle.note"/>
<stylenode LOCALIZED_TEXT="defaultstyle.floating">
<edge STYLE="hide_edge"/>
<cloud COLOR="#f0f0f0" SHAPE="ROUND_RECT"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.user-defined" POSITION="right">
<stylenode LOCALIZED_TEXT="styles.topic" COLOR="#18898b" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subtopic" COLOR="#cc3300" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subsubtopic" COLOR="#669900">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.important">
<icon BUILTIN="yes"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.AutomaticLayout" POSITION="right">
<stylenode LOCALIZED_TEXT="AutomaticLayout.level.root" COLOR="#000000">
<font SIZE="18"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,1" COLOR="#0033ff">
<font SIZE="16"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,2" COLOR="#00b439">
<font SIZE="14"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,3" COLOR="#990000">
<font SIZE="12"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,4" COLOR="#111111">
<font SIZE="10"/>
</stylenode>
</stylenode>
</stylenode>
</map_styles>
</hook>
<hook NAME="AutomaticEdgeColor" COUNTER="8"/>
<richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      Finite element analysis tool
    </p>
  </body>
</html>
</richcontent>
<node TEXT="Pre processor" POSITION="right" ID="ID_1736638943" CREATED="1470573320317" MODIFIED="1470576196546" LINK="https:/en.wikiversity.org/wiki/Introduction_to_Preprocessing">
<edge COLOR="#ff0000"/>
<richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      Tools that generate the FE model
    </p>
  </body>
</html>
</richcontent>
<node TEXT="Material models" ID="ID_1037702381" CREATED="1470575453854" MODIFIED="1470577918918" LINK="https:/en.wikipedia.org/wiki/Constitutive_equation%23Mechanical_properties_of_matter"><richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      Material constitutive equations
    </p>
  </body>
</html>
</richcontent>
<node TEXT="C++ code" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_804779549" CREATED="1470579151193" MODIFIED="1470579601517" LINK="https:/github.com/xcfem/xc/tree/master/src/material"/>
<node TEXT="Python scripts" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_1477192860" CREATED="1470579726272" MODIFIED="1470579872959" LINK="https:/github.com/xcfem/xc/tree/master/python_modules/materials"><richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      Scripts to deal with material models.
    </p>
  </body>
</html>
</richcontent>
</node>
<node TEXT="Uniaxial materials" ID="ID_170172386" CREATED="1470583135145" MODIFIED="1470583172722"><richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      Uniaxial stress-strain (or force-deformation) relationships.
    </p>
  </body>
</html>

</richcontent>
<node TEXT="C++ code" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_1414668606" CREATED="1470583280717" MODIFIED="1470583352129" LINK="https://github.com/xcfem/xc/tree/master/src/material/uniaxial"/>
</node>
<node TEXT="nD material" ID="ID_1002993846" CREATED="1470583876962" MODIFIED="1470583917491"><richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      stress-strain relationship at the gauss-point of a continuum element.
    </p>
  </body>
</html>

</richcontent>
<node TEXT="C++ code" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_425845149" CREATED="1470583927451" MODIFIED="1470583998442" LINK="https://github.com/xcfem/xc/tree/master/src/material/nD"/>
</node>
<node TEXT="Section materials" ID="ID_837560432" CREATED="1470584746848" MODIFIED="1470584825617"><richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      Cross-section generalized stress-strain relationships for Gauss points in beam-column and shell elements.
    </p>
  </body>
</html>

</richcontent>
<node TEXT="C++ code" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_975547562" CREATED="1470584838565" MODIFIED="1470584877452" LINK="https://github.com/xcfem/xc/tree/master/src/material/section"/>
</node>
</node>
<node TEXT="C++ code" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_1347797926" CREATED="1470576254653" MODIFIED="1470578203318" LINK="https:/github.com/xcfem/xc/tree/master/src/preprocessor"/>
<node TEXT="Python scripts" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_1729185159" CREATED="1470578181678" MODIFIED="1470579653216" LINK="https:/github.com/xcfem/xc/tree/master/python_modules/model"><richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      Using C++ code through (boost.)Python API.
    </p>
  </body>
</html>
</richcontent>
</node>
</node>
<node TEXT="Post processor" POSITION="left" ID="ID_295664252" CREATED="1470573493363" MODIFIED="1470582973273" LINK="https://en.wikipedia.org/wiki/Finite_element_model_data_post-processing">
<edge COLOR="#0000ff"/>
<richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      Obtaining useful information from the miriad of results
    </p>
  </body>
</html>
</richcontent>
<node TEXT="C++ code" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_785963618" CREATED="1470582655205" MODIFIED="1470582798279" LINK="https://github.com/xcfem/xc/tree/master/src/post_process"/>
<node TEXT="Python scripts" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_700936885" CREATED="1470582722763" MODIFIED="1470582798285" LINK="https://github.com/xcfem/xc/tree/master/python_modules/postprocess"/>
</node>
<node TEXT="Processor" POSITION="right" ID="ID_1348740801" CREATED="1470574975648" MODIFIED="1470575187425">
<edge COLOR="#00ff00"/>
<richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      Solution procedures ((non)linear, static, dynamic,...)
    </p>
  </body>
</html>
</richcontent>
<node TEXT="C++ code" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_958710547" CREATED="1470582172580" MODIFIED="1470582413659" LINK="https://github.com/xcfem/xc/tree/master/src/solution"/>
<node TEXT="Python scripts" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_1310532119" CREATED="1470582272628" MODIFIED="1470582413668" LINK="https://github.com/xcfem/xc/tree/master/python_modules/solution"><richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      Scripts to define solution procedures.
    </p>
  </body>
</html>

</richcontent>
</node>
</node>
<node TEXT="Documentation" POSITION="left" ID="ID_862942330" CREATED="1470581838074" MODIFIED="1470582806560" LINK="https:/github.com/xcfem/xc/tree/master/doc">
<edge COLOR="#ffff00"/>
<richcontent TYPE="DETAILS">

<html>
  <head>
    
  </head>
  <body>
    <p>
      In progress...
    </p>
  </body>
</html>

</richcontent>
</node>
<node TEXT="C++ code" LOCALIZED_STYLE_REF="styles.subsubtopic" POSITION="right" ID="ID_1970018318" CREATED="1470582429611" MODIFIED="1470582480237" LINK="https://github.com/xcfem/xc/tree/master/src">
<edge COLOR="#7c0000"/>
</node>
<node TEXT="Python scripts" LOCALIZED_STYLE_REF="styles.subsubtopic" POSITION="left" ID="ID_946320412" CREATED="1470582486921" MODIFIED="1470582798290" LINK="https://github.com/xcfem/xc/tree/master/python_modules">
<edge COLOR="#00007c"/>
</node>
</node>
</map>
