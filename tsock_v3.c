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


/*Construction, formatage et affichage des messages*/
void construire_message(char *message, char motif, int lg) {
int i;
for (i=0;i<lg;i++) message[i] = motif;
}

void format(int num){
    if(num >= 10000){
        printf("%d",num);
    }else if(num<10000 && num >= 1000){
        printf("-%d",num);
    }else if(num<1000 && num >= 100){
        printf("--%d",num);
    }else if(num<100 && num >= 10){
        printf("---%d",num);
    }else if(num<10 && num >= 0){
        printf("----%d",num);
    }
}

void afficher_message(char *message, int lg) {
int i;
    for (i=0;i<lg;i++) 
        printf("%c", message[i]); 
        printf("]\n");
}


/* UDP source */
void UDP_source(int port,int nb_msg, int longueur, char* hostName){ 
    int sock;
    struct sockaddr_in adr_distant; 
    memset((char*)&adr_distant, 0, sizeof(adr_distant)) ; /* reset */
    adr_distant.sin_family = AF_INET ;
    adr_distant.sin_port = port ;
    int lg_adr_distant = sizeof(adr_distant) ;
    struct hostent *hp ;
    int sent;
    char message[longueur] ;

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        { 
            printf("échec de création du socket\n") ;
            exit(1) ; 
        }
    
    if ((hp = gethostbyname(hostName)) == NULL)
    { 
        printf("Erreur gethostbyname\n") ;
        exit(1) ; 
        }
    memcpy( (char*)&(adr_distant.sin_addr.s_addr),
                     hp->h_addr,
                     hp->h_length ) ;

    printf("SOURCE: lg_msg_emis= %d , port= %d, nb_evois= %d, TP =UDP, dest=%s\n",longueur,port,nb_msg,hostName);
    for(int k = 0; k < nb_msg; k++)
    {
        printf("SOURCE: Envoi n°%d (%d) [",k+1,longueur);
        
        construire_message(message,'a'+k%26,longueur);
        format(k+1);
        afficher_message(message,longueur);
        sent= sendto(sock,message,longueur,0,(struct sockaddr*)&adr_distant,sizeof(adr_distant));
        if(sent == -1){
            printf("Echec de l'envoi\n");
            exit(1);
        }
    }
	
    printf("SOURCE: fin\n");
    if(close(sock)==-1){
        printf("Echec de destruction du socket \n");
        exit(1);
    }
}


/*TCP source */
void TCP_source(int port,int nb_msg, int longueur, char* hostName){ 
    int sock;
    struct sockaddr_in adr_distant; 
    memset((char*)&adr_distant, 0, sizeof(adr_distant)) ; /* reset */
    adr_distant.sin_family = AF_INET ;
    adr_distant.sin_port = port ;
    int lg_adr_distant = sizeof(adr_distant) ;
    struct hostent *hp ;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        { 
            printf("échec de création du socket\n") ;
            exit(1) ; 
        }
    
    if ((hp = gethostbyname(hostName)) == NULL)
    { 
        printf("Erreur gethostbyname\n") ;
        exit(1) ; 
        }
    memcpy( (char*)&(adr_distant.sin_addr.s_addr),
                     hp->h_addr,
                     hp->h_length ) ;

    // Connexion au serveur
    int co=-1; 
    int sent;
    char *message= malloc(longueur*sizeof(char)) ;

    if ((co=connect(sock,(struct sockaddr*)&adr_distant,sizeof(adr_distant))) == -1)
        { 
            printf("Echec de création de connexion\n") ;
            exit(1) ; 
        }
        
    
    printf("SOURCE: lg_msg_emis= %d , port= %d, nb_evois= %d, TP =UDP, dest=%s\n",longueur,port,nb_msg,hostName);
    for(int k = 0; k < nb_msg; k++)
    {
        printf("SOURCE: Envoi n°%d (%d) [",k+1,longueur);
        
        construire_message(message,'a'+k%26,longueur);
        format(k+1);
        afficher_message(message,longueur);
        sent= write(sock,message,longueur);
        if(sent == -1){
            printf("Echec de l'envoi\n");
            exit(1);
        }
    }
    printf("SOURCE: fin\n");
    if(close(sock)==-1){
        printf("Echec de destruction du socket \n");
        exit(1);
    }
}


/*UDP puits*/
void UDP_puit(int port,int nb_msg, int longueur, char* hostName){ 
    int sock;
    struct sockaddr_in adr_local; 
    memset((char*)&adr_local, 0, sizeof(adr_local)) ; /* reset */
    adr_local.sin_family = AF_INET ;
    adr_local.sin_port = port ;
    int lg_adr_local = sizeof(adr_local) ;
    char message[longueur] ;
    struct sockaddr_in adr_em; 
    memset((char*)&adr_em, 0, sizeof(adr_em)) ; /* reset */
    adr_em.sin_family = AF_INET ;
    adr_em.sin_port = port ;
    int plg_adr_em=sizeof(adr_em);

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        { 
            printf("échec de création du socket\n") ;
            exit(1) ; 
        }
    
    if (bind (sock, (struct sockaddr *)&adr_local, lg_adr_local) == -1)
    { 
        printf("Echec du bind\n") ;
        exit(1) ; 
    }

    struct hostent *hp ;
    if ((hp = gethostbyname(hostName)) == NULL)
    { 
        printf("Erreur gethostbyname\n") ;
        exit(1) ; 
        }
    memcpy( (char*)&(adr_local.sin_addr.s_addr),
                     hp->h_addr,
                     hp->h_length ) ;

    
    int i = 1;
    while(1){
        
        printf("PUITS: Reception n°%d (%d)[",i,longueur);
        int r = recvfrom(sock, message, longueur,0,(struct sockaddr*)&adr_em,&plg_adr_em);
        if(r == -1){
            printf("Echec de lecture\n");
            exit(1);
        }
        if(r == 0){
            break;
        }
       
        format(i);
        afficher_message(message,r);
        i++;
    }
    printf("PUITS: fin\n");
    if(close(sock)==-1){
        printf("Echec de destruction du socket \n");
        exit(1);
    }
}


/* TCP puits */
void TCP_puit(int port,int nb_msg, int longueur, char* hostName){ 
    int sock;
    struct sockaddr_in adr_local; 
    memset((char*)&adr_local, 0, sizeof(adr_local)) ; /* reset */
    adr_local.sin_family = AF_INET ;
    adr_local.sin_port = port ;
    int lg_adr_local = sizeof(adr_local) ;
    struct hostent *hp ;
    struct sockaddr_in adr_em; 
    int plg_adr_em= sizeof(adr_em);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        { 
            printf("Echec de création du socket\n") ;
            exit(1) ; 
        }
    
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
    memcpy( (char*)&(adr_local.sin_addr.s_addr),
                     hp->h_addr,
                     hp->h_length ) ;

   
    // Connexion 
    int wait= listen(sock,5);
    int co=accept(sock,(struct sockaddr*)&adr_em,&plg_adr_em); 
    char* message = malloc(longueur*sizeof(char));
   
    if (co == -1)
        { 
            printf("Echec de création de connexion\n") ;
            exit(1) ; 
        }

    int i = 1;
    while(1){
        
        int r = read(co, message, longueur);
        if(r == -1){
            printf("Echec de lecture\n");
            exit(1);
        }
        if(r==0){
            break;
        }
       
        printf("PUITS: Reception n°%d (%d) [",i,longueur);
        format(i);
        afficher_message(message,r);
        i++;
    }
    
    if(close(sock)==-1){
        printf("Echec de destruction du socket \n");
        exit(1);
    }
    printf("PUITS: fin\n");
}


/*Fonction principale*/
void main (int argc, char **argv)
{
	int c;
	extern char *optarg;
	extern int optind;
    int proto=1; 
	int nb_message = 10; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
    int lg_message = 30;
	int source = -1 ; /* 0=puits, 1=source */
	while ((c = getopt(argc, argv, "pusn:l:")) != -1) {
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
        	case 'l':
			lg_message = atoi(optarg);
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

	if (nb_message < 0 || nb_message >= 100000) {
		if (source == 1)
			printf("nb de tampons à envoyer : %d\n Trop  de messages \n", nb_message);
		else
			printf("nb de tampons à recevoir : %d\n Trop  de messages \n", nb_message);
	} else {
		if (source == 1) {
			printf("nb de tampons à envoyer = %d par défaut\n",nb_message);
            if(proto==0){
                UDP_source(atoi(argv[argc-1]),nb_message,lg_message,argv[argc-2]); //UDP source
            }else {
                TCP_source(atoi(argv[argc-1]),nb_message,lg_message,argv[argc-2]); //TCP source
            }
        }else if( source == 0){
            // Puit 
			printf("nb de tampons à envoyer =  %d par défaut\n",nb_message);
            if(proto==0){
                UDP_puit(atoi(argv[argc-1]),nb_message,lg_message,argv[argc-2]); //UDP puits
            }else {
                TCP_puit(atoi(argv[argc-1]),nb_message,lg_message,argv[argc-2]); //TCP puits
            }

        } else
		printf("nb de tampons à envoyer = infini\n");
	}
    
}
