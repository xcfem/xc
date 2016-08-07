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
<hook NAME="AutomaticEdgeColor" COUNTER="5"/>
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
<node TEXT="C++ code" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_804779549" CREATED="1470579151193" MODIFIED="1470579601517" LINK="https:/github.com/lcpt/xc/tree/master/src/material"/>
<node TEXT="Python scripts" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_1477192860" CREATED="1470579726272" MODIFIED="1470579872959" LINK="https:/github.com/lcpt/xc/tree/master/python_modules/materials"><richcontent TYPE="DETAILS">

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
</node>
<node TEXT="C++ code" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_1347797926" CREATED="1470576254653" MODIFIED="1470578203318" LINK="https:/github.com/lcpt/xc/tree/master/src/preprocessor"/>
<node TEXT="Python scripts" LOCALIZED_STYLE_REF="styles.subsubtopic" ID="ID_1729185159" CREATED="1470578181678" MODIFIED="1470579653216" LINK="https:/github.com/lcpt/xc/tree/master/python_modules/model"><richcontent TYPE="DETAILS">

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
<node TEXT="Post processor" POSITION="left" ID="ID_295664252" CREATED="1470573493363" MODIFIED="1470575386916">
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
</node>
</node>
</map>
