#ifndef HTTP_CLIENT_EX_H
#define HTTP_CLIENT_EX_H
#include <HttpClient.h>

/// HTTP client errors
#define HTTPC_ERROR_CONNECTION_REFUSED  (-1)
#define HTTPC_ERROR_SEND_HEADER_FAILED  (-2)
#define HTTPC_ERROR_SEND_PAYLOAD_FAILED (-3)
#define HTTPC_ERROR_NOT_CONNECTED       (-4)
#define HTTPC_ERROR_CONNECTION_LOST     (-5)
#define HTTPC_ERROR_NO_STREAM           (-6)
#define HTTPC_ERROR_NO_HTTP_SERVER      (-7)
#define HTTPC_ERROR_TOO_LESS_RAM        (-8)
#define HTTPC_ERROR_ENCODING            (-9)
#define HTTPC_ERROR_STREAM_WRITE        (-10)
#define HTTPC_ERROR_READ_TIMEOUT        (-11)

template <typename key_T, typename value_T>
class KeyValuePair
{
public:
    KeyValuePair(const key_T &key, const value_T &value = value_T())
    {
        this->key = key;
        this->value = value;
    }

    key_T key;
    value_T value;
};

class HttpClientEx : public HttpClient
{
public:
    HttpClientEx(Client &client) : HttpClient(client)
    {
    }

    int get()
    {
        return HttpClient::get(_host.c_str(), _port, _uri.c_str());
    }

    int post()
    {
        return HttpClient::post(_host.c_str(), _port, _uri.c_str());
    }

    int put()
    {
        return HttpClient::put(_host.c_str(), _port, _uri.c_str());
    }

    int startRequest(const char* aHttpMethod, const char* aUserAgent)
    {
        return HttpClient::startRequest(_host.c_str(), _port, _uri.c_str(), aHttpMethod, aUserAgent);
    }

public:
    typedef KeyValuePair<String, String> Headers;

public:
    bool begin(String url)
    {
        return parseUrl(url);
    }

    bool begin(const String& host, uint16_t port, const String& uri, const String &protocol = "http")
    {
        _host = host;
        _port = port;
        _uri = uri;
        _protocol = protocol;

        return true;
    }

    void collectHeaders(Headers headers[], int nHeaders);
    void sendAuthorizationHeader();
    static String errorToString(int error);

protected:
    String _host;
    uint16_t _port = 0;
    bool _secure = false;
    String _protocol;
    String _uri;
    String _base64Authorization;

protected:
    bool parseUrl(String url);
};

#endif // HTTP_CLIENT_EX_H