#ifndef HTTPREQUEST
#define HTTPREQUEST
#include <string>
#include <unordered_map>
#include <glog/logging.h>


class HttpRequest
{
public:
    void clear(); //clear previous infomation

    const std::string method() const { return m_method; }
    const std::string url() const { return m_url; }
    const std::string path() const { return m_path; }
    const std::string httpVersion() const { return m_httpVersion; }

    bool setMethod(const std::string method) { 
        m_method = std::move(method); 
        if (m_method == "POST" || 
            m_method == "GET" || 
            m_method == "HEAD" || 
            m_method == "DELETE" || 
            m_method == "PUT") {
        return true;
    }   
    return false;
    }
    void setUrl(const std::string uri) { m_url = std::move(uri); }
    void setPath(const std::string path) { m_path = std::move(path); }
    void setHttpVersion(const std::string version) { m_httpVersion = std::move(version); }

    const std::string getHeader(const std::string key) const
    {
        auto it = m_headers.find(key);
        if (it != m_headers.end()) {
            return it->second;
        }
        return "";
    }
    void setHeader(const std::string key, const std::string value) { m_headers[key] = value; }

    const std::string getQueryParam(const std::string key) const
    { 
        auto it = m_query.find(key);
        if (it != m_query.end()) {
            return it->second;
        }
        return ""; 
    }
    void setQueryParam(const std::string key, const std::string value) { m_query[key] = value; }

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
    }

private:
    std::string m_method;
    std::string m_url;
    std::string m_path;
    std::unordered_map<std::string, std::string> m_query;
    std::string m_httpVersion;

    std::unordered_map<std::string, std::string> m_headers;
};

#endif