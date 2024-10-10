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

def test_json():
    print("Testing malformed JSON in POST request")
    malformed_json = '{    "name"         : "John", "name": "25" }'  # Missing quotes around 'age'
    response = requests.post(SERVER_URL, data=malformed_json, headers={"Content-Type": "application/json"})
    print(f"Status code: {response.status_code}")

if __name__ == "__main__":  
    # test_get()
    # test_post()
    # test_put()
    # test_delete()
    # test_404()
    test_json()
