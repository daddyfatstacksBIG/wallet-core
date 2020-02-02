
// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Data.h"
#include "Ethereum/Transaction.h"
#include "HexCoding.h"
#include "TWTestUtilities.h"
#include "proto/Ethereum.pb.h"
#include "uint256.h"
#include <TrustWalletCore/TWEthereumSigner.h>

#include <gtest/gtest.h>

using namespace TW;
using namespace Ethereum;

TEST(TWEthereumSigner, EmptyValue) {
    auto str = std::string("");
    uint256_t zero = load(str);

    ASSERT_EQ(zero, uint256_t(0));
}

TEST(TWEthereumSigner, BigInt) {
    // Check uint256_t loading
    Data expectedData = {0x52, 0x08};
    auto value = uint256_t(21000);
    auto loaded = load(expectedData);
    ASSERT_EQ(loaded, value);

    // Check proto storing
    Proto::SigningInput input;
    auto storedData = store(value);
    input.set_gas_limit(storedData.data(), storedData.size());
    ASSERT_EQ(hex(input.gas_limit()), hex(expectedData));

    // Check proto loading
    auto protoLoaded = load(input.gas_limit());
    ASSERT_EQ(protoLoaded, value);
}

TEST(TWEthereumSigner, Sign) {
    // from http://thetokenfactory.com/#/factory
    // https://etherscan.io/tx/0x63879f20909a315bcffe629bc03b20e5bc65ba2a377bd7152e3b69c4bd4cd6cc
    Proto::SigningInput input;
    auto chainId = store(uint256_t(1));
    auto nonce = store(uint256_t(11));
    auto gasPrice = store(uint256_t(20000000000));
    auto gasLimit = store(uint256_t(1000000));
    auto data = parse_hex(
        "0x60a060405260046060527f48302e310000000000000000000000000000000000000000000000000000000060"
        "80526006805460008290527f48302e310000000000000000000000000000000000000000000000000000000882"
        "556100b5907ff652222313e28459528d920b65115c16c04f3efc82aaedc97be59f3f377c0d3f60206002600184"
        "1615610100026000190190931692909204601f01919091048101905b8082111561017957600081556001016100"
        "a1565b505060405161094b38038061094b83398101604052808051906020019091908051820191906020018051"
        "906020019091908051820191906020015050836000600050600033600160a060020a0316815260200190815260"
        "200160002060005081905550836002600050819055508260036000509080519060200190828054600181600116"
        "156101000203166002900490600052602060002090601f016020900481019282601f1061017d57805160ff1916"
        "8380011785555b506101ad9291506100a1565b5090565b8280016001018555821561016d579182015b82811115"
        "61016d57825182600050559160200191906001019061018f565b50506004805460ff1916831790556005805482"
        "5160008390527f036b6384b5eca791c62761152d0c79bb0604c104a5fb6f4eb0703f3154bb3db0602060026001"
        "851615610100026000190190941693909304601f90810184900482019386019083901061022d57805160ff1916"
        "8380011785555b5061025d9291506100a1565b82800160010185558215610221579182015b8281111561022157"
        "825182600050559160200191906001019061023f565b5050505050506106da806102716000396000f360606040"
        "52361561008d5760e060020a600035046306fdde038114610095578063095ea7b3146100f357806318160ddd14"
        "61016857806323b872dd14610171578063313ce5671461025c57806354fd4d501461026857806370a082311461"
        "02c657806395d89b41146102f4578063a9059cbb14610352578063cae9ca51146103f7578063dd62ed3e146105"
        "be575b6105f2610002565b6040805160038054602060026001831615610100026000190190921691909104601f"
        "81018290048202840182019094528383526105f493908301828280156106b75780601f1061068c576101008083"
        "540402835291602001916106b7565b61066260043560243533600160a060020a03908116600081815260016020"
        "908152604080832094871680845294825280832086905580518681529051929493927f8c5be1e5ebec7d5bd14f"
        "71427d1e84f3dd0314c0f7b2291e5b200ac8c7c3b925929181900390910190a35060015b92915050565b6102e2"
        "60025481565b610662600435602435604435600160a060020a0383166000908152602081905260408120548290"
        "108015906101c4575060016020908152604080832033600160a060020a03168452909152812054829010155b80"
        "156101d05750600082115b156106bf57600160a060020a03838116600081815260208181526040808320805488"
        "019055888516808452818420805489900390556001835281842033909616845294825291829020805487900390"
        "55815186815291519293927fddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef92"
        "81900390910190a35060016106c3565b61067660045460ff1681565b6040805160068054602060026001831615"
        "610100026000190190921691909104601f81018290048202840182019094528383526105f49390830182828015"
        "6106b75780601f1061068c576101008083540402835291602001916106b7565b600160a060020a036004351660"
        "00908152602081905260409020545b60408051918252519081900360200190f35b6105f4600580546040805160"
        "2060026001851615610100026000190190941693909304601f8101849004840282018401909252818152929183"
        "01828280156106b75780601f1061068c576101008083540402835291602001916106b7565b6106626004356024"
        "3533600160a060020a03166000908152602081905260408120548290108015906103845750600082115b156106"
        "ca5733600160a060020a0390811660008181526020818152604080832080548890039055938716808352918490"
        "208054870190558351868152935191937fddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4d"
        "f523b3ef929081900390910190a3506001610162565b604080516020604435600481810135601f810184900484"
        "028501840190955284845261066294813594602480359593946064949293910191819084018382808284375094"
        "965050505050505033600160a060020a0390811660008181526001602090815260408083209488168084529482"
        "5280832087905580518781529051929493927f8c5be1e5ebec7d5bd14f71427d1e84f3dd0314c0f7b2291e5b20"
        "0ac8c7c3b925929181900390910190a383600160a060020a031660405180807f72656365697665417070726f76"
        "616c28616464726573732c75696e743235362c81526020017f616464726573732c627974657329000000000000"
        "000000000000000000000000815260200150602e019050604051809103902060e060020a900433853086604051"
        "8560e060020a0281526004018085600160a060020a0316815260200184815260200183600160a060020a031681"
        "526020018280519060200190808383829060006004602084601f0104600f02600301f150905090810190601f16"
        "80156105965780820380516001836020036101000a031916815260200191505b50945050505050600060405180"
        "8303816000876161da5a03f19250505015156106d257610002565b6102e2600435602435600160a060020a0382"
        "8116600090815260016020908152604080832093851683529290522054610162565b005b604051808060200182"
        "81038252838181518152602001915080519060200190808383829060006004602084601f0104600f02600301f1"
        "50905090810190601f1680156106545780820380516001836020036101000a031916815260200191505b509250"
        "505060405180910390f35b604080519115158252519081900360200190f35b6040805160ff9092168252519081"
        "900360200190f35b820191906000526020600020905b81548152906001019060200180831161069a5782900360"
        "1f168201915b505050505081565b5060005b9392505050565b506000610162565b5060016106c3560000000000"
        "0000000000000000000000000000000000000000000000000003e8000000000000000000000000000000000000"
        "000000000000000000000000008000000000000000000000000000000000000000000000000000000000000000"
        "0400000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000"
        "000000000000000000000000000000000000000754204275636b73000000000000000000000000000000000000"
        "000000000000000000000000000000000000000000000000000000000000000000000000000003544f54000000"
        "0000000000000000000000000000000000000000000000000000");
    auto key = parse_hex("0x4646464646464646464646464646464646464646464646464646464646464646");

    input.set_chain_id(chainId.data(), chainId.size());
    input.set_nonce(nonce.data(), nonce.size());
    input.set_gas_price(gasPrice.data(), gasPrice.size());
    input.set_gas_limit(gasLimit.data(), gasLimit.size());
    input.set_payload(data.data(), data.size());
    input.set_private_key(key.data(), key.size());

    auto inputString = input.SerializeAsString();
    auto inputData = TWDataCreateWithBytes((const byte*)inputString.data(), inputString.size());

    auto outputData = TWEthereumSignerSign(inputData);

    auto output = Proto::SigningOutput();
    output.ParseFromArray(TWDataBytes(outputData), TWDataSize(outputData));

    ASSERT_EQ(
        hex(output.encoded()),
        "f90a9e0b8504a817c800830f42408080b90a4b60a060405260046060527f48302e310000000000000000000000"
        "00000000000000000000000000000000006080526006805460008290527f48302e310000000000000000000000"
        "000000000000000000000000000000000882556100b5907ff652222313e28459528d920b65115c16c04f3efc82"
        "aaedc97be59f3f377c0d3f602060026001841615610100026000190190931692909204601f0191909104810190"
        "5b8082111561017957600081556001016100a1565b505060405161094b38038061094b83398101604052808051"
        "906020019091908051820191906020018051906020019091908051820191906020015050836000600050600033"
        "600160a060020a0316815260200190815260200160002060005081905550836002600050819055508260036000"
        "509080519060200190828054600181600116156101000203166002900490600052602060002090601f01602090"
        "0481019282601f1061017d57805160ff19168380011785555b506101ad9291506100a1565b5090565b82800160"
        "01018555821561016d579182015b8281111561016d57825182600050559160200191906001019061018f565b50"
        "506004805460ff19168317905560058054825160008390527f036b6384b5eca791c62761152d0c79bb0604c104"
        "a5fb6f4eb0703f3154bb3db0602060026001851615610100026000190190941693909304601f90810184900482"
        "019386019083901061022d57805160ff19168380011785555b5061025d9291506100a1565b8280016001018555"
        "8215610221579182015b8281111561022157825182600050559160200191906001019061023f565b5050505050"
        "506106da806102716000396000f36060604052361561008d5760e060020a600035046306fdde03811461009557"
        "8063095ea7b3146100f357806318160ddd1461016857806323b872dd14610171578063313ce5671461025c5780"
        "6354fd4d501461026857806370a08231146102c657806395d89b41146102f4578063a9059cbb14610352578063"
        "cae9ca51146103f7578063dd62ed3e146105be575b6105f2610002565b60408051600380546020600260018316"
        "15610100026000190190921691909104601f81018290048202840182019094528383526105f493908301828280"
        "156106b75780601f1061068c576101008083540402835291602001916106b7565b610662600435602435336001"
        "60a060020a03908116600081815260016020908152604080832094871680845294825280832086905580518681"
        "529051929493927f8c5be1e5ebec7d5bd14f71427d1e84f3dd0314c0f7b2291e5b200ac8c7c3b9259291819003"
        "90910190a35060015b92915050565b6102e260025481565b610662600435602435604435600160a060020a0383"
        "166000908152602081905260408120548290108015906101c4575060016020908152604080832033600160a060"
        "020a03168452909152812054829010155b80156101d05750600082115b156106bf57600160a060020a03838116"
        "600081815260208181526040808320805488019055888516808452818420805489900390556001835281842033"
        "90961684529482529182902080548790039055815186815291519293927fddf252ad1be2c89b69c2b068fc378d"
        "aa952ba7f163c4a11628f55a4df523b3ef9281900390910190a35060016106c3565b61067660045460ff168156"
        "5b6040805160068054602060026001831615610100026000190190921691909104601f81018290048202840182"
        "019094528383526105f493908301828280156106b75780601f1061068c57610100808354040283529160200191"
        "6106b7565b600160a060020a03600435166000908152602081905260409020545b604080519182525190819003"
        "60200190f35b6105f46005805460408051602060026001851615610100026000190190941693909304601f8101"
        "84900484028201840190925281815292918301828280156106b75780601f1061068c5761010080835404028352"
        "91602001916106b7565b61066260043560243533600160a060020a031660009081526020819052604081205482"
        "90108015906103845750600082115b156106ca5733600160a060020a0390811660008181526020818152604080"
        "832080548890039055938716808352918490208054870190558351868152935191937fddf252ad1be2c89b69c2"
        "b068fc378daa952ba7f163c4a11628f55a4df523b3ef929081900390910190a3506001610162565b6040805160"
        "20604435600481810135601f810184900484028501840190955284845261066294813594602480359593946064"
        "949293910191819084018382808284375094965050505050505033600160a060020a0390811660008181526001"
        "6020908152604080832094881680845294825280832087905580518781529051929493927f8c5be1e5ebec7d5b"
        "d14f71427d1e84f3dd0314c0f7b2291e5b200ac8c7c3b925929181900390910190a383600160a060020a031660"
        "405180807f72656365697665417070726f76616c28616464726573732c75696e743235362c81526020017f6164"
        "64726573732c627974657329000000000000000000000000000000000000815260200150602e01905060405180"
        "9103902060e060020a9004338530866040518560e060020a0281526004018085600160a060020a031681526020"
        "0184815260200183600160a060020a031681526020018280519060200190808383829060006004602084601f01"
        "04600f02600301f150905090810190601f1680156105965780820380516001836020036101000a031916815260"
        "200191505b509450505050506000604051808303816000876161da5a03f19250505015156106d257610002565b"
        "6102e2600435602435600160a060020a0382811660009081526001602090815260408083209385168352929052"
        "2054610162565b005b604051808060200182810382528381815181526020019150805190602001908083838290"
        "60006004602084601f0104600f02600301f150905090810190601f168015610654578082038051600183602003"
        "6101000a031916815260200191505b509250505060405180910390f35b60408051911515825251908190036020"
        "0190f35b6040805160ff9092168252519081900360200190f35b820191906000526020600020905b8154815290"
        "6001019060200180831161069a57829003601f168201915b505050505081565b5060005b9392505050565b5060"
        "00610162565b5060016106c35600000000000000000000000000000000000000000000000000000000000003e8"
        "000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000"
        "000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000"
        "0000000000c0000000000000000000000000000000000000000000000000000000000000000754204275636b73"
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000000003544f54000000000000000000000000000000000000000000000000000000000026"
        "a042556c4f2a3f4e4e639cca524d1da70e60881417d4643e5382ed110a52719eafa0172f591a2a763d0bd6b13d"
        "042d8c5eb66e87f129c9dc77ada66b6041012db2b3");

    TWDataDelete(inputData);
}
