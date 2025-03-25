#!/bin/bash

# Sélectionner un répertoire aléatoire
dirs=("/etc" "/usr/local" "/var/log")
rep="${dirs[$RANDOM % ${#dirs[@]}]}"

echo "Exploration du répertoire : $rep"

# Générer le tableau HTML
output="table.html"
echo "<html><body><table border='1'>" > "$output"
echo "<tr><th>Nom du fichier</th><th>ID</th><th>Taille (octets)</th><th>Position sur le disque</th></tr>" >> "$output"

# Parcourir les fichiers et récupérer les infos
find "$rep" -maxdepth 1 -type f | while read -r file; do
    id=$(stat -c "%i" "$file")                  # ID du fichier
    size=$(stat -c "%s" "$file")                # Taille
    pos=$(stat -c "%o" "$file" 2>/dev/null)     # Position dans le cylindre 

    echo "<tr><td>$file</td><td>$id</td><td>$size</td><td>$pos</td></tr>" >> "$output"
done

echo "</table></body></html>" >> "$output"

echo "Le fichier HTML a été généré : $output"












