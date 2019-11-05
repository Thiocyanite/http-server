//
// Created by dominik on 04.11.19.
//

#ifndef HTTP_SERVER_HTTTPPARSER_H
#define HTTP_SERVER_HTTTPPARSER_H

#include "HttpRequest.h"

/*!
* @brief Simple and naive parser of http requets, relies on data being separated by single spaces.
* Can produce a HttpRequest class from a string.
* @details
* <ul>
*  <li> A static class.
*  <li> Creating a class instance is forbidden.
*  <li> Does not remember any state, and can be used by multiple threads at the same time.
*  <li> Works as a factory of HttpRequest objects.
*  <li> Parsed request string:
*      <ul>
*          <li> must contain only one http request, with the end indicating CR LF CR LF trimmed.
*          <li> use only single spaces as word's separators.
*          <li> use CR LF as line's separators.
*          <li> can't contain the body section.
*      </ul>
* </ul>
* @see HttpRequest
*/
class HttpParser
{
public:
    /*!
     * @brief Translates a http request from a string to a HttpRequest.
     * @details Constraints on the string containing the http request:
     *          <ul>
     *              <li> must contain only one http request, with the end indicating CR LF CR LF trimmed.
     *              <li> use only single spaces as word's separators.
     *              <li> use CR LF as line's separators.
     *              <li> can't contain the body section.
     *          </ul>
     * @param message Message to be parsed.
     * @return Message as HttpRequest
     * @see HttpRequest
     */
    HttpRequest static parse_message(const std::__cxx11::string &message);

private:
    /*!
     * @brief Translates the first line of http request which contains the request type, file, params and http protocol version.
     * @param httpRequest The http request in which the translated data will be stored.
     * @param line Line such as GET|PUT|POST|ect. /<filename>?<params> HTTP<digit>+.<digit>+
     */
    static void parse_request_type(HttpRequest &httpRequest, std::__cxx11::string &line);

    /*!
     * @brief Translates a line of header key and values to a multi-dictionary (one key  can hold multiple values).
     * @param httpRequest The http request in which the translated data will be stored.
     * @param line line of header key and values separated by single spaces
     */
    static void parse_keys_and_values(HttpRequest &httpRequest, std::__cxx11::string &line);
};

#include <string>
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <iostream>

#endif //HTTP_SERVER_HTTTPPARSER_H
