package test

import (
	"BrunoCoin/pkg"
	"BrunoCoin/pkg/utils"
	"bytes"
	"io"
	"os"
	"strings"
	"testing"
	"time"
)

func TestValidationChkBlkNilInput (t *testing.T){
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.ChkBlk(nil)

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

	ErrorMessage := "ERROR {ChkBlk}: The block's is nil\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}

func TestValidationChkTxNilInput (t *testing.T){
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.ChkTx(nil)

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

	ErrorMessage := "ERROR {ChkTx}: Received nil transaction\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}

func TestChkBlk (t *testing.T){
	utils.SetDebug(true)

	// set up
	genNd := NewGenNd()
	node2 := pkg.New(pkg.DefaultConfig(GetFreePort()))
	genNd.Conf.MnrConf.InitPOWD = utils.CalcPOWD(1)

	genNd.Start()
	node2.Start()

	// connect the two nodes
	genNd.ConnectToPeer(node2.Addr)
	time.Sleep(1 * time.Second)

	// gen node makes a transaction
	genNd.SendTx(10, 50, node2.Id.GetPublicKeyBytes())
	time.Sleep(1 * time.Second)

	// Check that both nodes have "seen" 1 transaction since
	// the second transaction should have never been made or
	// broadcast
	ChkTxSeenLen(t, genNd, 1)
	ChkTxSeenLen(t, node2, 1)

	// start mining
	node2.StartMiner()
	time.Sleep(3 * time.Second)

	// check the blockchain length
	ChkMnChnLen(t, genNd, 2)
	ChkMnChnLen(t, node2, 2)
	ChkMnChnCons(t, []*pkg.Node{genNd, node2})
}