/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>

/* Affichage et construction des messages*/
void construire_message(char *message, char motif, int lg) {
int i;
for (i=0;i<lg;i++) message[i] = motif;}

void afficher_message(char *message, int lg) {
int i;
//printf("message construit : ");
for (i=0;i<lg;i++) printf("%c", message[i]); printf("\n");}


/*UDP source*/
void UDP_source(int port,int nb_msg, int longueur, char* hostName){ 

    int sock;
    
    struct sockaddr_in adr_distant; 
    adr_distant.sin_family = AF_INET ;
    adr_distant.sin_port = port ;
    int lg_adr_distant = sizeof(adr_distant) ;
    struct hostent *hp ;
    int sent;
    char message[longueur] ;

    longueur = 30; 
    nb_msg = 10; 

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        { 
            printf("Echec de création du socket\n") ;
            exit(1) ; 
        }
    
    memset((char*)&adr_distant, 0, sizeof(adr_distant)) ; /* reset */
    
    if ((hp = gethostbyname(hostName)) == NULL)
    { 
        printf("Erreur gethostbyname\n") ;
        exit(1) ; 
    }
    memcpy( (char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length ) ;
    
    printf("SOURCE: lg_msg_emis= %d , port= %d, nb_evois= %d, TP =UDP, dest=%s\n",longueur,port,nb_msg,hostName);

    for(int k = 0; k < nb_msg; k++)
    {
        printf("SOURCE: Envoi n°%d (%d)\n",k+1,longueur);
        
        construire_message(message,'a'+k%26,longueur);
        afficher_message(message,longueur);
        sent= sendto(sock,message,longueur,0,(struct sockaddr*)&adr_distant,sizeof(adr_distant));
    }

    if(close(sock)==-1){
        printf("Echec de destruction du socket \n");
        exit(1);
    }
}


/*UDP puit*/
void UDP_puit(int port,int nb_msg, int longueur, char* hostName){ 
    
    int sock;
    struct sockaddr_in adr_local; 
    adr_local.sin_family = AF_INET ;
    adr_local.sin_port = port ;
    int lg_adr_local = sizeof(adr_local) ;
    struct hostent *hp ;

    char message[longueur] ;
    struct sockaddr_in adr_em; 
    int plg_adr_em;

    memset((char*)&adr_em, 0, sizeof(adr_em)) ; /* reset */
    adr_em.sin_family = AF_INET ;
    adr_em.sin_port = port ;
    
    memcpy( (char*)&(adr_local.sin_addr.s_addr),hp->h_addr,hp->h_length ) ;
    longueur = 30; 
    nb_msg = 10; 

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        { 
            printf("Echec de création du socket\n") ;
            exit(1) ; 
        }
    
    memset((char*)&adr_local, 0, sizeof(adr_local)) ; /* reset */
    
    if (bind (sock, (struct sockaddr *)&adr_local, lg_adr_local) == -1)
    { 
        printf("Echec du bind\n") ;
        exit(1) ; 
    }

    if ((hp = gethostbyname(hostName)) == NULL)
    { 
        printf("Erreur gethostbyname\n") ;
        exit(1) ; 
        }
    
    
    
    while(1){
        int read = recvfrom(sock, message, longueur,0,(struct sockaddr*)&adr_em,&plg_adr_em);
        afficher_message(message,longueur);
    }
    
    if(close(sock)==-1){
        printf("Echec de destruction du socket \n");
        exit(1);
    }
}


/* Fonction principale */
void main (int argc, char **argv)
{
	int c;
	extern char *optarg;
	extern int optind;
    int proto=0; 
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
    
	while ((c = getopt(argc, argv, "pn:s")) != -1) {
		switch (c) {
		case 'p':
			if (source == 1) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) {
				printf("usage: cmd [-p|-s][-n ##]\n");
				exit(1) ;
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
			break;

		case 'u':
			proto = 0;
		        break; 

		default:
			printf("usage: cmd [-p|-s][-n ##]\n");
			break;
		}
	}

	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##]\n");
		exit(1) ;
	}

	if (source == 1)
		printf("on est dans le source\n");
	else
		printf("on est dans le puits\n");

	if (nb_message != -1) {
		if (source == 1)
			printf("nb de tampons à envoyer : %d\n", nb_message);
		else
			printf("nb de tampons à recevoir : %d\n", nb_message);
	} else {
		if (source == 1) {
			nb_message = 10 ;
            int longueur = 30;
			printf("nb de tampons à envoyer = 10 par défaut\n");
            if(proto==0){
                UDP_source(atoi(argv[argc-1]),nb_message,longueur,argv[argc-2]);
            }else {
                // TCP (voir v2)
            }
        }else if( source == 0){
            // Puit 
            nb_message = 10 ;
            int longueur = 30;
			printf("nb de tampons à envoyer = 10 par défaut\n");
            if(proto==0){
                UDP_puit(atoi(argv[argc-1]),nb_message,longueur,argv[argc-2]); // UDP 
            }else {
                // TCP (voir v2)
            }

        } else
		printf("nb de tampons à envoyer = infini\n");
	}
    
}
