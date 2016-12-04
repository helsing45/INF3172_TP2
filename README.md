TP2 - Queue de commandes

L'objectif du TP est de développer un utilitaire cq (command queue) qui permet à des commandes de se faire exécuter séquentiellement, même lorsque celles-ci sont lancées de façon concurrente.

Le principe est qu'avant d'exécuter une commande, l'utilitaire se connecte à un serveur et attend son feu vert. De son coté, le serveur coordonne les différentes demandes des clients et s'assure de donner le feu vert (un seul à la fois) dans l'ordre d'arrivée des clients.

Description de l'outil
La communication entre le serveur et les clients se fait grâce à une socket UNIX. Les sockets UNIX sont un mécanisme de communication locale inter-processus qui se base sur l'API POSIX des sockets réseau (qui incluent TCP/IP par exemple).

Pages du man à consulter: unix(7), socket(2), bind(2), listen(2), accept(2), connect(2)

Le protocole de communication vous est imposé et est très simple car il se base sur l'envoi de simples caractères.

Lorsqu'un client veut exécuter une commande, il se connecte à la socket "cq.socket" du répertoire courant. Il envoie ensuite le caractère "R" pour indiquer au serveur qu'il est prêt (ready ou red) et attend la réponse du serveur. Si la réponse est "G" (go ou green), le client exécute la commande puis à la fin de l'exécution se déconnecte du serveur (ce qui a pour effet de signaler au serveur que le client à fini). Si autre chose arrive ou si la connexion est coupée, le client affiche un message d'erreur et se termine sans exécuter la commande.

Le serveur ouvre une socket UNIX "cq.socket" du répertoire courant et attend des clients. Lorsqu'un client se connecte, il lit un caractère. Si "K" est reçu, le serveur envoie "D" à tous les clients de la file d'attente puis se termine. Si "R" est reçu, le client est mis dans la file d'attente. Si autre chose est reçu, la connexion avec le client est coupée.

Il y a au maximum un client actif à la fois.
Si aucun client n'est actif ou lorsque la connexion avec le client actif est terminée, le suivant dans la file d'attente devient actif à son tour, le serveur lui envoie alors "G".

Lorsque le serveur se termine (via un signal ou via "K"), le fichier "cq.socket" est supprimé.

Objectifs pédagogiques

Ce travail pratique permet de réaliser un utilitaire système réaliste qui nécessite de s'initier aux points suivants:

supervision de processus
communication inter-processus (socket UNIX)
gestion des signaux
implémentation d'un ordonnanceur simple (file d'attente)
multiplexage d'entrées-sorties
Fonctionnalités demandées

Pour simplifier la réalisation de l'utilitaire cq voici l'ordre suggéré de développement. À chaque étape du développement correspond une option de l'utilitaire (en commençant par les options simples et primordiales). L'utilitaire final devra accepter l'ensemble des options.

Note: pour simplifier le développement, au maximum une seule option sera présente et sera toujours placée en premier argument.

Exécution simple (x)

Usage: cq -x commande [arguments...]

Avec l'option -x, la commande est exécutée immédiatement sans se connecter à un serveur.

Le code de retour de l'utilitaire est celui de la commande.

L'objectif de cette option est de vérifier le bon fonctionnement de l'exécution d'une commande.

Code de retour

En cas de succès, le code de retour de cq -x est celui de la commande exécutée.

En cas d'échec d’exécution de la commande, le code de retour est 127 et un message d'erreur est affiché.

Serveur factice (y)

Usage: cq -y

L'option -y (yes man, ou yellow) lance l'utilitaire en mode serveur factice, celui-ci crée la socket "cp.socket" dans le répertoire courant et attend les clients. Pour ce faire vous devez appeler dans l'ordre:

socket(AF_UNIX, SOCK_STREAM, 0)
bind
listen
Une fois la socket crée, le serveur simple devra:

accepter toutes les connexions (accept)
lire un caractère et vérifier que c'est "R" (recv)
écrire "G" (send)
fermer la socket (close)
puis attendre le client suivant.
En mode serveur factice, il n'y a pas donc pas d'ordonnancement et les clients sont exécutés immédiatement sans file d'attente. Le mode factice permet donc de tester avec un seul client à la fois.

Si un client envoie "K" au lieu de "R", le serveur supprime "cp.socket", répond "D", ferme la connexion, et termine avec un code de retour 0.

Si le client envoie autre chose que "K" ou "R", le serveur affiche un message d'erreur et ferme la connexion avec le client.

Lorsque le serveur se termine avec un Ctrl-C (ou tout autre signal adapté), le fichier socket doit aussi être supprimé.

Code de retour

cq -y retourne 0 lorsque celui-ci est terminé avec "K".

Dans ce cas, l'utilisateur a la garantie que le serveur s'est bien terminé, que ses clients ont été notifiés et que la socket existent plus.

En cas de problème d'initialisation du serveur, cq -y affiche un message d'erreur et retourne 1.

Note pédagogique

Normalement, un tel serveur ne devrait pas utiliser le répertoire courant pour stocker le fichier socket. En effet, rien ne garantie à l'utilisateur que le répertoire courant soit écrivable ni d’être sur une partition qui permet de tels fichiers. Un autre problème est qu'en cas de défaillance du serveur, des fichiers cq.socket pourraient s'accumuler à droite et à gauche.

La bonne pratique pour ce genre d'outil serait de créer ces sockets dans des endroits dédiés comme /tmp ou /var/run/user.

Toutefois, dans le cadre du TP, utiliser le répertoire courant simplifie le travail de développement et d'évaluation.

Terminator

Usage: cq -k

Avec l'option -k la commande se connecte au serveur, écrit "K", attend "D" puis se termine avec le code de retour 0.

Si la connexion échoue ou si "D" n'est pas reçu, un message est affiché et le client se termine avec le code de retour 1.

Le role de l'option est de terminer le serveur, qu'il soit factice ou non. Notons que "K" doit être traité immédiatement par le serveur et non mit en file d'attente.

Code de retour

cq -k retourne 0 si le serveur s'est correctement terminé.

S'il est impossible de connecter au serveur ou si le serveur se comporte de façon incorrecte, 1 est retourné et un message d'erreur est affiché.

Note pédagogique

Il est important de comprendre que cq -k est le moyen recommandé de terminer le serveur. Toutefois, la responsabilité de ce bon comportement est avant tout dans les main du serveur lui-meme.

En particulier, lorsque cq -k se termine avec succès, l'utilisateur a la garantie que la socket du serveur est bien fermée, que le serveur est bien terminé, et que le fichier 'cq.socket' a bien été supprimé.

$ cq -y &
$ cq -k
$ ls cq.socket
ls: cannot access cq.socket: No such file or directory
Client simple

Usage: cq -c commande [arguments...]

Avec l'option -c, le client simple se connecte au serveur, lui envoie "R" et attend "G". Lorsque "G" est reçu, la commande est exécutée jusqu'à sa terminaison, puis la connexion est fermée et le client se termine avec le même code de retour que la commande.

Si la connexion échoue ou si "G" n'est pas reçu (tout particulièrement si c'est un "D" qui est reçu), un message d'erreur est affiché et le client se termine avec le code de retour 1; la commande n'est pas exécutée.

Ce client respecte le comportement voulu et fonctionne avec tous les types de serveurs.

Code de retour

En cas de succès, le code de retour de cq -c est celui de la commande exécutée.

En cas d’échec de connexion au serveur, le code de retour est 1 et un message d'erreur est affiché.

Si le serveur se comporte mal, coupe la connection ou envoie "D", le client affiche un message d'erreur et retourne 1.

En cas d'échec d’exécution de la commande, le code de retour est 127 et un message d'erreur est affiché.

Serveur simple (s)

Usage: cq -s

L'option -s lance l'utilitaire en mode serveur simple.

If fonctionne comme le serveur factice mais implémente le comportement attendu: au maximum un client est actif à la fois, les autres sont dans la file d'attente et seront servis à tour de rôle.

Pour implémenter le serveur simple vous devez utiliser l'appel système select afin de pouvoir surveiller à la fois la connexion avec le client actif et les demande de connexion des nouveaux clients.

Il est important d'utilise select car cela permet au serveur de traiter les "K" qui arrivent meme si plusieurs clients sont dans la file d'attente.

Pour implémenter la file d'attente, inutile d'utiliser une structure de donnée compliquée. La plus simple (mais fonctionnelle) fera l'affaire. Pour simplifier le développement, on demandera que le serveur soit capable d'avoir au moins 15 clients en file d'attente. Au delà, le serveur pourra leur fermer la connection au nez.

Note pédagogique

Un client qui exécute une commande qui ne se termine pas bloque nécessairement tous les clients suivants.

Toutefois, si l'utilisateur termine de force le client qui bloque tout le monde (via un signal KILL par exemple), la connection au serveur est alors fermée par le système d'exploitation.

En fait, pour le serveur, il n'est pas possible (ni nécessaire dans le cadre du TP) de savoir pourquoi un client s'est terminé, la seule chose qui l’intéresse est de savoir si un client est toujours au bout de la socket.

La socket au client sert de ligne de vie, dès que celle-ci est fermée par le client (ou par le système à la terminaison du client), le serveur passe au client suivant.

Serveur Démon (d)

Usage: cq -d

L'option -d lance l'utilitaire en mode démon, c'est à dire que le serveur passe en arrière plan une fois la socket correctement établie.

Quand le serveur est lancé en arrière plan, le seul moyen de le terminer est d'utiliser un client avec l'option -k (ou de lui envoyer le signal approprié, bien évidemment).

Code de retour

0 est retourné quand sq.socket est correctement crée et que les client peuvent commencer à s'y connecter.

En cas de problème pour initialiser le serveur, 1 est retourné et un message d'erreur est affiché.

Note pédagogique

Il est important de comprendre la différence entre

cq -s &
cq -c echo hello
et

cq -d
cq -c echo hello
Dans le premier cas, le fork qui permet de passer en arrière plan a lieu avant même l'exécution de l'utilitaire. Ce qui veut dire que l'utilisateur ne sais pas si le serveur a bien été lancé. Cela veut dire aussi qu'au moment du cq -c, le serveur n'a peut-être pas encore été lancé en fonction des caprices de l'ordonnanceur ce qui fait que cq -c pourrait échouer.

Dans le second cas, lorsque la commande cq -d retourne avec success, l'utilisateur sait que le serveur est bien lancé. Cela veut dire aussi que lors du cq -c la socket du serveur est disponible et accepte les connexions.

L'option -d offre donc une valeur ajoutée par rapport à exécuter au niveau du shell le serveur en arrière plan.

Client ultime

Usage: cq commande [arguments...]

Sans aucune option, l'utilitaire fonctionne comme le client simple mais:

lance le serveur automatiquement si nécessaire.
retourne immédiatement à l'utilisateur une fois connecté au serveur, sans attendre le feu vert du serveur ni la terminaison de la commande. La commande sera alors exécutée en arrière plan.
Vu que l'attente du feu vert et l'exécution de la commande se font en arrière plan, l'utilisateur ne peut pas savoir simplement l'état de la commande et si elle s'est bien terminé (résoudre ce problème serait relativement facile mais compliquerait le TP sans apporter réellement un gain pédagogique).

Il peut toutefois facilement stocker la sortie de la commande pour le consulter plus tard.

$ cq commande > fichier_sortie
L'utilisateur peut également utiliser cq -c comme point de synchronisation. En effet cq -c attend nécessairement la terminaison de la commande. Ainsi on pourrait écrire

$ cq cmd1
$ cq cmd2
$ cq -c echo "fini!" # attendre la fin des deux commandes précédentes
Code de retour

Lorsque la connexion au serveur est correctement établie, 0 est retourné.

S'il est impossible de se connecter au serveur et qu'il est également impossible de lancer le serveur en arrière plan pour s'y connecter, un message d'erreur est affiché et 1 est retourné.

Note pédagogique

De la même manière que pour cq -s v. cq -d précédent, il est important de comprendre la différence entre

cq -c echo hello &
cq -c echo world &
et

cq echo hello
cq echo world
Dans le premier cas, le fork qui permet aux cq -c de s'exécuter en arrière plan a lieu très tôt. L'utilisateur ne sais donc pas si la connexion au serveur s'est bien passée. Pire, rien n'interdit (même si c'est hautement improbable) que le second cq -c soit ordonnancé avant le premier, et s’inscrive donc au niveau du serveur avant l'autre, ceci causant le "word" de s'afficher avant le "hello".

Dans le second cas, cq ne passe en arrière plan qu'une fois la connexion établie avec le serveur. L'utilisateur sait donc si la connexion s'est bien passé et il a la garantie que le premier cq est inscrit dans la file d'attente du serveur avant le second.

Récapitulatif des options

Nom: cq - command queue

Usage: cq [-c|-x] commande [arguments...]
       cq (-y|-d|-k|-s)

Enregistre une commande qui sera exécutée en arrière plan lorsque toutes
les commandes précédemment lancées avec `cq` seront terminées.

Options:

  -c  ne lance pas le serveur s'il n'existe pas et attend la terminaison de la commande.
  -d  démarre le serveur manuellement en arrière plan (fait implicitement par défaut).
  -k  termine le serveur.
  -x  exécute la commande directement sans se connecter au serveur (debug).
  -s  démarre le serveur manuellement en avant plan (debug).
  -y  démarre le serveur sans gestion de file d'attente (debug).
Réalisation
La réalisation de cet utilitaire se fera par groupes d'au plus deux personnes. Chaque membre de l'équipe devra maîtriser tous les aspects du programme.

Le programme devra être réalisé en C en utilisant principalement les appels systèmes UNIX vus en classe. Le code source final (correctement commenté et nettoyé) devra tenir dans un seul fichier C.
