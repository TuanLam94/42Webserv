Content-Disposition: form-data; name="file"; filename="json.py"
Content-Type: text/x-python

import requests

SERVER_URL = "http://localhost:8080"

# def test_get():
#     print("Testing GET request")
#     response = requests.get(SERVER_URL)
#     print(f"Status code: {response.status_code}")

# def test_post():
#     print("Testing POST request with data")
#     data = {"name": "John", "age": 25}
#     response = requests.post(SERVER_URL, data=data)
#     print(f"Status code: {response.status_code}")

# def test_put():
#     print("Testing PUT request")
#     json_data = {"name": "John", "age": 25}
#     response = requests.put(f"{SERVER_URL}/resource/1", json=json_data)
#     print(f"Status code: {response.status_code}")

# def test_delete():
#     print("Testing DELETE request")
#     response = requests.delete(f"{SERVER_URL}/resource/1")
#     print(f"Status code: {response.status_code}")

# def test_404():
#     print("Testing 404 on non-existing route")
#     response = requests.get(f"{SERVER_URL}/nonexisting")
#     print(f"Status code: {response.status_code}")

# def test_json1():
#     print("Testing malformed JSON in POST request")
#     malformed_json = '{    "name"  "       : "John", "age": "25"  }'  # Missing quotes around 'age'
#     response = requests.post(SERVER_URL, data=malformed_json, headers={"Content-Type": "application/json"})
#     print(f"Status code: {response.status_code}")

# def test_json2():
#     print("Testing malformed JSON in POST request")
#     malformed_json = '{    "name"         : "John, "age: "25"  }'  # Missing quotes around 'age'
#     response = requests.post(SERVER_URL, data=malformed_json, headers={"Content-Type": "application/json"})
#     print(f"Status code: {response.status_code}")


# if __name__ == "__main__":  
    # test_get()
    # test_post()
    # test_put()
    # test_delete()
    # test_404()
    # test_json1()
    # test_json2()

import http.client

# Connexion à ton serveur
conn = http.client.HTTPConnection("localhost", 8080)

# En-têtes HTTP avec Transfer-Encoding chunked
headers = {
    "Transfer-Encoding": "chunked",
    "Content-Type": "text/plain"
}

# Démarre la requête
conn.putrequest("POST", "/ton-endpoint")
for key, value in headers.items():
    conn.putheader(key, value)
conn.endheaders()

# Envoyer les données en chunks
body_chunks = [
    "4\r\nWiki\r\n",   # Un morceau de 4 octets (Wiki)
    "6\r\npedia \r\n", # Un autre morceau de 6 octets (pedia )
    "e\r\nin\r\n\r\nchunks.\r\n",  # Un autre de 14 octets (in\r\nchunks.\n)
    "0\r\n\r\n"        # Chunk final (0 pour indiquer la fin des chunks)
]

for chunk in body_chunks:
    conn.send(chunk.encode())

# Lire la réponse
# response = conn.getresponse()
# print(response.status, response.reason)
# data = response.read()
# print(data.decode())
# conn.close()