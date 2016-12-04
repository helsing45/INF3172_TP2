<div id="contenu">
	<h1>TP2 - Queue de commandes</h1>

<p>L'objectif du TP est de développer un utilitaire <code>cq</code> (<em>command queue</em>) qui permet à des commandes de se faire exécuter séquentiellement, même lorsque celles-ci sont lancées de façon concurrente.</p>

<p>Le principe est qu'avant d'exécuter une commande, l'utilitaire se connecte à un serveur et attend son feu vert.
De son coté, le serveur coordonne les différentes demandes des clients et s'assure de donner le feu vert (un seul à la fois) dans l'ordre d'arrivée des clients.</p>

<h2>Description de l'outil</h2>

<p>La communication entre le serveur et les clients se fait grâce à une socket UNIX.
Les sockets UNIX sont un mécanisme de communication locale inter-processus qui se base sur l'API POSIX des sockets réseau (qui incluent TCP/IP par exemple).</p>

<p>Pages du man à consulter: unix(7), socket(2), bind(2), listen(2), accept(2), connect(2)</p>

<p>Le protocole de communication vous est imposé et est très simple car il se base sur l'envoi de simples caractères.</p>

<p>Lorsqu'un client veut exécuter une commande, il se connecte à la socket "cq.socket" du répertoire courant.
Il envoie ensuite le caractère "R" pour indiquer au serveur qu'il est prêt (<em>ready</em> ou <em>red</em>) et attend la réponse du serveur.
Si la réponse est "G" (<em>go</em> ou <em>green</em>), le client exécute la commande puis à la fin de l'exécution se déconnecte du serveur (ce qui a pour effet de signaler au serveur que le client à fini).
Si autre chose arrive ou si la connexion est coupée, le client affiche un message d'erreur et se termine sans exécuter la commande.</p>

<p>Le serveur ouvre une socket UNIX "cq.socket" du répertoire courant et attend des clients.
Lorsqu'un client se connecte, il lit un caractère.
Si "K" est reçu, le serveur envoie "D" à tous les clients de la file d'attente puis se termine.
Si "R" est reçu, le client est mis dans la file d'attente.
Si autre chose est reçu, la connexion avec le client est coupée.</p>

<p>Il y a au maximum un client actif à la fois.<br>
Si aucun client n'est actif ou lorsque la connexion avec le client actif est terminée, le suivant dans la file d'attente devient actif à son tour, le serveur lui envoie alors "G".</p>

<p>Lorsque le serveur se termine (via un signal ou via "K"), le fichier "cq.socket" est supprimé.</p>

<h3>Objectifs pédagogiques</h3>

<p>Ce travail pratique permet de réaliser un utilitaire système réaliste qui nécessite de s'initier aux points suivants:</p>

<ul>
<li>supervision de processus</li>
<li>communication inter-processus (socket UNIX)</li>
<li>gestion des signaux</li>
<li>implémentation d'un ordonnanceur simple (file d'attente)</li>
<li>multiplexage d'entrées-sorties</li>
</ul>


<h3>Fonctionnalités demandées</h3>

<p>Pour simplifier la réalisation de l'utilitaire <code>cq</code> voici l'ordre suggéré de développement.
À chaque étape du développement correspond une option de l'utilitaire (en commençant par les options simples et primordiales).
L'utilitaire final devra accepter l'ensemble des options.</p>

<p>Note: pour simplifier le développement, au maximum une seule option sera présente et sera toujours placée en premier argument.</p>

<h3>Exécution simple (x)</h3>

<p>Usage: <code>cq -x commande [arguments...]</code></p>

<p>Avec l'option <code>-x</code>, la commande est exécutée immédiatement sans se connecter à un serveur.</p>

<p>Le code de retour de l'utilitaire est celui de la commande.</p>

<p>L'objectif de cette option est de vérifier le bon fonctionnement de l'exécution d'une commande.</p>

<p><strong>Code de retour</strong></p>

<p>En cas de succès, le code de retour de <code>cq -x</code> est celui de la commande exécutée.</p>

<p>En cas d'échec d’exécution de la commande, le code de retour est 127 et un message d'erreur est affiché.</p>

<h3>Serveur factice (y)</h3>

<p>Usage: <code>cq -y</code></p>

<p>L'option <code>-y</code> (<em>yes man</em>, ou <em>yellow</em>) lance l'utilitaire en mode serveur factice, celui-ci crée la socket "<code>cp.socket</code>" dans le répertoire courant et attend les clients.
Pour ce faire vous devez appeler dans l'ordre:</p>

<ul>
<li><code>socket(AF_UNIX, SOCK_STREAM, 0)</code></li>
<li><code>bind</code></li>
<li><code>listen</code></li>
</ul>


<p>Une fois la socket crée, le serveur simple devra:</p>

<ul>
<li>accepter toutes les connexions (<code>accept</code>)</li>
<li>lire un caractère et vérifier que c'est "R" (<code>recv</code>)</li>
<li>écrire "G" (<code>send</code>)</li>
<li>fermer la socket (<code>close</code>)</li>
<li>puis attendre le client suivant.</li>
</ul>


<p>En mode serveur factice, il n'y a pas donc pas d'ordonnancement et les clients sont exécutés immédiatement sans file d'attente.
Le mode factice permet donc de tester avec un seul client à la fois.</p>

<p>Si un client envoie "K" au lieu de "R", le serveur supprime "cp.socket", répond "D", ferme la connexion, et termine avec un code de retour 0.</p>

<p>Si le client envoie autre chose que "K" ou "R", le serveur affiche un message d'erreur et ferme la connexion avec le client.</p>

<p>Lorsque le serveur se termine avec un Ctrl-C (ou tout autre signal adapté), le fichier socket doit aussi être supprimé.</p>

<p><strong>Code de retour</strong></p>

<p><code>cq -y</code> retourne 0 lorsque celui-ci est terminé avec "K".</p>

<p>Dans ce cas, l'utilisateur a la garantie que le serveur s'est bien terminé, que ses clients ont été notifiés et que la socket existent plus.</p>

<p>En cas de problème d'initialisation du serveur, <code>cq -y</code> affiche un message d'erreur et retourne 1.</p>

<p><strong>Note pédagogique</strong></p>

<p>Normalement, un tel serveur ne devrait pas utiliser le répertoire courant pour stocker le fichier socket. En effet, rien ne garantie à l'utilisateur que le répertoire courant soit écrivable ni d’être sur une partition qui permet de tels fichiers.
Un autre problème est qu'en cas de défaillance du serveur, des fichiers <code>cq.socket</code> pourraient s'accumuler à droite et à gauche.</p>

<p>La bonne pratique pour ce genre d'outil serait de créer ces sockets dans des endroits dédiés comme <code>/tmp</code> ou <code>/var/run/user</code>.</p>

<p>Toutefois, dans le cadre du TP, utiliser le répertoire courant simplifie le travail de développement et d'évaluation.</p>

<h3>Terminator</h3>

<p>Usage: <code>cq -k</code></p>

<p>Avec l'option <code>-k</code> la commande se connecte au serveur, écrit "K", attend "D" puis se termine avec le code de retour 0.</p>

<p>Si la connexion échoue ou si "D" n'est pas reçu, un message est affiché et le client se termine avec le code de retour 1.</p>

<p>Le role de l'option est de terminer le serveur, qu'il soit factice ou non.
Notons que "K" doit être traité immédiatement par le serveur et non mit en file d'attente.</p>

<p><strong>Code de retour</strong></p>

<p><code>cq -k</code> retourne 0 si le serveur s'est correctement terminé.</p>

<p>S'il est impossible de connecter au serveur ou si le serveur se comporte de façon incorrecte, 1 est retourné et un message d'erreur est affiché.</p>

<p><strong>Note pédagogique</strong></p>

<p>Il est important de comprendre que <code>cq -k</code> est le moyen recommandé de terminer le serveur. Toutefois, la responsabilité de ce bon comportement est avant tout dans les main du serveur lui-meme.</p>

<p>En particulier, lorsque <code>cq -k</code> se termine avec succès, l'utilisateur a la garantie que la socket du serveur est bien fermée, que le serveur est bien terminé, et que le fichier 'cq.socket' a bien été supprimé.</p>

<pre><code>$ cq -y &amp;
$ cq -k
$ ls cq.socket
ls: cannot access cq.socket: No such file or directory
</code></pre>

<h3>Client simple</h3>

<p>Usage: <code>cq -c commande [arguments...]</code></p>

<p>Avec l'option <code>-c</code>, le client simple se connecte au serveur, lui envoie "R" et attend "G".
Lorsque "G" est reçu, la commande est exécutée jusqu'à sa terminaison, puis la connexion est fermée et le client se termine avec le même code de retour que la commande.</p>

<p>Si la connexion échoue ou si "G" n'est pas reçu (tout particulièrement si c'est un "D" qui est reçu), un message d'erreur est affiché et le client se termine avec le code de retour 1; la commande n'est pas exécutée.</p>

<p>Ce client respecte le comportement voulu et fonctionne avec tous les types de serveurs.</p>

<p><strong>Code de retour</strong></p>

<p>En cas de succès, le code de retour de <code>cq -c</code> est celui de la commande exécutée.</p>

<p>En cas d’échec de connexion au serveur, le code de retour est 1 et un message d'erreur est affiché.</p>

<p>Si le serveur se comporte mal, coupe la connection ou envoie "D", le client affiche un message d'erreur et retourne 1.</p>

<p>En cas d'échec d’exécution de la commande, le code de retour est 127 et un message d'erreur est affiché.</p>

<h3>Serveur simple (s)</h3>

<p>Usage: <code>cq -s</code></p>

<p>L'option <code>-s</code> lance l'utilitaire en mode serveur simple.</p>

<p>If fonctionne comme le serveur factice mais implémente le comportement attendu: au maximum un client est actif à la fois, les autres sont dans la file d'attente et seront servis à tour de rôle.</p>

<p>Pour implémenter le serveur simple vous devez utiliser l'appel système <code>select</code> afin de pouvoir surveiller à la fois la connexion avec le client actif et les demande de connexion des nouveaux clients.</p>

<p>Il est important d'utilise <code>select</code> car cela permet au serveur de traiter les "K" qui arrivent meme si plusieurs clients sont dans la file d'attente.</p>

<p>Pour implémenter la file d'attente, inutile d'utiliser une structure de donnée compliquée. La plus simple (mais fonctionnelle) fera l'affaire.
Pour simplifier le développement, on demandera que le serveur soit capable d'avoir au moins 15 clients en file d'attente. Au delà, le serveur pourra leur fermer la connection au nez.</p>

<p><strong>Note pédagogique</strong></p>

<p>Un client qui exécute une commande qui ne se termine pas bloque nécessairement tous les clients suivants.</p>

<p>Toutefois, si l'utilisateur termine de force le client qui bloque tout le monde (via un signal KILL par exemple), la connection au serveur est alors fermée par le système d'exploitation.</p>

<p>En fait, pour le serveur, il n'est pas possible (ni nécessaire dans le cadre du TP) de savoir pourquoi un client s'est terminé, la seule chose qui l’intéresse est de savoir si un client est toujours au bout de la socket.</p>

<p>La socket au client sert de ligne de vie, dès que celle-ci est fermée par le client (ou par le système à la terminaison du client), le serveur passe au client suivant.</p>

<h3>Serveur Démon (d)</h3>

<p>Usage: <code>cq -d</code></p>

<p>L'option <code>-d</code> lance l'utilitaire en mode démon, c'est à dire que le serveur passe en arrière plan une fois la socket correctement établie.</p>

<p>Quand le serveur est lancé en arrière plan, le seul moyen de le terminer est d'utiliser un client avec l'option <code>-k</code> (ou de lui envoyer le signal approprié, bien évidemment).</p>

<p><strong>Code de retour</strong></p>

<p>0 est retourné quand <code>sq.socket</code> est correctement crée et que les client peuvent commencer à s'y connecter.</p>

<p>En cas de problème pour initialiser le serveur, 1 est retourné et un message d'erreur est affiché.</p>

<p><strong>Note pédagogique</strong></p>

<p>Il est important de comprendre la différence entre</p>

<pre><code>cq -s &amp;
cq -c echo hello
</code></pre>

<p>et</p>

<pre><code>cq -d
cq -c echo hello
</code></pre>

<p>Dans le premier cas, le fork qui permet de passer en arrière plan a lieu avant même l'exécution de l'utilitaire. Ce qui veut dire que l'utilisateur ne sais pas si le serveur a bien été lancé. Cela veut dire aussi qu'au moment du <code>cq -c</code>, le serveur n'a peut-être pas encore été lancé en fonction des caprices de l'ordonnanceur ce qui fait que <code>cq -c</code> pourrait échouer.</p>

<p>Dans le second cas, lorsque la commande <code>cq -d</code> retourne avec success, l'utilisateur sait que le serveur est bien lancé. Cela veut dire aussi que lors du <code>cq -c</code> la socket du serveur est disponible et accepte les connexions.</p>

<p>L'option <code>-d</code> offre donc une valeur ajoutée par rapport à exécuter au niveau du shell le serveur en arrière plan.</p>

<h3>Client ultime</h3>

<p>Usage: <code>cq commande [arguments...]</code></p>

<p>Sans aucune option, l'utilitaire fonctionne comme le client simple mais:</p>

<ul>
<li>lance le serveur automatiquement si nécessaire.</li>
<li>retourne immédiatement à l'utilisateur une fois connecté au serveur, sans attendre le feu vert du serveur ni la terminaison de la commande. La commande sera alors exécutée en arrière plan.</li>
</ul>


<p>Vu que l'attente du feu vert et l'exécution de la commande se font en arrière plan, l'utilisateur ne peut pas savoir simplement l'état de la commande et si elle s'est bien terminé (résoudre ce problème serait relativement facile mais compliquerait le TP sans apporter réellement un gain pédagogique).</p>

<p>Il peut toutefois facilement stocker la sortie de la commande pour le consulter plus tard.</p>

<pre><code>$ cq commande &gt; fichier_sortie
</code></pre>

<p>L'utilisateur peut également utiliser <code>cq -c</code> comme point de synchronisation. En effet <code>cq -c</code> attend nécessairement la terminaison de la commande. Ainsi on pourrait écrire</p>

<pre><code>$ cq cmd1
$ cq cmd2
$ cq -c echo "fini!" # attendre la fin des deux commandes précédentes
</code></pre>

<p><strong>Code de retour</strong></p>

<p>Lorsque la connexion au serveur est correctement établie, 0 est retourné.</p>

<p>S'il est impossible de se connecter au serveur et qu'il est également impossible de lancer le serveur en arrière plan pour s'y connecter, un message d'erreur est affiché et 1 est retourné.</p>

<p><strong>Note pédagogique</strong></p>

<p>De la même manière que pour <code>cq -s</code> v. <code>cq -d</code> précédent, il est important de comprendre la différence entre</p>

<pre><code>cq -c echo hello &amp;
cq -c echo world &amp;
</code></pre>

<p>et</p>

<pre><code>cq echo hello
cq echo world
</code></pre>

<p>Dans le premier cas, le fork qui permet aux <code>cq -c</code> de s'exécuter en arrière plan a lieu très tôt. L'utilisateur ne sais donc pas si la connexion au serveur s'est bien passée. Pire, rien n'interdit (même si c'est hautement improbable) que le second <code>cq -c</code> soit ordonnancé avant le premier, et s’inscrive donc au niveau du serveur avant l'autre, ceci causant le "<code>word</code>" de s'afficher avant le "<code>hello</code>".</p>

<p>Dans le second cas, <code>cq</code> ne passe en arrière plan qu'une fois la connexion établie avec le serveur. L'utilisateur sait donc si la connexion s'est bien passé et il a la garantie que le premier <code>cq</code> est inscrit dans la file d'attente du serveur avant le second.</p>

<h3>Récapitulatif des options</h3>

<pre><code>Nom: cq - command queue

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
</code></pre>

	</div>