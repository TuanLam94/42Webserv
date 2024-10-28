# #!/usr/bin/python3
# import cgi
# import os

# # Print headers
# print("Content-Type: text/html\n")

# # Create instance of FieldStorage to get form data
# form = cgi.FieldStorage()

# # Check if the file field is in the form
# if "file" not in form:
#     print("<h1>No file was uploaded</h1>")
# else:
#     # Retrieve the file field from the form
#     file_item = form["file"]

#     # Check if the file was uploaded
#     if file_item.filename:
#         # Define the path where the uploaded file will be saved
#         upload_dir = "config/uploads"  # Ensure this directory exists and is writable
#         os.makedirs(upload_dir, exist_ok=True)
#         file_path = os.path.join(upload_dir, os.path.basename(file_item.filename))
        
#         # Write the uploaded file to the defined path
#         with open(file_path, 'wb') as f:
#             f.write(file_item.file.read())
        
#         print(f"<h1>File '{file_item.filename}' uploaded successfully!</h1>")
#         print(f"<p>Saved at: {file_path}</p>")
#     else:
#         print("<h1>No file was uploaded</h1>")

#!/usr/bin/env python3
import os
import cgi
import cgitb
cgitb.enable()  # Activer le débogage CGI pour voir les erreurs

# Définir le répertoire de téléchargement
upload_dir = "config/uploads"  # Remplacez par le chemin de votre dossier de destination

print("Content-Type: text/html")
print()  # Ligne vide nécessaire après les en-têtes

# Vérifiez si le répertoire de téléchargement existe, sinon créez-le
if not os.path.exists(upload_dir):
    os.makedirs(upload_dir)

form = cgi.FieldStorage()
file_item = form['file']

# Vérifier si un fichier a été téléchargé
if file_item.filename:
    # Enregistrer le fichier sur le serveur
    file_path = os.path.join(upload_dir, os.path.basename(file_item.filename))
    with open(file_path, 'wb') as output_file:
        output_file.write(file_item.file.read())
    
    print(f"<h1>Fichier {file_item.filename} téléversé avec succès !</h1>")
else:
    print("<h1>Aucun fichier n'a été sélectionné.</h1>")