# twitter-cli

A command line client for Twitter.

Create a backend in Python or something else that's fun.

## Dependencies 

On Fedora:
```
sudo dnf install gcc make curl-devel json-c-devel readline-devel liboauth-devel
```

## Building

Create a file called `api_keys.h` with the following content:

```c
#define CONSUMER_KEY "twitter_api_consumer_key"                            
#define CONSUMER_SECRET "twitter_api_consumer_secret"
```

```
git clone https://github.com/kil0meters/twitter-cli
cd twitter-cli
make
```
