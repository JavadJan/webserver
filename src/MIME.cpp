/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MIME.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 10:23:07 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/08 13:59:21 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/MIME.hpp"

MIME::MIME()
{
    // Text
    _mimeType[".txt"]   = "text/plain";
    _mimeType[".html"]  = "text/html";
    _mimeType[".htm"]   = "text/html";
    _mimeType[".css"]   = "text/css";
    _mimeType[".js"]    = "text/javascript";
    
    // Images
    _mimeType[".jpeg"]  = "image/jpeg";
    _mimeType[".jpg"]   = "image/jpeg";
    _mimeType[".png"]   = "image/png";
    _mimeType[".gif"]   = "image/gif";
    _mimeType[".bmp"]   = "image/bmp";
    
    // Files
    _mimeType[".pdf"]   = "application/pdf";
    _mimeType[".doc"]   = "application/msword";
    _mimeType[".zip"]   = "application/zip";
    
    // Data
    _mimeType[".json"]  = "application/json";
    _mimeType[".xml"]   = "application/xml";
}

std::string MIME::getSuffix(const std::string path)
{
    size_t pos = path.rfind(".");
    if(pos == std::string::npos)
        return "";
    return path.substr(pos);
}

std::string MIME::getMimeType(const std::string suffix)
{
    return _mimeType[suffix];
}
