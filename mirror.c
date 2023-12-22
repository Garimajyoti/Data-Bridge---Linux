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

    // Store information in correct format: Filename: <filename> /n Size: <size> bytes /n Date created: <date> /n File permissions: <permissions>
    snprintf(command, sizeof(command), "find . -name %s -printf \"Filename: %%f \\nSize: %%s bytes \\nDate created: %%t \\nFile permissions: %%m \\n\"", filename);

    // Open the pipe
    FILE* pipe = popen(command, "r");

    if (pipe == NULL) {
        perror("Error opening pipe");
        // Inform the client about the error
        send(client_socket, "Error opening pipe", sizeof("Error opening pipe"), 0);
    } else {
        // Read the file information from the pipe
        char buffer[1024];
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

    // char tempCmd[256];
    // sprintf(tempCmd, "find . -type f -size +%ldc -a -size -%ldc | wc -l", size1, size2);

    // // Run the command and print the result
    // FILE *fp = popen(tempCmd, "r");
    // if (fp == NULL) {
    //     perror("Error opening pipe");
    //     return EXIT_FAILURE;
    // }

    // int count;
    // fscanf(fp, "%d", &count);

    // pclose(fp);

    // // If no files are found, inform the client
    // if (count == 0) {
    //     send(client_socket, "No file found", sizeof("No file found"), 0);
    //     return;
    // }

    // Generate random number using date for filename
    char filename[256];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(filename, "getfztarfile_%d-%d-%d_%d-%d-%d.tar", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    sprintf(command, "find . -type f -size +%lldc -a -size -%lldc -exec tar -cvf %s {} +", size1, size2, filename);
    system(command); 

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

// Send all files with the given extensions in the given command. Recursive.
void handleGetFt(int client_socket, const char* command) {
    // Extract the file extensions from the command
    char extensions[256];
    sscanf(command, "getft %s", extensions);

    // Create a file list
    FILE *fileList = fopen("file_list.txt", "w");
    if (!fileList) {
        perror("Error creating file list");
        exit(EXIT_FAILURE);
    }

    // Build the find command to locate files with specified extensions in the current directory
    char findCommand[512];
    strcpy(findCommand, "find . \\( ");

    char* token = strtok(extensions, ",");
    while (token != NULL) {
        strcat(findCommand, "-name '*");
        strcat(findCommand, token);
        strcat(findCommand, "'");

        token = strtok(NULL, ",");
        if (token != NULL) {
            strcat(findCommand, " -o ");
        }
    }

    strcat(findCommand, " \\) -print > file_list.txt");

    // Execute the find command
    system(findCommand);

    // Generate random number using date for filename
    char filename[256];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(filename, "getfttarfile_%d-%d-%d_%d-%d-%d.tar", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    sprintf(command, "tar -cvf %s -T file_list.txt", filename);
    system(command);

    // Check if number of files is 0
    FILE* file = fopen("file_list.txt", "r");
    if (file == NULL) {
        perror("Error opening file list");
        send(client_socket, "Error opening file list", sizeof("Error opening file list"), 0);

        // Close the file
        fclose(file);

        // Remove the file list
        remove("file_list.txt");

        // Remove tar
        remove(filename);
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    if (fileSize == 0) {
        // Inform the client that no files were found
        send(client_socket, "No file found", sizeof("No file found"), 0);

        // Close the file
        fclose(file);

        // Remove the file list
        remove("file_list.txt");

        // Remove tar
        remove(filename);
        return;
    }

    // Close the file
    fclose(file);

    // Remove the file list
    remove("file_list.txt");

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

void handleGetFdb(int client_socket, const char* command) {
    // Extract date from the command
    char date[256];
    sscanf(command, "getfdb %s", date);

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
    sprintf(filename, "getfdbtarfile_%d-%d-%d_%d-%d-%d.tar.gz", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    char tempcmd[1024];
    // Get all files older than the given date
    sprintf(tempcmd, "find . -maxdepth 1 -type f -not -newermt '%s' -exec tar -czvf '%s' {} +", date, filename);

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
    sprintf(tempcmd, "find . -maxdepth 1 -type f -newermt '%s' -exec tar -czvf '%s' {} +", date, filename);

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
    // remove(filename);
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
    server_address.sin_port = htons(13245);  // Use a specific port number
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
