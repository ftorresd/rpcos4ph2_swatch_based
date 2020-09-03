package internal

import (
	"log"
	"os"
	"os/exec"
)

// RunChild runs the given command in background
func RunChild() chan error {
	if len(os.Args) < 2 {
		log.Fatal("no arguments given")
	}
	childArgs := os.Args[2:]
	cmd := exec.Command(os.Args[1], childArgs...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	ch := make(chan error, 1)
	log.Printf("starting child process %v", os.Args[1:])
	go func() {
		ch <- cmd.Run()
		log.Print("finished")
		close(ch)
	}()
	return ch
}
