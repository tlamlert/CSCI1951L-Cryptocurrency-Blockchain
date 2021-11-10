const Pin = artifacts.require('dummy/Pin.sol');
const Zrx = artifacts.require('dummy/Zrx.sol');
const Exc = artifacts.require('Exc.sol');
const Fac = artifacts.require('Factory.sol')
const Pool = artifacts.require('Pool.sol')

const SIDE = {
    BUY: 0,
    SELL: 1
};

contract('Factory', (accounts) => {
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
        fac = await Fac.new()
    });

    it('should create the pair and make 5', async () => {
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
        const pool = await Pool.at(poolAd);
        const checkMe = await pool.testing(1);
        assert(checkMe, 5);
    });
});
