package main

import (
	"io"
	"io/ioutil"
	"log"
	"net/http"
)

func main() {
	queue = make(map[string]map[string]chan []byte)
	http.HandleFunc("/ping", ping)
	http.HandleFunc("/", handle)
	log.Fatal(http.ListenAndServe(":8080", nil))
}

func ping(w http.ResponseWriter, r *http.Request) {
	w.Write([]byte("hello"))
}

func handle(w http.ResponseWriter, r *http.Request) {
	token := r.URL.Path[1:]
	if len(token) < 1 {
		w.WriteHeader(http.StatusNotFound)
		return
	}

	uuid := r.Header.Get("X-Client-UUID")
	if len(uuid) < 1 {
		w.WriteHeader(http.StatusBadRequest)
		return
	}

	log.Print(r.Method, token, uuid)

	if r.Method == http.MethodGet {
		w.Header().Set("Transfer-Encoding", "chunked")
		w.Header().Set("Content-Type", "application/octet-stream")
		subscribe(token, uuid, w)
	} else if r.Method == http.MethodPost {
		b, err := ioutil.ReadAll(r.Body)
		if err != nil {
			w.WriteHeader(http.StatusInternalServerError)
			return
		}
		publish(token, uuid, b)
	} else {
		w.WriteHeader(http.StatusMethodNotAllowed)
	}
}

var queue map[string]map[string]chan []byte

func publish(scope, uuid string, data []byte) {
	if _, ok := queue[scope]; !ok {
		queue[scope] = make(map[string]chan []byte)
	}
	targets := queue[scope]
	for k, target := range targets {
		if k == uuid {
			continue
		}
		log.Print("sending")
		target <- data
		log.Print("sent")
	}
}

func subscribe(scope, uuid string, w io.Writer) {
	flusher, ok := w.(http.Flusher)
	if !ok {
		log.Print("unable to obtain flusher")
	}
	if _, ok := queue[scope]; !ok {
		queue[scope] = make(map[string]chan []byte)
	}
	targets := queue[scope]
	if _, ok := targets[uuid]; ok {
		close(targets[uuid])
		delete(targets, uuid)
	}
	targets[uuid] = make(chan []byte)
	target := targets[uuid]
	for {
		log.Print("waiting")
		data, ok := <-target
		if !ok {
			log.Print("closed")
			return
		}
		n, err := w.Write(data)
		if err != nil {
			log.Print("disconnected")
			close(targets[uuid])
			delete(targets, uuid)
			return
		}
		flusher.Flush()
		log.Print("written ", n, " bytes")
	}
}
