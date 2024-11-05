import socket

host = 'localhost'
port = 8080

# Créer la connexion
with socket.create_connection((host, port)) as s:
    # En-têtes et corps en chunked encoding
    request = (
        "POST /your_endpoint HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: custom-client\r\n"
        "Accept: */*\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "4\r\n"
        "Wiki\r\n"
        "5\r\n"
        "pedia\r\n"
        "in\r\n"
        "chunks.\r\n"
        "0\r\n"
        "\r\n"
    )
    # Envoyer la requête
    s.sendall(request.encode())
    # Recevoir la réponse
    response = s.recv(4096)
    print(response.decode())

