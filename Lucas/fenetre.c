// pour compiler : gcc -Wall -g -lm fenetre.c -lcsfml-graphics  -o fenetre
//
#include <stdio.h>
#include <math.h>
#include <SFML/Graphics.h> 

#define MASS 1

typedef struct Node_{            // Node struct
  float posX, posY;
  float speX, speY;
  float velX, velY;
	float forX, forY;
} Node_;

typedef Node_ Node;


typedef struct Link_ {
	Node *nodeA;
	Node *nodeB;
	struct Link_ * next;
} Link_;

typedef Link_ * Link;

Link createLink(Link l, Node *nA, Node *nB){

  //DEBUG printf("AdresseA:%p  AdresseB:%p\n", nA, nB);
  if (l==NULL){
    l = malloc(sizeof(Link_));
    
    if(!l){ //malloc error
      printf("Error while mallocing a new list of links !\n");
      exit(1);
      }
    l->nodeA = nA;
    l->nodeB = nB;
    l->next = NULL;
  }
  else{
    Link tmp = l;
    while(tmp->next != NULL)
    {
      tmp = tmp->next;
    }
    
    Link newLink = malloc(sizeof(Link_));
    if(!newLink){ //malloc error
        printf("Error while mallocing a new link !\n");
        exit(1);
    }
    tmp->next = newLink;
    newLink->nodeA = nA;
    newLink->nodeB = nB;
    newLink->next = NULL;  
  }
  return l;
}


int genesis(Node **nodes, Link *links, char* filename, int* nodeCt)
{
  FILE * inputFile;
  inputFile = fopen(filename, "r");
  printf("Try to open %s\n", filename);
  
  if(inputFile==NULL)
    return(1); //error reading file !
  
  fscanf(inputFile, "%d", nodeCt); //get nodes count
  *nodes = malloc(sizeof(Node)*(*nodeCt)); //create nodes array
  
  int i;
  for(i=0;i<*nodeCt;i++){
    (*nodes)[i].posX = rand()%(640-20) + 10;
    (*nodes)[i].posY = rand()%(480-20) + 10;
  }
  
  int nodeA, nodeB;
  while(fscanf(inputFile, "%d-%d", &nodeA, &nodeB) == 2)
  {
    //DEBUG: printf("NoeudA:%d  NoeudB:%d\n", nodeA, nodeB);
    *links = createLink(*links, &((*nodes)[nodeA]), &((*nodes)[nodeB])); //create a link between the 2 specified nodes.
  }
  
  fclose(inputFile);
  return 0;
}


void simulation(float deltaT, float *x, float *y) {
  float vitesse = 10; // unité : pixel/secondes
  float deplacement = deltaT*vitesse; 
  *x += deplacement;
  *y += deplacement;  
}

void affichage(sfRenderWindow *fenetre, Node *nodes, int nodeCt, Link links) {
  // préparation des couleurs
  sfColor noir = {0,0,0,0}, blanc = {255,255,255,255}, gris = {128,128,128,128};
  
  sfShape **cercles = malloc(sizeof(sfShape*)*nodeCt);
  int i;
  for(i=0;i<nodeCt;i++){//creer les cercles 
    cercles[i] = sfShape_CreateCircle(nodes[i].posX, nodes[i].posY, 10, blanc, 1, gris);
  }
   
  // préparation de la ligne
  int linkCt = 0;
  Link tmp = links;
  while(tmp){
    linkCt++;
    tmp = tmp->next;
  }
  printf("linkcT = %d\n",linkCt);
  sfShape ** lignes = malloc(sizeof(sfShape*)*linkCt);
  for(i=0;i<linkCt;i++){
    lignes[i] = sfShape_CreateLine(links->nodeA->posX,links->nodeA->posY,links->nodeB->posX,links->nodeB->posY,1,blanc,1,gris); 
    links = links->next;
  }

  // préparation du texte à afficher
  
  sfString **texte = malloc(sizeof(sfString*)*nodeCt);
  char s[10];
  for(i=0;i<nodeCt;i++){
    texte[i] = sfString_Create();
    sprintf(s,"%d",i);
    sfString_SetText(texte[i],s);
    sfString_SetPosition(texte[i],nodes[i].posX +10, nodes[i].posY+10);
  }
  sfRenderWindow_Clear(fenetre,noir);// effacer l'écran  

  //sfRenderWindow_DrawShape(fenetre,cercle);  // tracer le cercle
  for(i=0;i<nodeCt;i++){//tracer les cercles
    sfRenderWindow_DrawShape(fenetre,cercles[i]);
  }
  for(i=0;i<linkCt;i++){
    sfRenderWindow_DrawShape(fenetre,lignes[i]);
  }    // tracer la ligne
  for(i=0;i<nodeCt;i++){//tracer les textes
    sfRenderWindow_DrawString(fenetre,texte[i]);
  } 
  sfRenderWindow_Display(fenetre); // visualiser le contenu de la fenetre    

  // faire le ménage en mémoire

  for(i=0;i<linkCt;i++)
    sfShape_Destroy(lignes[i]);
    
  for(i=0;i<nodeCt;i++){
    sfShape_Destroy(cercles[i]);
    sfString_Destroy(texte[i]);
  }
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
    deltaT = 0.01;// doit etre fixe sinon l'algo dépend de vitesse de la machine
                  // d'exécution !! (PB de correction, surtout avec valgrind!) 
                  // Ne modifiez donc pas cette valeur!
    
    char* testFile = "graphe.txt";
    
    Node *nodes = NULL;
    Link links = NULL;
    int nodeCt = 0;

    if (genesis(&nodes, &links, testFile,&nodeCt)==1)
      printf("Error reading file !\n");
     
    if(!nodes) printf("EMPTYH!\n");
    printf("Node Count = %d\n",nodeCt);
    int i = 0;
    for(i = 0;i < nodeCt; i++){
      
      printf("Premier noeud: %.1f %.1f\n", nodes[i].posX, nodes[i].posY);
    }
    
    
    //return(0);
    
    
    
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
      affichage(fenetre, nodes, nodeCt, links);
    }
    sfRenderWindow_Destroy(fenetre);    
    
    
    
    
    
    return EXIT_SUCCESS;
}
