# Devoir System
Un programme en C qui lit les fichiers à partir d'un répertoire sélectionné aléatoirement parmi **/etc/, /var/log/ et /usr/local/**, puis affiche les informations suivantes dans un tableau HTML :<br>
     >Taille de chaque fichier dans le rep sélectionné <br>
     >ID du chaque fichier<br>
     >Sa Position sur le disque

## Compilation
pour compiler le programme 
```bash
gcc info_files.c -o info
```
## Uilisation
Exécutez le programme en :
```bash
sudo ./info
```
Le fichier HTML généré sera nommé **resultat.html** et se trouvera dans le répertoire courant. Vous pouvez l’ouvrir avec votre navigateur préféré pour afficher les résultats.
