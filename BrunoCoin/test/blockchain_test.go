package test

import (
	"bytes"
	"encoding/hex"
	"io"
	"os"
	"strings"
	"testing"
)

func TestGetUTXOIncAmt (t *testing.T) {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up cluster
	cluster := NewCluster(2)
	StartCluster(cluster)
	ConnectCluster(cluster)
	cluster[0].Mnr.StartMiner()

	// testing
	cluster[0].Chain.GetUTXOForAmt(100000000, hex.EncodeToString(cluster[0].Id.GetPublicKeyBytes()))

	outC := make(chan string)
	// copy the output in a separate goroutine
	go func() {
		buf := bytes.Buffer{}
		io.Copy(&buf, r)
		outC <- buf.String()
	}()

	// back to normal state
	w.Close()
	os.Stdout = old  // restoring the real stdout
	out := <- outC

	ErrorMessage := "ERROR {GetUTXOForAmt}: Not enough balance. Current balance: 100000. Required amount: 100000000"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}


func TestAddNilInput (t *testing.T) {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.Chain.Add(nil)

	outC := make(chan string)
	// copy the output in a separate goroutine
	go func() {
		buf := bytes.Buffer{}
		io.Copy(&buf, r)
		outC <- buf.String()
	}()

	// back to normal state
	w.Close()
	os.Stdout = old  // restoring the real stdout
	out := <- outC

	ErrorMessage := "ERROR {bc.Add}: received nil input"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}
