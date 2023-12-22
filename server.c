// Akshat Nehra 110123650 | Noor Raza 110121961

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define CHUNK_SIZE 1024

void handleGetFn(int client_socket, const char* command) {
    // Extract the file name from the command
    char filename[256];
    sscanf(command, "getfn %s", filename);
    printf("Filename: %s\n", filename);

    // Create a buffer for the command output
    char buffer[1024];
    // Use snprintf to format the find command and store the output in the buffer
    snprintf(buffer, sizeof(buffer), "find . -name %s -printf \"Filename: %%f \\nSize: %%s bytes \\nDate created: %%t \\nFile permissions: %%m \\n\"", filename);

    // Open the pipe
    FILE* pipe = popen(buffer, "r");

    if (pipe == NULL) {
        perror("Error opening pipe");
        // Inform the client about the error
        send(client_socket, "Error opening pipe", sizeof("Error opening pipe"), 0);
    } else {
        // Read the file information from the pipe
        size_t bytesRead = fread(buffer, 1, sizeof(buffer), pipe);

        // Close the pipe
        pclose(pipe);

        // Check if the file exists
        if (bytesRead == 0) {
            // Inform the client that the file does not exist
            send(client_socket, "File not found", sizeof("File not found"), 0);
        } else {
            // Send the file information to the client
            send(client_socket, buffer, bytesRead, 0);
        }

        // Inform the server about successful file information
        printf("File information successful: %s\n", buffer);
    }
}

void handleGetFz(int client_socket, const char* command) {
    long size1, size2;

    // Extract the file size from the command
    sscanf(command, "getfz %lld %lld", &size1, &size2);

    // Build the find command to locate files with specified size in the current directory
    char findCommand[512];
    snprintf(findCommand, sizeof(findCommand), "find . -type f -size +%ldc -a -size -%ldc | tar czvf - --files-from -", size1, size2);

    // Print the generated command
    printf("Generated Command: %s\n", findCommand);

    // Open a pipe for reading the output of the command
    FILE* pipe = popen(findCommand, "r");
    if (pipe == NULL) {
        perror("Error opening pipe");
        send(client_socket, "Error creating tar file", sizeof("Error creating tar file"), 0);
        return;
    }

    // Send the "Start file transfer" message
    send(client_socket, "Start file transfer", sizeof("Start file transfer"), 0);

    // Read and send the tar file content in chunks
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), pipe)) > 0) {
        size_t totalSent = 0;
        while (totalSent < bytesRead) {
            ssize_t sentBytes = send(client_socket, buffer + totalSent, bytesRead - totalSent, 0);
            if (sentBytes == -1) {
                perror("Error sending file content");
                break;
            }
            totalSent += sentBytes;
        }
    }

    // Close the pipe
    pclose(pipe);

    // Inform the client that the file transfer is complete
    send(client_socket, "End file transfer", sizeof("End file transfer"), 0);

    // Inform the server about successful file transfer
    printf("File transfer successful: getfz\n");
}


void handleGetFt(int client_socket, const char* command) {
    // Extract the file extensions from the command
    char extensions[256];
    sscanf(command, "getft %[^\n]", extensions);

    // Build the find command to locate files with specified extensions in the current directory
    char findCommand[512];
    snprintf(findCommand, sizeof(findCommand), "find . -type f \\( -name '*.%s'", extensions);

    // Tokenize the extensions
    char* token = strtok(extensions, " ");
    int extensionCount = 0;
    while (token != NULL && extensionCount < 3) {
        snprintf(findCommand + strlen(findCommand), sizeof(findCommand) - strlen(findCommand), " -o -name '*.%s'", token);

        token = strtok(NULL, " ");
        extensionCount++;
    }

    strcat(findCommand, " \\) -print | tar czvf - --files-from -");

    // Open a pipe for reading the output of the command
    FILE* pipe = popen(findCommand, "r");
    if (pipe == NULL) {
        perror("Error opening pipe");
        send(client_socket, "Error creating tar file", sizeof("Error creating tar file"), 0);
        return;
    }

    // Send the "Start file transfer" message
    send(client_socket, "Start file transfer", sizeof("Start file transfer"), 0);

    // Read and send the tar file content in chunks
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), pipe)) > 0) {
        size_t totalSent = 0;
        while (totalSent < bytesRead) {
            ssize_t sentBytes = send(client_socket, buffer + totalSent, bytesRead - totalSent, 0);
            if (sentBytes == -1) {
                perror("Error sending file content");
                break;
            }
            totalSent += sentBytes;
        }
    }

    // Close the pipe
    pclose(pipe);

    // Inform the client that the file transfer is complete
    send(client_socket, "End file transfer", sizeof("End file transfer"), 0);

    // Inform the server about successful file transfer
    printf("File transfer successful: %s\n", extensions);
}




void handleGetFdb(int client_socket, const char* command) {
    // Extract date from the command
    char date[256];
    sscanf(command, "getfdb %s", date);

    // Build the find command to locate files with specified extensions in the current directory
    char findCommand[512];
    snprintf(findCommand, sizeof(findCommand), "find . -newermt '%s' -print | tar czvf - --files-from -", date);

    // Print the generated command
    printf("Generated Command: %s\n", findCommand);

    // Open a pipe for reading the output of the command
    FILE* pipe = popen(findCommand, "r");
    if (pipe == NULL) {
        perror("Error opening pipe");
        send(client_socket, "Error creating tar file", sizeof("Error creating tar file"), 0);
        return;
    }

    // Send the "Start file transfer" message
    send(client_socket, "Start file transfer", sizeof("Start file transfer"), 0);

    // Read and send the tar file content in chunks
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), pipe)) > 0) {
        size_t totalSent = 0;
        while (totalSent < bytesRead) {
            ssize_t sentBytes = send(client_socket, buffer + totalSent, bytesRead - totalSent, 0);
            if (sentBytes == -1) {
                perror("Error sending file content");
                break;
            }
            totalSent += sentBytes;
        }
    }

    // Close the pipe
    pclose(pipe);

    // Inform the client that the file transfer is complete
    send(client_socket, "End file transfer", sizeof("End file transfer"), 0);

    // Inform the server about successful file transfer
    printf("File transfer successful: %s\n", date);
}



void handleGetFda(int client_socket, const char* command) {
    // Extract date from the command
    char date[256];
    sscanf(command, "getfda %s", date);

    // Create a file list
    FILE *fileList = fopen("file_list.txt", "w");
    if (!fileList) {
        perror("Error creating file list");
        exit(EXIT_FAILURE);
    }

    // Build the find command to locate files with specified extensions in the current directory
    char findCommand[512];
    strcpy(findCommand, "find . -newermt '");
    strcat(findCommand, date);
    strcat(findCommand, "' -print > file_list.txt");

    // Execute the find command
    system(findCommand);

    // Generate random number using date for filename
    char filename[256];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(filename, "getfdatarfile_%d-%d-%d_%d-%d-%d.tar.gz", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    char tempcmd[1024];
    // Get all files older than the given date
    sprintf(tempcmd, "find . -maxdepth 5 -type f -newermt '%s' -exec tar -czvf '%s' {} +", date, filename);

    // Print the generated command
    printf("Generated Command: %s\n", tempcmd);

    // Execute the command
   system(tempcmd);

    // // Check the exit status
    // if (result != 0) {
    //     perror("Error running system command");
    //     printf("System command result: %d\n", result);
    //     send(client_socket, "Error running system command", sizeof("Error running system command"), 0);
    //     return;
    // }
    
    // Open the tar file for reading
    FILE* tarFile = fopen(filename, "rb");
    if (tarFile == NULL) {
        perror("Error opening tar file");
        send(client_socket, "Error opening tar file", sizeof("Error opening tar file"), 0);
        return;
    }

    // Send the "Start file transfer" message
    send(client_socket, "Start file transfer", sizeof("Start file transfer"), 0);

    // Send the tar file size to the client
    fseek(tarFile, 0, SEEK_END);
    long tarFileSize = ftell(tarFile);
    rewind(tarFile);
    send(client_socket, &tarFileSize, sizeof(long), 0);

    // Send the tar file content in chunks
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), tarFile)) > 0) {
        send(client_socket, buffer, bytesRead, 0);
    }

    fclose(tarFile);

    // Inform the client that the file transfer is complete
    send(client_socket, "End file transfer", sizeof("End file transfer"), 0);

    // Inform the server about successful file transfer
    printf("File transfer successful: %s\n", filename);

    // Remove the tar file
    remove(filename);
}

void pclientrequest(int client_socket) {
    while (1) {
        // Receive command from the client
        char command[256];

        // Use memset to clear the command buffer
        memset(command, 0, sizeof(command));

        // Receive the command
        int bytes_received = recv(client_socket, command, sizeof(command), 0);

        // Check for connection closure or error
        if (bytes_received <= 0) {
            // Close the client socket and exit the loop if an error or closure occurs
            close(client_socket);
            break;
        }

        // Validate and process the command
        if (strncmp(command, "getfn ", 6) == 0) {
            handleGetFn(client_socket, command);

        } else if(strncmp(command, "getfz ", 6) == 0) {
            handleGetFz(client_socket, command);

        } else if(strncmp(command, "getft ", 6) == 0) {
            handleGetFt(client_socket, command);

        } else if(strncmp(command, "getfdb ", 7) == 0) {
            handleGetFdb(client_socket, command);

        } else if(strncmp(command, "getfda ", 7) == 0) {
            handleGetFda(client_socket, command);

        } else if (strcmp(command, "quitc") == 0) {
            // Client requests to quit
            printf("Client requested to quit. Closing connection.\n");
            // Close the client socket and exit the loop
            close(client_socket);
        } else {
            // Inform the client about an invalid command
            // Prepare message with an invalid command and the command received
            char msg[256];
            snprintf(msg, sizeof(msg), "Invalid command: %s", command);
            send(client_socket, msg, sizeof(msg), 0);
        }

        // Close the client socket
        close(client_socket);
    }
}




int main() {
    // Set up the server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(12345);  // Use a specific port number
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Close the server socket if it is still bound from a previous execution
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1) {
        perror("Error setting socket options");
        exit(EXIT_FAILURE);
    }

    // Bind the server socket
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding server socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server listening for incoming connections...\n");

    int client_count = 0;

    while (1) {
        // Accept a client connection
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1) {
            perror("Error accepting client connection");
            exit(EXIT_FAILURE);
        }

        // Fork a child process to handle the client request
        pid_t pid = fork();

        if (pid == 0) {
            // Child process

            // Alternate between server and mirror for the first 8 connections
            if (client_count < 4) {
                printf("Handling connection %d: Server\n", client_count + 1);
            } else if (client_count < 8) {
                printf("Handling connection %d: Mirror\n", client_count + 1);
            } else {
                if (client_count % 2 == 0) {
                    printf("Handling connection %d: Server\n", client_count + 1);
                } else {
                    printf("Handling connection %d: Mirror\n", client_count + 1);
                }
            }

            client_count++;

            close(server_socket);  // Close server socket in child process
            pclientrequest(client_socket);
            exit(EXIT_SUCCESS);
        } else if (pid > 0) {
            // Parent process
            close(client_socket);  // Close client socket in parent process
        } else {
            perror("Error forking process");
            exit(EXIT_FAILURE);
        }
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
