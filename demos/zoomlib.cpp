/* ***********************************************************************
 *
 *  zoomlib.cpp (part of udemo)
 *  A simple example of semantic zooming
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ************************************************************************/

#include <iostream>
#include <cmath>
#include "zoomlib.hpp"
using namespace std;
using namespace ubit;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ZoomLib::ZoomLib() {
  //const char* s = 1 + "to";
  
  zpane.addAttr(UBackground::white);
  zpane.openMenuOn(UMouseEvent::LeftButton | UMouseEvent::RightButton);
  //zpane.viewportScale() = 0.85;
  zpane.viewportPos().set(125, 75);
  zpane.viewport().add(makeShelves());
  
  status = "Press the mouse in the canvas to open the Control menu";
  
  add(UOrient::vertical + uvflex()
      + uhflex() + zpane
      + ubottom()
      + ulabel(uhcenter() + UColor::navy + UFont::large + status)
     );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// NB: fixed cell size that does not depend on content
// (as we dont want to load content if it is not shown)

class ZCell : public UBox {  //UVbox
public:
  ZCell(float w, float h, int z,
        const UArgs& cellprops, const UArgs& title, const UArgs& content);
  void load();
private:
  bool is_loaded;
  UBox children;  //UVbox
  int z;
  double zscale;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static const float ZOOM_QUANTUM = 1.10;

ZCell::ZCell(float w, float h, int z, const UArgs& props,
             const UArgs& title, const UArgs& content)
: is_loaded(false)
{
  static UBackground& back = UBackground::lightgrey;
  this->z = z;
  this->zscale = pow(ZOOM_QUANTUM, z);
    
  children.addAttr(UOrient::vertical);
  addAttr(UOrient::vertical);
  add(back
     // load when the view is actually painted
     + UOn::paint / ucall(this, &ZCell::load) 

     // fixed cell size
     + usize(w*120, h*65)

     // vertically centered
     + uvcenter()

     // props: ecrase eventuellement les props precedentes si redefinies
     + props

     // title: only displayed in range (0, zscale(
     + uinscale(0., zscale-0.0001) / uelem(UBorder::line + UFont::bold + uhcenter() + title) 
     );
  
  children.add(content);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ZCell::load() {
  if (is_loaded) return;
  is_loaded = true;

  // content: only displayed in range (zscale, oo(
  add(uinscale(zscale, 10000) / children);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UBox& book(const char* isbn, const char* title) {
  UStr ima_name = isbn; ima_name.append(".jpg");
  return uhbox(uscale(.33)
               + uinscale(1.3,10000) / uelem(uscale(.15) + uima(ima_name) + "    ")
               + title);
}

UBox& book(const char* isbn, const char* dim, const char* title,
           const char* aut, const char*num) {
  return book(isbn, title);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define NONE UArgs::none
#define BORDER UBorder::line
#define MARGINS upadding(4,4)

UBox& ZoomLib::makeShelves() {
  UBox& subpart = uhbox
    (uhspacing(5)
     + makeShelves1()
     + new ZCell( 2, 5, 0, NONE, uscale(2.) + "Programmation",
                  uhbox(uhspacing(5) 
                        + makeShelves2()
                        + makeShelves3()
                        )
                  )
     + makeShelves4()
     );
  return subpart;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UBox& ZoomLib::makeShelves1() {
  UBox& s1 = *new ZCell
    (1, 1, 8, BORDER+MARGINS, ustr("Intelligence")+"artificielle",
     uscale(0.5)
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     );

  UBox& s2 = *new ZCell
    (1, 1, 0, BORDER+MARGINS, "Compil #1",
     uscale(0.5)
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     );

  UBox& s3 = *new ZCell
    (1, 1, 0, BORDER+MARGINS, "Compil #2",
     uscale(0.5)
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     );
  
  UBox& s4 = *new ZCell
    (1, 1, 8, BORDER+MARGINS, "IHM",
     uscale(0.5)
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     );

  UBox& s5 = *new ZCell
    (1, 1, 8, BORDER+MARGINS, "GÈnie logiciel",
     uscale(0.5)
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     + ubutton("xxxxx")
     );
  // -------------------------------------------------

  UBox& s2_3 = *new ZCell
    (1, 2, 10, UArgs::none, uscale(2.) + ustr("Compilation"),
     s2 + s3
     );

    UBox& s1_2_3_4 = *new ZCell
    (1, 4, 6, UArgs::none, ustr("Programmation"),
     s1+ s2_3 + s4 + s5
     );
  
    return uvbox(uvspacing(5) +  s1_2_3_4 + s5);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UBox& ZoomLib::makeShelves2()
{
  UBox& shelf1 = *new ZCell
    (1, 1, 8, BORDER+MARGINS, "Lisp & Scheme",
     uscale(1.)
     + book("089791368X" , "0.019 0.279 0.215","Lisp and Functional Programming : Proceedings of the 1990 ACM Conference on...","ACM","2")
     + book("0897914813", "0.019 0.279 0.215","1992 ACM conference on LISP and functionnal programming : proceedings","ACM","1")
     + book("007001115X" , "0.025 0.24 0.168","Anatomy of LISP","ALL","1")
     + book("0262560380" , "0.012 0.227 0.178","little LISPer","FRI","1")
     + book("0201175894" , "0.015372 0.23184 0.158004","Object-oriented programming in common LISP : a programmers's guide to CLOS","KEE","1")
     + book("2729605495", "0.019 0.279 0.215","langages LISP","QUE","1")
     + book("0262192888" , "0.034 0.234 0.21" ,"Scheme and the art of programming","SPR","2")
     + book("1555580416" , "0.048132 0.234108 0.1638","Common Lisp : the language","STE","2")
     + book("0262700409" , "0.023 0.277 0.216","PC Scheme user's guide and language reference manual","TEX","12")
     );
  
  // -------------------------------------------------

  UBox& cpp_a = *new ZCell
    (1, 1, 0, BORDER+MARGINS, "C++ #1",
     uscale(0.9)
     + book("2866015061"," programmation C et C++")
     + book("2212087179","Exercices en langage C++ : programmation orientÈe objet")
     + book("2212087748","Programmer en langage C++")
     + book("2225827052","Ingénierie des objets : Approche classe-relation application à C++")
     + book("2729606572","bibliotheque standard STL du C++")
     + book("2100000098","Conception et programmation orientees objet en C++")
     + book("0471160040","Programming with DirectToSom C++")
     + book("2866611055","Toute la puissance de C++ en quarante leçons")
     + book("1572315652","Inside visual C++")
     + book("2879080029","essentiel du C++")
     );
  
  UBox& cpp_b = *new ZCell
    (1, 1, 0, BORDER+MARGINS, "C++ #2",
     uscale(0.9)
     + book("2879080487" , "0.016 0.24 0.174","C++ efficace : 50 règles pour améliorer la conception de vos programmes","MEY","1")
     + book("1572315202" , "0.055692 0.226296 0.18648","Microsoft Visual C++ run-time library reference","MIC","1")
     + book("1572315210" , "0.048636 0.226296 0.186984","Microsoft Visual C++ language reference","MIC","1")
     + book("0201633981" , "0.033012 0.239652 0.191016","STL tutorial and reference Guide : C++ programming with the standard template library","MUS","9")
     + book("0131170031" , "0.022 0.233 0.178","draft standard C++ librairy","PLA","1")
     + book("2212089171" , "0.021 0.24 0.17","qualitÈ en C++ : regles de codage - patterns de programmation - fonctionnnement du compilateur","PRA","2")
     + book("0201889544" , "0.03276 0.237636 0.18522","C++ programming language","STR","2")
     + book("2729602941" , "0.022 0.23 0.16","langage C++","STR","2")
     + book("2736107659" , "0.024 0.185 0.114","Borland C++ : concepts fondamentaux","TER","2")
     + book("2736109201" , "0.018 0.185 0.114","Borland C++ : programmation orientÈe objet","TER","2")
     + book("1572315105" , "0.049392 0.236376 0.189504","Microsoft Visual C++ owner's manual, version 5.0","ZAR","1")
     );

  UBox& shelf2 = *new ZCell
    (1, 2, 8, NONE, uscale(2.0) + "C++",
     cpp_a + cpp_b
     );
  
  // -------------------------------------------------

  UBox& prolog = *new ZCell
    (.6, 1, 8, BORDER+MARGINS+UBackground::wheat, "Prolog",
     uscale(0.9)
	+ book("0201416069" , "0.03 0.232 0.171","Prolog : programming for artificial intelligence","BRA","2")
     + book("2729600760" , "0.018 0.229 0.159","Prolog","GIA","2")
     + book("0262150395" , "0.02772 0.236124 0.184212","craft of Prolog","KEE","1")
     + book("0262691639" , "0.034 0.229 0.2","The art of Prolog : advanced programming techniques","STE","1")
     + book("2225819483" , "0.031 0.247 0.18","art de Prolog","STE","1")
     + book("0201524244" , "0.026 0.24 0.168","Knowledge systems and prolog : developping expert, database and natural language systems","WAL","1")
     );

  UBox& pascal = *new ZCell
    (.4, 1, 10, BORDER+MARGINS+UBackground::lightblue, 
     uscale(0.5) + "Pascal et C",
     uscale(0.9) + UColor::blue 
     + book("2225822921" , "0.027 0.24 0.16","MÈthodologie de la programmation en langage C : principes et applications","BRA","1")
     + book("2729890580" , "0.011 0.24 0.162","Elements de genie logiciel : outil de developpement en C sous UNIX","CHA","1")
     + book("2212089864" , "0.019 0.2 0.145","Programmer en Turbo Pascal 7","DEL","1")
     + book("2225823014" , "0.012 0.24 0.16","langage C : problËmes et exercices","FEU","1")
     + book("2225820708" , "0.017 0.24 0.16","langage C","KER","5")
     + book("020119578X" , "0.016 0.235 0.18","Langage C du dÈbut au standard ANSI","MAZ","2")
     );
  UBox& shelf3 = uhbox(utop() + uleft() + prolog + pascal);
  
  // -------------------------------------------------

  UBox& fortran = *new ZCell
    (.40, 1, 8, BORDER+MARGINS, uscale(0.5) + "Fortran",
     uscale(0.8) + UColor::green
     + book("2903607117" , "0.015 0.22 0.155","pratique du Fortran 77 : 77 exercices resolus de Fortran V","LIG","1")
     + book("2903607311" , "0.008 0.24 0.165","FORTRAN 7 : Langage fortran V","LIG","1")
     + book("2903607699" , "0.008 0.24 0.165","Fortran 77","LIG","1")
     + book("2704210136" , "0.016 0.27 0.21","Programation Fortran : theorie et applications - 375 problemes resolus","LIP","1")
     + book("0198537727" , "0.017 0.233 0.155","Fortran 90 explained","MET","6")
     + book("052143064X" , "0.042588 0.249984 0.16884","Numerical recipes in Fortran : the art of scientific computing","PRE","1")
     );

  UBox& smalltalk = *new ZCell
    (.60, 1, 8, BORDER+MARGINS, "Smalltalk",
     uscale(0.8) + UColor::orange
     + book("013476904X" , "0.017892 0.234108 0.176652","Smalltalk best practice patterns","BEC","5")
     + book("0201136880" , "0.024444 0.231588 0.184716","Smalltalk - 80 : the language","GOL","3")
     + book("0133183874" , "0.03 0.233 0.173","Smalltalk : an introduction to application development using visualworks","HOP","7")
     + book("1884842119" , "0.033 0.228 0.178","The Smalltalk developer's guide to visualworks","HOW","1")
     + book("0134684141","0.019 0.279 0.215","Inside Smalltalk","LAL","2")
     + book("0133713458" , "0.016 0.233 0.175","The art and science of Smalltalk : An introduction to object-oriented programming using visualworks","LEW","2")
     + book("1884777279","0.019 0.279 0.215","Smalltalk, objects and design","LIU","1")
     + book("0131655493" , "0.008568 0.231336 0.177156","Smalltalk with style","SKU","3")
     + book("080530908X" , "0.024 0.233 0.187","IBM smalltalk : the language","SMI","2")
     );
  UBox& shelf4 = uhbox(utop() + uleft() + fortran + smalltalk);

  // -------------------------------------------------
       
  return *new ZCell
    (1, 5, 3, NONE, "Languages",
     uvspacing(5) + shelf1 + shelf2 + shelf3 + shelf4
     );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UBox& ZoomLib::makeShelves3() {
  UBox& s1 = *new ZCell
    (1, 1, 10, BORDER+MARGINS, ustr("Programmation")+"logique",
     uscale(0.75)
     + book("1860940234" , "0.028 0.222 0.162","Coordination programming, mechanisms, models and semantics","AND","1")
     + book("2705655441" , "0.02 0.19 0.14","Logique sans peine","CAR","1")
     + book("3540584854" , "0.024 0.235 0.154","Static analysis : proceedings","CHA","1")
     + book("3540572643" , "0.017 0.235 0.154","Static analysis : proceedings","COU","1")
     + book("0792395328" , "0.02 0.24 0.16","Logic program synthesis from incomplete information","FLE","1")
     + book("2879080622" , "0.026 0.24 0.175","Programmation par contraintes","FRO","1")
     + book("0262560585" , "0.053 0.228 0.175","Logic Programming : Proceedings of the Eight International Conference","FUR","1")
     + book("2866012496" , "0.023 0.235 0.155","Logique. Volume 1. methodes pour l'informatique fondamentale","GOC","1")
     + book("2225846421" , "0.013 0.21 0.135","Logique et mathematiques pour l'informatique et l'I.A. : 109 exercices corriges","JAC","1")
     + book("0137700091" , "0.024 0.24 0.178","Computation as logic","LAL","1")
     + book("2225821046" , "0.027 0.247 0.168","Logique, rÈduction, rÈsolution","LAL","1")
     + book("2866013808" , "0.02 0.233 0.153","Logique et fondements de l'informatique : logique du 1er ordre, calculabilitÈ et lambda-calcul","LAS","1")
     + book("2212036396","0.019 0.279 0.215","Enigmes et jeux logiques","LHO","1")
     + book("3540652248","0.019 0.279 0.215","Principles and practice of constraint programming : Proceedings","MAH","1")
     + book("0131232746" , "0.016 0.233 0.173","Programming from specifications","MOR","1")
				+ book("0521346320" , "0.022 0.25 0.175","Logic and computation : interactive proof with Cambridge LCF","PAU","1")
     + book("0201416433" , "0.016 0.233 0.171","Logic for computer science","REE","1")
     + book("0262193612" , "0.033 0.233 0.185","Principles and practice of constraint programmming : the Newport papers","SAR","1")
     + book("3540637532","0.019 0.279 0.215","Principles and practice of constraint programming : Proceedings","SMO","1")
     + book("0127016104","0.019 0.279 0.215","Foundations of constraint satisfaction","TSA","3")
     );

  UBox& s2 = *new ZCell
    (1, 1, 10, BORDER+MARGINS, ustr("Programmation")+"fonctionnelle",
     uscale(0.75)
     + book("0521416957" , "0.019908 0.253008 0.17892","Compiling with continuations","APP","1")
     + book("0134841891","0.019 0.279 0.215","Introduction to functionnal programming","BIR","1")
     + book("0134843460" , "0.02 0.233 0.174","Introduction to functional programming using Haskell","BIR","1")
     + book("0273088327" , "0.014 0.242 0.168","Lazy functional languages: abstract interpretation and compilation","BUR","1")
     + book("013192592X" , "0.015 0.233 0.174","Programming with Miranda","CLA","1")
     + book("2840741148" , "0.025 0.24 0.175","Approche fonctionnelle de la programmation","COU","1")
     + book("0521277248" , "0.019152 0.22806 0.152208","An introduction to functionnal programming systems using Haskell","DAV","2")
     + book("0201192497" , "0.035 0.24 0.158","Functional programming","FIE","1")
     + book("2225810567" , "0.013 0.036 0.158","Principes de programmation fonctionnelle","GLA","1")
     + book("0521318394" , "0.021 0.228 0.15","Introduction to combinators and (lambda)-calculus","HIN","1")
     + book("0201172348" , "0.024 0.24 0.17","Logical foundations of functional programming","HUE","1")
     + book("0201137445" , "0.028 0.24 0.167","Functional programming : practice and theory","MAC","2")
     + book("0201178125" , "0.017 0.234 0.155","An introduction to functional programming through Lambda calculus","MIC","1")
     + book("0262631326" , "0.01 0.228 0.152","The definition of standard ML","MIL","1")
     + book("020163337X" , "0.03 0.233 0.188","TCL and the TK Toolkit","OUS","3")
     + book("0521422256" , "0.027 0.246 0.174","ML for the working programmer","PAU","1")
     + book("0137219520" , "0.015 0.233 0.173","Implementing functional languages : A tutorial","PEY","2")
     + book("2225820090" , "0.022 0.238 0.158","Mise en oeuvre des langages fonctionnels de programmation","PEY","1")
     + book("0201416638" , "0.035 0.238 0.16","Functional programming and parallel graph rewriting","PLA","1")
     + book("0201129159" , "0.035 0.238 0.165","Elements of functional programming","REA","1")
     + book("1857283775" , "0.017 0.24 0.16","Applications of functional programming","RUN","1")
     + book("0201522438" , "0.022 0.238 0.165","Parallel functional languages and compilers","SZY","1")
     + book("0201403579" , "0.023 0.233 0.17","Haskell : the craft of functional programming","THO","1")
     + book("0201416670" , "0.025 0.24 0.16","Type theory and functional programming","THO","1")
     + book("0201422794" , "0.023 0.233 0.17","Miranda : the craft of functional programming","THO","1")
     + book("0077074114" , "0.016 0.225 0.148","Constructive foundations for functional languages","TUR","1")
     + book("0201172364" , "0.02 0.24 0.168","Research topics in functional programming","TUR","1")
     + book("0133316610" , "0.022 0.235 0.174","Functional programming using standard ML","WIK","1")
     );
  
  UBox& s3 = *new ZCell
    (1, 1, 8, BORDER+MARGINS, "Divers langages",
     uscale(0.75)
     + book("2729604197" , "0.033 0.23 0.157","Concepts et outils de programmation : le style fonctionnel, le style imperatif, avec Caml et Ada","ACC","1")
     + book("0201181274" , "0.048888 0.23058 0.18648","PostScript langage reference manual","ADO","1")
     + book("0201310066" , "0.020664 0.231588 0.184968","Java TM programming language","ARN","1")
     + book("284177077X" , "0.043 0.238 0.174","Perl en action : exemples et solutions pour les programmeurs en Perl","CHR","1")
     + book("0333395360" , "0.014 0.233 0.155","Program design using JSP : a practical introduction","KIN","1")
     + book("3540760334" , "0.014 0.235 0.155","B language and method : a guide to practical formal development","LAN","1")
     + book("2736100603","0.019 0.279 0.215","Introduction a ADA","LEB","1")
     + book("222577143X" , "0.013 0.24 0.16","ADA une introduction","LED","1")
     + book("020163452X","0.019 0.279 0.215","Java TM virtual machine specification","LIN","1")
     + book("0132479257" , "0.029988 0.231588 0.188496","Eiffel : the language","MEY","1")
     + book("0136780121" , "0.0315 0.242172 0.184464","Programming languages : design and implementation","PRA","1")
     + book("0387963995" , "0.028 0.24 0.162","Programming with sets : an introduction to SETL","SCH","1")
     + book("1565920422" , "0.015 0.228 0.151","Learning Perl","SCH","1")
     + book("2841770052" , "0.016 0.24 0.173","Introduction a Perl","SCH","1")
     + book("2841770397","0.019 0.279 0.215","Programmation avancee en Perl","SRI","1")
     + book("0937175641","0.019 0.279 0.215","Programming Perl","WAL","1")
     + book("2841770044" , "0.025 0.24 0.175","Programmation en PERL","WAL","1")
     + book("2729606394" , "0.022 0.24 0.16","langage Caml","WEI","4")
     + book("0262731037","0.019 0.279 0.215","formal semantics of programing languages : an introduction","WIN","1")
     );
  
  // -------------------------------------------------
  
  UBox& s4 = *new ZCell
    (1, 1, 0, BORDER+MARGINS, "Java #1",
     uscale(0.75)
     + book("3540661581","0.019 0.279 0.215","Formal syntax and semantics of java","ALV","1")
     + book("0201634554","0.019 0.279 0.215","Java programming language","ARN","1")
     + book("2841809943" , "0.018 0.24 0.178","langage Java","ARN","2")
     + book("2212090536","0.019 0.279 0.215","Java embarque : a la conquete des applications industrielles et de l'electronique grand public","BOS","1")
     + book("0201379635" , "0.024948 0.230832 0.18522","Inside servlets : server-side programming for the java platform","CAL","1")
     + book("0201310074" , "0.047124 0.232344 0.186732","Java tutorial : object-oriented programming for the Internet","CAM","2")
     + book("0201634546" , "0.032 0.233 0.186","Java tutorial : object-oriented programming for the Internet","CAM","1")
     + book("0201310023" , "0.062244 0.239904 0.196308","Java class libraries, volume 1 : Java.io, java.lang, java.math, java.net, java.txt, java.util","CHA","1")
					+ book("0201310031" , "0.052416 0.24192 0.19656","Java class libraries, volume 2 : Java.applet, java.awt, java.beans","CHA","1")
     + book("0201379678" , "0.043344 0.2016 0.139608","Java developpers almanac 1998","CHA","4")
     + book("2225834164","0.019 0.279 0.215","Java, la synthËse : vers la maturitÈ avec Java 2","CLA","1")
     + book("3540760520" , "0.012096 0.23562 0.155232","Essential Java fast : how to write object oriented software for the internet","COW","1")
					+ book("0130125075" , "0.043092 0.230328 0.177408","Java : how to program","DEI","1")
     + book("013014469X" , "0.05418 0.232848 0.176904","Core jini","EDW","1")
     );

  UBox& s5 = *new ZCell
    (1, 1, 0, BORDER+MARGINS, "Java #2",
     uscale(0.75)
     + book("156592262X" , "0.032004 0.2268 0.151956","Java in a nutshell : a desktop quick reference","FLA","23")
     + book("2841770095" , "0.025 0.24 0.172","Java in a nutshell : manuel de reference pour Java 1.0","FLA","2")
     + book("3540762019" , "0.028 0.233 0.155","Java and object orientation : an introduction","HUN","1")
     + book("0201695812" , "0.018648 0.232848 0.18774","Concurrent programming in JAVA : design principles and patterns","LEA","4")
     + book("067231438X" , "0.062496 0.237888 0.194796","Teach yourself Java 2 platform in 21 days","LEM","1")
     + book("047131952X" , "0.020412 0.230832 0.18774","Securing java : getting down to business with mobile code","MCG","2")
     + book("1565926056" , "0.021672 0.232092 0.17766","Enterprise javabeans","MON","1")
     + book("0471254061" , "0.038304 0.231084 0.187488","Programming mobile objects with Java","NEL","2")
     + book("0471297887" , "0.02 0.232 0.19","Developing JavaBeans using VisualAge for Java","NIL","3")
     + book("007882477X" , "0.03528 0.234864 0.184968","Javabeans programming from the ground up","ONE","1")
     + book("1576100561" , "0.035 0.235 0.19","Java database programming with JDBC","PAT","2")
     + book("0782120970" , "0.053424 0.229824 0.191016","Mastering JavaBeans","VAN","1")
     + book("0471247650" , "0.02898 0.230832 0.188244", "JAVA programming with CORBA : advanced techniques for building distributed applications","VOG","1")
     );
  
  UBox& s4_5 = *new ZCell
    (1, 2, 8, UArgs::none, uscale(2.) + "Java",
     s4 + s5
     );
  
  UBox& s3_4_5 = *new ZCell
    (1, 3, 3, UArgs::none, uscale(2.) + "Langages",
     s3 + s4_5
     );
  
  // -------------------------------------------------
       
  return *new ZCell
    (1, 5, 0, NONE, "Languages3",
     uvspacing(5) + s1 + s2 + s3_4_5
     );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UBox& ZoomLib::makeShelves4() {
  UBox& s1 = *new ZCell
    (1, 1, 8, BORDER+MARGINS, "GÈnÈralitÈs",
     uscale(0.7)
     + book("1860940234" , "0.028 0.222 0.162","Coordination programming, mechanisms, models and semantics","AND","1")
     + book("2705655441" , "0.02 0.19 0.14","Logique sans peine","CAR","1")
     + book("3540584854" , "0.024 0.235 0.154","Static analysis : proceedings","CHA","1")
     + book("3540572643" , "0.017 0.235 0.154","Static analysis : proceedings","COU","1")
     + book("0792395328" , "0.02 0.24 0.16","Logic program synthesis from incomplete information","FLE","1")
     + book("2879080622" , "0.026 0.24 0.175","Programmation par contraintes","FRO","1")
     + book("0262560585" , "0.053 0.228 0.175","Logic Programming : Proceedings of the Eight International Conference","FUR","1")
     + book("2866012496" , "0.023 0.235 0.155","Logique. Volume 1. mÈthodes pour l'informatique fondamentale","GOC","1")
     + book("2225846421" , "0.013 0.21 0.135","Logique et mathÈmatiques pour l'informatique et l'I.A. : 109 exercices corrigÈs","JAC","1")
     + book("0137700091" , "0.024 0.24 0.178","Computation as logic","LAL","1")
     + book("2225821046" , "0.027 0.247 0.168","Logique, rÈduction, rÈsolution","LAL","1")
     + book("2866013808" , "0.02 0.233 0.153","Logique et fondements de l'informatique : logique du 1er ordre, calculabilitÈ et lambda-calcul","LAS","1")
     + book("2212036396","0.019 0.279 0.215","Enigmes et jeux logiques","LHO","1")
     + book("3540652248","0.019 0.279 0.215","Principles and practice of constraint programming : Proceedings","MAH","1")
     + book("0131232746" , "0.016 0.233 0.173","Programming from specifications","MOR","1")
				+ book("0521346320" , "0.022 0.25 0.175","Logic and computation : interactive proof with Cambridge LCF","PAU","1")
     + book("0201416433" , "0.016 0.233 0.171","Logic for computer science","REE","1")
     + book("0262193612" , "0.033 0.233 0.185","Principles and practice of constraint programmming : the Newport papers","SAR","1")
     + book("3540637532","0.019 0.279 0.215","Principles and practice of constraint programming : Proceedings","SMO","1")
     + book("0127016104","0.019 0.279 0.215","Foundations of constraint satisfaction","TSA","3")
     );

  UBox& s2 = *new ZCell
    (1, 1, 0, BORDER+MARGINS, "Algo #1",
     uscale(0.7)
     + book("0521416957" , "0.019908 0.253008 0.17892","Compiling with continuations","APP","1")
     + book("0134841891","0.019 0.279 0.215","Introduction to functionnal programming","BIR","1")
     + book("0134843460" , "0.02 0.233 0.174","Introduction to functional programming using Haskell","BIR","1")
     + book("0273088327" , "0.014 0.242 0.168","Lazy functional languages: abstract interpretation and compilation","BUR","1")
     + book("013192592X" , "0.015 0.233 0.174","Programming with Miranda","CLA","1")
     + book("2840741148" , "0.025 0.24 0.175","Approche fonctionnelle de la programmation","COU","1")
     + book("0521277248" , "0.019152 0.22806 0.152208","An introduction to functionnal programming systems using Haskell","DAV","2")
     + book("0201192497" , "0.035 0.24 0.158","Functional programming","FIE","1")
     + book("2225810567" , "0.013 0.036 0.158","Principes de programmation fonctionnelle","GLA","1")
				+ book("0521318394" , "0.021 0.228 0.15","Introduction to combinators and (lambda)-calculus","HIN","1")
     + book("0201172348" , "0.024 0.24 0.17","Logical foundations of functional programming","HUE","1")
     + book("0201137445" , "0.028 0.24 0.167","Functional programming : practice and theory","MAC","2")
     + book("0201178125" , "0.017 0.234 0.155","An introduction to functional programming through Lambda calculus","MIC","1")
     + book("0262631326" , "0.01 0.228 0.152","The definition of standard ML","MIL","1")
     );

  UBox& s3 = *new ZCell
    (1, 1, 0, BORDER+MARGINS, "Algo #2",
     uscale(0.7)
     + book("3540661581","0.019 0.279 0.215","Formal syntax and semantics of java","ALV","1")
     + book("0201634554","0.019 0.279 0.215","Java programming language","ARN","1")
     + book("2841809943" , "0.018 0.24 0.178","langage Java","ARN","2")
     + book("2212090536","0.019 0.279 0.215","Java embarquÈ : ‡ la conquÍte des applications industrielles et de l'Èlectronique grand public","BOS","1")
     + book("0201379635" , "0.024948 0.230832 0.18522","Inside servlets : server-side programming for the java platform","CAL","1")
     + book("0201310074" , "0.047124 0.232344 0.186732","Java tutorial : object-oriented programming for the Internet","CAM","2")
     + book("0201634546" , "0.032 0.233 0.186","Java tutorial : object-oriented programming for the Internet","CAM","1")
     + book("0201310023" , "0.062244 0.239904 0.196308","Java class libraries, volume 1 : Java.io, java.lang, java.math, java.net, java.txt, java.util","CHA","1")
					+ book("0201310031" , "0.052416 0.24192 0.19656","Java class libraries, volume 2 : Java.applet, java.awt, java.beans","CHA","1")
     + book("0201379678" , "0.043344 0.2016 0.139608","Java developpers almanac 1998","CHA","4")
     + book("2225834164","0.019 0.279 0.215","Java, la synthËse : vers la maturitÈ avec Java 2","CLA","1")
     + book("3540760520" , "0.012096 0.23562 0.155232","Essential Java fast : how to write object oriented software for the internet","COW","1")
					+ book("0130125075" , "0.043092 0.230328 0.177408","Java : how to program","DEI","1")
     + book("013014469X" , "0.05418 0.232848 0.176904","Core jini","EDW","1")
     );
  
  UBox& s4 = *new ZCell
    (1, 1, 8, BORDER+MARGINS, ustr("ThÈorie")+"des langages",
     uscale(0.7)
     + book("156592262X" , "0.032004 0.2268 0.151956","Java in a nutshell : a desktop quick reference","FLA","23")
     + book("2841770095" , "0.025 0.24 0.172","Java in a nutshell : manuel de rÈfÈrence pour Java 1.0","FLA","2")
     + book("3540762019" , "0.028 0.233 0.155","Java and object orientation : an introduction","HUN","1")
     + book("0201695812" , "0.018648 0.232848 0.18774","Concurrent programming in JAVA : design principles and patterns","LEA","4")
     + book("067231438X" , "0.062496 0.237888 0.194796","Teach yourself Java 2 platform in 21 days","LEM","1")
     + book("047131952X" , "0.020412 0.230832 0.18774","Securing java : getting down to business with mobile code","MCG","2")
     + book("1565926056" , "0.021672 0.232092 0.17766","Enterprise javabeans","MON","1")
     + book("0471254061" , "0.038304 0.231084 0.187488","Programming mobile objects with Java","NEL","2")
     + book("0471297887" , "0.02 0.232 0.19","Developing JavaBeans using VisualAge for Java","NIL","3")
     + book("007882477X" , "0.03528 0.234864 0.184968","Javabeans programming from the ground up","ONE","1")
     + book("1576100561" , "0.035 0.235 0.19","Java database programming with JDBC","PAT","2")
     + book("0782120970" , "0.053424 0.229824 0.191016","Mastering JavaBeans","VAN","1")
     );

  UBox& s5 = *new ZCell
    (1, 1, 8, BORDER+MARGINS, ustr("Programmation")+"objet",
     uscale(0.8)
     + book("0521416957" , "0.019908 0.253008 0.17892","Compiling with continuations","APP","1")
     + book("0134841891","0.019 0.279 0.215","Introduction to functionnal programming","BIR","1")
     + book("0134843460" , "0.02 0.233 0.174","Introduction to functional programming using Haskell","BIR","1")
     + book("0273088327" , "0.014 0.242 0.168","Lazy functional languages: abstract interpretation and compilation","BUR","1")
     + book("013192592X" , "0.015 0.233 0.174","Programming with Miranda","CLA","1")
     + book("2840741148" , "0.025 0.24 0.175","Approche fonctionnelle de la programmation","COU","1")
     + book("0521277248" , "0.019152 0.22806 0.152208","An introduction to functionnal programming systems using Haskell","DAV","2")
     + book("0201192497" , "0.035 0.24 0.158","Functional programming","FIE","1")
     );
  // -------------------------------------------------

  UBox& s2_3 = *new ZCell
    (1, 2, 7, UArgs::none, ustr("Algorithmique")+"et structures"+"des donnÈes",
     s2 + s3
     );

    UBox& s2_3_4_5 = *new ZCell
    (1, 4, 2, UArgs::none, ustr("Programmation"),
     s2_3 + s4 + s5
     );
  
    // -------------------------------------------------
       
    return uvbox(uvspacing(5) + s1 + s2_3_4_5);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
