pragma solidity 0.5.3;
pragma experimental ABIEncoderV2;

/// @notice these commented segments will differ based on where you're deploying these contracts. If you're deploying
/// on remix, feel free to uncomment the github imports, otherwise, use the uncommented imports

// import "https://github.com/OpenZeppelin/openzeppelin-contracts/blob/solc-0.6/contracts/token/ERC20/IERC20.sol";
// import "https://github.com/OpenZeppelin/openzeppelin-contracts/blob/solc-0.6/contracts/math/SafeMath.sol";
import '../contracts/libraries/token/ERC20/ERC20.sol';
import '../contracts/libraries/math/SafeMath.sol';
import "./IExc.sol";

contract Exc is IExc{
    /// @notice simply notes that we are using SafeMath for uint, since Solidity's math is unsafe. For all the math
    /// you do, you must use the methods specified in SafeMath (found at the github link above), instead of Solidity's
    /// built-in operators.
    using SafeMath for uint;

    /// @notice these declarations are incomplete. You will still need a way to store the orderbook, the balances
    /// of the traders, and the IDs of the next trades and orders. Reference the NewTrade event and the IExc
    /// interface for more details about orders and sides.
    mapping(bytes32 => Token) public tokens;
    bytes32[] public tokenList;
    bytes32 constant PIN = bytes32('PIN');

    /// @notice, this is the more standardized form of the main wallet data structure, if you're using something a bit
    /// different, implementing a function that just takes in the address of the trader and then the ticker of a
    /// token instead would suffice
    mapping(address => mapping(bytes32 => uint)) public traderBalances;
    mapping(bytes32 => mapping(uint => Order[])) orderBook;
    uint nextOrderID = 0;
    uint nextTradeID = 0;

    /// @notice an event representing all the needed info regarding a new trade on the exchange
    event NewTrade(
        uint tradeId,
        uint orderId,
        bytes32 indexed ticker,
        address indexed trader1,
        address indexed trader2,
        uint amount,
        uint price,
        uint date
    );

    // getOrders, which simply returns the orders for a specific token on a specific side
    function getOrders(
        bytes32 ticker,
        Side side)
    external
    view
    exist(ticker)
    returns(Order[] memory) {
        uint side_uint = uint(side);
        return orderBook[ticker][side_uint];
    }

    // getTokens, which simply returns an array of the tokens currently traded on in the exchange
    function getTokens()
    external
    view
    returns(Token[] memory) {
        Token[] memory output = new Token[](tokenList.length);
        for (uint i = 0; i < tokenList.length; i++) {
            output[i] = tokens[tokenList[i]];
        }
        return output;
    }

    // addToken, which should add the token desired to the exchange by interacting with tokenList and tokens
    function addToken(
        bytes32 ticker,
        address tokenAddress)
    external {
        tokenList.push(ticker);
        tokens[ticker] = Token(ticker, tokenAddress);
    }

    // deposit, which should deposit a certain amount of tokens from a trader to their on-exchange wallet,
    // based on the wallet data structure you create and the IERC20 interface methods. Namely, you should transfer
    // tokens from the account of the trader on that token to this smart contract, and credit them appropriately
    function deposit(
        uint amount,
        bytes32 ticker)
    external
    exist(ticker) {
        if (IERC20(tokens[ticker].tokenAddress).transferFrom(msg.sender, address(this), amount)) {
            traderBalances[msg.sender][ticker] = SafeMath.add(traderBalances[msg.sender][ticker], amount);
        }
    }

    // withdraw, which should do the opposite of deposit. The trader should not be able to withdraw more than
    // they have in the exchange.
    function withdraw(
        uint amount,
        bytes32 ticker)
    external
    exist(ticker) {
        require(traderBalances[msg.sender][ticker] >= amount, "insufficient balance");
        if (IERC20(tokens[ticker].tokenAddress).transfer(msg.sender, amount)) {
            traderBalances[msg.sender][ticker] = SafeMath.sub(traderBalances[msg.sender][ticker], amount);
        }
    }

    // makeLimitOrder, which creates a limit order based on the parameters provided. This method should only be
    // used when the token desired exists and is not pine. This method should not execute if the trader's token or pine balances
    // are too low, depending on side. This order should be saved in the orderBook

    // a sorting algorithm for limit orders, based on best prices for market orders having the highest priority.
    // i.e., a limit buy order with a high price should have a higher priority in the orderbook.
    function makeLimitOrder(
        bytes32 ticker,
        uint amount,
        uint price,
        Side side)
    external
    exist(ticker)
    notPine(ticker) {
        uint side_uint = uint(side);
        if (side == Side.BUY) {
            require(traderBalances[msg.sender][PIN] >= SafeMath.mul(amount, price), 'insufficient pine balance');
        } else if (side == Side.SELL) {
            require(traderBalances[msg.sender][ticker] >= amount, 'insufficient token balance');
        }
        orderBook[ticker][side_uint].push(Order(nextOrderID, msg.sender, side, ticker, amount, 0, price, now));
        nextOrderID = SafeMath.add(nextOrderID, 1);

        // sort the array
        for (uint i = orderBook[ticker][side_uint].length - 1; i > 0; i--) {
            if ((side == Side.BUY && orderBook[ticker][side_uint][i-1].price > orderBook[ticker][side_uint][i].price) ||
                (side == Side.SELL && orderBook[ticker][side_uint][i-1].price < orderBook[ticker][side_uint][i].price)) {
                break;
            }
            Order memory a = orderBook[ticker][side_uint][i-1];
            orderBook[ticker][side_uint][i-1] = orderBook[ticker][side_uint][i];
            orderBook[ticker][side_uint][i] = a;
        }
    }

    // deleteLimitOrder, which will delete a limit order from the orderBook as
    // long as the same trader is deleting it.
    function deleteLimitOrder(
        uint id,
        bytes32 ticker,
        Side side)
    external
    exist(ticker)
    notPine(ticker)
    returns (bool) {
        uint side_uint = uint(side);
        for (uint i = 0; i < orderBook[ticker][side_uint].length; i++) {
            if (orderBook[ticker][side_uint][i].id == id) {
                require(msg.sender == orderBook[ticker][side_uint][i].trader, 'not the owner');
                for (uint j = i; j < orderBook[ticker][side_uint].length - 1; j++){
                    orderBook[ticker][side_uint][j] = orderBook[ticker][side_uint][j + 1];
                }
                orderBook[ticker][side_uint].pop();
                return true;
            }
        }
        return false;
    }

    // makeMarketOrder, which will execute a market order on the current orderbook. The market order need not be
    // added to the book explicitly, since it should execute against a limit order immediately. Make sure you are getting rid of
    // completely filled limit orders!
    function makeMarketOrder(
        bytes32 ticker,
        uint amount,
        Side side)
    public
    exist(ticker)
    notPine(ticker) {
        if (side == Side.BUY) {
            makeBUY(ticker, amount);
        } else if (side == Side.SELL) {
            makeSELL(ticker, amount);
        }
    }

    function get_cheapest_price(
        bytes32 ticker,
        uint amount)
    public
    view
    returns (uint) {
        uint side_uint = uint(Side.SELL);
        uint left = amount;
        uint price = 0;
        for (uint i = orderBook[ticker][side_uint].length - 1; i >= 0; i--) {
            if (left > 0) {
                uint available = SafeMath.sub(orderBook[ticker][side_uint][i].amount, orderBook[ticker][side_uint][i].filled);
                if (available > left) {
                    price = SafeMath.add(price, SafeMath.mul(left, orderBook[ticker][side_uint][i].price));
                    left = 0;
                } else {
                    price = SafeMath.add(price, SafeMath.mul(available, orderBook[ticker][side_uint][i].price));
                    left = SafeMath.sub(left, available);
                }
            } else {
                break;
            }
        }
        return price;
    }

    function makeBUY(
        bytes32 ticker,
        uint amount)
    private {
        uint side_uint = uint(Side.SELL);
        uint left = amount;

        for (uint i = orderBook[ticker][side_uint].length - 1; i >= 0; i--) {
            if (left > 0) {
                address curr_trader = orderBook[ticker][side_uint][i].trader;
                uint curr_price = orderBook[ticker][side_uint][i].price;
                uint available = SafeMath.sub(orderBook[ticker][side_uint][i].amount, orderBook[ticker][side_uint][i].filled);
                if (available > left) {
                    // adjust traders' balances
                    uint totalPrice = SafeMath.mul(left, curr_price);
                    traderBalances[msg.sender][ticker] = SafeMath.add(traderBalances[msg.sender][ticker], left);
                    traderBalances[msg.sender][PIN] = SafeMath.sub(traderBalances[msg.sender][PIN], totalPrice);
                    traderBalances[curr_trader][ticker] = SafeMath.sub(traderBalances[curr_trader][ticker], left);
                    traderBalances[curr_trader][PIN] = SafeMath.add(traderBalances[curr_trader][PIN], totalPrice);

                    // fill order and update left
                    orderBook[ticker][side_uint][i].filled = SafeMath.add(orderBook[ticker][side_uint][i].filled, left);
                    left = 0;

                    // emit trade
                    emit NewTrade(nextTradeID, nextOrderID, ticker, msg.sender, curr_trader, left, curr_price, now);
                    nextTradeID = SafeMath.add(nextTradeID, 1);
                } else {
                    // adjust traders' balances
                    uint totalPrice = SafeMath.mul(available, curr_price);
                    traderBalances[msg.sender][ticker] = SafeMath.add(traderBalances[msg.sender][ticker], available);
                    traderBalances[msg.sender][PIN] = SafeMath.sub(traderBalances[msg.sender][PIN], totalPrice);
                    traderBalances[curr_trader][ticker] = SafeMath.sub(traderBalances[curr_trader][ticker], available);
                    traderBalances[curr_trader][PIN] = SafeMath.add(traderBalances[curr_trader][PIN], totalPrice);

                    // fill order and update left
                    orderBook[ticker][side_uint][i].filled = SafeMath.add(orderBook[ticker][side_uint][i].filled, available);
                    left = SafeMath.sub(left, available);

                    // delete completely filled trade
                    orderBook[ticker][side_uint].pop();

                    // emit trade
                    emit NewTrade(nextTradeID, nextOrderID, ticker, msg.sender, curr_trader, available, curr_price, now);
                    nextTradeID = SafeMath.add(nextTradeID, 1);
                }
            } else {
                break;
            }
        }
    }

    function makeSELL(
        bytes32 ticker,
        uint amount)
    private {
        require(traderBalances[msg.sender][ticker] >= amount, 'insufficient tokens');
        uint side_uint = 0;
        uint left = amount;

        for (uint i = orderBook[ticker][0].length - 1; i >= 0; i--) {
            if (left > 0) {
                address curr_trader = orderBook[ticker][side_uint][i].trader;
                uint curr_price = orderBook[ticker][side_uint][i].price;
                uint available = SafeMath.sub(orderBook[ticker][side_uint][i].amount, orderBook[ticker][side_uint][i].filled);
                if (available > left) {
                    // adjust traders' balances
                    uint totalPrice = SafeMath.mul(left, curr_price);
                    traderBalances[msg.sender][ticker] = SafeMath.sub(traderBalances[msg.sender][ticker], left);
                    traderBalances[msg.sender][PIN] = SafeMath.add(traderBalances[msg.sender][PIN], totalPrice);
                    traderBalances[curr_trader][ticker] = SafeMath.add(traderBalances[curr_trader][ticker], left);
                    traderBalances[curr_trader][PIN] = SafeMath.sub(traderBalances[curr_trader][PIN], totalPrice);

                    // fill order and update left
                    orderBook[ticker][side_uint][i].filled = SafeMath.add(orderBook[ticker][side_uint][i].filled, left);
                    left = 0;

                    // emit trade
                    emit NewTrade(nextTradeID, nextOrderID, ticker, msg.sender, curr_trader, left, curr_price, now);
                    nextTradeID = SafeMath.add(nextTradeID, 1);
                } else {
                    // adjust traders' balances
                    uint totalPrice = SafeMath.mul(available, curr_price);
                    traderBalances[msg.sender][ticker] = SafeMath.sub(traderBalances[msg.sender][ticker], available);
                    traderBalances[msg.sender][PIN] = SafeMath.add(traderBalances[msg.sender][PIN], totalPrice);
                    traderBalances[curr_trader][ticker] = SafeMath.add(traderBalances[curr_trader][ticker], available);
                    traderBalances[curr_trader][PIN] = SafeMath.sub(traderBalances[curr_trader][PIN], totalPrice);

                    // fill order and update left
                    orderBook[ticker][side_uint][i].filled = SafeMath.add(orderBook[ticker][side_uint][i].filled, available);
                    left = SafeMath.sub(left, available);

                    // delete completely filled order
                    orderBook[ticker][side_uint].pop();

                    // emit trade
                    emit NewTrade(nextTradeID, nextOrderID, ticker, msg.sender, curr_trader, available, curr_price, now);
                    nextTradeID = SafeMath.add(nextTradeID, 1);
                }
            } else {
                break;
            }
        }
    }

    //todo: add modifiers for methods as detailed in handout
    modifier exist(bytes32 ticker) {
        require(tokens[ticker].tokenAddress != address(0), 'Token does not exist');
        _;
    }

    modifier notPine(bytes32 ticker) {
        require(ticker != PIN, 'Token is PINE');
        _;
    }

    function get_traderBalances(address trader, bytes32 ticker) external view returns(uint) {
        return traderBalances[trader][ticker];
    }

    function getNextOrderID() external view returns (uint) {
        return nextOrderID;
    }
}