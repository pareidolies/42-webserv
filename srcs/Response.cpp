/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdesseau <sdesseau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 18:44:28 by sdesseau          #+#    #+#             */
/*   Updated: 2023/05/17 19:12:49 by sdesseau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <sstream> // pour std::ostringstream

std::string search_content_type(std::string filename);

std::string get_boundary(const std::string& request)
{
    // Chercher la clé "Content-Type" dans la requête
    size_t pos = request.find("Content-Type: multipart/form-data;");
    if (pos != std::string::npos)
    {
        // Extraire la valeur du paramètre "boundary"
        pos = request.find("boundary=", pos);
        if (pos != std::string::npos) {
            pos += 9;
            size_t end_pos = request.find("\r\n", pos);
            std::string boundary = request.substr(pos, end_pos - pos);
            // std::cout << "BOUNDARY = " << boundary << std::endl;
            return ("--" + boundary);
        }
    }
    return ("");
}

std::string get_filename(const std::string& content)
{
    std::string filename;

    // Recherche de la ligne "Content-Disposition" qui contient le nom de fichier
    std::string disposition = "Content-Disposition: form-data; name=\"file\"; filename=\"";
    std::size_t dispositionStart = content.find(disposition);
    if (dispositionStart != std::string::npos) {
        // Recherche du début et de la fin du nom de fichier dans la ligne
        std::size_t filenameStart = dispositionStart + disposition.length();
        std::size_t filenameEnd = content.find("\"", filenameStart);
        if (filenameEnd != std::string::npos) {
            // Extrait le nom de fichier de la ligne
            filename = content.substr(filenameStart, filenameEnd - filenameStart);
        }
    }
    return (filename);
}

void save_file(const std::string& path, const std::string& content)
{
    std::ofstream file(path.c_str());
    if (file.is_open())
    {
        file << content;
        file.close();
    }
}

template <typename T>
std::string to_string_custom(const T& value)
{
    std::ostringstream os;
    os << value;
    return (os.str());
}

std::string read_file(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        return ("");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return (buffer.str());
}

std::string find_body(const std::string& request, const std::string& boundary)
{
    std::string data;

    // Find the starting position of the first boundary
    std::string::size_type start = request.find(boundary);
    if (start == std::string::npos) {
        // Boundary not found
        return (data);
    }

    // Find the ending position of the first boundary
    std::string::size_type end = request.find(boundary, start + boundary.length());
    if (end == std::string::npos) {
        // Boundary not found
        return (data);
    }

    // Extract the data between the boundaries
    data = request.substr(start + boundary.length(), end - (start + boundary.length()));

    return (data);
}

void removeFirstFourLines(std::string& data)
{
    // Find the position of the fourth occurrence of "\r\n"
    std::string::size_type lineStart = 0;
    for (int i = 0; i < 4; ++i) {
        lineStart = data.find("\r\n", lineStart);
        if (lineStart == std::string::npos) {
            // Not enough lines to skip
            return ;
        }
        lineStart += 2; // Move past the current "\r\n"
    }

    // Erase the first 4 lines
    if (lineStart != std::string::npos) {
        data.erase(0, lineStart);
    }
}

std::string get_file_extension(const std::string& filename)
{
    size_t dotIndex = filename.find_last_of(".");
    if (dotIndex != std::string::npos && dotIndex < filename.length() - 1)
    {
        return filename.substr(dotIndex);
    }
    return "";
}

bool is_cgi_script(const std::string& uri)
{
    // Ajoutez ici la logique spécifique pour déterminer si l'URI correspond à un script CGI.
    // Par exemple, vous pouvez vérifier si l'extension du fichier est associée à des scripts CGI, tels que .cgi ou .pl.
    size_t queryIndex = uri.find("?");
    if (queryIndex != std::string::npos)
    {
    std::string extension = get_file_extension(uri);

    // Vérifie si l'extension du fichier est dans la liste des extensions CGI
        if (extension == ".cgi" || extension == ".pl")
        {
            return true;
        }
    }
    return false;
}

std::string process_request(const Request& request) 
{
    std::cout << "Method: " << request.method << std::endl;
    std::string response;
    if (request.method == "GET")
    {
        // Traitement de la requête GET
        // Vérifiez si l'URI correspond à un script CGI
        if (is_cgi_script(request.uri))
        {
            // Exécutez le script CGI
            Configuration	conf("./conf_files/default.conf");
            conf.open_and_read_file();
            conf.init_config();
            CGI cgi(conf);
            int cgi_output = cgi.execute();

            // Construisez la réponse en utilisant la sortie du script CGI
            std::string body_size = to_string_custom(cgi.getBody().size());
            response = "HTTP/1.1 " + to_string_custom(cgi_output) + " OK\r\nContent-Type:text/html\r\nContent-Length: " + body_size + "\r\n\r\n" + cgi.getBody();
        }
        else if (request.uri == "/")
        {
            std::string body = read_file("www/site/pages/index.html");
            std::string body_size = to_string_custom(body.size());
            response = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length: " + body_size + "\r\n\r\n" + body;
        }
        else
        {
            std::cout << "URI: " << request.uri << std::endl;
            // Si l'URI est différent de "/", renvoyer le fichier correspondant
            std::string filename = request.uri.substr(1); // Supprimer le premier caractère "/"
            std::string body = read_file(filename);
            if (body == "") {
                // Si le fichier n'existe pas, renvoyer une réponse 404
                std::string content_type = "text/html";
				body = read_file("www/site/errorPages/404.html");
                std::string body_size = to_string_custom(body.size());
                response = "HTTP/1.1 200 OK\r\nContent-Type:";
                response += content_type;
                response += "\r\nContent-Length: " + body_size + "\r\n\r\n" + body;
            }
            else
            {
                std::string content_type = search_content_type(filename);
                std::string body_size = to_string_custom(body.size());
                response = "HTTP/1.1 200 OK\r\nContent-Type:";
                response += content_type;
                response += "\r\nContent-Length: " + body_size + "\r\n\r\n" + body;
            }
        }
    }
    else if (request.method == "DELETE")
    {
        // Traitement de la requête DELETE
        // check authorisations
        response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    }
    else if (request.method == "POST")
    {
        // Traitement de la requête POST
        // Vérifiez si l'URI correspond à un script CGI
        if (is_cgi_script(request.uri))
        {
            // Exécutez le script CGI
            Configuration	conf("./conf_files/default.conf");
            conf.open_and_read_file();
            conf.init_config();
            CGI cgi(conf);
            int cgi_output = cgi.execute();

            // Construisez la réponse en utilisant la sortie du script CGI
            std::string body_size = to_string_custom(cgi.getBody().size());
            response = "HTTP/1.1 " + to_string_custom(cgi_output) + " OK\r\nContent-Type:text/html\r\nContent-Length: " + body_size + "\r\n\r\n" + cgi.getBody();
        }
        else if (request.uri == "/upload")
        {
            // Extraire les données du corps de la requête
            std::string boundary = get_boundary(request.raw_request);
            std::string content = find_body(request.raw_request, boundary);
            std::string filename = get_filename(content);
            removeFirstFourLines(content);

            // Stocker le fichier dans un dossier spécifique
            save_file("www/site/files/downloads/" + filename, content);

            // Envoyer une réponse au client
            std::string body = read_file("www/site/pages/upload_complete.html");
            std::string body_size = to_string_custom(body.size());
            response = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length: " + body_size + "\r\n\r\n" + body;
        }
        else if (request.uri == "/form" || request.uri == "/comment")
        {
            std::string body = read_file("www/site/pages/upload_complete.html");
            std::string body_size = to_string_custom(body.size());
            response = "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nContent-Length: " + body_size + "\r\n\r\n" + body;
        }
        else
        {
            // Si l'URI n'est pas "/upload", renvoyer une réponse 404
            response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        }
    }
    else
    {
        // Requête non prise en charge
        response = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 0\r\n\r\n";
    }
    return (response);
}

std::string search_content_type(std::string filename)
{
    std::string content_type = "text/html";
    int i = filename.length();
    
    if (i >= 5)
    {
        // TEXT TYPE
        if (filename.substr(i - 4, 4) == ".css" )
           content_type = "text/css";
        else if (filename.substr(i - 4, 4) == ".csv")
            content_type = "text/csv";
        else if (filename.substr(i - 5, 5) == ".html")
            content_type = "text/html";
        else if (filename.substr(i - 4, 4) == ".xml")
            content_type = "text/xml";
        else if (filename.substr(i - 5, 5) == ".scss")
            content_type = "text/css";
        else if (filename.substr(i - 4, 4) == ".txt")
            content_type = "text/plain";

        // IMAGE TYPE
        if (filename.substr(i - 4, 4) == ".svg")
            content_type = "image/svg+xml";
        else if (filename.substr(i - 4, 4) == ".gif")
            content_type = "image/gif";
        else if (filename.substr(i - 5, 5) == ".jpeg")
            content_type = "image/jpeg";
        else if (filename.substr(i - 4, 4) == ".png")
            content_type = "image/webp";                 // show png as a webp
        else if (filename.substr(i - 5, 5) == ".tiff")
            content_type = "image/tiff";
        else if (filename.substr(i - 4, 4) == ".webp")
            content_type = "image/webp";
        else if (filename.substr(i - 4, 4) == ".ico")
            content_type = "image/x-icon";
        else if (filename.substr(i - 4, 4) == ".bmp")
            content_type = "image/bmp";
        else if (filename.substr(i - 4, 4) == ".jpg")
            content_type = "image/jpeg";
        else if (filename.substr(i - 4, 4) == ".jpe")
            content_type = "image/jpeg";
        else if (filename.substr(i - 4, 4) == ".jif")
            content_type = "image/jif";
        else if (filename.substr(i - 4, 4) == ".jfif")
            content_type = "image/jfif";
        else if (filename.substr(i - 4, 4) == ".jfi")
            content_type = "image/jfi";
        else if (filename.substr(i - 4, 4) == ".jpx")
            content_type = "image/jpx";
        else if (filename.substr(i - 4, 4) == ".jp2")
            content_type = "image/jp2";
        else if (filename.substr(i - 4, 4) == ".j2k")
            content_type = "image/j2k";
        else if (filename.substr(i - 4, 4) == ".j2c")
            content_type = "image/j2c";
        else if (filename.substr(i - 4, 4) == ".jpc")
            content_type = "image/jpc";
        
        // AUDIO TYPE
        if (filename.substr(i - 4, 4) == ".mp3")
            content_type = "audio/mpeg";
        else if (filename.substr(i - 4, 4) == ".wav")
            content_type = "audio/wav";
        else if (filename.substr(i - 5, 5) == ".flac")
            content_type = "audio/flac";
        else if (filename.substr(i - 4, 4) == ".aac")
            content_type = "audio/aac";
        else if (filename.substr(i - 4, 4) == ".ogg")
            content_type = "audio/ogg";
        else if (filename.substr(i - 4, 4) == ".oga")
            content_type = "audio/oga";
        else if (filename.substr(i - 4, 4) == ".m4a")
            content_type = "audio/m4a";
        else if (filename.substr(i - 4, 4) == ".m4b")
            content_type = "audio/m4b";
        else if (filename.substr(i - 4, 4) == ".m4p")
            content_type = "audio/m4p";
        else if (filename.substr(i - 4, 4) == ".m4r")
            content_type = "audio/m4r";
        else if (filename.substr(i - 4, 4) == ".m4v")
            content_type = "audio/m4v";
        else if (filename.substr(i - 4, 4) == ".m4s")
            content_type = "audio/m4s";
        else if (filename.substr(i - 4, 4) == ".m4a")
            content_type = "audio/m4a";
        
        // VIDEO TYPE
        if (filename.substr(i - 4, 4) == ".mp4")
            content_type = "video/mp4";
        else if (filename.substr(i - 4, 4) == ".m4v")
            content_type = "video/m4v";
        else if (filename.substr(i - 4, 4) == ".m4p")
            content_type = "video/m4p";
        else if (filename.substr(i - 4, 4) == ".m4b")
            content_type = "video/m4b";
        else if (filename.substr(i - 4, 4) == ".m4r")
            content_type = "video/m4r";
        else if (filename.substr(i - 4, 4) == ".m4s")
            content_type = "video/m4s";
        else if (filename.substr(i - 4, 4) == ".m4a")
            content_type = "video/m4a";
        else if (filename.substr(i - 4, 4) == ".m4v")
            content_type = "video/m4v";
        else if (filename.substr(i - 4, 4) == ".m4p")
            content_type = "video/m4p";
        else if (filename.substr(i - 4, 4) == ".m4b")
            content_type = "video/m4b";
        else if (filename.substr(i - 4, 4) == ".m4r")
            content_type = "video/m4r";
        else if (filename.substr(i - 4, 4) == ".m4s")
            content_type = "video/m4s";

        // APPLICATION TYPE
        
        if (filename.substr(i - 4, 4) == ".pdf")
            content_type = "application/pdf";
        else if (filename.substr(i - 4, 4) == ".doc")
            content_type = "application/msword";
        else if (filename.substr(i - 4, 4) == ".docx")
            content_type = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        else if (filename.substr(i - 4, 4) == ".xls")
            content_type = "application/vnd.ms-excel";
        else if (filename.substr(i - 4, 4) == ".xlsx")
            content_type = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        else if (filename.substr(i - 4, 4) == ".ppt")
            content_type = "application/vnd.ms-powerpoint";
        else if (filename.substr(i - 4, 4) == ".pptx")
            content_type = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        else if (filename.substr(i - 4, 4) == ".pps")
            content_type = "application/vnd.ms-powerpoint";
        else if (filename.substr(i - 4, 4) == ".ppsx")
            content_type = "application/vnd.openxmlformats-officedocument.presentationml.slideshow";
        else if (filename.substr(i - 4, 4) == ".odt")
            content_type = "application/vnd.oasis.opendocument.text";
        else if (filename.substr(i - 4, 4) == ".odp")
            content_type = "application/vnd.oasis.opendocument.presentation";
    }
    return (content_type);
}
