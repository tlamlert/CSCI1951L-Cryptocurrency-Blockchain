const Pin = artifacts.require('dummy/Pin.sol');
const Zrx = artifacts.require('dummy/Zrx.sol');
const Exc = artifacts.require('Exc.sol');

const SIDE = {
    BUY: 0,
    SELL: 1
};

contract('Exc', (accounts) => {
    let pin, zrx, exc;
    const [trader1, trader2] = [accounts[1], accounts[2]];
    const [PIN, ZRX] = ['PIN', 'ZRX']
        .map(ticker => web3.utils.fromAscii(ticker));

    beforeEach(async() => {
        ([pin, zrx] = await Promise.all([
            Pin.new(),
            Zrx.new()
        ]));
        exc = await Exc.new();
        // add PIN and ZRX to exc
        await exc.addToken(PIN, pin.address);
        await exc.addToken(ZRX, zrx.address);
        // mint tokens (in each token contract to each trader)
        await pin.mint(trader1, web3.utils.toWei('100'));
        await zrx.mint(trader1, web3.utils.toWei('100'));
        await pin.mint(trader2, web3.utils.toWei('200'));
        await zrx.mint(trader2, web3.utils.toWei('100'));
    });

    it('should deposit 100 wei to dex', async () => {
        await pin.approve(exc.address, web3.utils.toWei('100'), {from: trader1});
        await exc.deposit(web3.utils.toWei('100'), PIN, {from: trader1});
        assert(exc.get_traderBalances(trader1, PIN), web3.utils.toWei('100'));
    });

    it('should make 2 limit orders and delete the first one', async () => {
        await zrx.approve(exc.address, web3.utils.toWei('100'), {from: trader1});
        await exc.deposit(web3.utils.toWei('100'), ZRX, {from: trader1});
        await exc.makeLimitOrder(
            ZRX,
            web3.utils.toWei('70'),
            100,
            SIDE.SELL,
            {from: trader1});
        await exc.makeLimitOrder(
            ZRX,
            web3.utils.toWei('30'),
            100,
            SIDE.SELL,
            {from: trader1});
        assert(exc.get_traderBalances(trader1, ZRX), web3.utils.toWei('0'));
        await exc.deleteLimitOrder(
            0,
            ZRX,
            SIDE.SELL,
            {from: trader1});
        assert(exc.get_traderBalances(trader1, ZRX), web3.utils.toWei('70'));
        let sellOrders = await exc.getOrders(ZRX, SIDE.SELL);
        assert(sellOrders.length === 1);
    });

    it('should not deposit if token does not exist', async () => {
        try {
            await exc.deposit(web3.utils.toWei('100'), PIN, {from: trader1})
            let pinBalance = await exc.get_traderBalances(trader1, PIN);
        } catch (error) {
            assert(error, 'Token does not exist');
        }
    });

    it('should return limit order - SELL', async () => {
        await exc.addToken(ZRX, zrx.address);
        await zrx.approve(exc.address, web3.utils.toWei('100'), {from: trader1});
        await exc.deposit(web3.utils.toWei('100'), ZRX, {from: trader1});
        await exc.makeLimitOrder(ZRX, web3.utils.toWei('100'), 100, SIDE.SELL, {from: trader1});
        let orders = await exc.getOrders(ZRX, SIDE.SELL, {from: trader1});
        assert(orders[0].id, 1);
        assert(orders[0].trader, trader1);
        assert(orders[0].ticker, ZRX);
        assert(orders[0].amount, web3.utils.toWei('100'));
        assert(orders[0].price, 100);
        assert(orders[0].filled, 0);
    });

    it('should return limit order - BUY', async () => {
        await exc.addToken(PIN, pin.address);
        await exc.addToken(ZRX, zrx.address);
        await pin.approve(exc.address, web3.utils.toWei('100'), {from: trader1});
        await exc.deposit(web3.utils.toWei('100'), PIN, {from: trader1});
        await exc.makeLimitOrder(ZRX, web3.utils.toWei('10'), 10, SIDE.BUY, {from: trader1});
        let orders = await exc.getOrders(ZRX, SIDE.BUY, {from: trader1});
        assert(orders[0].id, 1);
        assert(orders[0].trader, trader1);
        assert(orders[0].ticker, ZRX);
        assert(orders[0].amount, web3.utils.toWei('10'));
        assert(orders[0].price, 10);
        assert(orders[0].filled, 0);
        });

    it('should withdraw', async () => {
        await pin.approve(exc.address, web3.utils.toWei('100'), {from: trader1});
        await exc.deposit(web3.utils.toWei('100'), PIN, {from: trader1});
        assert(exc.get_traderBalances(trader1, PIN), web3.utils.toWei('100'));
        await exc.withdraw(web3.utils.toWei('100'), PIN, {from: trader1});
        assert(exc.get_traderBalances(trader1, PIN), web3.utils.toWei('0'));

        await pin.approve(exc.address, web3.utils.toWei('100'), {from: trader1});
        await exc.deposit(web3.utils.toWei('100'), PIN, {from: trader1});
        assert(exc.get_traderBalances(trader1, PIN), web3.utils.toWei('100'));
        await exc.withdraw(web3.utils.toWei('100'), PIN, {from: trader1});
        assert(exc.get_traderBalances(trader1, PIN), web3.utils.toWei('0'));
    });

    it('deletes limit order', async () => {
        await zrx.approve(exc.address, web3.utils.toWei('100'), {from: trader1});
        await exc.deposit(web3.utils.toWei('100'), ZRX, {from: trader1});
        //assert(exc.get_traderBalances(trader1, PIN), web3.utils.toWei('100'));
        await exc.makeLimitOrder(ZRX, web3.utils.toWei('100'), 10, SIDE.SELL, {from: trader1});
        assert(exc.get_traderBalances(trader1, ZRX), web3.utils.toWei('0'));
        await exc.deleteLimitOrder(1, ZRX, SIDE.SELL, {from: trader1});
        //assert(exc.get_traderBalances(trader1, PIN), web3.utils.toWei('0'));
        assert(exc.get_traderBalances(trader1, ZRX), web3.utils.toWei('100'));
    });

    //Colby Anderson to Everyone (11:51 PM)
    it('should make and then delete limit order', async () => {
        await pin.approve(exc.address, web3.utils.toWei('100'), {from: trader1});
            await exc.deposit(
                web3.utils.toWei('100'),
                PIN,
                {from: trader1}
            );

            await exc.makeLimitOrder(
                ZRX,
                web3.utils.toWei('10'),
                10,
                SIDE.BUY,
                {from: trader1}
            );

            let buyOrders = await exc.getOrders(ZRX, SIDE.BUY);
            let sellOrders = await exc.getOrders(ZRX, SIDE.SELL);
            assert(buyOrders.length === 1);
            assert(buyOrders[0].trader === trader1);
            assert(sellOrders.length === 0);

            let shouldTrue = await exc.deleteLimitOrder(
                0,
                ZRX,
                SIDE.BUY,
                {from: trader1}
            );

            let buyOrdersAD = await exc.getOrders(ZRX, SIDE.BUY);
            assert(buyOrdersAD.length === 0);
            assert(shouldTrue, "delete worked!");
        });

    it('make market order, buy', async () => {
        await pin.approve(exc.address, web3.utils.toWei('100'), {from: trader1});
        await exc.deposit(
            web3.utils.toWei('100'),
            PIN,
            {from: trader1}
        );

        await zrx.approve(exc.address, web3.utils.toWei('100'), {from: trader2});
        await exc.deposit(
            web3.utils.toWei('100'),
            ZRX,
            {from: trader2}
        )

        assert(exc.get_traderBalances(trader1, PIN), web3.utils.toWei('100'));

        await exc.makeLimitOrder(
            ZRX,
            web3.utils.toWei('10'),
            10,
            SIDE.SELL,
            {from: trader2}
        );

        await exc.makeMarketOrder(
            ZRX,
            web3.utils.toWei('10'),
            SIDE.BUY,
            {from: trader1}
        );

        assert(exc.get_traderBalances(trader1, PIN), web3.utils.toWei('0'));
        assert(exc.get_traderBalances(trader1, ZRX), web3.utils.toWei('10'));
        assert(exc.get_traderBalances(trader2, PIN), web3.utils.toWei('100'));
        assert(exc.get_traderBalances(trader2, ZRX), web3.utils.toWei('90'));
    });

    it('should make 2 limit orders and then delete the first limit order', async () => {
        await pin.approve(exc.address, web3.utils.toWei('100'), {from: trader1});
        await exc.deposit(
            web3.utils.toWei('100'),
            PIN,
            {from: trader1}
        );

        await exc.makeLimitOrder(
            ZRX,
            web3.utils.toWei('10'),
            10,
            SIDE.BUY,
            {from: trader1}
        );

        await pin.approve(exc.address, web3.utils.toWei('200'), {from: trader2});
        await exc.deposit(
            web3.utils.toWei('200'),
            PIN,
            {from: trader2}
        );

        await exc.makeLimitOrder(
            ZRX,
            web3.utils.toWei('10'),
            9,
            SIDE.BUY,
            {from: trader2}
        );

        let buyOrders = await exc.getOrders(ZRX, SIDE.BUY);
        console.log(buyOrders[0]);
        console.log(buyOrders[1]);
        assert.lengthOf(buyOrders, 2, "starting length");
        assert.equal(buyOrders[0].id, 0);
        assert.equal(buyOrders[1].id, 1);

        let shouldTrue = await exc.deleteLimitOrder(
            0,
            ZRX,
            SIDE.BUY,
            {from: trader1}
        );

        let buyOrdersAD = await exc.getOrders(ZRX, SIDE.BUY);
        assert.lengthOf(buyOrdersAD, 1, "final length");
        assert(shouldTrue, "delete worked!");
        assert.equal(buyOrdersAD[0].id, 1);
    });

});

// truffle test ./test/dex_test.js
