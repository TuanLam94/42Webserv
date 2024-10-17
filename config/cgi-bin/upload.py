#!/usr/bin/python3
import cgi
import os

# Print headers
print("Content-Type: text/html\n")

# Create instance of FieldStorage to get form data
form = cgi.FieldStorage()

# Check if the file field is in the form
if "file" not in form:
    print("<h1>No file was uploaded</h1>")
else:
    # Retrieve the file field from the form
    file_item = form["file"]

    # Check if the file was uploaded
    if file_item.filename:
        # Define the path where the uploaded file will be saved
        upload_dir = "config/uploads"  # Ensure this directory exists and is writable
        os.makedirs(upload_dir, exist_ok=True)
        file_path = os.path.join(upload_dir, os.path.basename(file_item.filename))
        
        # Write the uploaded file to the defined path
        with open(file_path, 'wb') as f:
            f.write(file_item.file.read())
        
        print(f"<h1>File '{file_item.filename}' uploaded successfully!</h1>")
        print(f"<p>Saved at: {file_path}</p>")
    else:
        print("<h1>No file was uploaded</h1>")
