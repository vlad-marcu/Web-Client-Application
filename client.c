#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>      
#include <arpa/inet.h>
#include <stdlib.h>
#include "parson.h"
#include "helpers.h"
#include "requests.h"


int main(int argc, char *argv[]){   
	char command[BUFLEN];
	char* url;
	char* message;
	char* received_message;
	char* credentials[1];
	credentials[0] = calloc(BUFLEN, sizeof(char));
	char* error;
	char* cookie = calloc(BUFLEN, sizeof(char));
	char* ret;
	char* token = calloc(BUFLEN, sizeof(char));
	int sockfd;
   
   while(1) {

    printf("Tell me what to do: ");
    scanf("%s", command);
    
    if (strcmp(command, "register") == 0) {


        char username[BUFLEN];
        char password[BUFLEN];
    	sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
    	//Verificam daca s-a realizat conexiunea cu succes
    	if (sockfd < 0) {
    		perror("Eroare la conectare");
    	}
    	printf("username=");
    	scanf("%s", username);
    	printf("password=");
    	scanf("%s", password);
    	url = "/api/v1/tema/auth/register";

        //Transformam userul si password-ul in format JSON
    	JSON_Value *value = json_value_init_object();
        JSON_Object *object = json_value_get_object(value);
        json_object_set_string(object, "username", username);
        json_object_set_string(object, "password", password);
        credentials[0] = json_serialize_to_string_pretty(value);

        message = compute_post_request("34.118.48.238", url, "application/json", credentials, 1,NULL, 0, NULL);
        send_to_server(sockfd, message);
        received_message = receive_from_server(sockfd);

        //Verificam daca am intampinat erori
        error = strchr(received_message, '{');
        JSON_Value *parsed_value = json_parse_string(error);
        JSON_Object *get_obj = json_value_get_object(parsed_value);

            
        const char* error_identifier = json_object_dotget_string(get_obj, "error");
          if (error_identifier == NULL) {
                printf("Congratulations, you just registered !\r\n");
            } else {
                printf("Error: %s\r\n", error_identifier);
            }

     
    
         close_connection(sockfd);

    }
    if (strcmp(command, "login") == 0) {
    	char username[BUFLEN];
        char password[BUFLEN];
        //Verificam daca s-a realizat conexiunea cu succes
    	sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
    	if (sockfd < 0) {
    		perror("Eroare la conectare");
    	}
    	printf("username=");
    	scanf("%s", username);
    	printf("password=");
    	scanf("%s", password);
    	url = "/api/v1/tema/auth/login";

        //Transformam userul si password-ul in format JSON
    	JSON_Value *value = json_value_init_object();
        JSON_Object *object = json_value_get_object(value);
        json_object_set_string(object, "username", username);
        json_object_set_string(object, "password", password);
        credentials[0] = json_serialize_to_string_pretty(value);

        message = compute_post_request("34.118.48.238", url, "application/json", credentials, 1,NULL, 0, NULL);
        send_to_server(sockfd, message);
        received_message = receive_from_server(sockfd);
        //Extragem cookie-ul
         if((strstr(received_message,"connect.sid=")) != NULL) {

            ret = strstr(received_message,"connect.sid=");
            cookie = strtok(ret,"\r\n");
           
           
        }

         //Verificam daca am intampinat erori
        error = strchr(received_message, '{');
        JSON_Value *parsed_value = json_parse_string(error);
        JSON_Object *get_obj = json_value_get_object(parsed_value);

        const char* error_identifier = json_object_dotget_string(get_obj, "error");
          if (error_identifier == NULL) {
                printf("Congratulations, you just logged in !\r\n");
            } else {
                printf("Error: %s\r\n", error_identifier);
            }
          
         close_connection(sockfd);


    }
    if (strcmp(command, "enter_library") == 0) {

        //Verificam daca s-a realizat conexiunea cu succes
    	sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
    	if (sockfd < 0) {
    		perror("Eroare la conectare");
    	}
    	memset(credentials[0], 0, BUFLEN);
    	memcpy(credentials[0], cookie, BUFLEN);
    	url = "/api/v1/tema/library/access";
    	message = compute_get_request("34.118.48.238", url, NULL, credentials, 1, NULL);
        send_to_server(sockfd, message);
        received_message = receive_from_server(sockfd);

         //Verificam daca am intampinat erori
        error = strchr(received_message, '{');
        JSON_Value *parsed_value = json_parse_string(error);
        JSON_Object *get_obj = json_value_get_object(parsed_value);

        const char* error_identifier = json_object_dotget_string(get_obj, "error");
            if (error_identifier == NULL) {

                const char* received_token = json_object_dotget_string(get_obj, "token");
                //Ne salvam token-ul JWT
                memcpy(token, received_token, BUFLEN);
                printf("You entered the library !\r\n");
            } else {
                
                printf("Error: %s\r\n", error_identifier);
            }

        
         close_connection(sockfd);
            
    }
    if (strcmp(command, "get_books") == 0) {
    	//Verificam daca s-a realizat conexiunea cu succes
    	sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
    	if (sockfd < 0) {
    		perror("Eroare la conectare");
    	}
    	memset(credentials[0], 0, BUFLEN);
    	memcpy(credentials[0], token, BUFLEN);
    	url = "/api/v1/tema/library/books";
    	message = compute_get_request("34.118.48.238", url, NULL, NULL, 0, credentials[0]);
        send_to_server(sockfd, message);
        received_message = receive_from_server(sockfd);

        //Verificam daca am intampinat erori
        error = strchr(received_message, '{');
        JSON_Value *parsed_value = json_parse_string(error);
        JSON_Object *get_obj = json_value_get_object(parsed_value);
        const char* error_identifier = json_object_dotget_string(get_obj, "error");
        if (error_identifier == NULL) {
        	error = strchr(received_message, '[');
        	JSON_Value *parsed_value = json_parse_string(error);
            JSON_Array *list = json_value_get_array(parsed_value);
            size_t count = json_array_get_count(list);
            if (count != 0) {
            for (int i = 0; i < count; i++) {
                        //Printam informatiile despre carti
                        JSON_Object *get_obj = json_array_get_object(list, i);
                        const double id = json_object_dotget_number(get_obj, "id");
                        const char* title = json_object_dotget_string(get_obj, "title");            
                        printf("Id: %d,\r\n", (int)id);
                        printf("Title: %s\r\n\r\n", title);
                    }
                   

        } else {
        	printf("%s\r\n", "The library is empty !");
        } 
    }
    else {

        	printf("Error: %s\r\n", error_identifier);
        }
        
        close_connection(sockfd);



    }
    if (strcmp(command, "get_book") == 0) {
    	//Verificam daca s-a realizat conexiunea cu succes
    	 sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
    	 if (sockfd < 0) {
    		perror("Eroare la conectare");
    	}
    	 char id[BUFLEN];
    	 char new_url[BUFLEN];
    	 memset(new_url, 0, BUFLEN);
    	 printf("id=");
    	 scanf("%s", id);
    	 strcat(new_url, "/api/v1/tema/library/books/");
    	 strcat(new_url, id);
    	 memcpy(credentials[0], token, BUFLEN);
    	 message = compute_get_request("34.118.48.238", new_url, NULL, NULL, 0, credentials[0]);
         send_to_server(sockfd, message);
         received_message = receive_from_server(sockfd);
         //Verificam daca am intampinat erori
         error = strchr(received_message, '{');
         JSON_Value *parsed_value = json_parse_string(error);
         JSON_Object *get_obj = json_value_get_object(parsed_value);
         const char* error_identifier = json_object_dotget_string(get_obj, "error");
         if (error_identifier == NULL) {
         	        //Printam informatiile despre carte
         	        const char* title = json_object_dotget_string(get_obj, "title");
                    const char* author = json_object_dotget_string(get_obj, "author");
                    const char* publisher  = json_object_dotget_string(get_obj, "publisher");
                    const char* genre = json_object_dotget_string(get_obj, "genre");
                    const double page_count = json_object_dotget_number(get_obj, "page_count");

                    printf("ID: %s,\r\n", id);
                    printf("Title: %s,\r\n", title);
                    printf("Author: %s,\r\n", author);
                    printf("Publisher: %s,\r\n", publisher);
                    printf("Genre: %s,\r\n", genre);
                    printf("Page count: %d\r\n", (int) page_count);
                    
                } else {

                	printf("Error: %s\n\n", error_identifier);
                   
                }
    	 
    }
    if (strcmp(command, "add_book") == 0) {
    	//Verificam daca s-a realizat conexiunea cu succes
    	sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
    	if (sockfd < 0) {
    		perror("Eroare la conectare");
    	}
    	char field[BUFLEN];
    	JSON_Value *value = json_value_init_object();
        JSON_Object *object = json_value_get_object(value);
        printf("title=");
        scanf("%s", field);
        json_object_set_string(object, "title", field);
        printf("author=");
        scanf("%s", field);
        json_object_set_string(object, "author", field);
        printf("genre=");
        scanf("%s", field);
        json_object_set_string(object, "genre", field);
        printf("page_count=");
        scanf("%s", field);
        json_object_set_string(object, "page_count", field);
        printf("publisher=");
        scanf("%s", field);
        json_object_set_string(object, "publisher", field);
        memcpy(credentials[0], token, BUFLEN);
        url = "/api/v1/tema/library/books";
        char* book[1];
	    book[0] = calloc(BUFLEN, sizeof(char));
	    memcpy(book[0], json_serialize_to_string_pretty(value), BUFLEN);
	    message = compute_post_request("34.118.48.238", url, "application/json", book, 1,NULL,0, credentials[0]);
	    send_to_server(sockfd, message);
        received_message = receive_from_server(sockfd);
         //Verificam daca am intampinat erori
        error = strchr(received_message, '{');
        JSON_Value *parsed_value = json_parse_string(error);
        JSON_Object *get_obj = json_value_get_object(parsed_value);
        const char* error_identifier = json_object_dotget_string(get_obj, "error");
        if (error_identifier == NULL) {
                   printf("The book has been added to the library!\r\n");
                } else {
                   printf("Error: %s\r\n", error_identifier);
                }
         close_connection(sockfd);



    }
    if (strcmp(command, "delete_book") == 0) {
    	//Verificam daca s-a realizat conexiunea cu succes
    	sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
    	if (sockfd < 0) {
    		perror("Eroare la conectare");
    	}
    	char id[BUFLEN];
    	char new_url[BUFLEN];
    	memset(new_url, 0, BUFLEN);
    	printf("id=");
    	scanf("%s", id);
    	strcat(new_url, "/api/v1/tema/library/books/");
    	strcat(new_url, id);
    	memcpy(credentials[0], token, BUFLEN);
    	message = compute_delete_request("34.118.48.238", new_url, NULL, NULL, 0, credentials[0]);
        send_to_server(sockfd, message);
        received_message = receive_from_server(sockfd);
         //Verificam daca am intampinat erori
        error = strchr(received_message, '{');
        JSON_Value *parsed_value = json_parse_string(error);
        JSON_Object *get_obj = json_value_get_object(parsed_value);
        const char* error_identifier = json_object_dotget_string(get_obj, "error");
        if (error_identifier == NULL) {
                    printf("The book has been deleted from the library!\r\n");
                } else {
                    printf("Error: %s\r\n", error_identifier);
                }
                
        
         close_connection(sockfd);


    }
    if (strcmp(command, "logout") == 0) {
    	//Verificam daca s-a realizat conexiunea cu succes
    	sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
    	if (sockfd < 0) {
    		perror("Eroare la conectare");
    	}
        url = "/api/v1/tema/auth/logout";
        memset(credentials[0], 0, BUFLEN);
        memcpy(credentials[0], cookie, BUFLEN);
        message = compute_get_request("34.118.48.238", url, NULL, credentials, 1, NULL);
        send_to_server(sockfd, message);
        received_message = receive_from_server(sockfd);
        //Verificam daca am intampinat erori
        error = strchr(received_message, '{');
        JSON_Value *parsed_value = json_parse_string(error);
        JSON_Object *get_obj = json_value_get_object(parsed_value);
        const char* error_identifier = json_object_dotget_string(get_obj, "error");
        if (error_identifier == NULL) {
                printf("You have been logged out\r\n");
                memset(token,0, BUFLEN);
                memset(cookie,0, BUFLEN);
                memset(credentials[0], 0, BUFLEN);
                
            } else {
                printf("Error: %s\r\n", error_identifier);
            }
            
        close_connection(sockfd);
    }
    if (strcmp(command, "exit") == 0) {

    	return 0;
    }
}




   }












