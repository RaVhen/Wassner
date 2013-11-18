// pour compiler : gcc -Wall -g -lm fenetre.c -lcsfml-graphics  -o fenetre
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SFML/Graphics.h> 

#define MASS 1
#define GRAVITY 50000
#define SPRING 4
#define SPRINGSIZE 100

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


float distanceXY(float xa, float ya, float xb, float yb){
  return(sqrt(   ( (xb-xa)*(xb-xa) ) + ( (yb-ya)*(yb-ya) )  ));
}


void debugPrintNodes(Node *nodes, int nodeCt){
  if(!nodes) printf("EMPTYH!\n");
      printf("Node Count = %d\n",nodeCt);
      int i = 0;
       
      printf("Node \t X \t Y \t SpeX \t SpeY \t VelX \t VelY \t ForX \t ForY\n");
      for(i = 0;i < nodeCt; i++){
        
        //printf("Node #%d: \t %.1f \t %.1f\n", i,nodes[i].posX, nodes[i].posY);
        printf("%d: \t %f \t %f \t %f \t %f \t %f \t %f \t %f \t %f\n", i,nodes[i].posX, nodes[i].posY, nodes[i].speX, nodes[i].speY, nodes[i].velX, nodes[i].velY, nodes[i].forX, nodes[i].forY);
      }
      printf("\n");
}


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
  /*FILE * inputFile;
  inputFile = fopen(filename, "r");
  printf("Try to open %s\n", filename);
  
  if(inputFile==NULL)
    return(1); //error reading file !
  */
  scanf("%d", nodeCt); //get nodes count
  *nodes = malloc(sizeof(Node)*(*nodeCt)); //create nodes array
  
  int i;
  for(i=0;i<*nodeCt;i++){
    (*nodes)[i].posX = rand()%(640-20) + 10;
    (*nodes)[i].posY = rand()%(480-20) + 10;
  }
  
  int nodeA, nodeB;
  while(scanf("%d-%d", &nodeA, &nodeB) == 2)
  {
    //DEBUG: printf("NoeudA:%d  NoeudB:%d\n", nodeA, nodeB);
    *links = createLink(*links, &((*nodes)[nodeA]), &((*nodes)[nodeB])); //create a link between the 2 specified nodes.
  } 
  //fclose(inputFile);
  return 0;
}

void updateForces(Node *nodes, int nodeCt, Link links){
  float resultX = 0;
  float resultY = 0;
  int current = 0, neighbour = 0;
  float disX;
  float disY;
  float dis;
  Link curLink = links;
  
  
  
  // clean old forces
  for(current = 0; current < nodeCt; current++){
    nodes[current].forX = 0;
    nodes[current].forY = 0;
  }
  
  // LINK FORCES
  
  float deltaX, deltaY;
  float deltaDis;
  float sign;
  while(curLink != NULL){   
    disX = curLink->nodeB->posX - curLink->nodeA->posX;
    disY = curLink->nodeB->posY - curLink->nodeA->posY;
    //printf("disX %f          disY %f\n",disX,disY);
    dis = sqrt( (disX*disX) + (disY*disY) );
    deltaDis = SPRINGSIZE - distanceXY(curLink->nodeA->posX, curLink->nodeA->posY,curLink->nodeB->posX, curLink->nodeB->posY);
    
    float bigDis = distanceXY(curLink->nodeA->posX, curLink->nodeA->posY,curLink->nodeB->posX, curLink->nodeB->posY);
    //printf("     %f     \n",bigDis - SPRINGSIZE);
    deltaX = ((bigDis - SPRINGSIZE)*(disX))/2/bigDis;
    deltaY = ((bigDis - SPRINGSIZE)*(disY))/2/bigDis;
    //printf("%f       %f\n",deltaX,deltaY);
    sign = 1; //sign of deltaDis : >0 ~> compressed spring 
    
    curLink->nodeA->forX += SPRING * deltaX;
    curLink->nodeB->forX -= SPRING * deltaX;
    
    curLink->nodeA->forY += SPRING * deltaY;
    curLink->nodeB->forY -= SPRING * deltaY;
    
    curLink = curLink->next;
    }
  
  //repulsive with neighbourhood
  for(current = 0; current < nodeCt-1; current++){
    for(neighbour = current+1; neighbour < nodeCt; neighbour++){
      disX = nodes[current].posX - nodes[neighbour].posX;
      disY = nodes[current].posY - nodes[neighbour].posY;
      dis = (disX*disX) + (disY*disY);
      if(dis!=0){
	if(sqrt(dis) < 1000000){
	      resultX = GRAVITY*(MASS*MASS/(dis))*(abs(disX)/sqrt(dis));
	      resultY = GRAVITY*(MASS*MASS/(dis))*(abs(disY)/sqrt(dis));
	}
	else{
	  resultX = 0;
	  resultY = 0;
	}
	      //   printf("DEBUG: updateForces current=%d\tneighbour=%d\tx=%f\ty=%f\n", current, neighbour, resultX, resultY);
      }
      
      if(disX<0){ //if current is at the left of neighbour
	      nodes[current].forX -= resultX;
	      nodes[neighbour].forX += resultX;
      }
      if(disX>0){
	      nodes[current].forX += resultX;
	      nodes[neighbour].forX -= resultX;
      }
      if(disY < 0){
	      nodes[current].forY -= resultY;
	      nodes[neighbour].forY += resultY;
      }
      if(disY > 0){
	      nodes[current].forY += resultY;
	      nodes[neighbour].forY -= resultY;
      }
      printf("DEBUG: updateForces ~> [%d] X=%c%f Y=%c%f\n", current,(disX>0)?'+':'-' ,resultX,(disY>0)?'-':'+',resultY);
      // printf("DEBUG: updateForces ~> [%d] X=%f Y=%f\n", neighbour, nodes[neighbour].forX, nodes[neighbour].forY);
    }
  }
}

void updateVelocity(Node *nodes, int nodeCt){
  int current;
  for(current = 0; current < nodeCt; current++){
    nodes[current].velX = nodes[current].forX / MASS;
    nodes[current].velY = nodes[current].forY / MASS;
    nodes[current].velX *= 0.70;
    nodes[current].velY *= 0.70;
  }
}

void updateSpeed(Node *nodes, int nodeCt, float deltaT){
  int current;
  for(current = 0; current < nodeCt; current++){
    nodes[current].speX += nodes[current].velX * deltaT;
    nodes[current].speY += nodes[current].velY * deltaT;
    /*if(nodes[current].speX < 1)
      nodes[current].speX = 0;
    if(nodes[current].speY < 1)
    nodes[current].speY = 0;*/
  }
}

void updatePosition(Node *nodes, int nodeCt, float deltaT){
  int current;
  for(current = 0; current < nodeCt; current++){
    nodes[current].posX += nodes[current].speX * deltaT;
    nodes[current].posY += nodes[current].speY * deltaT;
  }
}

void simulation(Node *nodes, int nodeCt, float deltaT, Link links) {
  /*float vitesse = 10; // unité : pixel/secondes
  
  float deplacement = deltaT*vitesse; 
  *x += deplacement;
  *y += deplacement;  
  */
  updateForces(nodes, nodeCt, links);
  updateVelocity(nodes, nodeCt);
  updateSpeed(nodes, nodeCt, deltaT);
  updatePosition(nodes, nodeCt, deltaT);

  
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
  //Debug to print links number
  //printf("linkcT = %d\n",linkCt);
  sfShape ** lignes = malloc(sizeof(sfShape*)*linkCt);
  for(i=0;i<linkCt;i++){
    lignes[i] = sfShape_CreateLine(links->nodeA->posX,links->nodeA->posY,links->nodeB->posX,links->nodeB->posY,1,blanc,1,gris); 
    links = links->next;
  }

  // préparation du texte à afficher
  sfString **texte = malloc(sizeof(sfString*)*nodeCt);
  char *s = malloc(sizeof(char)*20);
  for(i=0;i<nodeCt;i++){
    texte[i] = sfString_Create(); // seems to create a reading memory eroor in valgrind, cant find documentation on it :/
    sprintf(s,"%d",i);
    sfString_SetText(texte[i],s);
    sfString_SetPosition(texte[i],nodes[i].posX +10, nodes[i].posY+10);
  }
  sfRenderWindow_Clear(fenetre,noir);// effacer l'écran  

  for(i=0;i<nodeCt;i++){//tracer les cercles
    sfRenderWindow_DrawShape(fenetre,cercles[i]);
    sfRenderWindow_DrawString(fenetre,texte[i]);
  }
  for(i=0;i<linkCt;i++) // Tracer des lignes
    sfRenderWindow_DrawShape(fenetre,lignes[i]);
  sfRenderWindow_Display(fenetre); // visualiser le contenu de la fenetre    

  free(s);
  // faire le ménage en mémoire
  for(i=0;i<linkCt;i++)
    sfShape_Destroy(lignes[i]);
  for(i=0;i<nodeCt;i++){
    sfShape_Destroy(cercles[i]);
    sfString_Destroy(texte[i]);
  }
}




int main(int argc, char **argv) {
    // initialisation graphique
    sfRenderWindow *fenetre;
    sfVideoMode mode = {640, 480, 32};
    sfWindowSettings settings = {24, 8, 0};
    fenetre = sfRenderWindow_Create(mode, "Ma fenetre",  sfResize | sfClose, settings);
    sfEvent evenement;
    
    // initialisation des données utilisé pour la simulation
    
    float deltaT; // temps entre deux moments discrets de la simulation
    deltaT = 0.01;// doit etre fixe sinon l'algo dépend de vitesse de la machine
                  // d'exécution !! (PB de correction, surtout avec valgrind!) 
                  // Ne modifiez donc pas cette valeur!
    char* testFile = malloc(sizeof(char*)*70);
    if(argc > 1)
      testFile = argv[1];
    else{
      //testFile = "graphe.txt";
    }
    printf("%s\n",testFile);
    Node *nodes = NULL;
    Link links = NULL;
    int nodeCt = 0;

    if (genesis(&nodes, &links, testFile,&nodeCt)==1)
      printf("Error reading file !\n");
     
    debugPrintNodes(nodes, nodeCt);
    
    
    //return(0);
    
    
    
    // ####### Boucle principale
    int debug=0;
    while (sfRenderWindow_IsOpened(fenetre)) {
      // gestion des evenements
      sfRenderWindow_GetEvent(fenetre, &evenement);
      if (evenement.Type == sfEvtClosed) { // sortie de boucle d'animation
        sfRenderWindow_Close(fenetre);
      } 
      
      //if(!debug){                
      // calcul de simulation
      simulation(nodes, nodeCt, deltaT, links); 
      
      // affichage
      affichage(fenetre, nodes, nodeCt, links);
      
      
      debugPrintNodes(nodes, nodeCt);
      debug++;
    }
    sfRenderWindow_Destroy(fenetre);    
    
    
    
    
    
    return EXIT_SUCCESS;
}
