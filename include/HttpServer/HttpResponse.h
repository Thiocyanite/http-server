//
// Created by dominik on 06.11.19.
//

#ifndef HTTP_SERVER_HTTPRESPONSE_H
#define HTTP_SERVER_HTTPRESPONSE_H

#include <HttpServer/HttpMessage.h>
#include <HttpServer/HttpParser.h>
#include <HttpServer/HttpStatusCodes.h>

/* @brief Http response representation.
 * @details This class allows for represenation of a http/1.1 response.\n
 *          Because the header keys may vary from response to response, this class stores header keys and\n
 *          vectors of coresponding values in a dictionary. Both values and keys are stored as strings.\n
 *          Conversion of those values is left to the class user.\n
 */
class HttpResponse : public HttpMessage
{
public:
    HttpResponse()
    = default;

    std::string to_str() const override;

    std::string get_response_line() const;

private:
    StatusCode status_code;

    friend class HttpResponseBuilder;
};
#endif //HTTP_SERVER_HTTPRESPONSE_H