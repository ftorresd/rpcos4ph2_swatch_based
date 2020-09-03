package main

import (
	"log"
	"net/http"

	"demo-cell-in-docker/internal"
)

func main() {
	http.HandleFunc("/", internal.GetProxyHandler())

	go func() {
		log.Print("Starting proxy server")
		err := http.ListenAndServe(":80", nil)
		if err != nil {
			log.Fatal(err)
		}
	}()

	err := <-internal.RunChild()
	if err != nil {
		log.Fatal(err)
	}
	log.Print("Halting")
}
