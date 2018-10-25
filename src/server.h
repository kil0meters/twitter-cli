#ifndef SERVER_H_
#define SERVER_H_

int generate_oauth_keys(void);

char *get_oauth_key(void);
char *get_oauth_key_secret(void);
char *get_oauth_verifier(void);

int server_start(void);
void server_stop(void);

#endif
