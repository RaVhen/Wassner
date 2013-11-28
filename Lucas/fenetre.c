// pour compiler : gcc -Wall -g -lm fenetre.c -lcsfml-graphics	-o fenetre
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SFML/Graphics.h> 


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define MASS 1
#define GRAVITY 60000
#define SPRING 0.5
#define SPRINGSIZE 70
#define DEBUG_PRINTNODES 0
#define DEBUG_SHOWCENTER 0
#define DEBUG_SHOWSPEEDVECTORS 1
#define DEBUG_SHOWVELOCITYVECTORS 1

typedef struct Node_{						// Node struct
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
	return(sqrt(	 ( (xb-xa)*(xb-xa) ) + ( (yb-ya)*(yb-ya) )	));
}




void debugPrintNodes(Node *nodes, int nodeCt){

	if(DEBUG_PRINTNODES){
		if(!nodes) printf("EMPTYH!\n");
				printf("Node Count = %d\n",nodeCt);
				int i = 0;
				 
				printf("Node \t\t X \t\t Y \t\t SpeX \t\t SpeY \t\t VelX \t\t VelY \t\t ForX \t\t ForY\n");
				for(i = 0;i < nodeCt; i++){
				
					//printf("Node #%d: \t %.1f \t %.1f\n", i,nodes[i].posX, nodes[i].posY);
					printf("%d: \t %f \t %f \t %f \t %f \t %f \t %f \t %f \t %f\n", i,nodes[i].posX, nodes[i].posY, nodes[i].speX, nodes[i].speY, nodes[i].velX, nodes[i].velY, nodes[i].forX, nodes[i].forY);
				}
				printf("\n");
	}
}


Link createLink(Link l, Node *nA, Node *nB){

	//DEBUG printf("AdresseA:%p	AdresseB:%p\n", nA, nB);
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
		(*nodes)[i].posX = rand()%(WINDOW_WIDTH-20) + 10;
		(*nodes)[i].posY = rand()%(WINDOW_HEIGHT-20) + 10;
	}
	
	int nodeA, nodeB;
	while(scanf("%d-%d", &nodeA, &nodeB) == 2)
	{
		//DEBUG: printf("NoeudA:%d NoeudB:%d\n", nodeA, nodeB);
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
	
	while(curLink != NULL){	 
		disX = curLink->nodeB->posX - curLink->nodeA->posX;
		disY = curLink->nodeB->posY - curLink->nodeA->posY;
		
		dis = sqrt( (disX*disX) + (disY*disY) );
		
		
		float bigDis = distanceXY(curLink->nodeA->posX, curLink->nodeA->posY,curLink->nodeB->posX, curLink->nodeB->posY);
		
		deltaX = ((bigDis - SPRINGSIZE)*(disX))/2/bigDis;
		deltaY = ((bigDis - SPRINGSIZE)*(disY))/2/bigDis;
		
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
	//if(sqrt(dis) < 1000000){
				resultX = GRAVITY*(MASS*MASS/(dis))*(abs(disX)/sqrt(dis));
				resultY = GRAVITY*(MASS*MASS/(dis))*(abs(disY)/sqrt(dis));
	//}
	//else{
	//	resultX = 0;
	//	resultY = 0;
	//}
				
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
			
		}
	}
}

void updateVelocity(Node *nodes, int nodeCt, float percentage, float highCut){
	int current;
	for(current = 0; current < nodeCt; current++){
		
		nodes[current].velX = percentage * nodes[current].forX / MASS;
		nodes[current].velY = percentage * nodes[current].forY / MASS;
		
		if(highCut>0){
			if(nodes[current].velX > highCut)
				nodes[current].velX = highCut;
			if(nodes[current].velX < - highCut)
				nodes[current].velX = - highCut;
				
			if(nodes[current].velY > highCut)
				nodes[current].velY = highCut;
			if(nodes[current].velY < - highCut)
				nodes[current].velY = - highCut;
		}
	}
}

void updateSpeed(Node *nodes, int nodeCt, float deltaT, float percentage, float lowCut, float highCut){
	int current;
	for(current = 0; current < nodeCt; current++){
		nodes[current].speX += nodes[current].velX * deltaT * percentage;
		nodes[current].speY += nodes[current].velY * deltaT * percentage;

		if(nodes[current].velX > -lowCut && nodes[current].velX < lowCut)
			nodes[current].speX = 0;
		if(nodes[current].velY > -lowCut && nodes[current].velY > -lowCut)
			nodes[current].speY = 0;
			
		if(highCut>0){
			if( nodes[current].speX > highCut)
				nodes[current].speX = highCut;
			if(nodes[current].speX < - highCut)
				nodes[current].speX = - highCut;
			
			if(nodes[current].speY > highCut)
				nodes[current].speY = highCut;
			if(nodes[current].speY < - highCut)
				nodes[current].speY = - highCut;
				
		}
	}
}

void updatePosition(Node *nodes, int nodeCt, float deltaT){
	int current;
	for(current = 0; current < nodeCt; current++){
		nodes[current].posX += nodes[current].speX * deltaT;
		nodes[current].posY += nodes[current].speY * deltaT;
	}
}

void correctPosition(Node *nodes, int nodeCt){

	int i;
	int xMin, xMax, yMin, yMax;
	
	xMin = (nodes[0].posX < nodes[1].posX) ? nodes[0].posX : nodes[1].posX;
	xMax = (nodes[0].posX > nodes[1].posX) ? nodes[0].posX : nodes[1].posX;
	
	yMin = (nodes[0].posY < nodes[1].posY) ? nodes[0].posY : nodes[1].posY;
	yMax = (nodes[0].posY > nodes[1].posY) ? nodes[0].posY : nodes[1].posY;
	
	
	
	for(i=2; i<nodeCt ; i++){	
	
		if(nodes[i].posX < xMin)
			xMin = nodes[i].posX;
			
		if(nodes[i].posX > xMax)
			xMax = nodes[i].posX;
			
		if(nodes[i].posY < yMin)
			yMin = nodes[i].posY;
			
		if(nodes[i].posY > yMax)
			yMax = nodes[i].posY;
	}
	
	float centerX = (xMax-xMin)/2 + xMin ;
	float centerY = (yMax-yMin)/2 + yMin;
	
	float deltaX = (WINDOW_WIDTH/2) - centerX;
	float deltaY = (WINDOW_HEIGHT/2) - centerY;
	
	for(i=0;i<nodeCt;i++){
		nodes[i].posX+=deltaX/2;
		nodes[i].posY+=deltaY/2;
	}
	
	
	
	

}

void avoidOverGap(Node *nodes, int nodeCt){


	int i;
	int overgap = 0;
	
	for(i=0; i<nodeCt ; i++){	
	
		if(nodes[i].posX < -500)
			overgap++;
			
		if(nodes[i].posX > WINDOW_WIDTH+500)
			overgap++;
			
		if(nodes[i].posY < -500)
			overgap++;
			
		if(nodes[i].posY > WINDOW_HEIGHT+500)
			overgap++;
	}
	
	if(overgap>0){ //if overgap, redefine another random starting position
		for(i=0;i<nodeCt;i++){
			nodes[i].posX = rand()%(WINDOW_WIDTH-20) + 10;
			nodes[i].posY = rand()%(WINDOW_HEIGHT-20) + 10;
			
			nodes[i].speX = 0;
			nodes[i].speY = 0;
			nodes[i].velX = 0;
			nodes[i].velY = 0;
		}
	}

}


void affichage(sfRenderWindow *fenetre, Node *nodes, int nodeCt, Link links) {
	// préparation des couleurs
	sfColor noir = {0,0,0,0}, blanc = {255,255,255,255}, gris = {128,128,128,128};
	sfColor rouge = {255,0,0,255}, vert = {0,255,0,255};

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
	
	//DEBUG VARIABLES :
	sfShape *centre;
	sfShape *circleXmax;
	sfShape *circleXmin;
	sfShape *circleYmax;
	sfShape *circleYmin;
	sfShape **vitesses;
	sfShape **accelerations;
	
	if(DEBUG_SHOWSPEEDVECTORS){	
		vitesses =  malloc(sizeof(sfShape*)*nodeCt);
		for(i=0;i<nodeCt;i++)
			vitesses[i] = sfShape_CreateLine(nodes[i].posX, nodes[i].posY, nodes[i].posX + nodes[i].speX , nodes[i].posY + nodes[i].speY ,1,vert,1,gris); //vitesses
	}
	
	if(DEBUG_SHOWVELOCITYVECTORS){
		accelerations = malloc(sizeof(sfShape*)*nodeCt);
		for(i=0;i<nodeCt;i++)
			accelerations[i] = sfShape_CreateLine(nodes[i].posX, nodes[i].posY, nodes[i].posX + nodes[i].velX *10, nodes[i].posY + nodes[i].velY *10,1,rouge,1,gris); //accelerations
	}		
	
	if(DEBUG_SHOWCENTER){
		float xMin, xMax, yMin, yMax;
		int nodeXmin, nodeXmax, nodeYmin, nodeYmax;
		
		xMin = (nodes[0].posX < nodes[1].posX) ? nodes[0].posX : nodes[1].posX;
		nodeXmin = (nodes[0].posX < nodes[1].posX) ? 0:1;
	
		xMax = (nodes[0].posX >= nodes[1].posX) ? nodes[0].posX : nodes[1].posX;
		nodeXmax = (nodes[0].posX >= nodes[1].posX) ? 0:1;
	
		yMin = (nodes[0].posY < nodes[1].posY) ? nodes[0].posY : nodes[1].posY;
		yMax = (nodes[0].posY >= nodes[1].posY) ? nodes[0].posY : nodes[1].posY;
	
		nodeYmin = (nodes[0].posY < nodes[1].posY) ? 0:1;
		nodeYmax = (nodes[0].posY >= nodes[1].posY) ? 0:1;
	
	
	
		for(i=0; i<nodeCt ; i++){	
	
			if(nodes[i].posX < xMin){
				xMin = nodes[i].posX;
				nodeXmin=i;
			}
		
			if(nodes[i].posX > xMax){
				xMax = nodes[i].posX;
				nodeXmax=i;
			}
			
			if(nodes[i].posY < yMin){
				yMin = nodes[i].posY;
				nodeYmin=i;
			}
			
			if(nodes[i].posY > yMax){
				yMax = nodes[i].posY;
				nodeYmax=i;
			}
		}
	
		circleXmax = malloc(sizeof(sfShape*));
		circleXmin = malloc(sizeof(sfShape*));
		circleYmax = malloc(sizeof(sfShape*));
		circleYmin = malloc(sizeof(sfShape*));
	
		circleXmin = sfShape_CreateCircle(nodes[nodeXmin].posX, nodes[nodeXmin].posY, 15, rouge, 1, gris);
		circleXmax = sfShape_CreateCircle(nodes[nodeXmax].posX, nodes[nodeXmax].posY, 15, rouge, 1, gris);
		circleYmin = sfShape_CreateCircle(nodes[nodeYmin].posX, nodes[nodeYmin].posY, 15, vert, 1, gris);
		circleYmax = sfShape_CreateCircle(nodes[nodeYmax].posX, nodes[nodeYmax].posY, 15, vert, 1, gris);
	
		centre = malloc(sizeof(sfShape*));
		centre = sfShape_CreateCircle((xMax-xMin)/2 + xMin, (yMax-yMin)/2 + yMin, 5, rouge, 1, gris);
		/*printf("DEBUG: CENTRE Xmin:%lf, Xmax:%lf  Ymin:%lf Ymax:%lf\n", xMin, xMax, yMin, yMax);
		printf("DEBUG: CENTRE X:%lf  Y:%lf\n", (xMax-xMin)/2, (yMax-yMin)/2);*/
	}
	
	sfRenderWindow_Clear(fenetre,noir);// effacer l'écran 
	
	if(DEBUG_SHOWCENTER){

		sfRenderWindow_DrawShape(fenetre, centre);
		
		sfRenderWindow_DrawShape(fenetre, circleXmin);
		sfRenderWindow_DrawShape(fenetre, circleXmax);
		sfRenderWindow_DrawShape(fenetre, circleYmin);
		sfRenderWindow_DrawShape(fenetre, circleYmax);
	}	
	
	
	for(i=0;i<nodeCt;i++){//tracer les cercles
		sfRenderWindow_DrawShape(fenetre,cercles[i]);
		sfRenderWindow_DrawString(fenetre,texte[i]);
	}
	for(i=0;i<linkCt;i++) // Tracer des lignes
		sfRenderWindow_DrawShape(fenetre,lignes[i]);
	
	
	if(DEBUG_SHOWSPEEDVECTORS){
		for(i=0;i<nodeCt;i++)
		sfRenderWindow_DrawShape(fenetre,vitesses[i]);
	}
	
	if(DEBUG_SHOWVELOCITYVECTORS){
		for(i=0;i<nodeCt;i++)
		sfRenderWindow_DrawShape(fenetre,accelerations[i]);
	}
	
	
	
	sfRenderWindow_Display(fenetre); // visualiser le contenu de la fenetre
	
	free(s);
	// faire le ménage en mémoire
	for(i=0;i<linkCt;i++)
		sfShape_Destroy(lignes[i]);
	free(lignes);
	
	for(i=0;i<nodeCt;i++){
		sfShape_Destroy(cercles[i]);
		sfString_Destroy(texte[i]);
	}
	free(cercles);
	free(texte);
	
	if(DEBUG_SHOWCENTER){
		sfShape_Destroy(centre);
		sfShape_Destroy(circleXmin);
		sfShape_Destroy(circleXmax);
		sfShape_Destroy(circleYmin);
		sfShape_Destroy(circleYmax);
	}
	
	if(DEBUG_SHOWSPEEDVECTORS){
		for(i=0;i<nodeCt;i++)
			sfShape_Destroy(vitesses[i]);
		free(vitesses);
	}
	
	if(DEBUG_SHOWVELOCITYVECTORS){
		for(i=0;i<nodeCt;i++)
			sfShape_Destroy(accelerations[i]);
		free(accelerations);	
	}
	
}



void simulation(Node *nodes, int nodeCt, float deltaT, Link links) {
	/*float vitesse = 10; // unité : pixel/secondes
	
	float deplacement = deltaT*vitesse; 
	*x += deplacement;
	*y += deplacement;	
	*/
	updateForces(nodes, nodeCt, links);
	updateVelocity(nodes, nodeCt, 1 , -1); //
	updateSpeed(nodes, nodeCt, deltaT, 100.0, 0.0, 100); //lowcut=0 for no lowcut
	updatePosition(nodes, nodeCt, deltaT);
	correctPosition(nodes, nodeCt);
	avoidOverGap(nodes, nodeCt);

}



int main(int argc, char **argv) {
		// initialisation graphique
		sfRenderWindow *fenetre;
		sfVideoMode mode = {WINDOW_WIDTH,WINDOW_HEIGHT, 32};
		sfWindowSettings settings = {24, 8, 0};
		fenetre = sfRenderWindow_Create(mode, "Ma fenetre",	sfResize | sfClose, settings);
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

		while (sfRenderWindow_IsOpened(fenetre)) {
			// gestion des evenements
			sfRenderWindow_GetEvent(fenetre, &evenement);
			if (evenement.Type == sfEvtClosed) { // sortie de boucle d'animation
				sfRenderWindow_Close(fenetre);
			} 
			

			// calcul de simulation

			simulation(nodes, nodeCt, deltaT, links); 

			// affichage
			affichage(fenetre, nodes, nodeCt, links);
			
			
			debugPrintNodes(nodes, nodeCt);
		}
		sfRenderWindow_Destroy(fenetre);
		
		
		
		return EXIT_SUCCESS;
}
