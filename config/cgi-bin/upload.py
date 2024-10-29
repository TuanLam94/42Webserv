#!/usr/bin/env python3

import os
import cgi

# Get the form data
form = cgi.FieldStorage()

# Check if a file was uploaded
if 'file' in form:
    file_item = form['file']
    
    # Get the filename and file content
    filename = file_item.filename
    file_content = file_item.file.read()
    
    # Save the file to the server
    upload_dir = 'config/uploads'
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)
    
    file_path = os.path.join(upload_dir, filename)
    with open(file_path, 'wb') as f:
        f.write(file_content)
    
    print('Content-Type: text/plain\n\nFile uploaded successfully:', filename)
else:
    print('Content-Type: text/plain\n\nNo file uploaded.')