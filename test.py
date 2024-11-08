import socket

host = 'localhost'
port = 8080

# Créer la connexion
with socket.create_connection((host, port)) as s:
    # En-têtes et corps en chunked encoding
    request = (
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: custom-client\r\n"
        "Accept: */*\r\n"
        "\r"
    )
    # Envoyer la requête
    s.sendall(request.encode())
    # Recevoir la réponse
    response = s.recv(4096)
    print(response.decode())

