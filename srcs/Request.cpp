/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdesseau <sdesseau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 18:41:57 by sdesseau          #+#    #+#             */
/*   Updated: 2023/05/02 19:07:39 by sdesseau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Request& parse_request(Request &m_request, char *m_buffer) {
    std::stringstream request_stream;                                                                                   
    request_stream << m_buffer;                                                                      
    std::string line;                                                                               
    bool headers_done = false;                                                                      
    while (std::getline(request_stream, line) && !line.empty()) {                                   
        if (!headers_done) {                                                                        
            if (line.find(":") != std::string::npos) {                                              
                std::string header_name = line.substr(0, line.find(":"));                           
                std::string header_value = line.substr(line.find(":") + 1);                         
                m_request.headers[header_name] = header_value;                                       
            } else {                                                                                
                headers_done = true;                                                                
                m_request.method = line.substr(0, line.find(" "));                                   
                m_request.uri = line.substr(line.find(" ") + 1, line.rfind(" ") - line.find(" ") - 1); 
            }
        } else {                                                                                    
            m_request.body += line + "\n";                                                           
        }
    }
    // std::cout << "URI : " << m_request.uri << ", METHOD : " << m_request.method << std::endl;
    return (m_request);                                                                                  
}
