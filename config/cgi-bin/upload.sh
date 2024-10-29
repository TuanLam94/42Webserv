# #!/bin/bash

# # Print headers
# echo "Content-Type: text/html"
# echo ""

# # Create a temporary file to store the uploaded data
# UPLOAD_DIR="config/uploads"
# mkdir -p $UPLOAD_DIR
# TEMP_FILE=$(mktemp $UPLOAD_DIR/upload_XXXXXX)

# # Read the input data from stdin and store it in the temp file
# cat > $TEMP_FILE

# # Extract the uploaded file from the multipart form data (simple example)
# FILE_NAME=$(grep -oP 'filename=".*?"' $TEMP_FILE | cut -d '"' -f 2)
# FILE_CONTENT_START=$(grep -n "^$" $TEMP_FILE | head -1 | cut -d ":" -f 1)
# tail -n +$((FILE_CONTENT_START + 1)) $TEMP_FILE > $UPLOAD_DIR/$FILE_NAME

# # Print result
# echo "<html><body>"
# echo "<h1>File '$FILE_NAME' uploaded successfully!</h1>"
# echo "<p>Saved at: $UPLOAD_DIR/$FILE_NAME</p>"
# echo "</body></html>"

# # Clean up the temporary file
# rm $TEMP_FILE

#!/bin/bash

# Définir le répertoire de téléchargement
UPLOAD_DIR="/path/to/uploads"  # Remplacez par le chemin de votre dossier de destination

# Vérifiez si le répertoire de téléchargement existe, sinon créez-le
[ ! -d "$UPLOAD_DIR" ] && mkdir -p "$UPLOAD_DIR"

# Lire les en-têtes et récupérer la limite (boundary)
read -r boundary
boundary="--${boundary#Content-Type: multipart/form-data; boundary=}"

# Lire jusqu'à la ligne du nom de fichier
while read -r header && [[ $header != *filename=* ]]; do :; done

# Extraire le nom du fichier
filename=$(echo "$header" | sed 's/.*filename="\(.*\)".*/\1/')

# Ignorer les en-têtes supplémentaires
while read -r header && [[ -n $header ]]; do :; done

# Lire le contenu jusqu'à la limite de fin et enregistrer le fichier
output_path="$UPLOAD_DIR/$filename"
dd iflag=fullblock of="$output_path" bs=1 2>/dev/null

# Afficher un message de confirmation
echo "Content-Type: text/html"
echo
echo "<html><body><h1>Fichier $filename téléversé avec succès !</h1></body></html>"
