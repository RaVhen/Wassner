// pour compiler : gcc -Wall -g -lm fenetre.c -lcsfml-graphics  -o fenetre
//
#include <stdio.h>
#include <SFML/Graphics.h> 

void simulation(float deltaT, float *x, float *y) {
  float vitesse = 10; // unité : pixel/secondes
  float deplacement = deltaT*vitesse; 
  *x += deplacement;
  *y += deplacement;  
}

void affichage(sfRenderWindow *fenetre, float *x, float*y) {
  // préparation des couleurs
  sfColor noir = {0,0,0,0}, blanc = {255,255,255,255}, gris = {128,128,128,128};

  // préparation du cercle
  sfShape *cercle = sfShape_CreateCircle(*x,*y,10,blanc,1,gris);
  // préparation de la ligne
  sfShape *ligne = sfShape_CreateLine(*x,*y,0,0,1,blanc,1,gris);

  // préparation du texte à afficher
  sfString *texte = sfString_Create();
  char s[10];
  sprintf(s,"%d",12);
  sfString_SetText(texte,s);
  sfString_SetPosition(texte,*x +10, *y+10);

  sfRenderWindow_Clear(fenetre,noir);// effacer l'écran  

  sfRenderWindow_DrawShape(fenetre,cercle);  // tracer le cercle
  sfRenderWindow_DrawShape(fenetre,ligne);   // tracer la ligne
  sfRenderWindow_DrawString(fenetre,texte);  // tracer le texte

  sfRenderWindow_Display(fenetre); // visualiser le contenu de la fenetre    

  // faire le ménage en mémoire
  sfShape_Destroy(cercle);
  sfShape_Destroy(ligne);
  sfString_Destroy(texte);
}

int main() {
    // initialisation graphique
    sfRenderWindow *fenetre;
    sfVideoMode mode = {640, 480, 32};
    sfWindowSettings settings = {24, 8, 0};
    fenetre = sfRenderWindow_Create(mode, "Ma fenetre",  sfResize | sfClose, settings);
    sfEvent evenement;
    
    // initialisation des données utilisé pour la simulation
    float x=0,y=0;
    float deltaT; // temps entre deux moments discrets de la simulation
    deltaT = 0.75;// doit etre fixe sinon l'algo dépend de vitesse de la machine
                  // d'exécution !! (PB de correction, surtout avec valgrind!) 
                  // Ne modifiez donc pas cette valeur!

    // Boucle principale
    while (sfRenderWindow_IsOpened(fenetre)) {
      // gestion des evenements
      sfRenderWindow_GetEvent(fenetre, &evenement);
      if (evenement.Type == sfEvtClosed) { // sortie de boucle d'animation
        sfRenderWindow_Close(fenetre);
      }                  
      // calcul de simulation
      simulation(deltaT,&x,&y); 
      // affichage
      affichage(fenetre,&x,&y);
    }
    sfRenderWindow_Destroy(fenetre);    

    return EXIT_SUCCESS;
}
