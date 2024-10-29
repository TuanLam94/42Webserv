#!/bin/bash

upload_dir="config/uploads"
if [[ ! -d "$upload_dir" ]]; then
    mkdir -p "$upload_dir"
fi

# Read the file content from the request body
file_content=$(awk '/^Content-Disposition: form-data; name="file"; filename="/ {
    for (i = 2; i <= NF; i++) { 
        if ($i ~ /^filename=/) {
            filename = substr($i, 10, length($i)-10)
        }
    }
    getline; getline; print
}' /dev/stdin)

# Save the file to the server
file_path="$upload_dir/$filename"
echo "$file_content" > "$file_path"

echo "Content-Type: text/plain"
echo ""
echo "File uploaded successfully: $filename"