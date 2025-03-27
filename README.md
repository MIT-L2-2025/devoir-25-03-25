# File System Analyzer

Un outil en C pour analyser les propriétés des fichiers et répertoires

## Fonctionnalités
- Analyse récursive des répertoires
- Extraction des métadonnées :
  - Type de fichier (régulier, dossier, lien, etc.)
  - Permissions (rwx)
  - Taille (en octets)
  - Numéro d'inode
  - Blocs alloués
- Affichage coloré et structuré

## Structure du fichier
```bash
.
├── include/
│   └── File.h      # Déclarations des fonctions
├── src/
│   ├── File.c      # Implémentation de l'analyse
├── util/
│   └── main.c      # Point d'entrée
└── README.md       # Documentation
```

## Compilation
```bash
gcc -Wall -Wextra util/main.c src/File.c -Iinclude -o main
```
## Utilisation
```bash
./main
```
## Exemple de sortie
```bash
=> Fichier : /etc/hosts
     -> Inode     : 123456
     -> Size      : 1024
     -> Mode      : fichier  
     -> Permisson : rw-r--r--
     -> Bloc      : 8
---------------------------------
```

