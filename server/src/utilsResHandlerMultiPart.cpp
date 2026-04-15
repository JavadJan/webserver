/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsResHandlerMultiPart.cpp                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkhavari <mkhavari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:54:10 by asemykin          #+#    #+#             */
/*   Updated: 2026/04/15 21:34:38 by mkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseHandler.hpp"

static std::string extractBoundary(const std::string &ctype)
{
    size_t posB = ctype.find("boundary=");
    if (posB == std::string::npos)
        return "";

    posB += 9; // length of "boundary="

    std::string b = ctype.substr(posB);

    // Trim CRLF or spaces
    while (!b.empty() && (b[0] == ' ' || b[0] == '\r'))
        b.erase(0, 1);

    while (!b.empty() && (b[b.size()-1] == ' ' || b[b.size()-1] == '\r'))
        b.erase(b.size()-1);

    return b;
}
static std::vector<std::string> splitByBoundary(const std::string &body,
                                                const std::string &boundary)
{
    std::vector<std::string> parts;

    // RFC: boundary lines start with "--"
    std::string marker = "--" + boundary;

    //size_t pos = 0;

    // 1. Find the first boundary
    size_t start = body.find(marker);
    if (start == std::string::npos)
        return parts;

    start += marker.length();

    while (true)
    {
        // 2. Find the next boundary
        size_t next = body.find(marker, start);
        if (next == std::string::npos)
            break;

        // Extract raw part block
        std::string part = body.substr(start, next - start);

        // Remove leading CRLF
        if (part.size() >= 2 && part[0] == '\r' && part[1] == '\n')
            part.erase(0, 2);
        else if (part.size() >= 1 && (part[0] == '\n'))
            part.erase(0, 1);

        // Remove trailing CRLF
        while (!part.empty() &&
               (part[part.size() - 1] == '\r' || part[part.size() - 1] == '\n'))
        {
            part.erase(part.size() - 1);
        }

        // Ignore empty parts (can happen)
        if (!part.empty())
            parts.push_back(part);

        // Move past this boundary
        start = next + marker.length();

        // 3. Check for final boundary "--"
        if (body.compare(start, 2, "--") == 0)
            break;
    }

    return parts;
}



// Content-Disposition: form-data; name="avatar"; filename="profile.png"
//Content-Disposition: form-data; name="username"
static PartInfo parsePartHeaders(const std::string &part)
{
    PartInfo info;

    size_t headerEnd = part.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        headerEnd = part.find("\n\n");

    if (headerEnd == std::string::npos)
        return info;

    std::string headers = part.substr(0, headerEnd);
    std::string content = part.substr(headerEnd + 4);

    // Extract filename
    size_t pos = headers.find("filename=\"");
    if (pos != std::string::npos)
    {
        pos += 10;
        size_t end = headers.find("\"", pos);
        if (end != std::string::npos)
            info.filename = headers.substr(pos, end - pos);
    }

    info.content = content;
    return info;
}

static void saveFile(const std::string& basePath, std::string filename, std::string content)
{
    std::string path = basePath + "/" + filename;
    if(BUG)
    {std::cout << "save file in " << path << std::endl;}
    std::ofstream file(path.c_str(), std::ios::binary);
    file.write(content.data(), content.size());
}
// static void saveFile(std::string filename, std::string content)
// {
// 	// std::string path = "./tmp/www/upload/" + filename; 
// 	// std::cout << "save file in " << path << std::endl;
// 	// std::ofstream file(path.c_str(), std::ios::binary);
// 	// file.write(content.data(), content.size());
// }

void ResponseHandler::handleUpload(const HttpRequest &req, const Config &server)
{
	(void)server;
    const std::string &body = req.getBody();
    const std::string &ctype = req.getContentType();

    // 1. Extract boundary
    std::string boundary = extractBoundary(ctype);

    // 2. Split body into parts
    std::vector<std::string> parts = splitByBoundary(body, boundary);

    // 3. For each part
    for (size_t i = 0; i < parts.size(); i++)
    {
        PartInfo info = parsePartHeaders(parts[i]);

        if (info.filename != "")
        {
            // 4. Save file
            // saveFile(info.filename, info.content);
            saveFile(full_path, info.filename, info.content);
        }
    }

    // 5. Respond
    res.setStatusCode(201);
    res.setContType(getMimeType(".txt"));
    res.setBody("");
}
