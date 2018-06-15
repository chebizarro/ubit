#include <iostream>
#include <ubit/ubit.hpp>
using namespace std;
using namespace ubit;


struct Demo : public UBox {
  Demo();
  UScrollpane spane;
};


int main(int argc, char* argv[]) {
  UAppli appli(argc, argv);
  Demo demo;

  UFrame frame(usize(400, 400) + demo);
  appli.add(frame);
  frame.show();
  
  return appli.start();
}


Demo::Demo() {

  UArgs level1 = UFont::bold + UColor::navy + UOn::select/UColor::yellow;
  
  UBox& file = utreenode
  (level1 + "Fichier",
   uitem("Biblioth�que de Projets...")
   + usepar()
   + uitem("Nouvelle pr�sentation")
   + uitem("Ouvrir...")
   + uitem("Fermer")
   + usepar()
   + uitem("Enregistrer")
   + uitem("Enregistrer sous...")
   + uitem("Enregistrer en tant que page Web...")
   + usepar()
   + uitem("Faire une vid�o")
   + usepar()
   + uitem("Aper�u de la page Web")
   + usepar()
   + uitem("Mise en page...")
   + uitem("Imprimer...")
   + usepar()
   + utreenode("Envoyer vers >") //sub
   + uitem("Propri�tes...")
   + usepar()
   );

  UBox& edit = utreenode
    (level1 + "Edition",
     uitem("Annuler")
     + uitem("R�p�ter")
     + usepar()
     + uitem("Couper")
     + uitem("Copier")
     + uitem("Copier vers l'album")
     + uitem("Coller")
     + uitem("Coller la s�lection de l'album")
     + uitem("Collage sp�cial...")
     + uitem("Coller comme lien hypertexte")
     + usepar()
     + uitem("Effacer")
     + uitem("S�lectionner tout")
     + uitem("Dupliquer")
     + uitem("Supprimer la diapositive")
     + usepar()
     + uitem("Rechercher...")
     + uitem("Remplacer...")
     + uitem("Atteindre la propri�te...")
     + usepar()
     + uitem("Objet")
     );
  
  UBox& view = utreenode
    (level1 + "Affichage",
     ucheckbox("Normal")  // check
     + ucheckbox("Diapositive") // check
     + ucheckbox("Trieuse de diapositives")  // check
     + uitem("Pages de commemtaires")
     + uitem("Outils du pr�sentateur")
     + uitem("Diaporama")
     + usepar()
     + utreenode("Masque > ")  // sub
     + uitem("Nuances de gris")
     + uitem("Diapositiev miniature")
     + usepar()
     + uitem("Bo�te � outils")
     + uitem("Palette de mise en forme")
     + utreenode("Barres d'outils >")  //sub
     + uitem("R�gle")
     + uitem("Rep�res")
     + usepar()
     + uitem("En-t�te et pied de page...")
     + uitem("Commentaires")
     + usepar()
     + uitem("Zoom...")
     );
     
  UBox& insert = utreenode
    (level1 + "Insertion",
     uitem("Nouvelle diapositive")
     + uitem("Dupliquer la diapositive")
     + usepar()
     + uitem("Num�ro de la diapositive")
     + uitem("Date et heure...")
     + uitem("Onglet")
     + uitem("Carct�res sp�ciaux...")
     + uitem("Commentaire")
     + usepar()
     + uitem("Diapositive de fichier...")
     + uitem("Diapositive de plan...")
     + usepar()
     + utreenode("Image")  // sub
     + uitem("Zone de texte")
     + utreenode("Vid�os et sons")  //sub
     + uitem("Graphique...")
     + uitem("Tableau...")
     + uitem("Objet...")
     + uitem("Lien hypertexte...")
     );

  UBox& format = utreenode
    (level1 + "Format",
     uitem("Police...")
     + uitem("Puces et num�ros...")
     + utreenode("Alignement")   //sub
     + uitem("Interligne...")
     + uitem("Changer la casse...")
     + uitem("Remplacer les polices...")
     + usepar()
     + uitem("Appliquer le mod�le...")
     + uitem("Mise en page des diapositives...")
     + uitem("Jeu de couleurs des diapositives...")
     + uitem("Arri�re plan de la diapositive...")
     + usepar()
     + uitem("Couleurs et traits..")
     + uitem("Objet...")
     );

  
  UBox& tree = utreebox
  (file
   + edit
   + view
   + insert
   + format
   + utreenode(level1 +"Outils")
   + utreenode(level1 +"Diaporama")
   + utreenode(level1 +"Fen�tres")
   + utreenode(level1 +"Aide")
   );
   
  spane.add(tree);
  spane.getVScrollbar()->setUnitIncrement(10);
  spane.getVScrollbar()->setBlockIncrement(10);

  add(uhflex() + uvflex() + spane);  
}

