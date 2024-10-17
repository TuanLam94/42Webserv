#!/bin/bash

# Print headers
echo "Content-Type: text/html"
echo ""

# Create a temporary file to store the uploaded data
UPLOAD_DIR="config/uploads"
mkdir -p $UPLOAD_DIR
TEMP_FILE=$(mktemp $UPLOAD_DIR/upload_XXXXXX)

# Read the input data from stdin and store it in the temp file
cat > $TEMP_FILE

# Extract the uploaded file from the multipart form data (simple example)
FILE_NAME=$(grep -oP 'filename=".*?"' $TEMP_FILE | cut -d '"' -f 2)
FILE_CONTENT_START=$(grep -n "^$" $TEMP_FILE | head -1 | cut -d ":" -f 1)
tail -n +$((FILE_CONTENT_START + 1)) $TEMP_FILE > $UPLOAD_DIR/$FILE_NAME

# Print result
echo "<html><body>"
echo "<h1>File '$FILE_NAME' uploaded successfully!</h1>"
echo "<p>Saved at: $UPLOAD_DIR/$FILE_NAME</p>"
echo "</body></html>"

# Clean up the temporary file
rm $TEMP_FILE