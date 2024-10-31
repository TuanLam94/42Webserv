# #!/r43r43r434rusr/bin/env python3

# import os
# import cgi

# # Get the form data
# form = cgi.FieldStorage()

# # Check if a file was uploaded
# if 'file' in form:
#     file_item = form['file']
    
#     # Get the filename and file content
#     filename = file_item.filename
#     file_content = file_item.file.read()
    
#     # Save the file to the server
#     upload_dir = 'config/uploads'
#     if not os.path.exists(upload_dir):
#         os.makedirs(upload_dir)
    
#     file_path = os.path.join(upload_dir, filename)
#     with open(file_path, 'wb') as f:
#         f.write(file_content)
    
#     print('Content-Type: text/plain\n\nFile uploaded successfully:', filename)
# else:
#     print('Content-Type: text/plain\n\nNo file uploaded.')

#!/usr/bin/python3
import os
import sys
import email
from email.parser import BytesParser
from io import BytesIO

def debug_log(message):
    """Write debug message to stderr"""
    sys.stderr.write(f"DEBUG: {message}\n")
    sys.stderr.flush()

def main():
    try:
        # Print headers immediately
        print("Content-Type: text/plain")
        print()
        
        # Log environment
        debug_log("=== Environment Variables ===")
        content_length = int(os.environ.get('CONTENT_LENGTH', '0'))
        content_type = os.environ.get('CONTENT_TYPE', '')
        debug_log(f"CONTENT_LENGTH: {content_length}")
        debug_log(f"CONTENT_TYPE: {content_type}")
        
        # Read raw input
        debug_log(f"Reading {content_length} bytes...")
        raw_data = sys.stdin.buffer.read(content_length)
        debug_log(f"Read {len(raw_data)} bytes")
        
        # Extract boundary from content type
        content_type = content_type.split('; boundary=')[-1]
        debug_log(f"Using boundary: {content_type}")
        
        # Parse multipart data manually
        data = raw_data.split(b'--' + content_type.encode())[1]  # Get first part
        
        # Parse headers
        header_end = data.find(b'\r\n\r\n')
        headers = data[:header_end].decode()
        debug_log(f"Headers: {headers}")
        
        # Parse content disposition to get filename
        filename = None
        for header in headers.split('\r\n'):
            if header.startswith('Content-Disposition'):
                for part in header.split(';'):
                    if 'filename=' in part:
                        filename = part.split('=')[-1].strip('"')
                        break
        
        if filename:
            debug_log(f"Found filename: {filename}")
            
            # Get file content (skip headers and the following \r\n\r\n)
            file_content = data[header_end + 4:]
            # Remove trailing boundary and \r\n
            file_content = file_content.rsplit(b'\r\n', 1)[0]
            
            # Create uploads directory if it doesn't exist
            upload_dir = 'config/uploads'
            os.makedirs(upload_dir, exist_ok=True)
            
            # Save file
            filepath = os.path.join(upload_dir, os.path.basename(filename))
            debug_log(f"Saving to: {filepath}")
            
            with open(filepath, 'wb') as f:
                f.write(file_content)
            
            file_size = os.path.getsize(filepath)
            debug_log(f"File saved successfully, size: {file_size} bytes")
            print(f"Successfully uploaded: {filename}")
        else:
            debug_log("No filename found in headers")
            print("Error: No filename provided")
            
    except Exception as e:
        import traceback
        debug_log(f"ERROR: {str(e)}")
        debug_log(traceback.format_exc())
        print(f"Error processing upload: {str(e)}")

if __name__ == "__main__":
    main()