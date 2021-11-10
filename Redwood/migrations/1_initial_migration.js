const Migrations = artifacts.require("Migrations");
const Exchange = artifacts.require("Exc");
const Factory = artifacts.require("Factory");

module.exports = function (deployer) {
  deployer.deploy(Migrations);
  deployer.deploy(Exchange);
  deployer.deploy(Factory);
};
