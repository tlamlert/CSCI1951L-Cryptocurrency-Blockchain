pragma solidity 0.5.3;
pragma experimental ABIEncoderV2;

interface IExc {

       
    enum Side {
        BUY,
        SELL
    }
    
    struct Token {
        bytes32 ticker;
        address tokenAddress;
    }
    
    struct Order {
        uint id;
        address trader;
        Side side;
        bytes32 ticker;
        uint amount;
        uint filled;
        uint price;
        uint date;
    }
    
    
    

    function getOrders(
      bytes32 ticker, 
      Side side) 
      external 
      view
      returns(Order[] memory);

    function getTokens() 
      external 
      view 
      returns(Token[] memory);
    
    function addToken(
        bytes32 ticker,
        address tokenAddress)
        external;
    
    function deposit(
        uint amount,
        bytes32 ticker)
        external;
    
    function withdraw(
        uint amount,
        bytes32 ticker)
        external;
    
    function makeLimitOrder(
        bytes32 ticker,
        uint amount,
        uint price,
        Side side)
        external;
    
        function deleteLimitOrder(
        uint id,
        bytes32 ticker,
        Side side)
    external returns (bool);
    
    function makeMarketOrder(
        bytes32 ticker,
        uint amount,
        Side side)
        external;
}