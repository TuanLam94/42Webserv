# #!/bin/bash

# upload_dir="config/uploads"
# if [[ ! -d "$upload_dir" ]]; then
#     mkdir -p "$upload_dir"
# fi

# # Read the file content from the request body
# file_content=$(awk '/^Content-Disposition: form-data; name="file"; filename="/ {
#     for (i = 2; i <= NF; i++) { 
#         if ($i ~ /^filename=/) {
#             filename = substr($i, 10, length($i)-10)
#         }
#     }
#     getline; getline; print
# }' /dev/stdin)

# # Save the file to the server
# file_path="$upload_dir/$filename"
# echo "$file_content" > "$file_path"

# echo "Content-Type: text/plain"
# echo ""
# echo "File uploaded successfully: $filename"

#!/bin/bash

# Send content type header
echo "Content-type: text/html"
echo

# Configuration
upload_dir="config/uploads"
max_file_size=$((10 * 1024 * 1024))  # 10MB limit

# Create upload directory if it doesn't exist
if [[ ! -d "$upload_dir" ]]; then
    mkdir -p "$upload_dir"
    chmod 755 "$upload_dir"
fi

# Function to decode URL-encoded strings
urldecode() {
    echo -e "${1//+/ }" | sed 's/%\([0-9A-F][0-9A-F]\)/\\x\1/g'
}

# Function to clean filename
clean_filename() {
    echo "$1" | sed 's/[^a-zA-Z0-9._-]/_/g'
}

# Read the content type header to get the boundary
read -r content_type
boundary=$(echo "$content_type" | sed -n 's/.*boundary=\(.*\)/\1/p')

# Read and process the multipart form data
temp_file=$(mktemp)
cat > "$temp_file"

# Extract filename and content
filename=$(grep -m 1 -i "Content-Disposition: .*filename=" "$temp_file" | sed -n 's/.*filename="\([^"]*\)".*/\1/p')
filename=$(clean_filename "$(urldecode "$filename")")

if [ -z "$filename" ]; then
    echo "<h2>Error: No file was uploaded</h2>"
    rm "$temp_file"
    exit 1
fi

# Extract the file content
sed -n "/${boundary}/,\$p" "$temp_file" | sed -e "1,/^\r$/d" -e "\$d" -e "\$d" > "${upload_dir}/${filename}"

# Check if file was successfully created
if [ -f "${upload_dir}/${filename}" ]; then
    file_size=$(stat -f%z "${upload_dir}/${filename}" 2>/dev/null || stat -c%s "${upload_dir}/${filename}")
    
    if [ "$file_size" -gt "$max_file_size" ]; then
        rm "${upload_dir}/${filename}"
        echo "<h2>Error: File size exceeds maximum limit of 10MB</h2>"
    else
        chmod 644 "${upload_dir}/${filename}"
        echo "<h2>File uploaded successfully</h2>"
        echo "<p>Filename: ${filename}</p>"
        echo "<p>Size: ${file_size} bytes</p>"
    fi
else
    echo "<h2>Error: Failed to upload file</h2>"
fi

# Clean up
rm "$temp_file"