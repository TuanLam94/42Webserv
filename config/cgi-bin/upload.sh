#!/bin/bash
# Set headers for the HTTP response
echo "Content-type: text/html"
echo ""

# Debug function
debugmsg() {
    echo "$1" >&2
}

# Debug messages for environment variables
debugmsg "HTTPS=$HTTPS"
debugmsg "REQUEST_METHOD=$REQUEST_METHOD"

# Check if the request method is POST
if [[ "$REQUEST_METHOD" == "POST" ]]; then
    debugmsg "CONTENT_LENGTH=$CONTENT_LENGTH"
    
    # Set a max upload size (for example, 10MB)
    MAX_FILE_UPLOAD_SIZE=$((10 * 1024 * 1024))
    
    # Verify content length is within the allowed size
    if [[ $CONTENT_LENGTH -gt 0 && $CONTENT_LENGTH -le $MAX_FILE_UPLOAD_SIZE ]]; then
        echo "Uploading file, please wait..."
        
        # Read the entire POST data into a variable
        raw_data=$(cat)
        
        # Extract boundary from CONTENT_TYPE
        boundary=$(echo "$CONTENT_TYPE" | sed -n 's/.*boundary=\(.*\).*/\1/p')
        debugmsg "Boundary: $boundary"
        
        # Create the output directory if it doesn't exist
        mkdir -p "config/uploads"
        
        # Use awk to parse multipart form-data more accurately
        file_name=$(echo "$raw_data" | awk -v boundary="$boundary" '
            BEGIN { RS=boundary; FS="\r\n" }
            /filename=/ {
                sub(/^.*filename="/, "", $2)
                sub(/".*$/, "", $2)
                print $2
            }
        ')
        
        file_content=$(echo "$raw_data" | awk -v boundary="$boundary" '
            BEGIN { RS=boundary; FS="\r\n"; content="" }
            /filename=/ {
                for (i=1; i<=NF; i++) {
                    if ($i ~ /^$/) {
                        for (j=i+1; j<=NF; j++) {
                            if ($j ~ /^--/) break
                            content = content $j "\n"
                        }
                        break
                    }
                }
            }
            END { 
                gsub(/\r$/, "", content)
                print content
            }
        ')
        
        # Default to a generic name if no filename is found
        outfile="config/uploads/${file_name:-uploaded_file}"
        
        # Write the file content to the output file
        echo -n "$file_content" > "$outfile"
        
        # Check for successful write
        if [[ $? -eq 0 ]]; then
            echo "<h2>File '${file_name:-uploaded_file}' has been uploaded successfully.</h2>"
        else
            echo "<h2>Error: Unable to write file!</h2>"
        fi
    else
        echo "<h2>File is too big for uploading, maximum allowed file size is $MAX_FILE_UPLOAD_SIZE bytes</h2>"
    fi
else
    echo "<h2>Error: Invalid method. Only POST requests are allowed.</h2>"
fi