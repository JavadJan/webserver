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

static std::vector<std::string> splitByBoundary(const std::string& body, const std::string& boundary)
{
    std::vector<std::string> parts;
    std::string full = "--" + boundary;

    size_t pos = body.find(full);
    if (pos == std::string::npos)
        return parts;

    pos += full.length();

    while (true)
    {
        size_t next = body.find(full, pos);
        if (next == std::string::npos)
            break;

        std::string part = body.substr(pos, next - pos);

        // remove leading CRLF
        if (part.substr(0, 2) == "\r\n")
            part.erase(0, 2);

        parts.push_back(part);

        pos = next + full.length();
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


static void saveFile(std::string filename, std::string content)
{
	std::string path = "./tmp/www/upload/" + filename; 
	std::ofstream file(path.c_str(), std::ios::binary);
	file.write(content.data(), content.size());
}

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
            saveFile(info.filename, info.content);
        }
    }

    // 5. Respond
    res.setStatusCode(201);
    res.setBody("");
}
