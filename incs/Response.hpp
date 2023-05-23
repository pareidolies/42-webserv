#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <fstream>
#include <string>   ///read file

class Response {
    public:

        Response(Client client);	
		~Response(void);
        bool send_response();

    private:
        Client								_client;
        Location							_location;
		Server								*_server;
        std::string							_extension;
        std::map<int, std::string>			_status_code_list;
        int									_status_code;
        std::string							_header_fields;
        std::string							_body;
		std::string							_path;
		std::string							_syscall_error;
        bool                                _get;
        bool                                _post;
        bool                                _delete;
        std::string                         _method;
        std::string                         _content_type;
        std::string                         _request_body;

        void								init_code_msg();
        void								get_body();
        void								set_autoindex_body();
        void								delete_file();
        bool								post_body();
        std::string							get_code_msg();
		std::string							get_file_content(std::string content);
        void								get_header_fields(int cont_Leng) ;
		bool								send_error_response();
		bool								send_successful_response();
		bool								set_defined_error_page();
		void								set_default_error_page();
        std::string content_mime_type(std::string extension);
        void get_autoindex_body();
};

#endif