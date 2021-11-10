package test

import (
	"BrunoCoin/pkg/block/tx"
	"BrunoCoin/pkg/block/tx/txi"
	"BrunoCoin/pkg/block/tx/txo"
	"BrunoCoin/pkg/miner"
	"BrunoCoin/pkg/proto"
	"bytes"
	"io"
	"os"
	"strings"
	"testing"
)


func TestGenCBTxNilInput (t *testing.T) {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.Mnr.GenCBTx(nil)

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

	ErrorMessage := "ERROR {GenCBTx} : Received nil transactions\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}

func TestGenCBTxEmptyInput (t *testing.T) {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.Mnr.GenCBTx([]*tx.Transaction{nil})

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

	ErrorMessage := "ERROR {GenCBTx} : Received nil transactions\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}

func TestGenCBTxSomeNilInput (t *testing.T) {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.Mnr.GenCBTx([]*tx.Transaction{tx.Deserialize(proto.NewTx(0, nil, nil, 0)), nil})

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

	ErrorMessage := "ERROR {GenCBTx} : Received nil transactions\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}

func TestHndBlkNilInput (t *testing.T) {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.Mnr.HndlBlk(nil)

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

	ErrorMessage := "ERROR {HndlBlk}: " + "received a nil block.\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}

func TestHndlTxNilInput (t *testing.T) {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.Mnr.HndlTx(nil)

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

	ErrorMessage := "ERROR {Miner.HndlTx}: " + "received a nil transaction.\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected taErrorMessage but received: %v", out)
	}
}

// send normal transaction to while miner is inactive
func TestHndlTxInactiveMiner (t *testing.T){
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.Mnr.HndlTx(nil)

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

	ErrorMessage := "ERROR {Miner.HndlTx}: " + "received a nil transaction.\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected taErrorMessage but received: %v", out)
	}
}

// send normal transaction to HndlTx while miner is active
func TestHndlTxActiveMiner (t *testing.T){
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.Mnr.HndlTx(nil)

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

	ErrorMessage := "ERROR {Miner.HndlTx}: " + "received a nil transaction.\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}

func TestHndlTxInactive (t *testing.T){
	// set-up described in other tests in the package
	cluster := NewCluster(2)
	StartCluster(cluster)
	ConnectCluster(cluster)

	ttxi := txi.TransactionInput{
		TransactionHash: "A",
		OutputIndex    : 1,
		UnlockingScript: "A",
		Amount         : 1,
	}
	txia := []*txi.TransactionInput{&ttxi}
	ttxo := txo.TransactionOutput{
		Amount :        1,
		LockingScript : "A",
		Liminal :      false,
	}
	txoa := []*txo.TransactionOutput{&ttxo}
	ttx := tx.Transaction {
		Version : 1,
		Inputs  : txia,
		Outputs : txoa,
		LockTime: 5,
	}

	cluster[0].Mnr.HndlTx(&ttx)
	if cluster[0].Mnr.TxP.Length() == 0 {
		t.Errorf("should be added")
	}
}

func TestHndlTxActive (t *testing.T){
	// set-up described in other tests in the package
	cluster := NewCluster(2)
	StartCluster(cluster)
	ConnectCluster(cluster)
	cluster[0].Mnr.StartMiner()

	ttxi := txi.TransactionInput{
		TransactionHash: "A",
		OutputIndex    : 1,
		UnlockingScript: "A",
		Amount         : 1,
	}
	txia := []*txi.TransactionInput{&ttxi}
	ttxo := txo.TransactionOutput{
		Amount :        1,
		LockingScript : "A",
		Liminal :      false,
	}
	txoa := []*txo.TransactionOutput{&ttxo}
	ttx := tx.Transaction {
		Version : 1,
		Inputs  : txia,
		Outputs : txoa,
		LockTime: 5,
	}

	cluster[0].Mnr.HndlTx(&ttx)
	if cluster[0].Mnr.TxP.Length() == 0 {
		t.Errorf("should be added")
	}
}

func TestHndlChkBlkNilInput (t *testing.T){
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.Mnr.HndlBlk(nil)

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

	ErrorMessage := "ERROR {HndlBlk}: received a nil block."

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}

// send nil tx to tp.Add
func TestTpAddNilInput (t *testing.T) {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up genesis node
	genNd := NewGenNd()
	genNd.Start()
	genNd.StartMiner()

	// testing
	genNd.Mnr.TxP.Add(nil)

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

	ErrorMessage := "ERROR {tp.Add}: received a nil transaction.\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}


// send transaction which will exceed the pool's capacity
func TestTpAddCapExceeded (t *testing.T) {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up a small transaction poll
	var tp = miner.NewTxPool(miner.SmallTxPCapConfig(1))

	// testing
	tx1 := tx.Deserialize(proto.NewTx(0, nil, nil, 0))
	tx2 := tx.Deserialize(proto.NewTx(0, nil, nil, 0))
	tp.Add(tx1)
	tp.Add(tx2)

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

	ErrorMessage := "ERROR {tp.Add}: capacity exceeded.\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}


// check txpool's size
func TestTpAddLen (t *testing.T) {
	// set up a transaction poll
	var tp = miner.NewTxPool(miner.DefaultConfig(0))

	// testing
	for i := 0; i < 10; i++ {
		tx := tx.Deserialize(proto.NewTx(0, nil, nil, 0))
		tp.Add(tx)
		if tp.Length() != uint32(i+1) {
			t.Errorf("expected length %d but got length %d", i+1, tp.Length())
		}
	}
}


func TestCalcPriNil (t *testing.T) {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// testing
	miner.CalcPri(nil)

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

	ErrorMessage := "ERROR {CalcPri}: received nil as input.\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}


func TestCalcPriZero (t *testing.T) {
	// set up transaction
	tx := tx.Deserialize(proto.NewTx(0, nil, nil, 0))

	// testing
	pri := miner.CalcPri(tx)

	if pri != 1 {
		t.Errorf("expected 1 but received: %d", pri)
	}
}


func TestCalcPriNormal(t *testing.T) {
	// set up transaction
	ttxi := txi.TransactionInput{
		Amount : 10,
	}
	txia := []*txi.TransactionInput{&ttxi}
	ttxo := txo.TransactionOutput{
		Amount : 1,
	}
	txoa := []*txo.TransactionOutput{&ttxo}
	ttx := tx.Transaction {
		Inputs  : txia,
		Outputs : txoa,
	}

	// testing
	pri := miner.CalcPri(&ttx)

	fee := ttxi.Amount - ttxo.Amount
	var factor uint32 = 100
	acPri := fee * factor / ttx.Sz()

	if pri != acPri {
		t.Errorf("expected %d but received: %d", acPri, pri)
	}
}


func TestChkTxsNil (t *testing.T) {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	// set up a transaction poll
	var tp = miner.NewTxPool(miner.DefaultConfig(0))

	// testing
	tp.ChkTxs([]*tx.Transaction{nil})

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

	ErrorMessage := "ERROR {ChkTxs} : received nil as input.\n"

	if !strings.Contains(out, ErrorMessage){
		t.Errorf("expected ErrorMessage but received: %v", out)
	}
}