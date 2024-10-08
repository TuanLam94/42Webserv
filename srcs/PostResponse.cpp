#include "../headers/response.hpp"

void Response::handlePostResponse()
{
    if (_request.getStatusCode()) {
        switch (_request.getStatusCode()) {
            case 400:
                handleBadRequest();
                break;
            case 405:
                handleMethodNotallowed();
                break;
            case 505:
                handleVersionNotSupported();
                break;
        }
    }
    else if (checkPostResponse()) {
        CreateFile();
    }
    else {
        handleConflict();
    }

    

    //check request status code
    //  400 bad request
    //  405 method not allowd
    //  505 http method not supported
    //      -> page d'erreur
    //
    //check permission
    //creer le fichier
    //-> renvoyer 201 Created
    //-> renvoyer 409 Conflict si existe deja ou quoi
    // Redirections ? 
}