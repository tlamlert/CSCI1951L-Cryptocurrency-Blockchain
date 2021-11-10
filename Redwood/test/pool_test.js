const Pin = artifacts.require('dummy/Pin.sol');
const Zrx = artifacts.require('dummy/Zrx.sol');
const Exc = artifacts.require('Exc.sol');
const Fac = artifacts.require('Factory.sol')
const Pool = artifacts.require('Pool.sol')

const SIDE = {
    BUY: 0,
    SELL: 1
};

contract('Pool', (accounts) => {
    let pin, zrx, exc, pool;
    const [trader1, trader2] = [accounts[1], accounts[2]];
    const [PIN, ZRX] = ['PIN', 'ZRX']
        .map(ticker => web3.utils.fromAscii(ticker));

    beforeEach(async() => {
        ([pin, zrx] = await Promise.all([
            Pin.new(),
            Zrx.new()
        ]));
        exc = await Exc.new();
        fac = await Fac.new();
        let event = await fac.createPair(
            pin.address,
            zrx.address,
            pin.address,
            exc.address,
            PIN,
            ZRX
        );
        let log = event.logs[0];
        let poolAd = log.args.pair;
        pool = await Pool.at(poolAd);
        // mint and give approval
        await pin.mint(trader1, web3.utils.toWei('200'));
        await zrx.mint(trader1, web3.utils.toWei('200'));
        await exc.addToken(PIN, pin.address);
        await exc.addToken(ZRX, zrx.address);
    });

    it('deposit 2 pin and 2 zrx', async () => {
        await pin.approve(pool.address, web3.utils.toWei('2'), {from: trader1});
        await zrx.approve(pool.address, web3.utils.toWei('2'), {from: trader1});
        await pool.deposit(web3.utils.toWei('2'), web3.utils.toWei('2'), {from: trader1});
        let pinBalance = await pool.get_traderBalances(trader1, PIN);
        let zrxBalance = await pool.get_traderBalances(trader1, ZRX);
        assert(pinBalance, web3.utils.toWei('2'));
        assert(zrxBalance, web3.utils.toWei('2'));
    });

    it('deposit 10 pin and 10 zrx, withdraw 4 pin and 4 zrx, final balance nonzero', async () => {
        await pin.approve(pool.address, web3.utils.toWei('10'), {from: trader1});
        await zrx.approve(pool.address, web3.utils.toWei('10'), {from: trader1});
        await pool.deposit(web3.utils.toWei('10'), web3.utils.toWei('10'), {from: trader1});
        await pool.withdraw(web3.utils.toWei('4'), web3.utils.toWei('4'), {from: trader1});
        let pinBalance = await pool.get_traderBalances(trader1, PIN);
        let zrxBalance = await pool.get_traderBalances(trader1, ZRX);
        assert(pinBalance, web3.utils.toWei('6'));
        assert(zrxBalance, web3.utils.toWei('6'));
    });

    it('deposit 10 pin and 10 zrx, withdraw 2 pin and 6 zrx, final balance nonzero', async () => {
        await pin.approve(pool.address, web3.utils.toWei('10'), {from: trader1});
        await zrx.approve(pool.address, web3.utils.toWei('10'), {from: trader1});
        await pool.deposit(web3.utils.toWei('10'), web3.utils.toWei('10'), {from: trader1});
        await pool.withdraw(web3.utils.toWei('2'), web3.utils.toWei('6'), {from: trader1});
        let pinBalance = await pool.get_traderBalances(trader1, PIN);
        let zrxBalance = await pool.get_traderBalances(trader1, ZRX);
        assert(pinBalance, web3.utils.toWei('8'));
        assert(zrxBalance, web3.utils.toWei('4'));
    });

    it('deposit 200 pin and 100 zrx, withdraw 200 pin and 100 zrx, final balance 0', async () => {
        await pin.approve(pool.address, web3.utils.toWei('100'), {from: trader1});
        await zrx.approve(pool.address, web3.utils.toWei('200'), {from: trader1});
        await pool.deposit(web3.utils.toWei('200'), web3.utils.toWei('100'), {from: trader1});
        await pool.withdraw(web3.utils.toWei('200'), web3.utils.toWei('100'), {from: trader1});
        let pinBalance = await pool.get_traderBalances(trader1, PIN);
        let zrxBalance = await pool.get_traderBalances(trader1, ZRX);
        assert(pinBalance, web3.utils.toWei('0'));
        assert(zrxBalance, web3.utils.toWei('0'));
    });

    it('deposit 200 zrx, withdraw 200 zrx, final balance 0', async () => {
        await pin.approve(pool.address, web3.utils.toWei('0'), {from: trader1});
        await zrx.approve(pool.address, web3.utils.toWei('200'), {from: trader1});
        await pool.deposit(web3.utils.toWei('200'), web3.utils.toWei('0'), {from: trader1});
        await pool.withdraw(web3.utils.toWei('200'), web3.utils.toWei('0'), {from: trader1});
        let pinBalance = await pool.get_traderBalances(trader1, PIN);
        let zrxBalance = await pool.get_traderBalances(trader1, ZRX);
        assert(pinBalance, web3.utils.toWei('0'));
        assert(zrxBalance, web3.utils.toWei('0'));
    });

    it('deposit 200 pin, withdraw 200 pin, final balance 0', async () => {
        await pin.approve(pool.address, web3.utils.toWei('200'), {from: trader1});
        await zrx.approve(pool.address, web3.utils.toWei('0'), {from: trader1});
        await pool.deposit(web3.utils.toWei('0'), web3.utils.toWei('200'), {from: trader1});
        await pool.withdraw(web3.utils.toWei('0'), web3.utils.toWei('200'), {from: trader1});
        let pinBalance = await pool.get_traderBalances(trader1, PIN);
        let zrxBalance = await pool.get_traderBalances(trader1, ZRX);
        assert(pinBalance, web3.utils.toWei('0'));
        assert(zrxBalance, web3.utils.toWei('0'));
    });

    it ('withdraws without depositing, final balance 0', async () => {
        await pin.approve(pool.address, web3.utils.toWei('200'), {from: trader1});
        await zrx.approve(pool.address, web3.utils.toWei('100'), {from: trader1});
        try {
            await pool.withdraw(web3.utils.toWei('200'), web3.utils.toWei('100'), {from: trader1});
        } catch (error) {
            let pinBalance = await pool.get_traderBalances(trader1, PIN);
            assert(pinBalance, web3.utils.toWei('0'));
        }
    });
});
