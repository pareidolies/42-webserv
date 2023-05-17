/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdesseau <sdesseau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 18:41:57 by sdesseau          #+#    #+#             */
/*   Updated: 2023/05/17 16:21:09 by sdesseau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void print_headers(const std::map<std::string, std::string>& headers)
{
    std::cout << "Headers :" << std::endl;

    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    // Vérifier la présence de la clé "Content-Type"
    std::map<std::string, std::string>::const_iterator it = headers.find("Content-Type");
    if (it == headers.end())
    {
        std::cerr << "Erreur : la clé Content-Type n'est pas présente dans les headers." << std::endl;
        return;
    }
    std::cout << "Content-Type: " << it->second << std::endl;
}

bool parse_request(Request &m_request, char *buffer) {
    std::stringstream request_stream;                                                                                   
    request_stream << buffer;                                                                      
    std::string line;                                                                               
    bool headers_done = false;   
    // std::cout << "BUFFER :::: "<< request_stream.str() << "END BUFFER          " << std::endl;  
    if (std::getline(request_stream, line) && !line.empty())
    {
        m_request.method = line.substr(0, line.find(" "));
                m_request.uri = line.substr(line.find(" ") + 1, line.rfind(" ") - line.find(" ") - 1); 
    }                                                                 
    while (std::getline(request_stream, line) && !line.empty()) {                 
        // std::cout << "LINE = " << line << std::endl;
            if (line.find(":") != std::string::npos) {
                // std::cout << "rentre dans find" << std::endl;
                std::string header_name = line.substr(0, line.find(":"));                           
                std::string header_value = line.substr(line.find(":") + 1);                         
                m_request.headers[header_name] = header_value;                             
            }         
    }
    // std::cout << "URI : " << m_request.uri << ", METHOD : " << m_request.method << std::endl;
    print_headers(m_request.headers);
    return (true);                                                                                  
}
