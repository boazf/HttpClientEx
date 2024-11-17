#include <HttpClientEx.h>
#include <base64.h>

void HttpClientEx::collectHeaders(Headers headers[], int nHeaders)
{
    String header;
    while(!endOfHeadersReached())
    {
        char c = (char)readHeader();
        if (c == '\r');
        else if (c == '\n')
        {
            int semicol = header.indexOf(':');
            String headerName = header.substring(0, semicol);
            Headers *collectHeader = NULL;
            for (int i = 0; i < nHeaders; i++)
                if (headers[i].key.equals(headerName))
                {
                    collectHeader = &headers[i];
                    break;
                }
            if (collectHeader != NULL)
            {
                    collectHeader->value = header.substring(semicol + 1);
                    collectHeader->value.trim();
            }
            header.clear();
        }
        else
            header += c;
    }
}

bool HttpClientEx::parseUrl(String url)
{
    // check for : (http: or https:)
    int index = url.indexOf(':');
    if(index < 0) {
        log_d("failed to parse protocol");
        return false;
    }

    String protocol = url.substring(0, index);
    if(protocol != "http" && protocol != "https") {
        log_d("unknown protocol '%s'", protocol.c_str());
        return false;
    }

    _port = (protocol == "https" ? 443 : 80);
    _secure = (protocol == "https");
    _protocol = protocol;
    
    log_v("url: %s", url.c_str());

    url.remove(0, (index + 3)); // remove http:// or https://

    index = url.indexOf('/');
    if (index == -1) {
        index = url.length();
        url += '/';
    }
    String host = url.substring(0, index);
    url.remove(0, index); // remove host part

    // get Authorization
    index = host.indexOf('@');
    if(index >= 0) {
        // auth info
        String auth = host.substring(0, index);
        host.remove(0, index + 1); // remove auth part including @
        _base64Authorization = base64::encode(auth);
        log_d("Auth: %s\n", auth.c_str());
        log_d("base64Authorization: %s\n", _base64Authorization.c_str());
    }

    // get port
    index = host.indexOf(':');
    String the_host;
    if(index >= 0) {
        the_host = host.substring(0, index); // hostname
        host.remove(0, (index + 1)); // remove hostname + :
        _port = host.toInt(); // get port
    } else {
        the_host = host;
    }

    _host = the_host;
    _uri = url;

    log_d("protocol: %s, host: %s port: %d url: %s", _protocol.c_str(), _host.c_str(), _port, _uri.c_str());

    return true;
}

void HttpClientEx::sendAuthorizationHeader()
{
    if (!_base64Authorization.isEmpty())
    {
        log_d("Sending authorization header\n");
        sendHeader("Authorization", ("Basic " + _base64Authorization).c_str());
    }
}

String HttpClientEx::errorToString(int error)
{
    switch(error) {
    case HTTPC_ERROR_CONNECTION_REFUSED:
        return F("connection refused");
    case HTTPC_ERROR_SEND_HEADER_FAILED:
        return F("send header failed");
    case HTTPC_ERROR_SEND_PAYLOAD_FAILED:
        return F("send payload failed");
    case HTTPC_ERROR_NOT_CONNECTED:
        return F("not connected");
    case HTTPC_ERROR_CONNECTION_LOST:
        return F("connection lost");
    case HTTPC_ERROR_NO_STREAM:
        return F("no stream");
    case HTTPC_ERROR_NO_HTTP_SERVER:
        return F("no HTTP server");
    case HTTPC_ERROR_TOO_LESS_RAM:
        return F("too less ram");
    case HTTPC_ERROR_ENCODING:
        return F("Transfer-Encoding not supported");
    case HTTPC_ERROR_STREAM_WRITE:
        return F("Stream write error");
    case HTTPC_ERROR_READ_TIMEOUT:
        return F("read Timeout");
    default:
        return String();
    }
}

