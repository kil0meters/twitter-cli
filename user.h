#ifndef USER_H_
#define USER_H_

typedef struct User {
    char *name;
    char *handle;
    char *bio;
    char *link;

    int follow_count;
    int following_count;
    
    char *access_token;
} User;

User * user_new(
    char *name,
    char *handle,
    char *bio,
    char *link,
    int follow_count,
    int following_count,
    char *access_token
);

void user_print(User *self);

#endif