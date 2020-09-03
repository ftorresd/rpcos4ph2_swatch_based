package internal

import (
	"log"
	"net/http"
	"net/http/httputil"
	"net/url"
	"os"
	"strings"
)

var shittyURL string

// XDAQ wants to return the FQDN in it's http redirects
// to do this it determines the hostname and includes it in its headers
// this, of course, breaks in a dockerized environment
// XDAQ does this because of the (deprecated) RFC 2616
// https://tools.ietf.org/html/rfc2616#section-14.30
// This RFC has been long replaced by another that allows for redirects
// without absolute URLs
// This proxy converts responses to RFC 7231, which replaced RFC 2616
// https://tools.ietf.org/html/rfc7231#section-7.1.2
func modifyResponse(r *http.Response) error {
	location := r.Header.Get("Location")
	if strings.Contains(location, shittyURL) {
		location = strings.Replace(location, shittyURL, "", 1)
		r.Header.Set("Location", location)
	}
	return nil
}

// GetProxyHandler fixes a glitch with XDAQ in docker containers
// by putting a proxy server between the user and XDAQ
func GetProxyHandler() func(http.ResponseWriter, *http.Request) {
	ip, err := getLocalIP()
	if err != nil {
		log.Fatal(err)
	}
	swatchURL, err := url.Parse("http://" + ip.String() + ":3333")
	if err != nil {
		log.Fatal(err)
	}

	hostname, err := os.Hostname()
	if err != nil {
		panic(err)
	}
	shittyURL = "http://" + hostname + ":3333"
	log.Printf("will replace '%s/' with '/'", shittyURL)

	proxyHandler := httputil.NewSingleHostReverseProxy(swatchURL)
	proxyHandler.ModifyResponse = modifyResponse
	return proxyHandler.ServeHTTP
}
