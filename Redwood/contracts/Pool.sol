pragma solidity 0.5.3;

import './Exc.sol';
// import "https://github.com/OpenZeppelin/openzeppelin-contracts/blob/solc-0.6/contracts/math/SafeMath.sol";
import '../contracts/libraries/math/SafeMath.sol';
import "./IExc.sol";

contract Pool {
    
    /// @notice some parameters for the pool to function correctly, feel free to add more as needed
    address private tokenP;
    address private token1;
    address private dex;
    bytes32 private tokenPT;
    bytes32 private token1T;
    
    // wallet data structures
    mapping(address => mapping(bytes32 => uint)) traderTokens;
    mapping(bytes32 => uint) totalTokens; //total tokens in this contract of all traders
    uint lastBUYOrderId;
    uint lastSELLOrderId;
    bool hasMadeFirstOrder = false;

    function initialize(address _token0, address _token1, address _dex, uint whichP, bytes32 _tickerQ, bytes32 _tickerT)
    external {
        require(whichP == 1 || whichP == 2, 'invalid whichP');

        if (whichP == 1) {
            tokenP = _token0;
            token1 = _token1;
        } else if (whichP == 2) {
            tokenP = _token1;
            token1 = _token0;
        }
        dex = _dex;
        tokenPT = _tickerQ;
        token1T = _tickerT;
    }

    function deposit(uint tokenAmount, uint pineAmount) external {
        if (hasMadeFirstOrder) {
            Exc(dex).deleteLimitOrder(lastBUYOrderId, token1T, IExc.Side.BUY);
            Exc(dex).deleteLimitOrder(lastSELLOrderId, token1T, IExc.Side.SELL);
        }

        if (IERC20(token1).transferFrom(msg.sender, address(this), tokenAmount)) {
            traderTokens[msg.sender][token1T] = SafeMath.add(traderTokens[msg.sender][token1T], tokenAmount);
            totalTokens[token1T] = SafeMath.add(totalTokens[token1T], tokenAmount);
            IERC20(token1).approve(dex, tokenAmount);
            Exc(dex).deposit(tokenAmount, token1T);
        }

        if (IERC20(tokenP).transferFrom(msg.sender, address(this), pineAmount)) {
            traderTokens[msg.sender][tokenPT] = SafeMath.add(traderTokens[msg.sender][tokenPT], pineAmount);
            totalTokens[tokenPT] = SafeMath.add(totalTokens[tokenPT], pineAmount);
            IERC20(tokenP).approve(dex, pineAmount);
            Exc(dex).deposit(pineAmount, tokenPT);
        }

        if (totalTokens[token1T] != 0 && totalTokens[tokenPT] != 0) {
            uint marketPrice = priceCalculation();
            // make buy order
            Exc(dex).makeLimitOrder(token1T, totalTokens[token1T], marketPrice, IExc.Side.BUY);
            lastBUYOrderId = SafeMath.sub(Exc(dex).getNextOrderID(), 1);

            // make sell order
            Exc(dex).makeLimitOrder(token1T, totalTokens[token1T], marketPrice, IExc.Side.SELL);
            lastSELLOrderId = SafeMath.sub(Exc(dex).getNextOrderID(), 1);
            hasMadeFirstOrder = true;
        }
    }

    function withdraw(uint tokenAmount, uint pineAmount) external {
        require(tokenAmount <= traderTokens[msg.sender][token1T]);
        require(pineAmount <= traderTokens[msg.sender][tokenPT]);

        Exc(dex).withdraw(tokenAmount, token1T);
        if (IERC20(token1).transfer(msg.sender, tokenAmount)) {
            traderTokens[msg.sender][token1T] = SafeMath.sub(traderTokens[msg.sender][token1T], tokenAmount);
            totalTokens[token1T] = SafeMath.sub(totalTokens[token1T], tokenAmount);
        }

        Exc(dex).withdraw(pineAmount, tokenPT);
        if (IERC20(tokenP).transfer(msg.sender, pineAmount)) {
            traderTokens[msg.sender][tokenPT] = SafeMath.sub(traderTokens[msg.sender][tokenPT], pineAmount);
            totalTokens[tokenPT] = SafeMath.sub(totalTokens[tokenPT], pineAmount);
        }
    }

    function priceCalculation() internal view returns (uint) {
        if (totalTokens[token1T] == 0) {
            return 0;
        } else {
            return SafeMath.div(totalTokens[tokenPT], totalTokens[token1T]);
        }
    }

    function testing(uint testMe) public view returns (uint) {
        if (testMe == 1) {
            return 5;
        } else {
            return 3;
        }
    }

    function get_traderBalances(address trader, bytes32 ticker) external view returns(uint) {
        require(ticker == tokenPT || ticker == token1T);
        return traderTokens[trader][ticker];
    }
}