// Akshat Nehra 110123650 | Noor Raza 110121961

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PORT 12345
#define SERVER_IP "127.0.0.1"

void sendCommand(int client_socket, const char* command) {
    send(client_socket, command, strlen(command), 0);
}

void handleGetFn(int client_socket) {
    // Receive the initial response
    char buffer[1024];
    int n = recv(client_socket, buffer, sizeof(buffer), 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server response: %s\n", buffer);
    }
}

void receiveAndSaveTarFile(int client_socket) {
    char buffer[1024];
    int n;

    // Receive the initial response
    n = recv(client_socket, buffer, sizeof(buffer), 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server response: %s\n", buffer);
    }

    // Check for "Start file transfer"
    if (strcmp(buffer, "Start file transfer") == 0) {
        // Receive the tar file size
        long tarFileSize;
        recv(client_socket, &tarFileSize, sizeof(long), 0);

        // Create a directory to store received files if it doesn't exist
        if (mkdir("received_files", 0700) != 0) {
            perror("Error creating directory");
            return;
        }

        // Create and open the tar file for writing
        FILE* tarFile = fopen("received_files/client_files.tar", "wb");
        if (tarFile == NULL) {
            perror("Error opening tar file for writing");
            return;
        }

        // Receive and write the tar file content in chunks
        size_t bytesReceived;
        while (tarFileSize > 0) {
            size_t bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytesRead > 0) {
                fwrite(buffer, 1, bytesRead, tarFile);
                tarFileSize -= bytesRead;
            }
        }

        fclose(tarFile);

        printf("Received tar file: client_files.tar\n");
    }

    // Check for the end of response
    n = recv(client_socket, buffer, sizeof(buffer), 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server response: %s\n", buffer);
    }
}

void handleGetFz(int client_socket) {
    char buffer[1024];
    int n;

    // Receive the initial response
    n = recv(client_socket, buffer, sizeof(buffer), 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server response: %s\n", buffer);
    }

    // Check for "Start file transfer"
    if (strcmp(buffer, "Start file transfer") == 0) {
        // Create a directory to store received files if it doesn't exist
        if (mkdir("f23Project", 0700) != 0) {
            perror("Error creating directory");
            return;
        }

        // Create and open the tar file for writing
        char filename[256];
        sprintf(filename, "f23Project/temp.tar.gz");

        FILE* tarFile = fopen(filename, "wb");
        if (tarFile == NULL) {
            perror("Error opening tar file for writing");
            return;
        }

        // Receive and write the tar file content in chunks
        size_t bytesReceived;
        while ((bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
            fwrite(buffer, 1, bytesReceived, tarFile);
        }

        fclose(tarFile);

        // Inform the client that the file transfer is complete
        send(client_socket, "End file transfer", sizeof("End file transfer"), 0);

        // Inform the client about successful file transfer
        printf("Received tar file: %s\n", filename);
    } else if(strcmp(buffer, "No file found") == 0) {
        printf("No file found\n");
        return;
    }
}




void handleGetFt(int client_socket) {
    char buffer[1024];
    int n;

    // Receive the initial response
    n = recv(client_socket, buffer, sizeof(buffer), 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server response: %s\n", buffer);
    }

    // Check for "Start file transfer"
    if (strcmp(buffer, "Start file transfer") == 0) {
        // Create a directory to store received files if it doesn't exist
        if (mkdir("f23Project", 0700) != 0) {
            perror("Error creating directory");
            return;
        }

        // Create and open the tar file for writing
        char filename[256];
        sprintf(filename, "f23Project/temp.tar.gz");

        FILE* tarFile = fopen(filename, "wb");
        if (tarFile == NULL) {
            perror("Error opening tar file for writing");
            return;
        }

        // Receive and write the tar file content in chunks
        size_t bytesReceived;
        while ((bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
            fwrite(buffer, 1, bytesReceived, tarFile);
        }

        fclose(tarFile);

        // Inform the client that the file transfer is complete
        send(client_socket, "End file transfer", sizeof("End file transfer"), 0);

        // Inform the client about successful file transfer
        printf("Received tar file: %s\n", filename);
    } else if(strcmp(buffer, "No file found") == 0) {
        printf("No file found\n");
        return;
    }

    // Check for the end of response
    n = recv(client_socket, buffer, sizeof(buffer), 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server response: %s\n", buffer);
    }
}




void handleGetFdb(int client_socket) {
    // Receive the initial response
    char buffer[1024];
    int n = recv(client_socket, buffer, sizeof(buffer), 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server response: %s\n", buffer);
    } else {
        perror("Error receiving initial response");
        return;
    }

    // Check for "Start file transfer"
    if (strcmp(buffer, "Start file transfer") == 0) {
        // Create a directory to store received files if it doesn't exist
        if (mkdir("f23Project", 0700) != 0) {
                perror("Error creating directory");
                return;
        }

        // Create and open the tar file for writing
        char filename[256];
        sprintf(filename, "f23Project/temp.tar.gz");

        FILE* tarFile = fopen(filename, "wb");
        if (tarFile == NULL) {
            perror("Error opening tar file for writing");
            return;
        }

        // Receive and write the tar file content in chunks
        while (1) {
            ssize_t bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytesRead > 0) {
                size_t totalWritten = 0;
                while (totalWritten < bytesRead) {
                    size_t bytesWritten = fwrite(buffer + totalWritten, 1, bytesRead - totalWritten, tarFile);
                    if (bytesWritten < bytesRead) {
                        perror("Error writing to tar file");
                        break;
                    }
                    totalWritten += bytesWritten;
                }
            } else if (bytesRead == 0) {
                // Connection closed by the server
                break;
            } else if (bytesRead == -1) {
                perror("Error receiving file content");
                break;
            }
        }

        fclose(tarFile);

        // Inform the client that the file transfer is complete
        if (send(client_socket, "End file transfer", sizeof("End file transfer"), 0) == -1) {
            perror("Error sending end file transfer message");
            return;
        }

        // Inform the client about successful file transfer
        printf("Received tar file: %s\n", filename);
    } else if (strcmp(buffer, "No file found") == 0) {
        printf("No file found\n");
    }
}



void handleGetFda(int client_socket) {
    // Receive the initial response
    char buffer[1024];
    int n = recv(client_socket, buffer, sizeof(buffer), 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server response: %s\n", buffer);
    }

    // Check for "Start file transfer"
    if (strcmp(buffer, "Start file transfer") == 0) {
        // Receive the tar file size
        long tarFileSize;
        recv(client_socket, &tarFileSize, sizeof(long), 0);

        // Create a directory to store received files if it doesn't exist
        if (mkdir("f23Project", 0700) != 0) {
            perror("Error creating directory");
            return;
        }

        // Create and open the tar file for writing
        char filename[256];
        sprintf(filename, "f23Project/temp.tar.gz");

        FILE* tarFile = fopen(filename, "wb");
        if (tarFile == NULL) {
            perror("Error opening tar file for writing");
            return;
        }

        // Receive and write the tar file content in chunks
        size_t bytesReceived;
        while (tarFileSize > 0) {
            size_t bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytesRead > 0) {
                fwrite(buffer, 1, bytesRead, tarFile);
                tarFileSize -= bytesRead;
            }
        }

        fclose(tarFile);

        // Inform the client that the file transfer is complete
        send(client_socket, "End file transfer", sizeof("End file transfer"), 0);

        // Inform the client about successful file transfer
        printf("Received tar file: %s\n", filename);
    } else if(strcmp(buffer, "No file found") == 0) {
        printf("No file found\n");
        return;
    }

    // Check for the end of response
    n = recv(client_socket, buffer, sizeof(buffer), 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server response: %s\n", buffer);
    }
}

int main() {
    

    // Client loop
    while (1) {
        // Set up the client socket
        int client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1) {
            perror("Error creating client socket");
            exit(EXIT_FAILURE);
        }

        // Configure server address
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

        // Connect to the server
        if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
            perror("Error connecting to server");
            exit(EXIT_FAILURE);
        }

        printf("Connected to the server\n");
        // Get user input
        printf("Enter command: ");
        char command[256];
        fgets(command, sizeof(command), stdin);

        // Remove newline character
        command[strcspn(command, "\n")] = '\0';

        // Validate and send the command to the server
        if (strcmp(command, "quitc") == 0) {
            sendCommand(client_socket, command);
            break;
        } else if(strcmp(command, "gettar") == 0) {
            sendCommand(client_socket, command);

            // Receive and print server responses
            receiveAndSaveTarFile(client_socket);
        } else if(strncmp(command, "getfn ", 6) == 0) {
            printf("Command: %s\n", command);
            sendCommand(client_socket, command);

            // Receive and print server responses
            handleGetFn(client_socket);
        } else if(strncmp(command, "getfz ", 6) == 0) {
            printf("Command: %s\n", command);
            sendCommand(client_socket, command);

            // Receive and print server responses
            handleGetFz(client_socket);
        } else if (strncmp(command, "getft ", 6) == 0) {
            printf("Command: %s\n", command);

            // Extract the file extensions from the command
            char extensions[256];
            sscanf(command, "getft %[^\n]", extensions);

            // Tokenize the extensions
            char* token = strtok(extensions, " ");
            int extensionCount = 0;
            while (token != NULL) {
                extensionCount++;
                token = strtok(NULL, " ");
            }

            if (extensionCount == 0 || extensionCount > 3) {
                // Invalid number of extensions
                printf("Invalid number of extensions: %d\n", extensionCount);
                send(client_socket, "Invalid number of extensions", sizeof("Invalid number of extensions"), 0);
            }

            // Valid command, proceed
            sendCommand(client_socket, command);

            // Receive and print server responses
            handleGetFt(client_socket);
        } else if(strncmp(command, "getfdb ", 7) == 0) {
            printf("Command: %s\n", command);
            sendCommand(client_socket, command);

            // Receive and print server responses
            handleGetFdb(client_socket);
        } else if(strncmp(command, "getfda ", 7) == 0) {
            printf("Command: %s\n", command);
            sendCommand(client_socket, command);

            // Receive and print server responses
            handleGetFdb(client_socket);
        } else {
            printf("Invalid command\n");
        }

        // Close the client socket
        close(client_socket);
    }

    return 0;
}
