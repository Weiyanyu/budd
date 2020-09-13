#ifndef HTTPREQUEST
#define HTTPREQUEST
#include <string>
#include <unordered_map>
#include <glog/logging.h>
#include "httpParameter.h"


class HttpRequest
{
public:
    void clear(); //clear previous infomation

    HttpMethod method() const { return m_method; }
    const std::string url() const { return m_url; }
    const std::string path() const { return m_path; }
    const std::string httpVersion() const { return m_httpVersion; }
    const std::string remoteAddress() const { return m_remoteAddress; }
    const std::string contentType() const { return getHeader("Content-Type"); }
    const size_t contentLenth() const { 
        std::string lengthStr = getHeader("Content-Length");
        return (size_t)std::stoi(lengthStr);
    }
    const bool hasBody() const { return m_hasBody; }


    bool setMethod(const std::string method);
    void setUrl(const std::string &uri) { m_url = uri; }
    void setPath(const std::string &path) { m_path = path; }
    void setHttpVersion(const std::string &version) { m_httpVersion = version; }
    void setRemoteAddress(const std::string &remoteAddress) { m_remoteAddress = remoteAddress; }
    void setHasBody(bool hasBody) { m_hasBody = hasBody; }

    void setFormBody(const std::string &key, const std::string& value) { m_formBody[key] = value; }
    void setQueryParam(const std::string& key, const std::string& value) { m_query[key] = value; }
    void setHeader(const std::string& key, const std::string& value) { m_headers[key] = value; }

    const std::string getFromBody(const std::string& key) const {
        auto it = m_formBody.find(key);
        if (it != m_formBody.end()) {
            return it->second;
        }
        return "";
    }
    const std::string getHeader(const std::string& key) const
    {
        auto it = m_headers.find(key);
        if (it != m_headers.end()) {
            return it->second;
        }
        return "";
    }

    const std::string getQueryParam(const std::string& key) const
    { 
        auto it = m_query.find(key);
        if (it != m_query.end()) {
            return it->second;
        }
        return ""; 
    }

    //debug
    const void toString() const {
        LOG(INFO) << "request method: " << m_method;
        LOG(INFO) << "request url: " << m_url;
        LOG(INFO) << "request path: " << m_path;
        LOG(INFO) << "request http version: " << m_httpVersion;
        LOG(INFO) << "request query param: ";
        
        for (auto &entry : m_query) {
            LOG(INFO) << "key : " << entry.first << "  value : " << entry.second;
        }

        LOG(INFO) << "request header: ";
        
        for (auto &entry : m_headers) {
            LOG(INFO) << "key : " << entry.first << "  value : " << entry.second;
        }

        LOG(INFO) << "request form body: ";
        for (auto &entry : m_formBody) {
            LOG(INFO) << "key : " << entry.first << "  value : " << entry.second;
        }
    }

private:
    HttpMethod m_method;
    std::string m_url;
    std::string m_path;
    std::unordered_map<std::string, std::string> m_query;
    std::string m_httpVersion;
    std::string m_remoteAddress;
    std::unordered_map<std::string, std::string> m_headers;
    std::unordered_map<std::string, std::string> m_formBody;
    bool m_hasBody;
};

#endif