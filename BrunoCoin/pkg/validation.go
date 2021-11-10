package pkg

import (
	"BrunoCoin/pkg/block"
	"BrunoCoin/pkg/block/tx"
	"fmt"
)

/*
 *  Brown University, CS1951L, Summer 2021
 *  Designed by: Colby Anderson, John Roy,
 *	Parker Ljung
 *
 */


// ChkBlk (CheckBlock) validates a block based on multiple
// conditions.
// To be valid:
// The block must be syntactically (ChkBlkSyn), semantically
// (ChkBlkSem), and configurally (ChkBlkConf) valid.
// Each transaction on the block must be syntactically (ChkTxSyn),
// semantically (ChkTxSem), and configurally (ChkTxConf) valid.
// Each transaction on the block must reference UTXO on the same
// chain (main or forked chain) and not be a double spend on that
// chain.
// Inputs:
// b *block.Block the block to be checked for validity
// Returns:
// bool True if the block is valid. false
// otherwise
// TODO:
// to be valid

// Each transaction on the block must reference UTXO on the same
// chain (main or forked chain) and not be a double spend on that
// chain.
// The block's size must be less than or equal to the largest
// allowed block size.
// The block hash must be less than the difficulty target.
// The block's first transaction must be of type Coinbase.

// Some helpful functions/methods/fields:
// note: let t be a transaction object
// note: let b be a block object
// t.IsCoinbase()
// b.SatisfiesPOW(...)
// n.Conf.MxBlkSz
// b.Sz()
// n.Chain.ChkChainsUTXO(...)
func (n *Node) ChkBlk(b *block.Block) bool {
	if b == nil {
		fmt.Printf("ERROR {ChkBlk}: The block's is nil\n")
		return false
	}
	if len(b.Transactions) == 0 {
		fmt.Printf("ERROR {ChkBlk}: The block's transaction length is zero\n")
		return false
	}
	if b.Transactions == nil {
		fmt.Printf("ERROR {ChkBlk}: The block's transaction is nil\n")
		return false
	}
	if !b.Transactions[0].IsCoinbase() {
		fmt.Printf("ERROR {ChkBlk}: The first transaction is not of type Coinbase\n")
		return false
	}
	if !n.Chain.ChkChainsUTXO(b.Transactions[1:], b.Hdr.PrvBlkHsh) {
		fmt.Printf("ERROR {ChkBlk}: Some utxo's do not exist on the current chain\n")
		return false
	}
	for _, t := range b.Transactions[1:] {
		if !n.ChkTx(t) {
			fmt.Printf("ERROR {ChkBlk}: Transaction (%v) is double spent\n", t.NameTag())
			return false
		}
	}
	if b.Sz() > n.Conf.MxBlkSz {
		fmt.Printf("ERROR {ChkBlk}: The block's size exceeds the maximum block size\n")
		return false
	}
	if !b.SatisfiesPOW(b.Hdr.DiffTarg) {
		fmt.Printf("ERROR {ChkBlk}: The block does not sitisfy proof of work\n")
		return false
	}

	if b.Transactions[0].Inputs == nil || b.Transactions[0].Outputs == nil {
		fmt.Printf("ERROR {ChkTx}: Transaction's inputs or outputs are nil\n")
		return false
	}
	if b.Transactions[0].SumOutputs() == 0 {
		fmt.Printf("ERROR {ChkTx}: Transaction's output sum is zero\n")
		return false
	}

	if b.Transactions[0].Sz() > n.Conf.MxBlkSz {
		fmt.Printf("ERROR {ChkTx}: Transaction size exceeds maximum block size\n")
		return false
	}
	return true
}


// ChkTx (CheckTransaction) validates a transaction.
// Inputs:
// t *tx.Transaction the transaction to be checked for validity
// Returns:
// bool True if the transaction is syntactically valid. false
// otherwise
// TODO:
// to be valid:

// The transaction's inputs and outputs must not be empty.
// The transaction's output amounts must be larger than 0.
// The sum of the transaction's inputs must be larger
// than the sum of the transaction's outputs.
// The transaction must not double spend any UTXO.
// The unlocking script on each of the transaction's
// inputs must successfully unlock each of the corresponding
// UTXO.
// The transaction must not be larger than the
// maximum allowed block size.

// Some helpful functions/methods/fields:
// note: let t be a transaction object
// note: let b be a block object
// note: let u be a transaction output object
// n.Conf.MxBlkSz
// t.Sz()
// u.IsUnlckd(...)
// n.Chain.GetUTXO(...)
// n.Chain.IsInvalidInput(...)
// t.SumInputs()
// t.SumOutputs()
func (n *Node) ChkTx(t *tx.Transaction) bool {
	if t == nil {
		fmt.Printf("ERROR {ChkTx}: Received nil transaction\n")
		return false
	}
	if t.Inputs == nil || t.Outputs == nil {
		fmt.Printf("ERROR {ChkTx}: Transaction's inputs or outputs are nil\n")
		return false
	}
	if t.SumOutputs() == 0 {
		fmt.Printf("ERROR {ChkTx}: Transaction's output sum is zero\n")
		return false
	}
	if t.SumOutputs() > t.SumInputs() {
		fmt.Printf("ERROR {ChkTx}: Output sum is greater than input sum\n")
		return false
	}
	for _, utxo := range t.Inputs {
		if n.Chain.IsInvalidInput(utxo) {
			fmt.Printf("ERROR {ChkTx}: Transaction is orphan\n")
			return false
		}
		if n.Chain.GetUTXO(utxo) == nil {
			fmt.Printf("ERROR {ChkTx}: Double spending\n")
			return false
		}
		if !n.Chain.GetUTXO(utxo).IsUnlckd(utxo.UnlockingScript) {
			fmt.Printf("ERROR {ChkTx}: Mismatch unlocking script\n")
			return false
		}
	}
	if t.Sz() > n.Conf.MxBlkSz {
		fmt.Printf("ERROR {ChkTx}: Transaction size exceeds maximum block size\n")
		return false
	}
	return true
}
