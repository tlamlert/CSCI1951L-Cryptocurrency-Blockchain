package test

import (
	"BrunoCoin/pkg/wallet"
	"bytes"
	"io"
	"os"
	"strings"
	"testing"
)

func TestWalletAddNilInput (t *testing.T){
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.Wallet.LmnlTxs.Add(nil);

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
	out := <-outC

	ErrorMessage := "ERROR {TxHeap.Add}: received a nil transaction."

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}

func TestWalletHndlBlkNilInput (t *testing.T){
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.Wallet.HndlBlk(nil)

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
	out := <-outC

	ErrorMessage := "ERROR {wallet.HndlBlk}: received a nil block.\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}

func TestWalletHndlTxsReqZeroInput (t *testing.T){
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	txq := wallet.TxReq{
		Amt: 0,
	}
	// testing
	genNd.Wallet.HndlTxReq(&txq)

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
	out := <-outC

	ErrorMessage := "ERROR {HndlTxReq} : The inputted amount is zero or below."

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}

func TestWalletHndlTxsReqMilInput (t *testing.T){
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	txq := wallet.TxReq{
		Amt: 1000000,
	}
	// testing
	genNd.Wallet.HndlTxReq(&txq)

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
	out := <-outC

	ErrorMessage := "ERROR {HndlTxReq}: Not enough balance"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}